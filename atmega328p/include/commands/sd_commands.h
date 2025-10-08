#ifndef ARDOS_SD_COMMANDS_H
#define ARDOS_SD_COMMANDS_H

#include <stdint.h>

// Commands available only when hasShield == true
void sd_init_command(void);
void sd_read_command(void);
void sd_write_command(void);
void sd_status_command(void);
void sd_format_command(void);
void sd_dump_command(void);
void sd_test_detection_command(void);
void sd_redetect_command(void);

// Helper functions
void sd_print_error(uint8_t error_code);
void sd_print_hex_data(uint8_t *data, uint16_t length);
uint32_t sd_parse_sector_number(char *str);

#endif