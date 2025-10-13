/**
 * UFMT - Ciencia da computacao
 * Estrutura de Dados
 * Prof. Ivairton
 * 
 * Pilha com encadeamento
 */

#ifndef __PILHA_ENC__
#define __PILHA_ENC__

#include<stdio.h>
#include<stdlib.h>

//Definicao de estrutura
struct est_no_pilha {
    int valor;
    struct est_no_pilha *prox;
};
typedef struct est_no_pilha tipo_pilha;

//Prototipacao
tipo_pilha *aloca_novo_no_pilha(int);
void empilha(tipo_pilha **, int);
int desempilha(tipo_pilha**);
int topo(tipo_pilha*);
void imprimePilha(tipo_pilha*);

//Implementacao das funcoes:

/**
 * @brief Funcao que aloca um novo no na memoria e retorna seu endereco.
 * @par valor: valor a ser armazenado no novo no
 * 
 * @return tipo_pilha*: endereco de memoria do novo noh alocado
 */
tipo_pilha *aloca_novo_no_pilha(int vl) {
    tipo_pilha *novo_no;
    novo_no = (tipo_pilha*) malloc(sizeof(tipo_pilha));
    if (novo_no != NULL) { //Se alocacao de memoria ocorreu normalmente
        novo_no->valor = vl;
        novo_no->prox = NULL;
    } else { //Caso tenha ocorrido falha na alocacao de memoria
        printf("[ERRO] Falha na alocacao de memoria para um novo noh\n");
    }
    return novo_no;
}

/**
 * @brief Funcao que empilha um novo valor da pilha encadeada
 * @par pilha: Ponteiro para o topo da estrutura encadeada
 * @par vl: valor a ser inserido
 */
void empilha(tipo_pilha **pl, int vl) {
    tipo_pilha *novo_no, *aux;
    novo_no = aloca_novo_no_pilha(vl);
    if (novo_no != NULL) {  //verifica se alocacao de memoria ocorreu corretamente
        novo_no->prox = (*pl);
        (*pl) = novo_no;
    }
}

/**
 * @brief Funcao que desempilha o valor da pilha
 * 
 * @return int: valor removido
 */
int desempilha(tipo_pilha **pilha) {
    tipo_pilha *aux;
    int vl;

    if ( (*pilha) != NULL ) { //Se pilha tem valor, remove e retorna valor
        vl = (*pilha)->valor;
        aux = (*pilha);
        (*pilha) = (*pilha)->prox;
        free(aux);
        return vl;
    } else { //Se fila estah vazia
        return -1;
    }
}

/**
 * @brief Funcao que retorna o elemento do topo
 * 
 * @param pilha 
 * @return int retorna o valor do topo da pilha, ou -1 caso vazia
 */
int topo(tipo_pilha *pilha) {
    if (pilha != NULL)
        return pilha->valor;
    else
        return -1;
}

/**
 * @brief Funcao para impressao da pilha. Tem finalidade de debugacao.
 */
void imprimePilha(tipo_pilha *aux) {
    printf("Pilha: (TOPO)->");
    while (aux != NULL) {
        printf("[%d]->", aux->valor);
        aux = aux->prox;
    }
    printf("NULO (BASE)\n");
}

#endif //__PILHA_ENC__
