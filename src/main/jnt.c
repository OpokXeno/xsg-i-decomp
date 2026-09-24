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
    unsigned char unmodeled_000[0x2C];
    /*
     * JNT_getStaticVal2/JNT_getVal2 read this as the default matrix slot for
     * their CUR_MATRIX_Get call and unconditionally increment it once per
     * call (`lw/sw ...,44($.)`).
     */
    int current_index; /* 0x2C */
    unsigned char unmodeled_030[0x770];
    /*
     * JNT_getStaticVal2/JNT_getVal2 read this as the matrix table their
     * CUR_MATRIX_Set/Get calls index by joint slot (`lw ...,1952($.)`);
     * JNT_setMatrix/JNT_setMatrix2 write it through JNT_MATRIX_BUFFER_OFFSET
     * above (same 0x7A0 slot, kept as a raw offset there for the codegen
     * reason given on that macro).
     */
    JntMatrix *matrix_buffer; /* 0x7A0 */
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

/*
 * Another partial view of the same scratchpad work area as JntWork (it is
 * one layout; JntWork does not model these slots): the general flags word
 * at 0x38, which JNT_setFlags writes and JNT_getFlags reads (`sw $4,56($1)`
 * / `lw $2,56($2)` off the 0x70000000 base). Callers ACT_resetMatrix,
 * ACT_updateMotionSub, ACT_modelDrawSub, ACT_setMotion and ACT_setMotion2
 * read/write it around the same joint update the matrix/curve slots above
 * serve.
 */
typedef struct JntFlagsWork {
    unsigned char unmodeled_000[0x38];
    int flags;
} JntFlagsWork;

/*
 * A view of the same work area for two adjacent slots past the end of
 * JntWork:
 * JNT_animSetFlags writes the animation flags word at 0x810
 * (`sw $4,2064($1)`) and JNT_setClipR writes the clipping radius right
 * after it at 0x814 (`swc1 $f12,2068($1)`); both are supplied by
 * ACT_updateMotionSub (JNT_animSetFlags also by ACT_modelDrawSub).
 */
typedef struct JntAnimWork {
    unsigned char unmodeled_000[0x810];
    int anim_flags;
    float clip_radius;
} JntAnimWork;

INCLUDE_ASM("asm/main/nonmatchings/jnt", get_fcvpack);

INCLUDE_ASM("asm/main/nonmatchings/jnt", get_fcvpack_fix);

INCLUDE_ASM("asm/main/nonmatchings/jnt", get_fcvpack_flag);

INCLUDE_ASM("asm/main/nonmatchings/jnt", JNT_getStaticVal);

INCLUDE_ASM("asm/main/nonmatchings/jnt", FCV_getAttrAndValue);

INCLUDE_ASM("asm/main/nonmatchings/jnt", JNT_getVal);

INCLUDE_ASM("asm/main/nonmatchings/jnt", JNT_getVal_staticChain);

INCLUDE_ASM("asm/main/nonmatchings/jnt", JNT_constructMatrix);

/*
 * CUR_MATRIX_* (defined in another TU, not yet recovered there) form a
 * current-matrix-register API: Set/Get load and store a JntMatrix through
 * it, Txyz4s/Rzyx4s apply a translation/rotation from a float triple onto it.
 */
void CUR_MATRIX_Set(JntMatrix *matrix);
void CUR_MATRIX_Get(JntMatrix *matrix);
void CUR_MATRIX_Txyz4s(float *translate);
void CUR_MATRIX_Rzyx4s(float *rotate);

/*
 * A joint static-value channel: JNT_getStaticVal2 reads its type (dispatch,
 * only types 1-5 apply a transform), its target joint slot and the
 * translate/rotate triples it feeds to CUR_MATRIX_Txyz4s/Rzyx4s, then
 * returns the following channel (`channel + 1`), which fixes its size at
 * 0x40 bytes, the same stride as JntElement.
 */
typedef struct JntStaticChannel {
    unsigned short type;           /* 0x00 */
    unsigned char unmodeled_02[0x0C];
    unsigned short matrix_index;   /* 0x0E */
    float translate[3];            /* 0x10 */
    unsigned char unmodeled_1c[4];
    float rotate[3];               /* 0x20 */
    unsigned char unmodeled_2C[0x14];
} JntStaticChannel;

/*
 * The type-1..4 body below (label apply_index) and the type==5 body above it
 * are byte-identical in the original (both do
 * CUR_MATRIX_Set/Txyz4s/Rzyx4s/Get on the same arguments), but the original
 * still emits them as two separate instruction sequences: the type==5 copy
 * falls straight through into an unconditional jump to the shared tail,
 * while the type 1..4 copy is a separate out-of-line block that falls
 * straight through into that same tail. A structured if/else-if or two
 * independent ifs both let 2.96 fold the two identical call sequences into
 * one shared block reached from two jumps, which the original does not do;
 * the goto below reproduces the original's two independently-placed copies.
 */
void *JNT_getStaticVal2(JntWork *work, JntStaticChannel *channel)
{
    int type;
    float *translate;
    float *rotate;
    void *next;

    type = channel->type;
    translate = channel->translate;
    rotate = channel->rotate;
    next = channel + 1;

    if (type < 5) {
        if (type == 0) {
            goto done;
        }
        goto apply_index;
    }
    if (type == 5) {
        CUR_MATRIX_Set(&work->matrix_buffer[channel->matrix_index]);
        CUR_MATRIX_Txyz4s(translate);
        CUR_MATRIX_Rzyx4s(rotate);
        CUR_MATRIX_Get(&work->matrix_buffer[work->current_index]);
    }
    goto done;

apply_index:
    CUR_MATRIX_Set(&work->matrix_buffer[channel->matrix_index]);
    CUR_MATRIX_Txyz4s(translate);
    CUR_MATRIX_Rzyx4s(rotate);
    CUR_MATRIX_Get(&work->matrix_buffer[work->current_index]);

done:
    work->current_index++;
    return next;
}

INCLUDE_ASM("asm/main/nonmatchings/jnt", JNT_getVal2);

INCLUDE_ASM("asm/main/nonmatchings/jnt", JNT_getVal_HOGE2);

INCLUDE_ASM("asm/main/nonmatchings/jnt", liner_interpolate);

INCLUDE_ASM("asm/main/nonmatchings/jnt", JNT_interpolate);

void JNT_setFlags(int flags)
{
    JntFlagsWork *work = (JntFlagsWork *)JNT_SCRATCH_BASE;
    work->flags = flags;
}

void JNT_setClipR(float clip_radius)
{
    JntAnimWork *work = (JntAnimWork *)JNT_SCRATCH_BASE;
    work->clip_radius = clip_radius;
}

void JNT_animSetFlags(int anim_flags)
{
    JntAnimWork *work = (JntAnimWork *)JNT_SCRATCH_BASE;
    work->anim_flags = anim_flags;
}

int JNT_getFlags(void)
{
    JntFlagsWork *work = (JntFlagsWork *)JNT_SCRATCH_BASE;
    return work->flags;
}

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

/*
 * A joint producer: JNT_initProducer's own evidence is a consumer count, an
 * 8-entry array at 0x24 it clears, and three trailing words it resets to an
 * idle state (0x64/0x68 to zero, 0x6C to -1).
 */
struct JntProducer {
    int count;              /* 0x00 */
    unsigned char unmodeled_04[0x20];
    int consumers[8];       /* 0x24, only evidenced as cleared by JNT_initProducer */
    unsigned char unmodeled_44[0x20];
    int pending_output;     /* 0x64 */
    int output_count;       /* 0x68, only evidenced as cleared by JNT_initProducer */
    int current_index;      /* 0x6C, -1 when none */
};

void JNT_initProducer(JntProducer *producer)
{
    int i;

    producer->pending_output = 0;
    producer->count = 0;
    producer->output_count = 0;
    producer->current_index = -1;
    for (i = 7; i >= 0; i--) {
        producer->consumers[i] = 0;
    }
}

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

/*
 * Hair-smoothing toggle at VA 0x004DC248: JNT_computeMatrix reads it with
 * `lb $2,flagSmoothHair` (VA 0x00314B14) to decide whether to apply the
 * interpolated hair matrix; this pair only sets/clears it.
 */
extern signed char flagSmoothHair;

void JNT_onSmoothHair(void)
{
    flagSmoothHair = 1;
}

void JNT_offSmoothHair(void)
{
    flagSmoothHair = 0;
}
