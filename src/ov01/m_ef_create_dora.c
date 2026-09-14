/*
 * OV01 original TU 35: 0x00a397c8..0x00a3a090 (7 functions)
 */
#include "common.h"
#include "shared.h"
#include "main/m_math.h"

INCLUDE_ASM("asm/nonmatchings/ov01/m_ef_create_dora", MEfCreate_DORA);

INCLUDE_ASM("asm/nonmatchings/ov01/m_ef_create_dora", makePath_00A398B8);

INCLUDE_ASM("asm/nonmatchings/ov01/m_ef_create_dora", makeHermiteParams_00A39A68);

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

INCLUDE_ASM("asm/nonmatchings/ov01/m_ef_create_dora", fnDORA_PO000);
