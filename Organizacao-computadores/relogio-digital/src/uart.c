/* ============================================================
 * uart.c — Driver do UART NS16550A para QEMU virt (rv64)
 *
 * Fornece:
 *   - Inicialização do UART com interrupção de recepção
 *   - Funções de transmissão (polling sobre THRE)
 *   - Rotina de interrupção de recepção (ISR)
 *   - Buffer circular de recepção e detecção de comando
 * ============================================================ */

#include "uart.h"

/* ---- Buffer de recepção e flags compartilhados com main.c ---- */
volatile char uart_rx_buf[UART_RX_BUF_SIZE];
volatile int  uart_rx_idx   = 0;
volatile int  uart_cmd_ready = 0;

/* ============================================================
 * uart_init — Inicializa o NS16550A
 *
 * Configuração:
 *   - Baud rate: divisor = 1 (QEMU não simula timing real)
 *   - Frame: 8 bits de dados, sem paridade, 1 stop bit (8N1)
 *   - FIFO habilitado e resetado
 *   - Interrupção de recepção (RDI) habilitada
 * ============================================================ */
void uart_init(void)
{
    /* Desabilitar todas as interrupções durante a configuração */
    UART_REG(UART_IER) = 0x00;

    /* Habilitar acesso ao Divisor Latch (DLAB=1) */
    UART_REG(UART_LCR) = UART_LCR_DLAB;

    /* Configurar divisor de baud rate = 1 */
    UART_REG(UART_DLL) = 0x01;   /* LSB */
    UART_REG(UART_DLM) = 0x00;   /* MSB */

    /* Configurar 8N1, desabilitar DLAB */
    UART_REG(UART_LCR) = UART_LCR_8N1;

    /* Habilitar e resetar FIFOs de TX e RX */
    UART_REG(UART_FCR) = 0x07;

    /* Habilitar interrupção de dado disponível na recepção */
    UART_REG(UART_IER) = UART_IER_RDI;
}

/* ============================================================
 * uart_putc — Transmite um caractere (polling)
 *
 * Aguarda o bit THRE (Transmit Holding Register Empty) no LSR
 * para garantir que o transmissor está livre antes de escrever.
 * ============================================================ */
void uart_putc(char c)
{
    while (!(UART_REG(UART_LSR) & UART_LSR_THRE))
        ;
    UART_REG(UART_THR) = (uint8_t)c;
}

/* ============================================================
 * uart_puts — Transmite uma string terminada em '\0'
 * ============================================================ */
void uart_puts(const char *s)
{
    while (*s)
        uart_putc(*s++);
}

/* ============================================================
 * uart_put2digit — Transmite um número de 0 a 99 com 2 dígitos
 * ============================================================ */
void uart_put2digit(int n)
{
    uart_putc('0' + (n / 10) % 10);
    uart_putc('0' + (n % 10));
}

/* ============================================================
 * uart_interrupt_handler — ISR de recepção do UART
 *
 * Chamada pela rotina geral de interrupções (trap.c) sempre
 * que o PLIC sinalizar a fonte 10 (UART0).
 *
 * Drena todos os bytes disponíveis no FIFO de recepção (loop
 * enquanto LSR.DR = 1).  Cada caractere é armazenado no
 * buffer uart_rx_buf.  Ao receber '\r' ou '\n', o buffer é
 * encerrado com '\0' e uart_cmd_ready é levantado para que
 * main() interprete o comando.
 * ============================================================ */
void uart_interrupt_handler(void)
{
    /* Drenar o FIFO de recepção por completo */
    while (UART_REG(UART_LSR) & UART_LSR_DR) {

        char c = (char)UART_REG(UART_RBR);

        if (c == '\r' || c == '\n') {
            /* Fim de linha: finalizar string e sinalizar comando */
            if (uart_rx_idx > 0) {
                uart_rx_buf[uart_rx_idx] = '\0';
                uart_cmd_ready = 1;
                /* uart_rx_idx é resetado por parse_command() em main.c */
            }
        } else {
            /* Armazenar caractere se houver espaço */
            if (uart_rx_idx < UART_RX_BUF_SIZE - 1) {
                uart_rx_buf[uart_rx_idx++] = c;
            }
        }
    }
}
