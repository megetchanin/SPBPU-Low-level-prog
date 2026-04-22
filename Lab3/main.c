#include "sw_timer.h"

#define UART_REG_TXFIFO 0x0
#define UART_BASE_ADDR  0x10010000

void end(void) asm("end");

static volatile int *uart = (int *)(void *)UART_BASE_ADDR;

static int putchar(char ch)
{
    while (uart[UART_REG_TXFIFO] < 0);
    return uart[UART_REG_TXFIFO] = ch & 0xFF;
}

static void puts_raw(const char *s)
{
    while (*s) putchar(*s++);
}

// Callback'и для таймеров
static void cb_oneshot(void *arg)
{
    (void)arg;
    puts_raw("ONESHOT  timer fired\r\n");
}

static void cb_multishot(void *arg)
{
    (void)arg;
    puts_raw("MULTISHOT timer fired\r\n");
}

static void cb_periodic(void *arg)
{
    (void)arg;
    puts_raw("PERIODIC timer fired\r\n");
}


void main(void)
{
    puts_raw("\r\n=== sw_timer demo started ===\r\n");

    sw_timer_init(1000000UL);

    sw_timer_add(3000000UL, ONESHOT, 1, cb_oneshot, 0);

    sw_timer_add(5000000UL, MULTISHOT, 3, cb_multishot, 0);

    sw_timer_add(7000000UL, PERIODIC, 0, cb_periodic, 0);

    puts_raw("timers armed, entering idle loop\r\n");

    for (;;) {
        asm volatile("wfi");
    }

    end();
}
