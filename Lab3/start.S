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
_start:
    csrr    t0, mhartid             # read current hart id
    bnez    t0, park                # single core only, park hart != 0

    la      sp, stack_top           # setup stack

    # -------------------------------------------------------------
    # PMP setup: закрываем CLINT (MTIME/MTIMECMP) от U-mode,
    # оставляем доступ ко всей остальной памяти/MMIO.
    #
    # pmp0: TOR, base = 0,           top = 0x02000000 -> R|W|X = 0x0F (TOR=1 -> 0x0F | 0x08 = 0x0F)
    # pmp1: TOR, base = 0x02000000,  top = 0x02010000 -> нет прав (A=TOR=0x08)
    # pmp2: TOR, base = 0x02010000,  top = 0xFFFFFFFFFFFFFFFF -> R|W|X
    #
    # pmpcfgN layout (1 byte per region):
    #   bit0 R, bit1 W, bit2 X, bit3..4 A (OFF=0, TOR=1, NA4=2, NAPOT=3), bit7 L
    # TOR + RWX  = 0x0F
    # TOR + none = 0x08
    # -------------------------------------------------------------

    li   t0, (0x02000000 >> 2)
    csrw pmpaddr0, t0
    li   t0, (0x02010000 >> 2)
    csrw pmpaddr1, t0
    li   t0, -1
    csrw pmpaddr2, t0

    # pmpcfg0 = 0x0F | (0x08 << 8) | (0x0F << 16)
    li   t0, 0x000F080F
    csrw pmpcfg0, t0

    # set trap_vector
    la   t0, trap_vector
    csrw mtvec, t0

    # mstatus setup:
    #   MPP  = 00  (U-mode после mret)
    #   MPIE = 1   (прерывания включатся после mret)
    #   MIE  = 0   (пока в M-mode — без прерываний)
    # Биты mstatus: MIE=3, MPIE=7, MPP[1:0]=11..12
    csrr t0, mstatus
    li   t1, (3 << 11)              # маска MPP
    not  t1, t1
    and  t0, t0, t1                 # MPP = 00
    li   t1, (1 << 7)               # MPIE
    or   t0, t0, t1
    csrw mstatus, t0

    # Входим в main в U-моде
    la   t0, main
    csrw mepc, t0
    mret

.global timer_setup
timer_setup:
    # Включаем бит MTIE (7) в регистре mie
    li   t0, (1 << 7)
    csrs mie, t0
    ret

.align 6
trap_vector:
  addi sp, sp, -32*REGBYTES
  SREG x1, 1*REGBYTES(sp)
  SREG x2, 2*REGBYTES(sp)
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
  csrr a0, mcause
  csrr a1, mepc
  mv a2, sp
  la x1, handle_trap
  jalr x1
  csrw mepc, a0
  LREG x1, 1*REGBYTES(sp)
  LREG x2, 2*REGBYTES(sp)
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
    j       park

.global end
end:
    j end
