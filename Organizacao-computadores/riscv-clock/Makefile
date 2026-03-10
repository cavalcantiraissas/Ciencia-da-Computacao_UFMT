CROSS    ?= riscv64-unknown-elf
CC        = $(CROSS)-gcc
OBJDUMP   = $(CROSS)-objdump

TARGET    = clock
SRC       = clock.S
LDSCRIPT  = link.ld

QEMU      = qemu-system-riscv32
QFLAGS    = -machine virt -nographic -bios none -kernel $(TARGET).elf -serial mon:stdio

.PHONY: all run clean dump

all: $(TARGET).elf

$(TARGET).elf: $(SRC) $(LDSCRIPT)
	$(CC) -march=rv32ima -mabi=ilp32 -nostdlib -nostartfiles -T$(LDSCRIPT) -o $@ $(SRC) -lgcc

run: $(TARGET).elf
	$(QEMU) $(QFLAGS)

dump: $(TARGET).elf
	$(OBJDUMP) -d -M no-aliases $<

clean:
	rm -f $(TARGET).elf
