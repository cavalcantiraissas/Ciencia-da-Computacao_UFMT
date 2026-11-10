OUTPUT_ARCH(riscv)
ENTRY(_start)

SECTIONS
{
    /* Na máquina "virt" do QEMU, a RAM começa em 0x80000000 */
    . = 0x80000000;

    .text : {
        *(.text.boot)   /* o código de boot vem primeiro */
        *(.text*)
    }
    .rodata : { *(.rodata*) }
    .data   : { *(.data*) }
    .bss    : {
        __bss_start = .;
        *(.bss*) *(COMMON)
        __bss_end = .;
    }

    . = ALIGN(16);
    . += 4096;          /* pilha de 4 KiB */
    __stack_top = .;
}
