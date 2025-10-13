#ifndef LISTA_DUPLA_H
#define LISTA_DUPLA_H

typedef struct No {
    int dado;
    struct No *prox, *ant;
} No;

typedef struct {
    No *inicio;
    No *fim;
} ListaDupla;

ListaDupla* criarListaDupla();
void inserirInicio(ListaDupla* lista, int dado);
void inserirFim(ListaDupla* lista, int dado);
void inserirPosicao(ListaDupla* lista, int dado, int posicao);
void removerInicio(ListaDupla* lista);
void removerFim(ListaDupla* lista);
void removerPosicao(ListaDupla* lista, int posicao);
void removerValor(ListaDupla* lista, int dado);
void exibirListaDupla(ListaDupla* lista);
void liberarListaDupla(ListaDupla* lista);

#endif
