#ifndef _gpio_h
#define _gpio_h
#include <stdbool.h>

typedef enum {
	PortA = 1,
	PortB,
	PortC,
	PortD,
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
	PinSCK = PinB5,
	PinMOSI = PinB4,
	PinSS = PinB2,
	P0 = PinD0,
	P1 = PinD1,
	P2 = PinD2,
	P3 = PinD3,
	P4 = PinD4,
	P5 = PinD5,
	P6 = PinD6,
	P7 = PinD7,
	P8 = PinB0,
	P9 = PinB1,
	P10 = PinB2,
	P11 = PinB3,
	P12 = PinB4,
	P13 = PinB5,
	A0 = PinC0,
	A1 = PinC1,
	A2 = PinC2,
	A3 = PinC3,
	A4 = PinC4,
	A5 = PinC5,
 } pin_t;

typedef enum {
	Input,
	InputPullup,
	Output,
} pin_mode_t;

#define pin_port(pin) ((port_t) (pin >> 4))
#define pin_number(pin) (pin & 0xF)
#define pin_mask(pin) ((uint8_t)(1 << pin_number(pin)))

#ifdef __cplusplus
extern "C" {
#endif
void pin_mode(pin_t pin, pin_mode_t mode);
void pin_set(pin_t pin);
void pin_reset(pin_t pin);
inline void pin_write(pin_t pin, bool value) {(value ? pin_set(pin) : pin_reset(pin));};
#ifdef __cplusplus
}
#endif
#endif
