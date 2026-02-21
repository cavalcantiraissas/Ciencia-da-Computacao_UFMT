# Explicação Completa — Pilha com `brk` e Fila com `malloc/free`

**Universidade Federal de Mato Grosso — UFMT**  
**Disciplina:** Organização de Computadores  
**Professor:** Linder Candido  
**Alunas:** Raissa Cavalcanti e Vitória Calonga  

---

## 1. O que o trabalho pede

O trabalho é dividido em duas partes, ambas em **Assembly RISC-V 64-bit**:

**Parte 1 — Pilha com `brk`**  
Implementar uma pilha de inteiros (32 bits) no heap usando **exclusivamente a syscall `brk`** para alocar e liberar memória. É proibido usar `malloc`, `free` ou qualquer biblioteca de alocação. O programa deve ter um menu interativo onde o usuário pode empilhar valores, desempilhar valores e ver o heap crescendo e encolhendo.

**Parte 2 — Fila com `malloc`/`free`**  
Implementar uma fila de inteiros (32 bits) no heap usando **exclusivamente `malloc` e `free`** da libc. O programa deve ter um menu interativo onde o usuário pode inserir e remover valores, demonstrando a ordem FIFO.

---

## 2. Conceitos fundamentais

### 2.1 O que é o heap?

Quando um programa Linux é executado, sua memória é dividida em regiões:

```
Endereços altos ┌──────────────────┐
                │      Stack       │  variáveis locais, endereços de retorno
                │        ↓         │  cresce para baixo
                │                  │
                │        ↑         │  cresce para cima
                │      Heap        │  alocação dinâmica  <-- usamos aqui
                ├──────────────────┤  <-- program break
                │  Dados (.bss)    │  variáveis globais não inicializadas
                │  Dados (.data)   │  variáveis globais inicializadas
Endereços baixos│  Código (.text)  │  instruções do programa
                └──────────────────┘
```

O **heap** é a região destinada à alocação dinâmica. O **program break** é o ponteiro que marca seu fim: tudo abaixo pertence ao processo, tudo acima não foi alocado ainda.

### 2.2 A syscall `brk`

`brk` é uma chamada ao sistema operacional que move o *program break*:

- `brk(0)` → retorna o endereço do break atual sem modificar nada
- `brk(endereço maior)` → expande o heap (aloca memória)
- `brk(endereço menor)` → contrai o heap (libera memória)

No Linux RISC-V 64-bit, `brk` é a syscall de número **214**. Toda syscall no RISC-V é feita assim:

```asm
li   a7, 214    # número da syscall em a7
li   a0, 0      # argumento em a0 (0 = só consulta)
ecall           # chama o kernel
                # resultado retorna em a0
```

### 2.3 `malloc` e `free`

São funções da biblioteca C padrão (libc) que gerenciam o heap internamente:

- `malloc(n)` → aloca `n` bytes e retorna o endereço. Mantém metadados internos (tamanho do bloco, lista de blocos livres) para permitir liberação em qualquer ordem.
- `free(ptr)` → marca o bloco como disponível para reutilização.

A diferença crucial em relação ao `brk` direto: `malloc`/`free` permitem liberar **qualquer bloco**, em **qualquer ordem**. O `brk` direto só consegue retrair o fim do heap, ou seja, só libera o bloco mais recente.

### 2.4 Por que `brk` funciona para pilha mas não para fila?

**Pilha (LIFO):** o elemento removido é sempre o mais recente, que está exatamente nos 4 bytes imediatamente antes do break. Portanto, basta recuar o break 4 bytes para liberar.

**Fila (FIFO):** o elemento removido é o mais antigo, que foi alocado primeiro e está em um endereço mais baixo no heap. Não é possível liberar apenas esse bloco com `brk` sem destruir todos os blocos alocados depois.

### 2.5 ABI RISC-V — por que salvamos registradores na stack?

A ABI (Application Binary Interface) define regras que todas as funções devem seguir para que possam se chamar corretamente:

| Registrador | Papel | Responsável por salvar |
|---|---|---|
| `a0`–`a7` | Argumentos e retorno | Chamador (caller-saved) |
| `ra` | Endereço de retorno | Chamado (callee-saved) |
| `s0`–`s11` | Valores preservados | Chamado (callee-saved) |
| `t0`–`t6` | Temporários | Chamador (caller-saved) |

Quando uma função A chama uma função B, qualquer valor em `a0`–`a7` e `t0`–`t6` pode ser destruído por B. Por isso, antes de qualquer `call`, salvamos na stack os valores que precisamos preservar usando `sd` (store doubleword), e restauramos depois com `ld` (load doubleword).

**Bug importante que encontramos:** após `call scanf`, o registrador `a1` é destruído pela libc. A solução é recarregar o endereço do buffer com `la` depois do `call`:

```asm
la   a0, fmt_scan_d
la   a1, buf_valor
call scanf
la   t0, buf_valor      # OBRIGATÓRIO: recarrega endereço após o call
lw   s0, 0(t0)          # agora lê corretamente
```

---

## 3. Parte 1 — Pilha com `brk` (`pilha_brk.s`)

### 3.1 Como a pilha funciona no heap

O topo da pilha coincide **sempre** com o *program break*. Visualizando:

```
Estado inicial:
  break → 0x4000024000

Após empilha(10):
  0x4000024000: [ 10 ]
  break → 0x4000024004   (break avançou +4)

Após empilha(20):
  0x4000024000: [ 10 ]
  0x4000024004: [ 20 ]
  break → 0x4000024008

Após empilha(30):
  0x4000024000: [ 10 ]
  0x4000024004: [ 20 ]
  0x4000024008: [ 30 ]
  break → 0x400002400c

Após desempilha():   retorna 30
  0x4000024000: [ 10 ]
  0x4000024004: [ 20 ]
  break → 0x4000024008   (break recuou -4, memória liberada)
```

### 3.2 Função `empilha`

```asm
empilha:
    # 1. Consulta break atual (onde vamos gravar o valor)
    li   a7, 214
    li   a0, 0
    ecall
    mv   s1, a0             # s1 = break antes

    # 2. Move break +4 (aloca espaço para 1 inteiro de 32 bits)
    addi a0, s1, 4
    li   a7, 214
    ecall

    # 3. Grava o valor com sw (store word = 32 bits)
    sw   s0, 0(s1)
```

`sw` (store word) grava exatamente 32 bits — o tamanho de um `int` — no endereço indicado.

### 3.3 Função `desempilha`

```asm
desempilha:
    # 1. Consulta break atual
    li   a7, 214
    li   a0, 0
    ecall
    mv   s0, a0             # s0 = break antes

    # 2. Lê o topo com lw (load word = 32 bits) em break-4
    addi t0, s0, -4
    lw   s1, 0(t0)

    # 3. Move break -4 (libera os 4 bytes do topo)
    mv   a0, t0
    li   a7, 214
    ecall

    mv   a0, s1             # retorna o valor
```

### 3.4 Menu interativo

O programa lê a opção do usuário com `scanf` em loop. A cada push, mostra o break crescendo. A cada pop, mostra o break encolhendo e o valor que foi desempilhado.

```
[1] Push (empilhar)
[2] Pop  (desempilhar)
[0] Sair
Opcao: 1
Valor a empilhar: 10
   >> PUSH(10) realizado.
   >> brk: 0x4000024000 -> 0x4000024004  (+4 bytes alocados)
```

### 3.5 Compilação

Como usa `printf` e `scanf` (para saída e entrada), compilamos com `gcc` que faz a ligação com a libc automaticamente. A **memória** ainda é gerenciada exclusivamente pelo `brk`.

```bash
riscv64-linux-gnu-gcc -o pilha_brk pilha_brk.s
qemu-riscv64 -L /usr/riscv64-linux-gnu ./pilha_brk
```

---

## 4. Parte 2 - Fila com `malloc`/`free` (`fila_malloc.s`)

### 4.1 Estrutura do nó

```
┌─────────────────────────────────────────┐
│  offset 0 │ value │ 4 bytes │  sw / lw  │
│  offset 4 │  pad  │ 4 bytes │    —      │
│  offset 8 │ next* │ 8 bytes │  sd / ld  │
└─────────────────────────────────────────┘
             total: 16 bytes por nó
```

- `value` usa `sw`/`lw` porque é um inteiro de 32 bits
- `next` usa `sd`/`ld` porque ponteiros em RISC-V 64-bit têm 8 bytes

### 4.2 Ponteiros globais

```asm
.section .bss
head: .quad 0    # aponta para o PRIMEIRO nó (frente — de onde removemos)
tail: .quad 0    # aponta para o ÚLTIMO nó  (fundo — onde inserimos)
```

`.quad` reserva 8 bytes, necessário para guardar um ponteiro de 64 bits.

### 4.3 Função `insert`

```
1. malloc(16)        → aloca 16 bytes para o novo nó
2. node->value = x   → sw (32 bits, offset 0)
3. node->next = NULL → sd zero (64 bits, offset 8)
4. Se head == NULL (fila vazia):
       head = tail = novo_nó
   Senão:
       tail->next = novo_nó
       tail = novo_nó
```

### 4.4 Função `remove_fila`

```
1. Se head == NULL → fila vazia, retorna -1
2. Lê old_head->value com lw (offset 0)
3. Avança head = head->next com ld (offset 8)
4. Se novo head == NULL → tail = NULL (fila ficou vazia)
5. free(old_head) → libera o nó removido
6. Retorna: valor em a0, endereço liberado em a1
```

### 4.5 Menu interativo

```
[1] Insert (inserir na fila)
[2] Remove (remover da fila)
[0] Sair
Opcao: 1
Valor a inserir: 100
   >> insert(100) realizado.
   >> No alocado no endereco: 0x40000036b0
----------------------------------------
Opcao: 2
   >> remove() = 100
   >> No liberado no endereco:  0x40000036b0
```

### 4.6 Compilação

```bash
riscv64-linux-gnu-gcc -o fila_malloc fila_malloc.s
qemu-riscv64 -L /usr/riscv64-linux-gnu ./fila_malloc
```

---

## 5. Ambiente e ferramentas

| Item | Detalhe |
|---|---|
| Computador | MacBook Pro 16-inch, 2019 |
| Processador host | 2,6 GHz 6-Core Intel Core i7 |
| Sistema operacional | macOS |
| Container | Docker + Ubuntu 22.04 |
| Cross-compilador | riscv64-linux-gnu-gcc (GCC 11) |
| Emulador | qemu-riscv64 (usermode) |
| Arquitetura alvo | RISC-V 64-bit (rv64gc) |

Como o Mac usa arquitetura Intel (x86-64) e não RISC-V, foi necessário um **cross-compilador** (gera código RISC-V rodando em x86-64) e o **QEMU** (emula o processador RISC-V para executar os binários). O Docker foi usado para isolar o ambiente Linux necessário.

---

## 6. Passo a passo completo para rodar

```bash
# 1. No Mac: instalar Docker Desktop em https://docker.com e abrir o app

# 2. Criar pasta e colocar os arquivos .s dentro
mkdir ~/trabalho_riscv
mv ~/Downloads/pilha_brk.s ~/trabalho_riscv/
mv ~/Downloads/fila_malloc.s ~/trabalho_riscv/

# 3. Subir container
docker run -it --rm -v ~/trabalho_riscv:/trabalho ubuntu:22.04 bash

# 4. Dentro do container: instalar ferramentas
apt-get update && apt-get install -y \
  gcc-riscv64-linux-gnu binutils-riscv64-linux-gnu \
  libc6-riscv64-cross qemu-user

# 5. Compilar e rodar Parte 1
cd /trabalho
riscv64-linux-gnu-gcc -o pilha_brk pilha_brk.s
qemu-riscv64 -L /usr/riscv64-linux-gnu ./pilha_brk

# 6. Compilar e rodar Parte 2
riscv64-linux-gnu-gcc -o fila_malloc fila_malloc.s
qemu-riscv64 -L /usr/riscv64-linux-gnu ./fila_malloc
```

---

## 7. Comparativo entre as abordagens

| Aspecto | Parte 1 — `brk` | Parte 2 — `malloc`/`free` |
|---|---|---|
| Estrutura | Pilha (LIFO) | Fila (FIFO) |
| Alocação | syscall `brk` (nº 214) | função `malloc` da libc |
| Liberação | recuar o break | função `free` da libc |
| Overhead | mínimo (só os dados) | header de controle por bloco |
| Fragmentação | impossível (LIFO) | gerenciada pela libc |
| Liberação fora de ordem | impossível | suportada |
| Ponto de entrada | `main` (com libc para I/O) | `main` (com libc) |
| Instrução de leitura | `lw` (32 bits) | `lw` (value) + `ld` (ponteiro) |
