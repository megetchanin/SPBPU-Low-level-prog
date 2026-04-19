#if __riscv_xlen == 64
# define LREG         ld
# define SREG         sd
# define REGBYTES     8
#else
# define LREG         lw
# define SREG         sw
# define REGBYTES     4
#endif

.global _start
.global timer_setup
.global trap_vector
.global end

_start:

    // Отсев лишних хартов
    csrr t0, mhartid
    bnez t0, park

    // Инициализация стека
    la sp, stack_top

    // Настройка PMP
    li t0, -1
    csrw pmpaddr0, t0 // покрываем все адр пространство
    li t0, 0x1F 
    csrw pmpcfg0, t0 // разрешаем чтение, запись, исполнение 

    // Вектор прерываний
    la t0, trap_vector // указываем на  trap_vector
    csrw mtvec, t0

    // Переход в U mode через mret
    la t0, main
    csrw mepc, t0 // куда вернуться после mret

    csrr t0, mstatus // Очищаем MPP
    li t1, ~((3 << 11) | (1 << 3)) 
    and t0, t0, t1
    li t1, (1 << 7) | (1 << 3) | 1   // Устанавливаем MPIE(7)=1, MIE(3)=1, UIE(0)=1
    or t0, t0, t1
    csrw mstatus, t0

    mret  // Переход в U-mode на адрес mepc

timer_setup:
    li t0, (1 << 7)       // MTIE (бит 7 в mie)
    beqz a0, disable_timer
    csrs mie, t0 // включить бит
    ret
disable_timer:
    csrc mie, t0 // выключить бит
    ret

trap_vector:

    // Сохраняем регистры на стек

    addi sp, sp, -32*REGBYTES
    SREG x1, 1*REGBYTES(sp)
    SREG x3, 3*REGBYTES(sp)
    SREG x4, 4*REGBYTES(sp)
    SREG x5, 5*REGBYTES(sp)
    SREG x6, 6*REGBYTES(sp)
    SREG x7, 7*REGBYTES(sp)
    SREG x8, 8*REGBYTES(sp)
    SREG x9, 9*REGBYTES(sp)
    SREG x10, 10*REGBYTES(sp)
    SREG x11, 11*REGBYTES(sp)
    SREG x12, 12*REGBYTES(sp)
    SREG x13, 13*REGBYTES(sp)
    SREG x14, 14*REGBYTES(sp)
    SREG x15, 15*REGBYTES(sp)
    SREG x16, 16*REGBYTES(sp)
    SREG x17, 17*REGBYTES(sp)
    SREG x18, 18*REGBYTES(sp)
    SREG x19, 19*REGBYTES(sp)
    SREG x20, 20*REGBYTES(sp)
    SREG x21, 21*REGBYTES(sp)
    SREG x22, 22*REGBYTES(sp)
    SREG x23, 23*REGBYTES(sp)
    SREG x24, 24*REGBYTES(sp)
    SREG x25, 25*REGBYTES(sp)
    SREG x26, 26*REGBYTES(sp)
    SREG x27, 27*REGBYTES(sp)
    SREG x28, 28*REGBYTES(sp)
    SREG x29, 29*REGBYTES(sp)
    SREG x30, 30*REGBYTES(sp)
    SREG x31, 31*REGBYTES(sp)

    // Выхываем C обработчик
    csrr a0, mcause
    csrr a1, mepc
    mv a2, sp
    la t0, handle_trap
    jalr t0
    csrw mepc, a0


    // Восстанавливаем регистры
    LREG x1, 1*REGBYTES(sp)
    LREG x3, 3*REGBYTES(sp)
    LREG x4, 4*REGBYTES(sp)
    LREG x5, 5*REGBYTES(sp)
    LREG x6, 6*REGBYTES(sp)
    LREG x7, 7*REGBYTES(sp)
    LREG x8, 8*REGBYTES(sp)
    LREG x9, 9*REGBYTES(sp)
    LREG x10, 10*REGBYTES(sp)
    LREG x11, 11*REGBYTES(sp)
    LREG x12, 12*REGBYTES(sp)
    LREG x13, 13*REGBYTES(sp)
    LREG x14, 14*REGBYTES(sp)
    LREG x15, 15*REGBYTES(sp)
    LREG x16, 16*REGBYTES(sp)
    LREG x17, 17*REGBYTES(sp)
    LREG x18, 18*REGBYTES(sp)
    LREG x19, 19*REGBYTES(sp)
    LREG x20, 20*REGBYTES(sp)
    LREG x21, 21*REGBYTES(sp)
    LREG x22, 22*REGBYTES(sp)
    LREG x23, 23*REGBYTES(sp)
    LREG x24, 24*REGBYTES(sp)
    LREG x25, 25*REGBYTES(sp)
    LREG x26, 26*REGBYTES(sp)
    LREG x27, 27*REGBYTES(sp)
    LREG x28, 28*REGBYTES(sp)
    LREG x29, 29*REGBYTES(sp)
    LREG x30, 30*REGBYTES(sp)
    LREG x31, 31*REGBYTES(sp)
    addi sp, sp, 32*REGBYTES
    mret

park:
    wfi
    j park

end:
    j end