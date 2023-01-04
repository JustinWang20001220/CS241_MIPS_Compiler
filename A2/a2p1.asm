lis $10 ; Reg10: output location
.word 0xffff000c
add $11, $1, $0 ; Reg11: the quetient changing
add $12, $0, $0 ; Reg12: the length of the decimal string
lis $13 ; Reg13 will always be 1
.word 1
lis $14 ; Reg14 will always be 10
.word 10
loop:
    div $



