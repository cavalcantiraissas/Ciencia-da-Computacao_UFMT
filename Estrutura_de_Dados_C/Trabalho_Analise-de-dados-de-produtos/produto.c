#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include "produto.h"

Produto* criarProduto(const char* asin, const char* titulo, float estrelas, float preco, int categoria_id) {
    if (!asin || !titulo || preco < 0) return NULL;

    Produto* novo = (Produto*)malloc(sizeof(Produto));
    if (!novo) return NULL;

    strncpy(novo->asin, asin, MAX_STR-1);
    novo->asin[MAX_STR-1] = '\0';
    strncpy(novo->titulo, titulo, MAX_STR-1);
    novo->titulo[MAX_STR-1] = '\0';
    novo->estrelas = estrelas;
    novo->preco = preco;
    novo->categoria_id = categoria_id;
    novo->esquerda = novo->direita = NULL;
    
    return novo;
}

static void coletarProdutosOrdenados(Produto* raiz, Produto** array, int* index) {
    if (!raiz || *index >= MAX_PRODUTOS) return;
    
    coletarProdutosOrdenados(raiz->esquerda, array, index);
    array[(*index)++] = raiz;
    coletarProdutosOrdenados(raiz->direita, array, index);
}

static int compararDecrescente(const void* a, const void* b) {
    const Produto* p1 = *(const Produto**)a;
    const Produto* p2 = *(const Produto**)b;
    if (p2->preco > p1->preco) return 1;
    if (p2->preco < p1->preco) return -1;
    return 0;
}

static int compararCrescente(const void* a, const void* b) {
    const Produto* p1 = *(const Produto**)a;
    const Produto* p2 = *(const Produto**)b;
    if (p1->preco > p2->preco) return 1;
    if (p1->preco < p2->preco) return -1;
    return 0;
}

void topNProdutos(Produto* raiz, Produto** array, int* count, int max, int maisCaros) {
    *count = 0; 
    
    if (!raiz || max <= 0) return;

    Produto** todosProdutos = (Produto**)malloc(MAX_PRODUTOS * sizeof(Produto*));
    if (!todosProdutos) return;

    int totalProdutos = 0;
    coletarProdutosOrdenados(raiz, todosProdutos, &totalProdutos);

    if (totalProdutos == 0) {
        free(todosProdutos);
        return;
    }

    qsort(todosProdutos, totalProdutos, sizeof(Produto*), 
          maisCaros ? compararDecrescente : compararCrescente);

    *count = (totalProdutos < max) ? totalProdutos : max;
    
    for (int i = 0; i < *count; i++) {
        array[i] = todosProdutos[i];
    }

    free(todosProdutos);
}

void liberarProdutos(Produto* raiz) {
    if (!raiz) return;
    
    liberarProdutos(raiz->esquerda);
    liberarProdutos(raiz->direita);
    free(raiz);
}

Produto* inserirProduto(Produto* raiz, Produto* novo) {
    if (!novo) return raiz;
    if (!raiz) return novo;

    if (novo->preco < raiz->preco) {
        raiz->esquerda = inserirProduto(raiz->esquerda, novo);
    } else {
        raiz->direita = inserirProduto(raiz->direita, novo);
    }
    return raiz;
}

void listarProdutosArquivo(Produto* raiz, FILE* f) {
    if (!raiz || !f) return;
    
    listarProdutosArquivo(raiz->esquerda, f);
    fprintf(f, "%s; R$%.2f; %s\n", raiz->titulo, raiz->preco, raiz->asin);
    listarProdutosArquivo(raiz->direita, f);
}

void listarProdutosEntrePrecosArquivo(Produto* raiz, float x, float y, FILE* f) {
    if (!raiz || !f) return;
    
    if (raiz->preco > x) {
        listarProdutosEntrePrecosArquivo(raiz->esquerda, x, y, f);
    }
    
    if (raiz->preco >= x && raiz->preco <= y) {
        fprintf(f, "%s; R$%.2f; %s\n", raiz->titulo, raiz->preco, raiz->asin);
    }
    
    if (raiz->preco < y) {
        listarProdutosEntrePrecosArquivo(raiz->direita, x, y, f);
    }
}
