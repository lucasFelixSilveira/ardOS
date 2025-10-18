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
#define MAX_THREADS 4

extern char buffer[BUFFER_SIZE];
extern uint16_t buffer_index;
extern Task task;
extern char temp[TEMP_SIZE];
extern bool serialMode;
#else
#include <stdbool.h>
#include <stdint.h>
#define BUFFER_SIZE 512
#define TEMP_SIZE 128
#define MAX_THREADS 16

extern char buffer[BUFFER_SIZE];
extern uint16_t buffer_index;
extern Task task;
extern char temp[TEMP_SIZE];
#endif

#endif
