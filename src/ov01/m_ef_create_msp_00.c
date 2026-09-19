/*
 * OV01 original TU 23: 0x00a33f08..0x00a346c8 (5 functions)
 */
#include "common.h"

INCLUDE_ASM("asm/nonmatchings/ov01/m_ef_create_msp_00", MEfCreate_MSP00);

INCLUDE_ASM("asm/nonmatchings/ov01/m_ef_create_msp_00", fnMSP00_PR000);

INCLUDE_ASM("asm/nonmatchings/ov01/m_ef_create_msp_00", fnMSP00_DM000);

INCLUDE_ASM("asm/nonmatchings/ov01/m_ef_create_msp_00", fnMSP00_DP000);

/* MEfObjDestroy (src/main/m_ef_obj.c) and sefHitEffect (src/main/sef.c) are
 * still asm in their defining TU; declared locally until published there. */
extern void MEfObjDestroy(void *self);
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
