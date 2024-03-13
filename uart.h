#ifndef _uart_h
#define _uart_h

#include <stdint.h>
#include <stdbool.h>

void uart_enable(uint32_t baudrate);
void uart_enable_tx_only(uint32_t baudrate);
void uart_enable_rx_only(uint32_t baudrate);
void uart_disable();

bool uart_can_write();
void uart_write(uint8_t *data, uint8_t len);

bool uart_can_read();
uint8_t uart_read();

#endif
