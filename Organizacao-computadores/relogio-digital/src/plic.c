/* ============================================================
 * plic.c — Driver do PLIC para QEMU virt (rv64)
 *
 * O PLIC (Platform-Level Interrupt Controller) roteia
 * interrupções de periféricos para os harts.  Cada fonte
 * possui uma prioridade (1–7) e cada contexto (hart × modo)
 * possui um registrador de threshold; apenas fontes com
 * prioridade > threshold geram interrupção no contexto.
 *
 * Contexto 0 = hart 0, modo M (Machine) — usado aqui.
 *
 * Fluxo para tratar uma interrupção externa (MEIE):
 *   1. plic_claim()    — lê o ID da fonte de maior prioridade
 *   2. tratar a fonte  — chamar ISR específica
 *   3. plic_complete() — escrever o ID de volta (acknowledge)
 * ============================================================ */

#include "plic.h"

/* ============================================================
 * plic_init — Configura prioridade e habilitação para UART0
 * ============================================================ */
void plic_init(void)
{
    /* Definir prioridade da fonte UART0 (source 10) como 1 */
    PLIC_PRIORITY(PLIC_UART0_IRQ) = 1;

    /* Habilitar source 10 no contexto 0 (hart 0, M-mode).
     * O registrador de 32 bits cobre as fontes 0–31;
     * bit n corresponde à fonte n.                           */
    PLIC_ENABLE_CTX0 = (1U << PLIC_UART0_IRQ);

    /* Threshold = 0: aceitar interrupções de prioridade >= 1 */
    PLIC_THRESHOLD_CTX0 = 0;
}

/* ============================================================
 * plic_claim — Lê o ID da interrupção pendente (claim)
 *
 * Retorna o ID da fonte de maior prioridade com interrupção
 * pendente habilitada para este contexto.  Uma leitura de
 * claim também atomicamente "toma posse" da interrupção,
 * impedindo que ela seja sinalizada novamente até que
 * plic_complete() seja chamado com o mesmo ID.
 * ============================================================ */
uint32_t plic_claim(void)
{
    return PLIC_CLAIM_CTX0;
}

/* ============================================================
 * plic_complete — Finaliza o tratamento de uma interrupção
 *
 * Escrever o ID de volta no registrador claim/complete
 * sinaliza ao PLIC que a interrupção foi atendida.
 * ============================================================ */
void plic_complete(uint32_t source)
{
    PLIC_CLAIM_CTX0 = source;
}
