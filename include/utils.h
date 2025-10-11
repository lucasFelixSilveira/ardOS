#ifndef ARDOS_UTILS_H
#define ARDOS_UTILS_H
#define BLINK_DELAY_MS 75
#include "config.h"
#if TARGET_AVR
#include <util/delay.h>
#include <avr/io.h>
#include "pins.h"
#define F_CPU 16000000UL
#define LOG_PIN PB5

#define LED_RED 312
#define LED_GREEN 12
#define LED_BLUE 312

#define BAUD 9600
#define UBRR_VALUE ((F_CPU/16/BAUD)-1)
#define BLINK(times, port, pin) \
    for( uint8_t i = 0; i < times; i++ ) { \
        INVERT(port, pin); \
        _delay_ms(BLINK_DELAY_MS); \
    }
#define DELAY(ms) _delay_ms(ms)
#else
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#define LED_RED GPIO_NUM_15
#define LED_GREEN GPIO_NUM_2
#define LED_BLUE GPIO_NUM_4
#define PORTB 0

#define BLINK(times, gpio_pin) \
    do { \
        gpio_reset_pin(gpio_pin); \
        gpio_set_direction(gpio_pin, GPIO_MODE_OUTPUT); \
        for (int i = 0; i < (times); i++) { \
            gpio_set_level(gpio_pin, 1); \
            vTaskDelay(pdMS_TO_TICKS(BLINK_DELAY_MS)); \
            gpio_set_level(gpio_pin, 0); \
            vTaskDelay(pdMS_TO_TICKS(BLINK_DELAY_MS)); \
        } \
    } while (0)

#define DELAY(ms) vTaskDelay(pdMS_TO_TICKS(ms))
#endif

#endif
