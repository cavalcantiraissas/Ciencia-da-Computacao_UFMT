# Sistemas Operacionais I

**Princípios e mecanismos dos sistemas operacionais modernos, com ênfase em concorrência: threads, exclusão mútua, variáveis de condição e o problema do produtor-consumidor na prática.**

[![C](https://img.shields.io/badge/C-pthreads-A8B9CC?logo=c&logoColor=white)](https://man7.org/linux/man-pages/man7/pthreads.7.html)
[![Python](https://img.shields.io/badge/Python-benchmarks-3776AB?logo=python&logoColor=white)](https://www.python.org/)
[![UFMT](https://img.shields.io/badge/UFMT-Ci%C3%AAncia_da_Computa%C3%A7%C3%A3o-003366)](../)

Trabalhos da disciplina de Sistemas Operacionais I do curso de Ciência da Computação da UFMT. O objetivo é entender a interface entre software e hardware e as abstrações que o sistema operacional oferece aos programas.

| | |
|---|---|
| **Docente** | Maxweel Silva Carmo |
| **Período** | 2026/1 |
| **Carga horária** | 64 horas |

---

## Trabalhos

| Trabalho | Descrição | Tecnologias |
|---|---|---|
| [opzip](./opzip) | Compressor RLE paralelo: threads produtoras comprimem blocos e uma escritora grava em ordem por um buffer circular limitado; inclui experimentos de *speedup*, tamanho do buffer e granularidade dos mutexes | C, pthreads, ThreadSanitizer, Python |

## Conteúdo da disciplina

- Processos e threads: comunicação, sincronização e escalonamento
- Gerenciamento de memória: paginação, memória virtual e algoritmos de substituição
- Sistemas de arquivos e gerenciamento de entrada e saída
- Concorrência e problemas clássicos
- Chamadas de sistema e programas concorrentes

## Bibliografia

- TANENBAUM, Andrew S. *Sistemas Operacionais Modernos*.
- TANENBAUM, Andrew S.; WOODHULL, Albert S. *Operating Systems: Design and Implementation*.
- OLIVEIRA, Rômulo Silva de; CARISSIMI, Alexandre; TOSCANI, Simão. *Sistemas Operacionais*.
- ARPACI-DUSSEAU, Remzi H.; ARPACI-DUSSEAU, Andrea C. *Operating Systems: Three Easy Pieces*.

---

[← Repositório principal](../)
