# fila_malloc.s — Parte 2: Fila Dinâmica no Heap usando malloc e free
# Universidade Federal de Mato Grosso — Organização de Computadores
# Professor: Linder Candido | Alunas: Raissa Cavalcanti e Vitória Calonga
#
# Estrutura de cada nó (16 bytes):
#   offset 0: value (4 bytes, int 32-bit)  -> sw / lw
#   offset 4: padding (4 bytes)
#   offset 8: next*  (8 bytes, ponteiro)   -> sd / ld

.extern malloc
.extern free
.extern printf
.extern scanf

.section .data
fmt_titulo:   .asciz "\n========================================\n"
fmt_titulo2:  .asciz "   FILA DINAMICA NO HEAP COM malloc\n"
fmt_titulo3:  .asciz "========================================\n"
fmt_menu:     .asciz "\n[1] Insert (inserir na fila)\n[2] Remove (remover da fila)\n[0] Sair\nOpcao: "
fmt_entrada:  .asciz "Valor a inserir: "
fmt_scan_d:   .asciz "%d"
fmt_insert:   .asciz "   >> insert(%d) realizado.\n   >> No alocado no endereco: 0x%lx\n"
fmt_remove:   .asciz "   >> remove() = %d\n   >> No liberado no endereco:  0x%lx\n"
fmt_vazia:    .asciz "   >> Fila vazia! Nada a remover.\n"
fmt_sep:      .asciz "----------------------------------------\n"
fmt_invalido: .asciz "   >> Opcao invalida.\n"

.section .bss
.align 3
head:      .quad 0
tail:      .quad 0
buf_opcao: .space 4
buf_valor: .space 4

.section .text
.global main

# insert: insere a0 no final da fila
insert:
    addi sp, sp, -32
    sd   ra, 24(sp)
    sd   s0, 16(sp)
    sd   s1, 8(sp)

    mv   s0, a0

    li   a0, 16
    call malloc
    mv   s1, a0             # s1 = endereço do novo nó

    sw   s0, 0(s1)          # node->value (32 bits)
    sd   zero, 8(s1)        # node->next = NULL (64 bits)

    la   t1, head
    ld   t2, 0(t1)
    bnez t2, .ins_cheio

    sd   s1, 0(t1)          # head = novo nó
    la   t1, tail
    sd   s1, 0(t1)          # tail = novo nó
    j    .ins_fim

.ins_cheio:
    la   t1, tail
    ld   t2, 0(t1)
    sd   s1, 8(t2)          # tail->next = novo nó
    sd   s1, 0(t1)          # tail = novo nó

.ins_fim:
    mv   a0, s1             # retorna endereço do nó

    ld   s1, 8(sp)
    ld   s0, 16(sp)
    ld   ra, 24(sp)
    addi sp, sp, 32
    ret

# remove_fila: remove do início da fila
#   Retorna: a0 = valor (-1 se vazia), a1 = endereço liberado
remove_fila:
    addi sp, sp, -32
    sd   ra, 24(sp)
    sd   s0, 16(sp)
    sd   s1, 8(sp)

    la   t0, head
    ld   s0, 0(t0)
    beqz s0, .rm_vazia      # head == NULL: fila vazia

    lw   s1, 0(s0)          # lê value (32 bits)

    ld   t1, 8(s0)          # head->next (64 bits)
    sd   t1, 0(t0)          # head = head->next

    bnez t1, .rm_free
    la   t2, tail
    sd   zero, 0(t2)        # tail = NULL (fila ficou vazia)

.rm_free:
    mv   t3, s0             # guarda endereço para retornar
    mv   a0, s0
    call free

    mv   a0, s1             # retorna valor
    mv   a1, t3             # retorna endereço liberado
    j    .rm_fim

.rm_vazia:
    li   a0, -1

.rm_fim:
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

    # Lê opção — recarrega endereço após call
    la   a0, fmt_scan_d
    la   a1, buf_opcao
    call scanf
    la   t0, buf_opcao      # recarrega endereço (a1 destruído pelo scanf)
    lw   s0, 0(t0)          # s0 = opção lida

    li   t1, 1
    beq  s0, t1, .op_insert

    li   t1, 2
    beq  s0, t1, .op_remove

    li   t1, 0
    beq  s0, t1, .sair

    la   a0, fmt_invalido
    call printf
    j    .menu_loop

.op_insert:
    la   a0, fmt_entrada
    call printf

    # Lê valor — recarrega endereço após call
    la   a0, fmt_scan_d
    la   a1, buf_valor
    call scanf
    la   t0, buf_valor      # recarrega endereço (a1 destruído)
    lw   s0, 0(t0)          # s0 = valor lido

    mv   a0, s0
    call insert
    mv   s1, a0             # s1 = endereço do nó alocado

    la   a0, fmt_insert
    mv   a1, s0             # %d  = valor inserido
    mv   a2, s1             # %lx = endereço do nó
    call printf

    la   a0, fmt_sep
    call printf
    j    .menu_loop

.op_remove:
    call remove_fila

    li   t0, -1
    beq  a0, t0, .fila_vazia

    mv   s0, a0             # s0 = valor removido
    mv   s1, a1             # s1 = endereço liberado

    la   a0, fmt_remove
    mv   a1, s0             # %d  = valor
    mv   a2, s1             # %lx = endereço
    call printf

    la   a0, fmt_sep
    call printf
    j    .menu_loop

.fila_vazia:
    la   a0, fmt_vazia
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
