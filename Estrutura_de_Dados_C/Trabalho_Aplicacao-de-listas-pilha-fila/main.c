/*
Universidade Federal de Mato Grosso
Disciplina de Estrutura de Dados I 

Sistema de Gerenciamento de Pedidos em um Restaurante
Aluna: Raissa Cavalcanti


Menu principal e lógica do programa

*/

#include "pedido.h"
#include "fila.h"
#include "pilha.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main() {
    Fila* filaInicio = NULL, *filaFim = NULL;
    Pilha* pilha = NULL;
    int opcao, num;
    char nome[50], itens[100];

    do {
        printf("\n1 - Novo Pedido\n2 - Iniciar Preparo\n3 - Finalizar Preparo\n4 - Entregar Pedido\n5 - Sair\n");
        scanf("%d", &opcao);
        getchar();
        switch(opcao) {
            case 1:
                printf("Numero do pedido: ");
                scanf("%d", &num);
                getchar();
                printf("Nome do cliente: ");
                fgets(nome, 50, stdin);
                nome[strcspn(nome, "\n")] = 0;
                printf("Itens do pedido: ");
                fgets(itens, 100, stdin);
                itens[strcspn(itens, "\n")] = 0;
                Pedido* novo = criarPedido(num, nome, itens);
                filaFim = enfileirar(&filaFim, novo);
                if (!filaInicio) filaInicio = filaFim;

                break;
            case 2:
                if (filaInicio) {
                    alterarStatus(filaInicio->pedido, "em preparo");
                    imprimirPedido(filaInicio->pedido);
                }
                break;
            case 3:
                if (filaInicio) {
                    alterarStatus(filaInicio->pedido, "pronto");
                    pilha = empilhar(pilha, filaInicio->pedido);
                    filaInicio = desenfileirar(&filaInicio);
                }
                break;
            case 4:
                if (pilha) {
                    alterarStatus(pilha->pedido, "entregue");
                    imprimirPedido(pilha->pedido);
                    pilha = desempilhar(&pilha);
                }
                break;
        }
    } while (opcao != 5);

    return 0;
}
