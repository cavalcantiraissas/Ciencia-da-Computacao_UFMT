#include <stdio.h>
#include <stdlib.h>
#include "lista_dupla.h"
#include "lista_circular.h"

int main() {
    printf("=== Lista Duplamente Encadeada ===\n");

    ListaDupla* listaD = criarListaDupla();
    inserirInicio(listaD, 10);
    inserirFim(listaD, 20);
    inserirInicio(listaD, 5);
    inserirPosicao(listaD, 15, 2);

    printf("Lista após inserções: ");
    exibirListaDupla(listaD);

    removerInicio(listaD);
    printf("Lista após remover início: ");
    exibirListaDupla(listaD);

    removerFim(listaD);
    printf("Lista após remover fim: ");
    exibirListaDupla(listaD);

    removerPosicao(listaD, 1);
    printf("Lista após remover posição 1: ");
    exibirListaDupla(listaD);

    inserirFim(listaD, 25);
    inserirFim(listaD, 30);
    inserirFim(listaD, 35);
    removerValor(listaD, 25);
    printf("Lista após remover valor 25: ");
    exibirListaDupla(listaD);

    liberarListaDupla(listaD);

    printf("\n=== Lista Circular Simplesmente Encadeada ===\n");

    ListaCircular* listaC = criarListaCircular();
    inserirInicioC(listaC, 10);
    inserirFimC(listaC, 20);
    inserirInicioC(listaC, 5);
    inserirFimC(listaC, 30);

    printf("Lista Circular após inserções: ");
    exibirListaCircular(listaC);

    removerInicioC(listaC);
    printf("Lista Circular após remover início: ");
    exibirListaCircular(listaC);

    removerFimC(listaC);
    printf("Lista Circular após remover fim: ");
    exibirListaCircular(listaC);

    girarLista(listaC);
    printf("Lista Circular após girar: ");
    exibirListaCircular(listaC);

    liberarListaCircular(listaC);

    return 0;
}
