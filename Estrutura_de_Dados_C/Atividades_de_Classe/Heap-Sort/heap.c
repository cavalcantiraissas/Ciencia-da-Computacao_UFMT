#include <stdio.h>
#include <stdlib.h>
#include "heap.h"

// Função auxiliar para trocar dois elementos
static void trocar(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

// Função para ajustar a estrutura da heap para cima
static void subir(Heap *heap, int indice) {
    while (indice > 0) {
        int pai = (indice - 1) / 2;
        if (heap->dados[pai] >= heap->dados[indice])
            break;
        trocar(&heap->dados[pai], &heap->dados[indice]);
        indice = pai;
    }
}

// Função para ajustar a estrutura da heap para baixo
static void descer(Heap *heap, int indice) {
    int maior = indice;
    int esquerda = 2 * indice + 1;
    int direita = 2 * indice + 2;

    if (esquerda < heap->tamanho && heap->dados[esquerda] > heap->dados[maior])
        maior = esquerda;
    if (direita < heap->tamanho && heap->dados[direita] > heap->dados[maior])
        maior = direita;

    if (maior != indice) {
        trocar(&heap->dados[indice], &heap->dados[maior]);
        descer(heap, maior);
    }
}

// Cria uma nova heap com capacidade informada
Heap* criarHeap(int capacidade) {
    Heap *novaHeap = (Heap*)malloc(sizeof(Heap));
    if (!novaHeap) return NULL;

    novaHeap->dados = (int*)malloc(capacidade * sizeof(int));
    if (!novaHeap->dados) {
        free(novaHeap);
        return NULL;
    }

    novaHeap->tamanho = 0;
    novaHeap->capacidade = capacidade;
    return novaHeap;
}

// Insere um novo elemento na heap
void inserir(Heap *heap, int valor) {
    if (heap->tamanho == heap->capacidade) {
        printf("Erro: Heap cheia!\n");
        return;
    }
    heap->dados[heap->tamanho] = valor;
    subir(heap, heap->tamanho);
    heap->tamanho++;
}

// Remove e retorna o elemento de maior prioridade
int remover(Heap *heap) {
    if (heap->tamanho == 0) {
        printf("Erro: Heap vazia!\n");
        return -1;
    }

    int raiz = heap->dados[0];
    heap->dados[0] = heap->dados[heap->tamanho - 1];
    heap->tamanho--;
    descer(heap, 0);

    return raiz;
}

// Verifica se a heap está vazia
bool estaVazia(Heap *heap) {
    return heap->tamanho == 0;
}

// Libera a memória alocada para a heap
void destruirHeap(Heap *heap) {
    if (heap) {
        free(heap->dados);
        free(heap);
    }
}
