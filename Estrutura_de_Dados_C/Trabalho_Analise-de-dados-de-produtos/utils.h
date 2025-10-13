#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include "categoria.h"

void gerarRelatorioFaixaPreco(Categoria* raiz, float min, float max, FILE* f);
void gerarRelatorioPorCategoria(Categoria* raiz, const char* nome, FILE* f);
void gerarTopNMaisCarosPorCategoria(Categoria* raiz, int n, FILE* f);
void gerarTopNMaisBaratosPorCategoria(Categoria* raiz, int n, FILE* f);
void listarCategoriasDisponiveis(Categoria* raiz);

#endif
