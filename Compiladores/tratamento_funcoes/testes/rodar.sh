#!/bin/sh
# Compila, monta, liga e executa cada programa de teste, comparando a saida
# com o arquivo <programa>.out (quando existir). A entrada padrao vem de
# <programa>.in (quando existir). Funciona em Linux x86-64 (compilador padrao)
# e em macOS x86-64 (compilador gerado com 'make compiler_macos').
#
# Uso (na pasta tratamento_funcoes): sh testes/rodar.sh
cd "$(dirname "$0")" || exit 1
tmp=$(mktemp -d)
trap 'rm -rf "$tmp"' EXIT

case "$(uname -s)" in
    Darwin) compiler=../compiler4macos; format=macho64 ;;
    *)      compiler=../compiler;       format=elf64 ;;
esac

falhas=0
for src in *.cc; do
    nome=${src%.cc}
    if ! $compiler "$src" > "$tmp/compilacao.txt"; then
        echo "FALHOU (compilacao): $src"; cat "$tmp/compilacao.txt"; falhas=$((falhas+1)); continue
    fi
    nasm -f $format "$src.asm" -o "$tmp/$nome.o" || { echo "FALHOU (nasm): $src"; falhas=$((falhas+1)); continue; }
    if [ "$format" = macho64 ]; then
        clang -arch x86_64 -nostartfiles -e _start "$tmp/$nome.o" -o "$tmp/$nome" 2>/dev/null
    else
        ld -m elf_x86_64 "$tmp/$nome.o" -o "$tmp/$nome"
    fi || { echo "FALHOU (ligacao): $src"; falhas=$((falhas+1)); continue; }

    entrada=/dev/null
    [ -f "$nome.in" ] && entrada="$nome.in"
    "$tmp/$nome" < "$entrada" > "$tmp/$nome.saida"
    if [ -f "$nome.out" ] && ! diff "$nome.out" "$tmp/$nome.saida" > "$tmp/diff.txt"; then
        echo "FALHOU (saida diferente): $src"; cat "$tmp/diff.txt"; falhas=$((falhas+1)); continue
    fi
    echo "ok: $src"
done
rm -f ./*.asm
[ $falhas -eq 0 ] && echo "Todos os testes passaram." || echo "$falhas teste(s) falharam."
[ $falhas -eq 0 ]
