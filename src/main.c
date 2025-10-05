#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h>

#include "pins.h"
#include "main.h"
#include "utils.h"

#include "serial/utils.h"
#include "serial/terminal.h"

extern char buffer[BUFFER_SIZE];
extern uint8_t buffer_index;
extern Task task;
extern char temp[TEMP_SIZE];

int main(void) {
    buffer_index = 0;
    DDRB |= (1 << LOG_PIN);
    uart_init();
    SETHIGH(PORTB, LOG_PIN);

    task = TASK_IDLE;

    while(1) {
        if( uart_data_available() ) osSerialTerminal();
        else SETLOW(PORTB, LOG_PIN);
    }
}
