#ifndef _gpio_h
#define _gpio_h
#include <stdbool.h>

typedef enum {
  Input,
  InputPullup,
  Output,
} gpio_pin_mode_t;

typedef enum {
  PinA0 = 0x0001,
  PinA1 = 0x0002,
  PinA2 = 0x0004,
  PinA3 = 0x0008,
  PinA4 = 0x0010,
  PinA5 = 0x0020,
  PinA6 = 0x0040,
  PinA7 = 0x0080,
  PinB0 = 0x0101,
  PinB1 = 0x0102,
  PinB2 = 0x0104,
  PinB3 = 0x0108,
  PinB4 = 0x0110,
  PinB5 = 0x0120,
  PinB6 = 0x0140,
  PinB7 = 0x0180,
  PinC0 = 0x0201,
  PinC1 = 0x0202,
  PinC2 = 0x0204,
  PinC3 = 0x0208,
  PinC4 = 0x0210,
  PinC5 = 0x0220,
  PinC6 = 0x0240,
  PinC7 = 0x0280,
  PinD0 = 0x0301,
  PinD1 = 0x0302,
  PinD2 = 0x0304,
  PinD3 = 0x0308,
  PinD4 = 0x0310,
  PinD5 = 0x0320,
  PinD6 = 0x0340,
  PinD7 = 0x0380,
} gpio_pin_t;

#ifdef __cplusplus
extern "C" {
#endif
void gpio_pin_mode(gpio_pin_t pin, gpio_pin_mode_t mode);
void gpio_pin_set(gpio_pin_t pin);
void gpio_pin_clear(gpio_pin_t pin);
void gpio_pin_write(gpio_pin_t pin, bool value);
#ifdef __cplusplus
}
#endif
#endif
