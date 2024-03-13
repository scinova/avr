#ifndef _i2c_h_
#define _i2c_h_

#include <stdint.h>
#include <stdbool.h>

void i2c_enable(uint32_t speed);
void i2c_disable();

void i2c_write_register(uint8_t address, uint8_t reg, uint8_t value);
void i2c_read_register(uint8_t address, uint8_t reg, uint8_t *value);

void i2c_write(uint8_t address, uint8_t *data, uint8_t len);
void i2c_read(uint8_t address, uint8_t *data, uint8_t len);
//void i2c_init(uint32_t bitrate);

#endif
