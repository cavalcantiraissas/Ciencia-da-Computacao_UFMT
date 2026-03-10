# Relógio Digital - RISC-V Bare Metal

> Implementação de um relógio digital em **Assembly RISC-V RV32IMA**, executando diretamente sobre o hardware emulado pelo **QEMU virt**, sem sistema operacional, sem bibliotecas externas.

**Disciplina:** Organização de Computadores - UFMT  
**Alunos:** Raissa Cavalcanti e Luiz Torres  
**Professor:** Linder Cândido

---

## Sumário

- [Visão Geral](#visão-geral)
- [Estrutura do Projeto](#estrutura-do-projeto)
- [Mapa de Memória](#mapa-de-memória)
- [Arquitetura do Software](#arquitetura-do-software)
- [Como Executar](#como-executar)
- [Uso do Relógio](#uso-do-relógio)
- [Detalhes Técnicos](#detalhes-técnicos)
- [Referências](#referências)

---

## Visão Geral

O projeto implementa um relógio digital funcional inteiramente em Assembly RISC-V, sem nenhuma camada de abstração. Todos os periféricos são controlados por acesso direto a registradores mapeados em memória (MMIO).

**Funcionalidades:**

| Funcionalidade | Mecanismo |
|---|---|
| Contagem de tempo (HH:MM:SS) | Interrupção do timer CLINT (Machine Timer Interrupt) |
| Exibição na UART | Chamada à `display_time` dentro da ISR do timer |
| Recepção de comandos | Interrupção externa via PLIC (Machine External Interrupt) |
| Ajuste de hora | Comando `T HH:MM:SS` processado na ISR da UART |
| Loop principal | Instrução `wfi` — sem busy-waiting |

---

## Estrutura do Projeto

```
riscv-clock/
├── clock.S       # Código-fonte Assembly (único arquivo de código)
├── link.ld       # Linker script — mapeia seções a partir de 0x80000000
├── Makefile      # Regras de build e execução
├── Dockerfile    # Ambiente Ubuntu 22.04 com toolchain + QEMU
└── README.md     # Este arquivo
```

### clock.S — Funções implementadas

```
_start          Inicialização de pilha, UART, PLIC, timer e CSRs
main_loop       Loop infinito com wfi
trap_handler    Vetor único de interrupções — despacha por mcause
isr_timer       ISR do timer: reprograma + tick + display
isr_external    ISR da UART via PLIC: claim, lê byte, bufferiza, complete
tick_second     Incrementa SS→MM→HH com wraparound 60/60/24
display_time    Envia \rHH:MM:SS pela UART
dec2            Converte número (0–99) em dois dígitos ASCII
parse_command   Valida e aplica comando "T HH:MM:SS"
p2d             Converte dois chars ASCII em inteiro ou -1 (erro)
timer_set       Calcula e escreve mtimecmp = mtime + 10.000.000
uart_putc       Envia 1 byte pela UART (polling no LSR)
uart_puts       Envia string null-terminated
```

---

## Mapa de Memória

Plataforma **QEMU virt** — periféricos acessados por escrita direta nos endereços físicos:

| Periférico | Endereço base | Descrição |
|---|---|---|
| RAM | `0x80000000` | 128 MB — onde código e dados residem |
| UART0 NS16550 | `0x10000000` | Comunicação serial |
| PLIC | `0x0C000000` | Platform-Level Interrupt Controller |
| CLINT mtime | `0x0200BFF8` | Contador de 64 bits (somente leitura) |
| CLINT mtimecmp | `0x02004000` | Comparador de 64 bits (leitura/escrita) |

---

## Arquitetura do Software

```
_start (10 passos de inicialização)
│
├─ 1. la sp, stack_top
├─ 2. csrw mtvec, trap_handler
├─ 3. UART IER = 1  (habilita IRQ de RX)
├─ 4. PLIC priority[IRQ10] = 1
├─ 5. PLIC enable bit10 = 1
├─ 6. PLIC threshold = 0
├─ 7. jal timer_set  (mtimecmp = mtime + 10M)
├─ 8. csrs mie, MTIE|MEIE
├─ 9. csrs mstatus, MIE
└─ 10. banner + display_time
         │
         ▼
main_loop: wfi  ←──────────────────────────┐
                                            │
trap_handler (.align 4)                     │
├── salva 19 regs na pilha (80 bytes)       │
├── csrr mcause                             │
│   ├── 0x80000007 → isr_timer             │
│   │   ├── timer_set  (+1 segundo)        │
│   │   ├── tick_second (SS→MM→HH)         │
│   │   └── display_time (\rHH:MM:SS)      │
│   └── 0x8000000B → isr_external          │
│       ├── PLIC claim → s0 = IRQ ID       │
│       ├── lê UART_RHR → s1 = byte        │
│       ├── eco + bufferiza em g_rxbuf      │
│       ├── CR/LF → parse_command          │
│       └── PLIC complete (sw s0)          │
└── restaura 19 regs + mret ───────────────┘
```

---

## Como Executar

### Pré-requisito

[Docker Desktop](https://www.docker.com/products/docker-desktop/) instalado e em execução.

### Build e execução (uma linha)

```bash
docker build --no-cache -t riscv-clock . && docker run -it --rm riscv-clock
```

### Compilação nativa (Linux)

```bash
# Instalar dependências (Ubuntu/Debian)
sudo apt install gcc-riscv64-unknown-elf qemu-system-misc make

# Compilar
make

# Executar
make run

# Ver disassembly anotado
make dump

# Limpar binários
make clean
```

### Flags de compilação

```
-march=rv32ima   Arquitetura RV32 com extensões M (mul/div) e A (atômicas)
-mabi=ilp32      ABI de 32 bits
-nostdlib        Sem biblioteca C padrão
-nostartfiles    Sem crt0.o (ponto de entrada é nosso _start)
-T link.ld       Usa o linker script do projeto
-lgcc            Suporte a instruções complexas (mul, div)
```

---

## Uso do Relógio

### Saída esperada ao iniciar

```
=== Relogio Digital RISC-V Bare Metal ===
Ajuste: T HH:MM:SS  (ex: T 14:30:00)
Sair  : Ctrl-A, depois X

00:00:01
```

### Ajustar a hora

Digite o comando diretamente no terminal e pressione **Enter**:

```
T 14:30:00
```

| Posição | Conteúdo | Exemplo |
|---|---|---|
| 0 | Letra `T` maiúscula | `T` |
| 1 | Espaço | ` ` |
| 2–3 | Horas (00–23) | `14` |
| 4 | Dois-pontos | `:` |
| 5–6 | Minutos (00–59) | `30` |
| 7 | Dois-pontos | `:` |
| 8–9 | Segundos (00–59) | `00` |

### Encerrar o QEMU

```
Ctrl-A   →   X
```

---

## Detalhes Técnicos

### Timer — leitura segura de 64 bits em CPU de 32 bits

O registrador `mtime` tem 64 bits. Em RV32 ele é lido em dois acessos de 32 bits. Se ocorrer carry entre as leituras o valor seria inconsistente. A solução é reler o HIGH após o LOW e repetir se houver diferença:

```asm
ts_retry:
    lw  t3, 0(t1)        # HIGH (1ª leitura)
    lw  t2, 0(t0)        # LOW
    lw  t4, 0(t1)        # HIGH (2ª leitura)
    bne t3, t4, ts_retry # carry: tenta de novo
```

### Timer — escrita atômica em mtimecmp

Para evitar disparo espúrio durante a escrita dos 64 bits em dois passos, o HIGH é inflado para `0x7FFFFFFF` antes de escrever o LOW:

```asm
sw  t6, 0(t1)   # HIGH = MAX (bloqueia disparo)
sw  t5, 0(t0)   # LOW definitivo
sw  t4, 0(t1)   # HIGH definitivo
```

### PLIC — preservação do IRQ ID entre Claim e Complete

O ID retornado no Claim é armazenado em `s0` (callee-saved), que sobrevive a todas as chamadas `jal` intermediárias:

```asm
lw  s0, 0(t0)        # Claim → s0
jal parse_command    # s0 preservado
sw  s0, 0(t0)        # Complete correto
```

### UART — offsets em bytes

O NS16550 é byte-addressable. O `IER` está no offset **byte 1** (não word 1 = byte 4). O offset correto é `1`.

---

## Referências

- [RISC-V Privileged Specification v20211203](https://github.com/riscv/riscv-isa-manual)
- [QEMU virt RISC-V](https://www.qemu.org/docs/master/system/riscv/virt.html)
- [NS16550 UART Datasheet (TI PC16550D)](https://www.ti.com/lit/ds/symlink/pc16550d.pdf)
- [SiFive PLIC Specification](https://github.com/riscv/riscv-plic-spec)
- [RISC-V Unprivileged ISA Manual](https://github.com/riscv/riscv-isa-manual/releases/latest)
