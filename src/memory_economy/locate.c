#include <stdint.h>
#include "main.h"

uint8_t locale(uint8_t word_index, char* temp, uint8_t temp_size) {
    uint8_t w = 0, ti = 0;
    for( uint8_t i = 0; i < buffer_index; i++ ) {
        if( buffer[i] != ' ' && ti < temp_size - 1 && w == word_index) temp[ti++] = buffer[i];
        else if( buffer[i] == ' ' ) {
            if( w == word_index ) break;
            w++;
            ti = 0;
        }
    }

    if( w != word_index ) { temp[0] = '\0'; return 0; }
    temp[ti] = '\0';
    return 1;
}
