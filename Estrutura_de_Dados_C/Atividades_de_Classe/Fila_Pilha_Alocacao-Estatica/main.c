#include <stdio.h>
#include "fila.h"
#include "pilha.h"

int main() {
    tipo_fila minha_fila;
    tipo_pilha minha_pilha;
    minha_fila.cont = 0;
    minha_pilha.cont = 0;
    int valor;

    printf("Informe 10 números inteiros:\n");
    for (int i = 0; i < TAM; i++) {
        printf("%d: ", i + 1);
        scanf("%d", &valor);
        insere_fila(&minha_fila, valor);
    }

    printf("\nTransferindo da fila para a pilha...\n");
    while (minha_fila.cont > 0) {
        valor = remove_fila(&minha_fila);
        empilha(&minha_pilha, valor);
    }

    printf("\nValores em ordem inversa:\n");
    while (minha_pilha.cont > 0) {
        printf("%d ", desempilha(&minha_pilha));
    }
    printf("\n");

    return 0;
}
