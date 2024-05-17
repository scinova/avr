#ifndef _spi_h_
#define _spi_h_

#include <stdint.h>

typedef volatile struct {
	uint8_t CR;
	uint8_t SR;
	uint8_t DR;
} spi_t;

typedef enum {
	SPI_CR_SPR  = 0b00000011,
		SPI_CR_SPR_DIV4   = 0x00,
		SPI_CR_SPR_DIV16  = 0x01,
		SPI_CR_SPR_DIV64  = 0x10,
		SPI_CR_SPR_DIV128 = 0x11,
	SPI_CR_CPHA = 0b00000100,
	SPI_CR_CPOL = 0b00001000,
		SPI_CR_CPOL_RISING  = 0,
		SPI_CR_CPOL_FALLING = 1,
	SPI_CR_MSTR = 0b00010000,
	SPI_CR_DORD = 0b00100000,
		SPI_CR_DORD_MSB = 0,
		SPI_CR_DORD_LSB = 1,
	SPI_CR_SPE  = 0b01000000,
	SPI_CR_SPIE = 0b10000000
} spi_spcr_t;

typedef enum {
	SPI_SR_SPI2X = 0b00000001,
	SPI_SR_WCOL  = 0b01000000,
	SPI_SR_SPIF    = 0b10000000
} spi_spsr_t;

#define SPI ((spi_t *)(0x20 + 0x2C))

#if defined (__AVR_ATmega2560__)
#define SPI_SCK  PinB7
#define SPI_MISO PinB6
#define SPI_MOSI PinB5
#define SPI_SS   PinB4
#elif defined (__AVR_ATmega328P__)
#define SPI_SCK  PinB5
#define SPI_MISO PinB4
#define SPI_MOSI PinB3
#define SPI_SS   PinB2
#endif

#ifdef __cplusplus
extern "C" {
#endif

void spi_enable();
void spi_disable();
uint8_t spi_transfer8(uint8_t data);
void spi_transfer(uint8_t * data, uint8_t len);

#ifdef __cplusplus
}
#endif
#endif
