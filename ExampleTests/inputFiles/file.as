.entry W
.extern V
   add $15,$27,$31
    sub $3,$23,$19
    and $2,$5,$13
    or $23,$5,$30
    nor $21,$23,$25
MAIN: move $10,$20
    mvhi $3,$13
    mvlo $21,$22
F: addi $4,-44,$23
 subi $31,564,$12
 andi $12,157,$21
 .entry C
 ori $1,-234,$23
 nori $13,324,$2
 blt $3,$22,MAIN
 .extern Z
 X: beq $23,$10,F
 C: bne $21,$22,F
  bgt $9,$8,MAIN
R: lb $4,345,$7
 sb $15,231,$2
 lw $30,-332,$31
 sw $12,345,$3
 lh $31,3423,$3
 jmp Z
 sh $12,-23,$23
 CNT: jmp $2
  jmp V
  la F
  call V
  W: .db 1,34,76,24
   .dh 23,45,75,32
   .dw 21314,32423,5645,675
STR: .asciz "nadav pivonia"
END: stop
