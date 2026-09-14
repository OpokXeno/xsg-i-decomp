# Readable standalone EE assembly hypothesis; no instruction words are embedded.
# Register, lane, memory, control-flow, and eligibility notes are in notes.md.
.text
.set noreorder
.set nomacro
.set noat


# xglQuaternionInterpolateLinear retains its original ABI and complete instruction schedule.
.align 2
.globl xglQuaternionInterpolateLinear
.ent xglQuaternionInterpolateLinear
xglQuaternionInterpolateLinear:
    mfc1	$3,$f12
    mfc1	$7,$f13
    qmtc2	$3,vf10
    qmtc2	$7,vf11
    lqc2	vf20,0($5)
    lqc2	vf21,0($6)
    vmul.xyzw	vf18xyzw,vf20xyzw,vf21xyzw
    addiu	$1,$0,0
    mtc1	$1,$f0
    vsubw.x	vf1x,vf0x,vf0w
    vaddy.x	vf23x,vf0x,vf18y
    vaddz.x	vf24x,vf0x,vf18z
    vaddw.x	vf25x,vf0x,vf18w
    vmulaw.x	ACCx,vf18x,vf0w
    vmaddaw.x	ACCx,vf23x,vf0w
    vmaddaw.x	ACCx,vf24x,vf0w
    vmaddw.x	vf26x,vf25x,vf0w
    qmfc2	$2,vf26
    mtc1	$2,$f1
    sll	$0,$0,0x0
    c.lt.s	$f0,$f1
    sll	$0,$0,0x0
    bc1t	L_0022aa34
    sll	$0,$0,0x0
    vmulx.xyzw	vf21xyzw,vf21xyzw,vf1x
L_0022aa34:
    vmulx.xyzw	vf20xyzw,vf20xyzw,vf10x
    vmulx.xyzw	vf21xyzw,vf21xyzw,vf11x
    vadd.xyzw	vf22xyzw,vf20xyzw,vf21xyzw
    vmul.xyzw	vf19xyzw,vf22xyzw,vf22xyzw
    vaddy.x	vf23x,vf0x,vf19y
    vaddz.x	vf24x,vf0x,vf19z
    vaddw.x	vf25x,vf0x,vf19w
    vmulaw.x	ACCx,vf19x,vf0w
    vmaddaw.x	ACCx,vf23x,vf0w
    vmaddaw.x	ACCx,vf24x,vf0w
    vmaddw.x	vf26x,vf25x,vf0w
    vrsqrt	Q,vf0w,vf26x
    vwaitq
    vmulq.xyzw	vf22xyzw,vf22xyzw,Q
    sqc2	vf22,0($4)
    jr	$31
    sll	$0,$0,0x0
.end xglQuaternionInterpolateLinear
