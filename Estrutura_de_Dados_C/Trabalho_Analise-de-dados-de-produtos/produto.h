#ifndef PRODUTO_H
#define PRODUTO_H

#include <stdio.h>
#include <float.h>

#define MAX_STR 256
#define MAX_PRODUTOS 1000000

typedef struct Produto {
    char asin[MAX_STR];
    char titulo[MAX_STR];
    float estrelas;
    float preco;
    int categoria_id;
    struct Produto* esquerda;
    struct Produto* direita;
} Produto;

Produto* criarProduto(const char* asin, const char* titulo, float estrelas, float preco, int categoria_id);
void liberarProdutos(Produto* raiz);
Produto* inserirProduto(Produto* raiz, Produto* novo);
void listarProdutosArquivo(Produto* raiz, FILE* f);  
void listarProdutosEntrePrecosArquivo(Produto* raiz, float x, float y, FILE* f);  
void topNProdutos(Produto* raiz, Produto** array, int* count, int max, int maisCaros);

#endif
