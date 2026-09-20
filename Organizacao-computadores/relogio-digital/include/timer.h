/* ============================================================
 * timer.h — Driver do temporizador CLINT para QEMU virt (rv64)
 *
 * Endereços conforme QEMU virt machine (ver relatorio.md,
 * seção 3.3): mtime em CLINT_BASE+0xBFF8, mtimecmp (hart 0)
 * em CLINT_BASE+0x4000.
 * ============================================================ */
#ifndef TIMER_H
#define TIMER_H

#include "types.h"

/* ---- Endereço base do CLINT (QEMU virt) ---- */
#define CLINT_BASE      0x02000000UL

/* ---- Registradores de 64 bits do CLINT (hart 0) ---- */
#define CLINT_MTIME     (*(volatile uint64_t *)(CLINT_BASE + 0xBFF8UL))
#define CLINT_MTIMECMP  (*(volatile uint64_t *)(CLINT_BASE + 0x4000UL))

/* ---- Frequência do CLINT: 10 MHz => 1 s = 10.000.000 ticks ---- */
#define TIMER_FREQ_HZ   10000000UL

/* ---- Variáveis do relógio, acessíveis a partir de main.c ---- */
extern volatile int clock_hours;
extern volatile int clock_minutes;
extern volatile int clock_seconds;
extern volatile int clock_updated;

/* ---- API do driver ---- */
void timer_init(void);
void timer_set_next(void);
void timer_interrupt_handler(void);

#endif /* TIMER_H */
