# Pilha com `brk` e Fila com `malloc/free` — Assembly RISC-V 64-bit

> **Disciplina:** Organização de Computadores  
> **Universidade:** Universidade Federal de Mato Grosso (UFMT)  
> **Professor:** Linder Candido  
> **Alunas:** Raissa Cavalcanti · Vitória Calonga  
> **Ambiente de desenvolvimento:** MacBook Pro 16-inch, 2019 — 2,6 GHz 6-Core Intel Core i7, macOS  
> **Execução:** Docker + Ubuntu 22.04 + QEMU RISC-V

---

## Sobre o projeto

Este repositório contém a implementação, em **Assembly RISC-V 64-bit**, de duas estruturas de dados alocadas dinamicamente no heap:

| Arquivo | Estrutura | Política | Alocação |
|---|---|---|---|
| `pilha_brk.s` | Pilha | LIFO | syscall `brk` |
| `fila_malloc.s` | Fila | FIFO | `malloc` / `free` da libc |

---

## Parte 1 - Pilha com `brk`

A pilha é implementada diretamente sobre o heap usando a syscall `brk` (número **214** no Linux RISC-V 64-bit). O topo da pilha coincide sempre com o *program break*.

**`push(x)`** — move o break `+4` bytes e grava o valor  
**`pop()`** — lê o valor em `break - 4` e move o break `-4` bytes

```
Após push(10), push(20), push(30):

  0x4000024000  [10]
  0x4000024004  [20]
  0x4000024008  [30]  <-- break - 4 (topo)
  0x400002400c  <-- break (fim do heap)
```

### Saída real obtida

```
=== Pilha Dinamica no Heap com brk ===
LIFO: push 10, 20, 30  =>  pop esperado: 30, 20, 10

----------------------------------------
PUSH(10)  brk: 0x4000024000 -> 0x4000024004  (+4 bytes alocados)
PUSH(20)  brk: 0x4000024004 -> 0x4000024008  (+4 bytes alocados)
PUSH(30)  brk: 0x4000024008 -> 0x400002400c  (+4 bytes alocados)
----------------------------------------
POP() = 30  brk: 0x400002400c -> 0x4000024008  (-4 bytes liberados)
POP() = 20  brk: 0x4000024008 -> 0x4000024004  (-4 bytes liberados)
POP() = 10  brk: 0x4000024004 -> 0x4000024000  (-4 bytes liberados)
----------------------------------------
```

---

## Parte 2 - Fila com `malloc`/`free`

A fila é uma **lista encadeada** onde cada nó (16 bytes) é alocado individualmente com `malloc` e liberado com `free`. Inserção ocorre no final (`tail`), remoção no início (`head`).

```
head --> [100 | *] --> [200 | *] --> [300 | NULL] <-- tail
```

### Saída real obtida

```
=== Fila Dinamica no Heap com malloc/free ===
Estrategia FIFO: insert 100, 200, 300 -> remove esperado: 100, 200, 300

----------------------------------------
INSERT(100)  -> novo no alocado em 0x40000036b0
INSERT(200)  -> novo no alocado em 0x40000036d0
INSERT(300)  -> novo no alocado em 0x40000036f0
----------------------------------------
REMOVE()   -> valor = 100  (no em 0x40000036b0 liberado)
REMOVE()   -> valor = 200  (no em 0x40000036d0 liberado)
REMOVE()   -> valor = 300  (no em 0x40000036f0 liberado)
----------------------------------------
```

---

## Como rodar

### Pré-requisitos

- [Docker Desktop](https://www.docker.com/products/docker-desktop/) instalado e em execução

### Passo a passo

**1. Clone o repositório**
```bash
git clone <url-do-repositorio>
cd <pasta>
```

**2. Suba o container Ubuntu com as ferramentas RISC-V**
```bash
docker run -it --rm -v $(pwd):/trabalho ubuntu:22.04 bash
```

**3. Dentro do container, instale o toolchain**
```bash
apt-get update && apt-get install -y \
  gcc-riscv64-linux-gnu \
  binutils-riscv64-linux-gnu \
  libc6-riscv64-cross \
  qemu-user
```

**4. Compile e execute a Parte 1 (Pilha com brk)**
```bash
cd /trabalho
riscv64-linux-gnu-gcc -o pilha_brk pilha_brk.s
qemu-riscv64 -L /usr/riscv64-linux-gnu ./pilha_brk
```

**5. Compile e execute a Parte 2 (Fila com malloc/free)**
```bash
riscv64-linux-gnu-gcc -o fila_malloc fila_malloc.s
qemu-riscv64 -L /usr/riscv64-linux-gnu ./fila_malloc
```

---

## Estrutura do repositório

```
.
├── pilha_brk.s      # Parte 1 — Pilha LIFO usando syscall brk
├── fila_malloc.s    # Parte 2 — Fila FIFO usando malloc/free
├── README.md        # Este arquivo
└── relatorio.docx   # Relatório técnico completo
```

---

## Tecnologias utilizadas

- **Assembly RISC-V 64-bit** — linguagem de programação
- **GCC cross-compiler** (`riscv64-linux-gnu-gcc`) — compilação
- **QEMU** (`qemu-riscv64`) — emulação do processador RISC-V
- **Docker + Ubuntu 22.04** — ambiente de execução isolado
- **Linux syscall `brk`** (nº 214) — alocação manual de memória
- **libc** (`malloc`, `free`, `printf`) — alocação gerenciada e saída formatada
