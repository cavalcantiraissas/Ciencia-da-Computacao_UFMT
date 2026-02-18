# Explicação - Pilha com `brk` e Fila com `malloc/free`

**Universidade Federal de Mato Grosso — UFMT**  
**Disciplina:** Organização de Computadores  
**Professor:** Linder Candido  
**Alunas:** Raissa Cavalcanti e Vitória Calonga  

---

## 1. O que o trabalho pede

O trabalho é dividido em duas partes independentes, ambas em **Assembly RISC-V 64-bit**:

**Parte 1 — Pilha no heap usando `brk`**
Implementar uma pilha de inteiros (32 bits) alocada dinamicamente no heap, usando **exclusivamente a syscall `brk`** para alocar e liberar memória. A pilha deve obedecer à política **LIFO** (Last In, First Out) e o programa deve mostrar os endereços do heap crescendo e encolhendo conforme as operações.

**Parte 2 — Fila no heap usando `malloc` e `free`**
Implementar uma fila de inteiros (32 bits) alocada dinamicamente no heap, usando **exclusivamente `malloc` e `free`** da libc. A fila deve obedecer à política **FIFO** (First In, First Out) e o programa deve demonstrar a ordem de remoção.

---

## 2. Conceitos fundamentais

### 2.1 O que é o heap?

A memória de um processo Linux é dividida em regiões:

```
Endereços altos  ┌─────────────────┐
                 │      Stack      │  ← cresce para baixo (variáveis locais, retornos)
                 │        ↓        │
                 │   (espaço livre)│
                 │        ↑        │
                 │      Heap       │  ← cresce para cima (alocação dinâmica)
                 ├─────────────────┤  ← program break
                 │  Dados (BSS)    │
                 │  Dados (.data)  │
Endereços baixos │  Código (.text) │
                 └─────────────────┘
```

O **heap** é a região reservada para alocação dinâmica. Ele começa logo após o segmento de dados e cresce em direção a endereços maiores. O **program break** é o ponteiro que delimita o fim do heap — tudo abaixo pertence ao processo; tudo acima ainda não foi alocado.

### 2.2 O que é a syscall `brk`?

`brk` é uma chamada de sistema que permite mover o *program break* diretamente:

- **`brk(0)`** → o kernel retorna o endereço do break atual, sem modificar nada
- **`brk(novo_endereço)`** → o kernel move o break para o novo endereço
  - Se `novo_endereço > break atual`: **aloca** memória (heap cresce)
  - Se `novo_endereço < break atual`: **libera** memória (heap encolhe)

No Linux RISC-V 64-bit, o número da syscall `brk` é **214**. A chamada é feita com:

```asm
li   a7, 214    # número da syscall
li   a0, 0      # argumento (0 = só consulta)
ecall           # chama o kernel; resultado fica em a0
```

### 2.3 O que é `malloc` e `free`?

São funções da biblioteca C padrão (libc) que gerenciam o heap de forma mais sofisticada:

- **`malloc(n)`** → aloca `n` bytes contíguos e retorna o endereço. Internamente, mantém metadados (listas de blocos livres, tamanhos) para permitir liberação em qualquer ordem.
- **`free(ptr)`** → marca o bloco apontado por `ptr` como disponível para reutilização.

A diferença essencial em relação ao `brk` direto: `malloc`/`free` permitem liberar qualquer bloco, em qualquer ordem, porque controlam metadados internos. O `brk` direto só consegue liberar o bloco mais recente (o do topo do heap).

### 2.4 Política LIFO (pilha)

**LIFO** = Last In, First Out. O último elemento inserido é o primeiro a ser removido. Pense em uma pilha de pratos: o último colocado é o primeiro retirado.

```
push(10) → [10]
push(20) → [10][20]
push(30) → [10][20][30]
pop()    → retorna 30, pilha: [10][20]
pop()    → retorna 20, pilha: [10]
pop()    → retorna 10, pilha: []
```

### 2.5 Política FIFO (fila)

**FIFO** = First In, First Out. O primeiro elemento inserido é o primeiro a ser removido. Pense em uma fila de banco: quem chegou primeiro, sai primeiro.

```
insert(100) → [100]
insert(200) → [100][200]
insert(300) → [100][200][300]
remove()    → retorna 100, fila: [200][300]
remove()    → retorna 200, fila: [300]
remove()    → retorna 300, fila: []
```

---

## 3. Parte 1 — Pilha com `brk` (`pilha_brk.s`)

### 3.1 Ideia central

O topo da pilha coincide **sempre** com o *program break*. Isso é possível porque a pilha segue LIFO: nunca precisamos acessar um elemento que não seja o mais recente.

```
Estado inicial:
  break → 0x4000024000  (fim do heap vazio)

Após push(10):
  0x4000024000: [10]    ← valor gravado aqui
  break → 0x4000024004  (break avançou +4)

Após push(20):
  0x4000024000: [10]
  0x4000024004: [20]
  break → 0x4000024008

Após pop():
  lê [20] de 0x4000024004 (= break - 4)
  break → 0x4000024004  (break recuou -4)
  retorna 20
```

### 3.2 Função `push`

```asm
push:
    # 1. Consulta o break atual
    li   a7, 214
    li   a0, 0
    ecall               # a0 = break atual (vamos escrever aqui)
    mv   s1, a0

    # 2. Move o break +4 (aloca 4 bytes para um inteiro de 32 bits)
    addi a0, s1, 4
    li   a7, 214
    ecall

    # 3. Grava o valor no endereço antigo do break
    sw   s0, 0(s1)      # sw = store word (32 bits)
```

`sw` (store word) grava 32 bits no endereço indicado — exatamente o tamanho de um `int` em C.

### 3.3 Função `pop`

```asm
pop:
    # 1. Consulta o break atual
    li   a7, 214
    li   a0, 0
    ecall
    mv   s0, a0         # s0 = break atual

    # 2. Lê o inteiro no topo (break - 4)
    addi t0, s0, -4
    lw   s1, 0(t0)      # lw = load word (32 bits)

    # 3. Libera os 4 bytes (move o break -4)
    mv   a0, t0
    li   a7, 214
    ecall

    mv   a0, s1         # retorna o valor
```

**Por que `brk` serve para liberar aqui?** Porque a política LIFO garante que o elemento removido é sempre o mais recente, que está exatamente nos 4 bytes imediatamente abaixo do break. Se a política fosse diferente (ex.: remoção do meio), não seria possível usar `brk` para liberar.

### 3.4 Saída obtida e análise

```
PUSH(10)  brk: 0x4000024000 -> 0x4000024004  (+4 bytes alocados)
PUSH(20)  brk: 0x4000024004 -> 0x4000024008  (+4 bytes alocados)
PUSH(30)  brk: 0x4000024008 -> 0x400002400c  (+4 bytes alocados)
POP() = 30  brk: 0x400002400c -> 0x4000024008  (-4 bytes liberados)
POP() = 20  brk: 0x4000024008 -> 0x4000024004  (-4 bytes liberados)
POP() = 10  brk: 0x4000024004 -> 0x4000024000  (-4 bytes liberados)
```

Observações:
- O heap cresceu de `0x4000024000` até `0x400002400c` (+12 bytes = 3 inteiros × 4 bytes)
- Os pops retornaram **30 → 20 → 10** (ordem LIFO confirmada)
- O heap voltou exatamente ao estado inicial após os três pops

---

## 4. Parte 2 — Fila com `malloc`/`free` (`fila_malloc.s`)

### 4.1 Estrutura do nó

A fila é uma **lista encadeada simples**. Cada nó ocupa **16 bytes** no heap (em RISC-V 64-bit):

```
offset  0: value (4 bytes) — inteiro armazenado  [sw / lw]
offset  4: padding (4 bytes) — alinhamento de memória
offset  8: next*  (8 bytes) — ponteiro para o próximo nó [sd / ld]
```

O campo `next` usa 8 bytes porque ponteiros em RISC-V 64-bit têm 64 bits. As instruções `sd` (store doubleword) e `ld` (load doubleword) operam com 8 bytes.

### 4.2 Variáveis globais

```asm
.section .bss
head: .quad 0    # ponteiro para o primeiro nó (NULL = fila vazia)
tail: .quad 0    # ponteiro para o último nó
```

`.quad` reserva 8 bytes (64 bits), necessário para guardar um ponteiro.

### 4.3 Função `insert`

```
1. malloc(16) → aloca 16 bytes; retorna endereço em a0
2. node->value = x       (sw, offset 0)
3. node->next  = NULL    (sd zero, offset 8)
4. Se head == NULL:
       head = tail = novo_nó         (fila estava vazia)
   Senão:
       tail->next = novo_nó          (encadeia no final)
       tail = novo_nó                (atualiza cauda)
```

### 4.4 Função `dequeue` (remove)

```
1. Se head == NULL: retorna erro (fila vazia)
2. old_head = head
3. valor = old_head->value           (lw, offset 0)
4. head = old_head->next             (ld, offset 8)
5. Se head == NULL: tail = NULL      (fila ficou vazia)
6. free(old_head)                    (libera o nó removido)
7. Retorna valor
```

### 4.5 Por que `malloc`/`free` aqui e não `brk`?

A fila remove do **início**, não do final. Se usássemos `brk`, teríamos que recuar o break para liberar o nó inicial, o que destruiria todos os nós inseridos depois — pois eles estão em endereços mais altos. O `malloc`/`free` mantém estruturas internas (listas de blocos livres, headers de controle) que permitem liberar qualquer bloco em qualquer ordem.

### 4.6 Saída obtida e análise

```
INSERT(100)  -> novo no alocado em 0x40000036b0
INSERT(200)  -> novo no alocado em 0x40000036d0
INSERT(300)  -> novo no alocado em 0x40000036f0
REMOVE()   -> valor = 100  (no em 0x40000036b0 liberado)
REMOVE()   -> valor = 200  (no em 0x40000036d0 liberado)
REMOVE()   -> valor = 300  (no em 0x40000036f0 liberado)
```

Observações:
- Os endereços dos nós estão espaçados de 0x20 (32 bytes) — o `malloc` adiciona um header de controle de 16 bytes além dos 16 bytes do nó
- Os valores removidos saíram na ordem **100 → 200 → 300** (FIFO confirmado)
- Cada nó foi liberado imediatamente após a remoção

---

## 5. Convenções da ABI RISC-V utilizadas

| Registrador | Papel | Quem salva |
|---|---|---|
| `a0`–`a7` | Argumentos e retorno | Chamador (caller-saved) |
| `ra` | Endereço de retorno | Chamado (callee-saved) |
| `s0`–`s11` | Valores preservados entre chamadas | Chamado (callee-saved) |
| `t0`–`t6` | Temporários | Chamador (caller-saved) |

Toda função que faz `call` para outra função precisa salvar `ra` na stack antes, e restaurar ao final. Valores que precisam sobreviver a uma chamada de subfunção devem ser guardados nos registradores `s0`–`s11` (e estes salvos/restaurados no prólogo/epílogo da função).

---

## 6. Ambiente e ferramentas utilizadas

| Item | Detalhe |
|---|---|
| Computador | MacBook Pro 16-inch, 2019 — 2,6 GHz 6-Core Intel Core i7 |
| Sistema operacional do host | macOS |
| Container | Docker + Ubuntu 22.04 (x86_64) |
| Cross-compilador | `riscv64-linux-gnu-gcc` (GCC 11) |
| Assembler | `riscv64-linux-gnu-as` (GNU Binutils) |
| Emulador | `qemu-riscv64` (usermode) |
| Biblioteca C (target) | `libc6-riscv64-cross` |

---

## 7. Passo a passo completo para reproduzir

### 7.1 Instalar o Docker Desktop no Mac

Acesse https://www.docker.com/products/docker-desktop/ e instale. Abra o Docker Desktop e aguarde o ícone estabilizar na barra de menu.

### 7.2 Preparar a pasta do projeto

```bash
# No terminal do Mac:
mkdir ~/trabalho_riscv
cd ~/trabalho_riscv
# Coloque pilha_brk.s e fila_malloc.s nesta pasta
```

### 7.3 Subir o container

```bash
docker run -it --rm -v ~/trabalho_riscv:/trabalho ubuntu:22.04 bash
```

### 7.4 Instalar as ferramentas dentro do container

```bash
apt-get update && apt-get install -y \
  gcc-riscv64-linux-gnu \
  binutils-riscv64-linux-gnu \
  libc6-riscv64-cross \
  qemu-user
```

### 7.5 Compilar e executar

```bash
cd /trabalho

# Parte 1
riscv64-linux-gnu-gcc -o pilha_brk pilha_brk.s
qemu-riscv64 -L /usr/riscv64-linux-gnu ./pilha_brk

# Parte 2
riscv64-linux-gnu-gcc -o fila_malloc fila_malloc.s
qemu-riscv64 -L /usr/riscv64-linux-gnu ./fila_malloc
```

---

## 8. Comparativo final entre as duas abordagens

| Característica | Parte 1 — `brk` | Parte 2 — `malloc`/`free` |
|---|---|---|
| Estrutura | Pilha (LIFO) | Fila (FIFO) |
| Syscall / função | `brk` (nº 214) | `malloc` + `free` (libc) |
| Overhead de memória | Mínimo (só os dados) | Header de controle por bloco |
| Fragmentação | Impossível (LIFO) | Possível (gerenciada pela libc) |
| Flexibilidade de liberação | Apenas o último bloco | Qualquer bloco, em qualquer ordem |
| Ligação | Standalone (`_start`) | Com libc (`main` + `gcc`) |
| Instrução de gravação | `sw` (32 bits) | `sw` (value) + `sd` (ponteiro) |
