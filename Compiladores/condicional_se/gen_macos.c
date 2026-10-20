/**
 * @file gen_macos.c
 * @brief Modulo gerador de codigo para macOS x86-64. Reaproveita todo o
 *        gerador (gen.c), trocando apenas os numeros das chamadas de sistema.
 */

#define TARGET_MACOS
#include "gen.c"
