#ifndef _ws2812b_h
#define _ws2812b_h

#include "gpio.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void ws2812b_write(pin_t pin, uint8_t * data, uint16_t len);

#ifdef __cplusplus
}
#endif
#endif
