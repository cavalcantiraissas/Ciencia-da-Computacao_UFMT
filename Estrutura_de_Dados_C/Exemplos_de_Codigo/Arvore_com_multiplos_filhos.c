/**
 * UFMT - Ciencia da Computacao
 * Estrutura de Dados 1 - Prof. Ivairton
 *
 * Implementacao da estrutura Arvore Multiplos Filhos
 *
 */

#include <stdio.h>
#include <stdlib.h>

// Definicao de Constantes
#define T 1 //T irah impactar na ordem da arvore, de modo a obedecer a relação Ordem=T*2+1 e Filhos=T*2+2
#define K (T*2+1)

//Definicao de tipos
struct est_arvM {
    int valores[K];
    struct est_arvM *filhos[K+1];
    int qtd;
};
typedef struct est_arvM tipoArvM;

//Prototipos das funcoes
tipoArvM *alocaNovoNoh();
int nohCheio(tipoArvM*);
void insereValorNoh(tipoArvM*, int);
void insereArvM (tipoArvM**, int);
void impressaoPreOrdemArvM(tipoArvM*);
void impressaoOrdemArvM(tipoArvM*);
void impressaoPosOrdemArvM(tipoArvM*);
//TODO:
//int buscaValor(tipoArvM*, int);
//int numValores(tipoArvM*);
//int numNohs(tipoArvM*);
//int alturaArvM(tipoArvM*);
//int numNohsPorNivel(tipoArvM*, int, int);
//int ehFolha(tipoArvM*);
//int maior(tipoArvM*);
//int menor(tipoArvM*);
//void removeArvM(tipoArvM**, int);
//Funcao que retorna todos os valores de um certo nível


/** Funcao principal **/
int main(int argc, char *argv[]) {
    tipoArvM *arv;
    arv = NULL;

    insereArvM(&arv, 30);
    insereArvM(&arv, 20);
    insereArvM(&arv, 10);

    insereArvM(&arv, 5);
    insereArvM(&arv, 7);

    insereArvM(&arv, 23);
    insereArvM(&arv, 26);
    insereArvM(&arv, 29);

    insereArvM(&arv,27);
    insereArvM(&arv,28);

    impressaoPreOrdemArvM(arv); printf("\n");
    impressaoOrdemArvM(arv); printf("\n");
    impressaoPosOrdemArvM(arv); printf("\n");

    return 1;
}

//Aloca um novo noh na memoria e retorna o endereco
tipoArvM *alocaNovoNoh() {
    int i;
    tipoArvM *novoNoh;
    novoNoh = (tipoArvM*) malloc(sizeof(tipoArvM));
    novoNoh->qtd = 0;
    for (i=0; i<K; i++) {
        novoNoh->valores[i] = 0;
        novoNoh->filhos[i] = NULL;
    }
    novoNoh->filhos[i] = NULL; //Atualiza o último filho
    return novoNoh;
}

// Verifica se um noh esta cheio.
// Retorna verdadeiro se cheio, ou falso, caso contrario
int nohCheio(tipoArvM* arv) {
    if (arv != NULL) {
        if (arv->qtd < K)
            return 0; //Ha espaco
    }
    return 1; //NAO ha espaco, nao pode ser inserido.
}

//Executa insercao ordenada em um noh com esapaco disponivel
void insereValorNoh(tipoArvM* no, int vl) {
    if (no != NULL) {
        if (no->qtd < K) {
            int i;
            i = no->qtd-1;
            while ( (i >= 0) && (no->valores[i] > vl) ) {
                no->valores[i+1] = no->valores[i];
                i--;
            }
            no->valores[i+1] = vl;
            no->qtd++;
        }
    }
}

//Insere um novo valor na arvore
void insereArvM (tipoArvM** arv, int vl) {
    //Se arvore vazia, aloca um no o noh e coloca o valor
    if ( (*arv) == NULL) {
        (*arv) = alocaNovoNoh();
        insereValorNoh( (*arv), vl);
    } else {
        //Verifica se ha espaco
        if ((*arv)->qtd < K) {
            insereValorNoh( (*arv), vl);
        } else {
            //Identifica o filho onde deve ocorrer a chamada recursiva
            int i;
            i = 0;
            while ( (i < K) && (vl > (*arv)->valores[i]) )
                i++;
            insereArvM(&((*arv)->filhos[i]), vl);
        }
    }
}

// Imprime a arvore usando o metodo de pre Ordem
void impressaoPreOrdemArvM(tipoArvM* arv) {
    int i;
    if (arv != NULL) {
        //Prioriza impressao dos valores
        for (i=0; i<arv->qtd; i++)
            printf("[%d]",arv->valores[i]);
        //Executa chamada recursiva
        for (i=0; i<arv->qtd+1; i++)
            impressaoPreOrdemArvM(arv->filhos[i]);
    }
}

// IMpressao em Ordem
void impressaoOrdemArvM(tipoArvM* arv) {
    int i;
    if (arv != NULL) {
        for (i=0; i < arv->qtd; i++) {
            impressaoOrdemArvM(arv->filhos[i]);
            printf("[%d]",arv->valores[i]);
        }
        impressaoOrdemArvM(arv->filhos[i]);
    }
}

// Impressao em pos Ordem
void impressaoPosOrdemArvM(tipoArvM* arv) {
    int i;
    if (arv != NULL) {
        //Prioriza impressao dos filhos
        for (i=0; i<arv->qtd+1; i++)
            impressaoPosOrdemArvM(arv->filhos[i]);
        //Imprime valores
        for (i=0; i < arv->qtd; i++)
            printf("[%d]",arv->valores[i]);
    }
}
