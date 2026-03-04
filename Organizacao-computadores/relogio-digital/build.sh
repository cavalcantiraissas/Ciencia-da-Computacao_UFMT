#!/bin/bash
# ============================================================
# build.sh — Compila o projeto dentro do container Docker
# ============================================================
set -e

IMAGE="relogio-riscv"

echo ">>> Construindo imagem Docker..."
docker build -t "$IMAGE" .

echo ""
echo ">>> Compilando projeto RISC-V..."
docker run --rm -v "$(pwd):/work" "$IMAGE" make clean all

echo ""
echo ">>> Build concluido com sucesso!"
echo "    Arquivos gerados: clock.elf  clock.bin"
