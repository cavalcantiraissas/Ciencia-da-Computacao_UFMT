FROM ubuntu:22.04
ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get install -y \
    gcc-riscv64-unknown-elf \
    qemu-system-misc \
    make \
    && rm -rf /var/lib/apt/lists/*
WORKDIR /project
COPY clock.S link.ld Makefile .
RUN make all
CMD ["make", "run"]