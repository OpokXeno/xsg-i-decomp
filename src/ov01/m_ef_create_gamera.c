/*
 * OV01 original TU 32: 0x00a387a8..0x00a39088 (7 functions)
 */
#include "common.h"
#include "ov01/m_ef_create.h"
#include "shared.h"
#include "main/m_math.h"

extern void MMathCalcHermitePrm(HermiteVector *tangent_start, HermiteVector *tangent_end,
                                const HermiteVector *point_prev,
                                const HermiteVector *point_start,
                                const HermiteVector *point_end,
                                const HermiteVector *point_next);

typedef struct GameraWork {
    unsigned char unmodeled_00[0x2d0];
    short segment; /* +0x2d0 */
    short path_frame; /* +0x2d2 */
    unsigned char unmodeled_2d4[12];
    HermiteVector control_points[5]; /* +0x2e0 */
    HermiteVector tangent_start; /* +0x330 */
    HermiteVector tangent_end; /* +0x340 */
} GameraWork;

INCLUDE_ASM("asm/nonmatchings/ov01/m_ef_create_gamera", MEfCreate_GAMERA);

INCLUDE_ASM("asm/nonmatchings/ov01/m_ef_create_gamera", makePath_00A388A0);

static void makeHermiteParams(int segment, GameraWork *state)
{
    const HermiteVector *point_prev;
    const HermiteVector *point_end;
    const HermiteVector *point_next;

    if (segment == 0) {
        point_prev = &state->control_points[0];
    } else {
        point_prev = &state->control_points[segment - 1];
    }

    point_end = &state->control_points[segment + 1];
    point_next = &state->control_points[segment + 2];
    if (segment >= 2) {
        point_next = point_end;
    }

    MMathCalcHermitePrm(&state->tangent_start, &state->tangent_end,
                        point_prev, &state->control_points[segment],
                        point_end, point_next);
}

static void makeHermiteCoord(float *destination, void *effect)
{
    unsigned char *base = (unsigned char *)effect;
    short segment = *(short *)(base + 720);
    short frame = *(short *)(base + 722);

    MMathCalcHermite(destination, (float)frame * 0.125f,
                     (HermiteVector *)(base + 816),
                     (HermiteVector *)(base + 832),
                     (HermiteVector *)(base + 736 + (segment << 4)),
                     (HermiteVector *)(base + 752 + (segment << 4)));
}

INCLUDE_ASM("asm/nonmatchings/ov01/m_ef_create_gamera", fnGAMERA_PR000);

INCLUDE_ASM("asm/nonmatchings/ov01/m_ef_create_gamera", fnGAMERA_DP000);

/* MEfObjDestroy (src/main/m_ef_obj.c) and sefHitEffect (src/main/sef.c) are
 * still asm in their defining TU; declared locally until published there. */
extern void sefHitEffect(void);

/* work's layout is unresolved beyond +0x70: the per-frame lifetime counter
 * this function advances, firing sefHitEffect on frame 0x18 and expiring
 * through MEfObjDestroy from frame 0x33 on. */
#define GAMERA_HIT_FRAME 0x18
#define GAMERA_LIFETIME 0x33

typedef struct GameraState {
    unsigned char unmodeled_00[0x70];
    short frame; /* +0x70 */
} GameraState;

static void fnGAMERA_PO000(void *self, void *work)
{
    GameraState *state = (GameraState *)work;

    state->frame++;
    if (state->frame == GAMERA_HIT_FRAME) {
        sefHitEffect();
    }
    if (state->frame >= GAMERA_LIFETIME) {
        MEfObjDestroy(self);
    }
}
