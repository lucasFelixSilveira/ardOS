#include <stdint.h>

#include "config.h"

#if TARGET_AVR
#include <avr/io.h>
#include <stdbool.h>
#include <util/delay.h>
#include "pins.h"
#include "main.h"
#include "utils.h"

#include "shield/log.h"

#include "serial/utils.h"
#include "serial/terminal.h"
char buffer[BUFFER_SIZE];
char temp[TEMP_SIZE];
bool serialMode;
#else
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "pins.h"
#include "main.h"
#include "utils.h"
char buffer[BUFFER_SIZE];
char temp[TEMP_SIZE];
#endif
// commands/blink.c
void blink(void);
// command_handler.c
void commands(void);
// memory_economy/locate.c
unsigned char locale(unsigned char word_index, char* temp, unsigned char temp_size);
// shield/log.c
void log_error(void);
void log_info(void);
void log_success(void);
void log_running(void);
void log_stop(void);
void log_ready(void);
void log_off(void);
void log_bip(void);
// shield/setup.c
void setup(void);
// commands/snake.c
void snake(void);
// memory_economy/strcmp.c
char ardos_strcmp(const char s1[], const char s2[]);
// serial/terminal.c
void osSerialTerminal(void);
// serial/utils.c
void uart_init(void);
unsigned char uart_data_available(void);
char uart_receive_char(void);
void uart_send_char(char c);
void uart_send_string(const char* str);
void uart_send_uint8(unsigned char n);
void uart_set_cursor(unsigned char row, unsigned char col);

/* XTENSA FORWARD DECLARATIONS -- USED ON XTENSA COMPILATION! */

Task task;
uint16_t buffer_index;

#if TARGET_AVR
int main(void) {
    buffer_index = 0;
    log_ready();

    uart_init();

    task = TASK_IDLE;
    DELAY(50);

    while(1) {
        if( (!serialMode) && uart_data_available() ) serialMode = true;
        if( uart_data_available() ) osSerialTerminal();
        else log_off();
    }
}
#elif TARGET_X86
void _start(void) {
    kernel_main();
}
#else
void app_main(void) {
    buffer_index = 0;
    task = TASK_IDLE;
    setup();
    log_ready();
    while(1) {
        if( uart_data_available() ) osSerialTerminal();
        else log_off();
    }
}
#endif
#include "config.h"
#include "utils.h"
#include "multithread.h"

#include "main.h"

#if TARGET_AVR
#include <avr/io.h>
#include <avr/io.h>
#include <util/delay.h>
#include "serial/terminal.h"
#include "serial/utils.h"
#endif

void blink(void) {
    while(task == TASK_BLINKING) {
        MULTITHREAD
        INVERT(PORTB, LED_GREEN);
        DELAY(300);
    }
}
#include "config.h"
#if TARGET_AVR
#include "shield/log.h"
#include <util/delay.h>

#include "serial/utils.h"
#include "memory_economy/strcmp.h"
#include "commands/blink.h"
#include "commands/snake.h"
#endif

#include "utils.h"
#include "main.h"

void commands(void) {
    buffer[0] = '\0';
    buffer_index = 0;

    if( ardos_strcmp(temp, "echo") == 0 ) {
        uart_send_string("echoing \"");
        uart_send_string(buffer);
        uart_send_string("\"...\r\n");
    }
    else if( ardos_strcmp(temp, "blink") == 0 ) {
        uart_send_string("Starts blinking! - Type kill to finish.\r\n");
        task = TASK_BLINKING;
        blink();
    }
    else if( ardos_strcmp(temp, "kill") == 0 && task != TASK_IDLE ) task = TASK_IDLE;
    else if( ardos_strcmp(temp, "clear") == 0 ) uart_send_string("\033[2J\033[H");
    else if( ardos_strcmp(temp, "snake") == 0 ) snake();
    else if( ardos_strcmp(temp, "help") == 0 ) {
        uart_send_string("Available commands:\r\n");
        uart_send_string("  echo    - Echo message\r\n");
        uart_send_string("  blink   - Start blinking LED\r\n");
        uart_send_string("  kill    - Stop current task\r\n");
        uart_send_string("  clear   - Clear screen\r\n");
        uart_send_string("  snake   - Snake game\r\n");
        uart_send_string("  help    - Show this help\r\n");
        // `
    }
    // else if( ardos_strcmp(temp, "sd-init") == 0 ) sd_init_command();
    // else if( ardos_strcmp(temp, "sd-read") == 0 ) sd_read_command();
    // else if( ardos_strcmp(temp, "sd-write") == 0 ) sd_write_command();
    // else if( ardos_strcmp(temp, "sd-status") == 0 ) sd_status_command();
    // else if( ardos_strcmp(temp, "sd-dump") == 0 ) sd_dump_command();
    // else if( ardos_strcmp(temp, "sd-format") == 0 ) sd_format_command();
    // else if( ardos_strcmp(temp, "sd-test") == 0 ) sd_test_detection_command();
    // else if( ardos_strcmp(temp, "sd-redetect") == 0 ) sd_redetect_command();
    else {
        uart_send_string("current@super % cleaned buffer - ERROR: command `");
        uart_send_string(temp);
        uart_send_string("` not found\r\n");
        log_info();
    }
}
#include <stdint.h>
#include "main.h"

uint8_t locale(uint8_t word_index, char* temp, uint8_t temp_size) {
    uint8_t w = 0, ti = 0;
    for( uint8_t i = 0; i < buffer_index; i++ ) {
        if( buffer[i] != ' ' && ti < temp_size - 1 && w == word_index) temp[ti++] = buffer[i];
        else if( buffer[i] == ' ' ) {
            if( w == word_index ) break;
            w++;
            ti = 0;
        }
    }

    if( w != word_index ) { temp[0] = '\0'; return 0; }
    temp[ti] = '\0';
    return 1;
}
#include "utils.h"
#include "pins.h"
#include "config.h"
#if TARGET_AVR
#include <avr/io.h>
#include <avr/io.h>
#endif

void log_error(void) {
    do {
        for( int i = 0; i < 2; i++ ) {
            SETHIGH(PORTD, LED_RED);
            DELAY(300);
            SETLOW(PORTD, LED_RED);
            DELAY(100);
        }
    } while(0);
}

void log_info(void) {
    do {
        SETHIGH(PORTD, LED_GREEN);
        SETHIGH(PORTD, LED_RED);
        DELAY(300);
        SETLOW(PORTD, LED_GREEN);
        SETLOW(PORTD, LED_RED);
        DELAY(100);
    } while(0);
}

void log_success(void) {
    do {
        for( int i = 0; i < 5; i++ ) {
            SETHIGH(PORTD, LED_GREEN);
            DELAY(20);
            SETLOW(PORTD, LED_GREEN);
            DELAY(201);
        }
    } while(0);
}

void log_running(void) {
    SETHIGH(PORTD, LED_BLUE);
}

void log_stop(void) {
    do {
        SETLOW(PORTD, LED_BLUE);
        DELAY(200);
        SETHIGH(PORTD, LED_GREEN);
        DELAY(100);
        SETLOW(PORTD, LED_GREEN);
    } while(0);
}

void log_ready(void) {
    do {
        SETHIGH(PORTD, LED_GREEN);
        SETHIGH(PORTD, LED_BLUE);
        SETHIGH(PORTD, LED_RED);
        DELAY(300);
        SETLOW(PORTD, LED_GREEN);
        SETLOW(PORTD, LED_BLUE);
        SETLOW(PORTD, LED_RED);
        DELAY(100);
    } while(0);
}

void log_off(void) {
    do {
        SETLOW(PORTD, LED_GREEN);
        SETLOW(PORTD, LED_BLUE);
        SETLOW(PORTD, LED_RED);
    } while(0);
}

void log_bip(void) {
    do {
        for( int i = 0; i < 5; i++ ) {
            SETHIGH(PORTD, LED_BLUE);
            DELAY(20);
            SETLOW(PORTD, LED_BLUE);
        }
    } while(0);
}
#include "config.h"
#if TARGET_AVR
#include <avr/io.h>
#include "pins.h"
#include "utils.h"

void setup(void) {
    SETOUTPUT(LED_GREEN);
    SETOUTPUT(LED_RED);
    SETOUTPUT(LED_BLUE);
}
#else
void setup(void) {
    gpio_reset_pin(LED_RED);
    gpio_reset_pin(LED_GREEN);
    gpio_reset_pin(LED_BLUE);

    gpio_set_direction(LED_RED, GPIO_MODE_OUTPUT);
    gpio_set_direction(LED_GREEN, GPIO_MODE_OUTPUT);
    gpio_set_direction(LED_BLUE, GPIO_MODE_OUTPUT);

    gpio_reset_pin(EXTRA_VCC);
    gpio_set_direction(EXTRA_VCC, GPIO_MODE_OUTPUT);
    gpio_set_level(EXTRA_VCC, 1);

    uart_init();
}
#endif
#include "config.h"
#if TARGET_AVR
#include <stdint.h>
#include <util/delay.h>
#include "serial/utils.h"
#include "main.h"
#include "utils.h"
#endif

void snake(void) {
    const uint8_t WIDTH = 16;
    const uint8_t HEIGHT = 8;
    uint8_t head = 0;
    uint8_t length = 3;
    uint8_t body[16];
    uint8_t apple = 27;
    char dir = 'D';

    for( uint8_t i = 0; i < length; i++ )
        body[i] = head - i;

    uart_send_string("\033[2J\033[H");
    for( uint8_t y = 0; y < HEIGHT; y++ ) {
        for( uint8_t x = 0; x < WIDTH; x++ ) {
            uart_send_char('.');
        }
        uart_send_string("\r\n");
    }

    while(length < 16) {
        if( uart_data_available() ) {
            char c = uart_receive_char();
            if( c == 'W' || c == 'w' ) dir = 'W';
            else if( c == 'A' || c == 'a' ) dir = 'A';
            else if( c == 'S' || c == 's' ) dir = 'S';
            else if( c == 'D' || c == 'd' ) dir = 'D';
            else if( c == 'K' || c == 'k' ) {
                task = TASK_IDLE;
                break;
            }
        }

        uint8_t x = head % WIDTH;
        uint8_t y = head / WIDTH;

        switch (dir) {
            case 'W': y = (y == 0) ? HEIGHT - 1 : y - 1; break;
            case 'S': y = (y + 1) % HEIGHT; break;
            case 'A': x = (x == 0) ? WIDTH - 1 : x - 1; break;
            case 'D': x = (x + 1) % WIDTH; break;
        }

        uint8_t new_head = y * WIDTH + x;

        uint8_t tail = body[length - 1];
        uart_set_cursor((tail / WIDTH) + 1, (tail % WIDTH) + 1);
        uart_send_char('.');

        for( int i = length - 1; i > 0; i-- )
            body[i] = body[i - 1];
        body[0] = new_head;
        head = new_head;

        uart_set_cursor((head / WIDTH) + 1, (head % WIDTH) + 1);
        uart_send_char('0');

        if( length > 1 ) {
            uint8_t neck = body[1];
            uart_set_cursor((neck / WIDTH) + 1, (neck % WIDTH) + 1);
            uart_send_char('o');
        }

        uint8_t ax = apple % WIDTH;
        uint8_t ay = apple / WIDTH;
        uart_set_cursor(ay + 1, ax + 1);
        uart_send_char('@');

        uart_set_cursor(HEIGHT + 1, 1);

        if( head == apple && length < 16 ) {
            body[length] = body[length - 1];
            length++;
            apple = (apple + 13) % (WIDTH * HEIGHT);
        }

        DELAY(120);
    }

    uart_set_cursor(HEIGHT + 1, 1);
    uart_send_string("Fim do jogo!\r\n");
}
#include <stdint.h>

char ardos_strcmp(const char s1[], const char s2[]) {
    while(*s1 && (*s1 == *s2)) s1++, s2++;
    return (int8_t)(*s1 - *s2);
}
#include "config.h"
#if TARGET_AVR
#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h>
#include "shield/log.h"

#include "serial/utils.h"
#include "utils.h"
#include "memory_economy/locale.h"
#include "pins.h"

#include "main.h"
#include "command_handler.h"
#endif

void osSerialTerminal(void) {
    char c = uart_receive_char();

    if( c == 13 || c == 10 ) {
        uart_send_string("\r\n");

        if(! locale(0, temp, sizeof(temp)) ) {
            uart_send_string("current@super % cleaned buffer - ERROR: command not found\r\n");
            buffer[0] = '\0';
            buffer_index = 0;
            return;
        }

        uint8_t len = 0;
        while( temp[len] ) len++;
        if( len > 0 && (temp[len-1] == '\r' || temp[len-1] == '\n') ) temp[len-1] = '\0';

        log_running();
        uart_send_string("current@super % running ");
        uart_send_string(temp);
        uart_send_string("...\r\n");
        commands();
        log_stop();
        return;
    }

    if( c == ':' ) {
        uart_send_string("current@super % cleaned buffer\r\n");
        buffer_index = 0;
        return;
    }

    if( buffer_index < BUFFER_SIZE - 1 ) {
        buffer[buffer_index++] = c;
        buffer[buffer_index] = '\0';
    }

    uart_send_string("\r");
    uart_send_string("current@user % ");
    uart_send_string(buffer);
    log_bip();
}
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
