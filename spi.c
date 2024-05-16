#include "spi.h"
#include "gpio.h"

void spi_enable() {
	pin_mode(SPI_SCK, Output);
	pin_mode(SPI_MOSI, Output);
	SPCR = _BV(SPE) | _BV(MSTR) | _BV(SPR0); // clock/4
}

void spi_disable() {
	SPCR = 0;
	pin_mode(SPI_SCK, Input);
	pin_mode(SPI_MOSI, Input);
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
