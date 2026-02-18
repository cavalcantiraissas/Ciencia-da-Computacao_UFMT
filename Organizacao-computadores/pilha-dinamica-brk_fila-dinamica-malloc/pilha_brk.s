# pilha_brk.s
# Parte 1: Pilha Dinâmica no Heap usando syscall brk
# Linguagem: Assembly RISC-V 64-bit | Compilar com: riscv64-linux-gnu-gcc
#
# CONCEITO DO brk:
#   brk(0)          -> retorna o break atual (sem modificar)
#   brk(novo_end)   -> move o break para novo_end
#   Syscall número 214 no Linux RISC-V 64-bit.
#
#   LIFO na prática:
#     push: break += 4  (aloca), grava valor no break antigo
#     pop:  lê valor em break-4, break -= 4  (libera)
# Compilar:
#   riscv64-linux-gnu-gcc -o pilha_brk pilha_brk.s
# Executar:
#   qemu-riscv64 ./pilha_brk

.extern printf          # Printf da libc para saída formatada
.section .data # String de formato para printf

fmt_titulo:  .asciz "\n Pilha Dinamica no Heap com brk\n"
fmt_lifo:    .asciz "LIFO: push 10, 20, 30  =>  pop esperado: 30, 20, 10\n\n"
fmt_push:    .asciz "PUSH(%d)  brk: 0x%lx -> 0x%lx  (+4 bytes alocados)\n"
fmt_pop:     .asciz "POP() = %d  brk: 0x%lx -> 0x%lx  (-4 bytes liberados)\n"

# Código
.section .text
.global main

# get_brk: retorna o program break atual em a0
# Chama brk(0) via ecall — o kernel devolve o break sem modificar nada
get_brk:
    li   a7, 214        # número da syscall brk no Linux RISC-V 64-bit
    li   a0, 0          # argumento 0 = apenas consulta, não modifica
    ecall               # após ecall, a0 = break atual
    ret

# push: empilha o inteiro em a0 no heap
#     1. brk(0) -> obtém break atual (onde o heap termina)
#     2. brk(break + 4) -> move o break +4 bytes (aloca 1 inteiro de 32 bits)
#     3. sw valor, 0(break_antigo) -> grava o inteiro no espaço recém-alocado
#   O topo da pilha SEMPRE coincide com o intervalo [break-4 .. break-1]
push:
    addi sp, sp, -32
    sd   ra, 24(sp)
    sd   s0, 16(sp)     # s0 = valor a empilhar
    sd   s1, 8(sp)      # s1 = break antigo (onde vamos escrever)

    mv   s0, a0         # salva o valor

    # passo 1: consulta o break atual
    li   a7, 214
    li   a0, 0
    ecall
    mv   s1, a0         # s1 = break antes do push

    # passo 2: move o break +4 (aloca espaço para 1 inteiro de 32 bits)
    addi a0, s1, 4      # novo_break = break_atual + 4
    li   a7, 214
    ecall               # kernel atualiza o program break

    # passo 3: grava o valor no endereço antigo do break
    sw   s0, 0(s1)      # sw = store word (32 bits)

    ld   s1, 8(sp)
    ld   s0, 16(sp)
    ld   ra, 24(sp)
    addi sp, sp, 32
    ret

# pop: desempilha o topo e retorna o valor em a0
#     1. brk(0) -> obtém break atual
#     2. lw valor, 0(break - 4) -> lê o inteiro no topo da pilha
#     3. brk(break - 4) -> move o break -4 bytes (libera o espaço do topo)
#
#   A liberação via brk é válida SOMENTE por causa do LIFO:
#   sempre removemos o ÚLTIMO elemento inserido.
pop:
    addi sp, sp, -32
    sd   ra, 24(sp)
    sd   s0, 16(sp)     # s0 = break antes do pop
    sd   s1, 8(sp)      # s1 = valor desempilhado

    #consulta o break atual
    li   a7, 214
    li   a0, 0
    ecall
    mv   s0, a0         # s0 = break antes

    #lê o valor no topo (break - 4)
    addi t0, s0, -4     # t0 = endereço do topo = break - 4
    lw   s1, 0(t0)      # lê 32 bits do topo

    #libera os 4 bytes (move break -4)
    mv   a0, t0         # novo_break = break - 4
    li   a7, 214
    ecall

    mv   a0, s1         # retorna o valor desempilhado

    ld   s1, 8(sp)
    ld   s0, 16(sp)
    ld   ra, 24(sp)
    addi sp, sp, 32
    ret

# demo_push: faz push de a0 e imprime o estado do heap com printf
demo_push:
    addi sp, sp, -48
    sd   ra, 40(sp)
    sd   s0, 32(sp)     # s0 = valor
    sd   s1, 24(sp)     # s1 = break ANTES
    sd   s2, 16(sp)     # s2 = break DEPOIS

    mv   s0, a0

    call get_brk
    mv   s1, a0

    mv   a0, s0
    call push

    call get_brk
    mv   s2, a0

    la   a0, fmt_push
    mv   a1, s0
    mv   a2, s1
    mv   a3, s2
    call printf

    ld   s2, 16(sp)
    ld   s1, 24(sp)
    ld   s0, 32(sp)
    ld   ra, 40(sp)
    addi sp, sp, 48
    ret

# demo_pop: faz pop e imprime o valor e o estado do heap com printf
demo_pop:
    addi sp, sp, -48
    sd   ra, 40(sp)
    sd   s0, 32(sp)     # s0 = break ANTES
    sd   s1, 24(sp)     # s1 = valor desempilhado
    sd   s2, 16(sp)     # s2 = break DEPOIS

    call get_brk
    mv   s0, a0

    call pop
    mv   s1, a0

    call get_brk
    mv   s2, a0

    la   a0, fmt_pop
    mv   a1, s1
    mv   a2, s0
    mv   a3, s2
    call printf

    ld   s2, 16(sp)
    ld   s1, 24(sp)
    ld   s0, 32(sp)
    ld   ra, 40(sp)
    addi sp, sp, 48
    ret

# ponto de entrada
main:
    addi sp, sp, -16
    sd   ra, 8(sp)

    la   a0, fmt_titulo
    call printf

    la   a0, fmt_lifo
    call printf

    la   a0, fmt_sep
    call printf

    li   a0, 10
    call demo_push

    li   a0, 20
    call demo_push

    li   a0, 30
    call demo_push

    la   a0, fmt_sep
    call printf

    call demo_pop
    call demo_pop
    call demo_pop

    la   a0, fmt_sep
    call printf

    li   a0, 0
    ld   ra, 8(sp)
    addi sp, sp, 16
    ret
