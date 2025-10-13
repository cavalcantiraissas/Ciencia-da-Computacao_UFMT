#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>

#define MAX_LINE_LENGTH 1024
#define INFINITY INT_MAX

typedef struct adjacencia {
    int vertice;
    int peso;
    struct adjacencia *prox;
} ADJACENCIA;

typedef struct vertice {
    ADJACENCIA *cab;
} VERTICE;

typedef struct grafo {
    int numVertices;
    int arestas;
    VERTICE *adj;
} GRAFO;

typedef struct PQNode {
    int vertice;
    int chave;
} PQNode;

typedef struct FilaPrioridade {
    PQNode *array;
    int tamanho;
    int capacidade;
} FilaPrioridade;

GRAFO* criaGrafo(int numVertices) {
    GRAFO *g = (GRAFO *) malloc(sizeof(GRAFO));
    g->numVertices = numVertices;
    g->arestas = 0;
    g->adj = (VERTICE *) malloc(numVertices * sizeof(VERTICE));

    for (int i = 0; i < numVertices; i++)
        g->adj[i].cab = NULL;

    return g;
}

ADJACENCIA* criaAdjacencia(int vertice, int peso) {
    ADJACENCIA* temp = (ADJACENCIA *) malloc(sizeof(ADJACENCIA));
    temp->vertice = vertice;
    temp->peso = peso;
    temp->prox = NULL;
    return temp;
}

void criaAresta(GRAFO* g, int v, int w, int peso) {
    ADJACENCIA *novaAdj = criaAdjacencia(w, peso);
    novaAdj->prox = g->adj[v].cab;
    g->adj[v].cab = novaAdj;
    
    novaAdj = criaAdjacencia(v, peso);
    novaAdj->prox = g->adj[w].cab;
    g->adj[w].cab = novaAdj;
    g->arestas++;
}

GRAFO* lerGrafoDeArquivo(const char* nomeArquivo) {
    FILE* arquivo = fopen(nomeArquivo, "r");
    if (!arquivo) {
        perror("Erro ao abrir o arquivo");
        exit(1);
    }

    char linha[MAX_LINE_LENGTH];
    int numVertices = 0;

    while (fgets(linha, sizeof(linha), arquivo) != NULL) {
        if (linha[0] == 'p') {
            sscanf(linha, "p sp %d", &numVertices);
            break;
        }
    }

    GRAFO* grafo = criaGrafo(numVertices);
    int v, w, peso;

    while (fgets(linha, sizeof(linha), arquivo)) {
        if (sscanf(linha, "a %d %d %d", &v, &w, &peso) == 3) {
            criaAresta(grafo, v - 1, w - 1, peso);
        }
    }

    fclose(arquivo);
    return grafo;
}

FilaPrioridade* criaFilaPrioridade(int capacidade) {
    FilaPrioridade *fila = (FilaPrioridade *) malloc(sizeof(FilaPrioridade));
    fila->capacidade = capacidade;
    fila->tamanho = 0;
    fila->array = (PQNode *) malloc(capacidade * sizeof(PQNode));
    return fila;
}

void troca(PQNode *a, PQNode *b) {
    PQNode temp = *a;
    *a = *b;
    *b = temp;
}

void minHeapify(FilaPrioridade *fila, int indice, int *posicao, int *chave) {
    int menor = indice;
    int esq = 2 * indice + 1;
    int dir = 2 * indice + 2;

    if (esq < fila->tamanho && chave[fila->array[esq].vertice] < chave[fila->array[menor].vertice])
        menor = esq;

    if (dir < fila->tamanho && chave[fila->array[dir].vertice] < chave[fila->array[menor].vertice])
        menor = dir;

    if (menor != indice) {
        posicao[fila->array[indice].vertice] = menor;
        posicao[fila->array[menor].vertice] = indice;
        troca(&fila->array[indice], &fila->array[menor]);
        minHeapify(fila, menor, posicao, chave);
    }
}

int extraiMinimo(FilaPrioridade *fila, int *posicao, int *chave) {
    if (fila->tamanho == 0)
        return -1;

    PQNode raiz = fila->array[0];
    PQNode ultimo = fila->array[fila->tamanho - 1];
    fila->array[0] = ultimo;
    posicao[raiz.vertice] = fila->tamanho - 1;
    posicao[ultimo.vertice] = 0;
    fila->tamanho--;
    minHeapify(fila, 0, posicao, chave);

    return raiz.vertice;
}

void diminuiChave(FilaPrioridade *fila, int vertice, int novaChave, int *posicao, int *chave) {
    int i = posicao[vertice];
    chave[vertice] = novaChave;

    while (i && chave[vertice] < chave[fila->array[(i - 1) / 2].vertice]) {
        posicao[vertice] = (i - 1) / 2;
        posicao[fila->array[(i - 1) / 2].vertice] = i;
        troca(&fila->array[i], &fila->array[(i - 1) / 2]);
        i = (i - 1) / 2;
    }
}

int encontraArvoreGeradoraMinimaPrim(GRAFO *grafo, const char *arquivoSaida) {
    int numVertices = grafo->numVertices;
    int *chave = (int *) malloc(numVertices * sizeof(int));
    int *pai = (int *) malloc(numVertices * sizeof(int));
    int *naFila = (int *) malloc(numVertices * sizeof(int));

    for (int i = 0; i < numVertices; i++) {
        chave[i] = INFINITY;
        pai[i] = -1;
        naFila[i] = 1;
    }

    chave[0] = 0;

    FilaPrioridade *fila = criaFilaPrioridade(numVertices);
    for (int i = 0; i < numVertices; i++) {
        fila->array[i].vertice = i;
        fila->array[i].chave = chave[i];
        fila->tamanho++;
    }

    while (fila->tamanho != 0) {
        int u = extraiMinimo(fila, naFila, chave);
        naFila[u] = 0;

        ADJACENCIA *adj = grafo->adj[u].cab;
        while (adj) {
            int v = adj->vertice;
            int peso = adj->peso;

            if (naFila[v] && peso < chave[v]) {
                pai[v] = u;
                chave[v] = peso;
                diminuiChave(fila, v, peso, naFila, chave);
            }
            adj = adj->prox;
        }
    }

    FILE *saida = fopen(arquivoSaida, "w");
    if (!saida) {
        perror("Erro ao abrir o arquivo de saída");
        exit(1);
    }

    int custoTotal = 0;
    for (int i = 1; i < numVertices; i++) {
        custoTotal += chave[i];
        fprintf(saida, "Aresta (%d - %d) Peso: %d\n", pai[i], i, chave[i]);
    }

    fprintf(saida, "Custo total da árvore geradora mínima: %d\n", custoTotal);
    fclose(saida);

    free(chave);
    free(pai);
    free(naFila);

    return custoTotal;
}

int main() {
    clock_t inicio, fim;
    double tempo;
    
    GRAFO *grafoCAL = lerGrafoDeArquivo("USA-road-d.CAL.gr");
    GRAFO *grafoNY = lerGrafoDeArquivo("USA-road-d.NY.gr");

    inicio = clock();
    int custoCAL = encontraArvoreGeradoraMinimaPrim(grafoCAL, "saidaCAL.txt");
    fim = clock();
    tempo = (double)(fim - inicio) / CLOCKS_PER_SEC;
    printf("Custo CAL: %d, Tempo: %f segundos\n", custoCAL, tempo);

    inicio = clock();
    int custoNY = encontraArvoreGeradoraMinimaPrim(grafoNY, "saidaNY.txt");
    fim = clock();
    tempo = (double)(fim - inicio) / CLOCKS_PER_SEC;
    printf("Custo NY: %d, Tempo: %f segundos\n", custoNY, tempo);

    return 0;
}
