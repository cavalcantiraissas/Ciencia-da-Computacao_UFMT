/**
 * @file gen.h
 * @author Ivairton M. Santos - UFMT - Computacao
 * @brief Modulo do gerador de codigo
 * @version 0.3
 * @date 2022-02-04
 * 
 */
#ifndef _GEN_H_
#define _GEN_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "struct_compiler.h"
#include "symbols.h"

// Quantidade de bytes reservada para cada variavel string (inclui o \0). Cabe
// qualquer constante string aceita pelo lexico (lexema limitado a MAX_CHAR).
#define STRING_VAR_SIZE MAX_CHAR

// Prototipos
void genAdd();
void genSub();
void genMult();
void genDiv();
void genNum(char num_string[MAX_TOKEN]);
void gen_preambule(void);
void gen_data_section(void);
void gen_preambule_code(void);
void gen_epilog_code(void);
void gen_assign(type_symbol_table_entry *var, type_symbol_table_string_entry *str_const);


#endif //_GEN_H_