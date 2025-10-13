#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_LINE_LENGTH 256

typedef struct {
    int vertice1;
    int vertice2;
    int peso;
} Aresta;

typedef struct {
    int numVertices;
    int numArestas;
    Aresta* arestas;
} Grafo;

Grafo* criaGrafo(int numVertices, int numArestas) {
    Grafo* grafo = (Grafo*) malloc(sizeof(Grafo));
    if (grafo == NULL) {
        fprintf(stderr, "Erro ao alocar memória para o grafo.\n");
        exit(EXIT_FAILURE);
    }
    grafo->numVertices = numVertices;
    grafo->numArestas = numArestas;
    
    grafo->arestas = (Aresta*) malloc(numArestas * sizeof(Aresta));
    if (grafo->arestas == NULL) {
        fprintf(stderr, "Erro ao alocar memória para as arestas.\n");
        free(grafo);
        exit(EXIT_FAILURE);
    }
    
    return grafo;
}

void lerGrafoDeArquivo(const char* nomeArquivo, Grafo* grafo) {
    FILE* arquivo = fopen(nomeArquivo, "r");
    if (arquivo == NULL) {
        perror("Erro ao abrir o arquivo");
        exit(EXIT_FAILURE);
    }

    char linha[MAX_LINE_LENGTH];
    int numArestas = 0;
    
    while (fgets(linha, sizeof(linha), arquivo) != NULL) {
        if (linha[0] == 'p') {
            sscanf(linha, "p sp %d %d", &grafo->numVertices, &grafo->numArestas);
            grafo->arestas = (Aresta*) realloc(grafo->arestas, grafo->numArestas * sizeof(Aresta));
            if (grafo->arestas == NULL) {
                fprintf(stderr, "Erro ao realocar memória para as arestas.\n");
                fclose(arquivo);
                exit(EXIT_FAILURE);
            }
            break;
        }
    }

    while (fgets(linha, sizeof(linha), arquivo)) {
        int v1, v2, peso;
        if (sscanf(linha, "a %d %d %d", &v1, &v2, &peso) == 3) {
            grafo->arestas[numArestas].vertice1 = v1 - 1;  
            grafo->arestas[numArestas].vertice2 = v2 - 1; 
            grafo->arestas[numArestas].peso = peso;
            numArestas++;
            if (numArestas >= grafo->numArestas) {
                break;  
            }
        }
    }

    fclose(arquivo);
}

int compare(const void* a, const void* b) {
    Aresta* aresta1 = (Aresta*)a;
    Aresta* aresta2 = (Aresta*)b;
    return aresta1->peso - aresta2->peso;
}

int find(int* parent, int i) {
    if (parent[i] != i) {
        parent[i] = find(parent, parent[i]); 
    }
    return parent[i];
}

void unionSets(int* parent, int* rank, int x, int y) {
    int xroot = find(parent, x);
    int yroot = find(parent, y);

    if (rank[xroot] < rank[yroot]) {
        parent[xroot] = yroot;
    } else if (rank[xroot] > rank[yroot]) {
        parent[yroot] = xroot;
    } else {
        parent[yroot] = xroot;
        rank[xroot]++;
    }
}

void kruskal(Grafo* grafo, int* custoTotal, double* tempoTotal) {
    Aresta* resultado = (Aresta*) malloc((grafo->numVertices - 1) * sizeof(Aresta));
    if (resultado == NULL) {
        fprintf(stderr, "Erro ao alocar memória para o resultado.\n");
        exit(EXIT_FAILURE);
    }

    qsort(grafo->arestas, grafo->numArestas, sizeof(grafo->arestas[0]), compare);

    int* parent = (int*) malloc(grafo->numVertices * sizeof(int));
    int* rank = (int*) malloc(grafo->numVertices * sizeof(int));
    if (parent == NULL || rank == NULL) {
        fprintf(stderr, "Erro ao alocar memória para parent ou rank.\n");
        free(resultado);
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < grafo->numVertices; i++) {
        parent[i] = i;
        rank[i] = 0;
    }

    int e = 0; 
    *custoTotal = 0; 
    clock_t inicio = clock();

    for (int i = 0; i < grafo->numArestas && e < grafo->numVertices - 1; i++) {
        Aresta aresta = grafo->arestas[i];

        int x = find(parent, aresta.vertice1);
        int y = find(parent, aresta.vertice2);

        if (x != y) {
            resultado[e++] = aresta;
            *custoTotal += aresta.peso; 
            unionSets(parent, rank, x, y);
        }
    }

    clock_t fim = clock(); 
    *tempoTotal = (double)(fim - inicio) / CLOCKS_PER_SEC; 

    printf("Arestas na árvore geradora mínima:\n");
    for (int i = 0; i < e; i++) {
        printf("%d - %d: %d\n", resultado[i].vertice1 + 1, resultado[i].vertice2 + 1, resultado[i].peso);
    }

    free(parent);
    free(rank);
    free(resultado);
}

void salvaResultado(const char* nomeArquivo, int custoTotal, double tempoTotal) {
    FILE* arquivo = fopen(nomeArquivo, "w");
    if (arquivo == NULL) {
        perror("Erro ao abrir o arquivo de saída");
        exit(EXIT_FAILURE);
    }
    fprintf(arquivo, "Custo total da árvore geradora mínima: %d\n", custoTotal);
    fprintf(arquivo, "Tempo total: %.6f segundos\n", tempoTotal);
    fclose(arquivo);
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Uso: %s <nome_do_arquivo> <arquivo_de_saida>\n", argv[0]);
        return EXIT_FAILURE;
    }

    Grafo* grafo = criaGrafo(0, 0);
    lerGrafoDeArquivo(argv[1], grafo);

    int custoTotal = 0;
    double tempoTotal = 0.0;
    kruskal(grafo, &custoTotal, &tempoTotal);
    
    salvaResultado(argv[2], custoTotal, tempoTotal);

    free(grafo->arestas);
    free(grafo);
    
    return EXIT_SUCCESS;
}
