
#include <stdint.h>
#include <stdbool.h>
#include "sw_timer.h"

#define MTIME_ADDR      0x0200BFF8
#define MTIMECMP_ADDR   0x02004000
#define SYS_TIMER_ADD   1

typedef struct {
    sw_timer_type_t type;
    long ticks_period;
    long ticks_left;
    void (*callback)(void*);
    void *arg;
    bool is_active;
    unsigned long times_to_shot;
    unsigned long shot_count;
} sw_timer_t;

static sw_timer_t timers[MAX_SOFT_TIMERS];
static uint64_t resolution = 0;
static volatile uint64_t * const mtime = (volatile uint64_t *)MTIME_ADDR;
static volatile uint64_t * const mtimecmp = (volatile uint64_t *)MTIMECMP_ADDR;

static long syscall(long n, long arg0, long arg1, long arg2) {
    register long a0 asm("a0") = n;
    register long a1 asm("a1") = arg0;
    register long a2 asm("a2") = arg1;
    register long a3 asm("a3") = arg2;
    
    asm volatile("ecall" : "+r"(a0) : "r"(a1), "r"(a2), "r"(a3) : "memory");
    return a0;
}

int sw_timer_init(unsigned long res) {
    resolution = res;
    for (int i = 0; i < MAX_SOFT_TIMERS; i++) {
        timers[i].is_active = false;
    }
    return 0;
}

sw_timer_handle_t sw_timer_add(unsigned long ticks, sw_timer_type_t type, unsigned long times_to_shot, timer_callback_t callback_fun, void *arg) {
    for (int i = 0; i < MAX_SOFT_TIMERS; i++) {
        if (!timers[i].is_active) {
            timers[i].type = type;
            timers[i].ticks_period = ticks;
            timers[i].ticks_left = ticks;
            timers[i].callback = callback_fun;
            timers[i].arg = arg;
            timers[i].is_active = true;
            timers[i].times_to_shot = times_to_shot;
            timers[i].shot_count = 0;
            
            syscall(SYS_TIMER_ADD, ticks, type, times_to_shot);
            return i;
        }
    }
    return -1;
}

bool sw_timer_is_active(sw_timer_handle_t index) {
    if (index >= 0 && index < MAX_SOFT_TIMERS) {
        return timers[index].is_active;
    }
    return false;
}

static void update_hardware_timer(void) {
    uint64_t next_time = *mtime + resolution;
    for (int i = 0; i < MAX_SOFT_TIMERS; i++) {
        if (timers[i].is_active) {
            uint64_t timer_expire = *mtime + timers[i].ticks_left * resolution;
            if (timer_expire < next_time) {
                next_time = timer_expire;
            }
        }
    }
    *mtimecmp = next_time;
}

void sw_timer_irq_handler(void) {
    uint64_t current_time = *mtime;
    bool any_active = false;
    
    for (int i = 0; i < MAX_SOFT_TIMERS; i++) {
        if (!timers[i].is_active) continue;
        
        any_active = true;
        
        if (current_time >= *mtimecmp) {
            if (timers[i].callback) {
                timers[i].callback(timers[i].arg);
            }
            
            timers[i].shot_count++;
            
            if (timers[i].type == ONESHOT) {
                timers[i].is_active = false;
            } 
            else if (timers[i].type == MULTISHOT) {
                if (timers[i].shot_count >= timers[i].times_to_shot) {
                    timers[i].is_active = false;
                } else {
                    timers[i].ticks_left = timers[i].ticks_period;
                }
            }
            else if (timers[i].type == PERIODIC) {
                timers[i].ticks_left = timers[i].ticks_period;
            }
        }
    }
    
    any_active = false;
    for (int i = 0; i < MAX_SOFT_TIMERS; i++) {
        if (timers[i].is_active) {
            any_active = true;
            break;
        }
    }
    
    if (any_active) {
        update_hardware_timer();
    } else {
        asm volatile("csrc mie, %0" : : "r"(0x80));
    }
}
