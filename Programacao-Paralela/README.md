# Programação Paralela

**Programas concorrentes e paralelos em Java, Python e C: monitores, semáforos, pools de threads, filas thread-safe e OpenMP.**

[![Java](https://img.shields.io/badge/Java-17%2B-ED8B00?logo=openjdk&logoColor=white)](https://openjdk.org/)
[![Python](https://img.shields.io/badge/Python-3-3776AB?logo=python&logoColor=white)](https://www.python.org/)
[![OpenMP](https://img.shields.io/badge/C-OpenMP-A8B9CC?logo=c&logoColor=white)](https://www.openmp.org/)
[![UFMT](https://img.shields.io/badge/UFMT-Ci%C3%AAncia_da_Computa%C3%A7%C3%A3o-003366)](../)

Trabalhos da disciplina de Programação Paralela do curso de Ciência da Computação da UFMT (prof. Anthony Ferreira La Marca). Com processadores de muitos núcleos em todo computador, programar de forma paralela é o caminho para reduzir o tempo de solução e atacar problemas maiores; os exercícios cobrem os mecanismos de coordenação entre threads e o paralelismo de dados com OpenMP.

---

## Trabalhos

| Trabalho | Descrição | Mecanismo |
|---|---|---|
| [Armazém](./Armazem) | Produtor-consumidor sobre um armazém de capacidade limitada | Monitor: `synchronized`, `wait`, `notifyAll` |
| [Linha de montagem](./TestLinhaMontagem) | Pipeline de três etapas ligadas por duas estações limitadas | `Semaphore` (mutex, vagas e itens) |
| [NotificationSystem](./NotificationSystem) | 40 notificações num pool de threads com fila limitada e rejeição personalizada | `ThreadPoolExecutor`, `ScheduledExecutorService` |
| [simulador_ingressos.py](simulador_ingressos.py) | 10 compradores concorrentes disputam 20 ingressos; uma thread de log recebe os eventos e o relatório final confere a consistência do estoque | `threading.Lock`, `queue.Queue` |
| [MaxVetor.c](MaxVetor.c) | Máximo de um vetor: máximo local por thread e uma única atualização do global em região crítica, comparado com `reduction(max:...)` e com a versão sequencial | OpenMP `critical` e `reduction` |
| [soma_arvore.c](soma_arvore.c) | Soma dos nós de uma árvore binária de busca, com uma tarefa por subárvore e conferência sequencial | OpenMP `task` e `taskwait` |

## Como executar

```bash
# Java (17+)
cd Armazem && javac *.java && java TestArmazem

# Python 3
python3 simulador_ingressos.py

# C com OpenMP: N elementos e número de threads são opcionais
gcc -fopenmp -O2 MaxVetor.c -o max_vetor && ./max_vetor 1000000 4
gcc -fopenmp -O2 soma_arvore.c -o soma_arvore && ./soma_arvore
```

No macOS, o `gcc` padrão é o clang, que não aceita `-fopenmp`. Instale o GCC do Homebrew (`brew install gcc`) e use `gcc-15` (ou a versão instalada) no lugar de `gcc`.

O Armazém e a Linha de montagem rodam indefinidamente; encerre com `Ctrl + C`.

## Conteúdo da disciplina

1. Avaliação de desempenho de programas paralelos
2. Programação paralela com memória compartilhada
3. Programação com passagem de mensagem
4. Programação para unidades de processamento gráfico (GPU)

---

[← Repositório principal](../)
