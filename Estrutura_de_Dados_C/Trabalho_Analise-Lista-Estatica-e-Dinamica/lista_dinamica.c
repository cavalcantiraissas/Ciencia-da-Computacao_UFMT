/*
Universidade Federal de Mato Grosso
Disciplina: Estrutura de Dados I
Alunos: Raissa Cavalcanti (RGA:202311722030) e Murilo Becker (RGA: 202411722030)

Teste de Lista dinamica com capacidade de 10.000

*/

#include <stdio.h>
#include <stdlib.h>
#include<time.h>
#include<stdbool.h>
#include<math.h>

typedef struct no{
    int valor;
    struct no *proximo;
}No;

typedef struct{
    No *inicio; 
    int tam; 
}Lista;


void InicializarLista(Lista *lista);
void InsereInicioLista(Lista*, int);
void InsereFimLista(Lista*, int);
void InserePosicaoLista(Lista*, int,int);
void ImprimeLista(Lista);
void LiberaLista(Lista *lista);


void InicializarLista(Lista *lista){
    lista->inicio =NULL;
    lista->tam = 0;
}


void InsereInicioLista(Lista *lista, int num){
    No *novo = malloc(sizeof(No));

    if(novo){
        novo->valor = num;
        novo->proximo = lista->inicio;
        lista->inicio = novo;
        lista->tam++; 
    } else {
        printf("[ERRO]!\n");
    }
}

void InsereFimLista(Lista *lista, int num){
    No *aux, *novo = malloc(sizeof(No));

    if(novo){
        novo->valor = num;
        novo->proximo = NULL;

    
        if(lista->inicio == NULL){ 
            lista->inicio = novo; 
        } else {
            aux = lista->inicio;
            while (aux->proximo)
               aux = aux->proximo;
            aux->proximo = novo;
        }
        lista->tam++;
    } else {
        printf("[ERRO]!\n");
    }
}

void InserePosicaoLista(Lista *lista, int num, int pos) {
    if (pos <= 0) {
        InsereInicioLista(lista, num);
        return;
    }
    No *aux = lista->inicio;
    No *novo = malloc(sizeof(No));

    if (novo) {
        novo->valor = num;
        for (int i = 0; aux && i < pos - 1; i++)
            aux = aux->proximo;

        if (aux) {
            novo->proximo = aux->proximo;
            aux->proximo = novo;
        } else {
            free(novo);
            printf("[ERRO] Posição inválida!\n");
        }
    }
}


void ImprimeLista(Lista lista){
    No *no = lista.inicio;
    printf("Lista [");
    while (no){
        printf("%d,", no->valor);
        no = no->proximo;
    }
    printf("]\n\nElementos =%d\n", lista.tam);

}

void LiberaLista(Lista *lista) {
    No *aux;
    while (lista->inicio) {
        aux = lista->inicio;
        lista->inicio = lista->inicio->proximo;
        free(aux);
    }
    lista->tam = 0;
}



