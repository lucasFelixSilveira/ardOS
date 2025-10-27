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
