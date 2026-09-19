/*
 * OV01 original TU 25: 0x00a34e90..0x00a358a0 (7 functions)
 */
#include "common.h"
#include "shared.h"
#include "m_ef_create_smp_01.h"

INCLUDE_ASM("asm/nonmatchings/ov01/m_ef_create_smp_01", MEfCreate_SMP01);

INCLUDE_ASM("asm/nonmatchings/ov01/m_ef_create_smp_01", updateSmoke);

INCLUDE_ASM("asm/nonmatchings/ov01/m_ef_create_smp_01", fnSMP01_PR000);

INCLUDE_ASM("asm/nonmatchings/ov01/m_ef_create_smp_01", fnSMP01_PR010);

INCLUDE_ASM("asm/nonmatchings/ov01/m_ef_create_smp_01", fnSMP01_DM000);

INCLUDE_ASM("asm/nonmatchings/ov01/m_ef_create_smp_01", fnSMP01_DP000);

extern void MEfObjDestroy(void *object);
extern void sefHitEffect(void);

static void fnSMP01_PO000(void *object, void *work)
{
    Smp01State *state = (Smp01State *)work;

    state->frame++;
    if (state->frame == 15) {
        sefHitEffect();
    }
    if (state->frame >= 25) {
        MEfObjDestroy(object);
    }
}
