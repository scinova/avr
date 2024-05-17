#ifndef _st7735_h
#define _st7735_h
#include "gpio.h"
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

void st7735_enable(pin_t cspin, pin_t dcpin);
void st7735_disable();
void st7735_set_rotation(int rotation);
void st7735_set_pixel(uint16_t x, uint16_t y, uint16_t color);
void st7735_set_block(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
void st7735_write_block(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t * data);

#ifdef __cplusplus
}
#endif
#endif
