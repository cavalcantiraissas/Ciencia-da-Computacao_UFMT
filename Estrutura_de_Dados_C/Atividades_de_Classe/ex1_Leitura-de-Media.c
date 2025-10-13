/*
Universidade Federal de Mato Grosso
Disciplina: Estrutura de dados 1
Lista de Exercícios sobre C
Aluna: Raissa Cavalcanti

Exercício 01: Leitura de média e comparação com nota
*/

#include <stdio.h>

int main(){
    float nota_1, nota_2, nota_3, media; 

    printf("Digite a sua primeira nota:\n");
    scanf("%f", &nota_1);

    printf("Digite a sua segunda nota:\n");
    scanf("%f", &nota_2);

    printf("Digite a sua terceira nota:\n");
    scanf("%f", &nota_3);

    media = ((nota_1 + nota_2 + nota_3) /3 );

    // Considerando que não há notas iguais

    if (media == nota_1){
        printf("Média é igual a nota da sua primeira prova."); 
    }
    else if (media == nota_2){
     printf("Média é igual a nota da sua segunda prova.");
    }

    else if (media == nota_3){
     printf("Média é igual a nota da sua terceira prova.");
    }

    else{
     printf("Sua média não é igual a nenhuma nota.");
    }
}
