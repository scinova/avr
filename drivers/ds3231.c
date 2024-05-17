#include "ds3231.h"
#include "i2c.h"
#include <time.h>

#define DS3231_ADDRESS 0x68

#define DS3231_TIME_REGISTER			0x00
#define	DS3231_ALARM1_REGISTER			0x07
#define	DS3231_ALARM2_REGISTER			0x0B
#define	DS3231_CONTROL_REGISTER			0x0E
#define EOSC	7	// (not) enable oscillator
#define BBSQW	6	// battery-backed square wave enable
#define CONV	5	// convert temperature
#define RS2		4	// rate select
#define RS1		3
#define INTCN	2	// interrupt control
#define A2IE	1	// alarm2 interrupt enable
#define A1IE	0	// alarm1 interrupt enable
#define	DS3231_STATUS_REGISTER			0x0F
#define OSF			7 // oscillator stop flag
#define EN32KHZ 3	// enable 32kHz
#define BSY			2 // busy
#define A2F			1	// alarm 2 flag
#define A1F			0 // alarm 1 flag
#define	DS3231_TEMPERATURE_REGISTER		0x11


static uint8_t bcd2dec(uint8_t val) { return val - 6 * (val >> 4); }
static uint8_t dec2bcd(uint8_t val) { return val + 6 * (val / 10); }

bool ds3231_oscillator_is_stopped() {
	uint8_t buf;
	volatile i2c_txn_t * txn = i2c_read_register(DS3231_ADDRESS, DS3231_STATUS_REGISTER, &buf);
	while (!txn->completed);
	return buf & (1<<EOSC);
}

void ds3231_enable_oscillator() {
	i2c_clear_register_bit(DS3231_ADDRESS, DS3231_CONTROL_REGISTER, EOSC);
}

void ds3231_set_time(struct tm *t) {
	uint8_t buffer[7] = {
			dec2bcd(t->tm_sec),
			dec2bcd(t->tm_min),
			dec2bcd(t->tm_hour),
			0, //dayofweek
			dec2bcd(t->tm_mday),
			dec2bcd(t->tm_mon),
			dec2bcd(t->tm_year - 2000U)};
	i2c_write_registers(DS3231_ADDRESS, DS3231_TIME_REGISTER, 7, &buffer[0]);
}

volatile uint8_t _timebuf[10];
volatile struct tm * _time;

static void _read_time_callback() {
	_time->tm_sec = bcd2dec(_timebuf[0] & 0x7F);
	_time->tm_min = bcd2dec(_timebuf[1]);
	_time->tm_hour = bcd2dec(_timebuf[2]);
	_time->tm_mday = bcd2dec(_timebuf[4]);
	_time->tm_mon = bcd2dec(_timebuf[5] & 0x7F);
	_time->tm_year = bcd2dec(_timebuf[6]) + 2000U;
}

void ds3231_read_time(struct tm *t) {
	i2c_read_registers_cb(DS3231_ADDRESS, DS3231_TIME_REGISTER, 7, &_timebuf[0], _read_time_callback);
}

void ds3231_enable_square_wave(ds3231_squarewave_frequency_t frequency) {
	i2c_write_register(DS3231_ADDRESS, DS3231_CONTROL_REGISTER, frequency << RS1);
}

void ds3231_disable_square_wave() {
	i2c_write_register(DS3231_ADDRESS, DS3231_CONTROL_REGISTER, 0);
}
