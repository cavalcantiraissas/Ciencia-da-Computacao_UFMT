/*
Universidade Federal de Mato Grosso
Disciplina de Estrutura de Dados I 

Sistema de Gerenciamento de Pedidos em um Restaurante
Aluna: Raissa Cavalcanti

Definição da Estrutura do Pedido

*/


#ifndef PEDIDO_H
#define PEDIDO_H

typedef struct est_pedido {
    int numero;
    char cliente[50];
    char itens[100];
    char status[20];
    struct est_pedido* prox;
} Pedido;

Pedido* criarPedido(int numero, char* cliente, char* itens);
void alterarStatus(Pedido* pedido, char* novoStatus);
void imprimirPedido(Pedido* pedido);

#endif
