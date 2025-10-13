#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef enum { VERMELHO, PRETO } Cor;

typedef struct NoARN {
    int dado;
    struct NoARN* esquerda;
    struct NoARN* direita;
    struct NoARN* pai;
    Cor cor;
} NoARN;

NoARN* criarNoARN(int dado) {
    NoARN* novoNo = (NoARN*)malloc(sizeof(NoARN));
    if (novoNo == NULL) {
        printf("Erro ao alocar memória para o novo nó.\n");
        exit(1);
    }
    novoNo->dado = dado;
    novoNo->esquerda = NULL;
    novoNo->direita = NULL;
    novoNo->pai = NULL;
    novoNo->cor = VERMELHO;
    return novoNo;
}

NoARN* rotacionarEsquerda(NoARN* raiz, NoARN* x) {
    NoARN* y = x->direita;
    x->direita = y->esquerda;
    if (y->esquerda != NULL) {
        y->esquerda->pai = x;
    }
    y->pai = x->pai;
    if (x->pai == NULL) {
        raiz = y;
    } else if (x == x->pai->esquerda) {
        x->pai->esquerda = y;
    } else {
        x->pai->direita = y;
    }
    y->esquerda = x;
    x->pai = y;
    return raiz;
}

NoARN* rotacionarDireita(NoARN* raiz, NoARN* y) {
    NoARN* x = y->esquerda;
    y->esquerda = x->direita;
    if (x->direita != NULL) {
        x->direita->pai = y;
    }
    x->pai = y->pai;
    if (y->pai == NULL) {
        raiz = x;
    } else if (y == y->pai->direita) {
        y->pai->direita = x;
    } else {
        y->pai->esquerda = x;
    }
    x->direita = y;
    y->pai = x;
    return raiz;
}

NoARN* balancearInsercao(NoARN* raiz, NoARN* k, int* numRotacoes) {
    NoARN* tio;
    while (k->pai != NULL && k->pai->cor == VERMELHO) {
        if (k->pai == k->pai->pai->esquerda) {
            tio = k->pai->pai->direita;
            if (tio != NULL && tio->cor == VERMELHO) {
                k->pai->cor = PRETO;
                tio->cor = PRETO;
                k->pai->pai->cor = VERMELHO;
                k = k->pai->pai;
            } else {
                if (k == k->pai->direita) {
                    k = k->pai;
                    raiz = rotacionarEsquerda(raiz, k);
                    (*numRotacoes)++;
                }
                k->pai->cor = PRETO;
                k->pai->pai->cor = VERMELHO;
                raiz = rotacionarDireita(raiz, k->pai->pai);
                (*numRotacoes)++;
            }
        } else {
            tio = k->pai->pai->esquerda;
            if (tio != NULL && tio->cor == VERMELHO) {
                k->pai->cor = PRETO;
                tio->cor = PRETO;
                k->pai->pai->cor = VERMELHO;
                k = k->pai->pai;
            } else {
                if (k == k->pai->esquerda) {
                    k = k->pai;
                    raiz = rotacionarDireita(raiz, k);
                    (*numRotacoes)++;
                }
                k->pai->cor = PRETO;
                k->pai->pai->cor = VERMELHO;
                raiz = rotacionarEsquerda(raiz, k->pai->pai);
                (*numRotacoes)++;
            }
        }
    }
    raiz->cor = PRETO;
    return raiz;
}

NoARN* inserirARN(NoARN* raiz, NoARN* novoNo, int* numRotacoes) {
    NoARN* y = NULL;
    NoARN* x = raiz;
    while (x != NULL) {
        y = x;
        if (novoNo->dado < x->dado) {
            x = x->esquerda;
        } else {
            x = x->direita;
        }
    }
    novoNo->pai = y;
    if (y == NULL) {
        raiz = novoNo;
    } else if (novoNo->dado < y->dado) {
        y->esquerda = novoNo;
    } else {
        y->direita = novoNo;
    }
    raiz = balancearInsercao(raiz, novoNo, numRotacoes);
    return raiz;
}

void percorrerEmOrdemARN(NoARN* raiz) {
    if (raiz != NULL) {
        percorrerEmOrdemARN(raiz->esquerda);
        printf("%d ", raiz->dado);
        percorrerEmOrdemARN(raiz->direita);
    }
}

NoARN* buscarARN(NoARN* raiz, int dado, int* comparacoes) {
    (*comparacoes)++;
    if (raiz == NULL || raiz->dado == dado) {
        return raiz;
    }
    if (dado < raiz->dado) {
        return buscarARN(raiz->esquerda, dado, comparacoes);
    } else {
        return buscarARN(raiz->direita, dado, comparacoes);
    }
}

int alturaARN(NoARN* raiz) {
    if (raiz == NULL) {
        return 0;
    }
    int alturaEsquerda = alturaARN(raiz->esquerda);
    int alturaDireita = alturaARN(raiz->direita);
    return (alturaEsquerda > alturaDireita ? alturaEsquerda : alturaDireita) + 1;
}

void buscar20PorCentoARN(NoARN* raiz, int* valores, int numValores) {
    int n = numValores * 0.20;
    int totalComparacoes = 0;
    double totalTempo = 0.0;
    srand(time(NULL));

    for (int i = 0; i < n; i++) {
        int index = rand() % numValores;
        int valorParaBuscar = valores[index];
        int comparacoes = 0;
        clock_t inicio = clock();
        NoARN* resultado = buscarARN(raiz, valorParaBuscar, &comparacoes);
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

    NoARN* raiz = NULL;
    int numValores = 5000; // Ajuste conforme necessário
    int* valores = (int*)malloc(numValores * sizeof(int));
    if (valores == NULL) {
        printf("Erro ao alocar memória para os valores.\n");
        return 1;
    }

    // Carregar os valores do arquivo
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
        NoARN* novoNo = criarNoARN(valores[i]);
        raiz = inserirARN(raiz, novoNo, &numRotacoes);
    }
    fclose(arquivo);

    printf("Árvore Rubro-Negra em ordem: ");
    percorrerEmOrdemARN(raiz);
    printf("\n");

    int alturaArvoreARN = alturaARN(raiz);
    printf("Altura da Árvore Rubro-Negra: %d\n", alturaArvoreARN);
    printf("Número total de rotações: %d\n", numRotacoes);

    buscar20PorCentoARN(raiz, valores, numValores);

    free(valores);
    return 0;
}
