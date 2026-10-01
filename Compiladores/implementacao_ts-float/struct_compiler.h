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
#define MAX_CHAR 32
#define MAX_TOKEN 32

#define true 1  //por conveniencia, para facilitar leitura de codigo
#define false 0 //por conveniencia, para facilitar leitura de codigo

// Definicao dos codigos dos tokens
#define ENDTOKEN '\0'
#define ERROR -1
#define NUM 1        //numeral inteiro, ex: 42
#define FLOAT_NUM 267 //numeral de ponto flutuante (com precisao), ex: 3.14
#define PLUS '+'
#define MINUS '-'
#define MULT '*'
#define DIV '/'
#define OPEN_PAR '('
#define CLOSE_PAR ')'
#define SEMICOLON ';'
//KeyWords
#define ID 256
#define IF 257
#define ELSE 258
#define THEN 259
#define WHILE 260
#define DO 261
#define READ 262
#define WRITE 263
#define INT 264
#define FLOAT 265
#define STRING 266
//#define ARROW
//#define CHAR
//#define BEGIN
//#define END
//#define EQUAL 
//#define GE
//#define LE


// Definicao da estrutura de dados 'token'
struct st_token {
    int tag;
    char lexema[MAX_CHAR];
    int value;
};
typedef struct st_token type_token;

#endif //_STRUCT_COMPILER_