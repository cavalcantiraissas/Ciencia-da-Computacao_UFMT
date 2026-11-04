# Arquitetura de Computadores

**Interface hardware-software e projeto de processadores: um processador RISC-V em Logisim, programas em Assembly e um jogo bare-metal.**

[![RISC-V](https://img.shields.io/badge/RISC--V-RV32I-283272?logo=riscv&logoColor=white)](https://riscv.org/)
[![Assembly](https://img.shields.io/badge/Assembly-RISC--V-555555)](#trabalhos)
[![Logisim-evolution](https://img.shields.io/badge/Logisim--evolution-3.7-2E7D32)](https://github.com/logisim-evolution/logisim-evolution)
[![UFMT](https://img.shields.io/badge/UFMT-Ci%C3%AAncia_da_Computa%C3%A7%C3%A3o-003366)](../)

Trabalhos da disciplina de Arquitetura de Computadores do curso de Ciência da Computação da UFMT. O foco é entender como um programa é executado pelo hardware: do conjunto de instruções (ISA) ao caminho de dados que o implementa.

---

## Trabalhos

| Trabalho | Descrição | Tecnologias |
|---|---|---|
| [Processador RISC-V](./risc-v) | Processador RV32 de ciclo único em Logisim-evolution, com bateria de testes por instrução e um programa de inversão de bytes | Logisim-evolution, Assembly RISC-V |
| [Labirinto RISC-V](./labirinto-riscv) | Jogo de labirinto bare-metal para o emulador Emulsiv, com tela de bitmap, teclado WASD e pontuação | Assembly RV32I, Emulsiv |

### Processador RISC-V

O circuito [`RISCV32.circ`](risc-v/RISCV32.circ) implementa um subconjunto do RV32I, dividido em subcircuitos:

| Subcircuito | Função |
|---|---|
| `RISCV` | Caminho de dados principal |
| `PCAdd` | Cálculo de PC + 4 |
| `OPDecode`, `ALUOpGen` | Decodificação do opcode e sinais de controle da ULA |
| `ImmGen` | Geração do imediato |
| `RegGen` | Banco de registradores |
| `SetLessThan`, `LUI` | Instruções `slt` e `lui` |
| `CalcMemAdd`, `LoadWord`, `ReadAndWrite`, `CSDecoder`, `ByteSelector`, `ByteShifter` | Acesso à memória por palavra e por byte |

Instruções cobertas pelos testes de [`testfinal.txt`](risc-v/testfinal.txt): `add`, `sub`, `and`, `or`, `xor`, `slt`, `sll`, `sra`, `addi`, `andi`, `lw`, `sw`, `lb`, `sb` e `lui`. Cada teste define o número de ciclos e o valor esperado de cada registrador ao final, com casos base, casos aleatórios e casos de borda.

O programa [`byte-reversal.txt`](risc-v/byte-reversal.txt) inverte a ordem dos bytes de duas palavras de dois jeitos (máscaras e deslocamentos, e `lb`/`sb`) e combina os resultados com `xor`.

**Para abrir:** instale o [Logisim-evolution](https://github.com/logisim-evolution/logisim-evolution) 3.7 ou superior e abra `RISCV32.circ`.

## Conteúdo da disciplina

- Modelo de von Neumann e ciclo de execução de instruções
- Arquitetura do conjunto de instruções (ISA) RISC-V
- Programação em Assembly
- Microarquiteturas: ciclo único, multiciclo e pipeline
- Hierarquia de memória e cache
- Evolução dos microprocessadores e lógica digital

## Bibliografia

- TANENBAUM, Andrew S. *Organização Estruturada de Computadores*.
- WEBER, Raul Fernando. *Fundamentos de Arquitetura de Computadores*.
- HARRIS, David; HARRIS, Sarah. *Digital Design and Computer Architecture: RISC-V Edition*.

---

[← Repositório principal](../)
