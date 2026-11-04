/**
 * @file tst_lex.c
 * @brief Modulo exclusivo para teste do analisador lexico (isolado)
 *
 * Le um arquivo-fonte e imprime cada token reconhecido (tag, nome da tag
 * e lexema), sem passar pelo sintatico. Para no fim do arquivo ou no
 * primeiro erro lexico.
 *
 * Uso:
 *   make tstLex
 *   ./tstLexCompiler <arquivo_de_entrada>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "struct_compiler.h"
#include "lex.h"

// Nome legivel de cada tag. Os tokens de um caractere (+, ;, = ...) usam
// o proprio caractere como tag; os demais sao as constantes de
// struct_compiler.h, que variam de uma etapa do compilador para outra.
static const char *tagName(int tag) {
    switch (tag) {
        case ERROR:        return "ERRO";
        case NUM:          return "NUM";
        case ID:           return "ID";
        case IF:           return "IF";
        case ELSE:         return "ELSE";
        case WHILE:        return "WHILE";
        case DO:           return "DO";
        case READ:         return "READ";
        case WRITE:        return "WRITE";
        case INT:          return "INT";
        case STRING:       return "STRING";
        case FLOAT:        return "FLOAT";
        case CHAR:         return "CHAR";
        case STRING_CONST: return "STRING_CONST";
        case CHAR_CONST:   return "CHAR_CONST";
#ifdef FLOAT_NUM
        case FLOAT_NUM:    return "FLOAT_NUM";
#endif
#ifdef THEN
        case THEN:         return "THEN";
#endif
#ifdef BEGIN
        case BEGIN:        return "BEGIN";
        case END:          return "END";
        case EQUAL:        return "EQUAL";
        case NE:           return "NE";
        case GE:           return "GE";
        case LE:           return "LE";
        case AND:          return "AND";
        case OR:           return "OR";
#endif
        default:           return "simbolo";
    }
}

int main(int argc, char *argv[]) {
    type_token *token;
    int total = 0;

    if (argc != 2) {
        fprintf(stderr, "Uso: %s <arquivo_de_entrada>\n", argv[0]);
        return 1;
    }

    initLex(argv[1]);

    printf("%-6s %-13s %s\n", "TAG", "NOME", "LEXEMA");
    while ((token = getToken()) != NULL && token->tag != ENDTOKEN) {
        printf("%-6d %-13s %s\n", token->tag, tagName(token->tag), token->lexema);
        total++;
        if (token->tag == ERROR) {
            free(token);
            fprintf(stderr, "Erro lexico: analise interrompida.\n");
            return 1;
        }
        free(token);
    }
    free(token);

    printf("\n%d token(s) reconhecido(s).\n", total);
    return 0;
}
