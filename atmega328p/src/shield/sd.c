#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include "shield/sd.h"
#include "main.h"

void SPI_init(void) {
    DDRB |= (1 << SD_CS_PIN);
    SD_CS_high();
    
    DDRB |= (1 << SD_MOSI_PIN) | (1 << SD_SCK_PIN);
    DDRB &= ~(1 << SD_MISO_PIN);

    SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0);
}

uint8_t SPI_transfer(uint8_t data) {
    SPDR = data;
    while (!(SPSR & (1 << SPIF)));
    return SPDR;
}

void SD_CS_low(void) {
    PORTB &= ~(1 << SD_CS_PIN);
    _delay_us(10);
}

void SD_CS_high(void) {
    PORTB |= (1 << SD_CS_PIN);
    _delay_us(10);
}

uint8_t SD_get_response(void) {
    uint8_t response;
    uint8_t retry = 0;
    
    do {
        response = SPI_transfer(0xFF);
        retry++;
    } while ((response == 0xFF) && (retry < 255));
    
    return response;
}

void SD_wait_ready(void) {
    uint8_t response;
    
    do {
        response = SPI_transfer(0xFF);
    } while (response != 0xFF);
}

void SD_send_command(uint8_t cmd, uint32_t arg, uint8_t crc) {
    SPI_transfer(0x40 | cmd);
    SPI_transfer((uint8_t)(arg >> 24));
    SPI_transfer((uint8_t)(arg >> 16));
    SPI_transfer((uint8_t)(arg >> 8));
    SPI_transfer((uint8_t)arg);
    SPI_transfer(crc);
}

uint8_t SD_read_sector(uint32_t sector, uint8_t *buffer) {
    uint8_t response;
    uint8_t token;
    uint16_t retry = 0;

    if (!hasShield) return SD_ERROR_INIT;

    SD_CS_low();
    
    SD_send_command(17, sector, 0xFF);
    response = SD_get_response();
    
    if (response != 0x00) {
        SD_CS_high();
        return SD_ERROR_RESPONSE;
    }

    do {
        token = SPI_transfer(0xFF);
        retry++;
    } while ((token != 0xFE) && (retry < 65535));

    if (token != 0xFE) {
        SD_CS_high();
        return SD_ERROR_READ;
    }

    for (uint16_t i = 0; i < SD_SECTOR_SIZE; i++) {
        buffer[i] = SPI_transfer(0xFF);
    }

    SPI_transfer(0xFF);
    SPI_transfer(0xFF);

    SD_CS_high();
    return SD_SUCCESS;
}

uint8_t SD_write_sector(uint32_t sector, uint8_t *buffer) {
    uint8_t response;
    uint8_t status;

    if (!hasShield) return SD_ERROR_INIT;

    SD_CS_low();
    
    SD_send_command(24, sector, 0xFF);
    response = SD_get_response();
    
    if (response != 0x00) {
        SD_CS_high();
        return SD_ERROR_RESPONSE;
    }

    SPI_transfer(0xFE);

    for (uint16_t i = 0; i < SD_SECTOR_SIZE; i++) {
        SPI_transfer(buffer[i]);
    }

    SPI_transfer(0xFF);
    SPI_transfer(0xFF);

    status = SPI_transfer(0xFF);
    if ((status & 0x1F) != 0x05) {
        SD_CS_high();
        return SD_ERROR_WRITE;
    }

    SD_wait_ready();
    
    SD_CS_high();
    return SD_SUCCESS;
}

uint8_t SD_read_multiple_sectors(uint32_t start_sector, uint16_t num_sectors, uint8_t *buffer) {
    if (!hasShield) return SD_ERROR_INIT;
    
    for (uint16_t i = 0; i < num_sectors; i++) {
        uint8_t result = SD_read_sector(start_sector + i, buffer + (i * SD_SECTOR_SIZE));
        if (result != SD_SUCCESS) {
            return result;
        }
        _delay_ms(1);
    }
    
    return SD_SUCCESS;
}

uint8_t SD_write_multiple_sectors(uint32_t start_sector, uint16_t num_sectors, uint8_t *buffer) {
    if (!hasShield) return SD_ERROR_INIT;
    
    for (uint16_t i = 0; i < num_sectors; i++) {
        uint8_t result = SD_write_sector(start_sector + i, buffer + (i * SD_SECTOR_SIZE));
        if (result != SD_SUCCESS) {
            return result;
        }
        _delay_ms(5);
    }
    
    return SD_SUCCESS;
}

uint8_t SD_init(void) {
    uint8_t response;
    uint16_t retry;

    if (!hasShield) return SD_ERROR_INIT;

    SPI_init();
    
    SD_CS_high();
    for (uint8_t i = 0; i < 16; i++) {
        SPI_transfer(0xFF);
    }
    _delay_ms(1);

    SD_CS_low();
    SD_send_command(0, 0, 0x95);
    response = SD_get_response();
    SD_CS_high();

    if (response != 0x01) {
        return SD_ERROR_RESPONSE;
    }

    SD_CS_low();
    SD_send_command(8, 0x1AA, 0x87);
    response = SD_get_response();
    
    if (response == 0x01) {
        for (uint8_t i = 0; i < 4; i++) {
            SPI_transfer(0xFF);
        }
    }
    SD_CS_high();

    retry = 0;
    do {
        SD_CS_low();
        SD_send_command(55, 0, 0xFF);
        response = SD_get_response();
        SD_CS_high();

        if (response > 0x01) {
            return SD_ERROR_RESPONSE;
        }

        SD_CS_low();
        SD_send_command(41, 0x40000000, 0xFF);
        response = SD_get_response();
        SD_CS_high();

        retry++;
        _delay_ms(1);
        
    } while ((response != 0x00) && (retry < 1000));

    if (response != 0x00) {
        return SD_ERROR_RESPONSE;
    }

    SD_CS_low();
    SD_send_command(58, 0, 0xFF);
    response = SD_get_response();
    
    if (response == 0x00) {
        for (uint8_t i = 0; i < 4; i++) {
            SPI_transfer(0xFF);
        }
    }
    SD_CS_high();

    SPCR &= ~((1 << SPR1) | (1 << SPR0));
    
    return SD_SUCCESS;
}


