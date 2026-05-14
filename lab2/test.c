typedef unsigned long uintptr_t;

#define UART_REG_TXFIFO 0x0
#define UART_BASE_ADDR 0x10013000
#define CLINT_BASE_ADDR 0x2000000
#define CLINT_MTIME_OFFSET 0xBFF8
#define CLINT_MTIMECMP_OFFSET 0x4000

void end(void) asm("end");

#define SYS_PUTCHAR 1
#define SYS_TIMER_ON 2

static volatile int *mtime_addr = (int *)(void *)(CLINT_BASE_ADDR + CLINT_MTIME_OFFSET);
static volatile int *mtimecmp_addr = (int *)(void *)(CLINT_BASE_ADDR + CLINT_MTIMECMP_OFFSET);
static volatile int *uart = (int *)(void *)UART_BASE_ADDR;

static long syscall(long n, long arg0, long arg1) {
    register long a0 asm("a0") = n;
    register long a1 asm("a1") = arg0;
    register long a2 asm("a2") = arg1;
    
    asm volatile("ecall" : "+r"(a0) : "r"(a1), "r"(a2) : "memory");
    return a0;
}

static void putchar(char ch) {
    while (uart[UART_REG_TXFIFO] & 0x80000000);
    uart[UART_REG_TXFIFO] = ch & 0xFF;
}

static void puts(const char *str) {
    while (*str) {
        putchar(*str++);
    }
}

static void enable_timer(void) {
    syscall(SYS_TIMER_ON, 0, 0);
}

void timer_setup(int enable);

long handle_trap(long cause, long epc, long regs[32]) {
    int is_interrupt = (cause >> 63) & 1;
    long code = cause & 0xFF;
    
    if (is_interrupt) {
        if (code == 7) {
            puts("Timer interrupt occurred!\n");
            timer_setup(0);
            puts("Timer disabled.\n");
        } else {
            puts("Unknown interrupt!\n");
            end();
        }
    } else {
        if (code == 8) {
            long syscall_num = regs[10];
            
            if (syscall_num == 1) {
                char ch = regs[11];
                while (uart[UART_REG_TXFIFO] & 0x80000000);
                uart[UART_REG_TXFIFO] = ch & 0xFF;
                return epc + 4;
            }
            else if (syscall_num == 2) {
                timer_setup(1);
                return epc + 4;
            }
            else {
                puts("Unknown syscall!\n");
                end();
            }
        } else {
            puts("Unknown exception!\n");
            end();
        }
    }
    
    return epc;
}

void main(void) {
    puts("Hello from Frygat! Lab2\n");
    puts("Enabling timer...\n");
    enable_timer();
    puts("Waiting for timer interrupt (WFI)...\n");
    asm volatile("wfi");
    puts("Program finished!\n");
    end();
}