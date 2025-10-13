/*
Universidade Federal de Mato Grosso
Disciplina: Estrutura de dados 1
Lista de Exercícios sobre C
Aluna: Raissa Cavalcanti

Exercício 05: Divisão por 3

*/

#include <stdio.h>

int main() {
    int valor, quociente, sobra;

    printf("Digite um número: ");
    scanf("%d", &valor);

    quociente = valor / 3;
    sobra = valor % 3;

    printf("O valor %d dividido por 3:\n", valor);
    printf("Quociente: %d\n", quociente);
    printf("Sobra: %d\n", sobra);

    return 0;
}
