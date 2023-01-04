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
beq $15, $0, skip ; skip it transforming to positive
    sub $20, $0, $13 ; Reg20 = -1
    mult $11, $20 ;
    mflo $11 ; move the positive result into Reg11
    add $20, $0, $0 ; restore Reg20 to 0
skip:
add $17, $0, $0 ; Reg17 equal to 0 from start memory location changing
lis $18 ; Reg18 is always 4
.word 4
lis $19 ; Reg19 is ascii for -
.word 45
storing:
    div $11, $14 ; divide the current by 10
    mfhi $21 ; Reg21 remainder
    mflo $11 ; update the remaining digit
    add $21, $21, $16 ; get the ascii value
    sw $21, 0($17) ; store digit in the memory location
    add $17, $17, $18 ; increment the memory pointer
    bne $11, $0, storing ;
breaking:
beq $15, $0, skiip ; skip adding "-" if it is positive
    sw $19, 0($17); store - in the memory location
    add $17, $17, $18 ; increment the memory pointer
skiip:
print:
    sub $17, $17, $18 ; decrement the memory pointer
    lw $29, 0($17) ; load the ascii character into REG30
    sw $29, 0($10) ; print the character
    bne $17, $0, print
lis $9 ; newline charactor
.word 10
sw $9, 0($10)
jr $31
