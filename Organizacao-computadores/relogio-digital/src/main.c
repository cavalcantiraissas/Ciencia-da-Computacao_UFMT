/* ============================================================
 * main.c — Relógio Digital em RISC-V Bare Metal
 *
 * Fluxo geral:
 *   1. Inicializar periféricos (UART, Timer, PLIC)
 *   2. Habilitar interrupções de timer e externa (PLIC/UART)
 *   3. Habilitar interrupções globais (mstatus.MIE)
 *   4. Loop principal: aguardar flags levantadas pelas ISRs
 *      e tomar a ação correspondente
 *
 * Flags:
 *   clock_updated  — levantada pela ISR do timer (timer.c)
 *                    → redesenhar hora no UART
 *   uart_cmd_ready — levantada pela ISR do UART (uart.c)
 *                    → interpretar e aplicar comando "T HH:MM:SS"
 * ============================================================ */

#include "types.h"
#include "uart.h"
#include "timer.h"
#include "plic.h"

/* Declarações externas (definidas em timer.c) */
extern volatile int clock_hours;
extern volatile int clock_minutes;
extern volatile int clock_seconds;
extern volatile int clock_updated;

/* ============================================================
 * display_time — Exibe "HH:MM:SS" na UART
 *
 * Usa '\r' (carriage return sem newline) para sobrescrever
 * sempre a mesma linha do terminal, dando o efeito de relógio
 * em tempo real sem scroll.
 * ============================================================ */
static void display_time(void)
{
    uart_putc('\r');
    uart_put2digit(clock_hours);
    uart_putc(':');
    uart_put2digit(clock_minutes);
    uart_putc(':');
    uart_put2digit(clock_seconds);
    uart_puts("  ");   /* espaços para limpar caracteres residuais */
}

/* ============================================================
 * is_digit — Verifica se c é dígito decimal
 * ============================================================ */
static int is_digit(char c)
{
    return (c >= '0' && c <= '9');
}

/* ============================================================
 * parse_command — Interpreta e aplica o comando "T HH:MM:SS"
 *
 * Formato esperado: T HH:MM:SS  (10 caracteres + '\0')
 *   Índice: 0='T', 1=' ', 2-3=HH, 4=':', 5-6=MM, 7=':', 8-9=SS
 *
 * Validações:
 *   - Prefixo "T "
 *   - Separadores ':' nas posições corretas
 *   - Todos os dígitos são numéricos
 *   - HH em [0,23], MM em [0,59], SS em [0,59]
 *
 * Em caso de sucesso, atualiza clock_hours/minutes/seconds.
 * Em caso de erro, exibe mensagem de ajuda.
 * ============================================================ */
static void parse_command(void)
{
    /* Trabalhar com cópia local para evitar aliasing volátil */
    char buf[UART_RX_BUF_SIZE];
    int i;

    /* Copiar buffer volátil para local */
    for (i = 0; i < UART_RX_BUF_SIZE; i++) {
        buf[i] = (char)uart_rx_buf[i];
        if (buf[i] == '\0') break;
    }

    /* Validar comprimento mínimo: "T HH:MM:SS" = 10 chars */
    if (i < 10) goto cmd_error;

    /* Validar estrutura: "T HH:MM:SS" */
    if (buf[0] != 'T' || buf[1] != ' ')    goto cmd_error;
    if (!is_digit(buf[2]) || !is_digit(buf[3])) goto cmd_error;
    if (buf[4] != ':')                      goto cmd_error;
    if (!is_digit(buf[5]) || !is_digit(buf[6])) goto cmd_error;
    if (buf[7] != ':')                      goto cmd_error;
    if (!is_digit(buf[8]) || !is_digit(buf[9])) goto cmd_error;

    {
        int h = (buf[2] - '0') * 10 + (buf[3] - '0');
        int m = (buf[5] - '0') * 10 + (buf[6] - '0');
        int s = (buf[8] - '0') * 10 + (buf[9] - '0');

        if (h < 0 || h > 23) goto cmd_error;
        if (m < 0 || m > 59) goto cmd_error;
        if (s < 0 || s > 59) goto cmd_error;

        /* Atualizar relógio — desabilitar interrupções para
         * evitar atualização parcial visível pela ISR do timer */
        clear_csr(mstatus, MSTATUS_MIE);
        clock_hours   = h;
        clock_minutes = m;
        clock_seconds = s;
        set_csr(mstatus, MSTATUS_MIE);

        uart_puts("\r\nHora atualizada para ");
        uart_put2digit(h); uart_putc(':');
        uart_put2digit(m); uart_putc(':');
        uart_put2digit(s);
        uart_puts("\r\n");
    }

    goto cmd_done;

cmd_error:
    uart_puts("\r\nComando invalido. Use: T HH:MM:SS\r\n");
    uart_puts("Exemplo: T 14:30:00\r\n");

cmd_done:
    /* Resetar buffer e flag */
    uart_rx_idx    = 0;
    uart_cmd_ready = 0;
}

/* ============================================================
 * main — Ponto de entrada do programa (após start.S)
 * ============================================================ */
int main(void)
{
    /* ----------------------------------------------------- *
     * 1. Inicializar periféricos                            *
     * ----------------------------------------------------- */
    uart_init();
    timer_init();
    plic_init();

    /* ----------------------------------------------------- *
     * 2. Mensagem de boas-vindas                            *
     * ----------------------------------------------------- */
    uart_puts("\r\n=== Relogio Digital RISC-V Bare Metal ===\r\n");
    uart_puts("Para ajustar a hora, envie: T HH:MM:SS\r\n");
    uart_puts("Exemplo: T 14:30:00\r\n");
    uart_puts("Iniciando em 00:00:00...\r\n");

    /* ----------------------------------------------------- *
     * 3. Habilitar interrupções no CSR mie:                 *
     *    - MTIE (bit 7) → timer do CLINT                   *
     *    - MEIE (bit 11)→ externos via PLIC (UART)         *
     * ----------------------------------------------------- */
    write_csr(mie, MIE_MTIE | MIE_MEIE);

    /* ----------------------------------------------------- *
     * 4. Habilitar interrupções globais (mstatus.MIE)       *
     * ----------------------------------------------------- */
    set_csr(mstatus, MSTATUS_MIE);

    /* ----------------------------------------------------- *
     * 5. Loop principal                                     *
     *                                                       *
     * O processador permanece neste loop a maior parte do   *
     * tempo.  A lógica de tempo e recepção ocorre           *
     * exclusivamente nas ISRs; o loop apenas reage a flags. *
     * ----------------------------------------------------- */
    while (1) {

        /* Redesenhar hora ao ser sinalizado pela ISR do timer */
        if (clock_updated) {
            clock_updated = 0;
            display_time();
        }

        /* Processar comando recebido via UART */
        if (uart_cmd_ready) {
            parse_command();
            /* Redesenhar imediatamente após ajuste manual */
            display_time();
        }
    }

    /* Nunca alcançado */
    return 0;
}
