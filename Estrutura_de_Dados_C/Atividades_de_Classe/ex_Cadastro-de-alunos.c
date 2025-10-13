/*
Federal University of Mato Grosso
Course: Data Structures 1
Exercise List on C
Student: Raissa Cavalcanti

The C program should register up to 40 students with name, RA, grades 1 and 2, 
and calculate the final average. It should then display the number of students approved 
(average ≥ 6), failed, and the average of all final averages.

*/ 

#include <stdio.h>

#define MAX_ALUNOS 40

typedef struct {
    char nome[100];
    int RA;
    float nota1, nota2, mediaFinal;
} Aluno;

int main() {
    int n, aprovados = 0, reprovados = 0;
    float somaMedias = 0.0;
    Aluno alunos[MAX_ALUNOS];
    
    printf("Quantos alunos você deseja cadastrar? (Máximo: %d): ", MAX_ALUNOS);
    scanf("%d", &n);
    
    for (int i = 0; i < n; i++) {
        printf("\nAluno %d\n", i+1);
        printf("Nome: ");
        getchar(); 
        fgets(alunos[i].nome, sizeof(alunos[i].nome), stdin);
        printf("RA: ");
        scanf("%d", &alunos[i].RA);
        printf("Nota 1: ");
        scanf("%f", &alunos[i].nota1);
        printf("Nota 2: ");
        scanf("%f", &alunos[i].nota2);
        
        alunos[i].mediaFinal = (alunos[i].nota1 + alunos[i].nota2) / 2;
        somaMedias += alunos[i].mediaFinal;
        
        if (alunos[i].mediaFinal >= 6.0) {
            aprovados++;
        } else {
            reprovados++;
        }
    }
    
    printf("\nResumo da Turma:\n");
    printf("Total de aprovados: %d\n", aprovados);
    printf("Total de reprovados: %d\n", reprovados);
    printf("Média das médias finais: %.2f\n", somaMedias / n);
    
    return 0;
}
