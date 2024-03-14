#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <stddef.h>
#include "adc.h"

static adc_conversion_t _adc_queue[ADC_QUEUE_LENGTH + 1];
static volatile uint8_t _adc_queue_head;
static volatile uint8_t _adc_queue_tail;

void adc_set_aref_reference() {
	ADMUX = 0;
}

inline void adc_set_vcc_reference() {
	ADMUX = _BV(REFS0);
}

void adc_set_1v1_reference() {
	ADMUX = _BV(REFS0) | _BV(REFS1);
#if defined (__AVR_ATmega256__)
	ADMUX = _BV(REFS1);
#elif defined (__AVR_ATtiny85__)
	ADMUX = _BV(REFS0) | _BV(REFS1);
#endif
}

#if defined (__AVR_ATtiny85__) || defined (__AVR_ATmega2560__)
void adc_set_2v56_reference() {
#if defined (__AVR_ATtiny85__)
	ADMUX &= ~(_BV(REFS0) | _BV(REFS2));
#else
	ADMUX &= ~_BV(REFS0);
#endif
	ADMUX |= _BV(REFS1);
}
#endif

void adc_enable(void)	{
	adc_set_vcc_reference();
	ADCSRA |= _BV(ADEN) | _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0); // clk/128
}

void adc_disable(void) {
	ADCSRA = 0;
	ADMUX = 0;
}

void _start_next_conversion() {
	adc_conversion_t * conversion = &_adc_queue[_adc_queue_tail];
	ADMUX = (ADMUX & 0xF0) | conversion->channel;
	ADCSRA |= _BV(ADSC);
}

adc_conversion_t * adc_convert(uint8_t channel) {
	uint8_t next = (_adc_queue_head < ADC_QUEUE_LENGTH - 1 ? _adc_queue_head + 1 : 0);
	if (next == _adc_queue_tail)
		return NULL;
	adc_conversion_t * conversion = &_adc_queue[_adc_queue_head];
	conversion->channel = channel;
	bool queue_was_empty = (_adc_queue_head == _adc_queue_tail);
	_adc_queue_head = next;
	if (queue_was_empty) {
		ADCSRA |= _BV(ADIE);
		_start_next_conversion();
	}
	return conversion;
}

ISR(ADC_vect) {
	adc_conversion_t * conversion = &_adc_queue[_adc_queue_tail];
	conversion->value = ADCW;
	conversion->completed = true;
	uint8_t next = (_adc_queue_tail < ADC_QUEUE_LENGTH - 1 ? _adc_queue_tail + 1 : 0);
	_adc_queue_tail = next;
	if (_adc_queue_head == _adc_queue_tail) {
		ADCSRA &= ~_BV(ADIE);
		return;
	}
	_start_next_conversion();
}
