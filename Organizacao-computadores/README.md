# Organização de Computadores

**Programação em Assembly RISC-V e sistemas bare-metal: chamadas de sistema, gerenciamento de memória, interrupções de hardware e periféricos mapeados em memória.**

[![RISC-V](https://img.shields.io/badge/RISC--V-RV32_%7C_RV64-283272?logo=riscv&logoColor=white)](https://riscv.org/)
[![Assembly](https://img.shields.io/badge/Assembly-GNU_as-555555)](#trabalhos)
[![QEMU](https://img.shields.io/badge/QEMU-RISC--V-FF6600?logo=qemu&logoColor=white)](https://www.qemu.org/)
[![Docker](https://img.shields.io/badge/Docker-ambiente_de_build-2496ED?logo=docker&logoColor=white)](https://www.docker.com/)
[![UFMT](https://img.shields.io/badge/UFMT-Ci%C3%AAncia_da_Computa%C3%A7%C3%A3o-003366)](../)

Trabalhos da disciplina de Organização de Computadores do curso de Ciência da Computação da UFMT. O foco é o funcionamento do computador do ponto de vista do programador de baixo nível: o conjunto de instruções RISC-V, o formato dos executáveis, a interface com o sistema operacional e, nos trabalhos bare-metal, o controle direto do hardware.

| | |
|---|---|
| **Docente** | Linder Cândido da Silva |
| **Período** | 2025/2 |
| **Carga horária** | 64 horas |

---

## Trabalhos

| Trabalho | Descrição | Tecnologias |
|---|---|---|
| [Análise de vetores](#trabalho-1-análise-de-vetores) | Leitura, estatísticas, Bubble Sort e busca binária num vetor de inteiros | Assembly RV64, libc, QEMU |
| [Pilha com `brk` e fila com `malloc`](./pilha-dinamica-brk_fila-dinamica-malloc) | Pilha que move o *program break* diretamente e fila encadeada com `malloc`/`free`, com menus interativos | Assembly RV64, QEMU, Docker |
| [Relógio digital em Assembly](./riscv-clock) | Relógio bare-metal com interrupções de temporizador e de UART, num único arquivo Assembly | Assembly RV32IMA, QEMU `virt`, Docker |
| [Relógio digital em C](./relogio-digital) | O mesmo relógio em C freestanding, com drivers separados para UART, CLINT e PLIC | C, RV64IMAC, QEMU `virt`, Docker |

### Trabalho 1: análise de vetores

O programa [`trab1.S`](trab1.S) lê de 1 a 100 inteiros, calcula soma, menor valor, maior valor e média, ordena o vetor com Bubble Sort e procura um valor por busca binária. Cada etapa é uma função separada, seguindo a convenção de chamada do RISC-V. Os relatórios estão em [`Relatorio_Completo_Trabalho1.pdf`](Relatorio_Completo_Trabalho1.pdf) e [`Relatorio_Resumo_Trabalho1.pdf`](Relatorio_Resumo_Trabalho1.pdf).

```bash
# Linux (Debian/Ubuntu) ou dentro de um contêiner ubuntu:22.04
apt-get install -y gcc-riscv64-linux-gnu qemu-user
riscv64-linux-gnu-gcc -o trab1 trab1.S
qemu-riscv64 -L /usr/riscv64-linux-gnu ./trab1
```

## Conteúdo da disciplina

- Arquitetura do processador RISC-V
- Linguagem de montagem e estrutura de executáveis
- Formato ELF e processo de ligação
- Chamadas de sistema e interface com o núcleo do sistema operacional
- Tratamento de exceções e interrupções de hardware
- Gerenciamento de memória: paginação e segmentação

## Bibliografia

- TANENBAUM, Andrew S. *Organização Estruturada de Computadores*.
- HENNESSY, John L.; PATTERSON, David A. *Organização e Projeto de Computadores*.
- ZHIRKOV, Igor. *Programação em Baixo Nível*.

---

[← Repositório principal](../)
