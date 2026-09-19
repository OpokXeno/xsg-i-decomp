/*
 * OV10 original TU 0: 0x00a00000..0x00a00410 (4 functions)
 */
#include "common.h"

INCLUDE_ASM("asm/nonmatchings/ov10/calc_vert_out_vec_edge", CalcVertOutVecEdge);

INCLUDE_ASM("asm/nonmatchings/ov10/calc_vert_out_vec_edge", yNewRenderInit);

INCLUDE_ASM("asm/nonmatchings/ov10/calc_vert_out_vec_edge", CardGameRoot);

void CardGameRoot(void);

void YmineTest(void) {
    CardGameRoot();
}
