PROG_NAME       = sw_timer_demo
RISCV64_GCC     = riscv64-unknown-elf-gcc
OBJDUMP         = riscv64-elf-objdump

# Число слотов в массиве таймеров — задаётся через макрос, как требует задание
MAX_SOFT_TIMERS ?= 8

CFLAGS  = -march=rv64g -mabi=lp64 -static -mcmodel=medany \
          -fvisibility=hidden -fno-builtin \
          -nostdlib -nostartfiles -g \
          -ffunction-sections -fdata-sections \
          -DMAX_SOFT_TIMERS=$(MAX_SOFT_TIMERS)

LDFLAGS = -T linker.ld -Wl,--gc-sections

.PHONY: all clean qemu qemu-debug dump

all: $(PROG_NAME)

start.o: start.S
	$(RISCV64_GCC) $(CFLAGS) -c start.S -o start.o

sw_timer.o: sw_timer.c sw_timer.h
	$(RISCV64_GCC) $(CFLAGS) -c sw_timer.c -o sw_timer.o

main.o: main.c sw_timer.h
	$(RISCV64_GCC) $(CFLAGS) -c main.c -o main.o

$(PROG_NAME): start.o sw_timer.o main.o linker.ld
	$(RISCV64_GCC) $(CFLAGS) -o $@ start.o sw_timer.o main.o $(LDFLAGS)

# Дизассемблер — удобно смотреть, куда попала секция .sw_timer
dump: $(PROG_NAME)
	$(OBJDUMP) -D $(PROG_NAME) > $(PROG_NAME).dump

# Запуск в QEMU
qemu: $(PROG_NAME)
	qemu-system-riscv64 -machine sifive_u -nographic -bios none -kernel $(PROG_NAME)

# Запуск в режиме отладки (gdb)
qemu-debug: $(PROG_NAME)
	qemu-system-riscv64 -machine sifive_u -nographic -bios none -S -s -kernel $(PROG_NAME)

clean:
	rm -f *.o *.dump $(PROG_NAME)