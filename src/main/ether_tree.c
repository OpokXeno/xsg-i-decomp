#include "common.h"

/*
 * EtherTreeObject points at a table of 80 tree-object records of 0x70 bytes
 * (EtherTreeObjectSet clears 0x2300 bytes of it; EtherTreeObjectGet matches a
 * 16-bit id at offset 0). EtherTreeObjectP is the fill cursor into that table:
 * EtherTreeObjectWorkGet hands out the current record and advances it by one.
 * No function claimed here reads a record, so the record type stays opaque.
 */
typedef struct EtherTreeObjectData EtherTreeObjectData;

extern EtherTreeObjectData *EtherTreeObject;
extern EtherTreeObjectData *EtherTreeObjectP;

void EtherTreeObjectGetClear(void)
{
    EtherTreeObjectP = EtherTreeObject;
}

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

/*
 * EtherTreeCenterSet, EtherTreeToCenterSet and EtherTreeCenterMove (this
 * unit, main/tu179) all read or write this struct through EtherTreeSystem.
 * flags bit 0 gates EtherTreeCenterSet, EtherTreeCenterMove and
 * EtherTreeDraw; EtherTreeCenterSet sets bit 1 and EtherTreeCenterMove
 * tests it before sliding centerX/centerY toward targetCenterX/
 * targetCenterY with subMoveSlide. Bytes no function claimed here writes or
 * reads stay unmodeled.
 */
typedef struct EtherTreeSystemData {
    unsigned char flags;
    unsigned char unmodeled_01[0x0f];
    float centerX;
    float centerY;
    unsigned char unmodeled_18[0x08];
    float targetCenterX;
    float targetCenterY;
    unsigned char unmodeled_28[0x48];
    int windowTexAddr;
} EtherTreeSystemData;

/*
 * EtherTreeDraw (this unit, main:0x002bc4f0) stores WindowTexAddrGet(2)'s
 * result at +0x70 (sw $2,112($6)) and hands its address to endPrintExtFunc;
 * nothing else in this allocation touches the span between it and
 * targetCenterY, so it stays unmodeled.
 */
extern EtherTreeSystemData *EtherTreeSystem;

void EtherTreeCenterSet(int axisMask, float x, float y)
{
    if ((EtherTreeSystem->flags & 1) &&
        (!(axisMask & 1) || EtherTreeSystem->centerX != EtherTreeSystem->targetCenterX) &&
        (!(axisMask & 2) || EtherTreeSystem->centerY != EtherTreeSystem->targetCenterY)) {
        EtherTreeSystem->centerX = x;
        EtherTreeSystem->centerY = y;
        EtherTreeSystem->flags |= 2;
    }
}

void EtherTreeToCenterSet(float targetX, float targetY)
{
    EtherTreeSystem->targetCenterX = targetX;
    EtherTreeSystem->targetCenterY = targetY;
}

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

/*
 * EtherTreeRight is a 2-entry array of "right panel" object-highlight slot
 * records (indexed by index*0x40 in EtherTreeRightSet, this unit, so each
 * record is 0x40 bytes). flags bit 0 marks the slot active; every function
 * here is itself gated by EtherTreeSystem->flags bit 0. mode drives a
 * 4-state open/close animation (subEtherTreeRightMain): 0 idle, 1 opening
 * (counter counts up by 8 to 48, then mode becomes 2), 2 held open (counter
 * pinned at 48), 3 closing (counter counts down by 8 to 0, then flags bit 0
 * is cleared and mode returns to 0). object is the EtherTreeObjectGet()
 * record the slot currently tracks (EtherTreeRightSet, this unit;
 * EtherTreeRightTargetChange). Bytes no function claimed here writes or
 * reads stay unmodeled.
 */
typedef struct EtherTreeRightData {
    unsigned char flags;
    unsigned char mode;
    unsigned char unmodeled_02[0x1e];
    EtherTreeObjectData *object;
    unsigned char unmodeled_24[0x0c];
    int counter;
    unsigned char unmodeled_34[0x0c];
} EtherTreeRightData;

extern EtherTreeRightData *EtherTreeRight;
extern EtherTreeObjectData *EtherTreeObjectGet(int id);

void EtherTreeRightTargetChange(int id, int index)
{
    EtherTreeRightData *record = &EtherTreeRight[index];

    if (!(EtherTreeSystem->flags & 1))
        return;
    if (record->flags & 1) {
        record->object = EtherTreeObjectGet(id);
    }
}

extern void subEtherTreeRightMain(EtherTreeRightData *record);

INCLUDE_ASM("asm/main/nonmatchings/ether_tree", subEtherTreeRightMain);

void EtherTreeRightMain(void)
{
    int i;

    if (!(EtherTreeSystem->flags & 1))
        return;

    for (i = 0; i < 2; i++) {
        subEtherTreeRightMain(&EtherTreeRight[i]);
    }
}

INCLUDE_ASM("asm/main/nonmatchings/ether_tree", subRightDraw);

INCLUDE_ASM("asm/main/nonmatchings/ether_tree", EtherTreeRightDraw);

/*
 * EtherTreeBlack is the fade-to-black slot: this unit provides the same
 * Set/ModeChange/Main/Draw family for it as for EtherTreeRight. mode drives
 * its animation and counter is the frame timer (EtherTreeBlackModeChange,
 * EtherTreeBlackMain); EtherTreeBlackSet resets both to idle. Bytes no
 * function claimed here writes or reads stay unmodeled.
 */
typedef struct EtherTreeBlackData {
    unsigned char unmodeled_00;
    unsigned char mode;
    short counter;
} EtherTreeBlackData;

extern EtherTreeBlackData *EtherTreeBlack;

void EtherTreeBlackSet(void)
{
    EtherTreeBlack->counter = (EtherTreeBlack->mode = 0);
}

INCLUDE_ASM("asm/main/nonmatchings/ether_tree", EtherTreeBlackModeChange);

INCLUDE_ASM("asm/main/nonmatchings/ether_tree", EtherTreeBlackDraw);

INCLUDE_ASM("asm/main/nonmatchings/ether_tree", EtherTreeBlackMain);

INCLUDE_ASM("asm/main/nonmatchings/ether_tree", EtherTreeLine2Set);

INCLUDE_ASM("asm/main/nonmatchings/ether_tree", EtherTreeLine2ModeChange);

INCLUDE_ASM("asm/main/nonmatchings/ether_tree", EtherTreeLine2SelectChange);

INCLUDE_ASM("asm/main/nonmatchings/ether_tree", subLine2_DrawType_1);

/*
 * EtherTreeLine2 is the second lower-menu line control block, gated like
 * every function in this file by EtherTreeSystem->flags bit 0.
 * selectedIndex is the index EtherTreeLine2SelectChange last latched;
 * changing it replays EtherTreeLine2ModeChange(0) (close) before
 * EtherTreeLine2ModeChange(1) (open) on the new index. Bytes no function
 * claimed here writes or reads stay unmodeled.
 */
typedef struct EtherTreeLine2Data {
    unsigned char flags;
    unsigned char unmodeled_01[0x03];
    int selectedIndex;
} EtherTreeLine2Data;

extern EtherTreeLine2Data *EtherTreeLine2;
extern void subLine2_DrawType_1(void);

void EtherTreeLine2Draw(void)
{
    EtherTreeLine2Data *line2 = EtherTreeLine2;

    if (!(EtherTreeSystem->flags & 1))
        return;
    if (!(line2->flags & 1))
        return;

    subLine2_DrawType_1();
}

INCLUDE_ASM("asm/main/nonmatchings/ether_tree", subLine2_OpenType_0);

INCLUDE_ASM("asm/main/nonmatchings/ether_tree", subLine2_OpenType_1);

INCLUDE_ASM("asm/main/nonmatchings/ether_tree", EtherTreeLine2Main);

INCLUDE_ASM("asm/main/nonmatchings/ether_tree", sub2ObjectLampDraw);

INCLUDE_ASM("asm/main/nonmatchings/ether_tree", EtherTreeObjectDraw);

INCLUDE_ASM("asm/main/nonmatchings/ether_tree", subTreeLineDraw);

void subTreeLineDraw_type_0(void)
{
}

INCLUDE_ASM("asm/main/nonmatchings/ether_tree", subTreeLineDraw_type_1);

INCLUDE_ASM("asm/main/nonmatchings/ether_tree", subTreeLineDraw_type_2);

INCLUDE_ASM("asm/main/nonmatchings/ether_tree", subTreeLineDraw_type_3);

INCLUDE_ASM("asm/main/nonmatchings/ether_tree", EtherTreeLineDraw);

INCLUDE_ASM("asm/main/nonmatchings/ether_tree", EtherTreeCursolDraw);

extern void EtherTreeObjectDraw(void);
extern void EtherTreeLineDraw(void);
extern void EtherTreeCursolDraw(void);
extern void EtherTreeRightDraw(void);
extern void EtherTreeBlackDraw(void);
extern int WindowTexAddrGet(int index);
extern void endPrintExtFunc(int kind, int id, void *data);

void EtherTreeDraw(void)
{
    if (!(EtherTreeSystem->flags & 1))
        return;

    EtherTreeSystem->windowTexAddr = WindowTexAddrGet(2);
    endPrintExtFunc(0, 0xE, &EtherTreeSystem->windowTexAddr);
    EtherTreeObjectDraw();
    EtherTreeLineDraw();
    EtherTreeLine2Draw();
    EtherTreeCursolDraw();
    EtherTreeRightDraw();
    EtherTreeBlackDraw();
    endPrintExtFunc(0, 0xF, 0);
}

INCLUDE_ASM("asm/main/nonmatchings/ether_tree", EtherTreeInit);

/* True for a robot ether index (annotations/slus_204.69_annotations.csv: 13 through 16). */
static int RoboEtherCheck(int etherIndex)
{
    return (unsigned int) (etherIndex - 13) < 4U;
}

INCLUDE_ASM("asm/main/nonmatchings/ether_tree", EtherTreeMain);
