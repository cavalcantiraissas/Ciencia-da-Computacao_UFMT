#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct NoAVL {
    int dado;
    struct NoAVL* esquerda;
    struct NoAVL* direita;
    int altura;
} NoAVL;

NoAVL* criarNoAVL(int dado) {
    NoAVL* novoNo = (NoAVL*)malloc(sizeof(NoAVL));
    if (novoNo == NULL) {
        printf("Erro ao alocar memória para o novo nó.\n");
        exit(1);
    }
    novoNo->dado = dado;
    novoNo->esquerda = NULL;
    novoNo->direita = NULL;
    novoNo->altura = 1;
    return novoNo;
}

int obterAltura(NoAVL* no) {
    if (no == NULL) {
        return 0;
    }
    return no->altura;
}

int obterBalanco(NoAVL* no) {
    if (no == NULL) {
        return 0;
    }
    return obterAltura(no->esquerda) - obterAltura(no->direita);
}

NoAVL* rotacionarDireita(NoAVL* y) {
    NoAVL* x = y->esquerda;
    NoAVL* T2 = x->direita;

    x->direita = y;
    y->esquerda = T2;

    y->altura = (obterAltura(y->esquerda) > obterAltura(y->direita) ? obterAltura(y->esquerda) : obterAltura(y->direita)) + 1;
    x->altura = (obterAltura(x->esquerda) > obterAltura(x->direita) ? obterAltura(x->esquerda) : obterAltura(x->direita)) + 1;

    return x;
}

NoAVL* rotacionarEsquerda(NoAVL* x) {
    NoAVL* y = x->direita;
    NoAVL* T2 = y->esquerda;

    y->esquerda = x;
    x->direita = T2;

    x->altura = (obterAltura(x->esquerda) > obterAltura(x->direita) ? obterAltura(x->esquerda) : obterAltura(x->direita)) + 1;
    y->altura = (obterAltura(y->esquerda) > obterAltura(y->direita) ? obterAltura(y->esquerda) : obterAltura(y->direita)) + 1;

    return y;
}

NoAVL* inserir(NoAVL* no, int dado, int* numRotacoes) {
    if (no == NULL) {
        return criarNoAVL(dado);
    }
    if (dado < no->dado) {
        no->esquerda = inserir(no->esquerda, dado, numRotacoes);
    } else if (dado > no->dado) {
        no->direita = inserir(no->direita, dado, numRotacoes);
    } else {
        return no;
    }

    no->altura = (obterAltura(no->esquerda) > obterAltura(no->direita) ? obterAltura(no->esquerda) : obterAltura(no->direita)) + 1;

    int balanco = obterBalanco(no);

    if (balanco > 1 && dado < no->esquerda->dado) {
        (*numRotacoes)++;
        return rotacionarDireita(no);
    }

    if (balanco < -1 && dado > no->direita->dado) {
        (*numRotacoes)++;
        return rotacionarEsquerda(no);
    }

    if (balanco > 1 && dado > no->esquerda->dado) {
        no->esquerda = rotacionarEsquerda(no->esquerda);
        (*numRotacoes)++;
        return rotacionarDireita(no);
    }

    if (balanco < -1 && dado < no->direita->dado) {
        no->direita = rotacionarDireita(no->direita);
        (*numRotacoes)++;
        return rotacionarEsquerda(no);
    }

    return no;
}

NoAVL* buscar(NoAVL* raiz, int dado, int* comparacoes) {
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

void percorrerEmOrdem(NoAVL* raiz) {
    if (raiz != NULL) {
        percorrerEmOrdem(raiz->esquerda);
        printf("%d ", raiz->dado);
        percorrerEmOrdem(raiz->direita);
    }
}

int alturaAVL(NoAVL* raiz) {
    if (raiz == NULL) {
        return 0;
    }
    return raiz->altura;
}

void buscar20PorCento(NoAVL* raiz, int* valores, int numValores) {
    int n = numValores * 0.20;
    int totalComparacoes = 0;
    double totalTempo = 0.0;
    srand(time(NULL));

    for (int i = 0; i < n; i++) {
        int index = rand() % numValores;
        int valorParaBuscar = valores[index];
        int comparacoes = 0;
        clock_t inicio = clock();
        NoAVL* resultado = buscar(raiz, valorParaBuscar, &comparacoes);
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

    NoAVL* raiz = NULL;
    int numValores = 5000; // Ajuste conforme necessário
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

    int numRotacoes = 0;
    for (int i = 0; i < numValores; i++) {
        if (fscanf(arquivo, "%d", &valores[i]) != 1) {
            printf("Erro ao ler valor do arquivo\n");
            fclose(arquivo);
            free(valores);
            return 1;
        }
        raiz = inserir(raiz, valores[i], &numRotacoes);
    }
    fclose(arquivo);

    printf("Árvore AVL em ordem: ");
    percorrerEmOrdem(raiz);
    printf("\n");

    int alturaArvoreAVL = alturaAVL(raiz);
    printf("Altura da Árvore AVL: %d\n", alturaArvoreAVL);
    printf("Número total de rotações: %d\n", numRotacoes);

    buscar20PorCento(raiz, valores, numValores);

    free(valores);
    return 0;
}
