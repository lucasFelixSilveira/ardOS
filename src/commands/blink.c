#include "multithread.h"
#include "main.h"
#include <avr/io.h>
#include <avr/io.h>
#include <util/delay.h>
#include "serial/terminal.h"
#include "serial/utils.h"
#include "utils.h"

void blink(void) {
    while(task == TASK_BLINKING) {
        MULTITHREAD
        PORTB ^= (1 << LOG_PIN);
        _delay_ms(300);
    }
}
