.section .text
.global _start
_start:
    li t0, 0x10013000
    li t1, 79
    sb t1, 0(t0)
    li t1, 75
    sb t1, 0(t0)
loop:
    j loop
