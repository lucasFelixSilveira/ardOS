.align 4
beep:
    movi a2, GPIO_OUT_REG
    movi a3, (GPIO(15) | GPIO(2) | GPIO(4) | GPIO(22))
    s32i a3, a2, 0

    call0 tick_10uF

    movi a2, GPIO_OUT_REG
    l32i a3, a2, 0
    movi a4, ~(GPIO(15) | GPIO(2) | GPIO(4))
    and a3, a3, a4
    s32i a3, a2, 0
    ret
