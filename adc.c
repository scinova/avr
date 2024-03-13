#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include "adc.h"

#define bit(x) _BV(x)

static uint8_t adc_channel;
uint16_t * volatile adc_value;

void adc_set_aref_reference() {
	ADMUX = 0;
}

inline void adc_set_vcc_reference() {
	ADMUX = bit(REFS0);
}

void adc_set_1v1_reference() {
	ADMUX = bit(REFS0) | bit(REFS1);
#if defined (__AVR_ATmega256__)
	ADMUX = bit(REFS1);
#elif defined (__AVR_ATtiny85__)
	ADMUX = bit(REFS0) | bit(REFS1);
#endif
}

#if defined (__AVR_ATtiny85__) || defined (__AVR_ATmega2560__)
void adc_set_2v56_reference() {
#if defined (__AVR_ATtiny85__)
	ADMUX &= ~(bit(REFS0) | bit(REFS2));
#else
	ADMUX &= ~bit(REFS0);
#endif
	ADMUX |= bit(REFS1);
}
#endif

void adc_enable(void)	{
	adc_set_vcc_reference();
	ADCSRA |=
			bit(ADPS2) | bit(ADPS1) | bit(ADPS0) | // clk/128
			bit(ADEN) | bit(ADIE);
}

void adc_disable(void) {
	ADCSRA = 0;
	ADMUX = 0;
}

void adc_read(uint8_t channel, uint16_t * volatile value) {
	adc_channel = channel;
	adc_value = value;
	ADMUX = (ADMUX & 0xF0) | channel;
	ADCSRA |= bit(ADIE) | bit(ADSC);
}

bool adc_ready() {
	return ADCSRA & ~bit(ADSC);
}

ISR(ADC_vect) {
	*adc_value = ADCW;
}
