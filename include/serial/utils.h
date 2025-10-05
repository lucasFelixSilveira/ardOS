#ifndef ARDOS_SERIAL_UTILS_H
#define ARDOS_SERIAL_UTILS_H
#include <stdint.h>
void uart_init(void);
uint8_t uart_data_available(void);
char uart_receive_char(void);
void uart_send_char(char c);
void uart_send_string(const char* str);
void uart_send_uint8(uint8_t n);
void uart_set_cursor(uint8_t row, uint8_t col);
#endif
