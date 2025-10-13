#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <pthread.h>
#include "categoria.h"
#include "produto.h" 

Categoria* criarCategoria(int id, const char* nome) {
    Categoria* cat = (Categoria*)malloc(sizeof(Categoria));
    if (!cat) return NULL;
    
    cat->id = id;
    strncpy(cat->nome, nome, MAX_NOME-1);
    cat->nome[MAX_NOME-1] = '\0';
    cat->pai = NULL;
    cat->numFilhos = 0;
    cat->capacidadeFilhos = CAPACIDADE_INICIAL_FILHOS;
    cat->filhos = (Categoria**)malloc(sizeof(Categoria*) * cat->capacidadeFilhos);
    if (!cat->filhos) {
        free(cat);
        return NULL;
    }
    cat->produtos = NULL;
    pthread_mutex_init(&cat->mutex, NULL);
    
    return cat;
}

void adicionarFilho(Categoria* pai, Categoria* filho) {
    if (!pai || !filho) return;
    
    pthread_mutex_lock(&pai->mutex);
    
    if (pai->numFilhos >= pai->capacidadeFilhos) {
        int nova_capacidade = pai->capacidadeFilhos * 2;
        Categoria** novos_filhos = (Categoria**)realloc(pai->filhos, 
                                    sizeof(Categoria*) * nova_capacidade);
        if (!novos_filhos) {
            pthread_mutex_unlock(&pai->mutex);
            return;
        }
        pai->filhos = novos_filhos;
        pai->capacidadeFilhos = nova_capacidade;
    }
    
    pai->filhos[pai->numFilhos++] = filho;
    filho->pai = pai;
    
    pthread_mutex_unlock(&pai->mutex);
}

void liberarCategorias(Categoria* raiz) {
    if (!raiz) return;
    
    pthread_mutex_lock(&raiz->mutex);
    
    for (int i = 0; i < raiz->numFilhos; i++) {
        liberarCategorias(raiz->filhos[i]);
    }
    
    liberarProdutos(raiz->produtos);
    free(raiz->filhos);
    
    pthread_mutex_unlock(&raiz->mutex);
    pthread_mutex_destroy(&raiz->mutex);
    free(raiz);
}

Categoria* buscarCategoriaPorId(Categoria* raiz, int id) {
    if (!raiz) return NULL;
    if (raiz->id == id) return raiz;
    
    for (int i = 0; i < raiz->numFilhos; i++) {
        Categoria* encontrada = buscarCategoriaPorId(raiz->filhos[i], id);
        if (encontrada) return encontrada;
    }
    return NULL;
}

Categoria* buscarCategoriaPorNome(Categoria* raiz, const char* nome) {
    if (!raiz || !nome) return NULL;
    if (strcmp(raiz->nome, nome) == 0) return raiz;
    
    for (int i = 0; i < raiz->numFilhos; i++) {
        Categoria* encontrada = buscarCategoriaPorNome(raiz->filhos[i], nome);
        if (encontrada) return encontrada;
    }
    return NULL;
}

Categoria* inicializarHierarquia() {
    Categoria* raiz = criarCategoria(0, "Diverso");
    if (!raiz) return NULL;
    
    Categoria* pessoal = criarCategoria(-1, "Pessoal");
    Categoria* eletronicos = criarCategoria(-1, "Eletrônicos");
    
    adicionarFilho(raiz, pessoal);
    adicionarFilho(raiz, eletronicos);
    adicionarFilho(raiz, criarCategoria(-1, "Casa"));
    adicionarFilho(raiz, criarCategoria(-1, "Automotivo"));
    adicionarFilho(raiz, criarCategoria(-1, "Saúde"));
    adicionarFilho(raiz, criarCategoria(-1, "Educação"));
    adicionarFilho(raiz, criarCategoria(-1, "Diversão"));
    adicionarFilho(raiz, criarCategoria(-1, "Viagem"));
    adicionarFilho(raiz, criarCategoria(-1, "Pet"));
    adicionarFilho(raiz, criarCategoria(-1, "Esporte"));
    
    if (pessoal) {
        adicionarFilho(pessoal, criarCategoria(-1, "Homem"));
        adicionarFilho(pessoal, criarCategoria(-1, "Mulher"));
        adicionarFilho(pessoal, criarCategoria(-1, "Criança"));
        adicionarFilho(pessoal, criarCategoria(-1, "Bebê"));
    }
    
    if (eletronicos) {
        adicionarFilho(eletronicos, criarCategoria(-1, "Computador"));
        adicionarFilho(eletronicos, criarCategoria(-1, "Celular"));
        adicionarFilho(eletronicos, criarCategoria(-1, "Videogame"));
    }
    
    return raiz;
}

void carregarCategoriasCSV(const char* filename, Categoria* raiz) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Erro ao abrir arquivo de categorias");
        return;
    }

    char line[1024];
    fgets(line, sizeof(line), file); 
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = 0;
        
        char* id_str = strtok(line, ",");
        char* nome = strtok(NULL, ",");
        
        if (!id_str || !nome) continue;
        
        int id = atoi(id_str);
        if (id <= 0) continue;
        
        Categoria* nova = criarCategoria(id, nome);
        if (nova) {
            adicionarFilho(raiz, nova);
        }
    }
    
    fclose(file);
}

void carregarProdutosCSV(const char* filename, Categoria* raiz) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Erro ao abrir arquivo de produtos");
        return;
    }

    char line[2048];
    fgets(line, sizeof(line), file); 
    
    int produtos_carregados = 0;
    int categorias_nao_encontradas = 0;
    int linhas_invalidas = 0;
    int produtos_sem_preco = 0;
    
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = '\0';
        
        char* tokens[10];
        int i = 0;
        char* token = strtok(line, ",");
        
        while (token && i < 10) {
            tokens[i++] = token;
            token = strtok(NULL, ",");
        }

        if (i < 9) {
            linhas_invalidas++;
            continue;
        }

        float price = atof(tokens[6]);
        int category_id = atoi(tokens[8]);
        
        if (price <= 0.0f) {
            produtos_sem_preco++;
            continue;
        }

        Produto* p = criarProduto(tokens[0], tokens[1], atof(tokens[4]), price, category_id);
        if (!p) {
            linhas_invalidas++;
            continue;
        }

        Categoria* cat = buscarCategoriaPorId(raiz, category_id);
        if (!cat) {
            cat = buscarCategoriaPorNome(raiz, "Diverso");
            if (!cat) {
                categorias_nao_encontradas++;
                free(p);
                continue;
            }
        }
        
        pthread_mutex_lock(&cat->mutex);
        cat->produtos = inserirProduto(cat->produtos, p);
        pthread_mutex_unlock(&cat->mutex);
        produtos_carregados++;
    }
    
    fclose(file);
    
    printf("\nResumo do carregamento:\n");
    printf("- Produtos carregados: %d\n", produtos_carregados);
    printf("- Produtos ignorados (sem preço válido): %d\n", produtos_sem_preco);
    printf("- Linhas inválidas: %d\n", linhas_invalidas);
    printf("- Categorias não encontradas: %d\n", categorias_nao_encontradas);
}

void listarCategorias(Categoria* raiz, int nivel) {
    if (!raiz) return;
    
    for (int i = 0; i < nivel; i++) printf("  ");
    printf("- %s (%d)\n", raiz->nome, raiz->id);
    
    for (int i = 0; i < raiz->numFilhos; i++) {
        listarCategorias(raiz->filhos[i], nivel + 1);
    }
}
