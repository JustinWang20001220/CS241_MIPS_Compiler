lis $3
.word 0
lis $2
.word 10
loop:
    div $1, $2
    mfhi $4
    add $3, $4, $3
    mflo $1
    bne $0, $1, loop
jr $31