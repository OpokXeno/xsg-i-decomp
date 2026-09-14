.text
.set noreorder
.globl sefCaclAllTarget
.ent sefCaclAllTarget
sefCaclAllTarget:
    addiu $29,$29,-16
    move $7,$0
    sd $31,0($29)
    sqc2 $vf0,0($4)
    lui $3,0x79
    addiu $3,$3,17280
    lw $2,3472($3)
    slt $2,$7,$2
    beqz $2,no_targets
    move $8,$0
    move $6,$3
    nop
target_loop:
    lw $5,128($6)
    beqzl $5,target_next
    lw $2,3472($3)
    lh $2,132($6)
    bnezl $2,target_next
    lw $2,3472($3)
    addiu $2,$5,16
    lqc2 $vf1,0($4)
    lqc2 $vf2,0($2)
    vadd.xyz $vf1xyz,$vf1xyz,$vf2xyz
    sqc2 $vf1,0($4)
    addiu $7,$7,1
    lw $2,3472($3)
target_next:
    addiu $8,$8,1
    slt $2,$8,$2
    bnez $2,target_loop
    addiu $6,$6,144
no_targets:
    beqz $7,return_without_division
    ld $31,0($29)
    mtc1 $7,$f12
    nop
    cvt.s.w $f12,$f12
    move $5,$4
    j MMathDivVectorS
    addiu $29,$29,16
    nop
return_without_division:
    jr $31
    addiu $29,$29,16
.end sefCaclAllTarget
.align 3
