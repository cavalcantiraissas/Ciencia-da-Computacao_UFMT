/**
 * @file synt.h
 * @author Prof. Ivairton M. Santos - UFMT - Computacao
 * @brief Modulo do analisado sintatico
 * @version 0.5
 * @date 2022-03-17
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
 *      program      -> declarations statements
 *      declarations -> declaration declarations | [vazio]
 *      declaration  -> type id ;
 *      type         -> int | float | char | string
 *      statements   -> statement statements | [vazio]
 *      statement    -> read_stmt | write_stmt | if_stmt | while_stmt | assign_stmt
 *      read_stmt    -> read id ;
 *      write_stmt   -> write id ; | write string_const ;
 *      if_stmt      -> if '(' B ')' block [ else block ]
 *      while_stmt   -> while '(' B ')' block
 *      block        -> '{' statements '}'        ('begin' / 'end' tambem aceitos)
 *      assign_stmt  -> id '=' valor ;            (valor compativel com o tipo de id:
 *                                                 int, char -> E inteira; float -> E;
 *                                                 string -> string_const | id string)
 *
 *      B            -> BOr                       (o resultado deve ser booleano)
 *      BOr          -> BAnd BOrR
 *      BOrR         -> '||' BAnd BOrR | [vazio]
 *      BAnd         -> BRel BAndR
 *      BAndR        -> '&&' BRel BAndR | [vazio]
 *      BRel         -> E [ rel_operator E ]      ([id | exp_aritmetica] op [id | exp_aritmetica])
 *      rel_operator -> '<' | '>' | '<=' | '>=' | '==' | '!='
 *
 *      E            -> T ER
 *      ER           -> '+' T ER | '-' T ER | [vazio]
 *      T            -> F TR
 *      TR           -> '*' F TR | '/' F TR | [vazio]
 *      F            -> '(' BOr ')' | '-' F | num | char_const | id
 *
 * Precedencia (da menor para a maior): '||', '&&', relacionais, '+' '-', '*' '/'.
 * As funcoes de B a F devolvem o tipo da expressao reconhecida (EXPR_INT,
 * EXPR_FLOAT ou EXPR_BOOL, ver struct_compiler.h), permitindo verificar, por
 * exemplo, que '&&' so recebe comparacoes e que '<' so recebe expressoes
 * aritmeticas; false (0) indica erro. Variaveis char valem como inteiros
 * (codigo ASCII); se um operando for int e o outro float, o int eh convertido.
 * '&&' e '||' sao avaliados com curto-circuito.
 */

// Prototipos
void error(const char *fmt, ...);
int match(int);
int program (void);
int declarations(void);
int statements (void);
int declaration(void);
int declarationV(char*, int);
int declarationF(char*, int);
int paramList(type_symbol_table_entry[MAX_PARAMS], int*);
int isType(int);
void funcSignature(type_symbol_table_func_entry*, char*, int);
int statement(void);
int block(void);
int ifStmt(void);
int whileStmt(void);
int idStmt(void);
int assignStmt(char*);
int callStmt(char*);
int argument(type_symbol_table_func_entry*, int, int*, char*);
int stringValue(char*);
const char *typeName(int);
int isNumeric(int);
int promote(int, int);

int B(void);
int BOr(void);
int BOrR(int);
int BAnd(void);
int BAndR(int);
int BRel(void);
int isRelOperator(int);
int boolOperator(int*);

int E(void);
int ER(int);
int T(void);
int TR(int);
int F(void);

int main(int, char**);

#endif //_SYNT_H_