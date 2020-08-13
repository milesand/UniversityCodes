.data
X:    .word 7
A:    .word 3
B:    .word 4
C:    .word 5
ans:  .asciiz "answer = "
endl: .asciiz "\n"

.text
.globl main
.ent main
main:
    lw $t0, X
    lw $t1, A
    lw $t2, B
    lw $t3, C
    mul $t4, $t0, $t0
    mul $t4, $t4, $t1
    mul $t5, $t2, $t0
    add $t4, $t4, $t5
    add $t4, $t4, $t3
    
    la $a0, ans
    li $v0, 4
    syscall
    
    move $a0, $t4
    li $v0, 1
    syscall
    
    la $a0, endl
    li $v0, 4
    syscall
    
    li $v0, 10
    syscall
.end main
