/*
Universidade Federal de Mato Grosso
Disciplina: Estrutura de Dados I
Alunos: Raissa Cavalcanti (RGA:202311722030) e Murilo Becker (RGA: 202411722030)
*/

#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<stdbool.h>
#include<math.h>


#ifndef lst_dpl_enc
#define lst_dpl_enc

typedef struct no{
    int valor;
    struct no *proximo;
    struct no *anterior;
}No;

void InsereFim(No **, int );
void InserePos(No **, int , int );
void InsereInicio(No **, int );
void ImprimeLista(No *);
void LiberaLista(No **lista);  

void InsereInicio(No **lista, int num){
    No *novo = malloc(sizeof(No));

    if(novo){
        novo->valor = num;
        novo->proximo = *lista;
        novo->anterior = NULL;
        if(*lista)
            (*lista)->anterior = novo;
        *lista = novo;
    }
    else
        printf("[ERRO]!\n");
}

void InsereFim(No **lista, int num){
    No *aux, *novo = malloc(sizeof(No));

    if(novo){
        novo->valor = num;
        novo->proximo = NULL;

        if(*lista == NULL){
            *lista = novo;
            novo->anterior = NULL;
        }
        else{
            aux = *lista;
            while(aux->proximo)
                aux = aux->proximo;
            aux->proximo = novo;
            novo->anterior = aux;
        }
    }
    else
        printf("[ERRO]!\n");
}

void InserePos(No **lista, int num, int pos) {
    No *aux = *lista;
    No *novo = malloc(sizeof(No));

    if (!novo) {
        printf("[ERRO] Falha na alocação de memória!\n");
        return;
    }

    novo->valor = num;

    if (pos <= 0 || *lista == NULL) {
        InsereInicio(lista, num);
        return;
    }

    for (int i = 0; aux && i < pos - 1; i++) {
        aux = aux->proximo;
    }

    if (aux) {
        novo->proximo = aux->proximo;
        if (aux->proximo) {
            aux->proximo->anterior = novo;
        }
        novo->anterior = aux;
        aux->proximo = novo;
    } else {
        printf("[ERRO] Posição inválida!\n");
        free(novo);
    }
}

void ImprimeLista(No *no){
    int cont = 0;
    printf("\n\tLista: ");
    while(no){
        printf("%d ", no->valor);
        no = no->proximo;
        cont++;
    }
    printf("\n\n");
    printf("\nelementos: %d ", cont);
}

void LiberaLista(No **lista) {
    No *aux;
    while (*lista) {
        aux = *lista;
        *lista = (*lista)->proximo;
        free(aux);
    }
}




#endif
