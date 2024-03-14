#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include "timer.h"

static volatile uint64_t _microseconds = 0;

ISR(TIMER0_OVF_vect) {
	_microseconds += 1024;
}

uint64_t timer_us() {
	return _microseconds;
}

uint32_t timer_ms() {
	return timer_us() / 1000;
}

void timer_enable() {
	TCCR0A |= _BV(WGM00) | _BV(WGM01);
	TCCR0B |= _BV(CS00) | _BV(CS01); //clk/64
#if defined (__AVR_ATtiny85__)
	TIMSK |= _BV(TOIE0);
#else
	TIMSK0 |= _BV(TOIE0);
#endif
	sei();
}

void timer_disable() {
	TCCR0A = 0;
	TCCR0B = 0;
#if defined (__AVR_ATtiny85__)
	TIMSK &= ~_BV(TOIE0);
#else
	TIMSK0 &= ~_BV(TOIE0);
#endif
}
