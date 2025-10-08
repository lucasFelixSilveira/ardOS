#include "utils.h"
#include <avr/io.h>
#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h>

void uart_init(void) {
    UBRR0H = (uint8_t)(UBRR_VALUE >> 8);
    UBRR0L = (uint8_t)UBRR_VALUE;
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

uint8_t uart_data_available(void) {
    return (UCSR0A & (1 << RXC0)) != 0;
}

char uart_receive_char(void) {
    while(!(UCSR0A & (1 << RXC0)));
    return UDR0;
}

void uart_send_char(char c) {
    while(!(UCSR0A & (1 << UDRE0)));
    UDR0 = c;
}

void uart_send_string(const char* str) {
    while (*str) uart_send_char(*str++);
}

void uart_send_uint8(uint8_t n) {
    if( n >= 100 ) uart_send_char('0' + (n / 100) % 10);
    if( n >= 10 ) uart_send_char('0' + (n / 10) % 10);
    uart_send_char('0' + n % 10);
}

void uart_set_cursor(uint8_t row, uint8_t col) {
    uart_send_char(27);
    uart_send_char('[');
    uart_send_uint8(row);
    uart_send_char(';');
    uart_send_uint8(col);
    uart_send_char('H');
}
