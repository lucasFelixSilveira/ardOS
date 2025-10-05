#include <util/delay.h>

#include "main.h"
#include "serial/utils.h"
#include "memory_economy/strcmp.h"
#include "commands/blink.h"
#include "commands/snake.h"

#include "utils.h"

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
    else {
        uart_send_string("current@super % cleaned buffer - ERROR: command `");
        uart_send_string(temp);
        uart_send_string("` not found\r\n");

        BLINK(5, PORTB, LOG_PIN);
    }
}
