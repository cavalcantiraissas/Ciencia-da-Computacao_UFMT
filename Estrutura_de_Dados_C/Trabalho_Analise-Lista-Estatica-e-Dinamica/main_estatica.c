/*
Universidade Federal de Mato Grosso
Disciplina: Estrutura de Dados I
Alunos: Raissa Cavalcanti (RGA:202311722030) e Murilo Becker( RGA:202411722030)

Teste de Lista Estática 
*/



#include<stdio.h>
#include<stdlib.h>
#include"lista_estatica.c"

int main() {
    int posicao, numeros; 
  
     clock_t tempo;
     tempo = clock(); 
  
    tipoLista lista; 
    InicializaLista(&lista, tam); 
      for (int i = 0; i < 3333; i++){ 
    numeros =  rand() %99999; 
       InsereInicio(&lista, numeros);
    }
  for (int j = 0; j < 3334; j++){ 
     numeros =  rand() % 99999; 
         InsereFim(&lista, numeros);
     }
       for (int k = 0; k < 3333; k++){
     numeros =  rand() % 99999;
     posicao = rand() % (lista.elementos + 1);
          InserePos(&lista, numeros, posicao); 
          
       }

tempo = clock() - tempo; 


   ImprimeLista(lista);
    
 printf("\nTempo de execucao: %lf", ((double)tempo)/((CLOCKS_PER_SEC)));
  getchar ( );

  LiberaLista(&lista);

    return 0;
}
