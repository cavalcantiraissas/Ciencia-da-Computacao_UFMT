# Compiladores

Repositório de atividades e trabalhos desenvolvidos durante a disciplina de **Compiladores**, do curso de Ciência da Computação.

## Sobre a disciplina

A disciplina apresenta a organização e a estrutura de um compilador, cobrindo desde a construção de um compilador simples de uma passagem até os principais estágios do processo de compilação: análise léxica, análise sintática, escopo e tabelas de símbolos, análise semântica, geração de código e interpretação de código.

As técnicas estudadas — especialmente análise léxica e sintática — têm aplicação além da construção de compiladores, sendo utilizadas em expressões regulares, planilhas eletrônicas, sistemas de banco de dados e inteligência artificial.

### Objetivos

- Apresentar os conceitos e capacitar o desenvolvimento de compiladores, com domínio das etapas de análise léxica, sintática, semântica e geração de código intermediário (ou interpretação).
- Desenvolver os componentes básicos de um compilador para uma linguagem proposta (parcialmente simplificada).
- Gerar código intermediário (equivalente a código de máquina) para uma determinada plataforma/arquitetura computacional.

### Metodologia

A disciplina é conduzida de forma incremental, em ciclos: a cada ciclo, o processo completo de análise léxica, sintática e geração de código é revisitado, incorporando novos elementos e conceitos, até se alcançar uma versão mais completa do compilador. Ou seja, os trabalhos deste repositório tendem a representar versões evolutivas de um mesmo compilador, e não módulos isolados.

### Tópicos abordados

1. Estrutura geral de um compilador e interpretador
2. Análise léxica com autômatos finitos
3. Tabelas de símbolos e escopos
4. Análise léxica codificada
5. Geradores de analisadores léxicos
6. Análise sintática preditiva, recursiva e descendente
7. Análise sintática preditiva dirigida por tabela
8. Construção de tabela de análise sintática
9. Árvores de sintaxe abstrata
10. Análise sintática ascendente
11. Geradores de analisadores sintáticos
12. Geração de código intermediário

## Conteúdo do repositório

- [analisador_sintatico.c](analisador_sintatico.c) — analisador sintático descendente recursivo para expressões aritméticas (soma, subtração, multiplicação e divisão), com precedência de operadores e detecção de erros de sintaxe.
- [gerador_codigo_registradores.c](gerador_codigo_registradores.c) — evolução do analisador sintático anterior, com geração de código para uma máquina com registradores (LOAD/ADD/SUB/MUL/DIV), gravado no arquivo `codigo.asm`.
- [inclusao_lexico/](inclusao_lexico) — compilador modularizado (léxico, sintático, gerador de código e `main` separados), com leitura do código-fonte a partir de arquivo, precedência de `*`/`/` sobre `+`/`-` e mensagens de erro léxico/sintático.
