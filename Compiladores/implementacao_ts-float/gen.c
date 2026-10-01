/**
 * @file gen.c
 * @author Ivairton M. Santos - UFMT - Computacao
 * @brief Codificacao do modulo gerador de codigo
 * @version 0.2
 * @date 2022-02-04
 * 
 */

// Inclusao do cabecalho
#include "gen.h"

// Inclusao explicita de variaveis globais de outro contexto (symbols.h)
extern type_symbol_table_variables global_symbol_table_variables;
extern type_symbol_table_string symbol_table_string;
char output_file_name[MAX_CHAR];
FILE *output_file;

/**
 * @brief Funcao que gera codigo de montagem para SOMA
 * 
 */
void genAdd(void) {
    printf("pop rax\n");
    printf("pop rbx\n");
    printf("add rax,rbx\n");
    printf("push rax\n");
}

/**
 * @brief Funcao que gera codigo de montagem para SUBTRACAO
 * 
 */
void genSub(void) {
    printf("pop rbx\n");
    printf("pop rax\n");
    printf("sub rax,rbx\n");
    printf("push rax\n");
}

/**
 * @brief Funcao que gera codigo de montagem para MULTIPLICACAO
 * 
 */
void genMult(void) {
    printf("pop rax\n");
    printf("pop rbx\n");
    printf("imult rax,rbx\n");
    printf("push rax\n");
}

/**
 * @brief Funcao que gera codigo de montagem para DIVISAO
 * 
 */
void genDiv(void) {
    printf("pop rbx\n");
    printf("pop rax\n");
    printf("idiv rax,rbx\n");
    printf("push rax\n");
}

/**
 * @brief Funcao que gera codigo de montagem para armazenamento de NUMERAL
 * 
 * @param num_string 
 */
void genNum(char num_string[MAX_TOKEN]) {
    printf("mov rax,%s\n", num_string);
    printf("push rax\n");
}

/**
 * @brief Funcao que gera codigo da secao de dados (declaracao de variaveis).
 */
void gen_data_section(void) {
    int i, n;
    
    fprintf(output_file, "section .data\n");

    // emite strings de formato fixo
    fprintf(output_file, "fmtstr0  db  \"%%d\",0\n");
    fprintf(output_file, "fmtstr1  db  \"%%s\",0\n");
    
    // processa cada simbolo da tabela e gera um ponteiro para cada variavel na memoria
    n = global_symbol_table_variables.n_variables;
    for (i = 0; i < n; i++) {
       fprintf(output_file, "%s  ", global_symbol_table_variables.variable[i].name); 
       
       switch(global_symbol_table_variables.variable[i].type) { //Por enquanto gera endereco zero
           case INT:
               fprintf(output_file, "dd  0\n");
               break;
           case FLOAT:
               fprintf(output_file, "dd  0.0\n"); //NASM aceita 'dd' tambem para float de precisao simples
               break;
           case STRING:
               fprintf(output_file, "db  \"                \" \n");
               break;
           default:
               fprintf(output_file, "[ERRO] Tipo desconhecido.\n");       
               break;           
       }
    }

    n = symbol_table_string.n_strings;
    for (i = 0; i < n; i++) {
        fprintf(output_file, "%s  db  %s\n", symbol_table_string.string[i].name, symbol_table_string.string[i].value);
    }
}