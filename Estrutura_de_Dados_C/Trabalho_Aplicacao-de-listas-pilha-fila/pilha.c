/*
Universidade Federal de Mato Grosso
Disciplina de Estrutura de Dados I 

Sistema de Gerenciamento de Pedidos em um Restaurante
Aluna: Raissa Cavalcanti


Implementação das funções da pilha

*/


#include "pilha.h"
#include <stdio.h>
#include <stdlib.h>

Pilha* empilhar(Pilha* topo, Pedido* pedido) {
    Pilha* novo = (Pilha*) malloc(sizeof(Pilha));
    novo->pedido = pedido;
    novo->prox = topo;
    return novo;
}

Pilha* desempilhar(Pilha** topo) {
    if (*topo == NULL) return NULL;
    Pilha* temp = *topo;
    *topo = (*topo)->prox;
    free(temp);
    return *topo;
}

void imprimirPilha(Pilha* topo) {
    Pilha* temp = topo;
    while (temp) {
        imprimirPedido(temp->pedido);
        temp = temp->prox;
    }
}
