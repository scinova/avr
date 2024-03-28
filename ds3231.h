#ifndef _ds3231_h
#define _ds3231_h

#include "i2c.h"
#include <time.h>

typedef enum {
	ds3231SquareWave1Hz  = 0,
	ds3231SquareWave1024Hz = 1,
	ds3231SquareWave4096Hz = 2,
	ds3231SquareWave8192Hz = 3
} ds3231_squarewave_frequency_t ;

bool ds3231_oscillator_is_stopped();
void ds3231_enable_oscillator();
void ds3231_set_time(struct tm * t);
void ds3231_read_time(struct tm * t);
void ds3231_enable_squarewave(ds3231_squarewave_frequency_t frequency);
void ds3231_disable_squarewave();

#endif
