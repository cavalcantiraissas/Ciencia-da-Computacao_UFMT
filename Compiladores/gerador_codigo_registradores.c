/**
 * UFMT -- Ciencia da Computacao
 * Compiladores - Prof. Ivairton
 *
 * Analisador sintatico descendente recursivo com geracao de codigo para
 * uma maquina COM REGISTRADORES (em vez de pilha). Numeros podem ter de
 * 1 a 3 digitos e o codigo gerado eh gravado no arquivo definido em
 * NOME_ARQ_SAIDA.
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
 *
 * Instrucoes geradas para a maquina de registradores:
 *
 *      LOAD R<i>, <valor>
 *      ADD  R<i>, R<i>, R<j>
 *      SUB  R<i>, R<i>, R<j>
 *      MUL  R<i>, R<i>, R<j>
 *      DIV  R<i>, R<i>, R<j>
 *
 * Alocacao de registradores: a pilha de controle (regStack) guarda quais
 * registradores contem os operandos pendentes durante a analise. Quando
 * dois operandos sao combinados, o resultado fica no registrador do
 * operando esquerdo e o registrador do operando direito eh liberado
 * (reciclado) para uso futuro.
**/
#include <stdio.h>
#include <stdlib.h>

//Constantes
#define MAX_CHAR      64
#define MAX_REGS      32
#define NOME_ARQ_SAIDA "codigo.asm"
#define true  1
#define false 0

//Variaveis globais - analise sintatica
int  pos = 0;
char string[MAX_CHAR];
char lookahead;
int  hasError = false;
int  lastDigitValue = 0;   //ultimo digito lido por digit()

//Variaveis globais - geracao de codigo com registradores
FILE *outFile;
int  regStack[MAX_REGS];   //registradores que guardam operandos pendentes
int  regTop = -1;
int  freeList[MAX_REGS];   //registradores livres (reciclados)
int  freeTop = -1;
int  nextReg = 0;          //proximo registrador nunca usado ainda

//Prototipos - analise sintatica
int  list();
int  listR();
int  term();
int  termR();
int  factor();
int  number();
int  digit();
int  match(char c);
void nextToken();
void reportError(const char *expected, char received, int position);

//Prototipos - geracao de codigo / registradores
int  getRegister();
void releaseRegister(int r);
void pushVal(int r);
int  popVal();
void gerarCombinacao(const char *mnemonico);


//  FUNCOES DE APOIO A ANALISE SINTATICA

int match(char c) {
    if ( lookahead == c ) {
        nextToken();
        return true;
    }
    return false;
}

void nextToken() {
    pos++;
    lookahead = string[pos];
}

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

    if ( outFile )
        fprintf(outFile, "; ERRO DE SINTAXE na posicao %d (recebido '%c', esperado %s)\n",
                position, (received == '\0') ? '$' : received, expected);
}


 //  FUNCOES DE APOIO A GERACAO DE CODIGO COM REGISTRADORES

//Retorna um registrador livre (reaproveitando um ja liberado, se houver)
int getRegister() {
    if ( freeTop >= 0 ) {
        return freeList[freeTop--];
    }
    return nextReg++;
}

//Devolve um registrador para a lista de livres, para reaproveitamento
void releaseRegister(int r) {
    freeList[++freeTop] = r;
}

//Empilha (na pilha de CONTROLE da analise, nao na maquina alvo) o
//registrador que guarda o valor recem calculado
void pushVal(int r) {
    regStack[++regTop] = r;
}

//Desempilha o registrador do valor mais recente
int popVal() {
    return regStack[regTop--];
}

//Gera o codigo de combinacao de dois operandos ja calculados (que estao
//no topo da pilha de controle) usando o mnemonico informado, deixando o
//resultado no registrador do operando esquerdo e liberando o registrador
//do operando direito.
void gerarCombinacao(const char *mnemonico) {
    int rDireita = popVal();
    int rEsquerda = popVal();

    fprintf(outFile, "%s R%d, R%d, R%d\n", mnemonico, rEsquerda, rEsquerda, rDireita);

    releaseRegister(rDireita);
    pushVal(rEsquerda);
}


//  REGRAS DE DERIVACAO DA GRAMATICA

//digit -> '0' | '1' | ... | '9'
//Apenas reconhece UM digito e guarda seu valor numerico em lastDigitValue.
//Nao gera codigo aqui: quem gera codigo eh number(), pois um numero pode
//ter ate 3 digitos e deve virar UMA UNICA instrucao de carga.
int digit() {
    if ( lookahead >= '0' && lookahead <= '9' ) {
        lastDigitValue = lookahead - '0';
        nextToken();
        return true;
    }
    return false;
}

//number -> digit digit? digit?   (1 a 3 digitos)
int number() {
    int count = 0;
    int valor = 0;

    while ( count < 3 && digit() ) {
        valor = valor * 10 + lastDigitValue;
        count++;
    }

    if ( count == 0 ) {
        reportError("um digito (0-9)", lookahead, pos);
        return false;
    }

    //Geracao de codigo: carrega o numero completo em um novo registrador
    int r = getRegister();
    fprintf(outFile, "LOAD R%d, %d\n", r, valor);
    pushVal(r);

    return true;
}

//factor -> number
int factor() {
    return number();
}

//termR -> ('*' | '/') factor termR | EPSILON
//Nivel de MAIOR precedencia (multiplicacao/divisao).
int termR() {
    if ( hasError ) return false;

    if ( lookahead == '*' ) {
        match('*');
        if ( !factor() ) return false;
        gerarCombinacao("MUL");
        return termR();
    } else if ( lookahead == '/' ) {
        match('/');
        if ( !factor() ) return false;
        gerarCombinacao("DIV");
        return termR();
    } else {
        return true; //EPSILON
    }
}

//term -> factor termR
int term() {
    if ( !factor() ) return false;
    return termR();
}

//listR -> ('+' | '-') term listR | EPSILON
//Nivel de MENOR precedencia (soma/subtracao).
int listR() {
    if ( hasError ) return false;

    if ( lookahead == '+' ) {
        match('+');
        if ( !term() ) return false;
        gerarCombinacao("ADD");
        return listR();
    } else if ( lookahead == '-' ) {
        match('-');
        if ( !term() ) return false;
        gerarCombinacao("SUB");
        return listR();
    } else if ( lookahead == '\0' ) {
        return true;
    } else {
        reportError("'+', '-', '*', '/' ou fim da cadeia", lookahead, pos);
        return false;
    }
}

//list -> term listR   (simbolo inicial da gramatica)
int list() {
    if ( !term() ) return false;
    return listR();
}


//  FUNCAO PRINCIPAL

int main() {

    int acept = false;

    outFile = fopen(NOME_ARQ_SAIDA, "w");
    if ( outFile == NULL ) {
        printf("Erro: nao foi possivel criar o arquivo de saida '%s'\n", NOME_ARQ_SAIDA);
        return 1;
    }

    fprintf(outFile, "; Codigo gerado para maquina com registradores\n");
    fprintf(outFile, "; -------------------------------------------\n");

    printf("Informe uma cadeia: ");
    scanf("%s", string); //leitura da cadeia

    lookahead = string[0]; //inicializacao do lookahead

    acept = list(); //Chamada da derivacao/funcao inicial da gramatica

    if ( acept && !hasError ) {
        int rFinal = popVal();
        fprintf(outFile, "; -------------------------------------------\n");
        fprintf(outFile, "; Resultado final disponivel em R%d\n", rFinal);
        printf("Cadeia PERTENCENTE a linguagem\n");
        printf("Codigo gerado com sucesso em '%s' (resultado em R%d)\n", NOME_ARQ_SAIDA, rFinal);
    } else {
        fprintf(outFile, "; -------------------------------------------\n");
        fprintf(outFile, "; Geracao de codigo INTERROMPIDA por erro de sintaxe.\n");
        printf("Cadeia NAO pertencente a linguagem\n");
        printf("Consulte '%s' para o codigo parcial gerado ate o ponto do erro.\n", NOME_ARQ_SAIDA);
    }

    fclose(outFile);

    return 0;
}
