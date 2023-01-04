lis $10 ; iterator add memory address 4 each time
.word 4
lis $11 ; comparitor with slt and count deductor
.word 1
add $5, $1, $0 ; store the moving address pointer in $5
add $7, $2, $0 ; number of element in reg7
lis $6
.word 32
lis $9
.word 0xffff000c
beq $2, $0, end
loop:
    lw $20, 0($5) ; store the current value in registor 20
    add $20, $20, $6
    beq $20, $6, 1
    add $20, $20, $6
    sw $20, 0($9)
    add $5, $5, $10
    sub $7, $7, $11
    bne $7, $0, loop
end:
jr $31