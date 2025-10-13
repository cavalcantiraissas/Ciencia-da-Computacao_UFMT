#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"
#include "categoria.h"  
#include "produto.h"    

void gerarTopNMaisCarosPorCategoria(Categoria* raiz, int n, FILE* f) {
    if (!raiz || !f || n <= 0) return;

    pthread_mutex_lock(&raiz->mutex);
    int temProdutos = (raiz->produtos != NULL);
    pthread_mutex_unlock(&raiz->mutex);

    if (temProdutos) {
        fprintf(f, "\nCategoria: %s - TOP %d MAIS CAROS\n", raiz->nome, n);
        
        Produto** topN = (Produto**)malloc(n * sizeof(Produto*));
        if (!topN) return;
        
        int count = 0;
        pthread_mutex_lock(&raiz->mutex);
        topNProdutos(raiz->produtos, topN, &count, n, 1);
        pthread_mutex_unlock(&raiz->mutex);
        
        if (count > 0) {
            for (int i = 0; i < count; i++) {
                fprintf(f, "%d. %s - R$%.2f (ASIN: %s)\n", 
                        i+1, topN[i]->titulo, topN[i]->preco, topN[i]->asin);
            }
        } else {
            fprintf(f, "  (Nenhum produto encontrado)\n");
        }
        
        free(topN);
    }

    for (int i = 0; i < raiz->numFilhos; i++) {
        gerarTopNMaisCarosPorCategoria(raiz->filhos[i], n, f);
    }
}

void gerarTopNMaisBaratosPorCategoria(Categoria* raiz, int n, FILE* f) {
    if (!raiz || !f || n <= 0) return;

    pthread_mutex_lock(&raiz->mutex);
    int temProdutos = (raiz->produtos != NULL);
    pthread_mutex_unlock(&raiz->mutex);

    if (temProdutos) {
        fprintf(f, "\nCategoria: %s - TOP %d MAIS BARATOS\n", raiz->nome, n);
        
        Produto** topN = (Produto**)malloc(n * sizeof(Produto*));
        if (!topN) return;
        
        int count = 0;
        pthread_mutex_lock(&raiz->mutex);
        topNProdutos(raiz->produtos, topN, &count, n, 0);
        pthread_mutex_unlock(&raiz->mutex);
        
        if (count > 0) {
            for (int i = 0; i < count; i++) {
                fprintf(f, "%d. %s - R$%.2f (ASIN: %s)\n", 
                        i+1, topN[i]->titulo, topN[i]->preco, topN[i]->asin);
            }
        } else {
            fprintf(f, "  (Nenhum produto encontrado)\n");
        }
        
        free(topN);
    }

    for (int i = 0; i < raiz->numFilhos; i++) {
        gerarTopNMaisBaratosPorCategoria(raiz->filhos[i], n, f);
    }
}

void listarCategoriasDisponiveis(Categoria* raiz) {
    if (!raiz) return;
    
    pthread_mutex_lock(&raiz->mutex);
    int temProdutos = (raiz->produtos != NULL);
    pthread_mutex_unlock(&raiz->mutex);
    
    if (temProdutos || raiz->numFilhos > 0) {
        printf("- %s (%s)\n", raiz->nome, temProdutos ? "contém produtos" : "categoria pai");
        
        for (int i = 0; i < raiz->numFilhos; i++) {
            listarCategoriasDisponiveis(raiz->filhos[i]);
        }
    }
}

void gerarRelatorioFaixaPreco(Categoria* raiz, float min, float max, FILE* f) {
    if (!raiz || !f) return;
    
    pthread_mutex_lock(&raiz->mutex);
    if (raiz->produtos) {
        fprintf(f, "\nCategoria: %s\n", raiz->nome);
        listarProdutosEntrePrecosArquivo(raiz->produtos, min, max, f);
    }
    pthread_mutex_unlock(&raiz->mutex);
    
    for (int i = 0; i < raiz->numFilhos; i++) {
        gerarRelatorioFaixaPreco(raiz->filhos[i], min, max, f);
    }
}

void gerarRelatorioPorCategoria(Categoria* raiz, const char* nome, FILE* f) {
    if (!raiz || !nome || !f) return;
    
    if (strcmp(raiz->nome, nome) == 0) {
        pthread_mutex_lock(&raiz->mutex);
        fprintf(f, "\nProdutos da categoria %s:\n", nome);
        if (raiz->produtos) {
            listarProdutosArquivo(raiz->produtos, f);
        } else {
            fprintf(f, "Nenhum produto encontrado nesta categoria.\n");
        }
        pthread_mutex_unlock(&raiz->mutex);
        return;
    }
    
    for (int i = 0; i < raiz->numFilhos; i++) {
        gerarRelatorioPorCategoria(raiz->filhos[i], nome, f);
    }
}
