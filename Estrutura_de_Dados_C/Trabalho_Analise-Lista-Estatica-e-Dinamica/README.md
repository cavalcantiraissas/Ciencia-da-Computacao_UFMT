# Análise de Listas: Estática, Dinâmica e Duplamente Encadeada

**Comparação prática entre três implementações de lista em C, com a mesma carga de inserções aleatórias e medição do tempo de execução.**

[![C](https://img.shields.io/badge/C-gcc-A8B9CC?logo=c&logoColor=white)](https://gcc.gnu.org/)
[![Relatório](https://img.shields.io/badge/relat%C3%B3rio-PDF-B30B00)](Relat%C3%B3rio_Trab.pdf)
[![Disciplina](https://img.shields.io/badge/disciplina-Estrutura_de_Dados_I-6f42c1)](../)

Trabalho da disciplina de Estrutura de Dados I (UFMT). O objetivo é evidenciar as diferenças de comportamento, as vantagens e as limitações da alocação estática (vetor de tamanho fixo) e da alocação dinâmica (nós encadeados), em uso de memória e em desempenho das operações.

---

## Implementações

| Lista | Implementação | Programa de teste |
|---|---|---|
| Estática, em vetor | [lista_estatica.c](lista_estatica.c) | [main_estatica.c](main_estatica.c) |
| Simplesmente encadeada | [lista_dinamica.c](lista_dinamica.c) | [main_dinamica.c](main_dinamica.c) |
| Duplamente encadeada | [lista_duplamente_encadeada.c](lista_duplamente_encadeada.c) | [main_duplamente_encadeada.c](main_duplamente_encadeada.c) |

Cada programa de teste insere valores aleatórios no início, no fim e em posições sorteadas, e mede o tempo total com `clock()`.

## Como executar

Os programas de teste incluem a implementação com `#include`, então basta compilar o `main`:

```bash
gcc main_estatica.c -o estatica && ./estatica
gcc main_dinamica.c -o dinamica && ./dinamica
gcc main_duplamente_encadeada.c -o dupla && ./dupla
```

## Documentação

| Arquivo | Conteúdo |
|---|---|
| [2021-2-CC-Trab06.pdf](2021-2-CC-Trab06.pdf) | Enunciado do trabalho |
| [Relatório_Trab.pdf](Relat%C3%B3rio_Trab.pdf) | Relatório com a análise comparativa |

---

[← Estrutura de Dados em C](../) · [Repositório principal](../../)
