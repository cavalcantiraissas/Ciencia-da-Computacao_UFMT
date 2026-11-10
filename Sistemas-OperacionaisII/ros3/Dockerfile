# Ambiente de desenvolvimento RISC-V (bare metal) para o ros3
FROM debian:stable-slim

RUN apt-get update && apt-get install -y --no-install-recommends \
        gcc-riscv64-unknown-elf \
        binutils-riscv64-unknown-elf \
        qemu-system-misc \
        gdb-multiarch \
        make \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /work
