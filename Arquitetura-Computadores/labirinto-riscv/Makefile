# Compila o labirinto para o Emulsiv (RV32I, sem biblioteca C) e gera o
# jogo.hex (Intel HEX) que é carregado no emulador.

CROSS   := riscv64-unknown-elf
AS      := $(CROSS)-as
LD      := $(CROSS)-ld
OBJCOPY := $(CROSS)-objcopy
OBJDUMP := $(CROSS)-objdump

ASFLAGS := -march=rv32i -mabi=ilp32
LDFLAGS := -m elf32lriscv -T emulsiv.ld

.PHONY: all clean dump

all: jogo.hex

jogo.hex: jogo.elf
	$(OBJCOPY) -O ihex $< $@

jogo.elf: startup.o jogo.o emulsiv.ld
	$(LD) $(LDFLAGS) startup.o jogo.o -o $@

%.o: %.s
	$(AS) $(ASFLAGS) $< -o $@

dump: jogo.elf
	$(OBJDUMP) -d $<

clean:
	rm -f *.o jogo.elf jogo.hex
