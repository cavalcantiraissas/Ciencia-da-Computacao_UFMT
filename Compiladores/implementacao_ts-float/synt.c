/**
 * @file synt.c
 * @author Prof. Ivairton M. Santos - UFMT - Computacao
 * @brief Codificacao do modulo do analisador sintatico
 * @version 0.4
 * @date 2022-02-04
 * 
 */

// Inclusao do cabecalho
#include "synt.h"

// Variaveis globais
type_token *lookahead;
extern type_symbol_table_variables global_symbol_table_variables;
extern type_symbol_table_string symbol_table_string;
extern char output_file_name[MAX_CHAR];
extern FILE *output_file;

/**
 * @brief Verifica se o proximo caracter (a frente) na cadeia eh o esperado
 * 
 * @param token_tag (int) codigo do token a ser verificado 
 * @return int true/false
 */
int match(int token_tag) {
    if ( lookahead->tag == token_tag ) {
        lookahead = getToken(); //Pega o proximo token por meio do lexico
        return true;
    }
    printf("[ERRO] Entrada processada: '%s' - tag esperada: '%d'\n", lookahead->lexema, token_tag);
    return false;
}

/**
 * @brief Regra de derivacao inicial
 */
int program (void) {
    return declarations();   
    //statements();  //Por enquanto nao processa comandos     
}

/**
 * @brief Regra de derivacao para declaracoes
 */
int declarations(void) {
    //while ( declaration() ); //Laco para processamento continuo das declaracoes
    int sucess;
    do {
        sucess = declaration();
    } while (sucess == true);
    if (sucess == ERROR)
        return false;
    else
        return true;
}

/**
 * @brief Regra de derivacao declaracao
 * @return int true/false
 */
int declaration (void) {
    type_symbol_table_entry *search_symbol;
    int ok1, ok2;
    type_symbol_table_entry *flag;
    char var_name[MAX_CHAR];
    int decl_type;

    //Verifica se a tag eh de um tipo conhecido (INT ou FLOAT)
    if ( lookahead->tag == INT || lookahead->tag == FLOAT ) {
        decl_type = lookahead->tag;  //guarda qual tipo foi declarado
        match(decl_type);
        strcpy(var_name, lookahead->lexema);
        search_symbol = sym_find( var_name, &global_symbol_table_variables );

        if ( search_symbol != NULL) {
            printf ("[ERRO] Variavel '%s' ja declarada.\n", var_name); 
            return ERROR;
        } else {
            ok1= match(ID); //Verifica se identificador vem a seguir
            ok2 = match (SEMICOLON); //Verifica se ; vem a seguir
            if (ok1 && ok2) {
                //value = NULL: a gramatica atual nao suporta inicializacao
                //(ex.: "int x = 5;"); o campo generico fica pronto para isso.
                flag = sym_declare( var_name, decl_type, 0, NULL, &global_symbol_table_variables);
                if (flag != NULL)
                    return true;
                else
                    return false;
            } else {
                //Falha ao casar identificador ou ';': erro de sintaxe real,
                //e NAO apenas "fim das declaracoes" (bug corrigido: antes
                //retornava 'false', que era tratado igual a ENDTOKEN e
                //deixava a compilacao seguir como se tivesse tido sucesso).
                return ERROR;
            }
        }
    } else if (lookahead ->tag == ENDTOKEN ) {
        //Verifica se fim de arquivo
        return false;         
    } else {
        printf ("[ERRO] Tipo desconhecido: %d %s.\n", lookahead->tag, lookahead->lexema);
        return ERROR; 
    }
}

/**
 * @brief Regra de derivacao para comandos
 */
int statements (void) {
   //while ( statement() );  //processa enquanto houver comandos
   int sucess;
   do {
        sucess = statement();
   } while (sucess == true);
   if (sucess == ERROR)
        return false;
    else
        return true;
}

/**
 * @brief Regra de derivacao que processa um comando
 * 
 * @return int true/false
 */
int statement (void) {
    return false;
}




//--------------------- MAIN -----------------------

/**
 * @brief Funcao principal (main) do compilador
 * 
 * @return int 
 */
int main(int argc, char *argv[]) {
    int sucess;

    //Inicializa a tabela de simbolo global
    initSymbolTableVariables(&global_symbol_table_variables);
    initSymbolTableString();

    //Verifica a passagem de parametro
    if (argc != 2) {
        printf("[ERRO]\n\tÉ necessário informar um arquivo de entrada (código) como parâmetro.\n\n");
        exit(EXIT_FAILURE);
    }
    
    initLex(argv[1]); //Carrega codigo
    lookahead = getToken(); //Inicializacao do lookahead

    sucess = program(); //Chamada da derivacao/funcao inicial da gramatica

    if (sucess != true) {
        printf("\t[ERRO] FALHA NA COMPILACAO DO PROGRAMA\n\n");
        return EXIT_FAILURE; //antes retornava sempre 1, mesmo em caso de sucesso
    } else {
        printSTVariables(&global_symbol_table_variables);

        strcpy(output_file_name, argv[1]);
        strcat(output_file_name, ".asm");
        output_file = fopen(output_file_name, "w+");
        gen_data_section(); //Gera codigo da secao de dados
        fclose(output_file);
        return EXIT_SUCCESS;
    }
}