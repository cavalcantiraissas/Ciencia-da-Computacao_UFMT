# Implementação da Tabela de Símbolos com suporte a `float`

Evolução do compilador de C simplificado (versão de [inclusao_lexico](../inclusao_lexico)) que passa a ter uma **tabela de símbolos** e a reconhecer números de **ponto flutuante**. Nesta etapa, o compilador processa apenas **declarações de variáveis** (`int` e `float`), registra cada variável na tabela de símbolos e gera a **seção de dados** (`section .data`) em assembly NASM.

## Gramática atual

```
program      -> declarations statements
declarations -> declaration declarations | ε
declaration  -> tipo id ;
tipo         -> int | float
statement    -> ε
```

`statements` ainda não é processado: `program()` chama apenas `declarations()`.

## Arquivos

| Arquivo | Descrição |
|---|---|
| [struct_compiler.h](struct_compiler.h) | Constantes globais, códigos dos tokens (incluindo `FLOAT_NUM` e as palavras reservadas `INT`/`FLOAT`) e a estrutura `type_token`. |
| [lex.h](lex.h) / [lex.c](lex.c) | Analisador léxico. Reconhece inteiros (`42`) e números com precisão (`3.14`): um ponto só faz parte do número se for seguido de dígito (`7.` gera `NUM` seguido de um token de erro para o `.`). Também reconhece identificadores, palavras reservadas, operadores, parênteses e `;`. |
| [symbols.h](symbols.h) / [symbols.c](symbols.c) | Tabela de símbolos de variáveis (nome, tipo, endereço e um `value` genérico do tipo `void*`, convertido conforme o tipo) e tabela de strings (rótulos `str0`, `str1`, ...). Inclui funções de busca, declaração, inicialização e impressão para depuração. O suporte a funções está esboçado em comentários. |
| [synt.h](synt.h) / [synt.c](synt.c) | Analisador sintático descendente recursivo e `main`. Valida as declarações, detecta variável redeclarada e erros de sintaxe (falta de identificador ou de `;`), imprime a tabela de símbolos e grava o arquivo `<entrada>.asm`. |
| [gen.h](gen.h) / [gen.c](gen.c) | Gerador de código. `gen_data_section()` emite a `section .data` com as strings de formato e uma entrada `dd` por variável (`dd 0` para `int`, `dd 0.0` para `float`). As funções `genAdd`, `genSub`, `genMult`, `genDiv` e `genNum` (pilha com `rax`/`rbx`) ainda não são usadas nesta etapa. |
| [tst_lex_numeros.c](tst_lex_numeros.c) | Programa de teste do léxico: lê `entrada_numeros.txt` e imprime se cada token é `NUM` ou `FLOAT_NUM`. |

## Compilação e execução

```bash
# compilador
gcc -Wall -o compiler lex.c symbols.c gen.c synt.c
./compiler entrada.txt          # gera entrada.txt.asm

# teste do léxico de números (precisa de entrada_numeros.txt na pasta atual)
gcc -Wall -o tst_lex_numeros tst_lex_numeros.c lex.c
./tst_lex_numeros
```

### Exemplo

Entrada (`entrada.txt`):

```c
int x;
float y;
int z;
```

Saída gerada (`entrada.txt.asm`):

```nasm
section .data
fmtstr0  db  "%d",0
fmtstr1  db  "%s",0
x  dd  0
y  dd  0.0
z  dd  0
```

Casos de erro tratados:

- `int x; int x;` → `[ERRO] Variavel 'x' ja declarada.`
- `int x float y;` → `[ERRO] Entrada processada: 'float' - tag esperada: '59'` (faltou `;`)

Em ambos os casos o programa termina com `FALHA NA COMPILACAO DO PROGRAMA` e código de saída `1`.

## Análise do código

A compilação com `gcc -Wall -Wextra` não gera avisos, e os casos acima se comportam como esperado. Os testes manuais e o AddressSanitizer revelaram os seguintes pontos:

### Problemas encontrados

1. **Leitura fora dos limites em `keyWordFind`** ([lex.c](lex.c)): o vetor `key_words` não termina com uma entrada sentinela (lexema vazio), mas o laço só para quando encontra `lexema[0] == '\0'`. Sempre que um identificador não é palavra reservada, a busca lê além do fim do vetor. O AddressSanitizer acusa `global-buffer-overflow` já em `int abc;`. Para corrigir, basta acrescentar `{0, "", 0}` ao final de `key_words`.
2. **Estouro de buffer em `getToken`** ([lex.c](lex.c)): identificadores e números são copiados para `buffer[MAX_CHAR]` (32 bytes) sem verificar o tamanho. Um identificador com mais de 31 caracteres faz o programa abortar. A mesma correção já foi feita em `inclusao_lexico`.
3. **Estouro de `output_file_name`** ([synt.c](synt.c)): o nome do arquivo de entrada mais `.asm` é copiado com `strcpy`/`strcat` para um vetor de 32 bytes. Caminhos com mais de 27 caracteres estouram o vetor. O retorno de `fopen` também não é verificado.
4. **`ch` declarado como `char`** ([lex.c](lex.c)): `fgetc` retorna `int`. Em plataformas onde `char` não tem sinal (por exemplo, Linux em ARM), a comparação com `EOF` nunca é verdadeira.
5. **Dois `ungetc` seguidos** ([lex.c](lex.c)): no caso `7.`, o léxico devolve dois caracteres ao fluxo, mas o padrão C só garante um caractere de *pushback*. Funciona no macOS e na glibc, mas não é portátil.
6. **`\r` não é tratado como espaço**: arquivos com quebra de linha do Windows (CRLF) geram `Tipo desconhecido: -1`.

### Observações menores

- A palavra `integer` está mapeada para a tag `NUM` (a mesma de um literal inteiro). Provavelmente o correto seria `INT`, ou removê-la.
- O valor de um `FLOAT_NUM` fica só no `lexema`. O campo `value` do token é `int` e não é preenchido para números.
- `genMult` emite `imult`, que não é uma instrução x86 válida (o correto é `imul`). `genDiv` usa `idiv rax,rbx`, mas `idiv` aceita um único operando (dividendo em `rdx:rax`). Essas funções ainda não são chamadas.
- Os tokens alocados com `malloc` em `getToken` nunca são liberados.
- `initSymbolTableString` percorre `MAX_SYMBOLS` (4096) posições, mas o limite de strings é `MAX_STRINGS` (64). Não há estouro, só trabalho desnecessário.
- `tst_lex_numeros.c` depende de `entrada_numeros.txt`, que não está na pasta.
