/**
 * @file gen_win.c
 * @author Ivairton M. Santos - UFMT - Computacao
 * @brief Modulo gerador de codigo para Windows x86-64. Reaproveita todo o
 *        gerador (gen.c), trocando apenas as rotinas de apoio (entrada/saida
 *        com printf/scanf), a entrada e o encerramento do programa.
 * @version 0.3
 * @date 2022-02-23
 * 
 */

#define TARGET_WIN
#include "gen.c"
