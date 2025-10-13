#include "fila_enc.h"

/**
 * @brief 
 * 
 * @param 
 * @param 
 * 
 * @return 
 */

tipo_no *aloca_no(int vl) {
    tipo_no *novo_no;
    novo_no = (tipo_no*) malloc(sizeof(tipo_no));
    if (novo_no != NULL) {
        novo_no->valor = vl;
        novo_no->prox = NULL;
    }
    return novo_no;
}

void insereFilaEnc(tipo_no **fila ,int vl) {
    if ( (*fila) == NULL) {
        (*fila) = aloca_no(vl);
    } else {
        tipo_no *novo_no, *aux;
        novo_no = aloca_no(vl);
        if (novo_no != NULL) {
            aux = (*fila);
            while (aux->prox != NULL) 
                aux = aux->prox;
            aux->prox = novo_no;         
        }
    }
}

/**
 * @brief 
 * 
 * @param fila 
 * @param fila 
 * 
 * @return 
 */

int removeFilaEnc(tipo_no **fila) {
    int valor;
    tipo_no *aux;

    if ((*fila) != NULL) {
        valor = (*fila)->valor;
        aux = (*fila);
        (*fila) = (*fila)->prox;
        free(aux);
        return valor;
    }
    return -1;
}

/**
 * @brief Funcao que insere no inicio da lista.
 * 
 * @param tipo_lista *lista - Estrutura da lista
 * @param int valor - valor a ser inserido na lista
 * 
 * @return int - verdadeiro se insercao com sucesso, falso caso contrário
 */

void imprimeFilaEnc(tipo_no *fila) {
    printf("FILA:\n\t|\n\t->");
    while (fila != NULL) {
        printf("[%d]->", fila->valor);
        fila = fila->prox;
    }
    printf("[NULL]\n");
}

int qtdvalores(tipo_no *fila){
    int qtd;
    while (fila != NULL){
        qtd++;
        fila = fila->prox;
    }
    return qtd;

}

