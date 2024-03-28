#ifndef _i2c_h_
#define _i2c_h_

#include <stdint.h>
#include <stdbool.h>

#ifndef I2C_TXN_BUFFER_SIZE
#define I2C_TXN_BUFFER_SIZE 10
#endif
#ifndef I2C_TX_BUFFER_SIZE
#define I2C_TX_BUFFER_SIZE 10
#endif

typedef void (* callback_t) (void);

typedef struct {
	uint8_t address;
	bool write;
	uint8_t len;
	volatile uint8_t * data;
	uint8_t processed;
	callback_t callback;
	bool completed;
} i2c_txn_t;

#ifdef __cplusplus
extern "C" {
#endif
void i2c_enable(uint32_t bitrate);
bool i2c_is_ready();

volatile i2c_txn_t * i2c_write(uint8_t address, uint8_t * data, uint8_t len);
volatile i2c_txn_t * i2c_write_cb(uint8_t address, uint8_t * data, uint8_t len, callback_t cb);
volatile i2c_txn_t * i2c_read(uint8_t address, volatile uint8_t * data, uint8_t len);
volatile i2c_txn_t * i2c_read_cb(uint8_t address, volatile uint8_t * data, uint8_t len, callback_t cb);

volatile i2c_txn_t * i2c_write_register(uint8_t address, uint8_t reg, uint8_t value);
volatile i2c_txn_t * i2c_read_register(uint8_t address, uint8_t reg, volatile uint8_t * value);
volatile i2c_txn_t * i2c_read_register_cb(uint8_t address, uint8_t reg, volatile uint8_t * value, callback_t cb);

volatile i2c_txn_t * i2c_write_registers(uint8_t address, uint8_t reg, uint8_t len, uint8_t * value);
volatile i2c_txn_t * i2c_read_registers(uint8_t address, uint8_t reg, uint8_t len, volatile uint8_t * value);
volatile i2c_txn_t * i2c_read_registers_cb(uint8_t address, uint8_t reg, uint8_t len, volatile uint8_t * value, callback_t cb);

void i2c_set_register_bit(uint8_t address, uint8_t reg, uint8_t bit);
void i2c_clear_register_bit(uint8_t address, uint8_t reg, uint8_t bit);
#ifdef __cplusplus
}
#endif
#endif
