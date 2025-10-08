#ifndef ARDOS_UTILS_H
#define ARDOS_UTILS_H
#include <util/delay.h>
#include <avr/io.h>
#include "pins.h"
#define F_CPU 16000000UL
#define LOG_PIN PB5
#define SHIELD_CHECK_PIN PC5
#define BAUD 9600
#define UBRR_VALUE ((F_CPU/16/BAUD)-1)
#define BLINK(times, port, pin) \
    for( uint8_t i = 0; i < times; i++ ) { \
        INVERT(port, pin); \
        _delay_ms(75); \
    }

#endif
