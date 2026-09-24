#include "common.h"
#include "shared.h"

INCLUDE_ASM("asm/main/nonmatchings/act_3", Footstep);

INCLUDE_ASM("asm/main/nonmatchings/act_3", DrawCircleShadow);

INCLUDE_ASM("asm/main/nonmatchings/act_3", DrawZeldaShadowSub);

INCLUDE_ASM("asm/main/nonmatchings/act_3", DrawZeldaShadow);

INCLUDE_ASM("asm/main/nonmatchings/act_3", DrawDropShadowSubChk);

INCLUDE_ASM("asm/main/nonmatchings/act_3", DrawDropShadowSubBack);

INCLUDE_ASM("asm/main/nonmatchings/act_3", DrawDropShadowSubFront);

INCLUDE_ASM("asm/main/nonmatchings/act_3", DrawDropShadowSub);

INCLUDE_ASM("asm/main/nonmatchings/act_3", DrawDropShadow);

INCLUDE_ASM("asm/main/nonmatchings/act_3", DrawDropCircle);

INCLUDE_ASM("asm/main/nonmatchings/act_3", ACT_DrawShadowBegin);

extern void nmlModelDirectSend(int mode, u8 *data, int count);
extern u8 Tail_10[];

void ACT_DrawShadowEnd(void)
{
    nmlModelDirectSend(1, Tail_10, 3);
}

INCLUDE_ASM("asm/main/nonmatchings/act_3", ACT_DrawShadow);

void ACT_DrawShadowInit(void)
{
}
