/*
Universidade Federal de Mato Grosso
Disciplina de Estrutura de Dados I 

Sistema de Gerenciamento de Pedidos em um Restaurante
Aluna: Raissa Cavalcanti


Definição da estrutura e funções da pilha

*/



#ifndef PILHA_H
#define PILHA_H
#include "pedido.h"

typedef struct est_pilha {
    Pedido* pedido;
    struct est_pilha* prox;
} Pilha;

Pilha* empilhar(Pilha* topo, Pedido* pedido);
Pilha* desempilhar(Pilha** topo);
void imprimirPilha(Pilha* topo);

#endif
