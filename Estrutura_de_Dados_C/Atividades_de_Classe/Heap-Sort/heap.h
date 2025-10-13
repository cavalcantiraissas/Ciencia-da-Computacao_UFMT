#ifndef HEAP_H
#define HEAP_H

#include <stdbool.h>

typedef struct {
    int *dados;
    int tamanho;
    int capacidade;
} Heap;

// Cria uma nova heap com capacidade definida
Heap* criarHeap(int capacidade);

// Insere um elemento na heap
void inserir(Heap *heap, int valor);

// Remove e retorna o elemento de maior prioridade
int remover(Heap *heap);

// Verifica se a heap está vazia
bool estaVazia(Heap *heap);

// Libera a memória alocada para a heap
void destruirHeap(Heap *heap);

#endif
