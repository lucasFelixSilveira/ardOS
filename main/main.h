#ifndef ARDOS_MAIN_H
#define ARDOS_MAIN_H
#include "config.h"

typedef enum {
    TASK_BLINKING,
    TASK_IDLE
} Task;

#if TARGET_AVR
#include <stdbool.h>
#include <stdint.h>
#define BUFFER_SIZE 64
#define TEMP_SIZE 16

extern char buffer[BUFFER_SIZE];
extern uint8_t buffer_index;
extern Task task;
extern char temp[TEMP_SIZE];
extern bool serialMode;
#else
#include <stdbool.h>
#include <stdint.h>
#define BUFFER_SIZE 256
#define TEMP_SIZE 64

extern char buffer[BUFFER_SIZE];
extern uint8_t buffer_index;
extern Task task;
extern char temp[TEMP_SIZE];
#endif

#endif
