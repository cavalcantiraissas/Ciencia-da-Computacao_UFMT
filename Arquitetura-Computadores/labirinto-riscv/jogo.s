# =============================================================================
# Labirinto em Assembly RISC-V (RV32I) para o emulador Emulsiv
# Disciplina: Arquitetura de Computadores - UFMT
#
# Mapa de 32 x 32 pixels na área de bitmap (1 byte por pixel, a partir de
# __bitmap_start = 0xC00). O jogador anda com W, A, S e D; tocar numa parede
# encerra a partida. A pontuação é o número de movimentos.
#
# Dispositivos (E/S mapeada em memória):
#   0xB0000000  teclado - status (bit 6 = tecla disponível; o jogo zera após ler)
#   0xB0000001  teclado - código ASCII da tecla
#   0xC0000000  terminal - cada byte escrito vira um caractere
#
# Registradores com papel fixo durante todo o jogo:
#   t0  = início do bitmap          s8 = status do teclado
#   s9  = dado do teclado           s10 = terminal
#   sp  = cor da parede (50)        gp  = cor do jogador (250)
#   tp  = cor do caminho (0)        a0  = pontuação (movimentos)
#   a2  = posição atual do jogador  a3  = posição da saída
#
# Observação: sp, gp e tp são reaproveitados como cores. Isso é possível
# porque o jogo não usa a pilha nem variáveis globais depois de iniciar.
# =============================================================================

        .text
        .globl main

main:
        li      t0, 2047
        addi    t0, t0, 1025            # t0 = 0xC00 (__bitmap_start)
        li      s8, 0xB0000000          # status do teclado
        addi    s9, s8, 1               # dado do teclado
        li      s10, 0xC0000000         # terminal
        li      sp, 50                  # cor da parede
        li      gp, 250                 # cor do jogador
        li      tp, 0                   # cor do caminho
        li      a0, 0                   # pontuação
        li      t1, 1024                # 32 x 32 pixels
        mv      t2, t0

# Pinta a tela inteira de parede.
fill_walls:
        beqz    t1, carve_path
        sb      sp, 0(t2)
        addi    t2, t2, 1
        addi    t1, t1, -1
        j       fill_walls

# Escava o corredor em serpentina: linhas de 28 pixels ligadas por descidas
# de 6. Cada chamada de carve_segment recebe início (t2), comprimento (s0)
# e passo (s1: +1 direita, -1 esquerda, +32 desce uma linha).
carve_path:
        addi    t2, t0, 33              # linha 1, coluna 1
        li      s0, 28
        li      s1, 1
        jal     carve_segment
        addi    t2, t0, 61              # desce pela coluna 29
        li      s0, 6
        li      s1, 32
        jal     carve_segment
        addi    t2, t0, 253             # linha 7, da coluna 29 para a esquerda
        li      s0, 28
        li      s1, -1
        jal     carve_segment
        addi    t2, t0, 225             # desce pela coluna 1
        li      s0, 6
        li      s1, 32
        jal     carve_segment
        addi    t2, t0, 417             # linha 13, da coluna 1 para a direita
        li      s0, 28
        li      s1, 1
        jal     carve_segment
        addi    t2, t0, 445             # desce pela coluna 29
        li      s0, 6
        li      s1, 32
        jal     carve_segment
        addi    t2, t0, 637             # linha 19, da coluna 29 para a esquerda
        li      s0, 28
        li      s1, -1
        jal     carve_segment

        addi    a2, t0, 33              # jogador começa na linha 1, coluna 1
        sb      gp, 0(a2)
        addi    a3, t0, 610             # saída: linha 19, coluna 2

# Espera uma tecla e calcula a posição de destino.
main_loop:
        lb      a4, 0(s8)
        andi    a4, a4, 64              # bit 6: há tecla disponível?
        beqz    a4, main_loop
        lb      a5, 0(s9)               # código ASCII da tecla
        sb      zero, 0(s8)             # confirma a leitura
        li      a6, 'w'
        beq     a5, a6, move_up
        li      a6, 'a'
        beq     a5, a6, move_left
        li      a6, 's'
        beq     a5, a6, move_down
        li      a6, 'd'
        beq     a5, a6, move_right
        j       main_loop               # outras teclas são ignoradas

move_up:
        addi    a7, a2, -32
        j       try_move
move_down:
        addi    a7, a2, 32
        j       try_move
move_left:
        addi    a7, a2, -1
        j       try_move
move_right:
        addi    a7, a2, 1
        j       try_move

# Move o jogador se o destino for caminho; senão, fim de jogo.
try_move:
        lb      s2, 0(a7)
        bne     s2, tp, game_over       # destino não é caminho: bateu na parede
        sb      tp, 0(a2)               # apaga o jogador da posição antiga
        sb      gp, 0(a7)               # desenha na nova
        mv      a2, a7
        addi    a0, a0, 1               # +1 movimento
        beq     a2, a3, won
        j       main_loop

game_over:
        la      s4, msg_gameover
        jal     print_str
        jal     print_score_digits
        j       end_loop

won:
        la      s4, msg_win
        jal     print_str
        jal     print_score_digits
        j       end_loop

# Pinta s0 pixels de caminho a partir de t2, avançando s1 a cada pixel.
carve_segment:
        beqz    s0, carve_done
carve_loop:
        sb      tp, 0(t2)
        add     t2, t2, s1
        addi    s0, s0, -1
        bgtz    s0, carve_loop
carve_done:
        ret

# Escreve no terminal a string terminada em zero apontada por s4.
print_str:
        lb      s5, 0(s4)
        beqz    s5, ps_done
        sb      s5, 0(s10)
        addi    s4, s4, 1
        j       print_str
ps_done:
        ret

# Escreve a pontuação (a0, de 0 a 999) com três dígitos, por subtrações
# sucessivas (o RV32I não tem divisão).
print_score_digits:
        mv      a1, a0
        li      t3, 0                   # centenas
        li      t4, 0                   # dezenas
ps_hund:
        addi    a1, a1, -100
        bltz    a1, ps_after_h
        addi    t3, t3, 1
        j       ps_hund
ps_after_h:
        addi    a1, a1, 100
ps_ten:
        addi    a1, a1, -10
        bltz    a1, ps_after_t
        addi    t4, t4, 1
        j       ps_ten
ps_after_t:
        addi    a1, a1, 10              # a1 = unidades
        addi    t3, t3, '0'
        addi    t4, t4, '0'
        addi    a1, a1, '0'
        sb      t3, 0(s10)
        sb      t4, 0(s10)
        sb      a1, 0(s10)
        ret

end_loop:
        j       end_loop

        .data
msg_win:
        .string "Voce Venceu! Pontuacao: "
msg_gameover:
        .string "Game Over! Pontuacao: "
