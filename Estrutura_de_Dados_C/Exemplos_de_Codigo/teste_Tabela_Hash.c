/***
 ** UFMT - Ciencia da Computacao
 ** Estrutura de Dados I
 **
 ** Programa de teste da tabela hash (Tabela_Hash.c): insere chaves que
 ** colidem, imprime a tabela e faz buscas.
 **
 ** Compilar: gcc teste_Tabela_Hash.c -o tabela_hash
 **
 **/

#include<stdio.h>
#include "Tabela_Hash.c"

int main() {
 tipo_tab_hash tab;
 int chaves[] = {15, 25, 3, 42, 35, 7, 12, 99};
 int buscas[] = {25, 35, 8, 99};
 int i;

 inicializaTabHash(&tab);
 for (i = 0; i < 8; i++)
  insereTabHash(&tab, chaves[i]);

 // 15, 25 e 35 caem na posicao 5; 42 e 12, na posicao 2
 imprimeTabHash(&tab);

 for (i = 0; i < 4; i++)
  printf("Busca %d: %s\n", buscas[i], buscaTabHash(&tab, buscas[i]) ? "encontrado" : "nao encontrado");

 for (i = 0; i < N; i++)
  liberaLista(&tab.tabela[i]);
 return 0;
}
