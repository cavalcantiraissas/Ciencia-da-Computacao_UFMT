/***
 ** UFMT - Ciência da Computação
 ** ED1 - Prof. Ivairton
 **
 ** Exercício de vetor usando C
 ***/

//Declaração de bibliotecas
#include<stdio.h>
#include<stdlib.h>

//Definição de uma constante
#define N 10

//Prototipo de funções
float media(int vetor[]);
int maior(int vetor[]);
int menor(int vetor[]);

//Função principal
int main() {
    int i;
    int vetor[N];

    //Leitura dos valores
    for (i=0; i<N; i++) {
        printf("Informe o valor %d: ", i+1);
        scanf("%d", &vetor[i]);
    }

    //Imprime os valores lidos
    printf("Valores: ");
    for (i=0; i<N; i++) {
        printf("[%d]", vetor[i]);
    }
    printf("\n");

    //Média
    printf("A média entre os valores é: %.2f\n", media(vetor) );

    //Maior valor
    printf("O maior valor presente no vetor é: %d\n", maior(vetor) );

    //Menor valor
    printf("O menor valor presente no vetor é: %d\n", menor(vetor) );

    return 1;
}

//Função que calcula a média entre os valores presentes no vetor
float media(int vetor[]) {
    int total, i;
    float vlMedia;
    total = 0;
    for (i=0; i<N; i++) {
        total = total + vetor[i];
    }
    vlMedia = (float) total / (float) N;
    return vlMedia;
}

//Funcao que encontra o maior valor presente no vetor
int maior(int vetor[]) {
    int maior, i;
    maior = vetor[0];
    for (i=1; i<N; i++) {
        if (vetor[i] > maior){
            maior = vetor[i];
        }
    }
    return maior;
}

//Função que encontra o menor valor presente no vetor
int menor(int vetor[]) {
    int menor, i;
    menor = vetor[0];
    for (i=1; i<N; i++) {
        if (vetor[i] < menor){
            menor = vetor[i];
        }
    }
    return menor;
}
