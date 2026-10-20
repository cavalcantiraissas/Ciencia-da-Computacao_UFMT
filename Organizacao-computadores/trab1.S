# Trabalho: Análise de Vetores em Assembly RISC-V
# Ambiente: RISC-V RV64 / Linux / GNU assembler
# Aluna: Raissa Cavalcanti


# Alocação Dados
.section .bss
    .align 2
    n: .space 4
    valor_para_consulta: .space 4
    vetor: .space 400            # 100 inteiros de 4 bytes


# Strings e formatos (somente leitura) usados com printf/scanf
.section .rodata
    msg_pede_n:      .string "Digite a quantidade de elementos (1 a 100): "
    msg_n_invalido:  .string "Valor invalido! Digite um N entre 1 e 100: "
    fmt_scan_int:    .string "%d"

    msg_vetor_orig:  .string "\nVetor original:\n"
    msg_vetor_ord:   .string "\nVetor ordenado:\n"
    fmt_elem_int:    .string "%d "
    fmt_newline:     .string "\n"

    fmt_soma:        .string "Soma: %d\n"
    fmt_menor:       .string "Menor valor: %d\n"
    fmt_maior:       .string "Maior valor: %d\n"
    fmt_media:       .string "Media (inteira): %d\n"

    msg_pede_busca:  .string "\nDigite um valor para pesquisar: "
    msg_achado:      .string "Valor encontrado no indice %d do vetor ordenado.\n"
    msg_nao_achado:  .string "Valor nao encontrado no vetor.\n"

    fmt_prompt_elem: .string "Elemento %d: "


# Código
.section .text
    .globl main
main:
    # ---- Prólogo: reserva frame e salva registradores que
    #      precisam sobreviver às chamadas de printf/scanf/funções ----
    addi sp, sp, -48
    sd   ra, 40(sp)     # endereço de retorno de main
    sd   s0, 32(sp)     # s0 = N (quantidade de elementos)
    sd   s1, 24(sp)     # s1 = endereço base do vetor
    sd   s2, 16(sp)     # s2 = resultado de busca (uso temporário)

    # 1. Ler N
    la   a0, msg_pede_n
    call printf
    la   a0, fmt_scan_int
    la   a1, n
    call scanf

    # 2. Verificar: 1 <= N <= 100  (repete a leitura até ser válido)
valida_n:
    lw   t0, n
    li   t1, 1
    blt  t0, t1, n_invalido      # N < 1  -> inválido
    li   t1, 100
    bgt  t0, t1, n_invalido      # N > 100 -> inválido
    j    n_valido
n_invalido:
    la   a0, msg_n_invalido
    call printf
    la   a0, fmt_scan_int
    la   a1, n
    call scanf
    j    valida_n
n_valido:
    lw   s0, n                  # s0 = N (sign-extend 32->64 automático no lw)
    la   s1, vetor               # s1 = &vetor[0]

    # 3. Chamar: read_vector
    mv   a0, s1
    mv   a1, s0
    call read_vector

    # 4. Mostrar vetor original: print_vector
    la   a0, msg_vetor_orig
    call printf
    mv   a0, s1
    mv   a1, s0
    call print_vector

    # 5. Calcular e mostrar: sum/min/max/avg
    mv   a0, s1
    mv   a1, s0
    call sum_vector
    mv   a1, a0                  # a1 = soma (argumento de printf)
    la   a0, fmt_soma
    call printf

    mv   a0, s1
    mv   a1, s0
    call min_vector
    mv   a1, a0
    la   a0, fmt_menor
    call printf

    mv   a0, s1
    mv   a1, s0
    call max_vector
    mv   a1, a0
    la   a0, fmt_maior
    call printf

    mv   a0, s1
    mv   a1, s0
    call avg_vector
    mv   a1, a0
    la   a0, fmt_media
    call printf

    # 6. Ordenar: sort_vector (bubble sort)
    mv   a0, s1
    mv   a1, s0
    call sort_vector

    # 7. Mostrar vetor ordenado: print_vector
    la   a0, msg_vetor_ord
    call printf
    mv   a0, s1
    mv   a1, s0
    call print_vector

    # 8. Ler valor para pesquisa e chamar find_value
    la   a0, msg_pede_busca
    call printf
    la   a0, fmt_scan_int
    la   a1, valor_para_consulta
    call scanf

    mv   a0, s1
    mv   a1, s0
    lw   a2, valor_para_consulta
    call find_value
    mv   s2, a0                  # s2 = índice encontrado (ou -1)

    # 9. Mostrar resultado da pesquisa
    bltz s2, busca_falhou
    la   a0, msg_achado
    mv   a1, s2
    call printf
    j    fim_main
busca_falhou:
    la   a0, msg_nao_achado
    call printf

fim_main:
    # Epílogo: restaura registradores e retorna 
    ld   ra, 40(sp)
    ld   s0, 32(sp)
    ld   s1, 24(sp)
    ld   s2, 16(sp)
    addi sp, sp, 48

    li   a0, 0
    ret


# read_vector
# Entrada:
#   a0 = endereço inicial do vetor
#   a1 = número de elementos
# Saída:
#   nenhuma
# Observação:
#   Esta função chama printf e scanf.
read_vector:
    addi sp, sp, -48
    sd   ra, 40(sp)
    sd   s0, 32(sp)       # s0 = endereço base do vetor (preservado entre chamadas)
    sd   s1, 24(sp)       # s1 = n (preservado entre chamadas)
    sd   s2, 16(sp)       # s2 = i (índice do laço)

    mv   s0, a0
    mv   s1, a1
    li   s2, 0            # i = 0

rv_loop:
    bge  s2, s1, rv_fim

    # printf("Elemento %d: ", i+1)
    la   a0, fmt_prompt_elem
    addi a1, s2, 1
    call printf

    # scanf("%d", &vetor[i])
    la   a0, fmt_scan_int
    slli t0, s2, 2         # offset = i*4
    add  a1, s0, t0        # a1 = &vetor[i]
    call scanf

    addi s2, s2, 1
    j    rv_loop

rv_fim:
    ld   ra, 40(sp)
    ld   s0, 32(sp)
    ld   s1, 24(sp)
    ld   s2, 16(sp)
    addi sp, sp, 48
    ret


# print_vector
# Entrada:
#   a0 = endereço inicial do vetor
#   a1 = número de elementos
# Saída:
#   nenhuma
# Imprimir cada elemento utilizando printf("%d ", vector[i]);
print_vector:
    addi sp, sp, -48
    sd   ra, 40(sp)
    sd   s0, 32(sp)       # s0 = endereço base
    sd   s1, 24(sp)       # s1 = n
    sd   s2, 16(sp)       # s2 = i

    mv   s0, a0
    mv   s1, a1
    li   s2, 0

pv_loop:
    bge  s2, s1, pv_fim

    slli t0, s2, 2
    add  t0, s0, t0
    lw   a1, 0(t0)          # a1 = vetor[i]
    la   a0, fmt_elem_int
    call printf

    addi s2, s2, 1
    j    pv_loop

pv_fim:
    la   a0, fmt_newline
    call printf

    ld   ra, 40(sp)
    ld   s0, 32(sp)
    ld   s1, 24(sp)
    ld   s2, 16(sp)
    addi sp, sp, 48
    ret


# sum_vector
# Entrada:
#   a0 = endereço inicial do vetor
#   a1 = número de elementos
# Saída:
#   a0 = soma dos elementos
# Não deve utilizar funções da libc.
sum_vector:
    # Função folha (não chama ninguém): não precisa de prólogo/epílogo
    # nem salvar registradores salvos, pois só usa t0-t3.
    li   t0, 0            # soma = 0
    li   t1, 0            # i = 0

sum_loop:
    bge  t1, a1, sum_fim
    slli t2, t1, 2
    add  t2, a0, t2
    lw   t3, 0(t2)
    add  t0, t0, t3
    addi t1, t1, 1
    j    sum_loop

sum_fim:
    mv   a0, t0
    ret


# min_vector
# Entrada:
#   a0 = endereço inicial do vetor
#   a1 = número de elementos
# Saída:
#   a0 = menor elemento
min_vector:
    lw   t0, 0(a0)        # min = vetor[0]
    li   t1, 1            # i = 1

min_loop:
    bge  t1, a1, min_fim
    slli t2, t1, 2
    add  t2, a0, t2
    lw   t3, 0(t2)         # candidato = vetor[i]
    bge  t3, t0, min_pula  # se candidato >= min atual, não atualiza
    mv   t0, t3
min_pula:
    addi t1, t1, 1
    j    min_loop

min_fim:
    mv   a0, t0
    ret


# max_vector
# Entrada:
#   a0 = endereço inicial do vetor
#   a1 = número de elementos
# Saída:
#   a0 = maior elemento
max_vector:
    lw   t0, 0(a0)        # max = vetor[0]
    li   t1, 1            # i = 1

max_loop:
    bge  t1, a1, max_fim
    slli t2, t1, 2
    add  t2, a0, t2
    lw   t3, 0(t2)         # candidato = vetor[i]
    ble  t3, t0, max_pula  # se candidato <= max atual, não atualiza
    mv   t0, t3
max_pula:
    addi t1, t1, 1
    j    max_loop

max_fim:
    mv   a0, t0
    ret


# avg_vector
# Entrada:
#   a0 = endereço inicial do vetor
#   a1 = número de elementos
# Saída:
#   a0 = média aproximada (inteira)
avg_vector:
    addi sp, sp, -16
    sd   ra, 8(sp)
    sd   s0, 0(sp)         # s0 = n (preservado durante a chamada a sum_vector)

    mv   s0, a1
    call sum_vector        # a0 (endereço) e a1 (n) já estão corretos
                            # sum_vector devolve a soma em a0
    div  a0, a0, s0        # média inteira = soma / n

    ld   ra, 8(sp)
    ld   s0, 0(sp)
    addi sp, sp, 16
    ret


# sort_vector
# Entrada:
#   a0 = endereço inicial do vetor
#   a1 = número de elementos
# Saída:
#   nenhuma
# Algoritmo obrigatório: bubble sort
sort_vector:
    # Função folha: só usa t0-t6, sem prólogo/epílogo necessário.
    addi t0, a1, -1        # limite externo = n-1

    li   t1, 0             # i = 0
bs_outer:
    bge  t1, t0, bs_outer_fim

    li   t2, 0             # j = 0
    sub  t3, t0, t1        # limite interno = (n-1) - i
bs_inner:
    bge  t2, t3, bs_inner_fim

    slli t4, t2, 2
    add  t4, a0, t4        # t4 = &vetor[j]
    lw   t5, 0(t4)          # vetor[j]
    lw   t6, 4(t4)          # vetor[j+1]
    ble  t5, t6, bs_sem_troca   # já em ordem -> não troca
    sw   t6, 0(t4)
    sw   t5, 4(t4)
bs_sem_troca:
    addi t2, t2, 1
    j    bs_inner

bs_inner_fim:
    addi t1, t1, 1
    j    bs_outer

bs_outer_fim:
    ret


# find_value
# Entrada:
#   a0 = endereço inicial do vetor
#   a1 = número de elementos
#   a2 = valor procurado
# Saída:
#   a0 = índice encontrado
# ou
#   a0 = -1 caso o valor não esteja presente.
find_value:
    # Função folha: busca binária, só usa t0-t4.
    # Pré-condição: vetor deve estar ORDENADO (crescente).
    li   t0, 0             # lo = 0
    addi t1, a1, -1        # hi = n-1

fv_loop:
    blt  t1, t0, fv_nao_achou    # hi < lo -> não encontrado

    add  t2, t0, t1
    srli t2, t2, 1          # mid = (lo+hi)/2

    slli t3, t2, 2
    add  t3, a0, t3
    lw   t4, 0(t3)           # vetor[mid]

    beq  t4, a2, fv_achou
    blt  t4, a2, fv_direita

    addi t1, t2, -1          # valor procurado é menor: hi = mid-1
    j    fv_loop
fv_direita:
    addi t0, t2, 1           # valor procurado é maior: lo = mid+1
    j    fv_loop

fv_achou:
    mv   a0, t2
    ret
fv_nao_achou:
    li   a0, -1
    ret
