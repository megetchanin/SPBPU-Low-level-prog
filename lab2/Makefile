PROG_NAME 	= test
RISCV64_GCC = riscv64-linux-gnu-gcc
GCC_FLAGS	= -march=rv64g -mabi=lp64 -static -mcmodel=medany \
			  -fvisibility=hidden -fno-builtin \
			  -nostdlib -nostartfiles -g

LD_FLAGS=-T linker.ld

.PHONY: all clean
all: $(PROG_NAME)

start.o: start.S
	$(RISCV64_GCC) $(GCC_FLAGS) -c start.S

test.o: test.c
	$(RISCV64_GCC) $(GCC_FLAGS) -c test.c

$(PROG_NAME): start.o test.o
	$(RISCV64_GCC) $(GCC_FLAGS) -o $@ start.o test.o $(LD_FLAGS)

clean:
	rm -f *.o $(PROG_NAME)