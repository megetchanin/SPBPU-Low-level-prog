#include "sw_timer.h"

typedef unsigned long  uintptr_t;
typedef unsigned long  uint64_t;  
typedef long           int64_t;

#define MTIMECMP_ADDR   0x02004000UL
#define MTIME_ADDR      0x0200BFF8UL


#define MIE_MTIE   (1UL << 7)

#define MCAUSE_INTERRUPT_BIT      (1UL << (__riscv_xlen - 1))
#define MCAUSE_MACHINE_TIMER_IRQ  (MCAUSE_INTERRUPT_BIT | 7UL)
#define MCAUSE_ECALL_FROM_U       8UL

#ifndef MAX_SOFT_TIMERS
#define MAX_SOFT_TIMERS 8
#endif

#define SW_TIMER_SECTION __attribute__((section(".sw_timer")))


typedef struct {
    sw_timer_type_t  type;
    long             ticks_period;   
    long             ticks;         
    timer_callback_t callback;
    void            *arg;
    bool             is_active;
    unsigned long    times_to_shot;  
} sw_timer_t;

static sw_timer_t timers[MAX_SOFT_TIMERS];
static uint64_t   resolution = 0;

static volatile uint64_t * const mtime    = (volatile uint64_t *)MTIME_ADDR;
static volatile uint64_t * const mtimecmp = (volatile uint64_t *)MTIMECMP_ADDR;

extern void timer_setup(void);


static int syscalls(unsigned long arg0, unsigned long arg1,
                    unsigned long arg2, unsigned long arg3,
                    unsigned long arg4, unsigned long arg5,
                    unsigned long arg6, unsigned long arg7)
{
    register uintptr_t a0 asm("a0") = (uintptr_t)(arg0);
    register uintptr_t a1 asm("a1") = (uintptr_t)(arg1);
    register uintptr_t a2 asm("a2") = (uintptr_t)(arg2);
    register uintptr_t a3 asm("a3") = (uintptr_t)(arg3);
    register uintptr_t a4 asm("a4") = (uintptr_t)(arg4);
    register uintptr_t a5 asm("a5") = (uintptr_t)(arg5);
    register uintptr_t a6 asm("a6") = (uintptr_t)(arg6);
    register uintptr_t a7 asm("a7") = (uintptr_t)(arg7);

    asm volatile("ecall"
        : "+r"(a0), "+r"(a1)
        : "r"(a2), "r"(a3), "r"(a4), "r"(a5), "r"(a6), "r"(a7)
        : "memory");

    return (int)a0;
}

SW_TIMER_SECTION
static void update_hardware_timer(void)
{
    int any_active = 0;
    for (int i = 0; i < MAX_SOFT_TIMERS; ++i) {
        if (timers[i].is_active) { any_active = 1; break; }
    }

    if (!any_active) {
        unsigned long mie;
        asm volatile("csrr %0, mie" : "=r"(mie));
        mie &= ~MIE_MTIE;
        asm volatile("csrw mie, %0" :: "r"(mie));
        return;
    }

    *mtimecmp = *mtime + resolution;
}

SW_TIMER_SECTION
static int do_sw_timer_add(unsigned long ticks,
                           sw_timer_type_t type,
                           unsigned long times_to_shot,
                           timer_callback_t cb,
                           void *arg)
{
    int idx = -1;
    for (int i = 0; i < MAX_SOFT_TIMERS; ++i) {
        if (!timers[i].is_active) { idx = i; break; }
    }
    if (idx < 0) return -1;

    timers[idx].type          = type;
    timers[idx].ticks_period  = (long)ticks;
    timers[idx].ticks         = (long)ticks;
    timers[idx].callback      = cb;
    timers[idx].arg           = arg;
    timers[idx].times_to_shot = times_to_shot;
    timers[idx].is_active     = true;

    timer_setup();
    update_hardware_timer();

    return idx;
}

SW_TIMER_SECTION
static int do_sw_timer_remove(sw_timer_handle_t idx)
{
    if (idx < 0 || idx >= MAX_SOFT_TIMERS) return -1;
    timers[idx].is_active = false;
    update_hardware_timer();
    return 0;
}

SW_TIMER_SECTION
static int do_sw_timer_change(sw_timer_handle_t idx, unsigned long new_ticks)
{
    if (idx < 0 || idx >= MAX_SOFT_TIMERS) return -1;
    if (!timers[idx].is_active)            return -1;
    timers[idx].ticks_period = (long)new_ticks;
    timers[idx].ticks        = (long)new_ticks;
    return 0;
}

int sw_timer_init(unsigned long res)
{
    // Сбрасываем таблицу таймеров 
    for (int i = 0; i < MAX_SOFT_TIMERS; ++i) {
        timers[i].is_active = false;
    }
    resolution = res;
    return 0;
}

sw_timer_handle_t sw_timer_add(unsigned long ticks,
                               sw_timer_type_t type,
                               unsigned long times_to_shot,
                               timer_callback_t callback_fun,
                               void *arg)
{
    // a7 = номер syscall, a0..a4 — аргументы 
    return syscalls((unsigned long)ticks,
                    (unsigned long)type,
                    (unsigned long)times_to_shot,
                    (unsigned long)callback_fun,
                    (unsigned long)arg,
                    0, 0,
                    SW_SYSCALL_ADD);
}

bool sw_timer_is_active(sw_timer_handle_t index)
{
    if (index < 0 || index >= MAX_SOFT_TIMERS) return false;
    return timers[index].is_active;
}

int sw_timer_remove(sw_timer_handle_t index)
{
    return syscalls((unsigned long)index, 0, 0, 0, 0, 0, 0, SW_SYSCALL_REMOVE);
}

int sw_timer_change(sw_timer_handle_t index, unsigned long new_ticks)
{
    return syscalls((unsigned long)index, (unsigned long)new_ticks,
                    0, 0, 0, 0, 0, SW_SYSCALL_CHANGE);
}


SW_TIMER_SECTION
uintptr_t handle_trap(uintptr_t mcause, uintptr_t mepc, uintptr_t *sp_saved);

SW_TIMER_SECTION
uintptr_t handle_trap(uintptr_t mcause, uintptr_t mepc, uintptr_t *sp_saved)
{
    if (mcause == MCAUSE_MACHINE_TIMER_IRQ) {
        // прерывание по таймеру 
        sw_timer_irq_handler();
        return mepc;   
    }

    if (mcause == MCAUSE_ECALL_FROM_U) {
        // Системный вызов из U mode
        unsigned long a0 = sp_saved[10];
        unsigned long a1 = sp_saved[11];
        unsigned long a2 = sp_saved[12];
        unsigned long a3 = sp_saved[13];
        unsigned long a4 = sp_saved[14];
        unsigned long a7 = sp_saved[17];

        long result = -1;
        switch (a7) {
        case SW_SYSCALL_ADD:
            result = do_sw_timer_add((unsigned long)a0,
                                     (sw_timer_type_t)a1,
                                     (unsigned long)a2,
                                     (timer_callback_t)a3,
                                     (void *)a4);
            break;
        case SW_SYSCALL_REMOVE:
            result = do_sw_timer_remove((sw_timer_handle_t)a0);
            break;
        case SW_SYSCALL_CHANGE:
            result = do_sw_timer_change((sw_timer_handle_t)a0,
                                        (unsigned long)a1);
            break;
        default:
            result = -1;
            break;
        }

        // Возрат a0 пользователю
        sp_saved[10] = (uintptr_t)result;

        // Сдвигаем на 4 байта чтобы не зациклиться
        return mepc + 4;
    }

    return mepc + 4;
}

SW_TIMER_SECTION
void sw_timer_irq_handler(void)
{
    for (int i = 0; i < MAX_SOFT_TIMERS; ++i) {
        if (!timers[i].is_active) continue;

        timers[i].ticks -= (long)resolution;
        if (timers[i].ticks > 0) continue;

        // Дергаем callback по истечению времени
        if (timers[i].callback) {
            timers[i].callback(timers[i].arg);
        }

        // Отключить или перезарядить
        switch (timers[i].type) {
        case ONESHOT:
            timers[i].is_active = false;
            break;

        case MULTISHOT:
            if (timers[i].times_to_shot > 1) {
                timers[i].times_to_shot -= 1;
                timers[i].ticks = timers[i].ticks_period;
            } else {
                timers[i].is_active = false;
            }
            break;

        case PERIODIC:
        default:
            timers[i].ticks = timers[i].ticks_period;
            break;
        }
    }

    update_hardware_timer();
}
