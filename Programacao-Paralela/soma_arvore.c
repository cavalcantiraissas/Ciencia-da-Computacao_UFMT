/*
 * Soma dos nós de uma árvore binária de busca com OpenMP tasks.
 *
 * Compilar: gcc -fopenmp -O2 soma_arvore.c -o soma_arvore
 * Executar: ./soma_arvore [N] [threads]
 */
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

typedef struct No {
    int valor;
    struct No *esq, *dir;
} No;

/* inserção iterativa em BST (valores repetidos vão para a direita) */
No *inserir(No *raiz, int valor) {
    No *novo = malloc(sizeof(No));
    if (!novo) { fprintf(stderr, "Sem memória\n"); exit(1); }
    novo->valor = valor;
    novo->esq = novo->dir = NULL;
    if (!raiz) return novo;

    No *atual = raiz;
    for (;;) {
        if (valor < atual->valor) {
            if (!atual->esq) { atual->esq = novo; break; }
            atual = atual->esq;
        } else {
            if (!atual->dir) { atual->dir = novo; break; }
            atual = atual->dir;
        }
    }
    return raiz;
}

void liberar(No *raiz) {
    while (raiz) {                      /* recursão só na esquerda; direita em laço */
        liberar(raiz->esq);
        No *dir = raiz->dir;
        free(raiz);
        raiz = dir;
    }
}

/* versão sequencial, para conferência */
long soma_seq(const No *no) {
    if (!no) return 0;
    return no->valor + soma_seq(no->esq) + soma_seq(no->dir);
}

/* versão paralela com tarefas */
long soma_tasks(No *no) {
    if (!no) return 0;

    long soma_esq = 0, soma_dir = 0;

    /* soma_esq/soma_dir são locais desta chamada; sem shared, cada tarefa
       receberia uma CÓPIA (firstprivate) e o resultado se perderia. */
    #pragma omp task shared(soma_esq) firstprivate(no) default(none)
    soma_esq = soma_tasks(no->esq);

    #pragma omp task shared(soma_dir) firstprivate(no) default(none)
    soma_dir = soma_tasks(no->dir);

    /* espera as duas filhas antes de usar os resultados */
    #pragma omp taskwait

    return no->valor + soma_esq + soma_dir;
}

int main(int argc, char *argv[]) {
    long n = (argc > 1) ? atol(argv[1]) : 200000L;
    int nthreads = (argc > 2) ? atoi(argv[2]) : omp_get_max_threads();
    if (n <= 0 || nthreads <= 0) {
        fprintf(stderr, "Uso: %s [N > 0] [threads > 0]\n", argv[0]);
        return 1;
    }

    srand(42);
    No *raiz = NULL;
    for (long i = 0; i < n; i++)
        raiz = inserir(raiz, rand() % 1000);

    long esperado = soma_seq(raiz);
    long total = 0;

    double t0 = omp_get_wtime();
    #pragma omp parallel num_threads(nthreads) shared(raiz, total) default(none)
    {
        #pragma omp single
        total = soma_tasks(raiz);       /* só uma thread inicia na raiz */
    }
    double t = omp_get_wtime() - t0;

    printf("N = %ld nós, threads = %d\n", n, nthreads);
    printf("Soma (tasks)     : %ld  (%.4f s)\n", total, t);
    printf("Soma (sequencial): %ld\n", esperado);
    printf("Resultado %s\n", total == esperado ? "CORRETO" : "DIVERGENTE");

    liberar(raiz);
    return 0;
}
