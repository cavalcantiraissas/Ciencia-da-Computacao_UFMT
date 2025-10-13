/*
Universidade Federal de Mato Grosso
Disciplina: Estrutura de Dados I
Alunos: Raissa Cavalcanti (RGA:202311722030) e Murilo Becker (RGA: 202411722030)

Teste de Lista dinamica

*/

#include<stdio.h>
#include<stdlib.h>
#include"lista_dinamica.c"


int main() {
    int posicao, numeros, tamanho;   
     clock_t tempo;
     tempo = clock();
     printf("Qual o tamanho da lista?\n");
     scanf("%d", &tamanho);

    Lista lista; 

     InicializarLista(&lista);   
      for (int i = 0; i < 3333; i++){ 
    numeros =  rand() %99999;
       InsereInicioLista(&lista, numeros);
    }
  for (int j = 0; j < 3334; j++){ 
     numeros =  rand() % 99999; 
         InsereFimLista(&lista, numeros);
     }
       for (int k = 0; k < 3333; k++){ 
     numeros =  rand() % 99999;
     posicao = rand() % (lista.tam + 1);
          InserePosicaoLista(&lista, numeros, posicao); 
          
       }

tempo = clock() - tempo; 

   ImprimeLista(lista);
    
 printf("\nTempo de execução: %lf", ((double)tempo)/((CLOCKS_PER_SEC)));


LiberaLista(&lista);

    return 0;
}
