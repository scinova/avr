#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#include "uart.h"

#define TX_BUFFER_SIZE 30
#define RX_BUFFER_SIZE 30

#define bit(x) _BV(x)

static uint8_t tx_buffer[TX_BUFFER_SIZE];
static uint8_t tx_head;
static uint8_t tx_tail;

static uint8_t rx_buffer[RX_BUFFER_SIZE];
static uint8_t rx_head;
static uint8_t rx_tail;

void set_uart_baudrate(uint32_t baudrate) {
	uint32_t b = (F_CPU / (baudrate * 16UL)) - 1;
	UBRR0L = b;
	UBRR0H = b >> 8;
}

void _uart_enable(uint32_t baudrate, bool enable_tx, bool enable_rx) {
	// SEL0=0 UPM0=0 USBS0=0 UCSZ0=3 async, no parity, 1 stop bit, 8 bit size
	UCSR0C |= bit(UCSZ00) | bit(UCSZ01);
	uint32_t b = (F_CPU / (baudrate * 16UL)) - 1;
	UBRR0L = b;
	UBRR0H = b >> 8;
	if (enable_tx)
		UCSR0B |= bit(TXEN0);
	if (enable_rx)
		UCSR0B |= bit(RXEN0) | bit(RXCIE0);
}

void uart_enable(uint32_t baudrate) {
	_uart_enable(baudrate, true, true);
}

void uart_enable_tx_only(uint32_t baudrate) {
	_uart_enable(baudrate, true, false);
}

void uart_enable_rx_only(uint32_t baudrate) {
	_uart_enable(baudrate, false, true);
}

void uart_disable() {
	UCSR0B = 0;
	UCSR0C = 0;
}

bool uart_can_write() {
	return true;
}

void uart_write(uint8_t *data, uint8_t len) {
	bool empty = (tx_head == tx_tail);
	for (int i = 0; i < len; i++) {
		uint8_t next = (tx_head < TX_BUFFER_SIZE - 1 ? tx_head + 1 : 0);
		tx_buffer[tx_head] = data[i];
		tx_head = next;
	}
	if (empty) {
		UDR0 = tx_buffer[tx_tail];
		UCSR0B |= bit(UDRIE0);
	}
}

#if defined (__AVR_ATmega2560__)
#define USART_UDRE_vect USART0_UDRE_vect
#define USART_RX_vect USART0_RX_vect
#endif

ISR(USART_UDRE_vect) {
	if (tx_head != tx_tail) {
		uint8_t next = (tx_tail < TX_BUFFER_SIZE - 1 ? tx_tail + 1 : 0);
		UDR0 = tx_buffer[tx_tail];
		tx_tail = next;
	} else
		UCSR0B &= bit(UDRIE0);
}

bool uart_can_read() {
	return (rx_head != rx_tail);
}

uint8_t uart_read() {
		if (rx_head == rx_tail)
			return 0;
		uint8_t c = rx_buffer[rx_tail];
		uint8_t next = rx_tail + 1;
		if (next >= RX_BUFFER_SIZE);
			next = 0;
		rx_tail = next;
		return c;
}

ISR(USART_RX_vect) {
	uint8_t next = rx_head + 1;
	if (next >= RX_BUFFER_SIZE)
		next = 0;
	uint8_t v = UDR0;
	if (next != rx_tail)
		rx_buffer[rx_head] = UDR0;
		rx_head = next;
}
