/***
 ** UFMT - Ciencia da Computacao
 ** Estrutura de Dados I - Prof. Ivairton M. Santos
 **
 ** Exercicio de implementacao de tabela Hash
 **
 **/
/*** Cabecalho para garantia de NAO multiplas declaracoes da biblioteca ***/
#ifndef TABHASH_C
#define TABHASH_C
/*** Inclusao de bibliotecas ***/
#include<stdlib.h>
#include<stdio.h>
#include "listaEnc.c"
/*** Constante ***/
#define N 10
/*** Definicao de estrutura ***/
struct est_tab_hash {
 tipo_lista *tabela[N];
 int qtd;
};
typedef struct est_tab_hash tipo_tab_hash;
/*** Prototipos ***/
int funcaoHash(int);
void inicializaTabHash(tipo_tab_hash*);
void insereTabHash(tipo_tab_hash*, int);
void imprimeTabHash(tipo_tab_hash*);
int buscaTabHash(tipo_tab_hash*, int k);
// Funcao hash
// PARAMETRO: chave (int)
// RETORNO: indice na tabela Hash (int) em razão da capacidade N da tabela
int funcaoHash(int k) {
 return ( k % N );
}
// Inicializa os componentes da tabela hash
// PARAMETRO: Tabela hahs (tipo_tab_hash*)
// RETORNO: -
void inicializaTabHash(tipo_tab_hash *tab_hash) {
 int i;
 tab_hash->qtd = 0;
 for (i=0; i<N; i++)
 tab_hash->tabela[i] = NULL;
}
// Insere dado na tabela hash, tratando as colisões por reposicionamento
// Nesta funcao, a chave será o próprio dado
// PARAMETRO: Tabela hahs (tipo_tab_hash*); chave (int)
// RETORNO: -
void insereTabHash(tipo_tab_hash *tab_hash, int k) {
 int pos;
 //Obtem posicao por meio da funcao hash
 pos = funcaoHash(k);
 //Inser o valor na lista ligada da tab hash
 insereFim(&tab_hash->tabela[pos], k);
 tab_hash->qtd++;
}
// Busca pela chave retornando verdade se encontrada
// PARAMETRO: tabela hash (tipo_tab_hash); chave (int) do dado
// RETORNO: verdadeiro se chave encontrada; falso caso contrário
int buscaTabHash(tipo_tab_hash *tab_hash, int k) {
 int pos;
 tipo_lista *aux;
 pos = funcaoHash(k);
 aux = tab_hash->tabela[pos];
 while ( aux != NULL ) {
 if (aux->valor == k)
 return 1;
 else
 aux = aux->prox;
 }
 return 0;
}
// Para debug, imprime a tabela hash
// PARAMETRO: tabela hash (tab_hash*)
// RETORNO: -
void imprimeTabHash(tipo_tab_hash *tab_hash) {
 int i;
 printf("Qtd de dados na tabela: %d\nTabela:\n", tab_hash->qtd);
 for ( i=0; i<N; i++) {
 printf("\t%d: ", i);
 imprimeLista(tab_hash->tabela[i]);
 }
 printf("\n");
}
#endif //TABHASH_C
