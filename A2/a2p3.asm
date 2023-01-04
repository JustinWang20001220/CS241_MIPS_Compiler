lis $10 ; iterator add memory address 4 each time
.word 4
lis $11 ; comparitor with slt and count deductor
.word 1
add $5, $1, $0 ; store the moving address pointer in $5
lw $6, 0($1); store the current maximum in $6
add $7, $2, $0 ; number of element in reg7
loop:
    lw $20, 0($5) ; store the current value in registor 20
    slt $21, $6, $20
    bne $21, $11, 1
    add $6, $20, $0
    add $5, $5, $10
    sub $7, $7, $11
    bne $7, $0, loop
add $3, $6, $0
jr $31


