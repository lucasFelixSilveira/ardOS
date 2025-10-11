#ifndef ARDUINO_PINS_UTILS_H
#define ARDUINO_PINS_UTILS_H
#if TARGET_AVR
#define SETHIGH(PORT, PIN) PORT |=  (1 << PIN)
#define SETLOW(PORT, PIN)  PORT &= ~(1 << PIN)
#define INVERT(PORT, PIN)  PORT ^=  (1 << PIN)

#define SETOUTPUT(PORT, PIN) SETHIGH(PORT, PIN)
#else
#include "driver/gpio.h"
#define SETHIGH(PORT, PIN) gpio_set_level(PIN, 1)
#define SETLOW(PORT, PIN)  gpio_set_level(PIN, 0)
#define INVERT(PORT, PIN)  gpio_set_level(PIN, !(gpio_get_level(PIN)))

#define SETOUTPUT(PORT, PIN) SETHIGH(PORT, PIN)
#endif
#endif
