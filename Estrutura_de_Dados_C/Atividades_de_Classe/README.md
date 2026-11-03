# Atividades de Classe: Estrutura de Dados I

**Exercícios introdutórios de C e implementações das estruturas lineares estudadas em sala: listas, filas, pilhas e heap.**

[![C](https://img.shields.io/badge/C-gcc-A8B9CC?logo=c&logoColor=white)](https://gcc.gnu.org/)
[![Disciplina](https://img.shields.io/badge/disciplina-Estrutura_de_Dados_I-6f42c1)](../)

Atividades feitas durante as aulas de Estrutura de Dados I (UFMT). As implementações seguem o padrão de módulo em C: um `.h` com a interface, um `.c` com a implementação e um `main.c` que exercita as operações.

---

## Estruturas implementadas

| Pasta | Estrutura | Operações |
|---|---|---|
| [Implementacao_Lista](./Implementacao_Lista) | Lista simplesmente encadeada | Inserção e remoção no início, no fim, por posição e por valor; busca por valor e por posição; contagem e impressão |
| [Insercao_Lista-Duplamente-Encadeada](./Insercao_Lista-Duplamente-Encadeada) | Lista duplamente encadeada e lista circular | Inserção e remoção no início, no fim e por posição; remoção por valor; rotação da lista circular; liberação da memória |
| [Fila_Pilha_Alocacao-Estatica](./Fila_Pilha_Alocacao-Estatica) | Fila e pilha em vetor | Enfileirar, desenfileirar, empilhar, desempilhar e imprimir |
| [Fila_Alocacao-Dinamica-Encadeada](./Fila_Alocacao-Dinamica-Encadeada) | Fila encadeada | Inserir, remover e imprimir |
| [Heap-Sort](./Heap-Sort) | Heap máximo (módulo, sem `main`) | Inserir, remover o maior, verificar se está vazio e destruir |

## Exercícios avulsos

| Arquivo | O que faz |
|---|---|
| [ex1_Leitura-de-Media.c](ex1_Leitura-de-Media.c) | Lê três notas, calcula a média e diz se ela é igual a alguma das notas |
| [ex2_Leitura-de-letras.c](ex2_Leitura-de-letras.c) | Lê quatro letras e mostra a menor |
| [ex3_Criacao-Sequencia](ex3_Criacao-Sequencia) | Lê dois valores e imprime os inteiros entre eles (código C, sem extensão `.c`) |
| [ex4_Multiplicacao.c](ex4_Multiplicacao.c) | Multiplica dois números por somas sucessivas |
| [ex5_Divisao-por-tres.c](ex5_Divisao-por-tres.c) | Mostra o quociente e o resto da divisão de um número por 3 |
| [ex_Cadastro-de-alunos.c](ex_Cadastro-de-alunos.c) | Cadastro de alunos com `struct` |
| [ex_Registros.c](ex_Registros.c) | Cadastro de produtos com preço de custo e de venda e cálculo do lucro |
| [Ordenacao_Merge-Sort.c](Ordenacao_Merge-Sort.c) | Ordenação por intercalação (Merge Sort) |

## Como compilar

```bash
# exercício avulso
gcc ex4_Multiplicacao.c -o ex4 && ./ex4

# o ex3 não tem extensão .c: informe a linguagem ao gcc
gcc -x c ex3_Criacao-Sequencia -o ex3 && ./ex3

# estrutura com vários arquivos
cd Implementacao_Lista
gcc *.c -o lista && ./lista
```

---

[← Estrutura de Dados em C](../) · [Repositório principal](../../)
