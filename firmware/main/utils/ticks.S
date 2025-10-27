.align 4
tick_10uF:
    movi    a4, 0x1600000
1:
    addi    a4, a4, -1
    bnez    a4, 1b
    ret
