/**
 * @file main.c
 * @author Prof. Ivairton M. Santos - UFMT - Computacao
 * @brief Funcao principal do compilador. Carrega o codigo fonte da expressao
 *        aritmetica a partir de um arquivo de entrada (passado por linha de
 *        comando) e invoca os analisadores lexico e sintatico.
 * @version 0.1
 * @date 2021-11-24
 *
 */

#include "synt.h"

int main(int argc, char *argv[]) {

    int acept;
    acept = false;

    if (argc != 2) {
        fprintf(stderr, "Uso: %s <arquivo_de_entrada>\n", argv[0]);
        return 1;
    }

    // Carrega codigo a partir do arquivo informado
    if ( !initLex(argv[1]) ) {
        return 1;
    }

    lookahead = getToken(); //Inicializacao do lookahead

    acept = list(); //Chamada da derivacao/funcao inicial da gramatica

    //Verifica aceitacao da cadeia/codigo
    if (acept) {
        printf("\tCadeia PERTENCENTE a linguagem\n");
    } else {
        printf("\tCadeia NAO pertencente a linguagem\n");
    }

    return 0;
}
