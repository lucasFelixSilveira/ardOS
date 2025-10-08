#ifndef ARDUINO_MAIN_H
#define ARDUINO_MAIN_H
#include <stdbool.h>
#include <stdint.h>
#define BUFFER_SIZE 64
#define TEMP_SIZE 16
typedef enum {
    TASK_BLINKING,
    TASK_IDLE
} Task;

extern char buffer[BUFFER_SIZE];
extern uint8_t buffer_index;
extern Task task;
extern char temp[TEMP_SIZE];
extern bool serialMode;
extern char *chunk;
extern bool hasShield;

// ADC functions for analog shield detection
void adc_init(void);
uint16_t read_adc_a5(void);
#endif
