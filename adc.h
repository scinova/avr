#ifndef adc_h
#define adc_h

#include <stdint.h>
#include <stdbool.h>

#if !defined(ADC_QUEUE_LENGTH)
	#define ADC_QUEUE_LENGTH 1
#endif

typedef struct {
	uint8_t channel;
	volatile bool completed;
	volatile uint16_t value;
} adc_conversion_t;

void adc_set_aref_reference();
void adc_set_vcc_reference();
void adc_set_1v1_reference();
#if defined (__AVR_ATtiny85__) | defined (__AVR_ATmega2560__)
void adc_set_2v56_reference();
#endif
void adc_enable();
void adc_disable();
adc_conversion_t * adc_convert(uint8_t channel);

#endif
