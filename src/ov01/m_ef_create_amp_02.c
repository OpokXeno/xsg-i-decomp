/*
 * OV01 original TU 27: 0x00a358a8..0x00a360e0 (5 functions)
 */
#include "common.h"
#include "shared.h"

INCLUDE_ASM("asm/nonmatchings/ov01/m_ef_create_amp_02", MEfCreate_AMP02);

INCLUDE_ASM("asm/nonmatchings/ov01/m_ef_create_amp_02", fnAMP02_PR000);

extern void *MEfCalcAngle(Vector4 *destination, const Vector4 *from, const Vector4 *to);
extern void MMathRotateMatrixYX(Vector4 *destination, const Vector4 *source, const Vector4 *angles);
extern void MMathScaleMatrix(Vector4 *out, const Vector4 *matrix, const Vector4 *scale);
extern void MEfDrawModel(const Vector4 *place, int entry, const char *texture);
extern Vector4 scale_0_00A515A0;

/* Same hit-frame threshold fnAMP02_PO000 (below) checks; DM000 stops drawing
   once that frame is reached. */
#define AMP02_DRAW_FRAME 30

/*
 * DM000's own view of the AMP02 work area: the model `entry`/`texture` pair
 * MEfCreate_AMP02 (outside this allocation) installs, the same `frame`
 * counter fnAMP02_PO000 advances, this effect's own `position` and the
 * `target` it turns to face.
 */
typedef struct Amp02DrawState {
    unsigned char unmodeled_00[0x40];
    int entry;             /* +0x40 */
    const char *texture;   /* +0x44 */
    unsigned char unmodeled_48[0x70 - 0x48];
    int frame;              /* +0x70 */
    unsigned char unmodeled_74[0xA0 - 0x74];
    Vector4 position;      /* +0xA0 */
    unsigned char unmodeled_B0[0xC0 - 0xB0];
    Vector4 target;         /* +0xC0 */
} Amp02DrawState;

/*
 * Draws AMP02 while it is still before the hit frame: aims a matrix at
 * `target` from `position`, scales it, forces the translation row's w lane
 * to the architectural VF0.w = 1 while loading `position` into it, then
 * draws the model.
 */
static void fnAMP02_DM000(void *self, Amp02DrawState *work) {
    Vector4 angles;
    Vector4 matrix[4];

    if (work->frame < AMP02_DRAW_FRAME) {
        MEfCalcAngle(&angles, &work->position, &work->target);
        MMathRotateMatrixYX(matrix, (const Vector4 *)0, &angles);
        MMathScaleMatrix(matrix, matrix, &scale_0_00A515A0);
        __asm__ __volatile__("lqc2 vf1, 0(%0)" : : "r"(&work->position) : "memory");
        __asm__ __volatile__("vmove.w vf1, vf0" : : : "memory");
        __asm__ __volatile__("sqc2 vf1, 48(%0)" : : "r"(matrix) : "memory");
        MEfDrawModel(matrix, work->entry, work->texture);
    }
}

INCLUDE_ASM("asm/nonmatchings/ov01/m_ef_create_amp_02", fnAMP02_DP000);

extern void MEfObjDestroy(void *self);
extern void sefHitEffect(void);

/*
 * The AMP02 effect record: the work area MEfCreate_AMP02 (outside this
 * allocation) installs fnAMP02_PO000 into. `frame` is the same counter role
 * the sibling MSP02 effect keeps at this same offset (MspEffect,
 * src/ov01/m_ef_create_msp_02.c): incremented once per call, it fires
 * sefHitEffect at 30 and MEfObjDestroy at 40.
 */
typedef struct Amp02Effect {
    unsigned char unmodeled_00[0x70];
    int frame;
} Amp02Effect;

static void fnAMP02_PO000(void *self, Amp02Effect *work)
{
    work->frame++;
    if (work->frame == 30) {
        sefHitEffect();
    }
    if (work->frame >= 40) {
        MEfObjDestroy(self);
    }
}
