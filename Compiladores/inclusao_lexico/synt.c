/**
 * @file synt.c
 * @author Prof. Ivairton M. Santos - UFMT - Computacao
 * @brief Codificacao do modulo do analisador sintatico
 * @version 0.2
 * @date 2021-11-24
 *
 * Gramatica reconhecida (com precedencia de '*' e '/' sobre '+' e '-'):
 *
 *   LIST   -> TERM LISTR
 *   LISTR  -> + TERM LISTR | - TERM LISTR | (vazio)
 *   TERM   -> DIGIT TERMR
 *   TERMR  -> * DIGIT TERMR | / DIGIT TERMR | (vazio)
 *   DIGIT  -> NUM
 *
 */

// Inclusao do cabecalho
#include "synt.h"

/**
 * @brief Traduz o codigo (tag) de um token para uma descricao textual, usada
 *        nas mensagens de erro sintatico.
 *
 * @param tag codigo do token
 * @return const char* descricao do token
 */
const char *tokenName(int tag) {
    switch (tag) {
        case NUM:      return "numero";
        case PLUS:     return "'+'";
        case MINUS:    return "'-'";
        case TIMES:    return "'*'";
        case DIVIDE:   return "'/'";
        case ENDTOKEN: return "fim da expressao";
        case ERROR:    return "token invalido";
        default:       return "token desconhecido";
    }
}

/**
 * @brief Verifica se o proximo caracter (a frente) na cadeia eh o esperado.
 *        Caso nao seja, gera mensagem de erro sintatico.
 *
 * @param token_tag (int) codigo do token a ser verificado
 * @return int true/false
 */
int match(int token_tag) {
    if ( lookahead->tag == token_tag ) {
        lookahead = getToken(); //Pega o proximo token por meio do lexico
        return true;
    }
    fprintf(stderr, "Erro sintatico na posicao %d: esperado %s, encontrado %s\n",
            lookahead->pos, tokenName(token_tag),
            (lookahead->tag == NUM || lookahead->tag == ERROR) ? lookahead->lexema : tokenName(lookahead->tag));
    return false;
}

/**
 * @brief Regra de derivacao da gramatica: LIST
 *
 * @return int true/false
 */
int list() {
    if ( term() && listR() )
        return true;
    return false;
}

/**
 * @brief Regra de derivacao da gramatica: LIST_RECURSIVO
 *
 * @return int true/false
 */
int listR() {
    int test1, test2;
    //Verifica ocorrencia de terminal '+'
    if ( lookahead->tag == PLUS ) {
        test1 = match(PLUS);
        test2 = term();
        genAdd(); //Geracao de codigo por meio de funcao do GERADOR
        if (test1 && test2)
            return listR();
        return false;
    } //Verifica ocorrencia de terminal '-'
    else if ( lookahead->tag == MINUS ) {
        test1 = match(MINUS);
        test2 = term();
        genSub(); //Geracao de codigo por meio de funcao do GERADOR
        if (test1 && test2)
            return listR();
        return false;
    } //Verifica se fim de entrada
    else {
        if ( lookahead->tag == ENDTOKEN )
            return true;
        //Caso todos os testes falhem, sobrou algo inesperado na entrada
        fprintf(stderr, "Erro sintatico na posicao %d: token inesperado %s\n",
                lookahead->pos,
                (lookahead->tag == NUM || lookahead->tag == ERROR) ? lookahead->lexema : tokenName(lookahead->tag));
        return false;
    }
}

/**
 * @brief Regra de derivacao da gramatica: TERM
 *
 * @return int true/false
 */
int term() {
    if ( digit() && termR() )
        return true;
    return false;
}

/**
 * @brief Regra de derivacao da gramatica: TERM_RECURSIVO
 *
 * @return int true/false
 */
int termR() {
    int test1, test2;
    //Verifica ocorrencia de terminal '*'
    if ( lookahead->tag == TIMES ) {
        test1 = match(TIMES);
        test2 = digit();
        genMul(); //Geracao de codigo por meio de funcao do GERADOR
        if (test1 && test2)
            return termR();
        return false;
    } //Verifica ocorrencia de terminal '/'
    else if ( lookahead->tag == DIVIDE ) {
        test1 = match(DIVIDE);
        test2 = digit();
        genDiv(); //Geracao de codigo por meio de funcao do GERADOR
        if (test1 && test2)
            return termR();
        return false;
    } //Producao vazia: quem chamou (LISTR) trata o token seguinte
    else {
        return true;
    }
}

/**
 * @brief Regra de derivacao da gramatica: DIGIT
 *
 * @return int true/false
 */
int digit() {
    char aux_lexema[MAX_TOKEN];
    strcpy(aux_lexema, lookahead->lexema); //armazena temporariamente o lexema

    if ( match(NUM) ) {
        genNum(aux_lexema); //Geracao de codigo por meio de funcao do GERADOR
        return true;
    }
    return false;
}
