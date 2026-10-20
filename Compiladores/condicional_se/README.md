# Comandos condicionais (`if`/`else`), `while`, atribuição e operadores lógicos

Evolução do compilador de C simplificado. Nesta etapa o compilador passa a reconhecer:

- o comando **`if`** (com `else` opcional) e o comando **`while`**, com blocos delimitados por `{ }` (ou `begin`/`end`);
- **expressões booleanas** no formato `[id | exp_aritmética] operador [id | exp_aritmética]`, combinadas com **`&&`** e **`||`** e agrupadas com parênteses;
- o **comando de atribuição** `id = valor;`, em que o valor precisa ser compatível com o tipo da variável;
- **expressões com `int`, `float` e `char`**, com conversão automática de `int` para `float` e menos unário;
- **`read`/`write` com conversão entre texto e número**, feita por rotinas de apoio em assembly;
- geração de código para **Linux**, **macOS** e **Windows** (x86-64).

## Gramática atual

```
program      -> declarations statements
declarations -> declaration declarations | ε
declaration  -> type id ;
type         -> int | float | char | string
statements   -> statement statements | ε
statement    -> read_stmt | write_stmt | if_stmt | while_stmt | assign_stmt
read_stmt    -> read id ;
write_stmt   -> write id ; | write string_const ;
if_stmt      -> if ( B ) block [ else block ]
while_stmt   -> while ( B ) block
block        -> { statements }                  (begin statements end também é aceito)
assign_stmt  -> id = valor ;

B            -> BOr                             (o resultado precisa ser booleano)
BOr          -> BAnd BOrR
BOrR         -> || BAnd BOrR | ε
BAnd         -> BRel BAndR
BAndR        -> && BRel BAndR | ε
BRel         -> E [ rel_operator E ]
rel_operator -> < | > | <= | >= | == | !=

E            -> T ER
ER           -> + T ER | - T ER | ε
T            -> F TR
TR           -> * F TR | / F TR | ε
F            -> ( BOr ) | - F | num | char_const | id
```

Precedência, da menor para a maior: `||`, `&&`, relacionais, `+ -`, `* /`. Assim, `a < b && b < c || c == 0` é avaliado como `((a < b) && (b < c)) || (c == 0)`.

As funções de `BOr` a `F` devolvem o **tipo** da expressão reconhecida (`EXPR_INT`, `EXPR_FLOAT` ou `EXPR_BOOL`), ou `false` em caso de erro. Com isso o sintático verifica que:

- os operadores aritméticos e relacionais recebem expressões numéricas dos dois lados (`x + 1 >= y - 4`). Se um lado for `int` e o outro `float`, o `int` é convertido (`cvtsi2sd`) e a operação é feita em ponto flutuante (`addsd`, `ucomisd`, ...);
- `&&` e `||` recebem comparações (`x > 1 && x` é erro);
- a condição de um `if`/`while` é booleana (`if (x - 1)` é erro);
- variáveis `string` não entram em expressões.

Variáveis e constantes `char` valem como inteiros (código ASCII), então `c = c + 2;` e `c == 'c'` funcionam. O menos unário vale para `int` e `float` (`b = -a * 2 + -(3 - 5);`).

### Curto-circuito em `&&` e `||`

O operando da direita só é avaliado quando necessário: em `a && b`, se `a` for falso o resultado é `0` sem avaliar `b`; em `a || b`, se `a` for verdadeiro o resultado é `1`. Assim, `if (x != 0 && 10 / x > 1)` não divide por zero quando `x` vale `0`. Para `x < 5 && x != 0` (sem os comentários):

```nasm
	...                 ; x < 5 -> empilha 0 ou 1
	pop rax
	cmp rax, 0
	jne label_bool1     ; esquerda verdadeira: avalia a direita
	push rax            ; esquerda falsa: resultado 0
	jmp label_bool2
label_bool1:
	...                 ; x != 0 -> empilha 0 ou 1 (o resultado)
label_bool2:
	pop rax             ; jump condicional do if
	cmp rax, 0
	jz label0
```

Os parênteses de `F` aceitam tanto expressões aritméticas quanto booleanas, então `(z / 5 != 1 || x * 2 <= 10)` e `3 * (x - 1) > z` funcionam.

### Atribuição: valor compatível com o tipo

| Variável | Valor aceito | Exemplo | Código gerado |
|---|---|---|---|
| `int` | expressão inteira (`int`/`char`) | `z = x + y * 2;` | `pop rax` / `mov [z], rax` |
| `float` | expressão inteira ou float (o inteiro é convertido) | `g = f * 2 + a;` | `pop rax` / `mov [g], rax` (bits IEEE 754 de precisão dupla) |
| `char` | expressão inteira | `c = c + 2;` | `pop rax` / `mov [c], al` |
| `string` | constante string ou outra variável string | `copia = nome;` | `call _rt_strcpy` (copia até o `\0`) |

`int` e `char` não recebem expressões `float` (haveria perda da parte fracionária) nem comparações.

### Entrada e saída

`read` lê uma linha da entrada e converte o texto para o tipo da variável. `write` converte o valor para texto e o escreve seguido de quebra de linha.

| Tipo | `read` | `write` |
|---|---|---|
| `int` | ignora espaços, aceita sinal (`-42`) | decimal (`42`) |
| `float` | `3.14`, `-0.5`, `10` | 6 casas decimais, arredondado (`3.140000`) |
| `char` | primeiro caractere da linha | o caractere |
| `string` | a linha inteira, sem a quebra de linha (até 255 caracteres) | o texto até o `\0` |

Constantes string aceitam as sequências de escape `\n`, `\t`, `\r`, `\0`, `\\`, `\"` e `\'`. Na seção de dados elas são escritas entre crases (`` `...` ``), que o NASM interpreta.

As conversões são feitas por **rotinas de apoio** (`_rt_read_int`, `_rt_write_float`, `_rt_strcpy`, ...), escritas em assembly no fim do arquivo gerado. No Linux e no macOS elas usam as chamadas de sistema `read`/`write` (instrução `syscall`, sem libc). No Windows elas chamam `printf`/`scanf`. Uma divisão inteira por zero encerra o programa com `[ERRO] Divisao por zero` e código de saída `1`.


## Arquivos

| Arquivo | Descrição |
|---|---|
| [struct_compiler.h](struct_compiler.h) | Códigos dos tokens. Remove `THEN`; adiciona `AND` (`&&`), `OR` (`||`), `STRING_CONST` e `CHAR_CONST`. `MAX_CHAR` passou de 32 para 256. |
| [lex.h](lex.h) / [lex.c](lex.c) | Analisador léxico. Reconhece `while`, `=`, `>`, `>=`, `!=`, `&&`, `\|\|`, `{`/`}` (mesmos tokens de `begin`/`end`) e constantes char e string. |
| [symbols.h](symbols.h) / [symbols.c](symbols.c) | Tabela de símbolos de variáveis e de strings. |
| [synt.h](synt.h) / [synt.c](synt.c) | Analisador sintático descendente recursivo e `main`. |
| [gen.h](gen.h) / [gen.c](gen.c) | Gerador de código (NASM, x86-64), com as rotinas de apoio. Gera para Linux por padrão. |
| [gen_macos.c](gen_macos.c) / [gen_win.c](gen_win.c) | Geradores para macOS e Windows: definem `TARGET_MACOS`/`TARGET_WIN` e incluem `gen.c`. Muda apenas a entrada e a saída do programa e as rotinas de apoio. |
| [testes/](testes) | Programas de teste válidos, com a entrada (`.in`) e a saída esperada (`.out`), e o script [rodar.sh](testes/rodar.sh). Os de [testes/erros/](testes/erros) devem ser rejeitados. |
| [code1.cc](code1.cc) / [code2.cc](code2.cc) | Exemplos originais. O `code2.cc` usa `if (1 - 1)`, que agora é rejeitado por não ser uma expressão booleana. |

## Compilação e testes

```bash
make                      # gera ./compiler (codigo para Linux)
./compiler testes/if_multiplos.cc     # gera testes/if_multiplos.cc.asm
make testes               # compila tudo de testes/ e confere se tudo de testes/erros/ e rejeitado
make executar             # compila, monta, liga e executa os testes, comparando com os .out
make clean
```

Montagem e ligação do código gerado em cada plataforma:

| Plataforma | Compilador | Montagem e ligação |
|---|---|---|
| Linux x86-64 | `make` → `./compiler` | `nasm -f elf64 prog.cc.asm` e `ld -m elf_x86_64 prog.cc.o -o prog` |
| macOS x86-64 | `make compiler_macos` → `./compiler4macos` | `nasm -f macho64 prog.cc.asm` e `clang -arch x86_64 -nostartfiles -e _start prog.cc.o -o prog` |
| Windows x86-64 | `make compiler_win` → `./compiler4win` | `nasm -f win64 prog.cc.asm` e `gcc prog.cc.obj -o prog.exe` |

No macOS, `sh testes/rodar.sh` usa o `compiler4macos`.

Se a compilação dá certo, o compilador imprime `Compilacao concluida` e retorna `0`. Se houver erro, ele imprime `FALHA NA COMPILACAO DO PROGRAMA`, apaga o `.asm` incompleto e retorna `1`.

### Exemplo: `if` com vários testes lógicos

Entrada (trecho de [testes/if_multiplos.cc](testes/if_multiplos.cc)):

```c
if (x < y && y < z || z == 0) {
    write "ordenados ou z zero";
}
```

Código gerado (sem os comentários):

```nasm
	push qword [x]
	push qword [y]
	pop rbx
	pop rax
	mov rcx, 1
	cmp rax, rbx
	jl label_bool0
	mov rcx, 0
label_bool0:
	mov rax, rcx
	push rax
	pop rax
	cmp rax, 0
	jne label_bool1
	push rax
	jmp label_bool2
label_bool1:
	push qword [y]
	push qword [z]
	pop rbx
	pop rax
	mov rcx, 1
	cmp rax, rbx
	jl label_bool3
	mov rcx, 0
label_bool3:
	mov rax, rcx
	push rax
label_bool2:
	pop rax
	cmp rax, 0
	je label_bool4
	push rax
	jmp label_bool5
label_bool4:
	push qword [z]
	mov rax, 0
	push rax
	pop rbx
	pop rax
	mov rcx, 1
	cmp rax, rbx
	je label_bool6
	mov rcx, 0
label_bool6:
	mov rax, rcx
	push rax
label_bool5:
	pop rax
	cmp rax, 0
	jz label0

	lea rsi, [str0]
	call _rt_write_str
	call _rt_newline
label0:
```

Cada comparação empilha `1` (verdadeiro) ou `0` (falso). `&&` só avalia `y < z` se `x < y` for verdadeiro (`label_bool1`). `||` só avalia `z == 0` se o resultado do `&&` for falso (`label_bool4`). Os comandos ficam com tabulação e só os labels ficam na coluna 0.

### Programas de teste

| Programa | O que testa |
|---|---|
| [if_simples.cc](testes/if_simples.cc) | `if` logo após as declarações, `read`/`write` |
| [if_else.cc](testes/if_else.cc) | `if`/`else` com id dos dois lados |
| [if_logicos.cc](testes/if_logicos.cc) | `&&`, `\|\|`, expressão aritmética de um lado e id do outro |
| [if_multiplos.cc](testes/if_multiplos.cc) | três e quatro testes no mesmo `if`, misturando `&&`, `\|\|` e parênteses, expressões dos dois lados |
| [if_aninhado_while.cc](testes/if_aninhado_while.cc) | `while` com `if`/`else` aninhados, bloco vazio e atribuições de todos os tipos |
| [begin_end.cc](testes/begin_end.cc) | blocos com `begin`/`end` |
| [entrada_saida.cc](testes/entrada_saida.cc) | `read`/`write` de todos os tipos com conversão, atribuição entre strings, escape `\t` |
| [float_char_unario.cc](testes/float_char_unario.cc) | expressões float, conversão int→float, menos unário, aritmética e comparação com char |
| [curto_circuito.cc](testes/curto_circuito.cc) | `&&`/`\|\|` sem avaliar a divisão por zero da direita, `while` com `&&`, escapes `\n` e `\"`, e erro de divisão por zero em tempo de execução |

Os 9 programas passam no Linux (Docker, imagem `gcc:13`), no macOS x86-64 e no Windows (`x86_64-w64-mingw32-gcc` e wine).

Casos de erro (todos em [testes/erros/](testes/erros)):

| Entrada | Mensagem |
|---|---|
| `if (x - 1) { ... }` | `Expressao booleana esperada: use um operador relacional ...` |
| `if (x > ) { ... }` | `Operando esperado (id, numero ou '('), encontrado ')'.` |
| `if (x > 1 && x) { ... }` | `O operador '&&' exige operandos booleanos (comparacoes).` |
| `if (x > 1 then) { ... }` | `Entrada inesperada: 'then'. Esperado: ')'.` |
| `if (x > 1) { ...` (sem `}`) | `Entrada inesperada: 'fim de arquivo'. Esperado: '}' (ou 'end').` |
| `x = 1; }` | `Fim de escopo '}' sem inicio de escopo correspondente.` |
| `while (x) { ... }` | `Expressao booleana esperada ...` |
| `while (x < 10) { x = x + ; }` | `Operando esperado (id, numero ou '('), encontrado ';'.` |
| `y = 1;` (não declarada) | `Variavel 'y' nao declarada.` |
| `int x; x = 3.14;` | `Tipo incompativel: variavel 'x' (int) nao pode receber uma expressao float (perda da parte fracionaria).` |
| `char c; c = "abc";` | `Tipo incompativel: variavel 'c' (char) nao pode receber a constante string "abc".` |
| `s = x;` (`s` string, `x` int) | `Tipo incompativel: variavel 's' (string) nao pode receber a variavel 'x' (int).` |
| `x = s + 1;` (`s` string) | `Variavel 's' eh do tipo string e nao pode ser usada em expressoes.` |
| `if (-(x > 1)) ...` | `O menos unario exige um operando aritmetico.` |
| `write "abc\q";` | `[ERRO LEXICO] Sequencia de escape invalida em string: \q` |
| `} else x = 2;` | `Entrada inesperada: 'x'. Esperado: '{' (ou 'begin').` |

Quando a condição de um `if`/`while` é inválida, a mensagem específica vem seguida de `Expressao booleana invalida no comando if.` (ou `while`).

## Alterações em relação à base

| Item | Arquivo | Alteração |
|---|---|---|
| Label da expressão booleana com caracteres inválidos (`p�IA�0`) | [gen.c](gen.c) | `gen_bool_label_name` concatenava o contador a um buffer não inicializado. Agora gera `label_bool0`, `label_bool1`, ... Faltava também o `\n` depois de `cmp eax,ebx`, o que juntava duas instruções na mesma linha. |
| Retorno de `B()` ignorado | [synt.c](synt.c) | `if` e `while` verificam o retorno de `B()` e emitem erro se a expressão for mal formada ou não for booleana. |
| `}` como fim de escopo | [lex.c](lex.c), [synt.c](synt.c) | `{`/`}` geram os mesmos tokens de `begin`/`end`. `statement()` retorna `false` no `}`. |
| `B()` só aceitava `ID OPERADOR EXP` | [synt.c](synt.c) | Os dois lados podem ser id, número ou expressão aritmética. |
| `while` | [lex.c](lex.c), [synt.c](synt.c) | Token gerado pelo léxico. `whileStmt()` retorna `true` só se `(`, `B`, `)` e o bloco forem reconhecidos. |
| Atribuição | [lex.c](lex.c), [synt.c](synt.c), [gen.c](gen.c) | Léxico gera `ASSIGN` para `=`. Nova regra `assignStmt()` verifica declaração e compatibilidade de tipo. Nova função `gen_assign()` gera `pop rax` / `mov [id], rax`. |
| `then` | [lex.c](lex.c), [struct_compiler.h](struct_compiler.h) | Palavra reservada removida. |
| Retorno do `if` | [synt.c](synt.c) | `ifStmt()` verifica cada derivação interna (`(`, `B`, `)`, bloco, `else` e bloco) e retorna `false` se alguma falhar. Antes retornava sempre `1`. |
| Variável em expressões | [synt.c](synt.c), [gen.c](gen.c) | Nova função `gen_push_var()` empilha o **conteúdo** da variável (`push qword [x]`), não o endereço. |
| Tabulação | [gen.c](gen.c) | Todos os comandos passam pela função `emit()`, que acrescenta a tabulação. Só `gen_label()` escreve na coluna 0. |
| `if` logo após as declarações | [synt.c](synt.c) | `declaration()` emitia `Tipo desconhecido` para qualquer token que não fosse tipo, `read` ou `write`. Agora qualquer token que não seja tipo encerra as declarações. |
| `&&` e `\|\|` | [lex.c](lex.c), [synt.c](synt.c), [gen.c](gen.c) | Regras `BOr`/`BAnd` com a precedência usual. `gen_logic_begin()` gera o desvio de curto-circuito. |

Outros problemas da base que foram corrigidos porque afetavam esta etapa:

- **Léxico.**
  - `>`/`>=` e `!=` nunca eram reconhecidos (havia um `TODO` e as entradas `"=="`, `"!="` ... em `key_words` nunca batiam).
  - `key_words` não tinha sentinela, então `keyWordFind` lia além do fim do vetor.
  - Constantes string perdiam o último caractere (`"Informe um valor:"` virava `"Informe um valor"`) e estouravam o buffer com mais de 31 caracteres.
  - `ch` era `char`, o que impedia reconhecer `EOF` corretamente.
  - Constantes string e a palavra reservada `string` tinham a mesma tag.
- **Gerador.**
  - `imult` e `idiv rax,rbx` não existem. Agora são `imul rax, rbx` e `cqo` / `idiv rbx`.
  - A seção de dados gravava strings de formato no lugar das variáveis (`valor: dd "%d", 4`). Agora cada variável recebe espaço do seu tipo, iniciado com zero: `dq 0` para `int`, já que `mov [id], rax` grava 8 bytes.
- **Sintático.**
  - O ramo `write "..."` não tinha `return`.
  - Tokens consumidos não eram liberados.
  - O nome do arquivo de saída estourava com caminhos longos.
  - `main` retornava `1` mesmo quando a compilação dava certo.

Problemas que antes eram limitações desta etapa e também foram resolvidos:

| Limitação | Solução |
|---|---|
| `&&`/`\|\|` avaliavam sempre os dois lados | Curto-circuito com desvios (`gen_logic_begin`) |
| Expressões só com `int`, `float`/`char`/`string` só recebiam constantes, sem menos unário | Tipos `EXPR_INT`/`EXPR_FLOAT` com promoção, aritmética SSE, `char` como inteiro, `-F`, atribuição entre strings (`_rt_strcpy`) |
| `read`/`write` trabalhavam com os bytes crus via `int 0x80` (32 bits), `read` lia do descritor 1 e as strings eram sempre escritas com 16 bytes | Rotinas de apoio com conversão texto↔número via `syscall` de 64 bits, leitura do descritor 0 e escrita até o `\0` |
| `\n` nas strings era escrito literalmente | Sequências de escape, com as constantes entre crases no NASM |
| `gen_win.c` não compilava | Agora inclui `gen.c` com `TARGET_WIN`, gerando código que chama `printf`/`scanf` |

O código compila sem avisos com `gcc -Wall -Wextra`, e o compilador roda sem erros com `-fsanitize=address,undefined` em todos os programas de teste.

### Observações

- `write` de `float` usa 6 casas decimais e funciona para valores até cerca de ±9,2 × 10¹² (o valor é multiplicado por 10⁶ e convertido para inteiro de 64 bits). No Windows a formatação é a do `printf("%f")`, sem esse limite.
- Os nomes das variáveis viram rótulos no assembly sem prefixo, como nas etapas anteriores. Uma variável chamada `rax` ou `main`, por exemplo, conflita com o NASM.
