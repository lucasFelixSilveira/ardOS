#ifndef ARDOS_SD_H
#define ARDOS_SD_H
#include <stdint.h>

#define SD_CS_PIN   PB2  // Chip Select
#define SD_MOSI_PIN PB3  // Master Out Slave In
#define SD_MISO_PIN PB4  // Master In Slave Out
#define SD_SCK_PIN  PB5  // Serial Clock

#define SD_SECTOR_SIZE 512
#define SD_SUCCESS 0
#define SD_ERROR_INIT 1
#define SD_ERROR_READ 2
#define SD_ERROR_WRITE 3
#define SD_ERROR_RESPONSE 4

// Basic SPI functions
void SPI_init(void);
uint8_t SPI_transfer(uint8_t data);

// CS control
void SD_CS_low(void);
void SD_CS_high(void);

// Initialization and commands
uint8_t SD_init(void);
void SD_send_command(uint8_t cmd, uint32_t arg, uint8_t crc);

// Read and write operations
uint8_t SD_read_sector(uint32_t sector, uint8_t *buffer);
uint8_t SD_write_sector(uint32_t sector, uint8_t *buffer);
uint8_t SD_read_multiple_sectors(uint32_t start_sector, uint16_t num_sectors, uint8_t *buffer);
uint8_t SD_write_multiple_sectors(uint32_t start_sector, uint16_t num_sectors, uint8_t *buffer);

// Utility functions
uint8_t SD_get_response(void);
void SD_wait_ready(void);

#endif
