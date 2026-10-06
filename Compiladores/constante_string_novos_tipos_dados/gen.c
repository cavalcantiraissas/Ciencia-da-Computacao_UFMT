/**
 * @file gen.c
 * @author Ivairton M. Santos - UFMT - Computacao
 * @brief Codificacao do modulo gerador de codigo
 * @version 0.2
 * @date 2022-02-23
 * 
 */

// Inclusao do cabecalho
#include "gen.h"

// Inclusao explicita de variaveis globais de outro contexto (symbols.h)
extern type_symbol_table_variables global_symbol_table_variables;
extern type_symbol_table_string symbol_table_string;
char output_file_name[FILENAME_MAX];
FILE *output_file;

/**
 * @brief Funcao que gera codigo de montagem para SOMA
 * 
 */
void genAdd() {
    printf("pop rax\n");
    printf("pop rbx\n");
    printf("add rax,rbx\n");
    printf("push rax\n");
}

/**
 * @brief Funcao que gera codigo de montagem para SUBTRACAO
 * 
 */
void genSub() {
    printf("pop rbx\n");
    printf("pop rax\n");
    printf("sub rax,rbx\n");
    printf("push rax\n");
}

/**
 * @brief Funcao que gera codigo de montagem para MULTIPLICACAO
 * 
 */
void genMult() {
    printf("pop rax\n");
    printf("pop rbx\n");
    printf("imult rax,rbx\n");
    printf("push rax\n");
}

/**
 * @brief Funcao que gera codigo de montagem para DIVISAO
 * 
 */
void genDiv() {
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
 * @brief Funcao que gera um preambulo que permite o uso das funcoes do C (scanf e printf)
 * 
 */
void gen_preambule(void) {
    fprintf(output_file, ";UFMT-Compiladores\n");
    fprintf(output_file, ";Prof. Ivairton\n");
    fprintf(output_file, ";Procedimento para geracao do executavel apos compilacao (em Linux):\n");
    fprintf(output_file, ";(1) compilacao do Assembly com nasm: $ nasm -f elf64 <nome_do_arquivo>\n");
    fprintf(output_file, ";(2) likedicao: $ ld -m elf_x86_64 <nome_arquivo_objeto>\n\n");
    fprintf(output_file, "extern printf\n");
    fprintf(output_file, "extern scanf\n");
}

/**
 * @brief Funcao que gera codigo da secao de dados (declaracao de variaveis).
 */
void gen_data_section(void) {
    int i, n;
    
    fprintf(output_file, "\n\tsection .data\n");
    
    // processa cada simbolo da tabela e gera um ponteiro para cada variavel na memoria
    n = global_symbol_table_variables.n_variables;
    for (i = 0; i < n; i++) {
       fprintf(output_file, "%s: ", global_symbol_table_variables.variable[i].name); 
       
       switch(global_symbol_table_variables.variable[i].type) { //Valor inicial zero; atribuicoes ocorrem na secao de codigo
            case INT:
                fprintf(output_file, "dd 0\n");
                break;
            case STRING:
                fprintf(output_file, "times %d db 0\n", STRING_VAR_SIZE);
                break;
            case FLOAT:
                fprintf(output_file, "dd 0.0\n"); //precisao simples (4 bytes), como o float do C
                break;
            case CHAR:
                fprintf(output_file, "db 0\n");
                break;
            default:
                fprintf(output_file, "[ERRO] Tipo desconhecido.\n");       
                break;           
       }
    }

    // processa cada simbolo da tabela de strings
    n = symbol_table_string.n_strings;
    for (i = 0; i < n; i++) {
        fprintf(output_file, "%s: db %s, 0\n",
            symbol_table_string.string[i].name, 
            symbol_table_string.string[i].value);
    }
}

/**
 * @brief Funcao que gera a marcacao do inicio da secao de codigo
 * 
 */
void gen_preambule_code(void) {
    fprintf(output_file, "\n\tsection .text\n");
    fprintf(output_file, "\tglobal main,_start\n");
    fprintf(output_file, "main:\n");
    fprintf(output_file, "_start:\n");
}

/**
 * @brief Funcao que encerra o codigo inserindo comandos de fechamento
 * 
 */
void gen_epilog_code(void) {
    //fprintf(output_file, "\nret\n");
    fprintf(output_file, "\n;encerra programa\n");
    fprintf(output_file, "mov ebx,0\n");
    fprintf(output_file, "mov eax,1\n");
    fprintf(output_file, "int 0x80\n");
}

/**
 * @brief Funcao que gera codigo de ATRIBUICAO de uma constante a uma variavel.
 * O valor eh lido da tabela de simbolos (deve ter sido guardado antes).
 * 
 * @param var variavel que recebe o valor
 * @param str_const entrada da tabela de strings com a constante (apenas para STRING)
 */
void gen_assign(type_symbol_table_entry *var, type_symbol_table_string_entry *str_const) {
    unsigned int float_bits;

    switch (var->type) {
        case INT:
            fprintf(output_file, "\n;%s = %d\n", var->name, *(int*) var->value);
            fprintf(output_file, "mov dword [%s], %d\n", var->name, *(int*) var->value);
            break;
        case FLOAT:
            //move o padrao de bits IEEE 754 do valor (4 bytes)
            memcpy(&float_bits, var->value, sizeof(float_bits));
            fprintf(output_file, "\n;%s = %g\n", var->name, *(float*) var->value);
            fprintf(output_file, "mov dword [%s], 0x%08X\n", var->name, float_bits);
            break;
        case CHAR:
            fprintf(output_file, "\n;%s = '%c'\n", var->name, *(char*) var->value);
            fprintf(output_file, "mov byte [%s], %d\n", var->name, *(char*) var->value);
            break;
        case STRING:
            //copia a constante (com o \0) da tabela de strings para a variavel
            fprintf(output_file, "\n;%s = %s\n", var->name, str_const->value);
            fprintf(output_file, "mov rsi, %s\n", str_const->name);
            fprintf(output_file, "mov rdi, %s\n", var->name);
            fprintf(output_file, "mov rcx, %zu\n", strlen((char*) var->value) + 1);
            fprintf(output_file, "cld\n");
            fprintf(output_file, "rep movsb\n");
            break;
        default:
            fprintf(output_file, "[ERRO] Tipo desconhecido.\n");
            break;
    }
}
