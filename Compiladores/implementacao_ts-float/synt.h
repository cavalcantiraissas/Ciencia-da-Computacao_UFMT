/**
 * @file synt.h
 * @author Prof. Ivairton M. Santos - UFMT - Computacao
 * @brief Modulo do analisado sintatico
 * @version 0.4
 * @date 2022-02-04
 * 
 */

#ifndef _SYNT_H_
#define _SYNT_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "struct_compiler.h"
#include "lex.h"
#include "gen.h"
#include "symbols.h"

/*
 * Gramatica:
 *      program -> declarations  statements
 *      declarations -> declaration declarations | [vazio]
 *      declaration -> tipo id ;
 *      tipo -> int | float
 *      statement -> [vazio]
 */


// Prototipos
int match(int token_tag);
int program (void);
int declarations(void);
int statements (void);
int declaration(void);
int statement(void);

int main(int argc, char *argv[]);

#endif //_SYNT_H_