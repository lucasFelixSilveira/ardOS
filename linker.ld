ENTRY(_start)
SECTIONS {
    . = 0x100000;
    .text : {
        *(.text)
        *(.rodata*)
    } :text =0
    .data : {
        *(.data)
    } :data =0
    .bss : {
        *(.bss)
    } :bss =0
}
PHDRS {
    text PT_LOAD FLAGS(5); /* RX */
    data PT_LOAD FLAGS(6); /* RW */
    bss PT_LOAD FLAGS(6); /* RW */
}
