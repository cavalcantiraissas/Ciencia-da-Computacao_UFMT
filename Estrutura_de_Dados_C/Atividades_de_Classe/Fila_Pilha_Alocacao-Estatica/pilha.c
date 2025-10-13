#include "pilha.h"
#include <stdio.h>

void empilha(tipo_pilha *pl, int vl) {
    if (pl->cont < TAM) {
        pl->vet_pilha[pl->cont++] = vl;
    } else {
        printf("[ATENCAO] Pilha cheia. Valor NAO foi empilhado!\n");
    }
}

int desempilha(tipo_pilha *pl) {
    if (pl->cont > 0) {
        return pl->vet_pilha[--pl->cont];
    } else {
        printf("[ATENCAO] Pilha vazia. NAO ha valor a ser desempilhado!\n");
        return -1;
    }
}

void imprime_pilha(tipo_pilha pl) {
    printf("PILHA:\n    ---\nBase[ ");
    for (int i = 0; i < pl.cont; i++) {
        printf("%d ", pl.vet_pilha[i]);
    }
    printf("\t<--TOPO\n    ---\n");
}
