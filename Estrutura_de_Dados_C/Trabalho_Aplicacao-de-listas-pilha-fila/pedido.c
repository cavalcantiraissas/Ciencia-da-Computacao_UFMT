/*
Universidade Federal de Mato Grosso
Disciplina de Estrutura de Dados I 

Sistema de Gerenciamento de Pedidos em um Restaurante
Aluna: Raissa Cavalcanti

Implementação das Funções para manipulação de pedidos

*/



#include "pedido.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Pedido* criarPedido(int numero, char* cliente, char* itens) {
    Pedido* novo = (Pedido*) malloc(sizeof(Pedido));
    novo->numero = numero;
    strcpy(novo->cliente, cliente);
    strcpy(novo->itens, itens);
    strcpy(novo->status, "pendente");
    novo->prox = NULL;
    return novo;
}

void alterarStatus(Pedido* pedido, char* novoStatus) {
    if (pedido) {
        strcpy(pedido->status, novoStatus);
    }
}

void imprimirPedido(Pedido* pedido) {
    if (pedido) {
        printf("Pedido #%d - Cliente: %s\nItens: %s\nStatus: %s\n", 
               pedido->numero, pedido->cliente, pedido->itens, pedido->status);
    }
}
