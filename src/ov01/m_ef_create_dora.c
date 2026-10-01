/*
 * OV01 original TU 35: 0x00a397c8..0x00a3a090 (7 functions)
 */
#include "common.h"
#include "shared.h"
#include "main/m_math.h"

INCLUDE_ASM("asm/nonmatchings/ov01/m_ef_create_dora", MEfCreate_DORA);

INCLUDE_ASM("asm/nonmatchings/ov01/m_ef_create_dora", makePath_00A398B8);

typedef unsigned int DoraQuadword __attribute__((mode(TI)));

/* The Hermite control-point storage shares the post-process state object.
 * The earlier quadword arrays preserve its observed sixteen-byte alignment. */
typedef struct DoraHermiteState {
    u32 unmodeled_00[4];
    unsigned char unmodeled_10[0x14];
    u32 actor_id;
    u32 actor_coord_id;
    unsigned char unmodeled_2c[0x44];
    short frame;
    unsigned char unmodeled_72[0x0e];
    DoraQuadword weapon_coord;
    HermiteVector actor_coord;
    short trail_active[34];
    short trail_segment[34];
    unsigned char unmodeled_128[8];
    DoraQuadword trail[34];
    short segment;
    short segment_frame;
    unsigned char unmodeled_354[0x0c];
    HermiteVector control_points[4];
    float path_progress[4];
    HermiteVector tangent_start;
    HermiteVector tangent_end;
    unsigned char mg_packet;
} DoraHermiteState;

extern void MMathCalcHermitePrm(HermiteVector *tangent_start,
                                HermiteVector *tangent_end,
                                const HermiteVector *point_previous,
                                const HermiteVector *point_start,
                                const HermiteVector *point_end,
                                const HermiteVector *point_next);

static void makeHermiteParams(int segment, DoraHermiteState *state)
{
    HermiteVector *point_previous;
    HermiteVector *point_start;
    HermiteVector *point_end;
    HermiteVector *point_next;

    if (segment == 0) {
        point_previous = &state->control_points[0];
    } else {
        point_previous = &state->control_points[segment - 1];
    }
    point_start = &state->control_points[segment];
    point_end = &state->control_points[segment + 1];
    if (segment < 2) {
        point_next = &state->control_points[segment + 2];
    } else {
        point_next = point_end;
    }

    MMathCalcHermitePrm(&state->tangent_start, &state->tangent_end,
                        point_previous, point_start, point_end, point_next);
}

static void makeHermiteCoord(float *destination, void *effect)
{
    unsigned char *base = (unsigned char *)effect;
    short segment = *(short *)(base + 848);
    short frame = *(short *)(base + 850);

    MMathCalcHermite(destination, (float)frame * 0.200000003f,
                     (HermiteVector *)(base + 944),
                     (HermiteVector *)(base + 960),
                     (HermiteVector *)(base + 864 + (segment << 4)),
                     (HermiteVector *)(base + 880 + (segment << 4)));
}

INCLUDE_ASM("asm/nonmatchings/ov01/m_ef_create_dora", fnDORA_PR000);

INCLUDE_ASM("asm/nonmatchings/ov01/m_ef_create_dora", fnDORA_DP000);

/* MEfObjDestroy (src/main/m_ef_obj.c) and sefHitEffect (src/main/sef.c) are
 * still asm in their defining TU; declared locally until published there. */
extern void MEfObjDestroy(void *self);
extern void sefHitEffect(void);

/* work's layout is unresolved beyond +0x70: the per-frame lifetime counter
 * this function advances, firing sefHitEffect on frame 0xf and expiring
 * through MEfObjDestroy from frame 0x30 on. */
#define DORA_HIT_FRAME 0xf
#define DORA_LIFETIME 0x30

typedef struct DoraState {
    unsigned char unmodeled_00[0x70];
    short frame; /* +0x70 */
} DoraState;

static void fnDORA_PO000(void *self, void *work)
{
    DoraState *state = (DoraState *)work;

    state->frame++;
    if (state->frame == DORA_HIT_FRAME) {
        sefHitEffect();
    }
    if (state->frame >= DORA_LIFETIME) {
        MEfObjDestroy(self);
    }
}
