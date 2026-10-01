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

// Prototipos
void genAdd(void);
void genSub(void);
void genMult(void);
void genDiv(void);
void genNum(char num_string[MAX_TOKEN]);
void gen_data_section(void);

#endif //_GEN_H_