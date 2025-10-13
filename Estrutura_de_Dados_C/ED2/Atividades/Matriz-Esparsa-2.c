#include <stdio.h>
#include <stdlib.h>

typedef struct tempNo {
    float valor;
    int coluna;
    struct tempNo* prox;
}NO;

typedef NO* PONT;

typedef struct {
    PONT* A;
    int linhas;
    int colunas;
} MATRIZ;

MATRIZ *inicializarMatriz(int lin, int col){
    int i;
    
    MATRIZ *novo;
    
    novo = (MATRIZ *) malloc (sizeof(MATRIZ));
    
    novo->A = (PONT *) malloc(lin*sizeof(PONT));
    novo->linhas = lin;
    novo->colunas = col;
    for(i=0;i<lin;i++)
        novo->A[i] = NULL;
    
    return novo;
}

void imprimeMatriz(MATRIZ* m){
    for(int l=0; l < m->linhas; l++){
        if(m->A[l] != NULL){
            PONT c = m->A[l];
            while( c != NULL){
                printf("  [%f|(%d,%d)]",c->valor,l,c->coluna);
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
        return 0.0; // Valor inválido, pode ser tratado de outra forma se necessário

    PONT atual = m->A[lin];

    while (atual != NULL && col != atual->coluna)
        atual = atual->prox;

    if (atual != NULL)
        return atual->valor;
    else
        return 0.0; // Valor inválido, pode ser tratado de outra forma se necessário
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


MATRIZ* multEscalarMatriz(MATRIZ* m, int x) {
    MATRIZ* resultado = inicializarMatriz(m->linhas, m->colunas); // Inicializa a matriz resultado

    // Percorre a matriz original
    for (int l = 0; l < m->linhas; l++) {
        if (m->A[l] != NULL) {
            PONT c = m->A[l];
            while (c != NULL) {
                // Multiplica o valor pelo escalar x e insere na matriz resultado
                float novo_valor = c->valor * x;
                insereMatriz(resultado, l, c->coluna, novo_valor);
                c = c->prox;
            }
        }
    }

    return resultado;
}

// A função main não deve ser alterada quando for enviar
int main(){
    
    MATRIZ *m, *n;
    int col,lin,qtvalores;
    int colVal,linVal,x;
    float val;
    scanf("%d",&qtvalores);
    scanf("%d",&lin);
    scanf("%d",&col);

    m = inicializarMatriz(lin,col);

    for(int i=0; i<qtvalores; i++){
        scanf("%d",&linVal);
        scanf("%d",&colVal);
        scanf("%f",&val);
        insereMatriz(m,linVal,colVal,val);
    }
    
    scanf("%d",&x);
    n = multEscalarMatriz(m, x);
    imprimeMatriz(n);


    return 0;
}
