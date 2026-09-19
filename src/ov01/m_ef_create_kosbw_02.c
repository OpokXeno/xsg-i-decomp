/*
 * OV01 original TU 36: 0x00a3a090..0x00a3aac8 (5 functions)
 */
#include "common.h"

INCLUDE_ASM("asm/nonmatchings/ov01/m_ef_create_kosbw_02", MEfCreate_KOSBW02);

INCLUDE_ASM("asm/nonmatchings/ov01/m_ef_create_kosbw_02", fnKOSBW02_PR000);

INCLUDE_ASM("asm/nonmatchings/ov01/m_ef_create_kosbw_02", fnKOSBW02_DM000);

INCLUDE_ASM("asm/nonmatchings/ov01/m_ef_create_kosbw_02", fnKOSBW02_DP000);

extern void MEfObjDestroy(void *self);
extern void sefHitEffect(void);

/*
 * The KOSBW02 effect record: the work area MEfCreate_KOSBW02 (outside this
 * allocation) installs fnKOSBW02_PO000 into. `frame` is the same counter role
 * the sibling MSP02 effect keeps at this same offset (MspEffect,
 * src/ov01/m_ef_create_msp_02.c): incremented once per call, it fires
 * sefHitEffect at 25 and MEfObjDestroy at 35.
 */
typedef struct Kosbw02Effect {
    unsigned char unmodeled_00[0x70];
    int frame;
} Kosbw02Effect;

static void fnKOSBW02_PO000(void *self, Kosbw02Effect *work)
{
    work->frame++;
    if (work->frame == 25) {
        sefHitEffect();
    }
    if (work->frame >= 35) {
        MEfObjDestroy(self);
    }
}
