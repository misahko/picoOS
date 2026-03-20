ENTRY(Reset_Handler)

MEMORY {
    RAM (rwx) : ORIGIN = 0x20000000, LENGTH = 256K
    FLASH (rx) : ORIGIN = 0x10000000, LENGTH = 4096K
}

_estack = ORIGIN(RAM) + LENGTH(RAM);

SECTIONS {
    .boot2 : {
        KEEP(*(.boot2))
    } > FLASH

    /* ЖОРСТКО ставимо адресу */
    . = ORIGIN(FLASH) + 0x100;
    .isr_vector : {
        . = ALIGN(4);
        KEEP(*(.isr_vector))
        . = ALIGN(4);
    } > FLASH

    .text : {
        . = ALIGN(4);
        *(.text*)
        *(.rodata*)
        . = ALIGN(4);
    } > FLASH

    .bss : {
        . = ALIGN(4);
        _sbss = .;
        *(.bss*)
        *(COMMON)
        _ebss = .;
        . = ALIGN(4);
    } > RAM

    .data : {
        . = ALIGN(4);
        _sdata = .;
        *(.data*)
        _edata = .;
        . = ALIGN(4);
    } > RAM AT > FLASH

    _sidata = LOADADDR(.data);
}