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

/*
 * Three more scratchpad slots share a fixed layout with the pair above, so
 * they are named with a partial struct instead of one offset macro per
 * field: JNT_setInterpMatrix stores its matrix-buffer argument at 0x7A4 and
 * its float argument at 0x800 (interp_matrix / interpolation below).
 *
 * JNT_setCurve stores its two pointer arguments at 0x7A8/0x7AC. Neither is
 * an end pointer: this same TU's JNT_getStaticVal (VA 0x0030EDC8) and
 * JNT_getVal (VA 0x0030F038) read them back as two independent per-element
 * 32-byte record tables, each gated on both slots being non-null and then
 * indexed by element_index*32 (the element's own 0x2C field):
 *   - JNT_getStaticVal: `sll $3,$11,5 / addu $3,$3,$5` with
 *     $5 = lw 0x7A8($10) -- 0x7A8 is the table it reads, static_value_records.
 *   - JNT_getVal: `sll $2,$6,5 / addu $3,$2,$3` with
 *     $3 = lw 0x7AC($17) -- 0x7AC is the table it reads, value_records.
 */
typedef struct JntWork {
    unsigned char unmodeled_000[0x7A4];
    JntMatrixBuffer interp_matrix; /* 0x7A4, JNT_setInterpMatrix */
    void *static_value_records;    /* 0x7A8, JNT_setCurve arg0; read by JNT_getStaticVal */
    void *value_records;           /* 0x7AC, JNT_setCurve arg1; read by JNT_getVal */
    unsigned char unmodeled_7b0[0x50];
    float interpolation; /* 0x800, JNT_setInterpMatrix */
} JntWork;

/*
 * A joint element is a fixed-size record: JNT_getElement indexes an array of
 * them and JNT_nextElement advances by exactly one. The layout beyond that
 * 0x40-byte stride is not recovered, so it stays a single unmodeled span
 * (docs/naming.md) rather than invented members.
 */
typedef struct JntElement {
    unsigned char unmodeled_00[0x40];
} JntElement;

/*
 * The joint resource header JNT_getRootElement reads: 0x10 bytes not
 * recovered here, then a self-relative offset field whose own address plus
 * its value is the joint's root element.
 */
typedef struct JntElementHeader {
    unsigned char unmodeled_00[0x10];
    int root_element_offset;
} JntElementHeader;

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

void JNT_setInterpMatrix(JntMatrixBuffer matrix_buffer, float interpolation)
{
    JntWork *work = (JntWork *)JNT_SCRATCH_BASE;
    work->interp_matrix = matrix_buffer;
    work->interpolation = interpolation;
}

void JNT_setCurve(void *static_value_records, void *value_records)
{
    JntWork *work = (JntWork *)JNT_SCRATCH_BASE;
    work->static_value_records = static_value_records;
    work->value_records = value_records;
}

INCLUDE_ASM("asm/main/nonmatchings/jnt", JNT_setMDLMatrix);

INCLUDE_ASM("asm/main/nonmatchings/jnt", JNT_setModelMatrix);

INCLUDE_ASM("asm/main/nonmatchings/jnt", JNT_getMoveElement);

void *JNT_getRootElement(void *joint)
{
    int *root_element_offset = &((JntElementHeader *)joint)->root_element_offset;
    return (unsigned char *)root_element_offset + *root_element_offset;
}

INCLUDE_ASM("asm/main/nonmatchings/jnt", JNT_setModel);

INCLUDE_ASM("asm/main/nonmatchings/jnt", JNT_setFCurve);

INCLUDE_ASM("asm/main/nonmatchings/jnt", JNT_setFCurve2);

INCLUDE_ASM("asm/main/nonmatchings/jnt", JNT_getRootTrans);

INCLUDE_ASM("asm/main/nonmatchings/jnt", JNT_getRootRotate);

INCLUDE_ASM("asm/main/nonmatchings/jnt", JNT_getRootScale);

INCLUDE_ASM("asm/main/nonmatchings/jnt", JNT_addConsumer);

INCLUDE_ASM("asm/main/nonmatchings/jnt", JNT_initProducer);

INCLUDE_ASM("asm/main/nonmatchings/jnt", JNT_startProduction);

void *JNT_getElement(void *elements, int index)
{
    return &((JntElement *)elements)[index];
}

void *JNT_nextElement(void *element)
{
    return (JntElement *)element + 1;
}

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
