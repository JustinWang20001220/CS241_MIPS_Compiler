mylabel1:
beq $3, $5, 123
bne $7, $23, -345
beq $3, $5, mylabel1
bne $7, $23, 0x4444
mylabel2:
.word 000
