/*
Universidade Federal de Mato Grosso
Disciplina: Estrutura de Dados I
Alunos: Raissa Cavalcanti (RGA:202311722030) e Murilo Becker (RGA: 202411722030)

Teste de Lista Estática com capacidade de 10.000

*/

#ifndef lst_est
#define lst_est

#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<stdbool.h>
#include<math.h>

#define tam 10000

struct strlist {
    int *listaq; 
    int elementos;
    int TAM; 
};

typedef struct strlist tipoLista;

int InicializaLista(tipoLista*, int);
int ListaCheia(tipoLista);
int ListaVazia(tipoLista);
int InsereInicio(tipoLista*, int);
int InsereFim(tipoLista*, int);
int InserePos(tipoLista*, int, int);
void ImprimeLista(tipoLista);
void LiberaLista(tipoLista *l); 

int InicializaLista(tipoLista* l, int tamanho) {
    if (l->listaq != NULL) {
        printf("[ERRO] Lista já inicializada!\n");
        return 0;
    }
    l->listaq = (int*) malloc(sizeof(int) * tamanho);
    if (l->listaq != NULL) {
        l->TAM = tamanho;
        l->elementos = 0;
        return 1;
    }
    return 0;
}



int ListaCheia(tipoLista l) {
    if ( l.elementos < l.TAM ) {
        return 0;
    } else {
        return 1;
    }
}

int ListaVazia(tipoLista l) {
    if ( l.elementos > 0 ) { 
        return 0;
    } else {
        return 1;
    }
}

int InsereInicio(tipoLista *l, int valor) { 
    if ( ! ListaCheia(*l) ) {
        int i;
        for (i = l->elementos; i > 0; i-- ) {
            l->listaq[i] = l->listaq[i-1]; 
        }
        l->listaq[0] = valor;
        l->elementos++;
        return 1;
    }
    return 0;
}

int InsereFim(tipoLista* l, int valor) {
    if ( ! ListaCheia (*l) ) {
        l->listaq[l->elementos++] = valor; 
        return 1;
    }
    return 0;
}


int InserePos(tipoLista* l, int valor, int pos) {
    if ( ! ListaCheia(*l) ) {
        if ( (pos >= 0) && (pos < l->TAM) && (pos <= l->elementos) ) {
            int i;
            for (i = l->elementos; i > pos; i-- ) {
                l->listaq[i] = l->listaq[i-1];
            }
            l->listaq[pos] = valor;
            l->elementos++;
            return 1;
        }
    }
    return 0;
}


void ImprimeLista(tipoLista l) {
    int i;
    printf("Lista: [");
    for (i = 0; i < l.elementos; i++) {
        printf("%d,", l.listaq[i]);
    }
    printf("]\n\nelementos = %d\n", l.elementos);
}

void LiberaLista(tipoLista *l) {
    free(l->listaq);
    l->listaq = NULL;
    l->elementos = 0;
    l->TAM = 0;
}

#endif
