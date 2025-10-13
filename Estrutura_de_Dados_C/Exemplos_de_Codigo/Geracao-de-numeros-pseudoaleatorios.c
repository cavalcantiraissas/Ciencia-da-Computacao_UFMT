/**
 * UFMT - Ciencia da Computacao
 * ED1 - Prof. Ivairton
 *
 * Exemplo de codigo que gera numeros pseudo aleatorios
 */

#include<stdio.h>
#include<stdlib.h>
#include<time.h>

int main() {
    int i;
    time_t t; //Usado na geração da semente:

    //Inicializa o gerador de números randomicos, a partir de uma semente
    srand( (unsigned) time(&t) ); //Inicializa com base em uma semente extraida do clock do PC: MANEIRA APROPRIADA
    //srand(1); //Inicializa com base em uma semente conhecida, a sequencia de sorteio será sempre igual

    //Sorteia 3 números entre 0 e 0
    i = rand() % 10; //O resto da divisao por 10 garante valores entre 0 e 9, ja que rand() devolve um valor de no máximo RAND_MAX, definido pela biblioteca stdlib (de pelo menos 32767)
    printf("%d\n", i);

    //segundo sorteio
    i = rand() % 10;
    printf("%d\n", i);

    //terceiro sorteio
    i = rand() % 10;
    printf("%d\n", i);

    return 1;
}
