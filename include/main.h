#ifndef ARDUINO_MAIN_H
#define ARDUINO_MAIN_H
#define BUFFER_SIZE 64
#define TEMP_SIZE 16
#include <stdint.h>
typedef enum {
    TASK_BLINKING,
    TASK_IDLE
} Task;

extern char buffer[BUFFER_SIZE];
extern uint8_t buffer_index = 0;
extern Task task;
extern char temp[TEMP_SIZE];
#endif
