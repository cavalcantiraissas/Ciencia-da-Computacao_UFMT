     .section .text.boot, "ax"

    /* _start precisa ser global porque é o ENTRY definido no linker.ld */
    .globl _start

_start:
    csrr    t0, mhartid             /* t0 = identificador deste hart */
    bnez    t0, park                /* se não for o hart 0, estaciona */


    la      sp, __stack_top

    
    la      t0, __bss_start         /* t0 = endereço atual a zerar */
    la      t1, __bss_end           /* t1 = fim da região (exclusivo) */
bss_loop:
    bgeu    t0, t1, bss_done        /* chegou ao fim (ou .bss vazia)? sai */
    sb      zero, 0(t0)             /* escreve 0 no byte apontado por t0 */
    addi    t0, t0, 1               /* avança para o próximo byte */
    j       bss_loop
bss_done:
    call    main

park:
    
    wfi
    j       park
