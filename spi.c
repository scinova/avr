#include <avr/io.h>
#include "spi.h"
#include "gpio.h"

#define SCK PinB5
#define MISO PinB4
#define MOSI PinB3

void spi_enable() {
	pin_mode(SCK, Output);
	pin_mode(MOSI, Output);
	SPCR = _BV(SPE) | _BV(MSTR) | _BV(SPR0); // clock/4
}

void spi_disable() {
	SPCR = 0;
	pin_mode(SCK, Input);
	pin_mode(MOSI, Input);
}

uint8_t spi_transfer8(uint8_t data) {
	SPDR = data;
	while (!(SPSR & _BV(SPIF)));
	return SPDR;
}

void spi_transfer(uint8_t * data, uint8_t len) {
	uint8_t *p = data;
	SPDR = *p;
	while (--len > 0) {
		uint8_t out = *(p + 1);
		while (!(SPSR & _BV(SPIF)));
		uint8_t in = SPDR;
		SPDR = out;
		*p++ = in;
	}
}
