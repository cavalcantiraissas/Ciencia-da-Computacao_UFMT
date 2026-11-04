/***
 ** UFMT - Ciencia da Computacao
 ** Estrutura de Dados I
 **
 ** Lista simplesmente encadeada de inteiros, usada pela Tabela_Hash.c
 ** para guardar as chaves que colidem na mesma posicao da tabela
 ** (tratamento de colisoes por encadeamento).
 **
 **/

/*** Cabecalho para garantia de NAO multiplas declaracoes da biblioteca ***/
#ifndef LISTAENC_C
#define LISTAENC_C

/*** Inclusao de bibliotecas ***/
#include<stdlib.h>
#include<stdio.h>

/*** Definicao de estrutura ***/
struct est_lista {
 int valor;
 struct est_lista *prox;
};
typedef struct est_lista tipo_lista;

/*** Prototipos ***/
tipo_lista* criaNo(int);
void insereInicio(tipo_lista**, int);
void insereFim(tipo_lista**, int);
int removeValor(tipo_lista**, int);
int buscaLista(tipo_lista*, int);
void imprimeLista(tipo_lista*);
void liberaLista(tipo_lista**);

// Aloca um novo no com o valor informado
// PARAMETRO: valor (int)
// RETORNO: ponteiro para o novo no (tipo_lista*)
tipo_lista* criaNo(int vlr) {
 tipo_lista *novo = (tipo_lista*) malloc(sizeof(tipo_lista));
 if (novo == NULL) {
  printf("Erro: memoria insuficiente.\n");
  exit(1);
 }
 novo->valor = vlr;
 novo->prox = NULL;
 return novo;
}

// Insere um valor no inicio da lista
// PARAMETRO: endereco do ponteiro da lista (tipo_lista**); valor (int)
// RETORNO: -
void insereInicio(tipo_lista **lst, int vlr) {
 tipo_lista *novo = criaNo(vlr);
 novo->prox = *lst;
 *lst = novo;
}

// Insere um valor no fim da lista
// PARAMETRO: endereco do ponteiro da lista (tipo_lista**); valor (int)
// RETORNO: -
void insereFim(tipo_lista **lst, int vlr) {
 tipo_lista *novo = criaNo(vlr);
 tipo_lista *aux;
 if (*lst == NULL) {
  *lst = novo;
  return;
 }
 aux = *lst;
 while (aux->prox != NULL)
  aux = aux->prox;
 aux->prox = novo;
}

// Remove a primeira ocorrencia de um valor
// PARAMETRO: endereco do ponteiro da lista (tipo_lista**); valor (int)
// RETORNO: verdadeiro se o valor foi removido; falso caso nao exista
int removeValor(tipo_lista **lst, int vlr) {
 tipo_lista *aux = *lst, *ant = NULL;
 while (aux != NULL && aux->valor != vlr) {
  ant = aux;
  aux = aux->prox;
 }
 if (aux == NULL)
  return 0;
 if (ant == NULL)
  *lst = aux->prox;
 else
  ant->prox = aux->prox;
 free(aux);
 return 1;
}

// Busca um valor na lista
// PARAMETRO: lista (tipo_lista*); valor (int)
// RETORNO: verdadeiro se encontrado; falso caso contrario
int buscaLista(tipo_lista *lst, int vlr) {
 while (lst != NULL) {
  if (lst->valor == vlr)
   return 1;
  lst = lst->prox;
 }
 return 0;
}

// Imprime os valores da lista em uma linha
// PARAMETRO: lista (tipo_lista*)
// RETORNO: -
void imprimeLista(tipo_lista *lst) {
 while (lst != NULL) {
  printf("%d -> ", lst->valor);
  lst = lst->prox;
 }
 printf("NULL\n");
}

// Libera todos os nos da lista
// PARAMETRO: endereco do ponteiro da lista (tipo_lista**)
// RETORNO: -
void liberaLista(tipo_lista **lst) {
 tipo_lista *aux;
 while (*lst != NULL) {
  aux = *lst;
  *lst = aux->prox;
  free(aux);
 }
}

#endif //LISTAENC_C
