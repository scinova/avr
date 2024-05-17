#include "ds1307.h"
#include "i2c.h"
#include <time.h>

static uint8_t bcd2dec(uint8_t val) { return val - 6 * (val >> 4); }
static uint8_t dec2bcd(uint8_t val) { return val + 6 * (val / 10); }

#define ADDR 0x68

typedef enum {
	SECONDS = 0,
	MINUTES,
	HOURS,
	DAY, 	//day of week
	DATE,	//day of month
	MONTH,
	YEAR,
	CONTROL,
	NVRAM // 56 bytes, 0x08 to 0x3f
} ds1307_register_t;

// SECONDS
#define CH			7 // clock halt
// HOURS
#define MODE12H	6 // 12h mode
// CONTROL
#define OUT			7
#define SQWE		4
#define RS1			1
#define RS0			0
#define RS			(1 << RS1) | (1 << RS0)

bool ds1307_is_halted() {
	volatile uint8_t data = 0;
	volatile i2c_txn_t * txn = i2c_read_register(ADDR, 0, &data);
	while (!txn->completed);
	return data & 0x80;
}

volatile i2c_txn_t * ds1307_enable_squarewave(ds1307_frequency_t frequency) {
	return i2c_write_register(ADDR, CONTROL, SQWE | frequency << RS0);
}

volatile i2c_txn_t * ds1307_disable_squarewave() {
	return i2c_write_register(ADDR, CONTROL, 0);
}

volatile uint8_t _time_buffer[10];
volatile struct tm * _time;

void _process_time() {
	_time->tm_sec = bcd2dec(_time_buffer[0] & 0x7f);
	_time->tm_min = bcd2dec(_time_buffer[1]);
	_time->tm_hour = bcd2dec(_time_buffer[2] & 0x3f);
	_time->tm_wday = bcd2dec(_time_buffer[3]);
	_time->tm_mday = bcd2dec(_time_buffer[4]);
	_time->tm_mon = bcd2dec(_time_buffer[5]) - 1;
	_time->tm_year = bcd2dec(_time_buffer[6]) + 100;
}

volatile i2c_txn_t * ds1307_read_time(volatile struct tm * dest) {
  _time = dest;
  return i2c_read_registers_cb(ADDR, 0, 7, &_time_buffer[0], _process_time);
}

volatile i2c_txn_t * ds1307_set_time(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second) {
	uint8_t _buf[7] = {
		0x80,
		dec2bcd(minute),
		dec2bcd(hour),
		0,
		dec2bcd(day),
		dec2bcd(month),
		dec2bcd(year - 2000)};
	i2c_write_registers(ADDR, 0, 7, &_buf[0]);
	return i2c_write_register(ADDR, SECONDS, dec2bcd(second) & 0x7f);
}

volatile i2c_txn_t * ds1307_set_time_tm(struct tm * t) {
	uint8_t buf[7] = {
		0x80,
		dec2bcd(t->tm_min),
		dec2bcd(t->tm_hour),
		0,
		dec2bcd(t->tm_mday),
		dec2bcd(t->tm_mon),
		dec2bcd(t->tm_year - 100)};
	i2c_write_register(ADDR, 0, buf[0]);
	i2c_write_register(ADDR, 1, buf[1]);
	i2c_write_register(ADDR, 2, buf[2]);
	i2c_write_register(ADDR, 3, buf[3]);
	i2c_write_register(ADDR, 4, buf[4]);
	i2c_write_register(ADDR, 5, buf[5]);
	i2c_write_register(ADDR, 6, buf[6]);
	return i2c_write_register(ADDR, SECONDS, dec2bcd(t->tm_sec) & 0x7f);
}
