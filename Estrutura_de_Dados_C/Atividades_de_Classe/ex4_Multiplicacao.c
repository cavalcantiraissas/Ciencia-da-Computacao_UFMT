/*
Universidade Federal de Mato Grosso
Disciplina: Estrutura de dados 1
Lista de Exercícios sobre C
Aluna: Raissa Cavalcanti

Exercício 04
*/

#include <stdio.h>

int main() {
    int valor1, valor2, resultado = 0;

    printf("Digite o primeiro valor para ser multiplicado: ");
    scanf("%d", &valor1);

    printf("Digite o segundo valor para multiplicar: ");
    scanf("%d", &valor2);

    for (int i = 0; i < valor2; i++) {
        resultado += valor1;
    }
    printf("Resultado de %d multiplicado por %d é: %d\n", valor1, valor2, resultado);

    return 0;
}
