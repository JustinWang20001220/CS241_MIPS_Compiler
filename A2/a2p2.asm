lis $4
.word 0
lis $5
.word 1
sltu $4, $1, $2
add $3, $1, $0
bne $5, $4, 1
add $3, $2, $0
jr $31