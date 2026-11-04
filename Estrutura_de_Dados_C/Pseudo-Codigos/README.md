# Algoritmos de Ordenação em Pseudocódigo

**Pseudocódigo comentado dos principais algoritmos de ordenação estudados em Estrutura de Dados I, com a complexidade de cada um.**

[![Pseudocódigo](https://img.shields.io/badge/formato-pseudoc%C3%B3digo-555555)](#algoritmos)
[![Disciplina](https://img.shields.io/badge/disciplina-Estrutura_de_Dados_I-6f42c1)](../)

Material de estudo da disciplina de Estrutura de Dados I (UFMT). Os arquivos têm extensão `.c` para facilitar o realce de sintaxe, mas são **pseudocódigo** com comentários sobre as estratégias de cada algoritmo, e não compilam. Implementações em C de Merge Sort e de heap estão em [Atividades_de_Classe](../Atividades_de_Classe).

---

## Algoritmos

| Arquivo | Algoritmo | Caso médio | Pior caso | Estável |
|---|---|---|---|---|
| [Bubble-Sort.c](Bubble-Sort.c) | Bubble Sort | O(n²) | O(n²) | Sim |
| [Insert-Sort.c](Insert-Sort.c) | Insertion Sort | O(n²) | O(n²) | Sim |
| [Shell-Sort.c](Shell-Sort.c) | Shell Sort | depende da sequência de saltos | O(n²) | Não |
| [Merge-Sort.c](Merge-Sort.c) | Merge Sort | O(n log n) | O(n log n) | Sim |
| [Heap-Sort.c](Heap-Sort.c) | Heap Sort | O(n log n) | O(n log n) | Não |
| [Quick-Sort_Recursivo.c](Quick-Sort_Recursivo.c) | Quick Sort com partição de Hoare (pivô no meio, dois índices que se cruzam) | O(n log n) | O(n²) | Não |
| [Quick-Sort_Iterativo.c](Quick-Sort_Iterativo.c) | Quick Sort com função de partição separada (Lomuto, pivô no fim) | O(n log n) | O(n²) | Não |

Os dois arquivos de Quick Sort são recursivos e diferem na forma de particionar; ambos comentam alternativas para a escolha do pivô. O de Insertion Sort compara duas estratégias de inserção.

---

[← Estrutura de Dados em C](../) · [Repositório principal](../../)
