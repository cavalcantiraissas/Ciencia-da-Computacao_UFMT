/* ============================================================
 * plic.h — Driver do PLIC para QEMU virt (rv64)
 *
 * Mapa de registradores conforme especificação do PLIC
 * (SiFive/QEMU virt), contexto 0 = hart 0, modo Machine.
 * ============================================================ */
#ifndef PLIC_H
#define PLIC_H

#include "types.h"

/* ---- Endereço base do PLIC (QEMU virt) ---- */
#define PLIC_BASE   0x0C000000UL

/* ---- Prioridade de cada fonte (4 bytes por fonte, a partir da 1) ---- */
#define PLIC_PRIORITY(id)   (*(volatile uint32_t *)(PLIC_BASE + 4UL * (id)))

/* ---- Habilitação de fontes 0-31 para o contexto 0 ---- */
#define PLIC_ENABLE_CTX0    (*(volatile uint32_t *)(PLIC_BASE + 0x2000UL))

/* ---- Threshold e claim/complete do contexto 0 ---- */
#define PLIC_THRESHOLD_CTX0 (*(volatile uint32_t *)(PLIC_BASE + 0x200000UL))
#define PLIC_CLAIM_CTX0     (*(volatile uint32_t *)(PLIC_BASE + 0x200004UL))

/* ---- Fonte de interrupção do UART0 no QEMU virt ---- */
#define PLIC_UART0_IRQ  10

/* ---- API do driver ---- */
void plic_init(void);
uint32_t plic_claim(void);
void plic_complete(uint32_t source);

#endif /* PLIC_H */
