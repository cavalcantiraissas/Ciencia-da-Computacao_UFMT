/**
 * UFMT - Bacharelado em Ciencia da Computacao
 * Prof. Ivairton
 *
 * Exercicio com manipulacao de registros.
 */

#include<stdio.h>
#include<stdlib.h>

// Definicao de constantes
#define APROVADO 1
#define REPROVADO 0

// Definições de tipos
struct estrutura_aluno {
    char nome[100];
    float n1;
    float n2;
    float mf;
    int status;
};
typedef struct estrutura_aluno tipo_aluno;

//Prototipo de funcao
float mediaTurma(tipo_aluno *turma, int qtd);
void imprimeAluno(tipo_aluno aluno);

//Funcao principal
int main(int argc, char *argv[]) {
    int i, qtd_alunos;

    printf("Informe a qtd de alunos a serem processados: ");
    scanf("%d", &qtd_alunos);

    //Declara e aloca estrutura para registrar dados dos alunos
    tipo_aluno *vet_alunos;
    vet_alunos = (tipo_aluno*) malloc(sizeof(tipo_aluno)*qtd_alunos);
    
    //Carrega dados dos alunos
    for (i=0; i<qtd_alunos; i++) {
        printf("ALUNO %d:\n", i+1);
        printf("Nome: ");
        scanf("%s", vet_alunos[i].nome);
        printf("Nota 1: ");
        scanf("%f", &vet_alunos[i].n1);
        printf("Nota 2: ");
        scanf("%f", &vet_alunos[i].n2);
    }

    //Calcula media e situacao final do aluno
    for (i=0; i<qtd_alunos; i++) {
        vet_alunos[i].mf = (vet_alunos[i].n1 + vet_alunos[i].n2) / 2;
        if (vet_alunos[i].mf < 5)
            vet_alunos[i].status = REPROVADO;
        else
            vet_alunos[i].status = APROVADO;
    }

    //Imprime dados
    printf("Media da turma = %.2f\n\n", mediaTurma(vet_alunos,qtd_alunos));
    for (i=0; i<qtd_alunos; i++)
        imprimeAluno(vet_alunos[i]);
        
    return 1;
}

/**
 * Funcao que calcula a media da nota final da turma
 */
float mediaTurma(tipo_aluno *turma, int qtd) {
    int i;
    float total;

    total = 0.0;
    for (i=0; i<qtd; i++) {
        total = total + turma[i].mf;
    }
    return ( total / (float) qtd);
}

/**
 * Imprime os dados de um aluno
 */
void imprimeAluno(tipo_aluno aluno) {
    printf("Aluno: %s\n", aluno.nome);
    printf("\tNota1: %.2f\n", aluno.n1);
    printf("\tNota2: %.2f\n", aluno.n2);
    printf("\tMedia Final: %.2f\n", aluno.mf);
    if (aluno.status == APROVADO)
        printf("\tSituacao: APROVADO\n\n");
    else
        printf("\tSituacao: REPROVADO\n\n");
}
