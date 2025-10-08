#include <avr/io.h>
#include <stdbool.h>
#include <stdint.h>
#include <util/delay.h>

#include "pins.h"
#include "main.h"
#include "utils.h"

#include "serial/utils.h"
#include "serial/terminal.h"
#include "shield/sd.h"

void adc_init(void) {
    ADMUX = (1 << REFS0) | (1 << MUX2) | (1 << MUX0);
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

uint16_t read_adc_a5(void) {
    ADCSRA |= (1 << ADSC);
    while (ADCSRA & (1 << ADSC));
    return ADC;
}

char buffer[BUFFER_SIZE];
uint8_t buffer_index;
Task task;
char temp[TEMP_SIZE];
bool serialMode;
char *chunk;
bool hasShield;
static char chunk_buffer[512];

int main(void) {

    buffer_index = 0;
    DDRB |= (1 << LOG_PIN);
    DDRC &= ~(1 << SHIELD_CHECK_PIN);
    PORTC &= ~(1 << SHIELD_CHECK_PIN);

    uart_init();
    adc_init();
    SETHIGH(PORTB, LOG_PIN);

    task = TASK_IDLE;
    
    chunk = chunk_buffer;
    
    _delay_ms(50);
    
    uint16_t voltage_sum = 0;
    uint8_t valid_readings = 0;
    
    for (uint8_t i = 0; i < 10; i++) {
        uint16_t adc_value = read_adc_a5();
        voltage_sum += adc_value;
        if (adc_value > 5) {
            valid_readings++;
        }
        _delay_ms(2);
    }
    
    uint16_t avg_adc = voltage_sum / 10;
    
    hasShield = (valid_readings > 0) || (avg_adc > 5);
    
    if (hasShield) {
        SPI_init();
    }

    while(1) {
        if( (!serialMode) && uart_data_available() ) serialMode = true;
        if( uart_data_available() ) osSerialTerminal();
        else SETLOW(PORTB, LOG_PIN);
    }
}
