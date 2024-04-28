#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#include <stdbool.h>
#include "uart.h"

static uint8_t tx_buffer[UART_TX_BUFFER_SIZE + 1];
static volatile uint8_t tx_buffer_head;
static volatile uint8_t tx_buffer_tail;

static volatile uint8_t rx_buffer[UART_RX_BUFFER_SIZE + 1];
static volatile uint8_t rx_buffer_head;
static volatile uint8_t rx_buffer_tail;

void _uart_enable(uint32_t bitrate, bool enable_tx, bool enable_rx) {
	// SEL0=0 UPM0=0 USBS0=0 UCSZ0=3 async, no parity, 1 stop bit, 8 bit size
	UCSR0C |= _BV(UCSZ00) | _BV(UCSZ01);
	uint32_t b = (F_CPU / (bitrate * 16UL)) - 1;
	UBRR0L = b;
	UBRR0H = b >> 8;
	//if (enable_tx)
	//	UCSR0B |= _BV(TXEN0);
	if (enable_rx)
		UCSR0B |= _BV(RXEN0) | _BV(RXCIE0);
	sei();
}

void uart_enable(uint32_t bitrate) {
	_uart_enable(bitrate, true, true);
}

void uart_enable_tx_only(uint32_t bitrate) {
	_uart_enable(bitrate, true, false);
}

void uart_enable_rx_only(uint32_t bitrate) {
	_uart_enable(bitrate, false, true);
}

void uart_disable() {
	UCSR0B = 0;
	UCSR0C = 0;
}

bool uart_tx_buffer_is_empty() {
	return (tx_buffer_head == tx_buffer_tail);
}

void uart_write(uint8_t * data, uint8_t len) {
	bool empty = (tx_buffer_head == tx_buffer_tail);
	for (int i = 0; i < len; i++) {
		uint8_t next = (tx_buffer_head < UART_TX_BUFFER_SIZE ? tx_buffer_head + 1 : 0);
		tx_buffer[tx_buffer_head] = data[i];
		tx_buffer_head = next;
	}
	if (empty)
		UCSR0B |= _BV(TXEN0) | _BV(UDRIE0);
}

void uart_print(uint8_t * data) {
	uart_write(data, strlen((char *)data));
}

#if defined (__AVR_ATmega2560__)
ISR(USART0_UDRE_vect) {
#else
ISR(USART_UDRE_vect) {
#endif
	if (tx_buffer_head != tx_buffer_tail) {
		uint8_t next = (tx_buffer_tail < UART_TX_BUFFER_SIZE ? tx_buffer_tail + 1 : 0);
		UDR0 = tx_buffer[tx_buffer_tail];
		tx_buffer_tail = next;
	} else
		UCSR0B &= ~_BV(TXEN0);
		//UCSR0B &= ~_BV(UDRIE0);
}

bool uart_data_is_available() {
	return (rx_buffer_head != rx_buffer_tail);
}

uint8_t uart_read() {
		if (rx_buffer_head == rx_buffer_tail)
			return 0;
		uint8_t next = (rx_buffer_tail < UART_RX_BUFFER_SIZE - 1 ? rx_buffer_tail + 1 : 0);
		uint8_t ch = rx_buffer[rx_buffer_tail];
		rx_buffer_tail = next;
		return ch;
}

#if defined (__AVR_ATmega2560__)
ISR(USART0_RX_vect) {
#else
ISR(USART_RX_vect) {
#endif
	uint8_t ch = UDR0;
	uint8_t next = (rx_buffer_head < UART_RX_BUFFER_SIZE - 1 ? rx_buffer_head + 1 : 0);
	if (next == rx_buffer_tail)
		return;
	rx_buffer[rx_buffer_head] = ch;
	rx_buffer_head = next;
}
