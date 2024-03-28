#ifndef adc_h
#define adc_h

#include <stdint.h>
#include <stdbool.h>

#if !defined(ADC_QUEUE_LENGTH)
	#define ADC_QUEUE_LENGTH 20
#endif

typedef void (* callback_t) (void);

typedef struct {
	uint8_t channel;
	volatile uint16_t * value;
	bool completed;
} adc_conversion_t;

typedef enum {
	adcReferenceARef = 0,
	adcReferenceVcc = 1,
	adcReference1V1 = 2,
	adcReference2V26 = 3
} adc_reference_t;

#ifdef __cplusplus
extern "C" {
#endif

void adc_set_aref_reference();
void adc_set_vcc_reference();
void adc_set_1v1_reference();
#if defined (__AVR_ATtiny85__) | defined (__AVR_ATmega2560__)
void adc_set_2v56_reference();
#endif

void adc_enable();//adc_reference_t reference);
void adc_disable();

void adc_convert(uint8_t channel, uint16_t * valueptr);
void adc_convert_cb(uint8_t channel, uint16_t * valueptr, callback_t callback);
#ifdef __cplusplus
}
#endif
#endif
