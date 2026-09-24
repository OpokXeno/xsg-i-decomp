/*
 * OV01 original TU 24: 0x00a346c8..0x00a34e90 (5 functions)
 */
#include "common.h"

INCLUDE_ASM("asm/nonmatchings/ov01/m_ef_create_bp_00", MEfCreate_BP00);

INCLUDE_ASM("asm/nonmatchings/ov01/m_ef_create_bp_00", fnBP00_PR000);

INCLUDE_ASM("asm/nonmatchings/ov01/m_ef_create_bp_00", fnBP00_PR010);

INCLUDE_ASM("asm/nonmatchings/ov01/m_ef_create_bp_00", fnBP00_DP000);

/* MEfObjDestroy (src/main/m_ef_obj.c) and sefHitEffect (src/main/sef.c) are
 * still asm in their defining TU; declared locally until published there. */
extern void MEfObjDestroy(void *self);
extern void sefHitEffect(void);

/* work's layout is unresolved beyond +0x70: the per-frame lifetime counter
 * this function advances, firing sefHitEffect at frame 0x21 and expiring
 * through MEfObjDestroy once it reaches 0x2D. counter, right after it, is
 * only ever incremented here in lockstep with frame; nothing recovered in
 * this TU reads it back, so its specific role (an "age" naming borrowed
 * from the unrelated SolbState.age, src/ov01/m_ef_create_solb.h) is not
 * evidenced here and is left neutral. */
#define BP00_HIT_FRAME 0x21
#define BP00_LIFETIME  0x2D

typedef struct BP00State {
    unsigned char unmodeled_00[0x70];
    int frame;   /* +0x70 */
    int counter; /* +0x74 */
} BP00State;

static void fnBP00_PO000(void *self, void *work)
{
    BP00State *state = (BP00State *)work;

    state->frame++;
    state->counter++;
    if (state->frame == BP00_HIT_FRAME) {
        sefHitEffect();
    }
    if (state->frame >= BP00_LIFETIME) {
        MEfObjDestroy(self);
    }
}
