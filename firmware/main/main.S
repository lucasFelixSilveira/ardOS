#include <xtensa/coreasm.h>
#include <xtensa/corebits.h>

#define HALF_HALF         0x200000
#define HALF              0x400000
#define ONE               0x800000

// GPIO REGISTERS
#define GPIO_OUT_REG        0x3FF44004
#define GPIO_ENABLE_W1TS    0x3FF44024
#define GPIO_ENABLE_W1TC    0x3FF44028
#define GPIO(pin)          (1 << pin)

// IO MUX REGISTERS - ESSENCIAIS PARA GPIO15
#define IO_MUX_BASE        0x3FF49000
#define IO_MUX_GPIO15      (IO_MUX_BASE + 0x78)  // SD_DATA3 - GPIO15
#define MCU_SEL_S          12
#define MCU_SEL            0x2                   // GPIO function

// UART REGISTERS (UART0)
#define UART_BASE          0x3FF40000
#define UART_FIFO          (UART_BASE + 0x000)
#define UART_STATUS        (UART_BASE + 0x01C)
#define UART_CLKDIV        (UART_BASE + 0x014)
#define UART_CONF0         (UART_BASE + 0x020)

// WATCHDOG REGISTERS
#define RTC_CNTL_WDTCONFIG0_REG   0x3FF48098
#define TIMG0_WDTCONFIG0_REG      0x3FF5F098
#define TIMG1_WDTCONFIG0_REG      0x3FF60098

// UART CONFIGURATION
#define UART_BAUD_RATE     115200
#define UART_CLK_FREQ      80000000
#define UART_CLKDIV_VAL    (UART_CLK_FREQ / UART_BAUD_RATE)

.section .text
.global app_main

// ========== DISABLE WATCHDOG ==========
.align 4
disable_watchdog:
    // Disable RTC WDT
    movi a2, RTC_CNTL_WDTCONFIG0_REG
    movi a3, 0
    s32i a3, a2, 0

    // Disable TIMG0 WDT
    movi a2, TIMG0_WDTCONFIG0_REG
    movi a3, 0
    s32i a3, a2, 0

    // Disable TIMG1 WDT
    movi a2, TIMG1_WDTCONFIG0_REG
    movi a3, 0
    s32i a3, a2, 0
    ret

// ========== CONFIGURE GPIO15 ==========
.align 4
configure_gpio15:
    // Configurar IO MUX para GPIO15 - ESSENCIAL!
    movi a2, IO_MUX_GPIO15
    l32i a3, a2, 0                // Ler valor atual
    movi a4, (MCU_SEL << MCU_SEL_S) // Setar MCU_SEL para GPIO function
    or a3, a3, a4
    s32i a3, a2, 0                // Escrever de volta

    // Habilitar como output
    movi a2, GPIO_ENABLE_W1TS
    movi a3, GPIO(15)
    s32i a3, a2, 0
    ret

// ========== BEEP FUNCTION ==========
.align 4
beep:
    /* Resetar GPIO15 */
    movi a2, 15
    call0 gpio_reset_pin

    /* Resetar GPIO2 */
    movi a2, 2
    call0 gpio_reset_pin

    /* Resetar qualquer pino */
    movi a2, 4
    call0 gpio_reset_pin

    /* Resetar qualquer pino */
    movi a2, 22
    call0 gpio_reset_pin

    // ATIVAR PINOS (GPIO15, GPIO2, GPIO4)
    movi a2, GPIO_OUT_REG
    movi a3, (GPIO(15) | GPIO(2) | GPIO(4) | GPIO(22))
    s32i a3, a2, 0

    call0 tick_10uF

    // DESATIVAR PINOS
    movi a2, GPIO_OUT_REG
    l32i a3, a2, 0
    movi a4, ~(GPIO(15) | GPIO(2) | GPIO(4))
    and a3, a3, a4
    s32i a3, a2, 0
    ret

.align 4
tick_10uF:
    movi    a4, 0x1600000
1:
    addi    a4, a4, -1
    bnez    a4, 1b
    ret

// ========== UART SIMPLES ==========
.align 4
uart_init:
    // Configurar baud rate
    movi a2, UART_CLKDIV
    movi a3, UART_CLKDIV_VAL
    s32i a3, a2, 0

    // Configurar UART: 8 bits, no parity, 1 stop bit
    movi a2, UART_CONF0
    movi a3, 0x0000001C
    s32i a3, a2, 0
    ret

.align 4
uart_send:
    // Esperar TX FIFO não estar cheio
    movi a3, UART_STATUS
1:
    l32i a4, a3, 0
    extui a4, a4, 16, 8   // TX count bits
    movi a5, 127
    bge a4, a5, 1b

    // Enviar caractere
    movi a3, UART_FIFO
    s32i a2, a3, 0
    ret

.align 4
uart_receive:
    // Verificar se há dados no RX FIFO
    movi a2, UART_STATUS
    l32i a3, a2, 0
    extui a3, a3, 0, 9    // RX count bits

    beqz a3, 2f

    // Ler caractere
    movi a2, UART_FIFO
    l32i a2, a2, 0
    ret

2:
    // Nenhum caractere disponível
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

// ========== BOOT AND MAIN ==========
.align 4
firmware_setup:
    // DESABILITAR WATCHDOG PRIMEIRO!
    call0 disable_watchdog

    // CONFIGURAR GPIO15 ANTES DE QUALQUER COISA!
    call0 configure_gpio15

    // Inicializar UART
    call0 uart_init

    // Configurar outros GPIOs
    movi a2, GPIO_ENABLE_W1TS
    movi a3, (GPIO(5) | GPIO(2) | GPIO(4) | GPIO(22))
    s32i a3, a2, 0

    movi a2, GPIO_OUT_REG
    movi a3, GPIO(22)
    s32i a3, a2, 0

    // Beep de boot
    call0 beep

    // Mensagem de boot via UART
    movi a2, boot_msg
    call0 uart_send_string

    ret

.align 4
app_main:
    // Setup stack pointer
    movi a1, 0x3FFE0000

    // Setup firmware
    call0 firmware_setup

    // Main loop
    j firmware_loop

.align 4
firmware_loop:
    // Tentar receber caractere
    call0 uart_receive

    // Verificar se recebeu algo (não 0xFF)
    movi a3, 0xFF
    beq a2, a3, firmware_loop

    // Echo do caractere recebido
    mov a4, a2
    call0 uart_send

    // Verificar se é comando 'b' para beep
    movi a3, 'b'
    bne a4, a3, 1f
    call0 beep
    movi a2, beep_msg
    call0 uart_send_string
    j firmware_loop

1:
    // Outro caractere - apenas echo
    movi a2, 0x0D
    call0 uart_send
    movi a2, 0x0A
    call0 uart_send
    j firmware_loop

.align 4
gpio_reset_pin:
    /* a2 = pino para resetar */

    /* 1. Configurar IO MUX para função GPIO (MCU_SEL = 2) */
    movi a3, IO_MUX_BASE
    slli a4, a2, 2          /* a4 = pino * 4 */
    add a3, a3, a4          /* a3 = endereço do IO MUX do pino */

    l32i a4, a3, 0          /* Ler valor atual */
    movi a5, 0xFFFFF3FF     /* Máscara para limpar MCU_SEL */
    and a4, a4, a5          /* Limpar bits 12-13 */
    movi a5, 0x2000         /* MCU_SEL = 2 (GPIO function) */
    or a4, a4, a5           /* Setar para GPIO */
    s32i a4, a3, 0          /* Escrever de volta */

    /* 2. Desabilitar output no GPIO */
    movi a3, GPIO_ENABLE_W1TC
    movi a4, 1
    sll a4, a2         /* a4 = (1 << pino) */
    s32i a4, a3, 0

    ret

// ========== DATA SECTION ==========
.section .rodata
.align 4
boot_msg:
    .string "\r\nSistema Pronto! Digite 'b' para beep.\r\n"

beep_msg:
    .string "\r\nBeep! GPIO15 ativado!\r\n"
