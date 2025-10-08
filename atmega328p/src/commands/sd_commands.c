#include <avr/io.h>
#include <util/delay.h>
#include <string.h>

#include "main.h"
#include "shield/sd.h"
#include "serial/utils.h"
#include "commands/sd_commands.h"
#include "utils.h"

void sd_init_command(void) {
    if (!hasShield) {
        uart_send_string("No shield\r\n");
        return;
    }

    uart_send_string("Init SD...\r\n");
    
    uint8_t result = SD_init();
    
    if (result == SD_SUCCESS) {
        uart_send_string("SD init OK\r\n");
    } else {
        uart_send_string("SD init ERR ");
        uart_send_char('0' + result);
        uart_send_string("\r\n");
    }
}

void sd_read_command(void) {
    if (!hasShield) {
        uart_send_string("No shield\r\n");
        return;
    }

    if (chunk == NULL) {
        uart_send_string("No buffer\r\n");
        return;
    }

    uart_send_string("Reading sector 0...\r\n");
    uint8_t result = SD_read_sector(0, (uint8_t*)chunk);
    
    if (result == SD_SUCCESS) {
        uart_send_string("Read OK\r\n");
        sd_print_hex_data((uint8_t*)chunk, 128);
    } else {
        uart_send_string("Read ERR ");
        uart_send_char('0' + result);
        uart_send_string("\r\n");
    }
}

void sd_write_command(void) {
    if (!hasShield) {
        uart_send_string("No shield\r\n");
        return;
    }

    if (chunk == NULL) {
        uart_send_string("No buffer\r\n");
        return;
    }

    uart_send_string("Writing sector 1000...\r\n");
    
    for (uint16_t i = 0; i < 512; i++) {
        ((uint8_t*)chunk)[i] = (uint8_t)(i % 256);
    }
    
    const char* sig = "ardOS_SD_TEST";
    for (uint8_t i = 0; i < 13; i++) {
        ((uint8_t*)chunk)[i] = sig[i];
    }
    
    uint8_t result = SD_write_sector(1000, (uint8_t*)chunk);
    
    if (result == SD_SUCCESS) {
        uart_send_string("Write OK\r\n");
        
        uart_send_string("Verifying...\r\n");
        memset(chunk, 0, 512);
        
        result = SD_read_sector(1000, (uint8_t*)chunk);
        if (result == SD_SUCCESS) {
            uart_send_string("Verify OK\r\n");
            sd_print_hex_data((uint8_t*)chunk, 64);
        } else {
            uart_send_string("Verify ERR\r\n");
        }
    } else {
        uart_send_string("Write ERR ");
        uart_send_char('0' + result);
        uart_send_string("\r\n");
    }
}

void sd_status_command(void) {
    uart_send_string("Shield: ");
    uart_send_string(hasShield ? "YES" : "NO");
    uart_send_string("\r\n");
    uart_send_string("Buffer: ");
    if (chunk != NULL) {
        uart_send_string("OK (512B)\r\n");
    } else {
        uart_send_string("NO\r\n");
    }
}

void sd_dump_command(void) {
    if (!hasShield) {
        uart_send_string("No shield\r\n");
        return;
    }

    if (chunk == NULL) {
        uart_send_string("No buffer\r\n");
        return;
    }

    uart_send_string("Dump:\r\n");
    
    for (uint32_t sector = 0; sector < 3; sector++) {
        uart_send_char('0' + sector);
        uart_send_string(":\r\n");
        
        uint8_t result = SD_read_sector(sector, (uint8_t*)chunk);
        if (result == SD_SUCCESS) {
            sd_print_hex_data((uint8_t*)chunk, 64);
        } else {
            uart_send_string("ERR ");
            uart_send_char('0' + result);
            uart_send_string("\r\n");
        }
        _delay_ms(50);
    }
}

void sd_format_command(void) {
    if (!hasShield) {
        uart_send_string("No shield\r\n");
        return;
    }

    if (chunk == NULL) {
        uart_send_string("No buffer\r\n");
        return;
    }

    uart_send_string("Formatting sector 1000...\r\n");
    
    for (uint16_t i = 0; i < 512; i++) {
        ((uint8_t*)chunk)[i] = (uint8_t)(i % 256);
    }
    
    const char* format_sig = "ardOS_FORMAT";
    for (uint8_t i = 0; i < 12; i++) {
        ((uint8_t*)chunk)[i] = format_sig[i];
    }
    
    uint8_t result = SD_write_sector(1000, (uint8_t*)chunk);
    if (result == SD_SUCCESS) {
        uart_send_string("Format OK\r\n");
    } else {
        uart_send_string("Format ERR ");
        uart_send_char('0' + result);
        uart_send_string("\r\n");
    }
}

void sd_test_detection_command(void) {
    uint16_t adc_value = read_adc_a5();
    
    uart_send_string("A5 ADC: ");
    uart_send_char('0' + (adc_value / 100));
    uart_send_char('0' + ((adc_value / 10) % 10));
    uart_send_char('0' + (adc_value % 10));
    uart_send_string("\r\n");
    
    uart_send_string("Shield: ");
    uart_send_string(hasShield ? "YES" : "NO");
    uart_send_string("\r\n");
}

void sd_redetect_command(void) {
    bool old_state = hasShield;
    
    uint16_t adc_value = read_adc_a5();
    hasShield = (adc_value > 5);
    
    uart_send_string("Was: ");
    uart_send_string(old_state ? "YES" : "NO");
    uart_send_string(" Now: ");
    uart_send_string(hasShield ? "YES" : "NO");
    uart_send_string("\r\n");
    
    if (hasShield && !old_state) {
        SPI_init();
    }
}

void sd_print_error(uint8_t error_code) {
    uart_send_char('0' + error_code);
    uart_send_string("\r\n");
}

void sd_print_hex_data(uint8_t *data, uint16_t length) {
    const char hex[] = "0123456789ABCDEF";
    
    for (uint16_t i = 0; i < length; i++) {
        if (i % 16 == 0) {
            uart_send_string("  ");
        }
        
        uart_send_char(hex[(data[i] >> 4) & 0x0F]);
        uart_send_char(hex[data[i] & 0x0F]);
        uart_send_char(' ');
        
        if ((i + 1) % 16 == 0) {
            uart_send_string("\r\n");
        }
    }
    
    if (length % 16 != 0) {
        uart_send_string("\r\n");
    }
}

uint32_t sd_parse_sector_number(char *str) {
    uint32_t result = 0;
    uint8_t i = 0;
    
    while (str[i] >= '0' && str[i] <= '9') {
        result = result * 10 + (str[i] - '0');
        i++;
    }
    
    return result;
}