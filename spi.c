#include <avr/io.h>
#include "spi.h"

#define SPIDDR DDRB
#define SCK 5
#define MISO 4
#define MOSI 3

void spi_enable() {
	SPIDDR |= _BV(SCK) | _BV(MOSI);
	SPCR = _BV(SPE) | _BV(MSTR); // clock/4
}

void spi_disable() {
	SPCR = 0;
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
