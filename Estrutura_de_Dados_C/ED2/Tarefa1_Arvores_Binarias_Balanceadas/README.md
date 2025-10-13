# Objetivo do Trabalho
## O objetivo deste trabalho é implementar e analisar três tipos de árvores binárias de busca: Árvore Binária de Busca (Clássica), Árvore AVL e Árvore Rubro-Negra. O trabalho é dividido em quatro etapas principais:

1. **Geração de Dados:** Criar arquivos contendo números inteiros únicos e aleatórios, que serão utilizados como entradas para as árvores.
2. **Implementação das Árvores:** Desenvolver e testar as funções para construção, manipulação e análise das árvores binárias.
3. **Análise de Desempenho:** Avaliar a altura das árvores e o número de rotações executadas durante as inserções, comparando os resultados entre os diferentes tipos de árvores.
4. **Avaliação de Busca:** Selecionar aleatoriamente 20% dos valores gerados, realizar buscas na árvore e medir o tempo e o número de comparações necessárias para encontrar cada valor.


## Detalhamento das Etapas:

# **Etapa 1:** Geração de Dados
**Objetivo:** Gerar arquivos contendo números inteiros únicos no intervalo de 1 a 100.000, com duas quantidades de valores: 5.000 e 20.000.

## Código: gerar_numeros.c

### Como Executar:
1. Compile o código: gcc gerar_numeros.c -o gerar_numeros
2. Gere arquivos com 5.000 números: ./gerar_numeros 5000 arquivo_5000.txt
3. Gere arquivos com 20.000 números: ./gerar_numeros 20000 arquivo_20000.txt

# **Etapa 2:** Implementação das Árvores
**Objetivo:** Implementar as funções necessárias para construção, inserção, busca e análise de cada tipo de árvore binária.

1. Árvore Binária de Busca (ABB):
    * Código: abb.c
    * Funções: Inserção, busca, cálculo da altura, e percurso em ordem.
    * Contagem de Rotações: Não aplicável.
2. Árvore AVL:
    * Código: avl.c
    * Funções: Inserção com balanceamento, busca, cálculo da altura, e percurso em ordem.
    * Contagem de Rotações: Implementada.
3. Árvore Rubro-Negra (ARN):
    * Código: arn.c
    * Funções: Inserção com balanceamento, busca, cálculo da altura, e percurso em ordem.
    * Contagem de Rotações: Implementada.
  

### Como Executar:
1. Compile os códigos para cada tipo de árvore:
    * gcc abb.c -o abb
    * gcc avl.c -o avl
    * gcc arn.c -o arn
2. Teste cada árvore com um arquivo de entrada gerado:
    * ./abb arquivo_5000.txt
    * ./avl arquivo_5000.txt
    * ./arn arquivo_5000.txt

# **Etapa 3:** Análise de Desempenho
**Objetivo:** Carregar os arquivos gerados na Etapa 1 em cada tipo de árvore e contabilizar a altura das árvores e o número de rotações executadas durante a inserção dos valores.

### Código:
1. Árvore Binária de Busca (ABB): abb.c (modificado para mostrar a altura da árvore).
2. Árvore AVL: avl.c (modificado para contar rotações e mostrar a altura da árvore).
3. Árvore Rubro-Negra (ARN): arn.c (modificado para contar rotações e mostrar a altura da árvore).

### Como Executar:
1. Compile e execute os códigos como na Etapa 2, e verifique a saída para a altura da árvore e o número de rotações.

# **Etapa 4:** Avaliação de Busca
**Objetivo:** Selecionar aleatoriamente 20% dos valores gerados, buscar cada valor na árvore, e calcular o tempo e o número de comparações realizadas.

### Código:
1. Árvore Binária de Busca (ABB): O código já está implementado para a Etapa 4.
2. Árvore AVL: O código já está implementado para a Etapa 4.
3. Árvore Rubro-Negra (ARN): O código já está implementado para a Etapa 4.

### Como Executar:
1. Compile e execute os códigos modificados para ABB e AVL:
    * gcc abb.c -o abb
    * gcc avl.c -o avl
2. Execute cada árvore com os arquivos de entrada:
    * ./abb arquivo_5000.txt
    * ./avl arquivo_5000.txt
    * ./arn arquivo_5000.txt

# Como Executar 

## Prepare os Arquivos de Entrada: 
Use o script gerar_numeros.c para criar arquivos de entrada com 5.000 e 20.000 números, como entrada_5000.txt e entrada_20000.txt.

## Compile os Códigos 
- gcc gerar_numeros.c -o gerar_numeros
- gcc abb.c -o abb
- gcc avl.c -o avl
- gcc arn.c -o arn

## Gere os Arquivos de Entrada: 
Execute o programa gerar_numeros para criar os arquivos de entrada:
- ./gerar_numeros 5000
- ./gerar_numeros 20000

## Execute os Programas das Árvores: 
- ./abb
- ./avl
- ./arn

