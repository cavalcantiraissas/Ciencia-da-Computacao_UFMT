#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define true 1
#define false 0
#define INT_MAX 32000
#define START 1
typedef int bool;
typedef int TIPOPESO;

typedef struct adjacencia {
    int vertice; 
    TIPOPESO peso; 
    struct adjacencia *prox; 
} ADJACENCIA;

typedef struct vertice {
    ADJACENCIA *cab; 
} VERTICE;

typedef struct grafo {
    int vertices;
    int arestas; 
    VERTICE *adj; 
} GRAFO;

GRAFO *criarGrafo(int v) {
    GRAFO *g = (GRAFO *)malloc(sizeof(GRAFO));
    g->vertices = v;
    g->arestas = 0;
    g->adj = (VERTICE *)malloc(v * sizeof(VERTICE));
    for (int i = 0; i < v; i++)
        g->adj[i].cab = NULL;
    return g;
}

ADJACENCIA *criaAdj(int v, int peso) {
    ADJACENCIA *temp = (ADJACENCIA *)malloc(sizeof(ADJACENCIA));
    temp->vertice = v;
    temp->peso = peso;
    temp->prox = NULL;
    return temp;
}

ADJACENCIA *inseriListAdj(ADJACENCIA *adj, int vf, TIPOPESO p) {
    ADJACENCIA *novo = criaAdj(vf, p);
    novo->prox = adj;
    return novo;
}

bool criaAresta(GRAFO *gr, int vi, int vf, TIPOPESO p) {
    if (!gr) return false;
    if (vf < 0 || vf >= gr->vertices) return false;
    if (vi < 0 || vi >= gr->vertices) return false;

    gr->adj[vi].cab = inseriListAdj(gr->adj[vi].cab, vf, p);
    gr->arestas++;
    return true;
}

void imprime(GRAFO *gr) {
    printf("Vertices: %d. Arestas: %d\n", gr->vertices, gr->arestas);
    for (int i = START; i < gr->vertices; i++) {
        printf("v%d: ", i);
        ADJACENCIA *ad = gr->adj[i].cab;
        while (ad) {
            printf("v%d(%d) ", ad->vertice, ad->peso);
            ad = ad->prox;
        }
        printf("\n");
    }
}

// ----------- DIJKSTRA ---------------------------
int *dijkstra(GRAFO *g, int s, int *p) {
    int *dist = (int *)malloc(g->vertices * sizeof(int));
    bool *visitado = (bool *)malloc(g->vertices * sizeof(bool));

    for (int i = 0; i < g->vertices; i++) {
        dist[i] = INT_MAX;
        visitado[i] = false;
        p[i] = -1;
    }
    dist[s] = 0;

    for (int i = 0; i < g->vertices - 1; i++) {
        int u = -1;
        for (int j = 0; j < g->vertices; j++) {
            if (!visitado[j] && (u == -1 || dist[j] < dist[u])) {
                u = j;
            }
        }
        visitado[u] = true;

        for (ADJACENCIA *adj = g->adj[u].cab; adj != NULL; adj = adj->prox) {
            if (dist[u] + adj->peso < dist[adj->vertice]) {
                dist[adj->vertice] = dist[u] + adj->peso;
                p[adj->vertice] = u;
            }
        }
    }
    free(visitado);
    return dist;
}

// ----------- BELLMAN FORD ---------------------------
int *bellman_ford(GRAFO *g, int s, int *p) {
    int *dist = (int *)malloc(g->vertices * sizeof(int));
    for (int i = 0; i < g->vertices; i++) {
        dist[i] = INT_MAX;
        p[i] = -1;
    }
    dist[s] = 0;

    for (int i = 1; i < g->vertices; i++) {
        for (int u = 0; u < g->vertices; u++) {
            for (ADJACENCIA *adj = g->adj[u].cab; adj != NULL; adj = adj->prox) {
                if (dist[u] != INT_MAX && dist[u] + adj->peso < dist[adj->vertice]) {
                    dist[adj->vertice] = dist[u] + adj->peso;
                    p[adj->vertice] = u;
                }
            }
        }
    }
    return dist;
}

void caminho(GRAFO *g, int origem, int destino, int *predecessores, FILE *saida) {
    if (destino == origem) {
        fprintf(saida, "v%d ", origem);
    } else if (predecessores[destino] == -1) {
        fprintf(saida, "Sem caminho de v%d para v%d\n", origem, destino);
    } else {
        caminho(g, origem, predecessores[destino], predecessores, saida);
        fprintf(saida, "v%d ", destino);
    }
}

int main() {
    char nomearquivo[] = "teste.gr";
    GRAFO *gr;
    FILE *arq;
    FILE *saida;

    char entrada[50], c;
    int qtdnos, arcs, n1, n2, peso;
    int *p;

    arq = fopen(nomearquivo, "r");
    if (arq == NULL) {
        puts("Erro ao abrir o arquivo");
        return 1;
    }

    while (fscanf(arq, "%c", &c) != EOF) {
        if (c == 'p') {
            fscanf(arq, " sp %d %d", &qtdnos, &arcs);
            gr = criarGrafo(qtdnos + 1);
            p = (int *)malloc(gr->vertices * sizeof(int));
        } else if (c == 'a') {
            fscanf(arq, " %d %d %d", &n1, &n2, &peso);
            criaAresta(gr, n1, n2, peso);
        }
    }

    fclose(arq);
    puts("Arestas carregadas.");

    imprime(gr);

    saida = fopen("resultado.txt", "w");
    if (saida == NULL) {
        printf("Erro ao abrir o arquivo de saída\n");
        return 1;
    }

    fprintf(saida, "Resultados Dijkstra:\n");
    int find = 1; 
    clock_t start = clock();
    int *distDijkstra = dijkstra(gr, find, p);
    clock_t end = clock();

    fprintf(saida, "Origem: %d\n", find);
    fprintf(saida, "Distâncias:\n");
    for (int i = START; i < gr->vertices; i++) {
        fprintf(saida, "v%d -> v%d: %d\n", find, i, distDijkstra[i]);
    }

    fprintf(saida, "\nCaminhos:\n");
    for (int i = START; i < gr->vertices; i++) {
        fprintf(saida, "v%d -> v%d: ", find, i);
        caminho(gr, find, i, p, saida);
        fprintf(saida, "\n");
    }

    fprintf(saida, "Tempo de execução Dijkstra: %f segundos\n", (double)(end - start) / CLOCKS_PER_SEC);
    free(distDijkstra);

    fprintf(saida, "\nResultados Bellman-Ford:\n");
    start = clock();
    int *distBellmanFord = bellman_ford(gr, find, p);
    end = clock();

    fprintf(saida, "Origem: %d\n", find);
    fprintf(saida, "Distâncias:\n");
    for (int i = START; i < gr->vertices; i++) {
        fprintf(saida, "v%d -> v%d: %d\n", find, i, distBellmanFord[i]);
    }

    fprintf(saida, "\nCaminhos:\n");
    for (int i = START; i < gr->vertices; i++) {
        fprintf(saida, "v%d -> v%d: ", find, i);
        caminho(gr, find, i, p, saida);
        fprintf(saida, "\n");
    }

    fprintf(saida, "Tempo de execução Bellman-Ford: %f segundos\n", (double)(end - start) / CLOCKS_PER_SEC);
    free(distBellmanFord);
    
    free(p);
    for (int i = 0; i < gr->vertices; i++) {
        ADJACENCIA *adj = gr->adj[i].cab;
        while (adj) {
            ADJACENCIA *temp = adj;
            adj = adj->prox;
            free(temp);
        }
    }
    free(gr->adj);
    free(gr);

    fclose(saida);
    return 0;
}
