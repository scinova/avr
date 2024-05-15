#include <avr/io.h>
#include <stdbool.h>
#include "gpio.h"

void pin_mode(pin_t pin, pin_mode_t mode) {
	gpio_t * gpio = GPIO(pin_port(pin));
	switch (mode) {
		case Input:
			gpio->DDR &= pin_mask(pin);
			gpio->PORT &= ~pin_mask(pin);
			break;
		case InputPullUp:
			gpio->DDR &= ~pin_mask(pin);
			gpio->PORT |= pin_mask(pin);
			break;
		case Output:
			gpio->DDR |= pin_mask(pin);
			gpio->PORT &= ~pin_mask(pin);
			break;
	}
}

void port_mode(port_t port, pin_mode_t mode) {
	gpio_t * gpio = GPIO(port);
	switch (mode) {
		case Input:
			gpio->DDR = 0x00;
			gpio->PORT = 0x00;
			break;
		case InputPullUp:
			gpio->DDR = 0x00;
			gpio->PORT = 0xFF;
			break;
		case Output:
			gpio->DDR = 0xFF;
			gpio->PORT = 0x00;
			break;
	}
}

void pin_set(pin_t pin) {
	GPIO(pin_port(pin))->PORT |= pin_mask(pin);
}

void pin_reset(pin_t pin) {
	GPIO(pin_port(pin))->PORT &= ~pin_mask(pin);
}

void pin_write(pin_t pin, bool value) {
	if (value)
		pin_set(pin);
	else
		pin_reset(pin);
}

bool pin_read(pin_t pin) {
	return GPIO(pin_port(pin))->PIN & pin_mask(pin);
}
void port_write_byte(port_t port, uint8_t value) {
	GPIO(port)->PORT = value;
}

uint8_t port_read_byte(port_t port) {
	return GPIO(port)->PIN;
}
