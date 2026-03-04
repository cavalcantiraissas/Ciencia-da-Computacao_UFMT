# Relatório Técnico - Trabalho 2
## Relógio Digital em RISC-V Bare Metal com UART e Interrupções

---

## 1. Introdução

Este relatório descreve a implementação de um relógio digital funcional em programação *bare metal* para a arquitetura RISC-V 64 bits. O sistema executa diretamente sobre o hardware emulado pelo QEMU (máquina `virt`), sem sistema operacional, sem bibliotecas de alto nível e sem rotinas externas. A comunicação com o usuário é feita via UART serial, e a contagem de tempo é realizada exclusivamente por meio de interrupções periódicas de um temporizador de hardware.

---

## 2. Arquitetura do Sistema

### 2.1 Plataforma de Hardware Emulado

O QEMU emula a máquina `virt`, que é uma plataforma de referência RISC-V com os seguintes periféricos relevantes:

| Periférico | Endereço Base | Descrição |
|---|---|---|
| UART NS16550A | `0x10000000` | Comunicação serial bidirecional |
| CLINT | `0x02000000` | Core Local Interruptor (timer) |
| PLIC | `0x0C000000` | Platform-Level Interrupt Controller |

O kernel ELF é carregado pelo QEMU no endereço `0x80000000` (DRAM), e a execução começa diretamente no símbolo `_start`, sem intermediários como o OpenSBI.

### 2.2 Mapa de Memória (Linker Script)

O script de ligação `linker.ld` define o layout das seções em memória:

```
0x80000000  .text       (código: start.S primeiro, depois demais)
            .rodata     (strings, constantes)
            .data       (variáveis inicializadas)
            .bss        (variáveis não inicializadas, zeradas em boot)
            pilha       (64 KB, crescimento descendente)
```

A seção `.text.start` foi criada para garantir que o código de inicialização (`_start`) seja sempre o primeiro a ser ligado e, portanto, fique no endereço de entrada `0x80000000`.

---

## 3. Estrutura do Programa

O projeto é organizado nos seguintes módulos:

### 3.1 `start.S` — Inicialização e Entrada de Trap

Este arquivo assembly tem duas responsabilidades críticas:

**`_start`** — rotina de inicialização:
1. Configura o ponteiro de pilha (`sp`) para o topo da área reservada pelo linker
2. Instala o vetor de trap (`mtvec`) apontando para `trap_entry` no modo direto (bits [1:0] = `00`)
3. Zera a seção BSS iterando de `_bss_start` até `_bss_end`
4. Chama `main()`

**`trap_entry`** — stub de salvamento/restauração de contexto:

Quando qualquer interrupção ou exceção ocorre, o hardware RISC-V salta automaticamente para o endereço em `mtvec`. O stub executa:

1. Aloca 256 bytes na pilha (32 registradores × 8 bytes)
2. Salva todos os 31 registradores de uso geral (x1–x31) na pilha
3. Chama `trap_handler()` em C
4. Restaura todos os registradores
5. Executa `mret` para retornar ao ponto de interrupção

O salvamento completo de contexto é necessário porque a ISR em C pode modificar qualquer registrador ao chamar subfunções.

### 3.2 `trap.c` — Despachador Geral de Interrupções

```c
void trap_handler(void) {
    unsigned long mcause = read_csr(mcause);
    int is_interrupt = (mcause >> 63) & 1;
    unsigned long cause = mcause & ~(1UL << 63);

    if (is_interrupt) {
        if (cause == IRQ_M_TIMER)  timer_interrupt_handler();
        else if (cause == IRQ_M_EXT) {
            uint32_t src = plic_claim();
            if (src == PLIC_UART0_IRQ) uart_interrupt_handler();
            plic_complete(src);
        }
    } else {
        while (1); // exceção não tratada: halt
    }
}
```

O CSR `mcause` codifica a causa do trap: o bit 63 distingue interrupção (1) de exceção síncrona (0); os demais bits identificam a causa específica. Para interrupções:
- Causa `7` → Interrupção de Timer (MTIP)
- Causa `11` → Interrupção Externa via PLIC (MEIP)

### 3.3 `timer.c` — Driver do Temporizador e Lógica do Relógio

O CLINT fornece dois registradores de 64 bits mapeados em memória:
- `mtime` (`0x200BFF8`): contador livre incrementado pelo hardware a 10 MHz
- `mtimecmp` (`0x2004000`): quando `mtime >= mtimecmp`, o bit MTIP é setado no CSR `mip`, gerando a interrupção de timer

**Inicialização:**
```c
void timer_init(void) {
    CLINT_MTIMECMP = CLINT_MTIME + TIMER_FREQ_HZ; // 1 s = 10.000.000 ticks
}
```

**ISR do timer:**
```c
void timer_interrupt_handler(void) {
    CLINT_MTIMECMP += TIMER_FREQ_HZ; // limpa MTIP e agenda próxima
    clock_seconds++;
    if (clock_seconds >= 60) { clock_seconds = 0; clock_minutes++; }
    if (clock_minutes >= 60) { clock_minutes = 0; clock_hours++;   }
    if (clock_hours   >= 24)   clock_hours   = 0;
    clock_updated = 1;
}
```

A estratégia de incrementar `mtimecmp` (em vez de reler `mtime + freq`) é importante para evitar deriva acumulada ao longo do tempo.

A única forma de limpar a interrupção de timer no RISC-V é escrever em `mtimecmp` com um valor maior que o `mtime` atual — não existe bit de flag para limpar manualmente.

### 3.4 `uart.c` — Driver UART NS16550A

O NS16550A é um UART clássico com registradores de 8 bits mapeados em memória.

**Inicialização:**
1. Habilitar acesso ao divisor de baud rate (DLAB=1 no LCR)
2. Configurar divisor = 1 (QEMU não simula temporização real)
3. Configurar frame 8N1: 8 bits de dados, sem paridade, 1 stop bit
4. Habilitar FIFO de recepção e transmissão
5. Habilitar interrupção de dado disponível (IER.RDI=1)
6. Limpar DLAB

**Transmissão (polling):**
```c
void uart_putc(char c) {
    while (!(UART_REG(UART_LSR) & UART_LSR_THRE));
    UART_REG(UART_THR) = c;
}
```
A transmissão usa polling sobre o bit THRE (Transmit Holding Register Empty) do LSR — aceitável pois transmissão não precisa de baixa latência nesta aplicação.

**ISR de recepção:**
```c
void uart_interrupt_handler(void) {
    while (UART_REG(UART_LSR) & UART_LSR_DR) {
        char c = UART_REG(UART_RBR);
        if (c == '\r' || c == '\n') {
            uart_rx_buf[uart_rx_idx] = '\0';
            uart_cmd_ready = 1;
        } else {
            uart_rx_buf[uart_rx_idx++] = c;
        }
    }
}
```
O FIFO é drenado completamente em cada interrupção para evitar overflow e novas interrupções perdidas.

### 3.5 `plic.c` — Driver do PLIC

O PLIC roteia interrupções de periféricos para os harts. O UART0 é a fonte 10 no QEMU virt.

**Inicialização:**
```c
void plic_init(void) {
    PLIC_PRIORITY(PLIC_UART0_IRQ) = 1;      // prioridade 1 para UART0
    PLIC_ENABLE_CTX0 = (1U << PLIC_UART0_IRQ); // habilitar no contexto 0
    PLIC_THRESHOLD_CTX0 = 0;                 // aceitar qualquer prioridade >= 1
}
```

**Protocolo de claim/complete:**
1. `plic_claim()` — lê o ID da fonte pendente (e "toma posse" atomicamente)
2. Tratar a interrupção
3. `plic_complete(id)` — escreve o ID de volta para sinalizar conclusão ao PLIC

Sem o `complete`, o PLIC não gerará novas interrupções da mesma fonte.

### 3.6 `main.c` — Loop Principal

```c
int main(void) {
    uart_init();
    timer_init();
    plic_init();

    // Exibir mensagem inicial
    uart_puts("=== Relogio Digital RISC-V Bare Metal ===\r\n");

    // Habilitar MTIE (timer) e MEIE (externo) no CSR mie
    write_csr(mie, MIE_MTIE | MIE_MEIE);

    // Habilitar interrupções globais (mstatus.MIE)
    set_csr(mstatus, MSTATUS_MIE);

    while (1) {
        if (clock_updated) {
            clock_updated = 0;
            display_time();
        }
        if (uart_cmd_ready) {
            parse_command();
            display_time();
        }
    }
}
```

A ordem de inicialização é importante: os periféricos são configurados antes de habilitar as interrupções globais para evitar que uma interrupção seja atendida antes que o hardware esteja pronto.

---

## 4. Habilitação de Interrupções no RISC-V

O RISC-V possui três níveis de controle de interrupções:

1. **CSR `mie`** (Machine Interrupt Enable): habilita tipos específicos
   - Bit 7 (`MTIE`): interrupção de timer
   - Bit 11 (`MEIE`): interrupção externa (PLIC)

2. **CSR `mstatus`** (bit 3, `MIE`): chave global de interrupções em M-mode

3. **PLIC**: habilita/desabilita fontes externas individuais por contexto

Para uma interrupção de UART ser recebida, os três níveis devem estar habilitados:
- `mie.MEIE = 1`
- `mstatus.MIE = 1`
- PLIC: source 10 habilitado no contexto 0

Para a interrupção de timer:
- `mie.MTIE = 1`
- `mstatus.MIE = 1`
- `mtimecmp` com valor futuro (menor que `mtime` atual)

---

## 5. Processamento do Comando de Ajuste

O formato do comando é `T HH:MM:SS`. O `parse_command()` valida:
- Caracteres 0-1: `"T "`
- Caracteres 2-3: dígitos para hora
- Caractere 4: `':'`
- Caracteres 5-6: dígitos para minuto
- Caractere 7: `':'`
- Caracteres 8-9: dígitos para segundo
- Valores dentro das faixas válidas

Durante a atualização dos contadores, as interrupções globais são brevemente desabilitadas (`clear_csr(mstatus, MSTATUS_MIE)`) para garantir que a ISR do timer não leia um estado parcialmente atualizado, e reabilitadas em seguida.

---

## 6. Ambiente de Build e Execução

O toolchain e o QEMU são encapsulados em um container Docker baseado em Ubuntu 22.04, eliminando a necessidade de instalação local de ferramentas RISC-V. Isso garante reprodutibilidade independente do sistema operacional host (macOS, Windows ou Linux).

**Flags de compilação relevantes:**
- `-march=rv64imac_zicsr`: ISA alvo com extensões de CSR explícitas
- `-mabi=lp64`: ABI de 64 bits sem FPU
- `-mcmodel=medany`: modelo de código para endereços arbitrários de 64 bits
- `-ffreestanding -nostdlib -nostartfiles`: sem runtime de C standard

---

## 7. Conclusão

O sistema implementa todos os requisitos especificados:

- Exibição contínua no formato `HH:MM:SS` via UART
- Contagem baseada exclusivamente em interrupções de temporizador
- Recepção de comandos via interrupção de UART
- Ajuste de hora com o comando `T HH:MM:SS`
- Respeito às faixas de valores (00–23 h, 00–59 m/s, retorno a 00:00:00)
- Estrutura com inicialização, loop principal, rotina geral de interrupção e rotinas específicas para timer e UART
- Execução bare metal sem SO ou bibliotecas externas
