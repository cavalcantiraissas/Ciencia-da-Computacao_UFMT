.PHONY: testes executar clean

compiler: lex.o synt.o gen.o symbols.o
	gcc -o compiler lex.o synt.o gen.o symbols.o -I. -g

# gera codigo para Windows x86-64 (nasm -f win64 + gcc, entrada/saida com printf/scanf)
compiler_win: lex.o synt.o gen_win.o symbols.o
	gcc -o compiler4win lex.o synt.o gen_win.o symbols.o -I. -g

# gera codigo para macOS x86-64 (nasm -f macho64 + clang)
compiler_macos: lex.o synt.o gen_macos.o symbols.o
	gcc -o compiler4macos lex.o synt.o gen_macos.o symbols.o -I. -g

lex.o: lex.c
	gcc -c -g -Wall lex.c

synt.o: synt.c
	gcc -c -g -Wall synt.c

gen.o: gen.c
	gcc -c -g -Wall gen.c

gen_win.o: gen_win.c gen.c
	gcc -c -g -Wall gen_win.c

gen_macos.o: gen_macos.c gen.c
	gcc -c -g -Wall gen_macos.c

symbols.o: symbols.c
	gcc -c -g -Wall symbols.c

tstLex: lex.o tst_lex.o
	gcc -o tstLexCompiler lex.o tst_lex.o -I. -g

tst_lex.o: tst_lex.c
	gcc -c -g -Wall tst_lex.c

# compila todos os programas de teste: os de testes/ devem compilar e os de
# testes/erros/ devem ser rejeitados pelo compilador
testes: compiler
	@for f in testes/*.cc; do ./compiler $$f || exit 1; done
	@for f in testes/erros/*.cc; do echo "== $$f"; ! ./compiler $$f || exit 1; done

# compila, monta (nasm), liga e executa os testes, comparando com a saida esperada
# (Linux x86-64; em macOS x86-64 use 'make compiler_macos' antes)
executar: compiler
	sh testes/rodar.sh

clean:
	rm -f *.o compiler compiler4win compiler4macos testes/*.asm testes/erros/*.asm
