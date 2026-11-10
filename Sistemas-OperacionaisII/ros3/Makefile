CROSS   = riscv64-unknown-elf-
CC      = $(CROSS)gcc
OBJDUMP = $(CROSS)objdump

CFLAGS  = -march=rv64imac_zicsr -mabi=lp64 -mcmodel=medany \
          -ffreestanding -nostdlib -O0 -g -Wall -Wextra
LDFLAGS = -T linker.ld -nostdlib

OBJS = boot.o main.o uart.o

all: kernel.elf

kernel.elf: $(OBJS) linker.ld
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.S
	$(CC) $(CFLAGS) -c $< -o $@

QEMU = qemu-system-riscv64 -machine virt -bios none -nographic -kernel kernel.elf

# Para sair do QEMU: Ctrl-A depois X
run: kernel.elf
	$(QEMU)

# Espera o GDB na porta 1234; em outro terminal: ./dev make -C baremetal gdb
debug: kernel.elf
	$(QEMU) -s -S

gdb:
	gdb-multiarch kernel.elf -ex "target remote :1234"

disasm: kernel.elf
	$(OBJDUMP) -d kernel.elf

clean:
	rm -f *.o kernel.elf

.PHONY: all run debug gdb disasm clean
