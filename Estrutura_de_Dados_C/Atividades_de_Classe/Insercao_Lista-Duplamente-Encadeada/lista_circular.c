#include <stdio.h>
#include <stdlib.h>
#include "lista_circular.h"

ListaCircular* criarListaCircular() {
    ListaCircular* lista = (ListaCircular*)malloc(sizeof(ListaCircular));
    if (!lista) return NULL;
    lista->fim = NULL;
    return lista;
}

void inserirInicioC(ListaCircular* lista, int valor) {
    NoC* novo = (NoC*)malloc(sizeof(NoC));
    if (!novo) return;
    novo->dado = valor;

    if (!lista->fim) { 
        novo->prox = novo;
        lista->fim = novo;
    } else {
        novo->prox = lista->fim->prox;
        lista->fim->prox = novo;
    }
}

void inserirFimC(ListaCircular* lista, int valor) {
    NoC* novo = (NoC*)malloc(sizeof(NoC));
    if (!novo) return;
    novo->dado = valor;

    if (!lista->fim) { 
        novo->prox = novo;
        lista->fim = novo;
    } else {
        novo->prox = lista->fim->prox;
        lista->fim->prox = novo;
        lista->fim = novo;
    }
}

void removerInicioC(ListaCircular* lista) {
    if (!lista->fim) return; 

    NoC* primeiro = lista->fim->prox;

    if (lista->fim == primeiro) { 
        lista->fim = NULL;
    } else {
        lista->fim->prox = primeiro->prox;
    }

    free(primeiro);
}

void removerFimC(ListaCircular* lista) {
    if (!lista->fim) return; 

    NoC* atual = lista->fim->prox;
    NoC* anterior = NULL;

    if (atual == lista->fim) { 
        free(lista->fim);
        lista->fim = NULL;
        return;
    }

    while (atual->prox != lista->fim->prox) {
        anterior = atual;
        atual = atual->prox;
    }

    anterior->prox = lista->fim->prox;
    free(lista->fim);
    lista->fim = anterior;
}

void girarLista(ListaCircular* lista) {
    if (!lista->fim || lista->fim->prox == lista->fim) return; 

    lista->fim = lista->fim->prox; 
}

void exibirListaCircular(ListaCircular* lista) {
    if (!lista->fim) {
        printf("Lista Circular Vazia\n");
        return;
    }

    NoC* atual = lista->fim->prox;
    do {
        printf("%d -> ", atual->dado);
        atual = atual->prox;
    } while (atual != lista->fim->prox);
    
    printf("(volta ao início)\n");
}

void liberarListaCircular(ListaCircular* lista) {
    if (!lista->fim) {
        free(lista);
        return;
    }

    NoC* atual = lista->fim->prox;
    NoC* temp;

    while (atual != lista->fim) {
        temp = atual;
        atual = atual->prox;
        free(temp);
    }

    free(lista->fim);
    free(lista);
}
