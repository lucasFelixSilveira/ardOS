#include "config.h"
#if TARGET_AVR
#include <avr/io.h>
#include "pins.h"
#include "utils.h"

void setup(void) {
    SETOUTPUT(LED_GREEN);
    SETOUTPUT(LED_RED);
    SETOUTPUT(LED_BLUE);
}
#else
void setup(void) {
    gpio_reset_pin(LED_RED);
    gpio_reset_pin(LED_GREEN);
    gpio_reset_pin(LED_BLUE);

    gpio_set_direction(LED_RED, GPIO_MODE_OUTPUT);
    gpio_set_direction(LED_GREEN, GPIO_MODE_OUTPUT);
    gpio_set_direction(LED_BLUE, GPIO_MODE_OUTPUT);

    gpio_reset_pin(EXTRA_VCC);
    gpio_set_direction(EXTRA_VCC, GPIO_MODE_OUTPUT);
    gpio_set_level(EXTRA_VCC, 1);

    uart_init();
}
#endif
