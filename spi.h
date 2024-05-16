#ifndef _spi_h_
#define _spi_h_

#include <stdint.h>
#include <avr/io.h>
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
