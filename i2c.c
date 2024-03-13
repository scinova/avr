#include <avr/io.h>
#include <avr/interrupt.h>
#include "i2c.h"

#define ACTIONS_QUEUE_SIZE 20
#define TX_QUEUE_SIZE 20
#define RX_QUEUE_SIZE 20

#define bit(x) ((uint8_t) 1U << (x))
#define cbi(sfr, b) (_SFR_BYTE(sfr) &= ~bit(b))
#define sbi(sfr, b) (_SFR_BYTE(sfr) |= bit(b))

typedef struct {
	uint8_t address;
	bool write;
	uint8_t len;
	uint8_t *dest;
	uint8_t done;
} i2c_action_t;

static i2c_action_t actions[ACTIONS_QUEUE_SIZE];
static uint8_t actions_head;
static uint8_t actions_tail;

static uint8_t tx_queue[TX_QUEUE_SIZE];
static uint8_t tx_head;
static uint8_t tx_tail;

static bool _busy;

void i2c_enable(uint32_t bitrate) {
	TWSR = 0; // no prescaler
	TWBR = ((F_CPU / bitrate) - 16) / 2;
	TWCR |= bit(TWEN); // enable twi
	TWCR |= bit(TWIE); // enable interrupt
}

void i2c_disable() {
	TWCR = 0;
}

void i2c_write(uint8_t address, uint8_t *data, uint8_t len) {
	i2c_action_t action;
	action.address = address;
	action.write = true;
	action.len = len;
	for (int i = 0; i < action.len; i++) {
		uint8_t next = (tx_head < TX_QUEUE_SIZE - 1 ? tx_head + 1 : 0);
		tx_queue[tx_head] = data[i];
		tx_head = next;
	}
	uint8_t next = (actions_head < ACTIONS_QUEUE_SIZE - 1 ? actions_head + 1 : 0);
	actions[actions_head] = action;
	actions_head = next;
}

void i2c_read(uint8_t address, uint8_t *data, uint8_t len) {
	i2c_action_t action;
	action.address = address;
	action.write = false;
	action.len = len;
	action.dest = data;
	uint8_t next = (actions_head < ACTIONS_QUEUE_SIZE - 1 ? actions_head + 1 : 0);
	actions[actions_head] = action;
	actions_head = next;
}
ISR (TWI_vect) {
	i2c_action_t action = actions[actions_tail];
	switch (TWSR & 0b11111000) {

		case 0x08: // start sent
		case 0x10: // repeated start sent
			cbi(TWCR, TWSTA);
			TWDR = (action.address << 1) | action.write;
			sbi(TWCR, TWINT);
			break;

		case 0x18: // sla+w sent, ack received
		case 0x28: // byte sent, ack received
			if (action.done == action.len) {
				// next transaction
				break;
			}
			uint8_t next = (tx_tail < TX_QUEUE_SIZE - 1 ? tx_tail + 1 : 0);
			TWDR = tx_queue[tx_tail];
			tx_tail = next;
			action.done++;
			sbi(TWCR, TWINT);
			break;

		case 0x40: // sla+r sent, ack received
		case 0x50: // data received, ack returned
			if (action.done == action.len) {
				// next transaction
				break;
			}
			action.dest[action.done++] = TWDR;
			sbi(TWCR, TWINT);
			break;

		case 0: // illegal start/stop
			_busy = false;
			break;
		case 0x20: // sla+w sent, nack received
		case 0x30: // data sent, nack received
		case 0x48: // sla+r sent, nack received
		case 0x58: // byte received, nack sent, end of transmission
		case 0x38: // arbitration lost
			_busy = false;
			sbi(TWCR, TWSTO);
			sbi(TWCR, TWINT);
			break;
	}
}

void i2c_write_register(uint8_t address, uint8_t reg, uint8_t value) {
	uint8_t buf[2] = {reg, value};
	i2c_write(address, buf, 2);
}

void i2c_write_registers(uint8_t address, uint8_t reg, uint8_t *values, uint8_t len) {
	//uint8_t buf = reg, value};
	i2c_write(address, &reg, 1);
}

void i2c_read_register(uint8_t address, uint8_t reg, uint8_t *value) {
	uint8_t buf = reg;
	i2c_write(address, &buf, 1);
	i2c_read(address, value, 1);
}
