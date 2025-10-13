/*
Universidade Federal de Mato Grosso
Disciplina de Estrutura de Dados I 

Sistema de Gerenciamento de Pedidos em um Restaurante
Aluna: Raissa Cavalcanti


Implementação das funções da Fila

*/


#include "fila.h"
#include <stdio.h>
#include <stdlib.h>

Fila* enfileirar(Fila** fim, Pedido* pedido) {
    Fila* novo = (Fila*) malloc(sizeof(Fila));
    if (!novo) {
        printf("Erro ao alocar memória para fila.\n");
        return NULL;
    }
    novo->pedido = pedido;
    novo->prox = NULL;
    
    if (*fim) {
        (*fim)->prox = novo;
    }
    *fim = novo;

    return novo;
}


Fila* desenfileirar(Fila** inicio) {
    if (*inicio == NULL) return NULL;
    Fila* temp = *inicio;
    *inicio = (*inicio)->prox;
    free(temp);
    return *inicio;
}

void imprimirFila(Fila* inicio) {
    Fila* temp = inicio;
    while (temp) {
        imprimirPedido(temp->pedido);
        temp = temp->prox;
    }
}
