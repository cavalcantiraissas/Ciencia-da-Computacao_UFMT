.extern printf

.section .data
fmt_titulo:  .asciz "\n=== Pilha Dinamica no Heap com brk ===\n"
fmt_lifo:    .asciz "LIFO: push 10, 20, 30  =>  pop esperado: 30, 20, 10\n\n"
fmt_sep:     .asciz "----------------------------------------\n"
fmt_push:    .asciz "PUSH(%d)  brk: 0x%lx -> 0x%lx  (+4 bytes alocados)\n"
fmt_pop:     .asciz "POP() = %d  brk: 0x%lx -> 0x%lx  (-4 bytes liberados)\n"

.section .text
.global main

get_brk:
    li   a7, 214
    li   a0, 0
    ecall
    ret

push:
    addi sp, sp, -32
    sd   ra, 24(sp)
    sd   s0, 16(sp)
    sd   s1, 8(sp)
    mv   s0, a0
    li   a7, 214
    li   a0, 0
    ecall
    mv   s1, a0
    addi a0, s1, 4
    li   a7, 214
    ecall
    sw   s0, 0(s1)
    ld   s1, 8(sp)
    ld   s0, 16(sp)
    ld   ra, 24(sp)
    addi sp, sp, 32
    ret

pop:
    addi sp, sp, -32
    sd   ra, 24(sp)
    sd   s0, 16(sp)
    sd   s1, 8(sp)
    li   a7, 214
    li   a0, 0
    ecall
    mv   s0, a0
    addi t0, s0, -4
    lw   s1, 0(t0)
    mv   a0, t0
    li   a7, 214
    ecall
    mv   a0, s1
    ld   s1, 8(sp)
    ld   s0, 16(sp)
    ld   ra, 24(sp)
    addi sp, sp, 32
    ret

demo_push:
    addi sp, sp, -48
    sd   ra, 40(sp)
    sd   s0, 32(sp)
    sd   s1, 24(sp)
    sd   s2, 16(sp)
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

demo_pop:
    addi sp, sp, -48
    sd   ra, 40(sp)
    sd   s0, 32(sp)
    sd   s1, 24(sp)
    sd   s2, 16(sp)
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
