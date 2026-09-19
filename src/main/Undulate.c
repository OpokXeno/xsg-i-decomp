#include "common.h"
#include "Undulate.h"

INCLUDE_ASM("asm/main/nonmatchings/Undulate", UnduInit);

INCLUDE_ASM("asm/main/nonmatchings/Undulate", UnduDataGetHeaderSub);

INCLUDE_ASM("asm/main/nonmatchings/Undulate", UnduDataGetHeader);

INCLUDE_ASM("asm/main/nonmatchings/Undulate", UnduParamInit);

INCLUDE_ASM("asm/main/nonmatchings/Undulate", UnduCheckSub);

INCLUDE_ASM("asm/main/nonmatchings/Undulate", UnduCheckSubHeightCheck);

static float UnduCheckSub(UnduWork *work, int subOffset);
static void UnduCheckSubHeightCheck(UnduWork *work, int subOffset, float height);

static void UnduCheckSubCheckAll(UnduWork *work) {
    int subOffset;
    int entryOffset;
    int i;
    float height;

    subOffset = work->subListOffset;
    for (i = 0; i < work->subCount; i++) {
        height = UnduCheckSub(work, subOffset);
        entryOffset = subOffset;
        subOffset += 0x18;
        if (height != -1000.0f) {
            UnduCheckSubHeightCheck(work, entryOffset, height);
        }
    }
}

INCLUDE_ASM("asm/main/nonmatchings/Undulate", CheckWallAttr);

INCLUDE_ASM("asm/main/nonmatchings/Undulate", UnduCheckSubCrossCheck);

INCLUDE_ASM("asm/main/nonmatchings/Undulate", UnduCheckResultCheck);

INCLUDE_ASM("asm/main/nonmatchings/Undulate", UnduCheck);

INCLUDE_ASM("asm/main/nonmatchings/Undulate", UnduGet2);

INCLUDE_ASM("asm/main/nonmatchings/Undulate", UnduGet);
