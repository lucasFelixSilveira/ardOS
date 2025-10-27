.align 4
beep:
    addi a1, a1, -12
    s32i a0, a1, 0
    s32i a2, a1, 4
    s32i a3, a1, 8

    movi a2, GPIO_OUT_REG
    movi a3, (GPIO(LED_RED) | GPIO(LED_GREEN) | GPIO(LED_BLUE) | GPIO(BUZZER_VCC))
    s32i a3, a2, 0

    call0 tick_10uF

    movi a2, GPIO_OUT_REG
    l32i a3, a2, 0
    movi a2, ~(GPIO(LED_RED) | GPIO(LED_GREEN) | GPIO(LED_BLUE))
    and a3, a3, a2
    movi a2, GPIO_OUT_REG
    s32i a3, a2, 0

    l32i a0, a1, 0
    l32i a2, a1, 4
    l32i a3, a1, 8
    addi a1, a1, 12

    ret
