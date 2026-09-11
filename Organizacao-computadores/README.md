# Organização de Computadores

Estudos voltados ao funcionamento interno dos sistemas computacionais modernos sob a perspectiva do programador de baixo nível.

## Informações Gerais
* Instituição: Universidade Federal de Mato Grosso
* Docente: Linder Cândido da Silva
* Período: 2025/2
* Carga Horária: 64 horas

## Objetivos
Compreender a arquitetura do conjunto de instruções (ISA) e desenvolver habilidades em programação Assembly para integração direta com o hardware.

## Conteúdo Programático
* Arquitetura do processador RISC V
* Linguagem de montagem (Assembly) e estrutura de executáveis
* Formato ELF e processo de ligação (Linking)
* Chamadas de sistema e interface com o núcleo do SO
* Tratamento de exceções e interrupções de hardware
* Gerenciamento de memória: Paginação e segmentação

## Projetos

| Projeto | Descrição |
|---|---|
| [pilha-dinamica-brk_fila-dinamica-malloc](./pilha-dinamica-brk_fila-dinamica-malloc) | Pilha (syscall `brk`) e fila (`malloc`/`free`) dinâmicas implementadas em Assembly RISC-V 64-bit |
| [relogio-digital](./relogio-digital) | Relógio digital bare metal em C para RISC-V 64 bits, com interrupções de timer e UART |
| [riscv-clock](./riscv-clock) | Relógio digital bare metal em Assembly RISC-V, executando sobre QEMU virt |

## Bibliografia Principal
* TANENBAUM, Andrew S. Organização Estruturada de Computadores
* HENNESSY, John L.; PATTERSON, David A. Organização e Projeto de Computadores
* ZHIRKOV, Igor. Programação em Baixo Nível
