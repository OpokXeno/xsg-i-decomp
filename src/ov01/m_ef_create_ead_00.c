/*
 * OV01 original TU 33: 0x00a39088..0x00a397c0 (4 functions)
 */
#include "common.h"

INCLUDE_ASM("asm/nonmatchings/ov01/m_ef_create_ead_00", MEfCreate_EAD00);

INCLUDE_ASM("asm/nonmatchings/ov01/m_ef_create_ead_00", fnEAD00_PR000);

INCLUDE_ASM("asm/nonmatchings/ov01/m_ef_create_ead_00", fnEAD00_DP000);

/* MEfObjDestroy (src/main/m_ef_obj.c) and sefHitEffect (src/main/sef.c) are
 * still asm in their defining TU; declared locally until published there. */
extern void MEfObjDestroy(void *self);
extern void sefHitEffect(void);

/* work's layout is unresolved beyond +0x70: the per-frame lifetime counter
 * this function advances, firing sefHitEffect and expiring through
 * MEfObjDestroy on the same frame, 0x46. */
#define EAD00_LIFETIME 0x46

typedef struct EAD00State {
    unsigned char unmodeled_00[0x70];
    int frame; /* +0x70 */
} EAD00State;

static void fnEAD00_PO000(void *self, void *work)
{
    EAD00State *state = (EAD00State *)work;

    state->frame++;
    if (state->frame == EAD00_LIFETIME) {
        sefHitEffect();
    }
    if (state->frame >= EAD00_LIFETIME) {
        MEfObjDestroy(self);
    }
}
