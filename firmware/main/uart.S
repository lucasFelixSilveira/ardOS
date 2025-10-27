#include <xtensa/coreasm.h>
#include <xtensa/corebits.h>

// UART REGISTERS (UART0)
#define UART_BASE          0x3FF40000
#define UART_FIFO          (UART_BASE + 0x000)
#define UART_STATUS        (UART_BASE + 0x01C)
#define UART_CLKDIV        (UART_BASE + 0x014)
#define UART_CONF0         (UART_BASE + 0x020)

// UART CONFIGURATION
#define UART_BAUD_RATE     115200
#define UART_CLK_FREQ      80000000
#define UART_CLKDIV_VAL    (UART_CLK_FREQ / UART_BAUD_RATE)

.section .text
.global app_main

.align 4
uart_init:
    movi a2, UART_CLKDIV
    movi a3, UART_CLKDIV_VAL
    s32i a3, a2, 0

    movi a2, UART_CONF0
    movi a3, 0x0000001C
    s32i a3, a2, 0
    ret

.align 4
uart_send:
    movi a3, UART_STATUS
1:
    l32i a4, a3, 0
    extui a4, a4, 16, 8
    movi a5, 127
    bge a4, a5, 1b

    // Enviar caractere
    movi a3, UART_FIFO
    s32i a2, a3, 0
    ret

.align 4
uart_receive:
    movi a2, UART_STATUS
    l32i a3, a2, 0
    extui a3, a3, 0, 9

    beqz a3, 2f

    // Ler caractere
    movi a2, UART_FIFO
    l32i a2, a2, 0
    ret

2:
    movi a2, 0xFF
    ret

.align 4
uart_send_string:
    mov a5, a2
1:
    l8ui a4, a5, 0
    beqz a4, 3f

    mov a2, a4
    call0 uart_send

    addi a5, a5, 1
    j 1b
3:
    ret
