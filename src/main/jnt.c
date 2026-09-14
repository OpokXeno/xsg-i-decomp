#include "common.h"
#include "jnt.h"

/*
 * The JNT producer's work area is in the EE scratchpad (SPR, 0x70000000).
 * Two of its slots are named here, because this file's two recovered
 * functions are their writers; the area itself is not recovered, so the
 * slots are named relative to a base the caller materializes rather than
 * modeled as members (see the block comment on JNT_setMatrix).
 *
 * The base has to stay a value the function holds: folding it into each
 * store's address instead (`*(T *)0x700007a0`) makes 2.96 emit the two
 * absolute stores as separate li/sw pairs, four instructions in place of the
 * original's single shared `lui $2,0x7000` and two displaced stores.
 */
#define JNT_SCRATCH_BASE ((unsigned char *)0x70000000)
#define JNT_MATRIX_BUFFER_OFFSET 0x7A0
#define JNT_MATRIX_SELECT_OFFSET 0x4BC
#define JNT_MATRIX_BUFFER(scratch) \
    (*(JntMatrixBuffer *)((scratch) + JNT_MATRIX_BUFFER_OFFSET))
#define JNT_MATRIX_SELECT(scratch) \
    (*(JntMatrixSelect *)((scratch) + JNT_MATRIX_SELECT_OFFSET))

INCLUDE_ASM("asm/main/nonmatchings/jnt", get_fcvpack);

INCLUDE_ASM("asm/main/nonmatchings/jnt", get_fcvpack_fix);

INCLUDE_ASM("asm/main/nonmatchings/jnt", get_fcvpack_flag);

INCLUDE_ASM("asm/main/nonmatchings/jnt", JNT_getStaticVal);

INCLUDE_ASM("asm/main/nonmatchings/jnt", FCV_getAttrAndValue);

INCLUDE_ASM("asm/main/nonmatchings/jnt", JNT_getVal);

INCLUDE_ASM("asm/main/nonmatchings/jnt", JNT_getVal_staticChain);

INCLUDE_ASM("asm/main/nonmatchings/jnt", JNT_constructMatrix);

INCLUDE_ASM("asm/main/nonmatchings/jnt", JNT_getStaticVal2);

INCLUDE_ASM("asm/main/nonmatchings/jnt", JNT_getVal2);

INCLUDE_ASM("asm/main/nonmatchings/jnt", JNT_getVal_HOGE2);

INCLUDE_ASM("asm/main/nonmatchings/jnt", liner_interpolate);

INCLUDE_ASM("asm/main/nonmatchings/jnt", JNT_interpolate);

INCLUDE_ASM("asm/main/nonmatchings/jnt", JNT_setFlags);

INCLUDE_ASM("asm/main/nonmatchings/jnt", JNT_setClipR);

INCLUDE_ASM("asm/main/nonmatchings/jnt", JNT_animSetFlags);

INCLUDE_ASM("asm/main/nonmatchings/jnt", JNT_getFlags);

INCLUDE_ASM("asm/main/nonmatchings/jnt", JNT_getAccessories);

INCLUDE_ASM("asm/main/nonmatchings/jnt", JNT_readAttribute);

/* JNT matrix-slot setters.
 *
 *
 * Semantics (facts):
 * - JNT_setMatrix stores a0 at scratchpad 0x700007A0 and zero at 0x700004BC:
 *     lui $v0,0x7000 / sw $a0,1952($v0) / jr $ra / sw $zero,1212($v0).
 * - JNT_setMatrix2 stores a1 at 0x700004BC and a0 at 0x700007A0, the buffer
 *   store filling the jr delay slot:
 *     lui $v0,0x7000 / sw $a1,1212($v0) / jr $ra / sw $a0,1952($v0).
 * - Caller ACT_resetMatrix (bounded jal sites at VA 0x00307620/0x0030765C)
 *   supplies caller object words +0x824/+0x828 (lw a0,2084/2088(s1) in the
 *   jal delay slots) as the buffer argument, and the surrounding reset path
 *   clears select-adjacent scratchpad words; that is the evidence for the
 *   0x7A0 buffer slot / 0x4BC select slot split. No unobserved workspace
 *   byte is asserted.
 *
 */

void JNT_setMatrix(JntMatrixBuffer matrix_buffer)
{
    unsigned char *scratch = JNT_SCRATCH_BASE;
    JNT_MATRIX_BUFFER(scratch) = matrix_buffer;
    JNT_MATRIX_SELECT(scratch) = 0;
}

void JNT_setMatrix2(JntMatrixBuffer matrix_buffer, JntMatrixSelect matrix_select)
{
    unsigned char *scratch = JNT_SCRATCH_BASE;
    JNT_MATRIX_BUFFER(scratch) = matrix_buffer;
    JNT_MATRIX_SELECT(scratch) = matrix_select;
}

INCLUDE_ASM("asm/main/nonmatchings/jnt", JNT_setInterpMatrix);

INCLUDE_ASM("asm/main/nonmatchings/jnt", JNT_setCurve);

INCLUDE_ASM("asm/main/nonmatchings/jnt", JNT_setMDLMatrix);

INCLUDE_ASM("asm/main/nonmatchings/jnt", JNT_setModelMatrix);

INCLUDE_ASM("asm/main/nonmatchings/jnt", JNT_getMoveElement);

INCLUDE_ASM("asm/main/nonmatchings/jnt", JNT_getRootElement);

INCLUDE_ASM("asm/main/nonmatchings/jnt", JNT_setModel);

INCLUDE_ASM("asm/main/nonmatchings/jnt", JNT_setFCurve);

INCLUDE_ASM("asm/main/nonmatchings/jnt", JNT_setFCurve2);

INCLUDE_ASM("asm/main/nonmatchings/jnt", JNT_getRootTrans);

INCLUDE_ASM("asm/main/nonmatchings/jnt", JNT_getRootRotate);

INCLUDE_ASM("asm/main/nonmatchings/jnt", JNT_getRootScale);

INCLUDE_ASM("asm/main/nonmatchings/jnt", JNT_addConsumer);

INCLUDE_ASM("asm/main/nonmatchings/jnt", JNT_initProducer);

INCLUDE_ASM("asm/main/nonmatchings/jnt", JNT_startProduction);

INCLUDE_ASM("asm/main/nonmatchings/jnt", JNT_getElement);

INCLUDE_ASM("asm/main/nonmatchings/jnt", JNT_nextElement);

INCLUDE_ASM("asm/main/nonmatchings/jnt", JNT_defaultConsumer);

INCLUDE_ASM("asm/main/nonmatchings/jnt", JNT_resetHair);

INCLUDE_ASM("asm/main/nonmatchings/jnt", JNT_hairID_00314318);

INCLUDE_ASM("asm/main/nonmatchings/jnt", JNT_computeHair);

INCLUDE_ASM("asm/main/nonmatchings/jnt", JNT_setInterrupt);

INCLUDE_ASM("asm/main/nonmatchings/jnt", JNT_getFilter);

INCLUDE_ASM("asm/main/nonmatchings/jnt", JNT_resetMatrix);

INCLUDE_ASM("asm/main/nonmatchings/jnt", JNT_computeMatrix);

INCLUDE_ASM("asm/main/nonmatchings/jnt", JNT_onSmoothHair);

INCLUDE_ASM("asm/main/nonmatchings/jnt", JNT_offSmoothHair);
