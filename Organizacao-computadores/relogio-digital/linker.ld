/* ============================================================
 * linker.ld — Script de ligação para QEMU virt rv64
 *
 * O QEMU carrega o kernel ELF com "-bios none -kernel" e
 * transfere o controle diretamente ao endereço de entrada
 * (_start), que deve estar em 0x80000000.
 * ============================================================ */

OUTPUT_ARCH(riscv)
ENTRY(_start)

MEMORY
{
    /* RAM: 128 MB a partir de 0x80000000 (QEMU virt padrão) */
    RAM (rwx) : ORIGIN = 0x80000000, LENGTH = 128M
}

SECTIONS
{
    /* ---- Código ----------------------------------------- */
    . = 0x80000000;

    .text :
    {
        /* start.S deve ser o primeiro objeto ligado */
        *(.text.start)
        *(.text .text.*)
    } > RAM

    /* ---- Dados somente-leitura -------------------------- */
    .rodata :
    {
        *(.rodata .rodata.*)
    } > RAM

    /* ---- Dados inicializados ---------------------------- */
    .data :
    {
        *(.data .data.*)
    } > RAM

    /* ---- Dados não inicializados (zerados no boot) ------ */
    .bss :
    {
        _bss_start = .;
        *(.bss .bss.*)
        *(COMMON)
        _bss_end = .;
    } > RAM

    /* ---- Pilha ------------------------------------------ */
    . = ALIGN(16);
    _stack_bottom = .;
    . += 0x10000;       /* 64 KB de pilha */
    _stack_top = .;
}
