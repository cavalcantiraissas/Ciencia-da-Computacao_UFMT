#include <stdio.h>
#include <stdlib.h>
#include "lista_dupla.h"

ListaDupla* criarListaDupla() {
    ListaDupla* lista = (ListaDupla*)malloc(sizeof(ListaDupla));
    lista->inicio = lista->fim = NULL;
    return lista;
}

void inserirInicio(ListaDupla* lista, int dado) {
    No* novo = (No*)malloc(sizeof(No));
    novo->dado = dado;
    novo->prox = lista->inicio;
    novo->ant = NULL;
    
    if (lista->inicio) lista->inicio->ant = novo;
    else lista->fim = novo;

    lista->inicio = novo;
}

void inserirFim(ListaDupla* lista, int dado) {
    No* novo = (No*)malloc(sizeof(No));
    novo->dado = dado;
    novo->prox = NULL;
    novo->ant = lista->fim;
    
    if (lista->fim) lista->fim->prox = novo;
    else lista->inicio = novo;

    lista->fim = novo;
}

void inserirPosicao(ListaDupla* lista, int dado, int posicao) {
    if (posicao == 0) {
        inserirInicio(lista, dado);
        return;
    }

    No* atual = lista->inicio;
    for (int i = 0; atual && i < posicao - 1; i++)
        atual = atual->prox;

    if (!atual) {
        inserirFim(lista, dado);
        return;
    }

    No* novo = (No*)malloc(sizeof(No));
    novo->dado = dado;
    novo->prox = atual->prox;
    novo->ant = atual;

    if (atual->prox) atual->prox->ant = novo;
    else lista->fim = novo;

    atual->prox = novo;
}

void removerInicio(ListaDupla* lista) {
    if (!lista->inicio) return;
    
    No* temp = lista->inicio;
    lista->inicio = temp->prox;
    if (lista->inicio) lista->inicio->ant = NULL;
    else lista->fim = NULL;
    
    free(temp);
}

void removerFim(ListaDupla* lista) {
    if (!lista->fim) return;

    No* temp = lista->fim;
    lista->fim = temp->ant;
    if (lista->fim) lista->fim->prox = NULL;
    else lista->inicio = NULL;

    free(temp);
}

void removerPosicao(ListaDupla* lista, int posicao) {
    if (!lista->inicio) return;
    if (posicao == 0) {
        removerInicio(lista);
        return;
    }

    No* atual = lista->inicio;
    for (int i = 0; atual && i < posicao; i++)
        atual = atual->prox;

    if (!atual) return;

    if (atual->prox) atual->prox->ant = atual->ant;
    if (atual->ant) atual->ant->prox = atual->prox;

    if (lista->fim == atual) lista->fim = atual->ant;

    free(atual);
}

void removerValor(ListaDupla* lista, int dado) {
    No* atual = lista->inicio;
    while (atual && atual->dado != dado)
        atual = atual->prox;

    if (!atual) return;

    if (atual == lista->inicio) removerInicio(lista);
    else if (atual == lista->fim) removerFim(lista);
    else {
        atual->ant->prox = atual->prox;
        atual->prox->ant = atual->ant;
        free(atual);
    }
}

void exibirListaDupla(ListaDupla* lista) {
    No* atual = lista->inicio;
    while (atual) {
        printf("%d <-> ", atual->dado);
        atual = atual->prox;
    }
    printf("NULL\n");
}

void liberarListaDupla(ListaDupla* lista) {
    while (lista->inicio) removerInicio(lista);
    free(lista);
}
