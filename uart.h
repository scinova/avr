#ifndef _uart_h
#define _uart_h
#include <stdint.h>
#include <stdbool.h>

#if !defined(UART_TX_BUFFER_SIZE)
#define UART_TX_BUFFER_SIZE 50
#endif
#if !defined(UART_RX_BUFFER_SIZE)
#define UART_RX_BUFFER_SIZE 50
#endif

#ifdef __cplusplus
extern "C" {
#endif
void uart_enable(uint32_t bitrate);
void uart_enable_tx_only(uint32_t bitrate);
void uart_enable_rx_only(uint32_t bitrate);
void uart_disable();

bool uart_tx_buffer_is_empty();
void uart_write(uint8_t * data, uint8_t len);
void uart_print(uint8_t * str);

bool uart_data_is_available();
uint8_t uart_read();
#ifdef __cplusplus
}
#endif
#endif
