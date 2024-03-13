#ifndef adc_h
#define adc_h

#include <stdint.h>
#include <stdbool.h>

void adc_set_aref_reference();
void adc_set_vcc_reference();
void adc_set_1v1_reference();
#if defined (__AVR_ATtiny85__) | defined (__AVR_ATmega2560__)
void adc_set_2v56_reference();
#endif
void adc_enable();
void adc_disable();
void adc_read(uint8_t channel, uint16_t * volatile val);
bool adc_ready();

#endif
