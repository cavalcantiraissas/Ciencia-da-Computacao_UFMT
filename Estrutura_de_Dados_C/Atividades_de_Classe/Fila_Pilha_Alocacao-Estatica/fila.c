#include "fila.h"
#include <stdio.h>

void insere_fila(tipo_fila *fl, int vl) {
    if (fl->cont < TAM) {
        fl->vet_fila[(fl->cont)++] = vl;
    } else {
        printf("[ATENCAO] Estrutura FILA estah cheia! Valor nao foi inserido.\n");
    }
}

int remove_fila(tipo_fila *fl) {
    int aux = -1;
    if (fl->cont > 0) {
        aux = fl->vet_fila[0];
        for (int i = 0; i < fl->cont - 1; i++) {
            fl->vet_fila[i] = fl->vet_fila[i + 1];
        }
        fl->cont--;
    } else {
        printf("[ATENCAO] Fila estah vazia. Nao ha valor a ser retornado.\n");
    }
    return aux;
}

void imprime_fila(tipo_fila fl) {
    if (fl.cont == 0) {
        printf("Fila vazia!\n");
    } else {
        printf("FILA: [ ");
        for (int i = 0; i < fl.cont; i++) {
            printf("%d ", fl.vet_fila[i]);
        }
        printf("]\tContador=%d\n", fl.cont);
    }
}
