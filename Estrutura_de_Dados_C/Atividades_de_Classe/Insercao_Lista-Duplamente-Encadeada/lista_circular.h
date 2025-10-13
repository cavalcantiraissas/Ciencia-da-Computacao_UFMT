#ifndef LISTA_CIRCULAR_H
#define LISTA_CIRCULAR_H

typedef struct NoC {
    int dado;
    struct NoC *prox;
} NoC;

typedef struct {
    NoC *fim;
} ListaCircular;

ListaCircular* criarListaCircular();
void inserirInicioC(ListaCircular* lista, int dado);
void inserirFimC(ListaCircular* lista, int dado);
void removerInicioC(ListaCircular* lista);
void removerFimC(ListaCircular* lista);
void girarLista(ListaCircular* lista);
void exibirListaCircular(ListaCircular* lista);
void liberarListaCircular(ListaCircular* lista);

#endif
