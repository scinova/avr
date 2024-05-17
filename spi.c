#include "spi.h"
#include "gpio.h"

void spi_enable() {
	pin_mode(SPI_SCK, Output);
	pin_mode(SPI_MOSI, Output);
	SPI->CR = SPI_CR_SPE | SPI_CR_MSTR | SPI_CR_SPR_DIV4;
}

void spi_disable() {
	SPI->CR = 0;
	pin_mode(SPI_SCK, Input);
	pin_mode(SPI_MOSI, Input);
}

uint8_t spi_transfer8(uint8_t data) {
	SPI->DR = data;
	while (!(SPI->SR & SPI_SR_SPIF));
	return SPI->DR;
}

void spi_transfer(uint8_t * data, uint8_t len) {
	uint8_t *p = data;
	SPI->DR = *p;
	while (--len > 0) {
		uint8_t out = *(p + 1);
		while (!(SPI->SR & SPI_SR_SPIF));
		uint8_t in = SPI->DR;
		SPI->DR = out;
		*p++ = in;
	}
}
