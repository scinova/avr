#include <avr/io.h>
#include <stdbool.h>
#include "gpio.h"

void gpio_pin_mode(gpio_pin_t pin, gpio_pin_mode_t mode) {
	volatile uint8_t * port = (volatile uint8_t *)(0x20 + (2 + 3 * (pin >> 8)));
	volatile uint8_t * ddr = (volatile uint8_t *)(0x20 + (1 + 3 * (pin >> 8)));
  uint8_t pin_mask = pin & 0xff;
	switch (mode) {
		case Input:
			*port &= ~pin_mask;
			break;
		case InputPullup:
      *ddr &= ~pin_mask;
      *port |= pin_mask;
			break;
		case Output:
			*ddr |= pin_mask;
			break;
	}
}

void gpio_pin_set(gpio_pin_t pin) {
	volatile uint8_t * port = (volatile uint8_t *)(0x20 + (2 + 3 * (pin >> 8)));
  uint8_t pin_mask = pin & 0xff;
  *port |= pin_mask;
}

void gpio_pin_clear(gpio_pin_t pin) {
	volatile uint8_t * port = (volatile uint8_t *)(0x20 + (2 + 3 * (pin >> 8)));
  uint8_t pin_mask = pin & 0xff;
  *port &= ~pin_mask;
}

void gpio_pin_write(gpio_pin_t pin, bool value) {
	if (value)
		gpio_pin_set(pin);
	else
		gpio_pin_clear(pin);
}
