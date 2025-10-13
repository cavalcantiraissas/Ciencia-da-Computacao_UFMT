#include "lista.h"

int main() {
    No *lista = NULL;

    // Inserções
    insereInicio(&lista, 10);
    insereInicio(&lista, 20);
    insereFim(&lista, 30);
    inserePos(&lista, 1, 25);

    printf("Lista após inserções:\n");
    imprimeLista(lista);

    // Remoções
    removeInicio(&lista);
    printf("Lista após remover do início:\n");
    imprimeLista(lista);

    removeFim(&lista);
    printf("Lista após remover do fim:\n");
    imprimeLista(lista);

    removePos(&lista, 1);
    printf("Lista após remover posição 1:\n");
    imprimeLista(lista);

    // Busca
    int valor = 25;
    int pos = buscaValor(lista, valor);
    if (pos != -1)
        printf("Valor %d encontrado na posição %d\n", valor, pos);
    else
        printf("Valor %d não encontrado\n", valor);

    // Contagem de elementos
    printf("Número total de elementos: %d\n", contaElementos(lista));

    return 0;
}
