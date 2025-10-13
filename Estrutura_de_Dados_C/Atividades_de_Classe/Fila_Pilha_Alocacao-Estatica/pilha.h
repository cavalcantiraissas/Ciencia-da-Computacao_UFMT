#ifndef PILHA_H
#define PILHA_H

#define TAM 10

struct est_pilha {
    int vet_pilha[TAM];
    int cont;
};
typedef struct est_pilha tipo_pilha;

void empilha(tipo_pilha *pl, int vl);
int desempilha(tipo_pilha *pl);
void imprime_pilha(tipo_pilha pl);

#endif
