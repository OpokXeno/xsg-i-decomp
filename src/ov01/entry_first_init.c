/*
 * OV01 original TU 7: 0x00a22628..0x00a24a98 (50 functions)
 */
#include "common.h"
#include "entry_first_init.h"

INCLUDE_ASM("asm/nonmatchings/ov01/entry_first_init", entryFirstInit);

INCLUDE_ASM("asm/nonmatchings/ov01/entry_first_init", battleProc);

INCLUDE_ASM("asm/nonmatchings/ov01/entry_first_init", battleProcEnd);

INCLUDE_ASM("asm/nonmatchings/ov01/entry_first_init", frameCntGet);

INCLUDE_ASM("asm/nonmatchings/ov01/entry_first_init", battleTimeGet);

INCLUDE_ASM("asm/nonmatchings/ov01/entry_first_init", cameraFlagSet);

INCLUDE_ASM("asm/nonmatchings/ov01/entry_first_init", cameraFlagGet);

INCLUDE_ASM("asm/nonmatchings/ov01/entry_first_init", pauseProc);

INCLUDE_ASM("asm/nonmatchings/ov01/entry_first_init", selectPL);

INCLUDE_ASM("asm/nonmatchings/ov01/entry_first_init", lastBossBtlChk);

INCLUDE_ASM("asm/nonmatchings/ov01/entry_first_init", bossBtlChk);

INCLUDE_ASM("asm/nonmatchings/ov01/entry_first_init", entryPhase00);

INCLUDE_ASM("asm/nonmatchings/ov01/entry_first_init", entryPhase10);

INCLUDE_ASM("asm/nonmatchings/ov01/entry_first_init", entryPhase15);

INCLUDE_ASM("asm/nonmatchings/ov01/entry_first_init", entryPhase16);

INCLUDE_ASM("asm/nonmatchings/ov01/entry_first_init", entryPhase20);

INCLUDE_ASM("asm/nonmatchings/ov01/entry_first_init", entryPhase25);

INCLUDE_ASM("asm/nonmatchings/ov01/entry_first_init", entryPhase26);

INCLUDE_ASM("asm/nonmatchings/ov01/entry_first_init", entryPhase30);

INCLUDE_ASM("asm/nonmatchings/ov01/entry_first_init", entryPhase31);

INCLUDE_ASM("asm/nonmatchings/ov01/entry_first_init", entryPhase35);

INCLUDE_ASM("asm/nonmatchings/ov01/entry_first_init", entryPhase36);

INCLUDE_ASM("asm/nonmatchings/ov01/entry_first_init", entryPhase40);

INCLUDE_ASM("asm/nonmatchings/ov01/entry_first_init", entryPhase45);

INCLUDE_ASM("asm/nonmatchings/ov01/entry_first_init", entryPhase50);

INCLUDE_ASM("asm/nonmatchings/ov01/entry_first_init", entryPhase51);

INCLUDE_ASM("asm/nonmatchings/ov01/entry_first_init", entryPhase60);

INCLUDE_ASM("asm/nonmatchings/ov01/entry_first_init", entryPhase70);

INCLUDE_ASM("asm/nonmatchings/ov01/entry_first_init", entryPhase80);

INCLUDE_ASM("asm/nonmatchings/ov01/entry_first_init", entryPhase90);

INCLUDE_ASM("asm/nonmatchings/ov01/entry_first_init", entryPhase100);

INCLUDE_ASM("asm/nonmatchings/ov01/entry_first_init", encountEffDisp);

INCLUDE_ASM("asm/nonmatchings/ov01/entry_first_init", cfSnap);

INCLUDE_ASM("asm/nonmatchings/ov01/entry_first_init", battleDisp);

INCLUDE_ASM("asm/nonmatchings/ov01/entry_first_init", vramCopyObjCreate);

INCLUDE_ASM("asm/nonmatchings/ov01/entry_first_init", vramCopyDummy);

INCLUDE_ASM("asm/nonmatchings/ov01/entry_first_init", vramCopyObj);

INCLUDE_ASM("asm/nonmatchings/ov01/entry_first_init", fadeIn);

INCLUDE_ASM("asm/nonmatchings/ov01/entry_first_init", fadeOut);

int fadeEndChk(void)
{
    return fadeFlag;
}

INCLUDE_ASM("asm/nonmatchings/ov01/entry_first_init", fadeInObj);

INCLUDE_ASM("asm/nonmatchings/ov01/entry_first_init", fadeOutObj);

INCLUDE_ASM("asm/nonmatchings/ov01/entry_first_init", fadeObjDraw);

INCLUDE_ASM("asm/nonmatchings/ov01/entry_first_init", encEnvMake);

INCLUDE_ASM("asm/nonmatchings/ov01/entry_first_init", encEffObjInit);

INCLUDE_ASM("asm/nonmatchings/ov01/entry_first_init", encEffObj);

INCLUDE_ASM("asm/nonmatchings/ov01/entry_first_init", effCalcMatrix);

INCLUDE_ASM("asm/nonmatchings/ov01/entry_first_init", encEffObjDraw);

INCLUDE_ASM("asm/nonmatchings/ov01/entry_first_init", encEffAppend);

INCLUDE_ASM("asm/nonmatchings/ov01/entry_first_init", encEffVramCopy);
