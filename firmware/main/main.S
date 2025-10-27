#include <xtensa/coreasm.h>
#include <xtensa/corebits.h>

#define LED_RED 15
#define LED_GREEN 2
#define LED_BLUE 4
#define BUZZER_VCC 22

#include "gpio.S"
#include "utils/beep.S"
#include "utils/ticks.S"

.section .rodata
.align 4
    boot_msg: .string "\r\nSistema Pronto! Digite 'b' para beep.\r\n"
    beep_msg: .string "\r\nBeep! GPIO15 ativado!\r\n"

.section .text
.global app_main

.align 4
firmware_setup:
    call0 disable_watchdog
    call0 configure_gpio15

    movi a2, 15
    call0 gpio_reset_pin

    movi a2, 2
    call0 gpio_reset_pin

    movi a2, 4
    call0 gpio_reset_pin

    movi a2, 22
    call0 gpio_reset_pin

    movi a2, GPIO_ENABLE_W1TS
    movi a3, (GPIO(LED_RED) | GPIO(LED_GREEN) | GPIO(LED_BLUE) | GPIO(BUZZER_VCC))
    s32i a3, a2, 0

    call0 beep

    movi a2, GPIO_OUT_REG
    movi a3, GPIO(LED_RED)
    s32i a3, a2, 0

    ret

.align 4
firmware_loop:
    j firmware_loop

.align 4
app_main:
    movi a1, 0x3FFE0000
    call0 firmware_setup
    j firmware_loop
