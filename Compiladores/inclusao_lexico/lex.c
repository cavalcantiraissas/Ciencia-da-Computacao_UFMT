/**
 * @file lex.c
 * @author Prof. Ivairton M. Santos - UFMT - Computacao
 * @brief Codificacao do modulo do analisador lexico
 * @version 0.1
 * @date 2021-11-24
 * 
 */

// Inclusao do cabecalho
#include "lex.h"

/**
 * @brief Processo de inicializacao do lexico. Carrega o codigo fonte da
 *        expressao aritmetica a partir de um arquivo de entrada.
 *
 * @param filename caminho do arquivo contendo o codigo fonte
 * @return int true se o arquivo foi carregado com sucesso, false caso contrario
 */
int initLex(char *filename) {
    FILE *fp;
    size_t len;

    fp = fopen(filename, "r");
    if (fp == NULL) {
        fprintf(stderr, "Erro: nao foi possivel abrir o arquivo de entrada '%s'\n", filename);
        return false;
    }

    len = fread(string, sizeof(char), MAX_SRC - 1, fp);
    string[len] = ENDTOKEN;
    fclose(fp);

    pos = 0;
    return true;
}

/**
 * @brief Analisa o proximo comando no código de entrada e retorna estrutura de
 * dados com Token correspondente (ponteiro).
 * 
 * @return type_token* 
 */
type_token *getToken() {
    char buffer[MAX_CHAR];
    int pos_buffer;
    int start;
    type_token *token;

    pos_buffer = 0;
    token = (type_token*) malloc(sizeof(type_token));

    // Consome espacos
    while ( isspace(string[pos]) ) {
        pos++;
    }

    // Marca a posicao inicial do token, usada nas mensagens de erro
    start = pos;
    token->pos = start;

    // Verifica se NUMERO
    if ( isdigit(string[pos]) ) {
        // constroi buffer com os digitos
        while ( isdigit(string[pos]) ) {
            buffer[pos_buffer++] = string[pos++];
        }
        buffer[pos_buffer] = '\0';
        token->tag = NUM;
        strcpy( token->lexema, buffer ); //copia buffer para lexema
    }
    //Verifica se PLUS (+)
    else if (string[pos] == PLUS) {
        token->tag = PLUS;
        strcpy(token->lexema, "+");
        pos++;
    }
    //Verifica se MINUS (-)
    else if (string[pos] == MINUS) {
        token->tag = MINUS;
        strcpy(token->lexema, "-");
        pos++;
    }
    //Verifica se TIMES (*)
    else if (string[pos] == TIMES) {
        token->tag = TIMES;
        strcpy(token->lexema, "*");
        pos++;
    }
    //Verifica se DIVIDE (/)
    else if (string[pos] == DIVIDE) {
        token->tag = DIVIDE;
        strcpy(token->lexema, "/");
        pos++;
    }
    //Verifica se FIM DE CADEIA
    else if (string[pos] == ENDTOKEN) {
        token->tag = ENDTOKEN;
        strcpy(token->lexema, "");
    }
    //ERRO: caractere nao reconhecido pelo lexico
    else {
        token->tag = ERROR;
        buffer[0] = string[pos];
        buffer[1] = '\0';
        strcpy(token->lexema, buffer);
        fprintf(stderr, "Erro lexico na posicao %d: caractere invalido '%c'\n", start, string[pos]);
        pos++; // avanca para evitar loop infinito sobre o caractere invalido
    }

    return token;
}