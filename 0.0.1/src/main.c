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
