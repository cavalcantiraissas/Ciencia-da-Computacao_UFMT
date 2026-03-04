/* ============================================================
 * trap.c — Rotina geral de tratamento de interrupções e
 *           exceções (trap handler)
 *
 * Esta função é chamada diretamente pelo stub assembly
 * trap_entry (em start.S) com o contexto de registradores
 * já salvo na pilha.
 *
 * Fluxo:
 *   1. Ler mcause para identificar tipo (interrupção/exceção)
 *      e causa
 *   2. Para interrupções (bit 63 de mcause = 1):
 *        - Causa 7  → timer → timer_interrupt_handler()
 *        - Causa 11 → external (PLIC) → claim, dispatch, complete
 *   3. Exceções não tratadas: loop infinito (halt)
 * ============================================================ */

#include "types.h"
#include "trap.h"
#include "timer.h"
#include "uart.h"
#include "plic.h"

/* ============================================================
 * trap_handler — Despachador principal de traps
 *
 * Identifica a origem do trap por meio do CSR mcause e invoca
 * a rotina específica correspondente.
 * ============================================================ */
void trap_handler(void)
{
    unsigned long mcause = read_csr(mcause);

    /* Bit 63 = 1 indica interrupção; 0 indica exceção síncrona */
    int is_interrupt = (mcause >> 63) & 1;
    unsigned long cause = mcause & ~(1UL << 63);

    if (is_interrupt) {

        if (cause == IRQ_M_TIMER) {
            /* ------------------------------------------------- *
             * Interrupção de Temporizador (MTIP)                *
             * Origem: CLINT — mtime >= mtimecmp                 *
             * ------------------------------------------------- */
            timer_interrupt_handler();

        } else if (cause == IRQ_M_EXT) {
            /* ------------------------------------------------- *
             * Interrupção Externa (MEIP)                        *
             * Origem: PLIC — periférico enviou interrupção      *
             * ------------------------------------------------- */
            uint32_t source = plic_claim();

            if (source == PLIC_UART0_IRQ) {
                /* Fonte 10 = UART0 */
                uart_interrupt_handler();
            }
            /* Acknowledge: informar ao PLIC que a interrupção
             * foi tratada (obrigatório para limpeza do MEIP)   */
            plic_complete(source);
        }
        /* Outras interrupções: ignorar silenciosamente */

    } else {
        /* ------------------------------------------------- *
         * Exceção síncrona não tratada — parar o sistema    *
         * ------------------------------------------------- */
        (void)cause;
        while (1)
            ;
    }
}
