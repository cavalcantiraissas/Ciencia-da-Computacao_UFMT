/**
 * UFMT - Bacharelado em Ciencia da Computacao
 * Prof. Ivairton
 *
 * Implementacao de FILA.
 */
#include<stdio.h>
#include<stdlib.h>

// Definicao de constantes
#define TAM 10

// Definicao de tipos/registros
struct est_fila {
    int fila[TAM];
    int qtd_elem;
};
typedef struct est_fila tipo_fila;

// Prototipo de funcoes
int filaVazia(tipo_fila *fl);
int filaCheia(tipo_fila *fl);
void insereFila(tipo_fila *fl, int valor);
int removeFila(tipo_fila *fl);
int primeiroFila(tipo_fila *fl);
void imprimeFila(tipo_fila *fl); //Funcao para debug/visualizar fila

int main() {
    tipo_fila fila_de_num;
    fila_de_num.qtd_elem = 0;

    insereFila(&fila_de_num, 10);
    insereFila(&fila_de_num, 20);
    insereFila(&fila_de_num, 30);

    imprimeFila(&fila_de_num);

    printf("Valor removido: %d\n\n", removeFila(&fila_de_num) );

    imprimeFila(&fila_de_num);
    return 0;
}

/**
 * Funcao que verifica se fila esta vazia.
 * Se estiver vazia a funcao retorna 1 (verdadeiro)
 * Se estiver com ao menos um valor a funcao retorna 0 (falso)
 */
int filaVazia(tipo_fila *fl) {
    if (fl->qtd_elem == 0) {
        return 1;
    } else {
        return 0;
    }
}

/**
 * Funcao que verifica se a fila esta cheia.
 * Se estiver cheia a funcao retorna 1 (verdadeiro)
 * Se houver espaco disponivel a funcao retorna 0 (falso)
 */
int filaCheia(tipo_fila *fl) {
    if (fl->qtd_elem == TAM) {
        return 1;
    } else {
        return 0;
    }
}

/**
 * Funcao que insere um novo elemento na fila
 */
void insereFila(tipo_fila *fl, int valor) {
    if (filaCheia(fl)) { //Verifica se fila estah cheia
        printf("[ERRO] Fila estah cheia, nao eh possivel inserir valor.\n");
    } else { //Havendo espaco disponivel, insere valor
        fl->fila[fl->qtd_elem++] = valor;
        //fl->fila[fl->qtd_elem] = valor;
        //fl->qtd_elem++;
    }
}

/**
 * Funcao que remove o elemento da fila, 
 * realizando o reposicionamento
 */
int removeFila(tipo_fila *fl) {
    int valor, i;
    if (filaVazia(fl)) {
        printf("[ERRO] Fila vazia, nao eh possivel remover valor.\n");
        return -1; //Retorna -1 como sinal de erro
    } else {
        valor = fl->fila[0]; //Guarda o valor a ser retornado
        //Laco para reposicionamento dos valores
        for (i=0; i<(fl->qtd_elem-1); i++) {
            fl->fila[i] = fl->fila[i+1];
        }
        fl->qtd_elem--;
        return valor;
    }
}

/**
 * Retorna primeiro elemento da fila, sem remove-lo
 */
int primeiroFila(tipo_fila *fl) {
    if (filaVazia(fl)) { //Verifica se fila esta vazia
        return -1; //Neste caso, estou optando por retornar -1 como sinal de erro
    } else {
        return fl->fila[0];
    }
}

/**
 * Imprime os elementos da fila. 
 * O principal objetivo desta funcao eh para debugacao.
 */
void imprimeFila(tipo_fila *fl) {
    int i;
    printf("Qtd_elems: %d\n", fl->qtd_elem);
    printf("Fila: \nSaida <- [ ");
    for (i=0; i<fl->qtd_elem; i++) {
        printf("%d ", fl->fila[i]);
    }
    printf("] <- Entrada\n\n");
}
