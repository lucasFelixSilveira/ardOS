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

void log_off(void) {
    do {
        SETLOW(PORTB, LED_GREEN);
        SETLOW(PORTB, LED_BLUE);
        SETLOW(PORTB, LED_RED);
    } while(0);
}

void log_bip(void) {
    do {
        for( int i = 0; i < 5; i++ ) {
            SETHIGH(PORTB, LED_BLUE);
            DELAY(20);
            SETLOW(PORTB, LED_BLUE);
        }
    } while(0);
}
