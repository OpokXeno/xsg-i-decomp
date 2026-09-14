.set noreorder
.set noat
.text
.ent fnECM01_DP000
fnECM01_DP000:
    addiu $29, $29, -0x120
    lui $2, %hi(mefCamParams)
    sd $16, 0xc0($29)
    addiu $2, $2, %lo(mefCamParams)
    sd $18, 0xd0($29)
    daddu $18, $5, $0
    sd $23, 0xf8($29)
    addiu $16, $18, 0x16c
    sd $17, 0xc8($29)
    daddu $5, $0, $0
    sd $19, 0xd8($29)
    daddu $4, $16, $0
    sd $20, 0xe0($29)
    daddu $6, $0, $0
    sd $21, 0xe8($29)
    daddu $23, $16, $0
    sd $22, 0xf0($29)
    sd $30, 0x100($29)
    sd $31, 0x108($29)
    swc1 $f21, 0x118($29)
    swc1 $f20, 0x110($29)
    lw $3, 0x4($2)
    sw $3, 0xa4($29)
    lw $2, 0x0($2)
    jal MGsGPInit
    sw $2, 0xa0($29)
    lw $2, 0x8($16)
    lui $5, 0x1000
    dsll32 $5, $5, 0
    ori $5, $5, 0x8000
    addiu $4, $0, 0x47
    addiu $6, $0, 0xe
    addiu $3, $2, 0x10
    addiu $9, $0, 0x1
    lui $8, 0x5
    ori $8, $8, 0xd
    sd $5, 0x0($2)
    sd $6, 0x8($2)
    addiu $13, $2, 0x30
    sw $9, 0x10($16)
    addiu $7, $2, 0x20
    sd $8, 0x0($3)
    addiu $10, $0, 0x4e
    sb $4, 0x8($3)
    addiu $4, $0, -0x1
    addiu $2, $18, 0x80
    addiu $3, $18, 0xa0
    ori $11, $0, 0x9880
    dsll $11, $11, 17
    addiu $12, $0, 0x42
    addiu $14, $0, 0x48
    sw $4, 0xa8($29)
    sb $10, 0x8($7)
    addiu $15, $0, 0x3
    sw $2, 0xb4($29)
    sd $11, 0x0($7)
    sw $3, 0xb0($29)
    sb $12, 0x8($13)
    sw $23, 0xac($29)
    sd $14, 0x0($13)
    lw $3, 0x8($16)
    lw $5, 0x10($16)
    lhu $2, 0x0($3)
    mult $5, $5, $15
    ori $2, $2, 0x3
    sh $2, 0x0($3)
    addiu $5, $5, 0x1
    lw $3, 0x8($16)
    lw $4, 0xc($16)
    sll $2, $5, 4
    addu $3, $3, $2
    addu $4, $4, $5
    sw $3, 0x8($16)
    sw $4, 0xc($16)
    lq $2, 0xe0($18)
    nop
L00A37E18:
    daddu $30, $0, $0
    lq $3, 0xd0($18)
    daddu $20, $0, $0
    sq $2, 0x10($29)
    addiu $4, $29, 0x20
    sq $3, 0x0($29)
    lw $2, 0xb4($29)
    lwc1 $f21, 0xc0($18)
    lq $8, 0x0($2)
    sq $8, 0x0($4)
    lw $3, 0xac($29)
    addiu $4, $0, 0x5151
    daddu $21, $0, $0
    addiu $22, $18, 0xf0
    lw $2, 0x8($3)
    lui $3, 0x4026
    ori $3, $3, 0x4000
    dsll32 $3, $3, 0
    ori $3, $3, 0x8000
    sd $4, 0x8($2)
    addiu $4, $0, 0x4
    sd $3, 0x0($2)
    addiu $19, $2, 0x10
    addiu $2, $0, 0x120
    lw $3, 0xac($29)
    sw $2, 0xb8($29)
    sw $4, 0x10($3)
    lwc1 $f0, 0xcc($18)
    lwc1 $f1, 0x70($18)
    cvt.s.w $f1, $f1
    mul.s $f0, $f0, $f1
    swc1 $f0, 0xc8($18)
L00A37E98:
    lh $3, 0x0($22)
    beqz $3, L00A38050
    addiu $2, $0, 0x2
    lwc1 $f0, 0xa8($29)
    cvt.s.w $f0, $f0
    lui $1, 0x3e99
    ori $1, $1, 0x999a
    mtc1 $1, $f1
    bne $3, $2, L00A37EC4
    mul.s $f20, $f0, $f1
    mtc1 $0, $f20
L00A37EC4:
    lw $2, 0xb8($29)
    sll $4, $21, 3
    addu $3, $18, $2
    addiu $2, $0, 0xff
    lh $5, 0xe($3)
    subu $4, $2, $4
    slti $3, $5, 0x28
    bnez $3, L00A37F0C
    sw $4, 0xc($29)
    addiu $2, $0, 0x3c
    addiu $3, $0, 0x14
    subu $2, $2, $5
    beql $3, $0, L00A37EFC
    break 0, 7
L00A37EFC:
    mult $2, $4, $2
    divu $0, $2, $3
    mflo $2
    sw $2, 0xc($29)
L00A37F0C:
    addiu $17, $29, 0x30
    daddu $5, $0, $0
    daddu $4, $17, $0
    jal MMathRotateMatrixYX
    addiu $6, $18, 0xb0
    addiu $2, $29, 0x20
    lqc2 $vf1, 0x0($2)
    vmove.w $vf1, $vf0
    sqc2 $vf1, 0x30($17)
    lw $2, 0x70($18)
    addiu $16, $29, 0x70
    lwc1 $f2, 0xcc($18)
    subu $2, $2, $21
    lwc1 $f1, 0xc8($18)
    mtc1 $2, $f0
    nop
    cvt.s.w $f0, $f0
    mul.s $f2, $f2, $f0
    add.s $f1, $f1, $f2
    sqc2 $vf0, 0x0($16)
    mfc1 $8, $f1
    qmtc2.ni $8, $vf4
    vcallms 0x20
    qmfc2.i $8, $vf1
    mtc1 $8, $f1
    nop
    mul.s $f1, $f21, $f1
    daddu $4, $16, $0
    daddu $5, $17, $0
    daddu $6, $16, $0
    jal MMathApplyMatrix
    swc1 $f1, 0x74($29)
    lqc2 $vf1, 0x0($16)
    vmove.w $vf1, $vf0
    sqc2 $vf1, 0x30($17)
    lqc2 $vf1, 0x30($17)
    vmove.w $vf1, $vf0
    sqc2 $vf1, 0x0($16)
    lw $5, 0xa0($29)
    addiu $4, $29, 0x80
    lw $6, 0xa4($29)
    jal MMathRotTransPersClip
    daddu $7, $16, $0
    beqz $2, L00A37FC8
    nop
    b L00A37FD8
    addiu $20, $0, 0x2
L00A37FC8:
    beqz $20, L00A37FD8
    ori $3, $0, 0x8000
    addiu $20, $20, -0x1
    sw $3, 0x8c($29)
L00A37FD8:
    sqc2 $vf0, 0x0($16)
    daddu $5, $17, $0
    daddu $4, $16, $0
    daddu $6, $16, $0
    jal MMathApplyMatrix
    swc1 $f20, 0x74($29)
    daddu $7, $16, $0
    lw $5, 0xa0($29)
    addiu $4, $29, 0x90
    jal MMathRotTransPersClip
    lw $6, 0xa4($29)
    beqz $2, L00A38018
    nop
    b L00A38028
    addiu $20, $0, 0x2
    nop
L00A38018:
    beqz $20, L00A38028
    ori $4, $0, 0x8000
    addiu $20, $20, -0x1
    sw $4, 0x9c($29)
L00A38028:
    lq $3, 0x0($29)
    addiu $30, $30, 0x1
    sq $3, 0x0($19)
    lq $2, 0x80($29)
    sq $2, 0x10($19)
    lq $3, 0x10($29)
    sq $3, 0x20($19)
    lq $2, 0x90($29)
    sq $2, 0x30($19)
    addiu $19, $19, 0x40
L00A38050:
    lw $2, 0xb8($29)
    addiu $22, $22, 0x2
    addiu $21, $21, 0x1
    addiu $3, $29, 0x20
    addiu $2, $2, 0x2
    lw $4, 0xb0($29)
    sw $2, 0xb8($29)
    lqc2 $vf1, 0x0($3)
    lqc2 $vf2, 0x0($4)
    vadd.xyz $vf1, $vf1, $vf2
    sqc2 $vf1, 0x0($3)
    lwc1 $f0, 0xc4($18)
    slti $2, $21, 0x1f
    bnez $2, L00A37E98
    add.s $f21, $f21, $f0
    blez $30, L00A380D0
    lw $2, 0xa8($29)
    lw $3, 0x8($23)
    lw $5, 0x10($23)
    lhu $2, 0x0($3)
    mult $5, $5, $30
    or $2, $2, $30
    sh $2, 0x0($3)
    addiu $5, $5, 0x1
    lw $3, 0x8($23)
    lw $4, 0xc($23)
    sll $2, $5, 4
    addu $3, $3, $2
    addu $4, $4, $5
    sw $3, 0x8($23)
    sw $4, 0xc($23)
    lw $2, 0xa8($29)
L00A380D0:
    addiu $2, $2, 0x2
    sw $2, 0xa8($29)
    slti $2, $2, 0x2
    bnel $2, $0, L00A37E18
    lq $2, 0xe0($18)
    jal MGsGPTerm
    daddu $4, $23, $0
    ld $16, 0xc0($29)
    ld $17, 0xc8($29)
    ld $18, 0xd0($29)
    ld $19, 0xd8($29)
    ld $20, 0xe0($29)
    ld $21, 0xe8($29)
    ld $22, 0xf0($29)
    ld $23, 0xf8($29)
    ld $30, 0x100($29)
    ld $31, 0x108($29)
    lwc1 $f21, 0x118($29)
    lwc1 $f20, 0x110($29)
    jr $31
    addiu $29, $29, 0x120
.end fnECM01_DP000
.align 3
