// GPIO REGISTERS
#define GPIO_OUT_REG        0x3FF44004
#define GPIO_ENABLE_W1TS    0x3FF44024
#define GPIO_ENABLE_W1TC    0x3FF44028
#define GPIO(pin)          (1 << pin)

// IO MUX REGISTERS
#define IO_MUX_BASE        0x3FF49000
#define IO_MUX_GPIO15      (IO_MUX_BASE + 0x78)
#define MCU_SEL_S          12
#define MCU_SEL            0x2

// WATCHDOG REGISTERS
#define RTC_CNTL_WDTCONFIG0_REG   0x3FF48080
#define TIMG0_WDTCONFIG0_REG      0x3FF5F048
#define TIMG1_WDTCONFIG0_REG      0x3FF60048
#define RTC_CNTL_WDTWPROTECT_REG  0x3FF480A4
#define TIMG0_WDTWPROTECT_REG     0x3FF5F064
#define TIMG1_WDTWPROTECT_REG     0x3FF60064

.section .text

// Desabilita o Watchdog, o que é essencial para
// garantir a que o sistema não fique reiniciando
// automaticamente!

.align 4
disable_watchdog:
    movi a2, RTC_CNTL_WDTWPROTECT_REG
    movi a3, 0x50D83AA1
    s32i a3, a2, 0

    movi a2, RTC_CNTL_WDTCONFIG0_REG
    movi a3, 0
    s32i a3, a2, 0

    movi a2, RTC_CNTL_WDTWPROTECT_REG
    movi a3, 0
    s32i a3, a2, 0

    movi a2, TIMG0_WDTWPROTECT_REG
    movi a3, 0x50D83AA1
    s32i a3, a2, 0

    movi a2, TIMG0_WDTCONFIG0_REG
    movi a3, 0
    s32i a3, a2, 0

    movi a2, TIMG0_WDTWPROTECT_REG
    movi a3, 0
    s32i a3, a2, 0

    movi a2, TIMG1_WDTWPROTECT_REG
    movi a3, 0x50D83AA1
    s32i a3, a2, 0

    movi a2, TIMG1_WDTCONFIG0_REG
    movi a3, 0
    s32i a3, a2, 0

    movi a2, TIMG1_WDTWPROTECT_REG
    movi a3, 0
    s32i a3, a2, 0

    ret

// Função para configurar GPIO15 como saida,
// sobreescrevendo o funcionamento padrão do ESP32

.align 4
configure_gpio15:
    movi a2, IO_MUX_GPIO15
    l32i a3, a2, 0
    movi a4, (MCU_SEL << MCU_SEL_S)
    or a3, a3, a4
    s32i a3, a2, 0

    movi a2, GPIO_ENABLE_W1TS
    movi a3, GPIO(15)
    s32i a3, a2, 0
    ret

// Função para resetar um pino GPIO.
// - O pino que deve ser resetado é passado como parâmetro a2

.align 4
gpio_reset_pin:
    movi a3, IO_MUX_BASE
    slli a4, a2, 2
    add a3, a3, a4

    l32i a4, a3, 0
    movi a5, 0xFFFFF3FF
    and a4, a4, a5
    movi a5, 0x2000
    or a4, a4, a5
    s32i a4, a3, 0

    movi a3, GPIO_ENABLE_W1TC
    movi a4, 1
    sll a4, a2
    s32i a4, a3, 0

    ret
