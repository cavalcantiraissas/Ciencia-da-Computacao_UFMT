#!/bin/bash
# ============================================================
# run.sh — Executa o relógio no QEMU dentro do container
#
# Como usar:
#   ./run.sh
#
# Para sair do QEMU:
#   Pressione Ctrl+A, depois X
#
# Para ajustar a hora, digite diretamente no terminal:
#   T HH:MM:SS  e pressione Enter
#   Exemplo:  T 14:30:00
# ============================================================
set -e

IMAGE="relogio-riscv"

# Compilar se necessário
if [ ! -f clock.elf ]; then
    echo ">>> clock.elf nao encontrado, compilando..."
    docker run --rm -v "$(pwd):/work" "$IMAGE" make
fi

echo ""
echo ">>> Iniciando QEMU com o relogio RISC-V..."
echo "    Para sair: Ctrl+A, depois X"
echo "    Para ajustar hora: T HH:MM:SS  (ex: T 14:30:00)"
echo ""

# -it: modo interativo (terminal) para receber digitação do usuario
docker run --rm -it -v "$(pwd):/work" "$IMAGE" \
    qemu-system-riscv64     \
        -machine virt        \
        -nographic           \
        -bios none           \
        -kernel clock.elf
