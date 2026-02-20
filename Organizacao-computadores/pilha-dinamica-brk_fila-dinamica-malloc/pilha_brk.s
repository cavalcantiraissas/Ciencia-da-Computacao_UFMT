# pilha_brk.s — Parte 1: Pilha Dinâmica no Heap usando syscall brk
# Universidade Federal de Mato Grosso — Organização de Computadores
# Professor: Linder Candido | Alunas: Raissa Cavalcanti e Vitória Calonga

.extern printf
.extern scanf

.section .data
fmt_titulo:   .asciz "\n========================================\n"
fmt_titulo2:  .asciz "   PILHA DINAMICA NO HEAP COM brk\n"
fmt_titulo3:  .asciz "========================================\n"
fmt_menu:     .asciz "\n[1] Push (empilhar)\n[2] Pop  (desempilhar)\n[0] Sair\nOpcao: "
fmt_entrada:  .asciz "Valor a empilhar: "
fmt_scan_d:   .asciz "%d"
fmt_push_ok:  .asciz "   >> PUSH(%d) realizado.\n   >> brk: 0x%lx -> 0x%lx  (+4 bytes alocados)\n"
fmt_pop_ok:   .asciz "   >> POP() = %d\n   >> brk: 0x%lx -> 0x%lx  (-4 bytes liberados)\n"
fmt_sep:      .asciz "----------------------------------------\n"
fmt_invalido: .asciz "   >> Opcao invalida.\n"

.section .bss
.align 3
buf_opcao: .space 4
buf_valor: .space 4

.section .text
.global main

# get_brk: retorna o program break atual em a0
get_brk:
    li  a7, 214
    li  a0, 0
    ecall
    ret

# empilha: recebe valor em a0
#   1. brk(0) -> break atual (onde gravaremos)
#   2. brk(break+4) -> aloca 4 bytes
#   3. sw valor no break antigo
empilha:
    addi sp, sp, -32
    sd   ra, 24(sp)
    sd   s0, 16(sp)
    sd   s1, 8(sp)

    mv   s0, a0             # s0 = valor

    li   a7, 214
    li   a0, 0
    ecall
    mv   s1, a0             # s1 = break antes

    addi a0, s1, 4
    li   a7, 214
    ecall                   # move break +4

    sw   s0, 0(s1)          # grava valor no break antigo

    ld   s1, 8(sp)
    ld   s0, 16(sp)
    ld   ra, 24(sp)
    addi sp, sp, 32
    ret

# desempilha: remove topo, retorna valor em a0
#   1. brk(0) -> break atual
#   2. lw (break-4) -> lê topo
#   3. brk(break-4) -> libera 4 bytes
desempilha:
    addi sp, sp, -32
    sd   ra, 24(sp)
    sd   s0, 16(sp)
    sd   s1, 8(sp)

    li   a7, 214
    li   a0, 0
    ecall
    mv   s0, a0             # s0 = break antes

    addi t0, s0, -4
    lw   s1, 0(t0)          # lê topo (32 bits)

    mv   a0, t0
    li   a7, 214
    ecall                   # move break -4 (libera)

    mv   a0, s1             # retorna valor

    ld   s1, 8(sp)
    ld   s0, 16(sp)
    ld   ra, 24(sp)
    addi sp, sp, 32
    ret

# PROGRAMA PRINCIPAL — menu interativo
main:
    addi sp, sp, -48
    sd   ra, 40(sp)
    sd   s0, 32(sp)
    sd   s1, 24(sp)
    sd   s2, 16(sp)

    la   a0, fmt_titulo
    call printf
    la   a0, fmt_titulo2
    call printf
    la   a0, fmt_titulo3
    call printf

.menu_loop:
    la   a0, fmt_menu
    call printf

    la   a0, fmt_scan_d
    la   a1, buf_opcao
    call scanf
    la   t0, buf_opcao      # recarrega endereço (a1 foi destruído pelo scanf)
    lw   s0, 0(t0)          # s0 = opção lida

    li   t1, 1
    beq  s0, t1, .op_push

    li   t1, 2
    beq  s0, t1, .op_pop

    li   t1, 0
    beq  s0, t1, .sair

    la   a0, fmt_invalido
    call printf
    j    .menu_loop

.op_push:
    la   a0, fmt_entrada
    call printf

    # Lê valor 
    la   a0, fmt_scan_d
    la   a1, buf_valor
    call scanf
    la   t0, buf_valor      # recarrega endereço 
    lw   s0, 0(t0)          # s0 = valor lido

    # Captura break ANTES
    call get_brk
    mv   s1, a0             # s1 = break antes

    # Executa empilha
    mv   a0, s0
    call empilha

    # Captura break DEPOIS
    call get_brk
    mv   s2, a0             # s2 = break depois

    la   a0, fmt_push_ok
    mv   a1, s0             # %d  = valor
    mv   a2, s1             # %lx = break antes
    mv   a3, s2             # %lx = break depois
    call printf

    la   a0, fmt_sep
    call printf
    j    .menu_loop

.op_pop:
    # Captura break ANTES
    call get_brk
    mv   s0, a0             # s0 = break antes

    call desempilha
    mv   s1, a0             # s1 = valor desempilhado

    # Captura break DEPOIS
    call get_brk
    mv   s2, a0             # s2 = break depois

    la   a0, fmt_pop_ok
    mv   a1, s1             # %d  = valor
    mv   a2, s0             # %lx = break antes
    mv   a3, s2             # %lx = break depois
    call printf

    la   a0, fmt_sep
    call printf
    j    .menu_loop

.sair:
    li   a0, 0
    ld   s2, 16(sp)
    ld   s1, 24(sp)
    ld   s0, 32(sp)
    ld   ra, 40(sp)
    addi sp, sp, 48
    ret
