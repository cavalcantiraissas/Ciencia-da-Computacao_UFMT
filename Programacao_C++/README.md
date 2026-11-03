# Programação em C++

**Orientação a objetos e recursos modernos de C++: classes, herança, polimorfismo, ponteiros inteligentes, exceções, templates e STL, com um jogo de batalhas por turnos como trabalho final.**

[![C++](https://img.shields.io/badge/C%2B%2B-11-00599C?logo=cplusplus&logoColor=white)](https://isocpp.org/)
[![Make](https://img.shields.io/badge/build-make-427819)](#como-compilar)
[![UFMT](https://img.shields.io/badge/UFMT-Ci%C3%AAncia_da_Computa%C3%A7%C3%A3o-003366)](../)

Trabalhos da disciplina de Programação II do curso de Ciência da Computação da UFMT, dedicada ao paradigma orientado a objetos em C++.

| | |
|---|---|
| **Docente** | Sandino Barros Jardim |
| **Período** | 2024/1 |
| **Carga horária** | 64 horas |

---

## Trabalho prático: batalhas Pokémon

Jogo de terminal em [`Trabalho_Pratico_Pokemon`](./Trabalho_Pratico_Pokemon) em que o jogador escolhe seus Pokémon e enfrenta o computador em batalhas por turnos.

- **Dados em arquivo:** espécies (tipos, HP, nível e atributos) em `pokemons.txt` e golpes (categoria, poder, precisão e tipo) em `ataques.txt`.
- **Cálculo de dano** com nível, poder do golpe, ataque e defesa (físicos ou especiais), acerto crítico, bônus por golpe do mesmo tipo do Pokémon e tabela de efetividade entre tipos.
- **Três níveis de dificuldade** e opção de trocar de Pokémon durante a batalha.
- **Ranking persistente** por apelido do jogador em `ranking.txt`.

```bash
cd Trabalho_Pratico_Pokemon
make
./main
```

Os enunciados estão em `_Prog2__Trabalho_2024_1.pdf` e `Direcionamento-Pokémon.pdf`.

## Listas de exercícios

Cada lista tem o enunciado em PDF e as soluções em [`Listas`](./Listas).

| Lista | Tema |
|---|---|
| [Lista 1](./Listas/Lista_1) | Fundamentos: validação de senha, substrings e outros exercícios introdutórios |
| [Lista 3](./Listas/Lista_3) | Gerenciamento de usuários com ponteiros inteligentes (`std::shared_ptr`) |
| [Lista 4](./Listas/Lista_4) | Encapsulamento e testes de unidade das classes |
| [Lista 5](./Listas/Lista_5) | Herança e polimorfismo (hierarquia de animais e outros) |
| [Lista 6](./Listas/Lista_6) | Simulação de fusão de cartas e vetores bidimensionais |
| [Lista 7](./Listas/Lista_7) | Sistema de biblioteca com livros e membros em arquivo, em módulos com `makefile` |
| [Lista 8](./Listas/Lista_8) | Exceções e *namespaces*: gestão de produtos e hierarquia de itens (eletrônicos e vestuário) |
| [Lista 9](./Listas/Lista_9) | Enumerações (`enum` e `enum class`) |
| [Lista 10](./Listas/Lista_10) | Templates: registro de alunos, fila e mapa genéricos, sistema de comparação |
| [Lista 11](./Listas/Lista_11) | Ordenação com funções da STL |
| [Lista 12](./Listas/Lista_12) | Depuração de *segmentation fault* com GDB (com relatório) |
| [Lista 13](./Listas/Lista_13) | Iteradores e contêineres da STL (`vector`, `list` e outros) |
| [Lista de revisão](./Listas/Lista_Revisao) | Balanceador de carga com *round robin*, *round robin* ponderado e escolha aleatória |

## Como compilar

```bash
g++ -std=c++11 arquivo.cpp -o programa
./programa
```

As listas 7 e 8 e o trabalho prático têm `makefile`: basta rodar `make` na pasta.

## Conteúdo da disciplina

- Alocação dinâmica de memória e ponteiros inteligentes
- Classes, atributos e métodos
- Encapsulamento, construtores e destrutores
- Herança, polimorfismo e funções virtuais
- Sobrecarga de operadores
- Tratamento de exceções e programação genérica (templates)
- Biblioteca padrão de templates (STL)

## Bibliografia

- DEITEL, H. M.; DEITEL, P. J. *C++: Como Programar*.
- STROUSTRUP, Bjarne. *Programming: Principles and Practice Using C++*.
- MIZRAHI, Victorine Viviane. *Treinamento em Linguagem C++*.

---

[← Repositório principal](../)
