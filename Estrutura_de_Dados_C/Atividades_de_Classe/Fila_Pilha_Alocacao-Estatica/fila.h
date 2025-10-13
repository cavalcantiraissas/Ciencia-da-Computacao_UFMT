#ifndef FILA_H
#define FILA_H

#include <stdbool.h>

#define TAM 10

struct est_fila {
    int vet_fila[TAM];
    int cont;
};
typedef struct est_fila tipo_fila;

void insere_fila(tipo_fila *fl, int vl);
int remove_fila(tipo_fila *fl);
void imprime_fila(tipo_fila fl);

#endif
