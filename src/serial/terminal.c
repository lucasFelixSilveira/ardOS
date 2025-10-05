#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h>

#include "serial/utils.h"
#include "memory_economy/locate.h"
#include "utils.h"
#include "pins.h"

#include "main.h"
#include "command_handler.h"

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
        while(temp[len]) len++;
        if(len > 0 && (temp[len-1] == '\r' || temp[len-1] == '\n')) temp[len-1] = '\0';

        SETHIGH(PORTB, LOG_PIN);
        uart_send_string("current@super % running ");
        uart_send_string(temp);
        uart_send_string("...\r\n");
        commands();
        SETLOW(PORTB, LOG_PIN);
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
    INVERT(PORTB, LOG_PIN);
}
