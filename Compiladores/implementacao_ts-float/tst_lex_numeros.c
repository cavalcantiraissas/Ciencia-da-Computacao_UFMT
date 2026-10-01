#include "lex.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
    type_token *tok;

    //Arquivo de entrada opcional; por padrao usa entrada_numeros.txt
    initLex(argc > 1 ? argv[1] : "entrada_numeros.txt");

    do {
        tok = getToken();
        if (tok->tag == NUM)
            printf("NUM       -> lexema='%s'\n", tok->lexema);
        else if (tok->tag == FLOAT_NUM)
            printf("FLOAT_NUM -> lexema='%s'\n", tok->lexema);
        else if (tok->tag == ENDTOKEN)
            printf("ENDTOKEN\n");
        else
            printf("outro tag=%d lexema='%s'\n", tok->tag, tok->lexema);
        if (tok->tag == ENDTOKEN) {
            free(tok);
            break;
        }
        free(tok);
    } while (true);

    return 0;
}
