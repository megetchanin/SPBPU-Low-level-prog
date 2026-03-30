start:
    
    li a0, LED_MATRIX_0_BASE 
    li a1, D_PAD_0_UP        
    li a2, D_PAD_0_DOWN 
    li a3, D_PAD_0_LEFT      
    li a4, D_PAD_0_RIGHT 
    
    li t0, 0xFF               
    li t1, 1                  
    li t2, 0
    
    sw t0, 0(a0)

while:
    
    lb t2, 0(a3) # Pressed left pad
    beq t2, t1, turn_off
    
    lb t2, 0(a4)
    beq t2, t1, turn_off
    
    lb t2, 0(a1) # up
    beq t2, t1, increment
    
    lb t2, 0(a2) # down
    beq t2, t1, decrement
    
    
        
    j while

increment:
    addi t0, t0, 10
    sw t0, 0(a0)
    j while
    
decrement:
    addi t0, t0, -10
    sw t0, 0(a0)
    j while
    
turn_off:
    li t0, 0
    sw t0, 0(a0)
    

end:
    j end
    