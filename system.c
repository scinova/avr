#include <avr/io.h>
#include <avr/interrupt.h>
#include "system.h"
#include <stdint.h>

static volatile uint64_t _microseconds;

ISR(TIMER0_OVF_vect) {
	_microseconds += 1024;
}

uint64_t system_us() {
	cli();
	uint64_t v = _microseconds;
	sei();
	return v;
}

uint32_t system_ms() {
	return system_us() / 1000;
}

void delay_ms(uint32_t ms) {
	uint32_t start = system_ms();
	while (system_ms() - start < ms);
}

void system_init(void) {
	sei();
	TCCR0A |= _BV(WGM00) | _BV(WGM01);
	TCCR0B |= _BV(CS00) | _BV(CS01); //clk/64
	TIMSK0 |= _BV(TOIE0);
}

int main() {
	system_init();
	setup();
	while (1)
		loop();
}
