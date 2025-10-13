#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>

#define true 1
#define false 0
#define START 1
typedef int bool;
typedef int TIPOPESO;

typedef struct adjacencia
{
    int vertice;
    TIPOPESO peso;
    struct adjacencia *prox;
} ADJACENCIA;

typedef struct vertice
{
    ADJACENCIA *cab;
} VERTICE;

typedef struct grafo
{
    int vertices;
    int arestas;
    VERTICE *adj;
} GRAFO;

GRAFO *criarGrafo(int v)
{
    GRAFO *g = (GRAFO *)malloc(sizeof(GRAFO));
    g->vertices = v;
    g->arestas = 0;
    g->adj = (VERTICE *)malloc(v * sizeof(VERTICE));

    for (int i = 0; i < v; i++)
        g->adj[i].cab = NULL;

    return g;
}

ADJACENCIA *criaAdj(int v, int peso)
{
    ADJACENCIA *temp = (ADJACENCIA *)malloc(sizeof(ADJACENCIA));
    temp->vertice = v;
    temp->peso = peso;
    temp->prox = NULL;
    return temp;
}

ADJACENCIA *inseriListAdj(ADJACENCIA *adj, int vi, int vf, TIPOPESO p)
{
    ADJACENCIA *novo = criaAdj(vf, p);
    if (!adj || adj->vertice > vf)
    {
        novo->prox = adj;
        return novo;
    }

    ADJACENCIA *ant = adj, *atual = adj->prox;
    while (atual && atual->vertice < vf)
    {
        ant = atual;
        atual = atual->prox;
    }

    novo->prox = atual;
    ant->prox = novo;
    return adj;
}

bool criaAresta(GRAFO *gr, int vi, int vf, TIPOPESO p)
{
    if (!gr || vi < 0 || vf < 0 || vi >= gr->vertices || vf >= gr->vertices)
        return false;

    gr->adj[vi].cab = inseriListAdj(gr->adj[vi].cab, vi, vf, p);
    gr->arestas++;
    return true;
}

void imprime(GRAFO *gr)
{
    printf("Vertices: %d. Arestas: %d\n", gr->vertices, gr->arestas);

    for (int i = START; i < gr->vertices; i++)
    {
        printf("v%d: ", i);
        ADJACENCIA *ad = gr->adj[i].cab;
        while (ad)
        {
            printf("v%d(%d) ", ad->vertice, ad->peso);
            ad = ad->prox;
        }
        printf("\n");
    }
}

// ---------- DIJKSTRA ----------
int *dijkstra(GRAFO *g, int s, int *p)
{
    int *dist = (int *)malloc(g->vertices * sizeof(int));
    bool *visitado = (bool *)malloc(g->vertices * sizeof(bool));

    for (int i = 0; i < g->vertices; i++)
    {
        dist[i] = INT_MAX;
        visitado[i] = false;
        p[i] = -1;
    }
    dist[s] = 0;

    for (int i = 0; i < g->vertices; i++)
    {
        int u = -1;
        for (int j = 0; j < g->vertices; j++)
        {
            if (!visitado[j] && (u == -1 || dist[j] < dist[u]))
                u = j;
        }

        if (dist[u] == INT_MAX)
            break;

        visitado[u] = true;

        ADJACENCIA *adj = g->adj[u].cab;
        while (adj)
        {
            int v = adj->vertice;
            int peso = adj->peso;

            if (dist[u] + peso < dist[v])
            {
                dist[v] = dist[u] + peso;
                p[v] = u;
            }
            adj = adj->prox;
        }
    }

    free(visitado);
    return dist;
}

void caminho(GRAFO *g, int from, int to, int *p, FILE *saida)
{
    if (to == from)
    {
        fprintf(saida, "%d", from);
        return;
    }
    else if (p[to] == -1)
    {
        fprintf(saida, "No path");
        return;
    }
    caminho(g, from, p[to], p, saida);
    fprintf(saida, " -> %d", to);
}

int main()
{
    char nomearquivo[] = "teste.gr";
    GRAFO *gr;
    FILE *arq, *saida;

    char entrada[50], c;
    int qtdnos, arcs, n1, n2, peso, j = 0;
    int *p;

    arq = fopen(nomearquivo, "r");
    saida = fopen("resultado.txt", "w");

    if (arq == NULL || saida == NULL)
    {
        puts("Erro ao abrir arquivo");
        return 0;
    }

    while (fscanf(arq, "%c", &c) != EOF)
    {
        if (c == 'c')
            fscanf(arq, "%[^\n]", entrada);
        else if (c == 'p')
        {
            fscanf(arq, " sp %d %d", &qtdnos, &arcs);
            gr = criarGrafo(qtdnos + 1);
            p = (int *)malloc(gr->vertices * sizeof(int));
        }
        else if (c == 'a')
        {
            fscanf(arq, " %d %d %d", &n1, &n2, &peso);
            criaAresta(gr, n1, n2, peso);
        }
    }

    fclose(arq);
    int vertice_origem = 1;
    int vertice1 = 4, vertice2 = 500, vertice3 = 1000;

    clock_t inicio, fim;
    double tempo_total;

    inicio = clock();
    int *dist = dijkstra(gr, vertice_origem, p);
    fim = clock();
    tempo_total = (double)(fim - inicio) / CLOCKS_PER_SEC;

    fprintf(saida, "Dijkstra - Vértice de origem: %d\n", vertice_origem);
    fprintf(saida, "Distância até v%d: %d\n", vertice1, dist[vertice1]);
    fprintf(saida, "Caminho: ");
    caminho(gr, vertice_origem, vertice1, p, saida);
    fprintf(saida, "\n");

    fprintf(saida, "Distância até v%d: %d\n", vertice2, dist[vertice2]);
    fprintf(saida, "Caminho: ");
    caminho(gr, vertice_origem, vertice2, p, saida);
    fprintf(saida, "\n");

    fprintf(saida, "Distância até v%d: %d\n", vertice3, dist[vertice3]);
    fprintf(saida, "Caminho: ");
    caminho(gr, vertice_origem, vertice3, p, saida);
    fprintf(saida, "\n");

    fprintf(saida, "Tempo de processamento: %lf segundos\n", tempo_total);

    free(dist);
    free(p);
    fclose(saida);

    return 0;
}
