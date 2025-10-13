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
    
    MATRIZ *novo;
    
    novo = (MATRIZ *) malloc (sizeof(MATRIZ));
    
    novo->A = (PONT *) malloc(lin*sizeof(PONT));
    novo->linhas = lin;
    novo->colunas = col;
    for(i = 0; i < lin; i++)
        novo->A[i] = NULL;
    
    return novo;
}

void imprimeMatriz(MATRIZ* m) {
    for(int l = 0; l < m->linhas; l++) {
        if(m->A[l] != NULL) {
            PONT c = m->A[l];
            while(c != NULL) {
                printf("  [%f|(%d,%d)]",c->valor,l,c->coluna);
                c = c->prox;
            }
            printf("\n");
        }
    }
}

// função que insere um valor na matriz
int insereMatriz(MATRIZ* m, int lin, int col, float val) {
    if (lin < 0 || lin >= m->linhas || col < 0 || col >= m->colunas)
        return 0;

    PONT ant = NULL, atual = m->A[lin];
    while (atual != NULL && atual->coluna < col) {
        ant = atual;
        atual = atual->prox;
    }

    if (atual != NULL && atual->coluna == col) {
        // Atualiza o valor se o elemento já existe
        atual->valor = val;
    } else {
        // Insere um novo elemento
        PONT novo = (PONT) malloc(sizeof(NO));
        novo->valor = val;
        novo->coluna = col;
        novo->prox = atual;

        if (ant == NULL) {
            m->A[lin] = novo;
        } else {
            ant->prox = novo;
        }
    }

    return 1;
}

// função que retorna o valor
float ValorMatriz(MATRIZ* m, int lin, int col) {
    if (lin < 0 || lin >= m->linhas || col < 0 || col >= m->colunas)
        return 0.0; // valor padrão para elementos inexistentes

    PONT atual = m->A[lin];
    while (atual != NULL) {
        if (atual->coluna == col) {
            return atual->valor;
        }
        atual = atual->prox;
    }
    return 0.0; // valor padrão para elementos inexistentes
}

// função que exclui um valor da matriz
int excluirMatriz(MATRIZ *m, int lin, int col) {
    if (lin < 0 || lin >= m->linhas || col < 0 || col >= m->colunas)
        return 0;

    PONT ant = NULL, atual = m->A[lin];
    while (atual != NULL && atual->coluna < col) {
        ant = atual;
        atual = atual->prox;
    }

    if (atual != NULL && atual->coluna == col) {
        if (ant == NULL) {
            m->A[lin] = atual->prox;
        } else {
            ant->prox = atual->prox;
        }
        free(atual);
        return 1;
    }

    return 0;
}

int main() {
    MATRIZ *m;
    int col, lin, qtvalores;
    int colVal, linVal;
    float val;
    scanf("%d", &qtvalores);
    scanf("%d", &lin);
    scanf("%d", &col);

    m = inicializarMatriz(lin, col);

    for(int i = 0; i < qtvalores; i++) {
        scanf("%d", &linVal);
        scanf("%d", &colVal);
        scanf("%f", &val);
        insereMatriz(m, linVal, colVal, val);
    }
    
    imprimeMatriz(m);
    
    scanf("%d", &linVal);
    scanf("%d", &colVal);
    excluirMatriz(m, linVal, colVal);
    imprimeMatriz(m);

    return 0;
}
