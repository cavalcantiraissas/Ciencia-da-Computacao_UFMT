/* ============================================================
 * types.h — Tipos básicos e acesso a CSRs para RISC-V bare metal
 * ============================================================ */
#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>

/* ---- Acesso a Control and Status Registers (CSR) ---- */

#define read_csr(reg)                                        \
    ({                                                        \
        unsigned long __tmp;                                  \
        __asm__ volatile ("csrr %0, " #reg : "=r"(__tmp));     \
        __tmp;                                                 \
    })

#define write_csr(reg, val)                                  \
    ({                                                        \
        __asm__ volatile ("csrw " #reg ", %0" :: "rK"(val));   \
    })

#define set_csr(reg, bit)                                    \
    ({                                                        \
        unsigned long __tmp = (unsigned long)(bit);            \
        __asm__ volatile ("csrs " #reg ", %0" :: "rK"(__tmp));  \
    })

#define clear_csr(reg, bit)                                  \
    ({                                                        \
        unsigned long __tmp = (unsigned long)(bit);            \
        __asm__ volatile ("csrc " #reg ", %0" :: "rK"(__tmp));  \
    })

/* ---- Bits do CSR mstatus ---- */
#define MSTATUS_MIE   (1UL << 3)   /* Machine Interrupt Enable (global) */

/* ---- Bits do CSR mie ---- */
#define MIE_MTIE      (1UL << 7)   /* Machine Timer Interrupt Enable */
#define MIE_MEIE      (1UL << 11)  /* Machine External Interrupt Enable */

#endif /* TYPES_H */
