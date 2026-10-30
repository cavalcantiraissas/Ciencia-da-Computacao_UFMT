/**
 * @file struct_compiler.h
 * @author Prof. Ivairton M. Santos - UFMT - Computacao
 * @brief Modulo para definicao de constantes e estruturas de dados do Sistema
 *        Compilador de C Simplificado
 * @version 0.3
 * @date 2021-12-09
 * 
 */

#ifndef _STRUCT_COMPILER_
#define _STRUCT_COMPILER_

// Definicao de constantes para todo o compilador
#define MAX_CHAR 256 //tamanho maximo de um lexema (inclui constantes string)
#define MAX_TOKEN 256

#define true 1  //por conveniencia, para facilitar leitura de codigo
#define false 0 //por conveniencia, para facilitar leitura de codigo

// Definicao dos codigos dos tokens
#define ENDTOKEN '\0'
#define ERROR -1
#define NUM 1
#define PLUS '+'
#define MINUS '-'
#define MULT '*'
#define DIV '/'
#define OPEN_PAR '('
#define CLOSE_PAR ')'
#define SEMICOLON ';'
#define DOUBLE_QUOTES '"'
#define SINGLE_QUOTE '\''
#define COMMA ','

//KeyWords
#define ID 256
#define IF 257
#define ELSE 258
#define WHILE 260
#define DO 261
#define READ 262
#define WRITE 263
#define INT 264
#define STRING 265
#define FLOAT 266
#define CHAR 267
#define BEGIN 268 // begin ou {
#define END 269   // end ou }

//Operadores
#define ASSIGN '='
#define EQUAL 270   // ==
#define NE 271      // !=
#define GE 272      // >=
#define LE 273      // <=
#define GT '>'
#define LT '<'
#define AND 274     // &&
#define OR 275      // ||

//Constantes (separadas das palavras reservadas de tipo)
#define STRING_CONST 276 // "texto"
#define CHAR_CONST 277   // 'c'

//Tipos das expressoes reconhecidas pelo sintatico (usados tambem pelo gerador)
#define EXPR_INT 1   //valor inteiro (int ou char)
#define EXPR_BOOL 2  //valor booleano (0 ou 1)
#define EXPR_FLOAT 3 //valor float (padrao IEEE 754 de precisao dupla)


// Definicao da estrutura de dados 'token'
struct st_token {
    int tag;
    char lexema[MAX_CHAR];
    int value;
};
typedef struct st_token type_token;

#endif //_STRUCT_COMPILER_