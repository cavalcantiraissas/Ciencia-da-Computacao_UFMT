/***
 ** UFMT - Ciência da Computação
 ** ED1 - Prof. Ivairton
 **
 ** Exercício de matriz usando C
 ***/

//Declaração de bibliotecas
#include<stdio.h>
#include<stdlib.h>

//Definição de uma constante
#define M 4
#define N 3

//Prototipo de funções
float mediaDasMedias(float mtz[][N]);
float melhorMedia(float mtz[][N]);
float melhorNota(float mtz[][N]);
float piorNota(float mtz[][N]);


//Função principal
int main() {
    int i,j;
    float notas[M][N];

    //Leitura dos valores
    for (i=0; i<M; i++) {
        printf("Aluno %d:\n", i+1);
        printf("\tInforme a Nota1: ");
        scanf("%f", &notas[i][0] );
        printf("\tInforme a Nota2: ");
        scanf("%f", &notas[i][1] );
    }

    //Calculo das médias de cada aluno
    for (i=0; i<M; i++) {
        notas[i][2] = (notas[i][0] + notas[i][1]) / 2;
    }

    //Imprime as notas e médias
    printf("Planilha de notas:\n");
    for (i=0; i<M; i++) {
        printf("\tAluno %d: Nota1=%.1f; Nota2=%.1f; MF=%.1f\n", i+1, notas[i][0],notas[i][1],notas[i][2]);
    }

    //Média
    printf("A média das médias finais é: %.2f\n", mediaDasMedias(notas) );
    //Maior media
    printf("A maior média da sala é: %.2f\n", melhorMedia(notas) );
    //Melhor nota
    printf("A melhor nota da sala é: %.2f\n", melhorNota(notas) );
    //Pior nota
    printf("A pior nota da sala é: %.2f\n", piorNota(notas) );

    return 1;
}

//Função que calcula a média entre as médias finais da turma
float mediaDasMedias(float mtz[][N]) {
    int i;
    float soma;
    soma = 0;
    for (i=0; i<M; i++) {
        soma = soma + mtz[i][2];
    }
    return soma / M;
}

//Função que encontra a melhor média final da turma
float melhorMedia(float mtz[][N]) {
    int i;
    float maior;
    maior = mtz[0][2];
    for (i=1; i<M; i++) {
        if (maior < mtz[i][2]) {
            maior = mtz[i][2];
        }
    }
    return maior;
}

//Função que encontra e melhor nota da sala
float melhorNota(float mtz[][N]) {
    int i, j;
    float vlNota;
    vlNota = mtz[0][0];
    for (i=0; i<M; i++) {
        for (j=0; j<N; j++){
            if (vlNota < mtz[i][j]) {
                vlNota = mtz[i][j];
            }
        }
    }
    return vlNota;
}

//Função que encontra a pior nota da sala
float piorNota(float mtz[][N]) {
    int i, j;
    float vlNota;
    vlNota = mtz[0][0];
    for (i=0; i<M; i++) {
        for (j=0; j<N; j++){
            if (vlNota > mtz[i][j]) {
                vlNota = mtz[i][j];
            }
        }
    }
    return vlNota;
}
