#include <avr/io.h>
#include <stdbool.h>
#include "gpio.h"

void pin_mode(pin_t pin, pin_mode_t mode) {
	volatile uint8_t * port = (volatile uint8_t *)(0x20 + (2 + 3 * pin_port(pin)));
	volatile uint8_t * ddr = (volatile uint8_t *)(0x20 + (1 + 3 * pin_port(pin)));
  //uint8_t pin_mask = pin & 0xf;
	switch (mode) {
		case Input:
			*port &= ~pin_mask(pin);
			break;
		case InputPullup:
      *ddr &= ~pin_mask(pin);
      *port |= pin_mask(pin);
			break;
		case Output:
			*ddr |= pin_mask(pin);
			break;
	}
}

void pin_set(pin_t pin) {
	volatile uint8_t * port = (volatile uint8_t *)(0x20 + (2 + 3 * pin_port(pin)));
  *port |= pin_mask(pin);
}

void pin_reset(pin_t pin) {
	volatile uint8_t * port = (volatile uint8_t *)(0x20 + (2 + 3 * pin_port(pin)));
  *port &= ~pin_mask(pin);
}
