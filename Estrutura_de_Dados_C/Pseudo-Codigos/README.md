# Algoritmos de Ordenação - Implementações em C

Implementações dos principais algoritmos de ordenação estudados na disciplina de Estrutura de Dados I. Cada arquivo contém a lógica completa do algoritmo com entrada e saída via terminal, adequado para análise de comportamento e complexidade.

---

## Algoritmos Implementados

| Arquivo | Algoritmo | Complexidade Média | Complexidade Pior Caso | Estável |
|---|---|---|---|---|
| `Bubble-Sort.c` | Bubble Sort | O(n²) | O(n²) | Sim |
| `Insert-Sort.c` | Insertion Sort | O(n²) | O(n²) | Sim |
| `Shell-Sort.c` | Shell Sort | O(n log n) | O(n²) | Não |
| `Merge-Sort.c` | Merge Sort | O(n log n) | O(n log n) | Sim |
| `Heap-Sort.c` | Heap Sort | O(n log n) | O(n log n) | Não |
| `Quick-Sort_Recursivo.c` | Quick Sort Recursivo | O(n log n) | O(n²) | Não |
| `Quick-Sort_Iterativo.c` | Quick Sort Iterativo | O(n log n) | O(n²) | Não |

---

## Como Compilar e Executar

```bash
gcc Merge-Sort.c -o mergesort
./mergesort
```

---

[Voltar para Estrutura de Dados em C](../README.md)
