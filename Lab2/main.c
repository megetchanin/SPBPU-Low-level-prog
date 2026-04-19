#include <stdint.h>

// Адреса для машины 'virt'
#define UART_BASE_ADDR 0x10000000
#define UART_REG_TXFIFO 0x0
#define CLINT_BASE_ADDR 0x2000000
#define CLINT_MTIME_OFFSET 0xBFF8
#define CLINT_MTIMECMP_OFFSET 0x4000

#define SYSCALL_PUTCHAR 1
#define SYSCALL_TIMER_ON 2

void end(void) asm("end") __attribute__((noreturn));
extern void timer_setup(int enable);

// Флаг для гарантии ожидания 
volatile int timer_fired = 0;

static volatile uint64_t *mtime_addr = (uint64_t *)(void *)(CLINT_BASE_ADDR + CLINT_MTIME_OFFSET);
static volatile uint64_t *mtimecmp_addr = (uint64_t *)(void *)(CLINT_BASE_ADDR + CLINT_MTIMECMP_OFFSET);
static volatile uint32_t *uart_tx = (uint32_t *)(void *)(UART_BASE_ADDR + UART_REG_TXFIFO);

static long syscall(long a0, long a1, long a2, long a3,
                    long a4, long a5, long a6, long a7) {
    register long r_a0 asm("a0") = a0;
    register long r_a1 asm("a1") = a1;
    register long r_a2 asm("a2") = a2;
    register long r_a3 asm("a3") = a3;
    register long r_a4 asm("a4") = a4;
    register long r_a5 asm("a5") = a5;
    register long r_a6 asm("a6") = a6;
    register long r_a7 asm("a7") = a7;

    asm volatile("ecall"
        : "+r"(r_a0), "+r"(r_a1)
        : "r"(r_a2), "r"(r_a3), "r"(r_a4), "r"(r_a5), "r"(r_a6), "r"(r_a7)
        : "memory");
    return r_a0;
}

static void putchar_mmode(char ch) {
    while ((*uart_tx >> 31) & 1); // Ждем готовности UART
    *uart_tx = ch;
}

long handle_trap(long cause, long epc, long regs[32]) {
    if (cause < 0) { // Прерывание
        if ((cause & 0x3F) == 7) { // Timer IRQ
            // Код 7 = Machine Timer Interrupt
            // Вывод маркера, доказывающего, что таймер сработал
            const char proof[] = "TIMER_IRQ!\n";
            for (int i = 0; proof[i]; i++) putchar_mmode(proof[i]);
            
            timer_setup(0);              // Отключаем прерывания
            *mtimecmp_addr = (uint64_t)-1; // Ставим таймер на MAX
            timer_fired = 1;             // Поднимаем флаг для C-кода
        } else {
            putchar_mmode('I'); putchar_mmode('?');
            end();
        }
    } else { // Исключение
        if (cause == 8) { // ecall
            long num = regs[17];
            if (num == SYSCALL_PUTCHAR) {
                putchar_mmode((char)regs[10]);
            } else if (num == SYSCALL_TIMER_ON) {
                timer_setup(1);
            } else {
                putchar_mmode('E'); putchar_mmode('?');
                end();
            }
            epc += 4;
        } else if (cause == 2) { // Illegal Instruction (wfi)
            epc += 4;
        } else {
            putchar_mmode('X'); putchar_mmode('?');
            end();
        }
    }
    return epc;
}

static void print(const char *s) {
    while (*s) {
        syscall(*s, 0, 0, 0, 0, 0, 0, SYSCALL_PUTCHAR);
        s++;
    }
}

void main(void) {
    print("Hello world from Danila Rylov!!!\n");
    
    // 1. Сначала ставим время (через 5 секунд = 50 млн тиков)
    *mtimecmp_addr = *mtime_addr + 50000000;
    
    // 2. Включаем прерывания
    syscall(0, 0, 0, 0, 0, 0, 0, SYSCALL_TIMER_ON);
    
    // 3. Спим
    asm volatile("wfi");
    
    // Ждем установки флага 
    while (!timer_fired) {
        asm volatile("nop");
    }
    
    print("Program finished\n");
    end();
}