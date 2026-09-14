.text
.set noreorder
.set nomacro

.align 2
.globl MMathSpecialVectorSub
.ent MMathSpecialVectorSub
MMathSpecialVectorSub:
    daddu $2,$4,$0
    lui $8,0x4d
    addiu $8,$8,-14016
    movz $6,$8,$6
    lui $1,0x3b00
    mtc1 $1,$f8
    sll $0,$0,0x0
    mul.s $f8,$f8,$f12
    mfc1 $8,$f8
    qmtc2 $8,vf4
    lqc2 vf1,0($5)
    lqc2 vf2,0($6)
    vmr32.xyzw vf3xyzw,vf0xyzw
    vsub.xyz vf1xyz,vf3xyz,vf1xyz
    vmulax.xyz ACCxyz,vf1xyz,vf4x
    vmaddw.xyz vf1xyz,vf2xyz,vf0w
    sqc2 vf1,0($2)
    jr $31
    sll $0,$0,0x0
.end MMathSpecialVectorSub
