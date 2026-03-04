/* ============================================================
 * timer.c — Driver do temporizador CLINT para QEMU virt (rv64)
 *
 * O CLINT (Core Local Interruptor) fornece dois registradores
 * mapeados em memória:
 *   mtime    — contador de 64 bits livre, incrementado a
 *              10 MHz pelo hardware do QEMU
 *   mtimecmp — comparador de 64 bits; quando mtime >= mtimecmp
 *              a interrupção MTIP (Machine Timer Interrupt
 *              Pending, mcause = 7) é gerada.
 *
 * Estratégia:
 *   - timer_init():    configura mtimecmp = mtime + TIMER_FREQ_HZ
 *   - timer_set_next(): avança mtimecmp += TIMER_FREQ_HZ
 *     (sem reler mtime, evitando deriva acumulada)
 * ============================================================ */

#include "timer.h"
#include "uart.h"

/* Variáveis do relógio — acessíveis a partir de main.c       */
volatile int clock_hours   = 0;
volatile int clock_minutes = 0;
volatile int clock_seconds = 0;
volatile int clock_updated = 0;   /* flag: main() deve redesenhar */

/* ============================================================
 * timer_init — Agenda a primeira interrupção de temporizador
 * ============================================================ */
void timer_init(void)
{
    CLINT_MTIMECMP = CLINT_MTIME + TIMER_FREQ_HZ;
}

/* ============================================================
 * timer_set_next — Agenda a próxima interrupção
 *
 * Incrementa mtimecmp em exatamente TIMER_FREQ_HZ ticks para
 * manter período constante de 1 segundo.
 * ============================================================ */
void timer_set_next(void)
{
    CLINT_MTIMECMP += TIMER_FREQ_HZ;
}

/* ============================================================
 * timer_interrupt_handler — ISR do temporizador (1 s)
 *
 * Chamada pela rotina geral de interrupções (trap.c) ao
 * detectar mcause == IRQ_M_TIMER.
 *
 * Sequência de operações:
 *   1. Agendar próxima interrupção (limpa MTIP escrevendo em
 *      mtimecmp — a única forma de limpar esta interrupção)
 *   2. Incrementar segundos, propagando carry para minutos e
 *      horas conforme as regras do relógio (00–59 s/min,
 *      00–23 h, retorno a 00:00:00 após 23:59:59)
 *   3. Levantar clock_updated para que o loop principal
 *      redesenhe a hora via UART
 * ============================================================ */
void timer_interrupt_handler(void)
{
    /* 1. Reagendar (também limpa a interrupção pendente) */
    timer_set_next();

    /* 2. Atualizar contadores do relógio */
    clock_seconds++;
    if (clock_seconds >= 60) {
        clock_seconds = 0;
        clock_minutes++;
        if (clock_minutes >= 60) {
            clock_minutes = 0;
            clock_hours++;
            if (clock_hours >= 24) {
                clock_hours = 0;
            }
        }
    }

    /* 3. Sinalizar main() para exibir a nova hora */
    clock_updated = 1;
}
