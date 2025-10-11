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
char buffer[256];
char temp[64];
#endif
// shield/log.c
void log_error(void);
void log_info(void);
void log_success(void);
void log_running(void);
void log_stop(void);
void log_ready(void);
// shield/setup.c
void setup(void);

/* XTENSA FORWARD DECLARATIONS */

Task task;
uint8_t buffer_index;

#if TARGET_AVR
int main(void) {
    buffer_index = 0;
    SETOUTPUT(DDRB, LOG_PIN);
    SETHIGH(PORTB, LOG_PIN);

    uart_init();

    task = TASK_IDLE;

    _delay_ms(50);

    while(1) {
        if( (!serialMode) && uart_data_available() ) serialMode = true;
        if( uart_data_available() ) osSerialTerminal();
        else SETLOW(PORTB, LOG_PIN);
    }
}
#else
void app_main(void) {
    buffer_index = 0;
    task = TASK_IDLE;
    setup();
    while(1) {
        log_error();
        DELAY(2000);
        log_info();
        DELAY(2000);
        log_success();
        DELAY(2000);
        log_running();
        DELAY(2000);
        log_stop();
        DELAY(2000);
    }
}
#endif

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
            SETHIGH(PORTB, LED_RED);
            DELAY(300);
            SETLOW(PORTB, LED_RED);
            DELAY(100);
        }
    } while(0);
}

void log_info(void) {
    do {
        SETHIGH(PORTB, LED_GREEN);
        SETHIGH(PORTB, LED_RED);
        DELAY(300);
        SETLOW(PORTB, LED_GREEN);
        SETLOW(PORTB, LED_RED);
        DELAY(100);
    } while(0);
}

void log_success(void) {
    do {
        for( int i = 0; i < 5; i++ ) {
            SETHIGH(PORTB, LED_GREEN);
            DELAY(20);
            SETLOW(PORTB, LED_GREEN);
            DELAY(201);
        }
    } while(0);
}

void log_running(void) {
    SETHIGH(PORTB, LED_BLUE);
}

void log_stop(void) {
    do {
        SETLOW(PORTB, LED_BLUE);
        DELAY(200);
        SETHIGH(PORTB, LED_GREEN);
        DELAY(100);
        SETLOW(PORTB, LED_GREEN);
    } while(0);
}

void log_ready(void) {
    do {
        SETHIGH(PORTB, LED_GREEN);
        SETHIGH(PORTB, LED_BLUE);
        SETHIGH(PORTB, LED_RED);
        DELAY(300);
        SETLOW(PORTB, LED_GREEN);
        SETLOW(PORTB, LED_BLUE);
        SETLOW(PORTB, LED_RED);
        DELAY(100);
    } while(0);
}
#include "config.h"
#if TARGET_AVR
void setup();
#else
void setup(void) {
    gpio_reset_pin(LED_RED);
    gpio_reset_pin(LED_GREEN);
    gpio_reset_pin(LED_BLUE);
    gpio_set_direction(LED_RED, GPIO_MODE_OUTPUT);
    gpio_set_direction(LED_GREEN, GPIO_MODE_OUTPUT);
    gpio_set_direction(LED_BLUE, GPIO_MODE_OUTPUT);
}
#endif
