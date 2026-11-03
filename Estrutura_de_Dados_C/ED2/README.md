# Estrutura de Dados II

**Árvores balanceadas, matrizes esparsas e algoritmos de grafos (caminho mínimo e árvore geradora mínima) implementados em C.**

[![C](https://img.shields.io/badge/C-gcc-A8B9CC?logo=c&logoColor=white)](https://gcc.gnu.org/)
[![Disciplina](https://img.shields.io/badge/disciplina-Estrutura_de_Dados_II-6f42c1)](../)

Atividades e trabalhos da disciplina de Estrutura de Dados II (UFMT), continuação de Estrutura de Dados I. O foco são estruturas não lineares e algoritmos sobre grafos, com medição de tempo e custo nas execuções.

---

## Trabalhos

| Trabalho | Descrição |
|---|---|
| [Árvores binárias balanceadas](./Tarefa1_Arvores_Binarias_Balanceadas) | ABB, AVL e Rubro-Negra comparadas em altura, número de rotações e custo de busca, com relatório |

## Atividades

| Arquivo | O que implementa | Entrada |
|---|---|---|
| [Dijkstra.c](Atividades/Dijkstra.c) | Caminho mínimo de Dijkstra sobre lista de adjacência | `teste.gr`; grava `resultado.txt` |
| [Bellman-Ford.c](Atividades/Bellman-Ford.c) | Caminho mínimo de Bellman-Ford, ao lado do Dijkstra para comparação | `teste.gr`; grava `resultado.txt` |
| [PRIM.c](Atividades/PRIM.c) | Árvore geradora mínima de Prim com fila de prioridade (heap mínimo) | Grafos rodoviários `USA-road-d.CAL.gr` e `USA-road-d.NY.gr` |
| [Kruskal.c](Atividades/Kruskal.c) | Árvore geradora mínima de Kruskal, com custo total e tempo de execução | `./kruskal <grafo> <saida>` |
| [Matriz-Esparsa.c](Atividades/Matriz-Esparsa.c) | Matriz esparsa com listas encadeadas por linha: inserção, consulta, exclusão e impressão | Entrada padrão |
| [Matriz-Esparsa-2.c](Atividades/Matriz-Esparsa-2.c) | Multiplicação de matriz esparsa por escalar | Entrada padrão |
| [Matriz-Esparsa-3.c](Atividades/Matriz-Esparsa-3.c), [-4.c](Atividades/Matriz-Esparsa-4.c) | Soma de matrizes esparsas | Entrada padrão |

Os arquivos de grafo (`.gr`, formato do 9º DIMACS Implementation Challenge) não estão no repositório. Os grafos rodoviários podem ser baixados na [página do desafio](http://www.diag.uniroma1.it/challenge9/download.shtml).

## Como compilar

```bash
cd Atividades
gcc Kruskal.c -o kruskal
./kruskal grafo.gr resultado.txt
```

## Conteúdo da disciplina

- Árvores binárias de busca e árvores balanceadas (AVL e Rubro-Negra)
- Árvores B e variantes
- Grafos: representação e percursos (BFS e DFS)
- Árvore geradora mínima: Prim e Kruskal
- Caminho mínimo: Dijkstra e Bellman-Ford
- Matrizes esparsas

---

[← Estrutura de Dados em C](../) · [Repositório principal](../../)
