#ifndef SW_TIMER_H
#define SW_TIMER_H

typedef int  bool;
#define true  1
#define false 0

typedef void (*timer_callback_t)(void *arg);

typedef int sw_timer_handle_t;

typedef enum {
    ONESHOT,     // Сработает один раз
    MULTISHOT,   // Сработает три раза
    PERIODIC     // Работает бесконечное количество раз
} sw_timer_type_t;

// Коды системных вызовов 
#define SW_SYSCALL_ADD     1
#define SW_SYSCALL_REMOVE  2
#define SW_SYSCALL_CHANGE  3

// API
int sw_timer_init(unsigned long resolution);

sw_timer_handle_t sw_timer_add(unsigned long ticks,
                               sw_timer_type_t type,
                               unsigned long times_to_shot,
                               timer_callback_t callback_fun,
                               void *arg);

bool sw_timer_is_active(sw_timer_handle_t index);

// На плюсик
int sw_timer_remove(sw_timer_handle_t index);
int sw_timer_change(sw_timer_handle_t index, unsigned long new_ticks);

#endif 
