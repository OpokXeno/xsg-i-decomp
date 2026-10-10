/*
 * OV01 original TU 23: 0x00a33f08..0x00a346c8 (5 functions)
 */
#include "common.h"
#include "ov01/m_ef_create.h"
#include "shared.h"

extern Matrix4 *MMathRotateMatrixYXZ(Matrix4 *destination,
                                     const Matrix4 *source,
                                     const Vector4 *angles);
extern Matrix4 *MMathScaleMatrix(Matrix4 *destination,
                                 Matrix4 *matrix,
                                 const Vector4 *scale);
extern void MEfDrawModel(const Vector4 *place, int entry, const char *texture);

INCLUDE_ASM("asm/nonmatchings/ov01/m_ef_create_msp_00", MEfCreate_MSP00);

INCLUDE_ASM("asm/nonmatchings/ov01/m_ef_create_msp_00", fnMSP00_PR000);

typedef struct MSP00DrawState {
    unsigned char unmodeled_00[0x40];
    int entry;                 /* +0x40 */
    const char *texture;       /* +0x44 */
    unsigned char unmodeled_48[0x28];
    int frame;                 /* +0x70 */
    unsigned char unmodeled_74[0x2c];
    Vector4 position;          /* +0xa0 */
    unsigned char unmodeled_b0[0x20];
    Vector4 angles;             /* +0xd0 */
} MSP00DrawState;

static void fnMSP00_DM000(void *self, MSP00DrawState *work)
{
    static const Vector4 scale = { 0.04f, 0.04f, 0.2f, 1.0f };
    Matrix4 matrix;

    if (work->frame < 18) {
        MMathRotateMatrixYXZ(&matrix, 0, &work->angles);
        MMathScaleMatrix(&matrix, &matrix, &scale);
        __asm__ __volatile__(
            "lqc2 vf1, 0(%1)\n\t"
            "vmove.w vf1, vf0\n\t"
            "sqc2 vf1, 48(%0)\n\t"
            :
            : "r"(&matrix), "r"(&work->position)
            : "memory"
        );
        MEfDrawModel((const Vector4 *)&matrix, work->entry, work->texture);
    }
}

INCLUDE_ASM("asm/nonmatchings/ov01/m_ef_create_msp_00", fnMSP00_DP000);

/* MEfObjDestroy is defined by src/main/m_ef_obj.c; sefHitEffect
 * is defined by src/main/sef.c. */
extern void sefHitEffect(void);

/* work's layout is unresolved beyond +0x70: the per-frame lifetime counter
 * this function advances, firing sefHitEffect at frame 0x12 and expiring
 * through MEfObjDestroy once it reaches 0x1C. */
#define MSP00_HIT_FRAME 0x12
#define MSP00_LIFETIME  0x1C

typedef struct MSP00State {
    unsigned char unmodeled_00[0x70];
    int frame; /* +0x70 */
} MSP00State;

static void fnMSP00_PO000(void *self, void *work)
{
    MSP00State *state = (MSP00State *)work;

    state->frame++;
    if (state->frame == MSP00_HIT_FRAME) {
        sefHitEffect();
    }
    if (state->frame >= MSP00_LIFETIME) {
        MEfObjDestroy(self);
    }
}
