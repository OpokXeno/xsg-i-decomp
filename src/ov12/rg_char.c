/*
 * OV12 original TU 2: 0x00a01ff8..0x00a024f8 (15 functions)
 */
#include "common.h"
#include "shared.h"
#include "rg_char.h"

INCLUDE_ASM("asm/nonmatchings/ov12/rg_char", _nonControlMethod);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_char", _nonDispMethod);

void RgCalcLocalForChar(RgMatrix dest_matrix, RgVector position,
                        RgVector facing, RgVector up_ref)
{
    RgVector neg_facing;

    XrgCopyVector(neg_facing, facing);
    XrgNegateVector(neg_facing, neg_facing);
    XrgCalcMatrixYtoZ(dest_matrix, neg_facing, up_ref);
    XrgCopyVectorXYZ(&dest_matrix[12], position);
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_char", RgCharAlloc);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_char", RgCharFree);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_char", InitRgChar);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_char", RgCharGetType);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_char", RgCharSetType);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_char", RgCharControlMethod);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_char", RgCharDispMethod);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_char", RgCharDestructMethod);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_char", RgCharPassTimeMethod);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_char", RgCharControl);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_char", RgCharPassTime);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_char", RgCharDisp);
