/*
Universidade Federal de Mato Grosso
Disciplina: Estrutura de dados 1
Lista de Exercícios sobre C
Aluna: Raissa Cavalcanti

Exercício 02: Leitura de 4 letras e definicao da maior letra
*/

#include <stdio.h>

int main() {
    char letras[4];
    char menor, maior;

    printf("Digite 4 letras:\n");
    for (int i = 0; i < 4; i++) {
        scanf(" %c", &letras[i]);
    }

    menor = maior = letras[0];

    for (int i = 1; i < 4; i++) {
        if (letras[i] < menor) {
            menor = letras[i];
        }
        if (letras[i] > maior) {
            maior = letras[i];
        }
    }

    printf("Menor letra: %c\n", menor);
    printf("Maior letra: %c\n", maior);

    return 0;
}
