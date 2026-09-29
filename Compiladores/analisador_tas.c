/**
 * UFMT - Ciencia da Computacao
 * Compiladores - Prof. Ivairton
 *
 * Unidade II - Analise Sintatica - Tabela de Analise Sintatica (TAS)
 *
 * Analisador sintatico DIRIGIDO POR TABELA (nao recursivo-descendente)
 * para expressoes aritmeticas com soma e multiplicacao, baseado na
 * gramatica LL(1):
 *
 *      E  -> T E'
 *      E' -> + T E' | EPSILON
 *      T  -> F T'
 *      T' -> * F T' | EPSILON
 *      F  -> ( E ) | id
 *
 * onde 'id' representa um operando (aqui, um numero inteiro).
 *
 * O programa implementa:
 *   1) A REPRESENTACAO DA TAS (tabela M[nao-terminal, terminal]);
 *   2) O DRIVER (algoritmo com pilha, conforme slide da aula);
 *   3) A resposta se a cadeia de entrada eh uma expressao aritmetica
 *      valida ou nao, com mensagens de erro detalhadas.
 *
 * Como bonus, o programa tambem imprime a TAS montada e o TRACO da
 * execucao do driver (pilha / entrada / producao aplicada), no mesmo
 * formato usado no exemplo do slide.
**/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_CHAR    256
#define MAX_TOKENS  128
#define MAX_STACK   256
#define MAX_RHS     4      //maior lado direito de producao desta gramatica
#define true 1
#define false 0



// SIMBOLOS DA GRAMATICA

typedef enum {
    //Terminais (indices 0..5, usados como coluna da TAS)
    SYM_ID = 0,
    SYM_PLUS,
    SYM_STAR,
    SYM_LPAREN,
    SYM_RPAREN,
    SYM_DOLLAR,
    //Nao-terminais (indices 0..4, usados como linha da TAS)
    SYM_E,
    SYM_ELINE,
    SYM_T,
    SYM_TLINE,
    SYM_F,
    //Especiais
    SYM_EPSILON,
    SYM_INVALID
} Symbol;

#define NUM_TERMS     6   //id,+,*,(,),$
#define NUM_NONTERMS  5   //E,E',T,T',F


int isTerminal(Symbol s) {
    return s >= SYM_ID && s <= SYM_DOLLAR;
}

int isNonTerminal(Symbol s) {
    return s >= SYM_E && s <= SYM_F;
}

//Indice da coluna (terminal) na TAS
int terminalIndex(Symbol s) {
    if ( s >= SYM_ID && s <= SYM_DOLLAR ) return s - SYM_ID;
    return -1;
}

//Indice da linha (nao-terminal) na TAS
int nonTerminalIndex(Symbol s) {
    if ( s >= SYM_E && s <= SYM_F ) return s - SYM_E;
    return -1;
}

const char *symbolName(Symbol s) {
    switch (s) {
        case SYM_ID:      return "id";
        case SYM_PLUS:    return "+";
        case SYM_STAR:    return "*";
        case SYM_LPAREN:  return "(";
        case SYM_RPAREN:  return ")";
        case SYM_DOLLAR:  return "$";
        case SYM_E:       return "E";
        case SYM_ELINE:   return "E'";
        case SYM_T:       return "T";
        case SYM_TLINE:   return "T'";
        case SYM_F:       return "F";
        case SYM_EPSILON: return "eps";
        default:          return "?";
    }
}



 // 1) REPRESENTACAO DA TAS (TABELA DE ANALISE SINTATICA)

typedef struct {
    int    valida;                 //false = celula em branco (erro sintatico)
    int    tamanho;                //quantidade de simbolos do lado direito
    Symbol simbolos[MAX_RHS];      //lado direito da producao (Y1 Y2 ... Yk)
} Producao;

Producao TAS[NUM_NONTERMS][NUM_TERMS];

//Preenche uma celula da TAS: TAS[naoTerminal][terminalLookahead] = producao
void definirProducao(Symbol naoTerminal, Symbol terminalLookahead,
                      Symbol simbolos[], int tamanho) {
    int i = nonTerminalIndex(naoTerminal);
    int j = terminalIndex(terminalLookahead);
    TAS[i][j].valida  = true;
    TAS[i][j].tamanho = tamanho;
    for (int k = 0; k < tamanho; k++)
        TAS[i][j].simbolos[k] = simbolos[k];
}

//Monta a TAS de acordo com a gramatica:
//   E  -> T E'
//   E' -> + T E' | EPSILON
//   T  -> F T'
//   T' -> * F T' | EPSILON
//   F  -> ( E ) | id
void construirTAS() {

    //inicializa tudo como celula em branco (erro sintatico)
    for (int i = 0; i < NUM_NONTERMS; i++)
        for (int j = 0; j < NUM_TERMS; j++)
            TAS[i][j].valida = false;

    Symbol rhs[MAX_RHS];

    // E -> T E'      (Primeiro(E) = { id, ( } )
    rhs[0]=SYM_T; rhs[1]=SYM_ELINE;
    definirProducao(SYM_E, SYM_ID,     rhs, 2);
    definirProducao(SYM_E, SYM_LPAREN, rhs, 2);

    // E' -> + T E'   (para lookahead '+')
    rhs[0]=SYM_PLUS; rhs[1]=SYM_T; rhs[2]=SYM_ELINE;
    definirProducao(SYM_ELINE, SYM_PLUS, rhs, 3);

    // E' -> EPSILON  (Seguinte(E') = { ), $ } )
    definirProducao(SYM_ELINE, SYM_RPAREN, NULL, 0);
    definirProducao(SYM_ELINE, SYM_DOLLAR, NULL, 0);

    // T -> F T'      (Primeiro(T) = { id, ( } )
    rhs[0]=SYM_F; rhs[1]=SYM_TLINE;
    definirProducao(SYM_T, SYM_ID,     rhs, 2);
    definirProducao(SYM_T, SYM_LPAREN, rhs, 2);

    // T' -> * F T'   (para lookahead '*')
    rhs[0]=SYM_STAR; rhs[1]=SYM_F; rhs[2]=SYM_TLINE;
    definirProducao(SYM_TLINE, SYM_STAR, rhs, 3);

    // T' -> EPSILON  (Seguinte(T') = { +, ), $ } )
    definirProducao(SYM_TLINE, SYM_PLUS,   NULL, 0);
    definirProducao(SYM_TLINE, SYM_RPAREN, NULL, 0);
    definirProducao(SYM_TLINE, SYM_DOLLAR, NULL, 0);

    // F -> id
    rhs[0]=SYM_ID;
    definirProducao(SYM_F, SYM_ID, rhs, 1);

    // F -> ( E )
    rhs[0]=SYM_LPAREN; rhs[1]=SYM_E; rhs[2]=SYM_RPAREN;
    definirProducao(SYM_F, SYM_LPAREN, rhs, 3);
}

//Retorna a producao TAS[X][a], ou NULL se a celula estiver em branco
Producao *buscarProducao(Symbol naoTerminal, Symbol terminal) {
    int i = nonTerminalIndex(naoTerminal);
    int j = terminalIndex(terminal);
    if ( i < 0 || j < 0 ) return NULL;
    if ( !TAS[i][j].valida ) return NULL;
    return &TAS[i][j];
}

//Imprime a TAS no mesmo formato do slide (para conferencia visual)
void imprimirTAS() {
    Symbol naoTerminais[NUM_NONTERMS] = {SYM_E, SYM_ELINE, SYM_T, SYM_TLINE, SYM_F};
    Symbol terminais[NUM_TERMS]       = {SYM_ID, SYM_PLUS, SYM_STAR, SYM_LPAREN, SYM_RPAREN, SYM_DOLLAR};

    printf("\n=========================== TAS (Tabela de Analise Sintatica) ===========================\n");
    printf("%-6s", "");
    for (int j = 0; j < NUM_TERMS; j++)
        printf("%-14s", symbolName(terminais[j]));
    printf("\n");

    for (int i = 0; i < NUM_NONTERMS; i++) {
        printf("%-6s", symbolName(naoTerminais[i]));
        for (int j = 0; j < NUM_TERMS; j++) {
            Producao *p = &TAS[i][j];
            char buf[32];
            if ( !p->valida ) {
                strcpy(buf, "-");
            } else if ( p->tamanho == 0 ) {
                snprintf(buf, sizeof(buf), "%s->eps", symbolName(naoTerminais[i]));
            } else {
                char rhsStr[24] = "";
                for (int k = 0; k < p->tamanho; k++)
                    strcat(rhsStr, symbolName(p->simbolos[k]));
                snprintf(buf, sizeof(buf), "%s->%s", symbolName(naoTerminais[i]), rhsStr);
            }
            printf("%-14s", buf);
        }
        printf("\n");
    }
    printf("===========================================================================================\n");
}


// LEXICO: transforma a cadeia digitada em uma lista de tokens
typedef struct {
    Symbol tipo;
    int    valor;     //valor numerico, quando tipo == SYM_ID
    int    posicao;   //posicao (indice) na cadeia original, p/ mensagens de erro
} Token;

Token tokens[MAX_TOKENS];
int   numTokens = 0;
int   erroLexico = false;

void tokenizar(const char *entrada) {
    int i = 0;
    int len = strlen(entrada);
    numTokens = 0;

    while ( i < len ) {
        char c = entrada[i];

        if ( c == ' ' ) {
            i++;
            continue;
        }

        if ( c >= '0' && c <= '9' ) {
            int inicio = i;
            int valor = 0;
            while ( i < len && entrada[i] >= '0' && entrada[i] <= '9' ) {
                valor = valor * 10 + (entrada[i] - '0');
                i++;
            }
            tokens[numTokens].tipo     = SYM_ID;
            tokens[numTokens].valor    = valor;
            tokens[numTokens].posicao  = inicio;
            numTokens++;
            continue;
        }

        Symbol tipo;
        switch (c) {
            case '+': tipo = SYM_PLUS;   break;
            case '*': tipo = SYM_STAR;   break;
            case '(': tipo = SYM_LPAREN; break;
            case ')': tipo = SYM_RPAREN; break;
            default:
                printf("\n==> ERRO LEXICO\n");
                printf("    Posicao   : %d\n", i);
                printf("    Caractere : '%c' nao reconhecido pela linguagem\n", c);
                erroLexico = true;
                return;
        }

        tokens[numTokens].tipo    = tipo;
        tokens[numTokens].valor   = 0;
        tokens[numTokens].posicao = i;
        numTokens++;
        i++;
    }

    //Marcador de fim de entrada
    tokens[numTokens].tipo    = SYM_DOLLAR;
    tokens[numTokens].valor   = 0;
    tokens[numTokens].posicao = len;
    numTokens++;
}

// 2) DRIVER (algoritmo de analise preditiva dirigida por tabela)

Symbol pilha[MAX_STACK];
int    topo = -1;
int    hasError = false;

void empilha(Symbol s) {
    pilha[++topo] = s;
}

Symbol desempilha() {
    return pilha[topo--];
}

//Monta uma string com o conteudo atual da pilha (fundo -> topo)
void pilhaParaString(char *buf) {
    buf[0] = '\0';
    for (int i = 0; i <= topo; i++)
        strcat(buf, symbolName(pilha[i]));
}

//Monta uma string com a entrada restante a partir do indice a
void entradaRestanteParaString(char *buf, int a) {
    buf[0] = '\0';
    for (int i = a; i < numTokens; i++) {
        if ( tokens[i].tipo == SYM_ID ) {
            char num[16];
            snprintf(num, sizeof(num), "%d", tokens[i].valor);
            strcat(buf, num);
        } else {
            strcat(buf, symbolName(tokens[i].tipo));
        }
    }
}

//Constroi a lista de terminais esperados para um nao-terminal X (usado
//nas mensagens de erro quando a celula da TAS esta em branco)
void terminaisEsperados(Symbol X, char *buf) {
    Symbol terminais[NUM_TERMS] = {SYM_ID, SYM_PLUS, SYM_STAR, SYM_LPAREN, SYM_RPAREN, SYM_DOLLAR};
    buf[0] = '\0';
    int primeiro = true;
    for (int j = 0; j < NUM_TERMS; j++) {
        if ( buscarProducao(X, terminais[j]) != NULL ) {
            if ( !primeiro ) strcat(buf, ", ");
            strcat(buf, symbolName(terminais[j]));
            primeiro = false;
        }
    }
    if ( primeiro ) strcpy(buf, "(nenhum - erro na propria TAS)");
}

void reportarErro(const char *esperado, Symbol recebido, int posicao) {
    if ( hasError ) return;
    hasError = true;

    char recebidoStr[16];
    if ( recebido == SYM_ID ) strcpy(recebidoStr, "numero");
    else strcpy(recebidoStr, symbolName(recebido));

    printf("\n==> ERRO DE SINTAXE\n");
    printf("    Posicao   : %d\n", posicao);
    printf("    Recebido  : %s\n", recebidoStr);
    printf("    Esperado  : %s\n", esperado);
}

//DRIVER: algoritmo de analise preditiva com pilha (conforme slide da aula)
//
//  01. empilha($)
//  02. empilha(S)
//  03. a <- lookahead
//  04. repita
//  05.    X <- desempilha()
//  06.    se (X eh terminal) ou (X = $) entao
//  07.       match(x)
//  08.    senao se (M[X,a] = Y1Y2...Yk) entao
//  09.       empilha(Yk,...,Y1)
//  10.       produz saida
//  11.    senao
//  12.       erro()
//  13. ate (X = $)
int parse() {
    int a = 0;              //ponteiro do lookahead (indice em tokens[])
    Symbol X;
    char strPilha[128], strEntrada[128];

    empilha(SYM_DOLLAR);
    empilha(SYM_E);          //simbolo inicial da gramatica

    printf("\n%-20s | %-25s | %s\n", "Pilha", "Entrada", "Producao aplicada");
    printf("---------------------------------------------------------------------\n");

    do {
        X = desempilha();
        Symbol lookahead = tokens[a].tipo;

        pilhaParaString(strPilha);
        entradaRestanteParaString(strEntrada, a);

        if ( isTerminal(X) || X == SYM_DOLLAR ) {
            //match(x): verifica compatibilidade e avanca o token
            if ( X == lookahead ) {
                printf("%-20s | %-25s | %s\n", strPilha, strEntrada, "");
                if ( a < numTokens - 1 ) a++; //avanca o lookahead
            } else {
                reportarErro(symbolName(X), lookahead, tokens[a].posicao);
                return false;
            }
        } else {
            //X eh nao-terminal: consulta a TAS
            Producao *p = buscarProducao(X, lookahead);
            if ( p == NULL ) {
                char esperados[64];
                terminaisEsperados(X, esperados);
                reportarErro(esperados, lookahead, tokens[a].posicao);
                return false;
            } else {
                //empilha Yk...Y1 (na ordem inversa, Y1 fica no topo)
                for (int k = p->tamanho - 1; k >= 0; k--)
                    empilha(p->simbolos[k]);

                char producaoStr[32];
                if ( p->tamanho == 0 )
                    snprintf(producaoStr, sizeof(producaoStr), "%s->eps", symbolName(X));
                else {
                    char rhs[24] = "";
                    for (int k = 0; k < p->tamanho; k++)
                        strcat(rhs, symbolName(p->simbolos[k]));
                    snprintf(producaoStr, sizeof(producaoStr), "%s->%s", symbolName(X), rhs);
                }
                printf("%-20s | %-25s | %s\n", strPilha, strEntrada, producaoStr);
            }
        }
    } while ( X != SYM_DOLLAR );

    return true;
}


// 3) FUNCAO PRINCIPAL: le a entrada e responde se eh valida ou nao
 

int main() {
    char entrada[MAX_CHAR];
    int aceita;

    construirTAS();
    imprimirTAS();

    printf("\nInforme uma expressao aritmetica (soma e multiplicacao, ex: 3+4*5): ");
    scanf("%s", entrada);

    tokenizar(entrada);

    if ( erroLexico ) {
        printf("\nCadeia NAO pertencente a linguagem (erro lexico)\n");
        return 0;
    }

    aceita = parse();

    printf("---------------------------------------------------------------------\n");

    if ( aceita && !hasError ) {
        printf("\n>>> Cadeia PERTENCENTE a linguagem (expressao aritmetica valida)\n");
    } else {
        printf("\n>>> Cadeia NAO pertencente a linguagem (expressao aritmetica invalida)\n");
    }

    return 0;
}