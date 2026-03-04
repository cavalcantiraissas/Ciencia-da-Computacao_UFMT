# ============================================================
# Makefile — Relógio Digital RISC-V Bare Metal
# ============================================================

# --- Toolchain ---
CROSS   := riscv64-unknown-elf
CC      := $(CROSS)-gcc
AS      := $(CROSS)-gcc
LD      := $(CROSS)-gcc
OBJCOPY := $(CROSS)-objcopy
OBJDUMP := $(CROSS)-objdump

# --- Alvos ---
TARGET  := clock

# --- Flags de compilação ---
# rv64imac_zicsr: RISC-V 64-bit, extensões Integer, Multiply,
#   Atomic, Compressed + Zicsr (CSR instructions).
# medany: modelo de código adequado para endereços em qualquer
#   região de 4 GB (necessário para 0x80000000).
MARCH   := rv64imac
MABI    := lp64

CFLAGS  := -march=$(MARCH)       \
            -mabi=$(MABI)         \
            -mcmodel=medany       \
            -ffreestanding        \
            -fno-common           \
            -fno-stack-protector  \
            -nostdlib             \
            -nostartfiles         \
            -Wall                 \
            -Wextra               \
            -O2                   \
            -g                    \
            -Iinclude

LDFLAGS := -T linker.ld           \
            -nostdlib             \
            -nostartfiles         \
            -march=$(MARCH)       \
            -mabi=$(MABI)         \
            -mcmodel=medany

# --- Fontes ---
C_SRCS  := src/main.c  \
            src/uart.c  \
            src/timer.c \
            src/trap.c  \
            src/plic.c

AS_SRCS := src/start.S

C_OBJS  := $(C_SRCS:.c=.o)
AS_OBJS := $(AS_SRCS:.S=.o)
ALL_OBJS := $(AS_OBJS) $(C_OBJS)

# ============================================================
# Regras
# ============================================================

.PHONY: all clean run dump

all: $(TARGET).elf $(TARGET).bin
	@echo ""
	@echo "Build concluido: $(TARGET).elf e $(TARGET).bin"

# --- ELF ---
$(TARGET).elf: $(ALL_OBJS)
	$(LD) $(LDFLAGS) $(ALL_OBJS) -o $@

# --- BIN (raw binary, útil para inspeção) ---
$(TARGET).bin: $(TARGET).elf
	$(OBJCOPY) -O binary $< $@

# --- Compilar arquivos C ---
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# --- Montar arquivos Assembly ---
%.o: %.S
	$(AS) $(CFLAGS) -c $< -o $@

# --- Executar no QEMU ---
run: $(TARGET).elf
	@echo ""
	@echo "Iniciando QEMU... (para sair: Ctrl+A, depois X)"
	@echo ""
	qemu-system-riscv64          \
	    -machine virt            \
	    -nographic               \
	    -bios none               \
	    -kernel $(TARGET).elf

# --- Desmontar ELF para depuração ---
dump: $(TARGET).elf
	$(OBJDUMP) -d -S $(TARGET).elf | less

# --- Limpar build ---
clean:
	rm -f $(ALL_OBJS) $(TARGET).elf $(TARGET).bin
