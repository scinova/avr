#ifndef ds1307_h
#define ds1307_h

#include <time.h>
#include "i2c.h"

typedef enum {
  ds1307Frequency1Hz =		0,
	ds1307Frequency4KHz =		1,
	ds1307Frequency8KHz =		2,
	ds1307Frequency32KHz = 	3
} ds1307_frequency_t;

bool ds1307_is_halted();
volatile i2c_txn_t * ds1307_set_time(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second);
volatile i2c_txn_t * ds1307_set_time_tm(struct tm * time);
volatile i2c_txn_t * ds1307_read_time(volatile struct tm * time);
volatile i2c_txn_t * ds1307_enable_squarewave(ds1307_frequency_t frequency);
volatile i2c_txn_t * ds1307_disable_squarewave();

//uint8_t ds1307_read_nvram (uint8_t address);
//void ds1307_read_nvram_block (uint8_t address, uint8_t *buf, uint8_t size);
//void ds1307_write_nvram (uint8_t address, uint8_t data);
//void ds1307_write_nvram_block (uint8_t address, uint8_t *buf, uint8_t size);

#endif
