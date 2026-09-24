/*
 * OV01 original TU 7: 0x00a22628..0x00a24a98 (50 functions)
 */
#include "common.h"
#include "entry_first_init.h"
#include "main/xgl_studio.h"
#include "ov01/battle_init.h"

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

extern void UnduInit(int flag);
extern void ACT_init(void);
extern void ACT_initSequence(void);
extern void ACT_DrawShadowInit(void);
extern void objInit(void);
extern void dataCdSyncClear(void);
extern void battleInit(void);
extern void sefInitEffectBattle(void);
extern void JNT_onSmoothHair(void);
extern void xglCullingIgnore(void);
extern void xglStudioChange(int studio_index);
extern void xglLightSetDefault(StudioLight *light);
extern void xglLightIntensityParallel(StudioLight *light, int index, Vector4 *intensity);
extern void xglLightIntensityAmbient(StudioLight *light, Vector4 *ambient);
void cfSnap(void);
extern void eBattleWinInit(void);
extern void thinkInit(void);
extern void MCamTakeControl(void);
extern int monsSetNoGet(void);
extern int thinkMonsTblNumGet(void);
extern int thinkNoGet(void);
extern int cfEncountGet(void);
extern const char D_00A46620[];
extern int printf(const char *format, ...);
extern void GameDefocusQuickSet(int first, int second, int third, int fourth);
extern int sndSysSePlay(int soundId);
int entryPhase10(void);
extern int battleDispMode;
extern Vector4 sIntPararel2_0;
extern Vector4 sIntAmbient_1;

int entryPhase00(void) {
    StudioLight *light;
    int cfEncount;
    int thinkNo;
    int thinkMonsTblNum;
    int monsSetNo;
    int mapNo;

    UnduInit(0);
    battleDispMode = 0;
    ACT_init();
    ACT_initSequence();
    ACT_DrawShadowInit();
    objInit();
    dataCdSyncClear();
    battleInit();
    sefInitEffectBattle();
    JNT_onSmoothHair();
    xglCullingIgnore();
    xglStudioChange(0);
    xglStudioGetLight(&light);
    xglLightSetDefault(light);
    xglLightIntensityParallel(light, 2, &sIntPararel2_0);
    xglLightIntensityAmbient(light, &sIntAmbient_1);
    cfSnap();
    eBattleWinInit();
    thinkInit();
    if (cameraFlagGet() != 0) {
        MCamTakeControl();
    }
    mapNo = mapNoGet();
    monsSetNo = monsSetNoGet();
    thinkMonsTblNum = thinkMonsTblNumGet();
    thinkNo = thinkNoGet();
    cfEncount = cfEncountGet();
    printf(D_00A46620, mapNo, monsSetNo, thinkMonsTblNum, thinkNo, cfEncount, cfEventGet());
    GameDefocusQuickSet(0, 1, 0xFFFFFF, 0);
    sndSysSePlay(0x25);
    battleSeq = entryPhase10;
    return 1;
}

INCLUDE_ASM("asm/nonmatchings/ov01/entry_first_init", entryPhase10);

extern int dataCdSync(void);
extern int D_00A5A214;
extern void sndMuTransPlay(int id);
extern void dataXtxLoad(void);
extern FadeObjTask *pFadeIn;
int entryPhase16(void);

int entryPhase15(void)
{
    if (dataCdSync() <= D_00A5A214) {
        sndMuTransPlay(0);
        D_00A5A214 = dataCdSync();
        dataXtxLoad();
        battleSeq = entryPhase16;
    }
    pFadeIn->delay = 2;
    return 1;
}

extern void dataMapLoad(int mapNo);
int entryPhase20(void);

int entryPhase16(void)
{
    if (dataCdSync() <= D_00A5A214) {
        D_00A5A214 = dataCdSync();
        dataMapLoad(mapNoGet());
        battleSeq = entryPhase20;
    }
    pFadeIn->delay = 2;
    return 1;
}

INCLUDE_ASM("asm/nonmatchings/ov01/entry_first_init", entryPhase20);

INCLUDE_ASM("asm/nonmatchings/ov01/entry_first_init", entryPhase25);

int entryPhase25(void);
extern int sefCheckLoad(void);
extern void dataUnitFileLoad2(int unit);
extern int pCreateUnit;

/*
 * D_00A5A220 is a second camera-event request record: camEventExec's other
 * caller (src/ov01/battle_init.c's D_00A57B80) fills the same two fields,
 * a type code at +0x00 and the acting unit at +0x08, before the call.
 */
typedef struct EntryCamEvent {
    int type;                  /* +0x00 */
    unsigned char unmodeled_04[4];
    BattleUnit *unit;          /* +0x08 */
} EntryCamEvent;
extern EntryCamEvent D_00A5A220;

/*
 * This TU's own call site sets up only the event-record argument, unlike
 * src/ov01/battle_init.c's accepted call, which declares camEventExec void
 * and passes a second (BattleUnit *) argument: each TU forward-declares the
 * function as its own call site uses it.
 */
extern int camEventExec(EntryCamEvent *event);

int entryPhase26(void)
{
    if (dataCdSync() <= D_00A5A214 && sefCheckLoad() == 0) {
        dataUnitFileLoad2(pCreateUnit);
        D_00A5A220.unit = (BattleUnit *)pCreateUnit;
        D_00A5A220.type = 0x17;
        camEventExec(&D_00A5A220);
        battleSeq = entryPhase25;
    }
    return 1;
}

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

extern int unitIdx;
extern void dataSndSeLoad2(int unit);
extern void sndSeTransPlay(int unit, int seId, int volume);
extern int entryPhase35(void);

int entryPhase31(void)
{
    if (dataCdSync() <= D_00A5A214) {
        if (pCvUnit != 0) {
            dataSndSeLoad2(pCvUnit);
            sndSeTransPlay(pCvUnit, 0x18, 1);
        }
        D_00A5A214 = dataCdSync();
        unitIdx = 0;
        battleSeq = entryPhase35;
    }
    return 1;
}

INCLUDE_ASM("asm/nonmatchings/ov01/entry_first_init", entryPhase35);

extern void dataUnitFileLoad2(int unit);
extern int entryPhase35(void);
extern int sefCheckLoad(void);
extern int pCreateUnit;

int entryPhase36(void)
{
    if (dataCdSync() <= D_00A5A214 && sefCheckLoad() == 0) {
        dataUnitFileLoad2(pCreateUnit);
        battleSeq = entryPhase35;
    }
    return 1;
}

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

/*
 * This call site reads battleMain's return register even though
 * src/ov01/battle_init.c's own accepted definition declares it void; each TU
 * forward-declares the function as its own call site uses it.
 */
extern int battleMain(void);
extern int battleRetCodeGet(void);
extern BattleUnit *lastUnitGet(void);
extern int sndMuStop(void);
extern void GameDefocusSet(int first, int second, int third);
int entryPhase51(void);

int entryPhase50(void)
{
    int unit;

    if (battleMain() == 0) {
        D_00A5A214 = dataCdSync();
        if (battleRetCodeGet() == 1) {
            unit = selectPL((int)lastUnitGet());
            pCvUnit = unit;
            if (unit != 0) {
                dataSndSeLoad(unit, 0x19);
            }
            sndMuStop();
            if (lastBossBtlChk() == 0) {
                sndMuTransPlay(1);
            }
        } else {
            pCvUnit = 0;
        }
        GameDefocusSet(-1, 0, 0);
        battleSeq = entryPhase51;
    }
    return 1;
}

extern void dataSndSeLoad2(int unit);
extern void sndSeTransPlay(int unit, int seId, int volume);
int entryPhase60(void);

int entryPhase51(void)
{
    if (dataCdSync() <= D_00A5A214) {
        if (pCvUnit != 0) {
            dataSndSeLoad2(pCvUnit);
            sndSeTransPlay(pCvUnit, 0x19, 1);
        }
        battleSeq = entryPhase60;
    }
    return 1;
}

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

extern void calcDeadReset(void);
extern void dataLeaderReload(void);
extern int sndMuStop(void);

int entryPhase100(void) {
    calcDeadReset();
    dataLeaderReload();
    sndMuStop();
    return 0;
}

extern void sefCreateEffect(int *params);

/*
 * The local effect-request block sefCreateEffect reads (evidenced only by
 * this call and by src/ov01/battle_init.c's DecoEffParams, which shares the
 * same 0x24-byte size and +0x14 subId offset): only subId is set here.
 */
typedef struct EncounterEffectRequest {
    unsigned char unmodeled_00[0x14];
    short subId;                      /* +0x14 */
    unsigned char unmodeled_16[0x24 - 0x16];
} EncounterEffectRequest;

void encountEffDisp(void) {
    EncounterEffectRequest params;

    memset(&params, 0, sizeof(params));
    if (cfEncountGet() & 1) {
        params.subId = 0xA42;
        sefCreateEffect((int *)&params);
    }
    if (cfEncountGet() & 2) {
        params.subId = 0xA43;
        sefCreateEffect((int *)&params);
    }
    if (cfEncountGet() & 4) {
        params.subId = 0xA44;
        sefCreateEffect((int *)&params);
    }
}

extern int grBBIdxGet(void);
extern void grDBIdxGet(int *dispIdx, int *unused);
extern ObjectTask *vramCopyObjCreate(int srcAddr, int dstAddr);

void cfSnap(void) {
    int dispIdx;
    int unused;

    grDBIdxGet(&dispIdx, &unused);
    vramCopyObjCreate(dispIdx, grBBIdxGet());
}

INCLUDE_ASM("asm/nonmatchings/ov01/entry_first_init", battleDisp);

extern ObjectTask *objEntry2(void *argument, void (*callback)(ObjectTask *task));
extern void objStdInit(ObjectTask *task);
void vramCopyDummy(void);
void vramCopyObj(ObjectTask *task);

ObjectTask *vramCopyObjCreate(int srcAddr, int dstAddr) {
    ObjectTask *task;
    VramCopyObjTask *self;

    task = objEntry2(vramCopyDummy, vramCopyObj);
    objStdInit(task);
    self = (VramCopyObjTask *)task;
    self->srcAddr = srcAddr;
    self->dstAddr = dstAddr;
    return task;
}

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

void fadeOutObj(FadeObjTask *self);
extern void fadeObjDraw(FadeObjTask *self);

int fadeOut(int speed, int delay) {
    FadeObjTask *self;

    self = (FadeObjTask *)objEntry2(fadeOutObj, (void (*)(ObjectTask *))fadeObjDraw);
    self->delay = delay;
    self->speed = speed;
    self->life = 0x80;
    fadeFlag = 0;
    return (int)self;
}

int fadeEndChk(void)
{
    return fadeFlag;
}

void fadeInObj(ObjectTask *task) {
    FadeObjTask *self = (FadeObjTask *)task;

    if (self->delay != 0) {
        self->delay--;
        return;
    }
    self->life += self->speed;
    if ((unsigned int)self->life >= 0x81) {
        fadeFlag = 1;
        objRemove(task);
    }
}

extern void fadeObjDraw(FadeObjTask *self);

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
        fadeObjDraw(self);
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
