/*
 * OV01 original TU 7: 0x00a22628..0x00a24a98 (50 functions)
 */
#include "common.h"
#include "entry_first_init.h"

extern void sefInitEffect(void);
extern void MBattleInit(void);
extern void dataBatDatLoad(void);
extern void dataPlUnitInit(void);
extern void thinkNoSet(int thinkNo);
extern void mapNoSet(int mapNo);

void entryFirstInit(void) {
    sefInitEffect();
    MBattleInit();
    dataBatDatLoad();
    dataPlUnitInit();
    thinkNoSet(0);
    mapNoSet(1);
}

INCLUDE_ASM("asm/nonmatchings/ov01/entry_first_init", battleProc);

INCLUDE_ASM("asm/nonmatchings/ov01/entry_first_init", battleProcEnd);

extern int frameCnt;

int frameCntGet(void) {
    return frameCnt;
}

int battleTimeGet(void) {
    return frameCntGet();
}

void cameraFlagSet(int flag) {
    cameraFlag = flag;
}

int cameraFlagGet(void) {
    return cameraFlag;
}

int pauseProc(void) {
    return 1;
}

INCLUDE_ASM("asm/nonmatchings/ov01/entry_first_init", selectPL);

extern int mapNoGet(void);

/* lastBossBtlChk (0x00a22a60): map 0x34 is the game's final-boss battle map. */
#define MAP_NO_LAST_BOSS 0x34

int lastBossBtlChk(void) {
    return mapNoGet() == MAP_NO_LAST_BOSS;
}

extern int cfEventGet(void);

int bossBtlChk(void)
{
    /* Boss battle requires cfEventGet bit 3 (0x8) set and bit 4 (0x10) clear. */
    if (cfEventGet() & 8) {
        if (!(cfEventGet() & 0x10)) {
            return 1;
        }
    }
    return 0;
}

INCLUDE_ASM("asm/nonmatchings/ov01/entry_first_init", entryPhase00);

INCLUDE_ASM("asm/nonmatchings/ov01/entry_first_init", entryPhase10);

INCLUDE_ASM("asm/nonmatchings/ov01/entry_first_init", entryPhase15);

INCLUDE_ASM("asm/nonmatchings/ov01/entry_first_init", entryPhase16);

INCLUDE_ASM("asm/nonmatchings/ov01/entry_first_init", entryPhase20);

INCLUDE_ASM("asm/nonmatchings/ov01/entry_first_init", entryPhase25);

INCLUDE_ASM("asm/nonmatchings/ov01/entry_first_init", entryPhase26);

extern int dataCdSync(void);
extern int selectPL(int excludeUnit);
extern void dataSndSeLoad(int unit, int seId);
extern int entryPhase31(void);

extern int D_00A5A214;
extern int pCvUnit;

int entryPhase30(void) {
    int unit;

    D_00A5A214 = dataCdSync();
    unit = selectPL(0);
    pCvUnit = unit;
    if (unit != 0) {
        dataSndSeLoad(unit, 0x18);
    }
    battleSeq = &entryPhase31;
    return 1;
}

INCLUDE_ASM("asm/nonmatchings/ov01/entry_first_init", entryPhase31);

INCLUDE_ASM("asm/nonmatchings/ov01/entry_first_init", entryPhase35);

INCLUDE_ASM("asm/nonmatchings/ov01/entry_first_init", entryPhase36);

extern int cfEncountGet(void);
extern void calcCfEncount(int flags);
extern void thinkInitExec(void);
extern void sdvSaveAmbient(void);
extern void sefSetupEffect(void);
extern int entryPhase45(void);

int entryPhase40(void) {
    calcCfEncount(cfEncountGet());
    thinkInitExec();
    sdvSaveAmbient();
    sefSetupEffect();
    battleSeq = &entryPhase45;
    return 1;
}

INCLUDE_ASM("asm/nonmatchings/ov01/entry_first_init", entryPhase45);

INCLUDE_ASM("asm/nonmatchings/ov01/entry_first_init", entryPhase50);

INCLUDE_ASM("asm/nonmatchings/ov01/entry_first_init", entryPhase51);

extern int battleAfter(void);
extern int fadeOut(int, int);
extern int entryPhase70(void);

int entryPhase60(void) {
    if (battleAfter() == 0) {
        fadeOut(4, 2);
        battleSeq = entryPhase70;
    }
    return 1;
}

INCLUDE_ASM("asm/nonmatchings/ov01/entry_first_init", entryPhase70);

INCLUDE_ASM("asm/nonmatchings/ov01/entry_first_init", entryPhase80);

extern int resultProc(void);
extern int entryPhase100(void);

int entryPhase90(void) {
    if (resultProc() == 0) {
        battleSeq = entryPhase100;
    }
    return 1;
}

INCLUDE_ASM("asm/nonmatchings/ov01/entry_first_init", entryPhase100);

INCLUDE_ASM("asm/nonmatchings/ov01/entry_first_init", encountEffDisp);

INCLUDE_ASM("asm/nonmatchings/ov01/entry_first_init", cfSnap);

INCLUDE_ASM("asm/nonmatchings/ov01/entry_first_init", battleDisp);

INCLUDE_ASM("asm/nonmatchings/ov01/entry_first_init", vramCopyObjCreate);

void vramCopyDummy(void) {

}

extern int grVramCopyFBtoFB(int src, int dst);
extern void objRemove(ObjectTask *task);

void vramCopyObj(ObjectTask *task) {
    VramCopyObjTask *self = (VramCopyObjTask *)task;

    grVramCopyFBtoFB(self->srcAddr, self->dstAddr);
    objRemove(task);
}

INCLUDE_ASM("asm/nonmatchings/ov01/entry_first_init", fadeIn);

INCLUDE_ASM("asm/nonmatchings/ov01/entry_first_init", fadeOut);

int fadeEndChk(void)
{
    return fadeFlag;
}

INCLUDE_ASM("asm/nonmatchings/ov01/entry_first_init", fadeInObj);

extern void fadeObjDraw(void);

void fadeOutObj(FadeObjTask *self)
{
    if (self->delay != 0) {
        self->delay--;
        return;
    }

    self->life -= self->speed;
    if (self->life == 0) {
        /* The original stores 0 into life here a second time (a separate
         * `sw` right after the subtraction's own store), even though it is
         * already 0 on this path. */
        self->life = 0;
        fadeObjDraw();
        fadeFlag = 1;
        objRemove(&self->task);
    }
}

INCLUDE_ASM("asm/nonmatchings/ov01/entry_first_init", fadeObjDraw);

INCLUDE_ASM("asm/nonmatchings/ov01/entry_first_init", encEnvMake);

INCLUDE_ASM("asm/nonmatchings/ov01/entry_first_init", encEffObjInit);

INCLUDE_ASM("asm/nonmatchings/ov01/entry_first_init", encEffObj);

INCLUDE_ASM("asm/nonmatchings/ov01/entry_first_init", effCalcMatrix);

INCLUDE_ASM("asm/nonmatchings/ov01/entry_first_init", encEffObjDraw);

INCLUDE_ASM("asm/nonmatchings/ov01/entry_first_init", encEffAppend);

INCLUDE_ASM("asm/nonmatchings/ov01/entry_first_init", encEffVramCopy);
