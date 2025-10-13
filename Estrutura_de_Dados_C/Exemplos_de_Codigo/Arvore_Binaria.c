/**
 * UFMT - Ciência da Computação
 * Disciplina de ED1
 * Prof. Ivairton Santos
 * 
 * Implementação da estrutura de dados Árvore binária
 * 
 */

#include <stdio.h>
#include <stdlib.h>


//*** Definição da estrutura do dado do nó da árvore binária ***//
struct estruturaArvBin {
	int valor;
	struct estruturaArvBin *esq;
	struct estruturaArvBin *dir;
};
typedef struct estruturaArvBin tipoArvBin;


//*** Prototipação ***//
tipoArvBin *alocaNo(int valor);
void insereArvBin(tipoArvBin **no, int valor);
void impOrdem(tipoArvBin *no);
void impPreOrdem(tipoArvBin *no);
void impPosOrdem(tipoArvBin *no);
/***
	TODO: funções a serem implementadas:
	-Busca Valor
	-Conta número de nós na árvore
	-Conta o número de nós folhas
	-Calcula a altura da árvore
	-Contabiliza o número de nós por nível
	-Verifica se um certo nó é folha
	-Remove um nó da árvore
***/



//*** Implementação das funções ***//

//Implementação da função que aloca um novo nó na memória
tipoArvBin *alocaNo(int valor) {
	tipoArvBin *novoNo = (tipoArvBin*) malloc( sizeof(tipoArvBin) );
	novoNo->valor = valor;
	novoNo->esq = NULL;
	novoNo->dir = NULL;
	return novoNo;
}
	
//Função que insere um novo valor na árvore binária
void insereArvBin(tipoArvBin **no, int valor) {
	
	//Verifica se o nó nulo foi alcançado
	if (*no == NULL) {
		(*no) = alocaNo(valor);
	} else if ( valor < (*no)->valor ) {
		insereArvBin( &(*no)->esq, valor);
	} else if ( valor > (*no)->valor ) {
		insereArvBin( &(*no)->dir, valor);
	} else {
		printf("[ERRO] - Valor %d já existe na árvore.\n", valor);
	}

}

//Função para impressão da árvore em Ordem
void impOrdem(tipoArvBin *no) {
	if (no != NULL) {
		impOrdem(no->esq);
		printf("[%d]", no->valor);
		impOrdem(no->dir);
	}
}

//Função para impressão da árvore em Ordem
void impPreOrdem(tipoArvBin *no) {
	if (no != NULL) {
		printf("[%d]", no->valor);
		impPreOrdem(no->esq);
		impPreOrdem(no->dir);
	}
}

//Função para impressão da árvore em Ordem
void impPosOrdem(tipoArvBin *no) {
	if (no != NULL) {
		impPosOrdem(no->esq);
		impPosOrdem(no->dir);
		printf("[%d]", no->valor);
	}
}
