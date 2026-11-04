# Exemplos de Código: Estrutura de Dados I

**Exemplos de referência das aulas de Estrutura de Dados I: estruturas lineares, árvores, heap, tabela hash e fundamentos de C.**

[![C](https://img.shields.io/badge/C-gcc-A8B9CC?logo=c&logoColor=white)](https://gcc.gnu.org/)
[![Disciplina](https://img.shields.io/badge/disciplina-Estrutura_de_Dados_I-6f42c1)](../)

Códigos usados como base de consulta nas aulas de Estrutura de Dados I (UFMT, prof. Ivairton M. Santos). Parte deles é programa completo; parte são módulos de funções sem `main`, ou trechos escritos em pseudocódigo para estudo, como indicado na tabela.

---

## Conteúdo

### Fundamentos

| Arquivo | Conteúdo | Situação |
|---|---|---|
| [Ponteiros.c](Ponteiros.c) | Ponteiros, aritmética de ponteiros e passagem por referência | Compila |
| [Manipulando_vetor.c](Manipulando_vetor.c) | Operações sobre vetores | Compila |
| [Manipulando_Matriz.c](Manipulando_Matriz.c) | Operações sobre matrizes | Compila |
| [Manipulacao_Registros.c](Manipulacao_Registros.c) | Registros (`struct`) | Compila |
| [Geracao-de-numeros-pseudoaleatorios.c](Geracao-de-numeros-pseudoaleatorios.c) | Geração de números pseudoaleatórios | Compila |

### Estruturas lineares

| Arquivo | Conteúdo | Situação |
|---|---|---|
| [Implementacao_PILHA.c](Implementacao_PILHA.c) | Pilha em vetor | Compila |
| [Implementacao_FILA.c](Implementacao_FILA.c) | Fila em vetor | Compila |
| [Pilha_com_Encadeamento.c](Pilha_com_Encadeamento.c) | Pilha encadeada | Módulo sem `main` |
| [Fila-com-encadeamento.c](Fila-com-encadeamento.c) | Fila encadeada | Módulo sem `main` |
| [Lista_com_alocacao_dinamica.c](Lista_com_alocacao_dinamica.c) | Lista simplesmente encadeada | Pseudocódigo |
| [Lista_duplamente-encadeada.c](Lista_duplamente-encadeada.c) | Lista duplamente encadeada | Pseudocódigo |
| [Lista_circular_alocacao_dinamica.c](Lista_circular_alocacao_dinamica.c) | Lista circular | Pseudocódigo |

### Estruturas não lineares

| Arquivo | Conteúdo | Situação |
|---|---|---|
| [Arvore_Binaria.c](Arvore_Binaria.c) | Árvore binária de busca | Módulo sem `main` |
| [Arvore_com_multiplos_filhos.c](Arvore_com_multiplos_filhos.c) | Árvore com número variável de filhos | Compila |
| [Heap.c](Heap.c) | Heap | Compila |
| [Tabela_Hash.c](Tabela_Hash.c) | Tabela hash com função `k % 10`, inserção, busca e impressão; colisões tratadas por encadeamento | Módulo sem `main` |
| [listaEnc.c](listaEnc.c) | Lista encadeada usada pela tabela hash: inserção no início e no fim, remoção, busca, impressão e liberação | Módulo sem `main` |
| [teste_Tabela_Hash.c](teste_Tabela_Hash.c) | Programa de teste da tabela hash, com chaves que colidem e buscas | Compila |

## Como compilar

```bash
gcc Implementacao_PILHA.c -o pilha
./pilha

gcc teste_Tabela_Hash.c -o tabela_hash   # inclui Tabela_Hash.c e listaEnc.c
./tabela_hash
```

Os módulos sem `main` podem ser incluídos num programa de teste próprio.

---

[← Estrutura de Dados em C](../) · [Repositório principal](../../)
