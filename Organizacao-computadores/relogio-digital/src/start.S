/* ============================================================
 * start.S — Código de inicialização bare metal RISC-V 64 bits
 *
 * Responsabilidades:
 *   1. Configurar o ponteiro de pilha (sp)
 *   2. Instalar o vetor de trap (mtvec) apontando para
 *      trap_entry (modo direto)
 *   3. Zerar a seção BSS
 *   4. Chamar main()
 *
 * trap_entry:
 *   - Salva TODOS os registradores na pilha
 *   - Chama trap_handler() em C
 *   - Restaura os registradores
 *   - Executa mret
 * ============================================================ */

    .section .text.start
    .global  _start

_start:
    /* ----------------------------------------------------- *
     * 1. Configurar stack pointer                           *
     * ----------------------------------------------------- */
    la   sp, _stack_top

    /* ----------------------------------------------------- *
     * 2. Instalar trap vector (modo direto)                 *
     *    trap_entry deve estar alinhado a 4 bytes.          *
     * ----------------------------------------------------- */
    la   t0, trap_entry
    csrw mtvec, t0

    /* ----------------------------------------------------- *
     * 3. Zerar BSS                                          *
     * ----------------------------------------------------- */
    la   t0, _bss_start
    la   t1, _bss_end
.Lclear_bss:
    bge  t0, t1, .Ldone_bss
    sd   zero, 0(t0)
    addi t0, t0, 8
    j    .Lclear_bss
.Ldone_bss:

    /* ----------------------------------------------------- *
     * 4. Chamar main()                                      *
     * ----------------------------------------------------- */
    call main

    /* Nunca deve chegar aqui — loop infinito de segurança */
.Lhalt:
    j    .Lhalt


/* ============================================================
 * trap_entry — Ponto de entrada de todas as exceções e
 *              interrupções (mtvec modo direto).
 *
 * Salva o contexto completo do processador (todos os 31
 * registradores de uso geral, exceto x0/zero), chama a
 * rotina C trap_handler(), restaura o contexto e executa
 * mret para retornar ao ponto de execução original.
 * ============================================================ */
    .section .text
    .global  trap_entry
    .align   2               /* alinhamento a 4 bytes (2^2) */

trap_entry:
    /* ---- Alocar frame na pilha: 32 slots × 8 bytes = 256 bytes ---- */
    addi sp, sp, -256

    /* ---- Salvar registradores de uso geral (x1–x31) ---- */
    sd   ra,    0(sp)     /* x1  */
    /* x2 (sp) salvo logo após, com valor ANTES de addi acima */
    sd   gp,   16(sp)     /* x3  */
    sd   tp,   24(sp)     /* x4  */
    sd   t0,   32(sp)     /* x5  */
    sd   t1,   40(sp)     /* x6  */
    sd   t2,   48(sp)     /* x7  */
    sd   s0,   56(sp)     /* x8  */
    sd   s1,   64(sp)     /* x9  */
    sd   a0,   72(sp)     /* x10 */
    sd   a1,   80(sp)     /* x11 */
    sd   a2,   88(sp)     /* x12 */
    sd   a3,   96(sp)     /* x13 */
    sd   a4,  104(sp)     /* x14 */
    sd   a5,  112(sp)     /* x15 */
    sd   a6,  120(sp)     /* x16 */
    sd   a7,  128(sp)     /* x17 */
    sd   s2,  136(sp)     /* x18 */
    sd   s3,  144(sp)     /* x19 */
    sd   s4,  152(sp)     /* x20 */
    sd   s5,  160(sp)     /* x21 */
    sd   s6,  168(sp)     /* x22 */
    sd   s7,  176(sp)     /* x23 */
    sd   s8,  184(sp)     /* x24 */
    sd   s9,  192(sp)     /* x25 */
    sd   s10, 200(sp)     /* x26 */
    sd   s11, 208(sp)     /* x27 */
    sd   t3,  216(sp)     /* x28 */
    sd   t4,  224(sp)     /* x29 */
    sd   t5,  232(sp)     /* x30 */
    sd   t6,  240(sp)     /* x31 */

    /* Salvar sp original (= sp + 256, valor antes do addi) */
    addi t0, sp, 256
    sd   t0,   8(sp)      /* x2 = sp original */

    /* ---- Chamar o despachador de interrupções em C ---- */
    call trap_handler

    /* ---- Restaurar registradores de uso geral ---- */
    ld   ra,    0(sp)
    /* não restaurar sp agora; restaurar ao final */
    ld   gp,   16(sp)
    ld   tp,   24(sp)
    ld   t0,   32(sp)
    ld   t1,   40(sp)
    ld   t2,   48(sp)
    ld   s0,   56(sp)
    ld   s1,   64(sp)
    ld   a0,   72(sp)
    ld   a1,   80(sp)
    ld   a2,   88(sp)
    ld   a3,   96(sp)
    ld   a4,  104(sp)
    ld   a5,  112(sp)
    ld   a6,  120(sp)
    ld   a7,  128(sp)
    ld   s2,  136(sp)
    ld   s3,  144(sp)
    ld   s4,  152(sp)
    ld   s5,  160(sp)
    ld   s6,  168(sp)
    ld   s7,  176(sp)
    ld   s8,  184(sp)
    ld   s9,  192(sp)
    ld   s10, 200(sp)
    ld   s11, 208(sp)
    ld   t3,  216(sp)
    ld   t4,  224(sp)
    ld   t5,  232(sp)
    ld   t6,  240(sp)

    /* Restaurar sp */
    addi sp, sp, 256

    /* ---- Retornar da interrupção/exceção ---- */
    mret
