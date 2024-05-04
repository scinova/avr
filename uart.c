#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#include <stdbool.h>
#include "uart.h"

static uint8_t tx_buffer[UART_TX_BUFFER_SIZE + 1];
static volatile uint8_t tx_buffer_head;
static volatile uint8_t tx_buffer_tail;

static uart_handler_t _handler;

void _uart_enable(uint32_t bitrate, bool enable_rx) {
	// SEL0=0 UPM0=0 USBS0=0 UCSZ0=3 async, no parity, 1 stop bit, 8 bit size
	UCSR0C |= _BV(UCSZ00) | _BV(UCSZ01);
	uint32_t b = (F_CPU / (bitrate * 16UL)) - 1;
	UBRR0L = (uint8_t)(b & 0xff);
	UBRR0H = (uint8_t)(b >> 8);
	if (enable_rx)
		UCSR0B |= _BV(RXEN0) | _BV(RXCIE0);
	sei();
}

void uart_enable(uint32_t bitrate, uart_handler_t handler) {
	_handler = handler;
	_uart_enable(bitrate, true);
}

void uart_enable_tx_only(uint32_t bitrate) {
	_uart_enable(bitrate, false);
}

void uart_enable_rx_only(uint32_t bitrate, uart_handler_t handler) {
	_handler = handler;
	_uart_enable(bitrate, true);
}

void uart_disable() {
	UCSR0B = 0;
	UCSR0C = 0;
	_handler = NULL;
}

bool uart_tx_buffer_is_empty() {
	return (tx_buffer_head == tx_buffer_tail);
}

void uart_write(uint8_t * data, uint8_t len) {
	cli();
	bool empty = (tx_buffer_head == tx_buffer_tail);
	for (int i = 0; i < len; i++) {
		tx_buffer[tx_buffer_head] = data[i];
		tx_buffer_head = (tx_buffer_head < UART_TX_BUFFER_SIZE ? tx_buffer_head + 1 : 0);
	}
	if (empty)
		UCSR0B |= _BV(TXEN0) | _BV(UDRIE0);
	sei();
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
}

#if defined (__AVR_ATmega2560__)
ISR(USART0_RX_vect) {
#else
ISR(USART_RX_vect) {
#endif
	uint8_t ch = UDR0;
	if (_handler)
		_handler(ch);
}
