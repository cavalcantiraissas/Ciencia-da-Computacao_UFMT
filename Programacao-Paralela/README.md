# Programação Paralela

**Programas concorrentes e paralelos em Java, Python e C: monitores, semáforos, pools de threads, filas thread-safe, OpenMP e MPI.**

[![Java](https://img.shields.io/badge/Java-17%2B-ED8B00?logo=openjdk&logoColor=white)](https://openjdk.org/)
[![Python](https://img.shields.io/badge/Python-3-3776AB?logo=python&logoColor=white)](https://www.python.org/)
[![OpenMP](https://img.shields.io/badge/C-OpenMP-A8B9CC?logo=c&logoColor=white)](https://www.openmp.org/)
[![MPI](https://img.shields.io/badge/C-MPI-A8B9CC?logo=c&logoColor=white)](https://www.open-mpi.org/)
[![UFMT](https://img.shields.io/badge/UFMT-Ci%C3%AAncia_da_Computa%C3%A7%C3%A3o-003366)](../)

Trabalhos da disciplina de Programação Paralela do curso de Ciência da Computação da UFMT (prof. Anthony Ferreira La Marca). Com processadores de muitos núcleos em todo computador, programar de forma paralela é o caminho para reduzir o tempo de solução e atacar problemas maiores; os exercícios cobrem os mecanismos de coordenação entre threads o paralelismo de dados com OpenMP e a troca de mensagens entre processos com MPI.

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
| [caldeira_mpi.c](caldeira_mpi.c) | Pipeline de segurança de uma caldeira: sensor (rank 0) envia 100 leituras de 50 a 200 °C, o controlador (rank 1) abre a válvula de alívio acima de 100 °C e repassa o evento ao logger (rank 2), que grava console e `caldeira_log.txt` | MPI `Ssend` (síncrono) e `Bsend` com buffer pré-alocado |
| [estacao_clima_mpi.c](estacao_clima_mpi.c) | Estação meteorológica (rank 0) envia leituras de temperatura e umidade à central (rank 1), que preenche `alerta_ativo` com `'S'` em calor extremo e seco, tempestade/ar saturado ou geada, e devolve a struct; casos fixos conferem as fronteiras de cada condição | MPI tipo derivado: `Get_address`, `Type_create_struct`, `Type_commit`, `Type_free` |

## Como executar

```bash
# Java (17+)
cd Armazem && javac *.java && java TestArmazem

# Python 3
python3 simulador_ingressos.py

# C com OpenMP: N elementos e número de threads são opcionais
gcc -fopenmp -O2 MaxVetor.c -o max_vetor && ./max_vetor 1000000 4
gcc -fopenmp -O2 soma_arvore.c -o soma_arvore && ./soma_arvore

# C com MPI: exatamente 3 processos; leituras e intervalo (ms) são opcionais
mpicc -O2 caldeira_mpi.c -o caldeira_mpi && mpirun -np 3 ./caldeira_mpi
mpirun -np 3 ./caldeira_mpi 10 100   # teste rápido: 10 leituras a cada 100 ms

# C com MPI: exatamente 2 processos; o número de leituras aleatórias é opcional
mpicc -O2 estacao_clima_mpi.c -o estacao_clima_mpi && mpirun -np 2 ./estacao_clima_mpi 5
```

No macOS, o `gcc` padrão é o clang, que não aceita `-fopenmp`. Instale o GCC do Homebrew (`brew install gcc`) e use `gcc-15` (ou a versão instalada) no lugar de `gcc`. Para o MPI, `brew install open-mpi` fornece o `mpicc` e o `mpirun`.

O Armazém e a Linha de montagem rodam indefinidamente; encerre com `Ctrl + C`.

## Conteúdo da disciplina

1. Avaliação de desempenho de programas paralelos
2. Programação paralela com memória compartilhada
3. Programação com passagem de mensagem
4. Programação para unidades de processamento gráfico (GPU)

---

[← Repositório principal](../)
