/*
 * OV01 original TU 27: 0x00a358a8..0x00a360e0 (5 functions)
 */
#include "common.h"

INCLUDE_ASM("asm/nonmatchings/ov01/m_ef_create_amp_02", MEfCreate_AMP02);

INCLUDE_ASM("asm/nonmatchings/ov01/m_ef_create_amp_02", fnAMP02_PR000);

INCLUDE_ASM("asm/nonmatchings/ov01/m_ef_create_amp_02", fnAMP02_DM000);

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
