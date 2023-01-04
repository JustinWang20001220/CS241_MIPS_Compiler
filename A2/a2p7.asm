; $3 will be storing all the result of f(n, k)
stirling:
    sw $1, -4($30); these are two input 
    sw $2, -8($30); ---
    sw $10, -12($30); $10 = 1 to decrement recursive call
    sw $11, -16($30); $11 is the intermediate value (n-1) * f(n-1,k)
    sw $12, -20($30); $12 = stirling
    sw $13, -24($30); $13 = end
    sw $31, -28($30);
    lis $31
    .word 28
    sub $30, $30, $31

    lis $10
    .word 1
    lis $12
    .word stirling
    lis $13
    .word end

    ; checking for base case
    beq $1, $0, checkforsecond
    ; after $1 is not zero
    beq $2, $0, singlezero
    ; both $1 and $2 not zero
    beq $0, $0, recursive
    
    checkforsecond:
        beq $2, $0, doublezero
        beq $0, $0, singlezero

    doublezero:
        add $3, $0, $10 ; make $3 = 1
        beq $0, $0, end
    singlezero:
        add $3, $0, $0 ; make $3 = 0
        beq $0, $0, end

    
    recursive:
    sub $1, $1, $10 ; sub first parameter 1
    jalr $12; $3 = f(n-1, k)
    multu $1, $3; (n-1) * f(n-1, k)
    mflo $11; $11 = (n-1)f(n-1, k)
    sub $2, $2, $10; k = k - 1
    jalr $12; calling $3 = f(n-1, k-1)
    add $3, $11, $3; $3 = (n-1) * f(n-1,k) + f(n-1,k-1)

end:
    lis $31
    .word 28
    add $30, $30, $31
    lw $1, -4($30)
    lw $2, -8($30)
    lw $10, -12($30)
    lw $11, -16($30)
    lw $12, -20($30)
    lw $13, -24($30)
    lw $31, -28($30)
    jr $31




