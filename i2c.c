#include <avr/io.h>
#include <avr/interrupt.h>
#include "i2c.h"
#include <stddef.h>

static volatile i2c_txn_t txns[I2C_TXN_BUFFER_SIZE + 1];
static volatile uint8_t txns_head = 0;
static volatile uint8_t txns_tail = 0;

static volatile uint8_t tx_buffer[I2C_TX_BUFFER_SIZE + 1];
static volatile uint8_t tx_head = 0;
static volatile uint8_t tx_tail = 0;

void i2c_enable(uint32_t bitrate) {
	TWSR = 0; // no prescaler
	TWBR = ((F_CPU / bitrate) - 16) / 2;
}

void i2c_disable() {
	TWCR = 0;
}

bool i2c_is_ready() {
	return (txns_head == txns_tail);
}

void i2c_send_start() {
	TWCR = _BV(TWEN) | _BV(TWSTA) | _BV(TWIE) | _BV(TWINT);
}

volatile i2c_txn_t * _i2c_write(uint8_t address, uint8_t *data, uint8_t len, callback_t cb) {
	volatile i2c_txn_t * txn = &txns[txns_head];
	txn->address = address;
	txn->callback = cb;
	txn->write = true;
	txn->len = len;
	txn->processed = 0;
	txn->completed = false;
	for (int i = 0; i < txn->len; i++) {
		tx_buffer[tx_head] = data[i];
		tx_head = (tx_head < I2C_TX_BUFFER_SIZE ? tx_head + 1 : 0);
	}
	bool was_empty = (txns_head == txns_tail);
	txns_head = (txns_head < I2C_TXN_BUFFER_SIZE ? txns_head + 1 : 0);
	if (was_empty)
		i2c_send_start();
	return txn;
}

volatile i2c_txn_t * i2c_write(uint8_t address, uint8_t * data, uint8_t len) {
	return _i2c_write(address, data, len, 0);
}

volatile i2c_txn_t * i2c_write_cb(uint8_t address, uint8_t * data, uint8_t len, callback_t cb) {
	return _i2c_write(address, data, len, cb);
}

volatile i2c_txn_t * _i2c_read(uint8_t address, volatile uint8_t * data, uint8_t len, callback_t cb) {
	volatile i2c_txn_t * txn = &txns[txns_head];
	txn->callback = cb;
	txn->address = address;
	txn->write = false;
	txn->len = len;
	txn->processed = 0;
	txn->data = data;
	txn->completed = false;
	bool was_empty = (txns_head == txns_tail);
	txns_head = (txns_head < I2C_TXN_BUFFER_SIZE ? txns_head + 1 : 0);
	if (was_empty)
		i2c_send_start();
	return txn;
}

volatile i2c_txn_t * i2c_read(uint8_t address, volatile uint8_t * data, uint8_t len) {
	return _i2c_read(address, data, len, 0);
}

volatile i2c_txn_t * i2c_read_cb(uint8_t address, volatile uint8_t * data, uint8_t len, callback_t cb) {
	return _i2c_read(address, data, len, cb);
}

void next_txn() {
	txns_tail = (txns_tail < I2C_TXN_BUFFER_SIZE ? txns_tail + 1 : 0);
	if (txns_head != txns_tail)
		TWCR = _BV(TWEN) | _BV(TWSTA) | _BV(TWIE) | _BV(TWINT);
	else
		TWCR = 0;//_BV(TWEN) | _BV(TWSTO) | _BV(TWIE) | _BV(TWINT);
}

ISR (TWI_vect) {
	volatile i2c_txn_t * txn = &(txns[txns_tail]);
	switch (TWSR & 0b11111000) {
		case 0x08: // 8 start sent
		case 0x10: // 16 repeated start sent
			TWDR = (txn->address << 1) | !txn->write;
			TWCR = _BV(TWEN) | _BV(TWIE) | _BV(TWINT);
			break;

		case 0x18: // 24 sla+w sent, ack received
			TWDR = tx_buffer[tx_tail];
			TWCR = _BV(TWEN) | _BV(TWIE) | _BV(TWINT);
			break;

		case 0x28: // byte sent, ack received
			tx_tail = (tx_tail < I2C_TX_BUFFER_SIZE ? tx_tail + 1 : 0);
			txn->processed++;
			if (txn->processed == txn->len) {
				if (txn->callback != 0)
					txn->callback();
				txn->completed = true;
				next_txn();
				break;
			}
			TWDR = tx_buffer[tx_tail];
			TWCR = _BV(TWEN) | _BV(TWIE) | _BV(TWINT);
			break;

		case 0x40: // sla+r sent, ack received
			TWCR = _BV(TWEN) | _BV(TWIE) | _BV(TWINT) | _BV(TWEA);
			break;

		case 0x50: // data received, ack returned
			txn->data[txn->processed] = TWDR;
			txn->processed++;
			if (txn->processed < txn->len)
				TWCR = _BV(TWEN) | _BV(TWIE) | _BV(TWINT) | _BV(TWEA);
			else
				TWCR = _BV(TWEN) | _BV(TWIE) | _BV(TWINT);
			break;

		case 0x58: // byte received, nack sent, end of transmission
			txn->data[txn->processed] = TWDR;
			txn->processed++;
			if (txn->callback != 0)
				txn->callback();
			txn->completed = true;
			next_txn();
			break;

		case 0: // illegal start/stop
		case 0x20: // sla+w sent, nack received
		case 0x30: // data sent, nack received
		case 0x48: // sla+r sent, nack received
		case 0x38: // arbitration lost
			TWCR |= _BV(TWSTO);
			TWCR |= _BV(TWINT);
			break;
		case 0xf8:
			while (1) {};
			break;
	}
}

volatile i2c_txn_t * i2c_write_register(uint8_t address, uint8_t reg, uint8_t value) {
	uint8_t buf[2] = {reg, value};
	return i2c_write(address, buf, 2);
}

volatile i2c_txn_t * i2c_write_registers(uint8_t address, uint8_t reg, uint8_t len, uint8_t * values) {
	uint8_t buf[len + 1];
	buf[0] = reg;
	for (int x = 0; x < len; x++)
		buf[x + 1] = values[x];
	return i2c_write(address, buf, len + 1);
}

volatile i2c_txn_t * i2c_read_register(uint8_t address, uint8_t reg, volatile uint8_t * value) {
	uint8_t buf = reg;
	i2c_write(address, &buf, 1);
	return i2c_read(address, value, 1);
}

volatile i2c_txn_t * i2c_read_register_cb(uint8_t address, uint8_t reg, volatile uint8_t * value, callback_t callback) {
	uint8_t buf = reg;
	i2c_write(address, &buf, 1);
	return i2c_read_cb(address, value, 1, callback);
}

volatile i2c_txn_t * i2c_read_registers(uint8_t address, uint8_t reg, uint8_t len, volatile uint8_t * value) {
	uint8_t buf = reg;
	i2c_write(address, &buf, 1);
	return i2c_read(address, value, len);
}

volatile i2c_txn_t * i2c_read_registers_cb(uint8_t address, uint8_t reg, uint8_t len, volatile uint8_t * value, callback_t cb) {
	uint8_t buf = reg;
	i2c_write(address, &buf, 1);
	return i2c_read_cb(address, value, len, cb);
}

void i2c_set_register_bit(uint8_t address, uint8_t reg, uint8_t bit) {
	volatile uint8_t buf;
	volatile i2c_txn_t * txn = i2c_read_register(address, reg, &buf);
	while (!txn->completed);
	i2c_write_register(address, reg, *txn->data | (1 << bit));
	while (!txn->completed);
}

void i2c_clear_register_bit(uint8_t address, uint8_t reg, uint8_t bit) {
	volatile uint8_t buf;
	volatile i2c_txn_t * txn = i2c_read_register(address, reg, &buf);
	while (!txn->completed);
	i2c_write_register(address, reg, *txn->data & ~(1 << bit));
	while (!txn->completed);
}

bool i2c_read_register_bit(uint8_t address, uint8_t reg, uint8_t bit) {
	volatile uint8_t buf;
	volatile i2c_txn_t * txn = i2c_read_register(address, reg, &buf);
	while (!txn->completed);
	return *txn->data & (1 << bit);
}

void i2c_set_register_bits(uint8_t address, uint8_t reg, uint8_t bits, uint8_t mask) {
	volatile uint8_t buf;
	volatile i2c_txn_t * txn = i2c_read_register(address, reg, &buf);
	while (!txn->completed);
	i2c_write_register(address, reg, (*txn->data & ~mask) | (bits & mask));
	while (!txn->completed);
}
