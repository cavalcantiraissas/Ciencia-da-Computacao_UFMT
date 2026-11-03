# Ciência da Computação · UFMT

**Trabalhos, projetos e materiais de estudo do Bacharelado em Ciência da Computação da Universidade Federal de Mato Grosso, organizados por disciplina: de estruturas de dados e compiladores a sistemas operacionais, bancos de dados e programação paralela.**

[![C](https://img.shields.io/badge/C-A8B9CC?logo=c&logoColor=white)](#disciplinas)
[![C++](https://img.shields.io/badge/C%2B%2B-00599C?logo=cplusplus&logoColor=white)](#disciplinas)
[![Java](https://img.shields.io/badge/Java-ED8B00?logo=openjdk&logoColor=white)](#disciplinas)
[![Python](https://img.shields.io/badge/Python-3776AB?logo=python&logoColor=white)](#disciplinas)
[![RISC-V](https://img.shields.io/badge/RISC--V_Assembly-283272?logo=riscv&logoColor=white)](#disciplinas)
[![SQL](https://img.shields.io/badge/SQL-MySQL_%7C_PostgreSQL-4479A1?logo=postgresql&logoColor=white)](#disciplinas)
[![JavaScript](https://img.shields.io/badge/JavaScript-F7DF1E?logo=javascript&logoColor=black)](#disciplinas)
[![Status](https://img.shields.io/badge/curso-em_andamento-yellow)](#sobre-o-curso)

Cada pasta corresponde a uma disciplina e tem um README com o conteúdo, os trabalhos e como executá-los. Os trabalhos maiores têm README próprio, com arquitetura, decisões de projeto, instruções de execução e limitações conhecidas. O repositório é atualizado conforme o curso avança.

---

## Sumário

- [Projetos em destaque](#projetos-em-destaque)
- [Disciplinas](#disciplinas)
- [Como navegar](#como-navegar)
- [Sobre o curso](#sobre-o-curso)
- [Autora](#autora)

---

## Projetos em destaque

| Projeto | O que é | Tecnologias |
|---|---|---|
| [Compilador de C simplificado](./Compiladores) | Construído em cinco etapas, do analisador de expressões a funções com `jal`/`jr`, gerando assembly x86-64 para Linux, macOS e Windows, com 47 casos de erro testados | C, NASM |
| [vsfs](./Sistemas-OperacionaisII) | Sistema de arquivos do OSTEP montado com FUSE: superbloco, bitmaps, inodes, diretórios, hard links e um `fsck` próprio | Python, FUSE |
| [opzip](./Sistemas-OperacionaisI/opzip) | Compressor RLE paralelo com escrita ordenada por buffer circular, *speedup* medido e um deadlock real diagnosticado | C, pthreads |
| [Relógios bare-metal](./Organizacao-computadores) | O mesmo relógio digital em Assembly e em C, sem sistema operacional, movido por interrupções de temporizador e de UART | RISC-V, QEMU |
| [FrotaGesta](./Lab_Banco-Dados/frotagesta) | Controle de frota com regras de negócio em gatilhos, procedure transacional e interface desktop | PostgreSQL, Python |
| [Árvores balanceadas](./Estrutura_de_Dados_C/ED2/Tarefa1_Arvores_Binarias_Balanceadas) | ABB, AVL e Rubro-Negra comparadas em altura, rotações e custo de busca | C |

## Disciplinas

| Disciplina | Trabalhos | Tecnologias |
|---|---|---|
| [Arquitetura de Computadores](./Arquitetura-Computadores) | Processador RV32 em Logisim com bateria de testes; jogo de labirinto bare-metal | Logisim, Assembly RISC-V |
| [Banco de Dados](./Banco-de-dados) | Biblioteca digital em 3FN com 10 consultas analíticas | MySQL |
| [Compiladores](./Compiladores) | Compilador incremental de C simplificado, em cinco etapas | C, NASM |
| [Engenharia de Software](./Engenharia_Software) | Domínio hospitalar no modelo de domínio rico | Python, UML |
| [Estrutura de Dados em C](./Estrutura_de_Dados_C) | Listas, pilhas, filas, heap, árvores balanceadas, matrizes esparsas e grafos | C |
| [Laboratório de Banco de Dados](./Lab_Banco-Dados) | FrotaGesta: gatilhos, procedure, views e interface desktop | PostgreSQL, Python |
| [Organização de Computadores](./Organizacao-computadores) | Vetores em Assembly, pilha com `brk`, fila com `malloc` e dois relógios bare-metal | RISC-V, C, QEMU, Docker |
| [Programação em C++](./Programacao_C++) | 13 listas de exercícios e um jogo de batalhas por turnos | C++ |
| [Programação em Java](./Programacao_Java) | Exercícios e um gerenciador de livros e filmes em Swing | Java, Swing |
| [Programação Paralela](./Programacao-Paralela) | Monitores, semáforos, pools de threads e OpenMP | Java, Python, C |
| [Programação para Dispositivos Móveis](./Programacao-Disp-Moveis) | Em andamento (2026/1) | Android, Kotlin, Flutter |
| [Programação Web](./Programacao-Web) | Módulos, closures, DOM, `fetch` e uma aplicação Flask | JavaScript, Python, Flask |
| [Projeto de Software](./Projeto-Software) | Em andamento (2026/1) | UML, padrões de projeto |
| [Redes de Computadores](./Redes_Computadores) | Cliente WHOIS e eco cliente-servidor sobre TCP | Python, sockets |
| [Sistemas Operacionais I](./Sistemas-OperacionaisI) | Compressor paralelo com produtor-consumidor | C, pthreads |
| [Sistemas Operacionais II](./Sistemas-OperacionaisII) | Sistema de arquivos vsfs, em três fases | Python, FUSE |

## Como navegar

- **README da disciplina:** conteúdo, docente, bibliografia e a lista de trabalhos.
- **README do trabalho:** o que foi feito, como funciona, como compilar e executar, e limitações.
- **Relatórios e enunciados** em PDF ficam na pasta do respectivo trabalho.

A maior parte do código usa só as ferramentas padrão de cada linguagem (`gcc`, `g++`, `javac`, `python3`). Os trabalhos que dependem de ambiente específico, como RISC-V, QEMU, FUSE ou um SGBD, trazem as instruções de instalação, e alguns oferecem um contêiner Docker.

## Sobre o curso

| | |
|---|---|
| **Instituição** | Universidade Federal de Mato Grosso (UFMT), Campus Araguaia |
| **Curso** | Bacharelado em Ciência da Computação |
| **Situação** | Em andamento |

## Autora

**Raissa Cavalcanti** · [GitHub](https://github.com/cavalcantiraissas) · [LinkedIn](https://www.linkedin.com/in/cavalcantiraissa/)
