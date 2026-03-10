OUTPUT_ARCH(riscv)
ENTRY(_start)

MEMORY
{
    RAM (rwx) : ORIGIN = 0x80000000, LENGTH = 128M
}

SECTIONS
{
    . = 0x80000000;

    .text :
    {
        *(.text*)
    } > RAM

    . = ALIGN(4);

    .rodata :
    {
        *(.rodata*)
    } > RAM

    . = ALIGN(4);

    .data :
    {
        *(.data*)
    } > RAM

    . = ALIGN(4);

    .bss :
    {
        __bss_start = .;
        *(.bss*)
        *(COMMON)
        __bss_end = .;
    } > RAM
}
