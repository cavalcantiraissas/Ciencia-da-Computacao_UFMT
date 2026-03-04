# ============================================================
# Dockerfile — Ambiente de build e execução
#
# Contém:
#   - gcc-riscv64-unknown-elf : toolchain bare metal RISC-V 64
#   - qemu-system-misc        : inclui qemu-system-riscv64
#   - make                    : sistema de build
#
# Uso:
#   docker build -t relogio-riscv .
#   docker run --rm -it -v $(pwd):/work relogio-riscv make run
# ============================================================

FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
        gcc-riscv64-unknown-elf          \
        binutils-riscv64-unknown-elf     \
        qemu-system-misc                 \
        make                             \
    && rm -rf /var/lib/apt/lists/*

# Verificar instalação das ferramentas
RUN riscv64-unknown-elf-gcc --version && \
    qemu-system-riscv64 --version

WORKDIR /work

# Comando padrão: compilar
CMD ["make"]
