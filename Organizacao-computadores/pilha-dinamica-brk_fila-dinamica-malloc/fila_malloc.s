# fila_malloc.s
# Parte 2: Fila (FIFO) Dinâmica no Heap usando malloc e free da libc
# Disciplina: Arquitetura e Sistemas Operacionais
# Linguagem: Assembly RISC-V 64-bit (Linux, linkado com libc via gcc)
#
# CONCEITO:
#   Implementamos uma fila encadeada (linked list) onde cada nó é alocado
#   individualmente com malloc e liberado com free.
#
#   Estrutura de cada nó (16 bytes no total em rv64):
#     offset 0: value (4 bytes - o inteiro armazenado)
#     offset 4: padding (4 bytes - alinhamento)
#     offset 8: next   (8 bytes - ponteiro para o próximo nó)
#
#   Política FIFO: inserção no final (tail), remoção no início (head).
#
#   head -> [nó1] -> [nó2] -> [nó3] -> NULL
#                                       ^
#                                      tail
#
# Compilar e rodar:
#   riscv64-linux-gnu-gcc -o fila_malloc fila_malloc.s
#   qemu-riscv64 ./fila_malloc

# Declaração de funções externas (libc)
.extern malloc           # malloc(size_t size) -> void*
.extern free             # free(void* ptr)
.extern printf           # printf(const char* fmt, ...) -> int

# Seção de dados
.section .data

fmt_titulo:   .asciz "=== Fila Dinamica no Heap com malloc/free ===\n"
fmt_fifo:     .asciz "Estrategia FIFO: insert 100, 200, 300 -> remove esperado: 100, 200, 300\n\n"
fmt_insert:   .asciz "INSERT(%d)  -> novo no alocado em 0x%lx\n"
fmt_remove:   .asciz "REMOVE()   -> valor = %d  (no em 0x%lx liberado)\n"
fmt_sep:      .asciz "----------------------------------------\n"
fmt_erro_vazia: .asciz "ERRO: tentativa de remover de fila vazia!\n"

# Seção BSS — variáveis globais da fila
.section .bss
.align 3
head: .quad 0            # ponteiro para o primeiro nó da fila (NULL = vazia)
tail: .quad 0            # ponteiro para o último nó da fila

# Constante: tamanho de cada nó da fila
.set NODE_SIZE, 16       # 4 (value) + 4 (pad) + 8 (next*) = 16 bytes
.set OFF_VALUE, 0        # offset do campo value dentro do nó
.set OFF_NEXT,  8        # offset do campo next dentro do nó


# Seção de código
.section .text
.global main


# insert: Insere inteiro em a0 no FINAL da fila (tail)
#     1. malloc(NODE_SIZE) => aloca 16 bytes para o novo nó
#     2. Grava o valor no offset 0 do nó
#     3. Grava NULL no offset 8 (campo next), pois este será o último
#     4. Se a fila está vazia (head == NULL): head = tail = novo_nó
#        Senão: tail->next = novo_nó  e  tail = novo_nó
insert:
    addi sp, sp, -32
    sd   ra, 24(sp)
    sd   s0, 16(sp)      # s0 = valor a inserir
    sd   s1, 8(sp)       # s1 = ponteiro do novo nó

    mv   s0, a0          # salva o valor

    #aloca memória para o novo nó
    li   a0, 16          # NODE_SIZE = 16 bytes
    call malloc          # retorna ponteiro em a0
    mv   s1, a0          # s1 = endereço do novo nó

    #preenche os campos do novo nó
    sw   s0, 0(s1)       # node->value = s0 (store word, 32 bits)
    li   t0, 0
    sd   t0, 8(s1)       # node->next = NULL (store doubleword, 8 bytes)

    #encadeia o nó na fila
    la   t1, head
    ld   t2, 0(t1)       # t2 = head atual
    bnez t2, .ins_nao_vazia  # se head != NULL, fila não está vazia

    #fila estava vazia: head = tail = novo_nó
    sd   s1, 0(t1)       # head = s1
    la   t1, tail
    sd   s1, 0(t1)       # tail = s1
    j    .ins_fim

.ins_nao_vazia:
    #fila não está vazia: tail->next = novo_nó, tail = novo_nó
    la   t1, tail
    ld   t2, 0(t1)       # t2 = tail atual
    sd   s1, 8(t2)       # tail->next = novo_nó
    sd   s1, 0(t1)       # tail = novo_nó (atualiza ponteiro global)

.ins_fim:
    #retorna endereço do nó alocado em a0 (útil para imprimir)
    mv   a0, s1

    ld   s1, 8(sp)
    ld   s0, 16(sp)
    ld   ra, 24(sp)
    addi sp, sp, 32
    ret

# dequeue: Remove o nó do INÍCIO da fila (head) e retorna o valor em a0
#          Retorna -1 se a fila estiver vazia (a1 = endereço do nó, ou 0)
#     1. Se head == NULL: retorna -1 (fila vazia)
#     2. Salva o endereço do nó atual (old_head) e lê seu valor
#     3. Avança head para head->next
#     4. Se head ficou NULL: tail também vira NULL (fila ficou vazia)
#     5. free(old_head) => libera a memória do nó removido
#     6. Retorna o valor em a0 e o endereço liberado em a1
dequeue:
    addi sp, sp, -32
    sd   ra, 24(sp)
    sd   s0, 16(sp)      # s0 = ponteiro do nó a remover
    sd   s1, 8(sp)       # s1 = valor lido

    la   t0, head
    ld   s0, 0(t0)       # s0 = head atual
    beqz s0, .deq_vazia  # se head == NULL: fila vazia

    #lê o valor do nó da frente
    lw   s1, 0(s0)       # s1 = head->value (32 bits)

    #avança head para o próximo nó
    ld   t1, 8(s0)       # t1 = head->next
    sd   t1, 0(t0)       # head = head->next

    #se a fila ficou vazia, zera tail também
    bnez t1, .deq_free   # se head->next != NULL, fila ainda tem elementos
    la   t2, tail
    sd   zero, 0(t2)     # tail = NULL

.deq_free:
    #libera a memória do nó removido
    mv   a1, s0          # salva endereço para retornar (a1)
    mv   a0, s0          # argumento para free
    call free

    mv   a0, s1          # retorna o valor em a0
    mv   a1, s0          # retorna endereço liberado em a1 (para impressão)
    j    .deq_fim

.deq_vazia:
    li   a0, -1          # código de erro: fila vazia
    li   a1, 0
    j    .deq_fim

.deq_fim:
    ld   s1, 8(sp)
    ld   s0, 16(sp)
    ld   ra, 24(sp)
    addi sp, sp, 32
    ret

# demo_insert: Chama insert e imprime resultado com printf
demo_insert:
    addi sp, sp, -16
    sd   ra, 8(sp)
    sd   s0, 0(sp)

    mv   s0, a0          # salva o valor

    # chama insert: retorna endereço do nó em a0
    mv   a0, s0
    call insert
    mv   a1, s0          # a1 = valor (para printf)
    mv   a2, a0          # a2 = endereço do nó alocado (para printf)

    # printf("INSERT(%d)  -> novo no alocado em 0x%lx\n", valor, endereco)
    la   a0, fmt_insert
    # a1 já tem o valor, a2 já tem o endereço
    call printf

    ld   s0, 0(sp)
    ld   ra, 8(sp)
    addi sp, sp, 16
    ret

# demo_remove: Chama dequeue e imprime resultado com printf
demo_remove:
    addi sp, sp, -16
    sd   ra, 8(sp)

    # chama dequeue: retorna valor em a0, endereço do nó liberado em a1
    call dequeue

    mv   a2, a1          # a2 = endereço do nó liberado (para printf)
    mv   a1, a0          # a1 = valor retornado (para printf)

    # printf("REMOVE()   -> valor = %d  (no em 0x%lx liberado)\n", valor, end)
    la   a0, fmt_remove
    call printf

    ld   ra, 8(sp)
    addi sp, sp, 16
    ret

# main: Ponto de entrada (linkado com libc via gcc)
# Demonstra o comportamento FIFO da fila
main:
    addi sp, sp, -16
    sd   ra, 8(sp)

    # --- cabeçalho ---
    la   a0, fmt_titulo
    call printf

    la   a0, fmt_fifo
    call printf

    la   a0, fmt_sep
    call printf

    # --- insert 100, 200, 300 ---
    li   a0, 100
    call demo_insert

    li   a0, 200
    call demo_insert

    li   a0, 300
    call demo_insert

    la   a0, fmt_sep
    call printf

    # --- remove x3: deve retornar 100, 200, 300 (FIFO) ---
    call demo_remove
    call demo_remove
    call demo_remove

    la   a0, fmt_sep
    call printf

    # retorna 0 para o sistema operacional
    li   a0, 0
    ld   ra, 8(sp)
    addi sp, sp, 16
    ret
