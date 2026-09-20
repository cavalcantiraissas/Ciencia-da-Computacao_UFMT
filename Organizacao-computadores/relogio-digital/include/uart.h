/* ============================================================
 * uart.h — Driver do UART NS16550A para QEMU virt (rv64)
 *
 * Endereço base e registradores conforme QEMU virt machine
 * (ver relatorio.md, seção 2.1).
 * ============================================================ */
#ifndef UART_H
#define UART_H

#include "types.h"

/* ---- Endereço base do UART0 (QEMU virt) ---- */
#define UART_BASE   0x10000000UL

/* ---- Acesso a um registrador de 8 bits do UART ---- */
#define UART_REG(offset) (*(volatile uint8_t *)(UART_BASE + (offset)))

/* ---- Offsets dos registradores (DLAB=0, salvo indicação) ---- */
#define UART_RBR    0   /* Receiver Buffer Register (leitura) */
#define UART_THR    0   /* Transmitter Holding Register (escrita) */
#define UART_IER    1   /* Interrupt Enable Register */
#define UART_FCR    2   /* FIFO Control Register (escrita) */
#define UART_LCR    3   /* Line Control Register */
#define UART_LSR    5   /* Line Status Register */

/* ---- Offsets válidos apenas quando LCR.DLAB = 1 ---- */
#define UART_DLL    0   /* Divisor Latch LSB */
#define UART_DLM    1   /* Divisor Latch MSB */

/* ---- Bits do LCR ---- */
#define UART_LCR_DLAB  0x80  /* Divisor Latch Access Bit */
#define UART_LCR_8N1   0x03  /* 8 bits de dados, sem paridade, 1 stop bit */

/* ---- Bits do IER ---- */
#define UART_IER_RDI   0x01  /* Receiver Data Available Interrupt */

/* ---- Bits do LSR ---- */
#define UART_LSR_DR    0x01  /* Data Ready */
#define UART_LSR_THRE  0x20  /* Transmit Holding Register Empty */

/* ---- Buffer de recepção compartilhado com main.c ---- */
#define UART_RX_BUF_SIZE 32

extern volatile char uart_rx_buf[UART_RX_BUF_SIZE];
extern volatile int  uart_rx_idx;
extern volatile int  uart_cmd_ready;

/* ---- API do driver ---- */
void uart_init(void);
void uart_putc(char c);
void uart_puts(const char *s);
void uart_put2digit(int n);
void uart_interrupt_handler(void);

#endif /* UART_H */
