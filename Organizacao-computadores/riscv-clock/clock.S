# Relogio Digital - RISC-V RV32IMA Bare Metal
# QEMU: qemu-system-riscv32 -machine virt -bios none -kernel clock.elf -nographic
# UART0 NS16550 : 0x10000000
# PLIC          : 0x0C000000  (hart0 M-mode)
# CLINT mtime   : 0x0200BFF8
# CLINT mtimecmp: 0x02004000
# RAM           : 0x80000000

.option norvc           # desabilita instrucoes RVC (16-bit comprimidas)

# UART NS16550 - offsets em BYTES a partir de UART_BASE
.equ UART_BASE,    0x10000000
.equ UART_RHR,     0           # Receive  Holding Register (leitura, DLAB=0)
.equ UART_THR,     0           # Transmit Holding Register (escrita, DLAB=0)
.equ UART_IER,     1           # Interrupt Enable Register (byte offset 1)
.equ UART_LSR,     5           # Line Status Register      (byte offset 5)
.equ LSR_DR,       0x01        # Bit 0: Data Ready (dado recebido)
.equ LSR_THRE,     0x20        # Bit 5: TX Holding Empty

# PLIC - enderecos absolutos para hart0 M-mode
# UART0 = IRQ 10 no QEMU virt
.equ PLIC_PRIORITY, 0x0C000000  # base: reg de prioridade da IRQ N = base + N*4
.equ PLIC_ENABLE,   0x0C002000  # habilita IRQs 0-31 do hart0 M-mode
.equ PLIC_THRESHOLD,0x0C200000  # threshold do hart0 M-mode
.equ PLIC_CLAIM,    0x0C200004  # claim/complete do hart0 M-mode
.equ UART_IRQ,      10

# CLINT - enderecos absolutos (GNU AS nao aceita CONST+offset em lw/sw)
.equ MTIME_LO,     0x0200BFF8
.equ MTIME_HI,     0x0200BFFC
.equ MTIMECMP_LO,  0x02004000
.equ MTIMECMP_HI,  0x02004004
.equ TIMER_HZ,     10000000    # 10 MHz = 1 segundo

# mcause values para RV32 (bit 31 = interrupt)
.equ CAUSE_TIMER,    0x80000007
.equ CAUSE_EXTERNAL, 0x8000000B

# CSR bits
.equ MIE_MTIE,    (1 << 7)
.equ MIE_MEIE,    (1 << 11)
.equ MSTATUS_MIE, (1 << 3)

# PILHA - em .bss; stack_top e o label APOS o espaco (topo da pilha)
.section .bss
.align 4
.space 4096
stack_top:

# .data
.section .data
.align 2

g_horas:    .byte 0
g_minutos:  .byte 0
g_segundos: .byte 0
.align 2

g_rxbuf:    .space 16   # buffer de recepcao de comandos
g_rxlen:    .byte 0     # quantidade de bytes no buffer
.align 2

str_banner:
    .ascii "\r\n=== Relogio Digital RISC-V Bare Metal ===\r\n"
    .ascii "Ajuste: T HH:MM:SS  (ex: T 14:30:00)\r\n"
    .ascii "Sair  : Ctrl-A, depois X\r\n\0"
str_ok:
    .ascii "\r\nHora ajustada!\r\n\0"
str_err:
    .ascii "\r\nERRO: use T HH:MM:SS\r\n\0"

.section .text
.global _start

# _start
_start:

    # PASSO 1: pilha
    la      sp, stack_top

    # PASSO 2: trap handler
    la      t0, trap_handler
    csrw    mtvec, t0

    # PASSO 3: UART - habilita interrupcao de dado recebido (bit 0 do IER)
    li      t0, UART_BASE
    li      t1, 1
    sb      t1, UART_IER(t0)

    # PASSO 4: PLIC - seta prioridade da IRQ 10 (UART0) para 1
    li      t0, PLIC_PRIORITY
    li      t1, UART_IRQ
    slli    t1, t1, 2           # offset = IRQ * 4
    add     t0, t0, t1
    li      t2, 1
    sw      t2, 0(t0)

    # PASSO 5: PLIC - habilita IRQ 10 no enable register
    li      t0, PLIC_ENABLE
    li      t1, (1 << UART_IRQ)
    sw      t1, 0(t0)

    # PASSO 6: PLIC - threshold 0 (aceita qualquer prioridade >= 1)
    li      t0, PLIC_THRESHOLD
    sw      zero, 0(t0)

    # PASSO 7: arma o timer para disparar em 1 segundo
    jal     timer_set

    # PASSO 8: habilita Machine Timer + Machine External Interrupt
    li      t0, (MIE_MTIE | MIE_MEIE)
    csrs    mie, t0

    # PASSO 9: habilita interrupcoes globais
    li      t0, MSTATUS_MIE
    csrs    mstatus, t0

    # PASSO 10: exibe banner e hora inicial (00:00:00)
    la      a0, str_banner
    jal     uart_puts
    jal     display_time

# Loop principal - wfi economiza CPU aguardando interrupcoes
main_loop:
    wfi
    j       main_loop

# timer_set - programa mtimecmp = mtime_atual + TIMER_HZ
# Leitura de mtime em 64-bit com protecao contra carry entre words
timer_set:

    li      t0, MTIME_LO
    li      t1, MTIME_HI

ts_retry:
    lw      t3, 0(t1)           # mtime HIGH (primeira leitura)
    lw      t2, 0(t0)           # mtime LOW
    lw      t4, 0(t1)           # mtime HIGH (segunda leitura)
    bne     t3, t4, ts_retry    # carry ocorreu, tenta de novo

    # new_cmp = mtime + TIMER_HZ
    li      t4, TIMER_HZ
    add     t5, t2, t4          # cmp_lo = mtime_lo + hz
    sltu    t4, t5, t2          # carry out do low
    add     t4, t3, t4          # cmp_hi = mtime_hi + carry

    # Escrita atomica: HIGH = MAX antes de escrever LOW (evita disparo espurio)
    li      t0, MTIMECMP_LO
    li      t1, MTIMECMP_HI
    li      t6, 0x7FFFFFFF
    sw      t6, 0(t1)           # HIGH temporario = MAX
    sw      t5, 0(t0)           # LOW definitivo
    sw      t4, 0(t1)           # HIGH definitivo

    jr      ra

# trap_handler - vetor unico, trata todas as interrupcoes
.align 4
trap_handler:

    # Salva contexto completo: caller-saved + s0/s1/s2 (usados em subfuncoes)
    addi    sp, sp, -80
    sw      ra,   0(sp)
    sw      t0,   4(sp)
    sw      t1,   8(sp)
    sw      t2,  12(sp)
    sw      t3,  16(sp)
    sw      t4,  20(sp)
    sw      t5,  24(sp)
    sw      t6,  28(sp)
    sw      a0,  32(sp)
    sw      a1,  36(sp)
    sw      a2,  40(sp)
    sw      a3,  44(sp)
    sw      a4,  48(sp)
    sw      a5,  52(sp)
    sw      a6,  56(sp)
    sw      a7,  60(sp)
    sw      s0,  64(sp)
    sw      s1,  68(sp)
    sw      s2,  72(sp)

    # Identifica origem e despacha
    csrr    t0, mcause

    li      t1, CAUSE_TIMER
    beq     t0, t1, isr_timer

    li      t1, CAUSE_EXTERNAL
    beq     t0, t1, isr_external

    j       trap_exit           # excecao sincrona ou IRQ desconhecida: ignora

# ISR Timer
isr_timer:
    jal     timer_set           # reprograma mtimecmp (+1s)
    jal     tick_second         # incrementa HH:MM:SS
    jal     display_time        # exibe hora atualizada na UART
    j       trap_exit

# ISR External (PLIC / UART)
isr_external:

    # Claim: descobre qual IRQ disparou; salva ID em s0 para usar no complete
    li      t0, PLIC_CLAIM
    lw      s0, 0(t0)           # s0 = IRQ ID (preservado ate o complete)

    li      t1, UART_IRQ
    bne     s0, t1, isr_done    # nao e UART, vai direto pro complete

    # Le byte da UART
    li      t0, UART_BASE
    lb      s1, UART_RHR(t0)    # s1 = byte recebido (preservado)

    # Eco do caractere
    mv      a0, s1
    jal     uart_putc

    # CR ou LF = fim de comando
    li      t0, '\r'
    beq     s1, t0, isr_eol
    li      t0, '\n'
    beq     s1, t0, isr_eol

    # Guarda no buffer (limite: 15 caracteres uteis)
    la      t0, g_rxlen
    lbu     t1, 0(t0)
    li      t2, 15
    bge     t1, t2, isr_done    # buffer cheio: descarta

    la      t2, g_rxbuf
    add     t2, t2, t1
    sb      s1, 0(t2)
    addi    t1, t1, 1
    la      t0, g_rxlen
    sb      t1, 0(t0)
    j       isr_done

isr_eol:
    # Null-termina o buffer e processa o comando
    la      t0, g_rxlen
    lbu     t1, 0(t0)
    la      t2, g_rxbuf
    add     t2, t2, t1
    sb      zero, 0(t2)

    jal     parse_command       # interpreta "T HH:MM:SS"

    # Reseta buffer
    la      t0, g_rxlen
    sb      zero, 0(t0)

isr_done:
    # Complete: devolve IRQ ao PLIC usando o ID salvo em s0
    li      t0, PLIC_CLAIM
    sw      s0, 0(t0)

trap_exit:
    lw      ra,   0(sp)
    lw      t0,   4(sp)
    lw      t1,   8(sp)
    lw      t2,  12(sp)
    lw      t3,  16(sp)
    lw      t4,  20(sp)
    lw      t5,  24(sp)
    lw      t6,  28(sp)
    lw      a0,  32(sp)
    lw      a1,  36(sp)
    lw      a2,  40(sp)
    lw      a3,  44(sp)
    lw      a4,  48(sp)
    lw      a5,  52(sp)
    lw      a6,  56(sp)
    lw      a7,  60(sp)
    lw      s0,  64(sp)
    lw      s1,  68(sp)
    lw      s2,  72(sp)
    addi    sp, sp, 80

    mret

# tick_second - incrementa SS -> MM -> HH com wraparound correto
tick_second:
    addi    sp, sp, -4
    sw      ra, 0(sp)

    la      t0, g_segundos
    lbu     t1, 0(t0)
    addi    t1, t1, 1
    li      t2, 60
    blt     t1, t2, ts_seg
    li      t1, 0

    la      t0, g_minutos
    lbu     t2, 0(t0)
    addi    t2, t2, 1
    li      t3, 60
    blt     t2, t3, ts_min
    li      t2, 0

    la      t0, g_horas
    lbu     t3, 0(t0)
    addi    t3, t3, 1
    li      t4, 24
    blt     t3, t4, ts_hora
    li      t3, 0

ts_hora:
    la      t0, g_horas
    sb      t3, 0(t0)
    la      t0, g_minutos

ts_min:
    sb      t2, 0(t0)
    la      t0, g_segundos

ts_seg:
    sb      t1, 0(t0)

    lw      ra, 0(sp)
    addi    sp, sp, 4
    jr      ra

# display_time - envia "\rHH:MM:SS  " pela UART
display_time:
    addi    sp, sp, -4
    sw      ra, 0(sp)

    li      a0, '\r'
    jal     uart_putc

    la      t0, g_horas
    lbu     a0, 0(t0)
    jal     dec2

    li      a0, ':'
    jal     uart_putc

    la      t0, g_minutos
    lbu     a0, 0(t0)
    jal     dec2

    li      a0, ':'
    jal     uart_putc

    la      t0, g_segundos
    lbu     a0, 0(t0)
    jal     dec2

    li      a0, ' '
    jal     uart_putc
    li      a0, ' '
    jal     uart_putc

    lw      ra, 0(sp)
    addi    sp, sp, 4
    jr      ra

# dec2 - imprime a0 (0-99) como dois digitos ASCII
dec2:
    addi    sp, sp, -8
    sw      ra, 0(sp)
    sw      s2, 4(sp)           # usa s2 para preservar valor durante as chamadas

    mv      s2, a0
    li      t0, 10
    div     t1, s2, t0          # dezena
    rem     t2, s2, t0          # unidade

    li      t3, '0'
    add     a0, t1, t3
    jal     uart_putc

    li      t3, '0'
    add     a0, t2, t3
    jal     uart_putc

    lw      ra, 0(sp)
    lw      s2, 4(sp)
    addi    sp, sp, 8
    jr      ra

# parse_command - interpreta "T HH:MM:SS" de g_rxbuf
# Usa s0=HH, s1=MM, s2=SS (callee-saved, preservados no trap_handler)
parse_command:
    addi    sp, sp, -16
    sw      ra,  0(sp)
    sw      s0,  4(sp)
    sw      s1,  8(sp)
    sw      s2, 12(sp)

    la      t0, g_rxbuf

    # Posicao 0: 'T'
    lbu     t1, 0(t0)
    li      t2, 'T'
    bne     t1, t2, pc_err

    # Posicao 1: espaco
    lbu     t1, 1(t0)
    li      t2, ' '
    bne     t1, t2, pc_err

    # Posicoes 2,3: HH
    lbu     a0, 2(t0)
    lbu     a1, 3(t0)
    jal     p2d
    li      t1, -1
    beq     a0, t1, pc_err
    li      t1, 24
    bge     a0, t1, pc_err
    mv      s0, a0

    # Posicao 4: ':'
    la      t0, g_rxbuf
    lbu     t1, 4(t0)
    li      t2, ':'
    bne     t1, t2, pc_err

    # Posicoes 5,6: MM
    lbu     a0, 5(t0)
    lbu     a1, 6(t0)
    jal     p2d
    li      t1, -1
    beq     a0, t1, pc_err
    li      t1, 60
    bge     a0, t1, pc_err
    mv      s1, a0

    # Posicao 7: ':'
    la      t0, g_rxbuf
    lbu     t1, 7(t0)
    li      t2, ':'
    bne     t1, t2, pc_err

    # Posicoes 8,9: SS
    lbu     a0, 8(t0)
    lbu     a1, 9(t0)
    jal     p2d
    li      t1, -1
    beq     a0, t1, pc_err
    li      t1, 60
    bge     a0, t1, pc_err
    mv      s2, a0

    # Aplica valores (desabilita IRQs para escrita atomica)
    li      t0, MSTATUS_MIE
    csrc    mstatus, t0

    la      t0, g_horas
    sb      s0, 0(t0)
    la      t0, g_minutos
    sb      s1, 0(t0)
    la      t0, g_segundos
    sb      s2, 0(t0)

    li      t0, MSTATUS_MIE
    csrs    mstatus, t0

    la      a0, str_ok
    jal     uart_puts
    jal     display_time
    j       pc_fim

pc_err:
    la      a0, str_err
    jal     uart_puts

pc_fim:
    lw      ra,  0(sp)
    lw      s0,  4(sp)
    lw      s1,  8(sp)
    lw      s2, 12(sp)
    addi    sp, sp, 16
    jr      ra

# p2d - converte dois chars ASCII em inteiro (0-99) ou -1 se invalido
# a0 = char dezena, a1 = char unidade -> a0 = valor
p2d:
    li      t0, '0'
    li      t1, '9'
    blt     a0, t0, p2d_err
    bgt     a0, t1, p2d_err
    blt     a1, t0, p2d_err
    bgt     a1, t1, p2d_err
    sub     t2, a0, t0          # dezena numerica
    sub     t3, a1, t0          # unidade numerica
    li      t4, 10
    mul     t2, t2, t4
    add     a0, t2, t3
    jr      ra

p2d_err:
    li      a0, -1
    jr      ra

# uart_putc - envia byte em a0 aguardando TX vazio (polling em LSR)
uart_putc:
    li      t0, UART_BASE

uart_tx_wait:
    lbu     t1, UART_LSR(t0)
    andi    t1, t1, LSR_THRE
    beqz    t1, uart_tx_wait

    sb      a0, UART_THR(t0)
    jr      ra

# uart_puts - envia string null-terminated apontada por a0
uart_puts:
    addi    sp, sp, -8
    sw      ra, 0(sp)
    sw      s0, 4(sp)

    mv      s0, a0

uart_puts_loop:
    lbu     a0, 0(s0)
    beqz    a0, uart_puts_done
    jal     uart_putc
    addi    s0, s0, 1
    j       uart_puts_loop

uart_puts_done:
    lw      ra, 0(sp)
    lw      s0, 4(sp)
    addi    sp, sp, 8
    jr      ra