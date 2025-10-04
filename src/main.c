#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h>

void osTerminal(void);

#define F_CPU 16000000UL
#define LOG_PIN PB5
#define BAUD 9600
#define UBRR_VALUE ((F_CPU/16/BAUD)-1)

#define SETHIGH(PORT, PIN) PORT |=  (1 << PIN)
#define SETLOW(PORT, PIN)  PORT &= ~(1 << PIN)
#define INVERT(PORT, PIN)  PORT ^=  (1 << PIN)

#define MULTITHREAD \
    if( uart_data_available() ) osTerminal();

#define BLINK(times, port, pin) \
    for( uint8_t i = 0; i < times; i++ ) { \
        INVERT(port, pin); \
        _delay_ms(75); \
    }

int8_t strcmp_fast(const char s1[], const char s2[]) {
    while(*s1 && (*s1 == *s2)) s1++, s2++;
    return (int8_t)(*s1 - *s2);
}

void uart_init(void) {
    UBRR0H = (uint8_t)(UBRR_VALUE >> 8);
    UBRR0L = (uint8_t)UBRR_VALUE;
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

uint8_t uart_data_available(void) {
    return (UCSR0A & (1 << RXC0)) != 0;
}

char uart_receive_char(void) {
    while(!(UCSR0A & (1 << RXC0)));
    return UDR0;
}

void uart_send_char(char c) {
    while(!(UCSR0A & (1 << UDRE0)));
    UDR0 = c;
}

void uart_send_string(const char* str) {
    while (*str) uart_send_char(*str++);
}

void uart_send_uint8(uint8_t n) {
    if( n >= 100 ) uart_send_char('0' + (n / 100) % 10);
    if( n >= 10 ) uart_send_char('0' + (n / 10) % 10);
    uart_send_char('0' + n % 10);
}

void uart_set_cursor(uint8_t row, uint8_t col) {
    uart_send_char(27);
    uart_send_char('[');
    uart_send_uint8(row);
    uart_send_char(';');
    uart_send_uint8(col);
    uart_send_char('H');
}

typedef enum {
    TASK_BLINKING,
    TASK_IDLE
} Task;

#define BUFFER_SIZE 64
char buffer[BUFFER_SIZE];
uint8_t buffer_index = 0;
Task task;

uint8_t locale(uint8_t word_index, char* temp, uint8_t temp_size) {
    uint8_t w = 0, ti = 0;
    for( uint8_t i = 0; i < buffer_index; i++ ) {
        if( buffer[i] != ' ' && ti < temp_size - 1 ) {
            if( w == word_index ) temp[ti++] = buffer[i];
        } else if( buffer[i] == ' ' ) {
            if( w == word_index ) break;
            w++;
            ti = 0;
        }
    }
    if( w != word_index ) { temp[0] = '\0'; return 0; }
    temp[ti] = '\0';
    return 1;
}

void snake(void) {
    const uint8_t WIDTH = 16;
    const uint8_t HEIGHT = 8;
    uint8_t head = 0;
    uint8_t length = 3;
    uint8_t body[16];
    uint8_t apple = 27;
    uint8_t tick = 0;
    char dir = 'D';

    for( uint8_t i = 0; i < length; i++ )
        body[i] = head - i;

    uart_send_string("\033[2J\033[H");
    for( uint8_t y = 0; y < HEIGHT; y++ ) {
        for( uint8_t x = 0; x < WIDTH; x++ ) {
            uart_send_char('.');
        }
        uart_send_string("\r\n");
    }

    while(length < 16) {
        if( uart_data_available() ) {
            char c = uart_receive_char();
            if( c == 'W' || c == 'w' ) dir = 'W';
            else if( c == 'A' || c == 'a' ) dir = 'A';
            else if( c == 'S' || c == 's' ) dir = 'S';
            else if( c == 'D' || c == 'd' ) dir = 'D';
            else if( c == 'K' || c == 'k' ) {
                task = TASK_IDLE;
                break;
            }
        }

        uint8_t x = head % WIDTH;
        uint8_t y = head / WIDTH;

        switch (dir) {
            case 'W': y = (y == 0) ? HEIGHT - 1 : y - 1; break;
            case 'S': y = (y + 1) % HEIGHT; break;
            case 'A': x = (x == 0) ? WIDTH - 1 : x - 1; break;
            case 'D': x = (x + 1) % WIDTH; break;
        }

        uint8_t new_head = y * WIDTH + x;

        uint8_t tail = body[length - 1];
        uart_set_cursor((tail / WIDTH) + 1, (tail % WIDTH) + 1);
        uart_send_char('.');

        for( int i = length - 1; i > 0; i-- )
            body[i] = body[i - 1];
        body[0] = new_head;
        head = new_head;

        uart_set_cursor((head / WIDTH) + 1, (head % WIDTH) + 1);
        uart_send_char('o');

        if( length > 1 ) {
            uint8_t neck = body[1];
            uart_set_cursor((neck / WIDTH) + 1, (neck % WIDTH) + 1);
            uart_send_char('o');
        }

        uint8_t ax = apple % WIDTH;
        uint8_t ay = apple / WIDTH;
        uart_set_cursor(ay + 1, ax + 1);
        uart_send_char('@');

        if( head == apple && length < 16 ) {
            body[length] = body[length - 1];
            length++;
            apple = (apple + 13) % (WIDTH * HEIGHT);
        }

        _delay_ms(250);
        tick++;
    }

    uart_set_cursor(HEIGHT + 1, 1);
    uart_send_string("Fim do jogo!\r\n");
}

char temp[16];

void blink(void) {
    while(task == TASK_BLINKING) {
        MULTITHREAD
        PORTB ^= (1 << LOG_PIN);
        _delay_ms(300);
    }
}

void commands(void) {
    if( strcmp_fast(temp, "echo") == 0 ) {
        uart_send_string("echoing \"");
        uart_send_string(buffer);
        uart_send_string("\"...\r\n");
    }
    else if( strcmp_fast(temp, "blink") == 0 ) {
        buffer[0] = '\0';
        buffer_index = 0;

        uart_send_string("Starts blinking! - Type kill to finish.\r\n");
        task = TASK_BLINKING;
        blink();
        task = TASK_IDLE;
    }
    else if( strcmp_fast(temp, "kill") == 0 && task != TASK_IDLE ) {
        task = TASK_IDLE;
    }
    else if( strcmp_fast(temp, "clear") == 0 ) {
        uart_send_string("\033[2J\033[H");
    }
    else if( strcmp_fast(temp, "snake") == 0 ) {
        snake();
    } else {
        uart_send_string("current@super % cleaned buffer - ERROR: command `");
        uart_send_string(temp);
        uart_send_string("` not found\r\n");

        BLINK(5, PORTB, LOG_PIN);
    }
}

void osTerminal(void) {
    char c = uart_receive_char();

    if( c == 13 || c == 10 ) {
        uart_send_string("\r\n");

        if(! locale(0, temp, sizeof(temp)) ) {
            uart_send_string("current@super % cleaned buffer - ERROR: command not found\r\n");
            buffer[0] = '\0';
            buffer_index = 0;
            return;
        }

        uint8_t len = 0;
        while(temp[len]) len++;
        if(len > 0 && (temp[len-1] == '\r' || temp[len-1] == '\n')) temp[len-1] = '\0';

        SETHIGH(PORTB, LOG_PIN);

        uart_send_string("current@super % running ");
        uart_send_string(temp);
        uart_send_string("...\r\n");

        commands();

        SETLOW(PORTB, LOG_PIN);

        buffer[0] = '\0';
        buffer_index = 0;
        return;
    }

    if( c == ':' ) {
        uart_send_string("current@super % cleaned buffer\r\n");
        buffer_index = 0;
        return;
    }

    if( buffer_index < BUFFER_SIZE - 1 ) {
        buffer[buffer_index++] = c;
        buffer[buffer_index] = '\0';
    }

    uart_send_string("\r");
    uart_send_string("current@user % ");
    uart_send_string(buffer);

    INVERT(PORTB, LOG_PIN);
}

int main(void) {
    DDRB |= (1 << LOG_PIN);
    uart_init();
    SETHIGH(PORTB, LOG_PIN);

    task = TASK_IDLE;

    while(1) {
        if( uart_data_available() ) osTerminal();
        else SETLOW(PORTB, LOG_PIN);
    }
}
