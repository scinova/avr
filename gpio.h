#ifndef _gpio_h
#define _gpio_h

#include <stdint.h>
#include <stdbool.h>

typedef volatile struct {
	uint8_t PIN;
	uint8_t DDR;
	uint8_t PORT;
} gpio_t;

#if defined (__AVR_ATtiny85__)
#define GPIOBASE 0x13
#else
#define GPIOBASE 0x00
#endif
#define GPIO(x) ((gpio_t *)(0x20 + GPIOBASE + 3 * x))

typedef enum {
	Input,
	InputPullUp,
	Output,
} pin_mode_t;

typedef enum {
	PortA = 0,
	PortB = 1,
	PortC = 2,
	PortD = 3,
} port_t;

typedef enum {
	PinA0 = 0x00,
	PinA1 = 0x01,
	PinA2 = 0x02,
	PinA3 = 0x03,
	PinA4 = 0x04,
	PinA5 = 0x05,
	PinA6 = 0x06,
	PinA7 = 0x07,
	PinB0 = 0x10,
	PinB1 = 0x11,
	PinB2 = 0x12,
	PinB3 = 0x13,
	PinB4 = 0x14,
	PinB5 = 0x15,
	PinB6 = 0x16,
	PinB7 = 0x17,
	PinC0 = 0x20,
	PinC1 = 0x21,
	PinC2 = 0x22,
	PinC3 = 0x23,
	PinC4 = 0x24,
	PinC5 = 0x25,
	PinC6 = 0x26,
	PinC7 = 0x27,
	PinD0 = 0x30,
	PinD1 = 0x31,
	PinD2 = 0x32,
	PinD3 = 0x33,
	PinD4 = 0x34,
	PinD5 = 0x35,
	PinD6 = 0x36,
	PinD7 = 0x37,
} pin_t;

//#define pin_port(pin) ((port_t) (pin >> 4))
//#define pin_number(pin) (pin & 0x0F)
//#define pin_mask(pin) ((uint8_t)(1 << pin_number(pin)))

#ifdef __cplusplus
extern "C" {
#endif

inline port_t pin_port(pin_t pin) {
	return (port_t)(pin >> 4);
}

inline uint8_t pin_number(pin_t pin) {
	return pin & 0x0F;
}

inline uint8_t pin_mask(pin_t pin) {
	return 1 << pin_number(pin);
}

void pin_mode(pin_t pin, pin_mode_t mode);
void pin_set(pin_t pin);
void pin_reset(pin_t pin);
void pin_write(pin_t pin, bool value);
bool pin_read(pin_t pin);
void port_mode(port_t port, pin_mode_t mode);
void port_write_byte(port_t port, uint8_t value);
uint8_t port_read_byte(port_t port);

#ifdef __cplusplus
}
#endif
#endif
