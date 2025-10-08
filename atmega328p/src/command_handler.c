#include <util/delay.h>

#include "main.h"
#include "serial/utils.h"
#include "memory_economy/strcmp.h"
#include "commands/blink.h"
#include "commands/snake.h"
#include "commands/sd_commands.h"

#include "utils.h"

void commands(void) {
    buffer[0] = '\0';
    buffer_index = 0;

    if( ardos_strcmp(temp, "echo") == 0 ) {
        uart_send_string("echoing \"");
        uart_send_string(buffer);
        uart_send_string("\"...\r\n");
    }
    else if( ardos_strcmp(temp, "blink") == 0 ) {
        uart_send_string("Starts blinking! - Type kill to finish.\r\n");
        task = TASK_BLINKING;
        blink();
    }
    else if( ardos_strcmp(temp, "kill") == 0 && task != TASK_IDLE ) task = TASK_IDLE;
    else if( ardos_strcmp(temp, "clear") == 0 ) uart_send_string("\033[2J\033[H");
    else if( ardos_strcmp(temp, "snake") == 0 ) snake();
    else if( ardos_strcmp(temp, "help") == 0 ) {
        uart_send_string("Available commands:\r\n");
        uart_send_string("  echo    - Echo message\r\n");
        uart_send_string("  blink   - Start blinking LED\r\n");
        uart_send_string("  kill    - Stop current task\r\n");
        uart_send_string("  clear   - Clear screen\r\n");
        uart_send_string("  snake   - Snake game\r\n");
        uart_send_string("  help    - Show this help\r\n");
        if( hasShield ) {
            uart_send_string("\r\nSD Card commands (Shield detected):\r\n");
            uart_send_string("  sd-init   - Initialize SD card\r\n");
            uart_send_string("  sd-read   - Read sector 0 (512 bytes)\r\n");
            uart_send_string("  sd-write  - Write test data to sector 1000\r\n");
            uart_send_string("  sd-status - Show SD shield status\r\n");
            uart_send_string("  sd-dump   - Dump first 3 sectors\r\n");
            uart_send_string("  sd-format - Format test sector 1000\r\n");
            uart_send_string("  sd-test   - Test shield detection\r\n");
            uart_send_string("  sd-redetect - Re-detect shield\r\n");
        } else {
            uart_send_string("\r\nSD Shield: NOT DETECTED\r\n");
        }
    }
    else if( ardos_strcmp(temp, "sd-init") == 0 ) sd_init_command();
    else if( ardos_strcmp(temp, "sd-read") == 0 ) sd_read_command();
    else if( ardos_strcmp(temp, "sd-write") == 0 ) sd_write_command();
    else if( ardos_strcmp(temp, "sd-status") == 0 ) sd_status_command();
    else if( ardos_strcmp(temp, "sd-dump") == 0 ) sd_dump_command();
    else if( ardos_strcmp(temp, "sd-format") == 0 ) sd_format_command();
    else if( ardos_strcmp(temp, "sd-test") == 0 ) sd_test_detection_command();
    else if( ardos_strcmp(temp, "sd-redetect") == 0 ) sd_redetect_command();
    else {
        uart_send_string("current@super % cleaned buffer - ERROR: command `");
        uart_send_string(temp);
        uart_send_string("` not found\r\n");

        BLINK(5, PORTB, LOG_PIN);
    }
}
