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
