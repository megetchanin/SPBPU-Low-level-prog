start:
li a5, 0
li a6, 1
li a7, 2
li s3, 3
li s2, 4


li t0, 0xFF01
li t1, 0xFF
li t2, 0xFFFF00

0:
la a0, LED_MATRIX_0_BASE
la s0, SWITCHES_0_BASE
lw s1, 0(s0)

beq s1, a5, zeroo
beq s1, a6, first
beq s1, a7, second
beq s1, s3, third
bge s1, s2, end

zeroo:
sw zero, 0(a0)
j 0b

first:
sw t0, 0(a0)
sw zero, 0(a0)
j 0b

second:
sw t1, 0(a0)
sw zero, 0(a0)
j 0b

third:
sw t2, 0(a0)
sw zero, 0(a0)
j 0b

end:
    sw zero, 0(a0)
    j end