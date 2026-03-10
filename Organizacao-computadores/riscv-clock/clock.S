# Relogio Digital - RISC-V RV32IMA Bare Metal
# QEMU: qemu-system-riscv32 -machine virt -bios none -kernel clock.elf -nographic
# UART0 NS16550 : 0x10000000
# PLIC          : 0x0C000000 
# CLINT mtime   : 0x0200BFF8
# CLINT mtimecmp: 0x02004000
# RAM           : 0x80000000


# NOTA TÉCNICA: MANIPULAÇÃO ATÔMICA DE 64 BITS EM ARQUITETURA DE 32 BITS (RV32)
# Como o mtime e mtimecmp são registradores de 64 bits e o barramento é de 32,
# a leitura e escrita ocorrem em duas etapas, o que exige proteção:
#
#    LEITURA SEGURA (Loop ts_retry):
#    Evita a leitura de um valor inconsistente caso ocorra um "carry" 
#    da parte baixa (MTIME_LO) para a parte alta (MTIME_HI) durante o processo.
#    O código lê a parte alta duas vezes; se o valor mudar, a leitura é repetida.
.option norvc          

# UART NS16550 - offsets em BYTES a partir de UART_BASE
.equ UART_BASE,    0x10000000  # Porta da UART - Começa no mapa de memória
.equ UART_RHR,     0           # Leitura - busca o caractere digitado
.equ UART_THR,     0           # Escrita - código coloca o caractere que quer enviar para tela
.equ UART_IER,     1           # Interruptor - gerar interrupçãoo sempre que chegar letra nova
.equ UART_LSR,     5           # Sinalizador de estado
.equ LSR_DR,       0x01        # Conferir se há dado para ser lido
.equ LSR_THRE,     0x20        # Verificar se UART terminou de enviar o caractere

# PLIC - "Gerente central" de interrupções - Recebe pedidos externos e organiza
# UART0 = IRQ 10 no QEMU virt
.equ PLIC_PRIORITY, 0x0C000000  # Nivel de importancia de cada interrupcao
.equ PLIC_ENABLE,   0x0C002000  # Chave para cada fonte de interrupção 
.equ PLIC_THRESHOLD,0x0C200000  # Nível de urgência para interromper o processador
.equ PLIC_CLAIM,    0x0C200004  # claim/complete
.equ UART_IRQ,      10

# CLINT - Gerar interrupções no timer || Relogio de pulso interno do processador
.equ MTIME_LO,     0x0200BFF8 # Parte baixa
.equ MTIME_HI,     0x0200BFFC # Parte alta
.equ MTIMECMP_LO,  0x02004000 # "Despertador" do processador
.equ MTIMECMP_HI,  0x02004004 # MTIME = MTIMECMP hardware dispara
.equ TIMER_HZ,     10000000    # 10 MHz = 1 segundo

# mcause values para RV32 (bit 31 = interrupt) - Painel de identificacao do processador
.equ CAUSE_TIMER,    0x80000007 # Tempo acabou - Vem do Clint
.equ CAUSE_EXTERNAL, 0x8000000B # Vem do plic

# CSR bits - Interruptores
.equ MIE_MTIE,    (1 << 7)  # Interrompe o processador a cada 1s
.equ MIE_MEIE,    (1 << 11) # Permite que disp. ext. interrompam o processador
.equ MSTATUS_MIE, (1 << 3) # "Chave Geral" -> Se estiver desligada, nada funciona.

# Sistema configura. Liga os bits 7 e 11 em MIE e o 3 em mstatus. 

# Organização da memória RAM
# PILHA 
.section .bss
.align 4
.space 4096 # Reserva 4kb de memória
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


# Garantir inicialização segura e odernada. Infraestrutura da memória, como lidar com eventos,
# configuração dos periféricos (UART/PLIC) e ligação das interrupções globais
_start:

    # PASSO 1: pilha - processador "anota" coisas temporariamente
    la      sp, stack_top # Aponta o sp para o topo da memória

    # PASSO 2: trap handler
    la      t0, trap_handler 
    csrw    mtvec, t0 #Escrita em registrador de controle e status

    # PASSO 3: UART - habilita interrupcao de dado recebido 
    li      t0, UART_BASE
    li      t1, 1
    sb      t1, UART_IER(t0) 

    # PASSO 4: PLIC - seta prioridade da IRQ 10 (UART0) para 1
    li      t0, PLIC_PRIORITY
    li      t1, UART_IRQ
    slli    t1, t1, 2    # cada prioridade ocupa 4bytes (10x4)       
    add     t0, t0, t1 # Deslocamento do endereço base
    li      t2, 1 
    sw      t2, 0(t0) # Prioridade nivel 1

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
    csrs    mie, t0 # Permite ser interrompido por esses dois

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
    addi    sp, sp, -80 # Reserva 80 bytes na pilha (20 regisytadores x 4 bytes cada) || Quando a interrupção termina, o processador pod restaurar exatamente o estado em que estava
    sw      ra,   0(sp) 
    sw      t0,   4(sp)
    sw      t1,   8(sp) # Salva registradores temporários (t0-t6)
    sw      t2,  12(sp)
    sw      t3,  16(sp)
    sw      t4,  20(sp)
    sw      t5,  24(sp)
    sw      t6,  28(sp)
    sw      a0,  32(sp) # Salva registradores de argumento (a0-a7)
    sw      a1,  36(sp)
    sw      a2,  40(sp)
    sw      a3,  44(sp)
    sw      a4,  48(sp)
    sw      a5,  52(sp)
    sw      a6,  56(sp)
    sw      a7,  60(sp)
    sw      s0,  64(sp) # Salva registradores salvos (s0-s2) usados nas subfunções
    sw      s1,  68(sp)
    sw      s2,  72(sp)

    # Identifica origem e despacha
    csrr    t0, mcause # Leitura de Registrador de controle e status

    li      t1, CAUSE_TIMER # Se for o Timer, desvia para a rotina do relógio
    beq     t0, t1, isr_timer 

    li      t1, CAUSE_EXTERNAL # Se for externo (teclado), desvia para rotina UART
    beq     t0, t1, isr_external

    j       trap_exit           # excecao sincrona ou IRQ desconhecida: ignora


# Rotinas de serviço de interrupção
# ISR Timer - Sempre que 1 segundo se passa no hardware, o processador vem pra cá
isr_timer:
    jal     timer_set           # reprograma mtimecmp (+1s)
    jal     tick_second         # incrementa HH:MM:SS
    jal     display_time        # exibe hora atualizada na UART
    j       trap_exit

# ISR External (PLIC / UART) - Lida com a entrada de dados do usuário
isr_external:

    # Claim: descobre qual IRQ disparou; salva ID em s0 para usar no complete
    li      t0, PLIC_CLAIM
    lw      s0, 0(t0)           # Reivindica a interrupção no PLIC


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

trap_exit: # Recupera todos os 20 registradores da pilha
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
    addi    sp, sp, 80 # Libera o espaço da pilha

    mret # Volta para o programa principal

# tick_second - incrementa SS -> MM -> HH 
tick_second:
    addi    sp, sp, -4 # Reserva espaço na pilha
    sw      ra, 0(sp) # Salva o endereço de retorno

# Logica de Segundos
    la      t0, g_segundos 
    lbu     t1, 0(t0) # Carrega segundos atuais
    addi    t1, t1, 1 # Incrementa + 1
    li      t2, 60
    blt     t1, t2, ts_seg # Se <60, pula para salva
    li      t1, 0 # Se = 60, zera segundos e continua para minutos

# Logica de Minutos
    la      t0, g_minutos
    lbu     t2, 0(t0)
    addi    t2, t2, 1
    li      t3, 60
    blt     t2, t3, ts_min # Se< 60, pula para salvar
    li      t2, 0 # Se = 60, zera minutos e continua para horas
# Logica de Horas
    la      t0, g_horas
    lbu     t3, 0(t0)
    addi    t3, t3, 1
    li      t4, 24
    blt     t3, t4, ts_hora # Se <24, pula pra salvar
    li      t3, 0 # Se = 24, zera horas (virada do dia)

ts_hora:
    la      t0, g_horas
    sb      t3, 0(t0) # Salva hora final
    la      t0, g_minutos

ts_min:
    sb      t2, 0(t0) # Salva minuto final
    la      t0, g_segundos

ts_seg:
    sb      t1, 0(t0) # Salva segundo final

    lw      ra, 0(sp) # Restaura ra
    addi    sp, sp, 4 # Libera pilha
    jr      ra # Retorna

# display_time - envia "\rHH:MM:SS  " pela UART
display_time:
    addi    sp, sp, -4
    sw      ra, 0(sp)

    li      a0, '\r' # Caractere 'Carriage Return':  move o cursosr para o inicio da linha
    jal     uart_putc # Sem isso, o relógio imprimiria uma linha nova a cada segundo

    la      t0, g_horas # Carrega o valor númerico de horas
    lbu     a0, 0(t0)
    jal     dec2 # Converte o número 2 caractere ASCII (Ex: 9 -> "09")

    li      a0, ':' # Separador visual
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

# dec2 - imprime a0 (0-99) como dois digitos ASCII e envia à UART
dec2:
    addi    sp, sp, -8
    sw      ra, 0(sp)
    sw      s2, 4(sp)           # usa s2 para preservar valor original de a0 durante as chamadas

    mv      s2, a0
    li      t0, 10
    div     t1, s2, t0          # t1 = Dezena (ex: 14/10 = 1)
    rem     t2, s2, t0          # t2 = Unidade (ex: 14%10 = 4)

    li      t3, '0' # Base ACII para números (48)
    add     a0, t1, t3 # Converte dígito em caractere 
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
    sw      s0,  4(sp) # Guardara a nova hora
    sw      s1,  8(sp) # Guardara o novo minuto
    sw      s2, 12(sp) # Guardara o novo segundo

    la      t0, g_rxbuf # Aponta para o buffer onde UART salvou a digitacao

    # Posicao 0: 'T' - O comando deve começar com "T "
    lbu     t1, 0(t0)
    li      t2, 'T'
    bne     t1, t2, pc_err # Se não, erro

    # Posicao 1: espaco
    lbu     t1, 1(t0)
    li      t2, ' '
    bne     t1, t2, pc_err # Se não tiver espaço, erro

    # Posicoes 2,3: HH
    lbu     a0, 2(t0)
    lbu     a1, 3(t0)
    jal     p2d # Converte os 2 caracteres em um número inteiro
    li      t1, -1
    beq     a0, t1, pc_err # Retorna -1 se os caracteres não forem números
    li      t1, 24
    bge     a0, t1, pc_err # Hora dev ser < 24
    mv      s0, a0

    # Posicao 4: ':'- Valida primeiro separador
    la      t0, g_rxbuf
    lbu     t1, 4(t0)
    li      t2, ':'
    bne     t1, t2, pc_err

    # Posicoes 5,6: MM - Processa minutos
    lbu     a0, 5(t0)
    lbu     a1, 6(t0)
    jal     p2d
    li      t1, -1
    beq     a0, t1, pc_err
    li      t1, 60 # Minuto deve ser < 60
    bge     a0, t1, pc_err
    mv      s1, a0

    # Posicao 7: ':' - Valida segundo separador
    la      t0, g_rxbuf
    lbu     t1, 7(t0)
    li      t2, ':'
    bne     t1, t2, pc_err

    # Posicoes 8,9: SS 0 processa segundos
    lbu     a0, 8(t0)
    lbu     a1, 9(t0)
    jal     p2d
    li      t1, -1
    beq     a0, t1, pc_err
    li      t1, 60 
    bge     a0, t1, pc_err # Segundo deve ser < 60
    mv      s2, a0

 # Desabilita interrupções globais para garantir que o relógio não 
 # tente incrementar o tempo enquanto estamos alterando as variáveis.
    li      t0, MSTATUS_MIE
    csrc    mstatus, t0

    la      t0, g_horas
    sb      s0, 0(t0)
    la      t0, g_minutos
    sb      s1, 0(t0)
    la      t0, g_segundos
    sb      s2, 0(t0)

# Reabilita interrupções 
    li      t0, MSTATUS_MIE
    csrs    mstatus, t0

    la      a0, str_ok # Mensagem de sucesso
    jal     uart_puts
    jal     display_time
    j       pc_fim

pc_err:
    la      a0, str_err # Mensagem de Erro
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
    # Verifica se ambos os caracteres estão no intervalo 0-9 
    blt     a0, t0, p2d_err
    bgt     a0, t1, p2d_err
    blt     a1, t0, p2d_err
    bgt     a1, t1, p2d_err
    sub     t2, a0, t0          # dezena numerica
    sub     t3, a1, t0          # unidade numerica
    li      t4, 10
    mul     t2, t2, t4 # Multiplica dezena por 10
    add     a0, t2, t3 # Soma com a unidade
    jr      ra

p2d_err:
    li      a0, -1 # Retorna erro
    jr      ra

# uart_putc - Envia UM byte para a UART (Transmissão)
uart_putc:
    li      t0, UART_BASE

uart_tx_wait:
# Polling: Lê o line status register para ver se o buffer de saída está vazio
    lbu     t1, UART_LSR(t0)
    andi    t1, t1, LSR_THRE #Verifica o bit de "Trasmitter Holding Register Empty"
    beqz    t1, uart_tx_wait #Se estiver ocupado, espera em loop

    sb      a0, UART_THR(t0) # Envia o caractere para o Hardware
    jr      ra

# uart_puts - envia string null-terminated apontada por a0
uart_puts:
    addi    sp, sp, -8
    sw      ra, 0(sp)
    sw      s0, 4(sp)

    mv      s0, a0 # Salva endereço da string em s0

uart_puts_loop:
    lbu     a0, 0(s0) # Carrega próximo caractere
    beqz    a0, uart_puts_done # Se for zero (null terminator), fim da string
    jal     uart_putc # Envia caractere via UART
    addi    s0, s0, 1 # Avança para o próximo endereço
    j       uart_puts_loop

uart_puts_done:
    lw      ra, 0(sp)
    lw      s0, 4(sp)
    addi    sp, sp, 8
    jr      ra
