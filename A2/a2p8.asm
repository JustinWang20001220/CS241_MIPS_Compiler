sw $31, -4($30)
lis $31
.word 4

sub $30, $30, $31
lis $20 ; used to test for base case
.word -1 
lis $21
.word 2; right index diff
lis $22
.word 4; $22 for memory arithematic
lis $23
.word maxheight
lis $24
.word 1 ;left index diff
add $13, $0, $0; staring index
jalr $23

lis $31
.word 4
add $30, $30, $31
lw $31, -4($30)
jr $31

maxheight:
    sw $10, -4($30); $10 intermediate comparitor
    sw $11, -8($30); $11 left index
    sw $12, -12($30); $12 right index
    sw $13, -16($30); $13 root index
    sw $14, -20($30); stack pointer changing size
    sw $15, -24($30); address 
    sw $31, -28($30);
    lis $14
    .word 28
    sub $30, $30, $14

    ; base case
    bne $13, $20, recursive
    add $3, $0, $0
    beq $0, $0, end

    recursive:
    add $11, $13, $24; get left index 
    add $12, $13, $21; get right index

    mult $11, $22
    mflo $11
    add $15, $11, $1
    lw $13, 0($15)
    jalr $23
    add $10, $3, $0

    mult $12, $22
    mflo $12
    add $15, $12, $1
    lw $13, 0($15)
    jalr $23

    slt $12, $10, $3
    beq $12, $24, 1
    add $3, $10, $0
    add $3, $3, $24

end:
    lis $14
    .word 28
    add $30, $30, $14
    lw $10, -4($30); $10 intermediate comparitor
    lw $11, -8($30); $11 left index
    lw $12, -12($30); $12 right index
    lw $13, -16($30); $13 root index
    lw $14, -20($30); stack pointer changing size
    lw $15, -24($30); address 
    lw $31, -28($30);
    jr $31