PROG_NAME = test
RISCV64_GCC = riscv64-linux-gnu-gcc
GCC_FLAGS = -march=rv64g -mabi=lp64 -static -mcmodel=medany -fvisibility=hidden -fno-builtin -nostdlib -nostartfiles -g -DMAX_SOFT_TIMERS=10

LD_FLAGS = -T linker.ld -Wl,--gc-sections

.PHONY: all clean
all: $(PROG_NAME)

start.o: start.S
	$(RISCV64_GCC) $(GCC_FLAGS) -c start.S

sw_timer.o: sw_timer.c sw_timer.h
	$(RISCV64_GCC) $(GCC_FLAGS) -ffunction-sections -fdata-sections -c sw_timer.c

test.o: test.c sw_timer.h
	$(RISCV64_GCC) $(GCC_FLAGS) -ffunction-sections -fdata-sections -c test.c

$(PROG_NAME): start.o sw_timer.o test.o
	$(RISCV64_GCC) $(GCC_FLAGS) -o $@ start.o sw_timer.o test.o $(LD_FLAGS)

clean:
	rm -f *.o $(PROG_NAME)