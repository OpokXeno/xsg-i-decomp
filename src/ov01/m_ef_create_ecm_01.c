/*
 * OV01 original TU 30: 0x00a37ab8..0x00a38190 (4 functions)
 */
#include "common.h"

INCLUDE_ASM("asm/nonmatchings/ov01/m_ef_create_ecm_01", MEfCreate_ECM01);

INCLUDE_ASM("asm/nonmatchings/ov01/m_ef_create_ecm_01", fnECM01_PR000);

ACCEPTED_ASM("src/ov01/m_ef_create_ecm_01", fnECM01_DP000);

/* MEfObjDestroy (src/main/m_ef_obj.c) and sefHitEffect (src/main/sef.c) are
 * still asm in their defining TU; declared locally until published there. */
extern void MEfObjDestroy(void *self);
extern void sefHitEffect(void);

/* work's layout is unresolved beyond +0x70: the per-frame lifetime counter
 * this function advances, firing sefHitEffect at frame 0x3C and expiring
 * through MEfObjDestroy once it reaches 0x5A. */
typedef struct ECM01State {
    unsigned char unmodeled_00[0x70];
    int frame; /* +0x70 */
} ECM01State;

static void fnECM01_PO000(void *self, void *work)
{
    ECM01State *state = (ECM01State *)work;

    state->frame++;
    if (state->frame == 0x3C) {
        sefHitEffect();
    }
    if (state->frame >= 0x5A) {
        MEfObjDestroy(self);
    }
}
