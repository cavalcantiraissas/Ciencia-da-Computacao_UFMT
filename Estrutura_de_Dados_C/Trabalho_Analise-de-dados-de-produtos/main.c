#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "utils.h"
#include "categoria.h"

void handle_sigsegv(int sig) {
    fprintf(stderr, "\nErro: Segmentation fault detectado!\n");
    exit(EXIT_FAILURE);
}

int main() {
    signal(SIGSEGV, handle_sigsegv);
    
    printf("Inicializando hierarquia de categorias...\n");
    Categoria* raiz = inicializarHierarquia();
    if (!raiz) {
        fprintf(stderr, "Erro ao criar hierarquia de categorias!\n");
        return EXIT_FAILURE;
    }

    printf("Carregando categorias...\n");
    carregarCategoriasCSV("amazon_categories.csv", raiz);
    
    printf("Carregando produtos...\n");
    carregarProdutosCSV("amazon_products.csv", raiz);

    printf("\n=== RELATÓRIO 1: Produtos por faixa de preço ===\n");
    float min, max;
    printf("Informe o valor mínimo: ");
    scanf("%f", &min);
    printf("Informe o valor máximo: ");
    scanf("%f", &max);

    FILE* f1 = fopen("relatorio_faixa_preco.txt", "w");
    if (f1) {
        gerarRelatorioFaixaPreco(raiz, min, max, f1);
        fclose(f1);
        printf("Relatório 'relatorio_faixa_preco.txt' gerado com sucesso.\n");
    } else {
        perror("Erro ao criar relatório de faixa de preço");
    }

    printf("\n=== RELATÓRIO 2: Produtos por categoria ===\n");
    printf("Categorias disponíveis:\n");
    listarCategoriasDisponiveis(raiz);

    char nomeCategoria[100];
    printf("\nDigite o nome da categoria: ");
    scanf(" %[^\n]", nomeCategoria);
    
    FILE* f2 = fopen("relatorio_categoria_escolhida.txt", "w");
    if (f2) {
        gerarRelatorioPorCategoria(raiz, nomeCategoria, f2);
        fclose(f2);
        printf("Relatório 'relatorio_categoria_escolhida.txt' gerado com sucesso.\n");
    } else {
        perror("Erro ao criar relatório por categoria");
    }

    printf("\n=== RELATÓRIO 3: Top 5 mais caros por categoria ===\n");
    FILE* f3 = fopen("relatorio_top5_caros.txt", "w");
    if (f3) {
        gerarTopNMaisCarosPorCategoria(raiz, 5, f3);
        fclose(f3);
        printf("Relatório 'relatorio_top5_caros.txt' gerado com sucesso.\n");
    } else {
        perror("Erro ao criar relatório de produtos mais caros");
    }

    printf("\n=== RELATÓRIO 4: Top 5 mais baratos por categoria ===\n");
    FILE* f4 = fopen("relatorio_top5_baratos.txt", "w");
    if (f4) {
        gerarTopNMaisBaratosPorCategoria(raiz, 5, f4);
        fclose(f4);
        printf("Relatório 'relatorio_top5_baratos.txt' gerado com sucesso.\n");
    } else {
        perror("Erro ao criar relatório de produtos mais baratos");
    }

    liberarCategorias(raiz);
    return EXIT_SUCCESS;
}
