#include <stdio.h>
#include <stdlib.h>

void merge(int vetor[], int esquerda, int meio, int direita) {
    int tamanhoEsq = meio - esquerda + 1;
    int tamanhoDir = direita - meio;
    
    int *vetEsq = (int *)malloc(tamanhoEsq * sizeof(int));
    int *vetDir = (int *)malloc(tamanhoDir * sizeof(int));
    
    if (vetEsq == NULL || vetDir == NULL) {
        printf("Erro ao alocar memória.\n");
        exit(1);
    }
    
    for (int i = 0; i < tamanhoEsq; i++)
        vetEsq[i] = vetor[esquerda + i];
    for (int j = 0; j < tamanhoDir; j++)
        vetDir[j] = vetor[meio + 1 + j];
    
    int i = 0, j = 0, k = esquerda;
    
    while (i < tamanhoEsq && j < tamanhoDir) {
        if (vetEsq[i] <= vetDir[j]) {
            vetor[k] = vetEsq[i];
            i++;
        } else {
            vetor[k] = vetDir[j];
            j++;
        }
        k++;
    }
    
    while (i < tamanhoEsq) {
        vetor[k] = vetEsq[i];
        i++;
        k++;
    }
    
    while (j < tamanhoDir) {
        vetor[k] = vetDir[j];
        j++;
        k++;
    }
    
    free(vetEsq);
    free(vetDir);
}

void merge_sort(int vetor[], int esquerda, int direita) {
    if (esquerda < direita) {
        int meio = esquerda + (direita - esquerda) / 2;
        
        merge_sort(vetor, esquerda, meio);
        merge_sort(vetor, meio + 1, direita);
        merge(vetor, esquerda, meio, direita);
    }
}

void imprimir_vetor(int vetor[], int tamanho) {
    for (int i = 0; i < tamanho; i++) {
        printf("%d ", vetor[i]);
    }
    printf("\n");
}

int main() {
    int vetor[] = {38, 27, 43, 3, 9, 82, 10};
    int tamanho = sizeof(vetor) / sizeof(vetor[0]);
    
    printf("Vetor original: ");
    imprimir_vetor(vetor, tamanho);
    
    merge_sort(vetor, 0, tamanho - 1);
    
    printf("Vetor ordenado: ");
    imprimir_vetor(vetor, tamanho);
    
    return 0;
}
