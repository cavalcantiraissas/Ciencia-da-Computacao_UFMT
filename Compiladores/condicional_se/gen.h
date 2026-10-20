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
void genAdd(int);
void genSub(int);
void genMult(int);
void genDiv(int);
void genNeg(int);
void genIntToFloat(int);
void genNum(char num_string[MAX_TOKEN]);
void genFloatNum(char num_string[MAX_TOKEN]);
void gen_push_var(char*, int);
void gen_assign(char*, int);
void gen_assign_string(char*, char*);
void gen_preambule(void);
void gen_data_section(void);
void gen_preambule_code(void);
void gen_epilog_code(void);
void gen_runtime(void);
void gen_read(char*, int);
void gen_write(char*, int);
void gen_label_name(char*);
void gen_label(char*);
void gen_cond_jump(char*);
void gen_incond_jump(char*);
void gen_bool(int, int);
void gen_bool_label_name(char*);
void gen_logic_begin(int, char*);

#endif //_GEN_H_