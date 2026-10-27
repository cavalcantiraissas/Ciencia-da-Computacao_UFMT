/*
 * Valor máximo de um vetor com OpenMP.
 * Cada thread calcula seu máximo local e, só no final, atualiza
 * max_global dentro de uma região critical.
 *
 * Compilar: gcc -fopenmp -O2 MaxVetor.c -o max_vetor
 * Executar: ./max_vetor [N] [threads]
 */
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <omp.h>

int main(int argc, char *argv[]) {
    long n = (argc > 1) ? atol(argv[1]) : 10000000L;
    int nthreads = (argc > 2) ? atoi(argv[2]) : omp_get_max_threads();
    if (n <= 0 || nthreads <= 0) {
        fprintf(stderr, "Uso: %s [N > 0] [threads > 0]\n", argv[0]);
        return 1;
    }

    int *v = malloc(n * sizeof(int));
    if (!v) { fprintf(stderr, "Sem memória\n"); return 1; }

    /* vetor desordenado, sequencial e com semente fixa (reprodutível) */
    srand(42);
    for (long i = 0; i < n; i++)
        v[i] = rand() % 1000000;

    /* versão explícita: máximo local por thread + critical no final */
    int max_global = INT_MIN;
    double t0 = omp_get_wtime();

    #pragma omp parallel num_threads(nthreads)
    {
        int max_local = INT_MIN;          /* privada: declarada dentro da região */

        #pragma omp for
        for (long i = 0; i < n; i++)
            if (v[i] > max_local)
                max_local = v[i];

        /* uma única atualização por thread, protegida */
        #pragma omp critical
        {
            if (max_local > max_global)
                max_global = max_local;
        }
    }
    double t_explicito = omp_get_wtime() - t0;

    /* conferência 1: versão sequencial */
    int max_seq = INT_MIN;
    for (long i = 0; i < n; i++)
        if (v[i] > max_seq) max_seq = v[i];

    /* comparação: redução automática */
    int max_red = INT_MIN;
    t0 = omp_get_wtime();
    #pragma omp parallel for reduction(max:max_red) num_threads(nthreads)
    for (long i = 0; i < n; i++)
        if (v[i] > max_red) max_red = v[i];
    double t_red = omp_get_wtime() - t0;

    printf("N = %ld, threads = %d\n", n, nthreads);
    printf("max_global (critical): %d  (%.4f s)\n", max_global, t_explicito);
    printf("max (reduction)      : %d  (%.4f s)\n", max_red, t_red);
    printf("max (sequencial)     : %d\n", max_seq);
    printf("Resultado %s\n",
           (max_global == max_seq && max_red == max_seq) ? "CORRETO" : "DIVERGENTE");

    free(v);
    return 0;
}