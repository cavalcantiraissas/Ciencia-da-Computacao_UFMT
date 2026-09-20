/* ============================================================
 * trap.h — Despachador geral de interrupções e exceções
 * ============================================================ */
#ifndef TRAP_H
#define TRAP_H

/* ---- Códigos de causa em mcause (interrupções, bit 63 = 1) ---- */
#define IRQ_M_TIMER  7   /* Machine Timer Interrupt (MTIP) */
#define IRQ_M_EXT    11  /* Machine External Interrupt (MEIP, via PLIC) */

void trap_handler(void);

#endif /* TRAP_H */
