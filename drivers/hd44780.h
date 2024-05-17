#ifndef _hd44780_h
#define _hd44780_h
#include "gpio.h"
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

void hd44780_enable(pin_t rs_pin, pin_t rw_pin, pin_t e_pin, port_t data_port);
void hd44780_disable();
void hd44780_print(uint8_t x, uint8_t y, uint8_t * data);

#ifdef __cplusplus
}
#endif
#endif
