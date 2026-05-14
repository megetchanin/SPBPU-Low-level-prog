#include "sw_timer.h"

#define UART_REG_TXFIFO 0x0
#define UART_BASE_ADDR 0x10013000

void end(void) asm("end");

static volatile int *uart = (int *)(void *)UART_BASE_ADDR;

static void putchar(char ch) {
    uart[UART_REG_TXFIFO] = ch & 0xFF;
}

static void puts(const char *str) {
    while (*str) putchar(*str++);
}

void callback1(void *arg) {
    puts("One-shot timer triggered!\n");
}

void callback2(void *arg) {
    puts("Multi-shot timer triggered!\n");
}

void callback3(void *arg) {
    puts("Periodic timer triggered!\n");
}

long handle_trap(long cause, long epc, long regs[32]) {
    int is_interrupt = (cause >> 63) & 1;
    long code = cause & 0xFF;
    
    if (is_interrupt) {
        if (code == 7) {
            sw_timer_irq_handler();
            return epc;
        }
    } else {
        if (code == 8) {
            long syscall_num = regs[10];
            if (syscall_num == 1) {
                volatile uint64_t *mtimecmp = (volatile uint64_t *)0x02004000;
                volatile uint64_t *mtime = (volatile uint64_t *)0x0200BFF8;
                unsigned long ticks = regs[11];
                *mtimecmp = *mtime + ticks;
                asm volatile("csrs mie, %0" : : "r"(0x80));
                return epc + 4;
            }
        }
    }
    return epc;
}

void main(void) {
    puts("Lab3: Software Timer\n");
    
    sw_timer_init(1000);
    
    sw_timer_add(5, ONESHOT, 1, callback1, NULL);
    sw_timer_add(10, MULTISHOT, 3, callback2, NULL);
    sw_timer_add(15, PERIODIC, 0, callback3, NULL);
    
    puts("Timers added. Waiting...\n");
    
    while(1) {
        asm volatile("wfi");
    }
    
    end();
}