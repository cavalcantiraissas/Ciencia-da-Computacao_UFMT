#include <stdio.h>
#include <stdlib.h>

typedef struct tempNo {
    float valor;
    int coluna;
    struct tempNo* prox;
} NO;

typedef NO* PONT;

typedef struct {
    PONT* A;
    int linhas;
    int colunas;
} MATRIZ;

MATRIZ *inicializarMatriz(int lin, int col) {
    int i;
    MATRIZ *novo = (MATRIZ *) malloc(sizeof(MATRIZ));
    novo->A = (PONT *) malloc(lin * sizeof(PONT));
    novo->linhas = lin;
    novo->colunas = col;
    for (i = 0; i < lin; i++)
        novo->A[i] = NULL;
    return novo;
}

void imprimeMatriz(MATRIZ* m) {
    for (int l = 0; l < m->linhas; l++) {
        if (m->A[l] != NULL) {
            PONT c = m->A[l];
            while (c != NULL) {
                printf("  [%f|(%d,%d)]", c->valor, l, c->coluna);
                c = c->prox;
            }
            printf("\n");
        }
    }
}

PONT criaNo(int col, float val) {
    PONT novo = (PONT)malloc(sizeof(NO));
    novo->coluna = col;
    novo->valor = val;
    novo->prox = NULL;
    return novo;
}

int insereMatriz(MATRIZ* m, int lin, int col, float val) {
    if (lin < 0 || lin >= m->linhas || col < 0 || col >= m->colunas) {
        printf("Posição fora dos limites da matriz.\n");
        return 0;
    }

    PONT novo = criaNo(col, val);

    if (m->A[lin] == NULL || col < m->A[lin]->coluna) {
        novo->prox = m->A[lin];
        m->A[lin] = novo;
    } else {
        PONT ant = NULL;
        PONT atual = m->A[lin];

        while (atual != NULL && col > atual->coluna) {
            ant = atual;
            atual = atual->prox;
        }

        if (atual != NULL && col == atual->coluna) {
            // Elemento já existe na matriz, atualize o valor
            atual->valor = val;
            free(novo);
        } else {
            ant->prox = novo;
            novo->prox = atual;
        }
    }

    return 1;
}

float ValorMatriz(MATRIZ* m, int lin, int col) {
    if (lin < 0 || lin >= m->linhas || col < 0 || col >= m->colunas)
        return 0.0;

    PONT atual = m->A[lin];

    while (atual != NULL && col != atual->coluna)
        atual = atual->prox;

    if (atual != NULL)
        return atual->valor;
    else
        return 0.0;
}

int excluirMatriz(MATRIZ* m, int lin, int col) {
    if (lin < 0 || lin >= m->linhas || col < 0 || col >= m->colunas) {
        printf("Posição fora dos limites da matriz.\n");
        return 0;
    }

    PONT ant = NULL;
    PONT atual = m->A[lin];

    while (atual != NULL && col != atual->coluna) {
        ant = atual;
        atual = atual->prox;
    }

    if (atual != NULL) {
        if (ant == NULL)
            m->A[lin] = atual->prox;
        else
            ant->prox = atual->prox;

        free(atual);
        return 1;
    } else {
        printf("Elemento não encontrado na matriz.\n");
        return 0;
    }
}

MATRIZ* soma2Matriz(MATRIZ* m, MATRIZ* n) {
    if (m->linhas != n->linhas || m->colunas != n->colunas) return NULL;

    MATRIZ* resultado = inicializarMatriz(m->linhas, m->colunas);

    for (int i = 0; i < m->linhas; i++) {
        PONT atualM = m->A[i];
        PONT atualN = n->A[i];

        while (atualM != NULL || atualN != NULL) {
            if (atualM != NULL && (atualN == NULL || atualM->coluna < atualN->coluna)) {
                insereMatriz(resultado, i, atualM->coluna, atualM->valor);
                atualM = atualM->prox;
            } else if (atualN != NULL && (atualM == NULL || atualN->coluna < atualM->coluna)) {
                insereMatriz(resultado, i, atualN->coluna, atualN->valor);
                atualN = atualN->prox;
            } else {
                float soma = atualM->valor + atualN->valor;
                if (soma != 0)
                    insereMatriz(resultado, i, atualM->coluna, soma);
                atualM = atualM->prox;
                atualN = atualN->prox;
            }
        }
    }

    return resultado;
}

float somatorioMatriz(MATRIZ* m) {
    float soma = 0.0;
    for (int lin = 0; lin < m->linhas; lin++) {
        PONT atual = m->A[lin];
        while (atual != NULL) {
            soma += atual->valor;
            atual = atual->prox;
        }
    }
    return soma;
}

// A função main não deve ser alterada quando for enviar
int main() {

    MATRIZ *m, *n, *sum;
    int col, lin, qtvalores;
    int colVal, linVal;
    float val, resultadoSoma;
    
    // Leitura da primeira matriz
    scanf("%d", &qtvalores);
    scanf("%d", &lin);
    scanf("%d", &col);

    m = inicializarMatriz(lin, col);

    for (int i = 0; i < qtvalores; i++) {
        scanf("%d", &linVal);
        scanf("%d", &colVal);
        scanf("%f", &val);
        insereMatriz(m, linVal, colVal, val);
    }

    // Leitura da segunda matriz
    scanf("%d", &qtvalores);
    scanf("%d", &lin);
    scanf("%d", &col);

    n = inicializarMatriz(lin, col);

    for (int i = 0; i < qtvalores; i++) {
        scanf("%d", &linVal);
        scanf("%d", &colVal);
        scanf("%f", &val);
        insereMatriz(n, linVal, colVal, val);
    }

    // Realiza a soma das duas matrizes
    sum = soma2Matriz(m, n);

    // Imprime a matriz resultante
    imprimeMatriz(sum);

    // Calcula e imprime o somatório dos valores da matriz resultante
    resultadoSoma = somatorioMatriz(sum);
    printf("Somatório: %f\n", resultadoSoma);

    return 0;
}
