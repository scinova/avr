#ifndef _spi_h_
#define _spi_h_

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
