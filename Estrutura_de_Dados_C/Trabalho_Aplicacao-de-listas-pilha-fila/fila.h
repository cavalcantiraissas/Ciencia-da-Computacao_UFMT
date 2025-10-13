/*
Universidade Federal de Mato Grosso
Disciplina de Estrutura de Dados I 

Sistema de Gerenciamento de Pedidos em um Restaurante
Aluna: Raissa Cavalcanti


Definição da estrutura e funções da fila

*/



#ifndef FILA_H
#define FILA_H
#include "pedido.h"

typedef struct est_fila {
    Pedido* pedido;
    struct est_fila* prox;
} Fila;

Fila* enfileirar(Fila** fim, Pedido* pedido);
Fila* desenfileirar(Fila** inicio);
void imprimirFila(Fila* inicio);

#endif
