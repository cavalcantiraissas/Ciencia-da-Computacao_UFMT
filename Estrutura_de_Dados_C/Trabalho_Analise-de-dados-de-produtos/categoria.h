#ifndef CATEGORIA_H
#define CATEGORIA_H

#include <pthread.h>
#include "produto.h"  

#define MAX_NOME 100
#define CAPACIDADE_INICIAL_FILHOS 10

typedef struct Categoria {
    int id;
    char nome[MAX_NOME];
    struct Categoria* pai;
    struct Categoria** filhos;
    int numFilhos;
    int capacidadeFilhos;
    Produto* produtos;
    pthread_mutex_t mutex;
} Categoria;

Categoria* criarCategoria(int id, const char* nome);
void adicionarFilho(Categoria* pai, Categoria* filho);
void liberarCategorias(Categoria* raiz);
Categoria* buscarCategoriaPorId(Categoria* raiz, int id);
Categoria* buscarCategoriaPorNome(Categoria* raiz, const char* nome);
Categoria* inicializarHierarquia();
void carregarCategoriasCSV(const char* filename, Categoria* raiz);
void carregarProdutosCSV(const char* filename, Categoria* raiz);
void listarCategorias(Categoria* raiz, int nivel);

#endif
