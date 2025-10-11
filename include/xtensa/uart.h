// serial/utils.c
void uart_init(void);
unsigned char uart_data_available(void);
char uart_receive_char(void);
void uart_send_char(char c);
void uart_send_string(const char* str);
void uart_send_uint8(unsigned char n);
void uart_set_cursor(unsigned char row, unsigned char col);
