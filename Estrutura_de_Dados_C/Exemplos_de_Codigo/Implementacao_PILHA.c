/**
 * UFMT - Bacharelado em Ciencia da Computacao
 * Prof. Ivairton
 *
 * Implementacao de PILHA.
 */
#include<stdio.h>
#include<stdlib.h>

// Definicao de constantes
#define TAM 10

// Definicao de tipos/registros
struct est_pilha {
    int pilha[TAM];
    int qtd_elem;
};
typedef struct est_pilha tipo_pilha;

// Prototipo de funcoes
void empilha(tipo_pilha *pl, int vl);
int desempilha(tipo_pilha *pl);
int topo(tipo_pilha *pl);
void imprimePilha(tipo_pilha *pl); //Para debug

// Funcao principal
int main(int argc, char *argv[]) {
    tipo_pilha minha_pilha;
    minha_pilha.qtd_elem = 0;

    empilha(&minha_pilha, 100);
    empilha(&minha_pilha, 200);
    empilha(&minha_pilha, 300);

    imprimePilha(&minha_pilha);

    printf("Valor no topo da pilha (nao remove) = %d\n\n", topo(&minha_pilha));

    printf("Valor desempilhado = %d\n", desempilha(&minha_pilha));
    printf("Valor desempilhado = %d\n", desempilha(&minha_pilha));

    imprimePilha(&minha_pilha);

    return 0;
}

/**
 * Funcao que empilha um novo valor da pilha
 */
void empilha(tipo_pilha *pl, int vl) {
    //Verifica se pilha tem espaco disponivel
    if (pl->qtd_elem < TAM) {
        pl->pilha[pl->qtd_elem++] = vl;
    } else {
        printf("[ERRO] Pilha estah cheia!\nNao foi possivel empilhar novo valor.\n");
    }
}

/**
 * Funcao que remove (desempilha) o valor do topo da pilha
 */
int desempilha(tipo_pilha *pl) {
    //verifica se pilha nao estah vazia
    if (pl->qtd_elem > 0) {
        return pl->pilha[--pl->qtd_elem];
    } else {
        printf("[ERRO] Pilha estah vazia.\nNao eh possivel desempilhar um valor.\n");
    }
}

/**
 * Funcao que retorna o valor do topo, sem remove-lo
 */
int topo(tipo_pilha *pl) {
    return pl->pilha[pl->qtd_elem-1];
}

/**
 * Funcao para DEBUG, que imprime a pilha
 */
void imprimePilha(tipo_pilha *pl) {
    int i;
    printf("Estrutura da pilha:\nQtd_elem: %d\nPilha:\ntopo ->", pl->qtd_elem);
    for ( i = (pl->qtd_elem-1);  i >= 0; i--) {
        printf("\t%d\n", pl->pilha[i]);
    }

}
