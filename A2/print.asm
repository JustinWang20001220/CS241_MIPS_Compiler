print:
sw $10, -4($30) 
sw $12, -8($30)
sw $13, -12($30)
sw $14, -16($30)
sw $15, -20($30)
sw $16, -24($30)
sw $17, -28($30)
sw $18, -32($30)
sw $19, -36($30)
sw $20, -40($30)
sw $21, -44($30)
sw $29, -48($30)
sw $1, -52($30)
sw $11, -56($30)
sw $9, -60($30)
sw $3, -64($30)
sw $25, -68($30)
lis $3
.word 68
sub $30, $30, $3

lis $19 ; Reg19 is ascii for -
.word 45
lis $10 ; Reg10: output location
.word 0xffff000c
add $11, $1, $0 ; Reg11: the quetient changing
add $12, $0, $0 ; Reg12: the length of the decimal string
lis $13 ; Reg13 will always be 1
.word 1
lis $14 ; Reg14 will always be 10
.word 10
slt $15, $11, $0 ; Reg15 is 1 if negative number, 0 if positive
lis $16 ; Reg16 ; the difference between ascii and actual number
.word 48
lis $18 ; Reg18 is always 4
.word 4
beq $15, $0, skip ; skip it transforming to positive
    sw $19, 0($10) ; print the '-'
    sub $20, $0, $13 ; Reg20 = -1
    mult $11, $20 ;
    mflo $11 ; move the positive result into Reg11
    add $20, $0, $0 ; restore Reg20 to 0
skip:
sub $30, $30, $18 ; update the stack pointer to a empty location
add $25, $30, $0 ; remeber the og stack pointer
storing:
    div $11, $14 ; divide the current by 10
    mfhi $21 ; Reg21 remainder
    mflo $11 ; update the remaining digit
    add $21, $21, $16 ; get the ascii value
    sw $21, 0($30) ; store digit in the memory location
    sub $30, $30, $18 ; increment the memory pointer
    bne $11, $0, storing ;
printing:
    add $30, $30, $18 ; decrement the memory pointer
    lw $29, 0($30) ; load the ascii character into REG30
    sw $29, 0($10) ; print the character
    bne $30, $25, printing
add $30, $30, $18 
lis $9 ; newline charactor
.word 10
sw $9, 0($10)

add $30, $30, $3
lw $10, -4($30) 
lw $12, -8($30)
lw $13, -12($30)
lw $14, -16($30)
lw $15, -20($30)
lw $16, -24($30)
lw $17, -28($30)
lw $18, -32($30)
lw $19, -36($30)
lw $20, -40($30)
lw $21, -44($30)
lw $29, -48($30)
lw $1, -52($30)
lw $11, -56($30)
lw $9, -60($30)
lw $3, -64($30)
lw $25, -68($30)
jr $31