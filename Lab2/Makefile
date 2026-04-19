# Toolchain
RISCV_PREFIX = riscv64-unknown-elf-
CC = $(RISCV_PREFIX)gcc
AS = $(RISCV_PREFIX)as
LD = $(RISCV_PREFIX)ld
OBJCOPY = $(RISCV_PREFIX)objcopy

# Flags - для 32-битной архитектуры
ARCH = rv32im_zicsr
ABI = ilp32
CFLAGS = -march=$(ARCH) -mabi=$(ABI) -nostdlib -ffreestanding -O2 -Wall -Wextra -mcmodel=medany
ASFLAGS = -march=$(ARCH)
LDFLAGS = -m elf32lriscv -T linker.ld -nostdlib

# Targets
TARGET = lab2
OBJS = start.o main.o

all: $(TARGET).elf $(TARGET).bin

%.o: %.S
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET).elf: $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $^

$(TARGET).bin: $(TARGET).elf
	$(OBJCOPY) -O binary $< $@

clean:
	rm -f *.o *.elf *.bin

run: $(TARGET).elf
	qemu-system-riscv32 -M virt -nographic -bios none -kernel $<

debug: $(TARGET).elf
	qemu-system-riscv32 -M virt -nographic -bios none -kernel $< -s -S

.PHONY: all clean run debug