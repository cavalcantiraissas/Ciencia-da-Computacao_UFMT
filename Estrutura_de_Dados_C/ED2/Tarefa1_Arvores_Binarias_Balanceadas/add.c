#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct NoABB {
    int dado;
    struct NoABB* esquerda;
    struct NoABB* direita;
} NoABB;

NoABB* criarNo(int dado) {
    NoABB* novoNo = (NoABB*)malloc(sizeof(NoABB));
    if (novoNo == NULL) {
        printf("Erro ao alocar memória para o novo nó.\n");
        exit(1);
    }
    novoNo->dado = dado;
    novoNo->esquerda = NULL;
    novoNo->direita = NULL;
    return novoNo;
}

NoABB* inserir(NoABB* raiz, int dado) {
    if (raiz == NULL) {
        return criarNo(dado);
    }
    if (dado < raiz->dado) {
        raiz->esquerda = inserir(raiz->esquerda, dado);
    } else {
        raiz->direita = inserir(raiz->direita, dado);
    }
    return raiz;
}

NoABB* buscar(NoABB* raiz, int dado, int* comparacoes) {
    (*comparacoes)++;
    if (raiz == NULL || raiz->dado == dado) {
        return raiz;
    }
    if (dado < raiz->dado) {
        return buscar(raiz->esquerda, dado, comparacoes);
    } else {
        return buscar(raiz->direita, dado, comparacoes);
    }
}

void percorrerEmOrdem(NoABB* raiz) {
    if (raiz != NULL) {
        percorrerEmOrdem(raiz->esquerda);
        printf("%d ", raiz->dado);
        percorrerEmOrdem(raiz->direita);
    }
}

int alturaABB(NoABB* raiz) {
    if (raiz == NULL) {
        return 0;
    }
    int alturaEsquerda = alturaABB(raiz->esquerda);
    int alturaDireita = alturaABB(raiz->direita);
    return (alturaEsquerda > alturaDireita ? alturaEsquerda : alturaDireita) + 1;
}

void buscar20PorCento(NoABB* raiz, int* valores, int numValores) {
    int n = numValores * 0.20;
    int totalComparacoes = 0;
    double totalTempo = 0.0;
    srand(time(NULL));

    for (int i = 0; i < n; i++) {
        int index = rand() % numValores;
        int valorParaBuscar = valores[index];
        int comparacoes = 0;
        clock_t inicio = clock();
        NoABB* resultado = buscar(raiz, valorParaBuscar, &comparacoes);
        clock_t fim = clock();
        double tempo = (double)(fim - inicio) / CLOCKS_PER_SEC;

        totalComparacoes += comparacoes;
        totalTempo += tempo;
    }

    printf("Total de tempo para busca dos valores: %f segundos\n", totalTempo);
    printf("Total de comparações: %d\n", totalComparacoes);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Uso: %s <nome_do_arquivo>\n", argv[0]);
        return 1;
    }

    NoABB* raiz = NULL;
    int numValores = 5000; 
    int* valores = (int*)malloc(numValores * sizeof(int));
    if (valores == NULL) {
        printf("Erro ao alocar memória para os valores.\n");
        return 1;
    }

    FILE* arquivo = fopen(argv[1], "r");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo %s\n", argv[1]);
        free(valores);
        return 1;
    }

    for (int i = 0; i < numValores; i++) {
        if (fscanf(arquivo, "%d", &valores[i]) != 1) {
            printf("Erro ao ler valor do arquivo\n");
            fclose(arquivo);
            free(valores);
            return 1;
        }
        raiz = inserir(raiz, valores[i]);
    }
    fclose(arquivo);

    printf("Árvore Binária de Busca em ordem: ");
    percorrerEmOrdem(raiz);
    printf("\n");

    int alturaArvoreABB = alturaABB(raiz);
    printf("Altura da Árvore Binária de Busca: %d\n", alturaArvoreABB);

    buscar20PorCento(raiz, valores, numValores);

    free(valores);
    return 0;
}
