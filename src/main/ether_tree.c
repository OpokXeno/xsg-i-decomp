#include "common.h"

INCLUDE_ASM("asm/main/nonmatchings/ether_tree", EtherTreeObjectGetClear);

INCLUDE_ASM("asm/main/nonmatchings/ether_tree", EtherTreeObjectWorkGet);

INCLUDE_ASM("asm/main/nonmatchings/ether_tree", EtherTreeFirstDataGet);

INCLUDE_ASM("asm/main/nonmatchings/ether_tree", EtherTreeObjectGet);

INCLUDE_ASM("asm/main/nonmatchings/ether_tree", EtherTreeLineColorGet);

INCLUDE_ASM("asm/main/nonmatchings/ether_tree", sub2ParentChildSet);

INCLUDE_ASM("asm/main/nonmatchings/ether_tree", subParentChildSet);

INCLUDE_ASM("asm/main/nonmatchings/ether_tree", subPosSet);

INCLUDE_ASM("asm/main/nonmatchings/ether_tree", subPosSet2);

INCLUDE_ASM("asm/main/nonmatchings/ether_tree", subPosSet3);

INCLUDE_ASM("asm/main/nonmatchings/ether_tree", sub2JoutoYGet);

INCLUDE_ASM("asm/main/nonmatchings/ether_tree", subJoutoPosSet);

INCLUDE_ASM("asm/main/nonmatchings/ether_tree", EtherTreeParaSet);

INCLUDE_ASM("asm/main/nonmatchings/ether_tree", EtherTreeLineSet);

INCLUDE_ASM("asm/main/nonmatchings/ether_tree", EtherTreeTargetChange);

INCLUDE_ASM("asm/main/nonmatchings/ether_tree", EtherTreeWorkClear);

INCLUDE_ASM("asm/main/nonmatchings/ether_tree", EtherTreeObjectSet);

INCLUDE_ASM("asm/main/nonmatchings/ether_tree", EtherTreeCenterSet);

INCLUDE_ASM("asm/main/nonmatchings/ether_tree", EtherTreeToCenterSet);

/* Move a floating-point position toward its target without crossing it. */
void subMoveSlide(float *position, float *target, float rate)
{
    float difference = *position - *target;

    if (difference == 0.0f)
        return;

    if (*target < *position) {
        *position = *position - (difference * rate + 1.0f);
        if (*position < *target)
            *position = *target;
    } else {
        *position = *position - (difference * rate - 1.0f);
        if (*target < *position)
            *position = *target;
    }
}

INCLUDE_ASM("asm/main/nonmatchings/ether_tree", EtherTreeCenterMove);

INCLUDE_ASM("asm/main/nonmatchings/ether_tree", EtherTreeRightSet);

INCLUDE_ASM("asm/main/nonmatchings/ether_tree", EtherTreeRightModeChange);

INCLUDE_ASM("asm/main/nonmatchings/ether_tree", EtherTreeRightTargetChange);

INCLUDE_ASM("asm/main/nonmatchings/ether_tree", subEtherTreeRightMain);

INCLUDE_ASM("asm/main/nonmatchings/ether_tree", EtherTreeRightMain);

INCLUDE_ASM("asm/main/nonmatchings/ether_tree", subRightDraw);

INCLUDE_ASM("asm/main/nonmatchings/ether_tree", EtherTreeRightDraw);

INCLUDE_ASM("asm/main/nonmatchings/ether_tree", EtherTreeBlackSet);

INCLUDE_ASM("asm/main/nonmatchings/ether_tree", EtherTreeBlackModeChange);

INCLUDE_ASM("asm/main/nonmatchings/ether_tree", EtherTreeBlackDraw);

INCLUDE_ASM("asm/main/nonmatchings/ether_tree", EtherTreeBlackMain);

INCLUDE_ASM("asm/main/nonmatchings/ether_tree", EtherTreeLine2Set);

INCLUDE_ASM("asm/main/nonmatchings/ether_tree", EtherTreeLine2ModeChange);

INCLUDE_ASM("asm/main/nonmatchings/ether_tree", EtherTreeLine2SelectChange);

INCLUDE_ASM("asm/main/nonmatchings/ether_tree", subLine2_DrawType_1);

INCLUDE_ASM("asm/main/nonmatchings/ether_tree", EtherTreeLine2Draw);

INCLUDE_ASM("asm/main/nonmatchings/ether_tree", subLine2_OpenType_0);

INCLUDE_ASM("asm/main/nonmatchings/ether_tree", subLine2_OpenType_1);

INCLUDE_ASM("asm/main/nonmatchings/ether_tree", EtherTreeLine2Main);

INCLUDE_ASM("asm/main/nonmatchings/ether_tree", sub2ObjectLampDraw);

INCLUDE_ASM("asm/main/nonmatchings/ether_tree", EtherTreeObjectDraw);

INCLUDE_ASM("asm/main/nonmatchings/ether_tree", subTreeLineDraw);

INCLUDE_ASM("asm/main/nonmatchings/ether_tree", subTreeLineDraw_type_0);

INCLUDE_ASM("asm/main/nonmatchings/ether_tree", subTreeLineDraw_type_1);

INCLUDE_ASM("asm/main/nonmatchings/ether_tree", subTreeLineDraw_type_2);

INCLUDE_ASM("asm/main/nonmatchings/ether_tree", subTreeLineDraw_type_3);

INCLUDE_ASM("asm/main/nonmatchings/ether_tree", EtherTreeLineDraw);

INCLUDE_ASM("asm/main/nonmatchings/ether_tree", EtherTreeCursolDraw);

INCLUDE_ASM("asm/main/nonmatchings/ether_tree", EtherTreeDraw);

INCLUDE_ASM("asm/main/nonmatchings/ether_tree", EtherTreeInit);

INCLUDE_ASM("asm/main/nonmatchings/ether_tree", RoboEtherCheck);

INCLUDE_ASM("asm/main/nonmatchings/ether_tree", EtherTreeMain);
