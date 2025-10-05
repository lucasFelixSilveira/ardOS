#ifndef ARDUINO_PINS_UTILS_H
#define ARDUINO_PINS_UTILS_H
#define SETHIGH(PORT, PIN) PORT |=  (1 << PIN)
#define SETLOW(PORT, PIN)  PORT &= ~(1 << PIN)
#define INVERT(PORT, PIN)  PORT ^=  (1 << PIN)
#endif
