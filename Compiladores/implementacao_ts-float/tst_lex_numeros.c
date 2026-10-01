#include "lex.h"
#include <stdio.h>

int main() {
    type_token *tok;

    initLex("entrada_numeros.txt");

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
    } while (tok->tag != ENDTOKEN);

    return 0;
}
