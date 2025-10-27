#include "config.h"
#if TARGET_AVR
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
#else

#include "utils.h"
#include "driver/uart.h"
#include <string.h>

#define UART_PORT_NUM      UART_NUM_0
#define UART_BAUD_RATE     115200
#define UART_BUF_SIZE      1024

void uart_init(void) {
    uart_config_t uart_config = {
        .baud_rate = UART_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    uart_driver_install(UART_PORT_NUM, UART_BUF_SIZE * 2, 0, 0, NULL, 0);
    uart_param_config(UART_PORT_NUM, &uart_config);
    uart_set_pin(UART_PORT_NUM, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
}

uint8_t uart_data_available(void) {
    size_t len = 0;
    uart_get_buffered_data_len(UART_PORT_NUM, &len);
    return len > 0;
}

char uart_receive_char(void) {
    uint8_t data;
    int len = uart_read_bytes(UART_PORT_NUM, &data, 1, portMAX_DELAY);
    if( len > 0 ) {
        return (char)data;
    }
    return 0; // ou algum erro
}

void uart_send_char(char c) {
    uart_write_bytes(UART_PORT_NUM, &c, 1);
}

void uart_send_string(const char* str) {
    uart_write_bytes(UART_PORT_NUM, str, strlen(str));
}

void uart_send_uint8(uint8_t n) {
    char buf[4] = {0};
    if (n >= 100) {
        buf[0] = '0' + (n / 100) % 10;
        buf[1] = '0' + (n / 10) % 10;
        buf[2] = '0' + n % 10;
        buf[3] = '\0';
    } else if (n >= 10) {
        buf[0] = '0' + (n / 10) % 10;
        buf[1] = '0' + n % 10;
        buf[2] = '\0';
    } else {
        buf[0] = '0' + n;
        buf[1] = '\0';
    }
    uart_send_string(buf);
}

void uart_set_cursor(uint8_t row, uint8_t col) {
    char esc_seq[16];
    snprintf(esc_seq, sizeof(esc_seq), "\x1b[%d;%dH", row, col);
    uart_send_string(esc_seq);
}
#endif
