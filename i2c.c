#include <avr/io.h>
#include <avr/interrupt.h>
#include "i2c.h"
#include <stddef.h>

#define ACTIONS_QUEUE_SIZE 40
#define TX_BUFFER_SIZE 40

static volatile i2c_action_t actions[ACTIONS_QUEUE_SIZE + 1];
static volatile uint8_t actions_head = 0;
static volatile uint8_t actions_tail = 0;

static volatile uint8_t tx_buffer[TX_BUFFER_SIZE + 1];
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
	return (actions_head == actions_tail);
}

void i2c_send_start() {
	TWCR = _BV(TWEN) | _BV(TWSTA) | _BV(TWIE) | _BV(TWINT);
}

volatile i2c_txn_t * _i2c_write(uint8_t address, uint8_t *data, uint8_t len, callback_t cb) {
	volatile i2c_txn_t * action = &actions[actions_head];
	action->address = address;
	action->callback = cb;
	action->write = true;
	action->len = len;
	action->processed = 0;
	action->completed = false;
	for (int i = 0; i < action->len; i++) {
		tx_buffer[tx_head] = data[i];
		tx_head = (tx_head < TX_BUFFER_SIZE ? tx_head + 1 : 0);
	}
	bool was_empty = (actions_head == actions_tail);
	actions_head = (actions_head < ACTIONS_QUEUE_SIZE ? actions_head + 1 : 0);
	if (was_empty)
		i2c_send_start();
	return action;
}

volatile i2c_txn_t * i2c_write(uint8_t address, uint8_t * data, uint8_t len) {
	return _i2c_write(address, data, len, 0);
}

volatile i2c_txn_t * i2c_write_cb(uint8_t address, uint8_t * data, uint8_t len, callback_t cb) {
	return _i2c_write(address, data, len, cb);
}

volatile i2c_txn_t * _i2c_read(uint8_t address, volatile uint8_t * data, uint8_t len, callback_t cb) {
	volatile i2c_action_t * action = &actions[actions_head];
	action->callback = cb;
	action->address = address;
	action->write = false;
	action->len = len;
	action->processed = 0;
	action->data = data;
	action->completed = false;
	bool was_empty = (actions_head == actions_tail);
	actions_head = (actions_head < ACTIONS_QUEUE_SIZE ? actions_head + 1 : 0);
	if (was_empty)
		i2c_send_start();
	return action;
}

volatile i2c_txn_t * i2c_read(uint8_t address, volatile uint8_t * data, uint8_t len) {
	return _i2c_read(address, data, len, 0);
}

volatile i2c_txn_t * i2c_read_cb(uint8_t address, volatile uint8_t * data, uint8_t len, callback_t cb) {
	return _i2c_read(address, data, len, cb);
}

void next_action() {
	actions_tail = (actions_tail < ACTIONS_QUEUE_SIZE ? actions_tail + 1 : 0);
	if (actions_head != actions_tail)
		TWCR = _BV(TWEN) | _BV(TWSTA) | _BV(TWIE) | _BV(TWINT);
	else
		TWCR = 0;//_BV(TWEN) | _BV(TWSTO) | _BV(TWIE) | _BV(TWINT);
}

ISR (TWI_vect) {
	volatile i2c_action_t * action = &(actions[actions_tail]);
	switch (TWSR & 0b11111000) {
		case 0x08: // 8 start sent
		case 0x10: // 16 repeated start sent
			TWDR = (action->address << 1) | !action->write;
			TWCR = _BV(TWEN) | _BV(TWIE) | _BV(TWINT);
			break;

		case 0x18: // 24 sla+w sent, ack received
			TWDR = tx_buffer[tx_tail];
			TWCR = _BV(TWEN) | _BV(TWIE) | _BV(TWINT);
			break;

		case 0x28: // byte sent, ack received
			tx_tail = (tx_tail < TX_BUFFER_SIZE ? tx_tail + 1 : 0);
			action->processed++;
			if (action->processed == action->len) {
				if (action->callback != 0)
					action->callback();
				action->completed = true;
				next_action();
				break;
			}
			TWDR = tx_buffer[tx_tail];
			TWCR = _BV(TWEN) | _BV(TWIE) | _BV(TWINT);
			break;

		case 0x40: // sla+r sent, ack received
			TWCR = _BV(TWEN) | _BV(TWIE) | _BV(TWINT) | _BV(TWEA);
			break;

		case 0x50: // data received, ack returned
			action->data[action->processed] = TWDR;
			action->processed++;
			if (action->processed < action->len)
				TWCR = _BV(TWEN) | _BV(TWIE) | _BV(TWINT) | _BV(TWEA);
			else
				TWCR = _BV(TWEN) | _BV(TWIE) | _BV(TWINT);
			break;

		case 0x58: // byte received, nack sent, end of transmission
			action->data[action->processed] = TWDR;
			action->processed++;
			if (action->callback != 0)
				action->callback();
			action->completed = true;
			next_action();
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
