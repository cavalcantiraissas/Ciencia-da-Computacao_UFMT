#ifndef LISTA_H
#define LISTA_H

#include <stdio.h>
#include <stdlib.h>

// Definição da estrutura do nó
typedef struct No {
    int valor;
    struct No *prox;
} No;

// Funções de manipulação da lista
void insereInicio(No **lst, int vlr);
void insereFim(No **lst, int vlr);
void inserePos(No **lst, int pos, int vlr);
void removeInicio(No **lst);
void removeFim(No **lst);
void removePos(No **lst, int pos);
void removeValor(No **lst, int vlr);
int buscaValor(No *lst, int vlr);
int buscaPosicao(No *lst, int pos);
int contaElementos(No *lst);
void imprimeLista(No *lst);

#endif
