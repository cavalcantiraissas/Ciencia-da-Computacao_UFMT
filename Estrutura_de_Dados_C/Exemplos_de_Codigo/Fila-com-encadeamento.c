/**
 * UFMT - Ciencia da computacao
 * Estrutura de Dados
 * Prof. Ivairton
 * 
 * Fila com encadeamento
 */

#ifndef __FILA_ENC__
#define __FILA_ENC__

#include<stdio.h>
#include<stdlib.h>

//Definicao de estrutura
struct est_no {
    int valor;
    struct est_no *prox;
};
typedef struct est_no tipo_no;

//Prototipacao
tipo_no *aloca_novo_no(int);
void insereFila(tipo_no **, int);
int removeFila(tipo_no**);
void imprimeFila(tipo_no*);

//Implementacao das funcoes:

/**
 * @brief Funcao que aloca um novo no na memoria e retorna seu endereco.
 * @par valor: valor a ser armazenado no novo no
 * 
 * @return tipo_no*: endereco de memoria do novo noh alocado
 */
tipo_no *aloca_novo_no(int vl) {
    tipo_no *novo_no;
    novo_no = (tipo_no*) malloc(sizeof(tipo_no));
    if (novo_no != NULL) { //Se alocacao de memoria ocorreu normalmente
        novo_no->valor = vl;
        novo_no->prox = NULL;
    } else { //Caso tenha ocorrido falha na alocacao de memoria
        printf("[ERRO] Falha na alocacao de memoria para um novo noh\n");
    }
    return novo_no;
}

/**
 * @brief Funcao que insere um novo valor da fila encadeada
 * @par fila: Ponteiro para o inicio da estrutura encadeada
 * @par valor: valor a ser inserido
 */
void insereFila(tipo_no **fila, int vl) {
    tipo_no *novo_no, *aux;
    novo_no = aloca_novo_no(vl);
    if (novo_no != NULL) {
        if ( (*fila) == NULL ) { //Se fila estah vazia
            (*fila) = novo_no;
        } else { //Percorre estrutura buscando o ultimo elemento
            aux = (*fila);
            while (aux->prox != NULL)
                aux = aux->prox;
            aux->prox = novo_no;
        }
    }
}

/**
 * @brief Funcao que removo o primeiro valor da fila
 * 
 * @return int: valor removido
 */
int removeFila(tipo_no **fila) {
    tipo_no *aux;
    int vl;

    if ( (*fila) != NULL ) { //Se fila tem valor, remove e retorna valor
        vl = (*fila)->valor;
        aux = (*fila);
        (*fila) = (*fila)->prox;
        free(aux);
        return vl;
    } else { //Se fila estah vazia
        return -1;
    }
}

/**
 * @brief Funcao para impressao da fila. Tem finalidade de debugacao
 */
void imprimeFila(tipo_no *aux) {
    printf("Fila->");
    while (aux != NULL) {
        printf("[%d]->", aux->valor);
        aux = aux->prox;
    }
    printf("NULO\n");
}


#endif //__FILA_ENC__
