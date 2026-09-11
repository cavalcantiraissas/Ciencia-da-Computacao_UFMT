/**
 * UFMT -- Ciencia da Computacao
 * Compiladores - Prof. Ivairton
 *
 * Analisador sintatico descendente recursivo para expressoes aritmeticas
 * com soma, subtracao, multiplicacao e divisao, respeitando a precedencia
 * dos operadores. Numeros podem ter de 1 a 3 digitos.
 *
 * Gramatica:
 *
 *      list   -> term listR
 *      listR  -> ('+' | '-') term listR | EPSILON
 *      term   -> factor termR
 *      termR  -> ('*' | '/') factor termR | EPSILON
 *      factor -> number
 *      number -> digit digit? digit?      (1 a 3 digitos)
 *      digit  -> '0' | '1' | ... | '9'
**/
#include <stdio.h>
#include <stdlib.h>

//Constantes
#define MAX_CHAR 64
#define true 1
#define false 0

//Variaveis globais
int pos = 0;
char string[MAX_CHAR];
char lookahead;
int hasError = false;      //indica se ja foi reportado um erro de sintaxe

//Prototipos
int list();
int listR();
int term();
int termR();
int factor();
int number();
int digit();
int match(char c);
void nextToken();
void reportError(const char *expected, char received, int position);


//Verifica se o caractere atual (lookahead) eh o esperado; se for,
//consome o token avancando para o proximo caractere da cadeia.
int match(char c) {
    if ( lookahead == c ) {
        nextToken();
        return true;
    }
    return false;
}


//Funcao que passa ao proximo simbolo da cadeia
void nextToken() {
    pos++;
    lookahead = string[pos];
}


//Reporta um erro de sintaxe com posicao, o que foi recebido e o que
//era esperado. Apenas o primeiro erro encontrado eh exibido (para nao
//gerar uma cascata de mensagens decorrentes do primeiro problema).
void reportError(const char *expected, char received, int position) {
    if ( hasError ) return;
    hasError = true;

    printf("\n==> ERRO DE SINTAXE\n");
    printf("    Posicao   : %d\n", position);

    if ( received == '\0' )
        printf("    Recebido  : <fim da cadeia>\n");
    else
        printf("    Recebido  : '%c'\n", received);

    printf("    Esperado  : %s\n", expected);
}


//Funcao da regra de derivacao 'digit'
//Reconhece um unico digito de '0' a '9'.
int digit() {
    if (
        match('0') || match('1') ||
        match('2') || match('3') ||
        match('4') || match('5') ||
        match('6') || match('7') ||
        match('8') || match('9') )
    {
        return true;
    }
    return false;
}


//Funcao da regra de derivacao 'number'
//Reconhece numeros inteiros de 1 a 3 digitos.
int number() {
    int count = 0;

    while ( count < 3 && digit() ) {
        count++;
    }

    if ( count == 0 ) {
        reportError("um digito (0-9)", lookahead, pos);
        return false;
    }

    return true;
}


//Funcao da regra de derivacao 'factor'
//Delega para number; fica isolada para facilitar futuras extensoes
//da gramatica (ex.: parenteses).
int factor() {
    if ( number() )
        return true;
    return false;
}


//Funcao da regra de derivacao 'termR'
//Trata '*' e '/', que tem PRECEDENCIA sobre '+' e '-' por estarem em
//um nivel mais interno da gramatica.
int termR() {
    if ( hasError ) return false;

    if ( lookahead == '*' ) {
        match('*');
        if ( !factor() ) return false;
        return termR();
    } else if ( lookahead == '/' ) {
        match('/');
        if ( !factor() ) return false;
        return termR();
    } else {
        //EPSILON: nada a consumir aqui, quem decide se isso eh valido
        //eh quem chamou term()/termR() (list() ou listR())
        return true;
    }
}


//Funcao da regra de derivacao 'term'
int term() {
    if ( !factor() ) return false;
    return termR();
}


//Funcao da regra de derivacao 'listR'
int listR() {
    if ( hasError ) return false;

    if ( lookahead == '+' ) {
        match('+');
        if ( !term() ) return false;
        return listR();
    } else if ( lookahead == '-' ) {
        match('-');
        if ( !term() ) return false;
        return listR();
    } else if ( lookahead == '\0' ) {
        return true;
    } else {
        reportError("'+', '-' ou fim da cadeia", lookahead, pos);
        return false;
    }
}


//Funcao da regra de derivacao 'list' (simbolo inicial da gramatica)
int list() {
    if ( !term() ) return false;
    return listR();
}


//Funcao principal
int main() {

    int acept = false;

    printf("Informe uma cadeia: ");
    scanf("%s", string); //leitura da cadeia

    lookahead = string[0]; //inicializacao do lookahead

    acept = list(); //Chamada da derivacao/funcao inicial da gramatica

    if ( acept && !hasError ) {
        printf("Cadeia PERTENCENTE a linguagem\n");
    } else {
        printf("Cadeia NAO pertencente a linguagem\n");
    }

    return 0;
}
