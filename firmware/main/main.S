#include <xtensa/coreasm.h>
#include <xtensa/corebits.h>

#include "gpio.S"
#include "uart.S"
#include "utils/beep.S"
#include "utils/ticks.S"

.section .text
.global app_main

.align 4
firmware_setup:
    call0 disable_watchdog
    call0 configure_gpio15
    call0 uart_init

    movi a2, 15
    call0 gpio_reset_pin

    movi a2, 2
    call0 gpio_reset_pin

    movi a2, 4
    call0 gpio_reset_pin

    movi a2, 22
    call0 gpio_reset_pin

    movi a2, GPIO_ENABLE_W1TS
    movi a3, (GPIO(15) | GPIO(2) | GPIO(4) | GPIO(22))
    s32i a3, a2, 0

    call0 beep

    movi a2, boot_msg
    call0 uart_send_string

    ret

.align 4
firmware_loop:
    call0 uart_receive

    movi a3, 0xFF
    beq a2, a3, firmware_loop

    mov a4, a2
    call0 uart_send

    movi a3, 'b'
    bne a4, a3, 1f
    call0 beep
    movi a2, beep_msg
    call0 uart_send_string
    j firmware_loop

1:
    movi a2, 0x0D
    call0 uart_send
    movi a2, 0x0A
    call0 uart_send
    j firmware_loop

.align 4
app_main:
    movi a1, 0x3FFE0000
    call0 firmware_setup
    j firmware_loop

.section .rodata
.align 4
boot_msg:
    .string "\r\nSistema Pronto! Digite 'b' para beep.\r\n"

beep_msg:
    .string "\r\nBeep! GPIO15 ativado!\r\n"
