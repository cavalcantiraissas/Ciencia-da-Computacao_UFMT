#include "lista.h"

// Insere no início da lista
void insereInicio(No **lst, int vlr) {
    No *novoNo = (No*)malloc(sizeof(No));
    if (novoNo == NULL) {
        printf("Erro ao alocar memória\n");
        return;
    }
    novoNo->valor = vlr;
    novoNo->prox = *lst;
    *lst = novoNo;
}

// Insere no fim da lista
void insereFim(No **lst, int vlr) {
    No *novoNo = (No*)malloc(sizeof(No));
    if (novoNo == NULL) {
        printf("Erro ao alocar memória\n");
        return;
    }
    novoNo->valor = vlr;
    novoNo->prox = NULL;

    if (*lst == NULL) {
        *lst = novoNo;
    } else {
        No *aux = *lst;
        while (aux->prox != NULL) {
            aux = aux->prox;
        }
        aux->prox = novoNo;
    }
}

// Insere em uma posição específica
void inserePos(No **lst, int pos, int vlr) {
    if (pos < 0) return;

    No *novoNo = (No*)malloc(sizeof(No));
    if (novoNo == NULL) {
        printf("Erro ao alocar memória\n");
        return;
    }
    novoNo->valor = vlr;

    if (pos == 0 || *lst == NULL) {
        novoNo->prox = *lst;
        *lst = novoNo;
    } else {
        No *aux = *lst;
        for (int i = 0; i < pos - 1 && aux->prox != NULL; i++) {
            aux = aux->prox;
        }
        novoNo->prox = aux->prox;
        aux->prox = novoNo;
    }
}

// Remove do início
void removeInicio(No **lst) {
    if (*lst == NULL) return;
    No *temp = *lst;
    *lst = (*lst)->prox;
    free(temp);
}

// Remove do fim
void removeFim(No **lst) {
    if (*lst == NULL) return;

    No *aux = *lst, *ant = NULL;
    while (aux->prox != NULL) {
        ant = aux;
        aux = aux->prox;
    }

    if (ant == NULL) { // Apenas um elemento
        *lst = NULL;
    } else {
        ant->prox = NULL;
    }

    free(aux);
}

// Remove por posição
void removePos(No **lst, int pos) {
    if (*lst == NULL || pos < 0) return;

    No *aux = *lst, *ant = NULL;
    for (int i = 0; i < pos && aux != NULL; i++) {
        ant = aux;
        aux = aux->prox;
    }

    if (aux == NULL) return;

    if (ant == NULL) {
        *lst = aux->prox;
    } else {
        ant->prox = aux->prox;
    }

    free(aux);
}

// Remove por valor
void removeValor(No **lst, int vlr) {
    No *aux = *lst, *ant = NULL;
    while (aux != NULL && aux->valor != vlr) {
        ant = aux;
        aux = aux->prox;
    }

    if (aux == NULL) return;

    if (ant == NULL) {
        *lst = aux->prox;
    } else {
        ant->prox = aux->prox;
    }

    free(aux);
}

// Busca por valor e retorna posição (-1 se não encontrado)
int buscaValor(No *lst, int vlr) {
    int pos = 0;
    while (lst != NULL) {
        if (lst->valor == vlr) return pos;
        lst = lst->prox;
        pos++;
    }
    return -1;
}

// Busca por posição e retorna valor (-1 se inválido)
int buscaPosicao(No *lst, int pos) {
    int i = 0;
    while (lst != NULL) {
        if (i == pos) return lst->valor;
        lst = lst->prox;
        i++;
    }
    return -1;
}

// Conta número de elementos
int contaElementos(No *lst) {
    int count = 0;
    while (lst != NULL) {
        count++;
        lst = lst->prox;
    }
    return count;
}

// Imprime lista
void imprimeLista(No *lst) {
    while (lst != NULL) {
        printf("%d -> ", lst->valor);
        lst = lst->prox;
    }
    printf("NULL\n");
}
