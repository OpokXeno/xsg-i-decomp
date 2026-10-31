/*
 * OV01 original TU 7: 0x00a22628..0x00a24a98 (50 functions)
 */
#include "common.h"
#include "entry_first_init.h"
#include "main/xgl_studio.h"
#include "ov01/battle_init.h"

static int cameraFlag = 1;
static int frameCnt;
static EntryPhaseFunc battleSeq;
static int fadeFlag;

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

#include "ov01/calc.h"

/*
 * This TU's own view of battle_init.c's BattleUnit/Actor (there kept behind
 * an opaque typedef, full body local to that TU): only the fields the
 * flag check in entryPhase45 reads (BattleUnit.actor +0x14 and Actor.flags
 * +0x00, both evidenced there by src/ov01/battle_init.c's actUnitTblMake).
 */
typedef struct EntryActor {
    unsigned int flags; /* +0x00 */
} EntryActor;

/*
 * Actor.flags bit that unitDispOff (src/ov01/unit_cmd.c) sets on a unit's
 * motion actor and unitActupdate tests to skip the motion update: the actor
 * is display-off and update-disabled while it is set.
 */
#define ENTRY_ACTOR_FLAG_DISP_OFF 0x8u

typedef struct EntryBattleUnit {
    unsigned char unmodeled_00[0x14];
    EntryActor *actor;  /* +0x14 */
} EntryBattleUnit;

extern int calcBattleChk(ObjectTask *unit);
extern CalcUnitParam *calcUPGet(ObjectTask *unit);
extern int unitLiveNumGet2(int side);
extern int unitTblGet(int side, EntryBattleUnit ***table);
extern int rnd(int max);

/*
 * selectPL's own charaId cutoff (`lh $3,0x38($2)`/`slti $3,$3,0x8`): the
 * comparison itself only proves calcUPGet(unit)->charaId < 8.
 */
#define SELECT_PL_CHARA_ID_LIMIT 8

/*
 * unitTblGet(0, ...) returns the player side's three records (unit_cmd.c's
 * unitTbl), so `count` is bounded by
 * 3 and `picked` never needs more slots than that.
 */
ObjectTask *selectPL(ObjectTask *excludeUnit)
{
    EntryBattleUnit **table;
    EntryBattleUnit *picked[3];
    int count;
    int i;
    int pickedCount;
    int idx;

    if (excludeUnit != 0) {
        if (calcBattleChk(excludeUnit) != 0) {
            if (calcUPGet(excludeUnit)->charaId < SELECT_PL_CHARA_ID_LIMIT) {
                return excludeUnit;
            }
        }
    }

    if (unitLiveNumGet2(0) == 0) {
        return 0;
    }

    i = 0;
    count = unitTblGet(0, &table);
    pickedCount = 0;
    if (count > 0) {
        for (; i < count; i++) {
            if (calcBattleChk((ObjectTask *)table[i]) != 0
                && calcUPGet((ObjectTask *)table[i])->charaId < SELECT_PL_CHARA_ID_LIMIT) {
                picked[pickedCount] = table[i];
                pickedCount++;
            }
        }
    }

    if (pickedCount == 0) {
        return 0;
    }

    idx = rnd(pickedCount - 1);
    return (ObjectTask *)picked[idx];
}

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
const char D_00A46620[] = "** map=%d monsSet=%d/%d Think=%d cfEncount=%X cfEvent=%X**\n";
extern int printf(const char *format, ...);
extern void GameDefocusQuickSet(int first, int second, int third, int fourth);
extern int sndSysSePlay(int soundId);
int entryPhase10(void);
static int battleDispMode;
static Vector4 sIntPararel2_0 = { 0.0f, 0.0f, 0.0f, 1.0f };
static Vector4 sIntAmbient_1 = { 0.5f, 0.5f, 0.5f, 1.0f };

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
static int loadNum;
extern void sndMuTransPlay(int id);
extern void dataXtxLoad(void);
static FadeObjTask *pFadeIn;
int entryPhase16(void);

int entryPhase15(void)
{
    if (dataCdSync() <= loadNum) {
        sndMuTransPlay(0);
        loadNum = dataCdSync();
        dataXtxLoad();
        battleSeq = entryPhase16;
    }
    pFadeIn->delay = 2;
    return 1;
}

extern void dataMapLoad(int mapNo);
int entryPhase20(void);
int entryPhase25(void);

/*
 * This TU's own declaration order of its static objects (pCreateUnit then
 * camEv then unitIdx) has to stay the same relative order the already
 * matched entryPhase26/30/31 established (their gate pass proved these
 * addresses); entryPhase20 needs camEv and unitIdx before their own
 * definitions further down, so each gets an early `extern` mention here,
 * in that same order, ahead of its real storage.
 */
extern ObjectTask *pCreateUnit;

/*
 * camEv is a second camera-event request record: camEventExec's other
 * caller (src/ov01/battle_init.c's D_00A57B80) fills the same two fields,
 * a type code at +0x00 and the acting unit at +0x08, before the call.
 */
typedef struct EntryCamEvent {
    int type;                  /* +0x00 */
    unsigned char unmodeled_04[4];
    BattleUnit *unit;          /* +0x08 */
    unsigned char unmodeled_0c[4];
} EntryCamEvent;
extern EntryCamEvent camEv;
extern ObjectTask *pCvUnit;
extern int unitIdx;

/*
 * This TU's own call site sets up only the event-record argument, unlike
 * src/ov01/battle_init.c's accepted call, which declares camEventExec void
 * and passes a second (BattleUnit *) argument: each TU forward-declares the
 * function as its own call site uses it.
 */
extern int camEventExec(EntryCamEvent *event);

/*
 * This TU's own call site passes mapNoGet()'s result as mapInit's argument
 * (`daddu $4,$2,$0` before `jal mapInit`), unlike src/ov01/map_disp.c's
 * accepted definition, which declares mapInit void and ignores it: each TU
 * forward-declares the function as its own call site uses it.
 */
extern void mapInit(int mapNo);
extern void camInit(void);
extern int camInitExec(void);
extern int encEffCmd;

int entryPhase16(void)
{
    if (dataCdSync() <= loadNum) {
        loadNum = dataCdSync();
        dataMapLoad(mapNoGet());
        battleSeq = entryPhase20;
    }
    pFadeIn->delay = 2;
    return 1;
}

int entryPhase20(void)
{
    if (dataCdSync() <= loadNum) {
        mapInit(mapNoGet());
        camInit();
        camInitExec();
        camEv.type = 4;
        camEventExec(&camEv);
        battleDispMode = 1;
        encEffCmd = 1;
        loadNum = dataCdSync();
        unitIdx = 0;
        battleSeq = entryPhase25;
    } else {
        pFadeIn->delay = 2;
    }
    return 1;
}

/*
 * plUnitTblGet's own PlUnitEntry (src/ov01/battle_init.c): this TU reads
 * only partyId (+0x00), the slot-occupied flag entryPhase25's own search
 * and status-display match both test (`lh $x,0x0($y)`).
 */
typedef struct EntryPlUnit {
    short partyId;              /* +0x00 */
    unsigned char unmodeled_02[6];
} EntryPlUnit;

extern EntryPlUnit *plUnitTblGet(void);
extern ObjectTask *unitPlCreate(s16 *pl);
extern void menuStatDispCreate(void);
extern int entryPhase30(void);
extern int entryPhase26(void);

int entryPhase25(void)
{
    EntryPlUnit *entries;
    ObjectTask *unit;
    int i;
    int lastOccupiedIdx;

    /* Advance unitIdx to the next occupied party slot, if any. */
    entries = plUnitTblGet();
    if (unitIdx < 3 && entries[unitIdx].partyId == 0) {
        do {
            unitIdx++;
            if (unitIdx >= 3) {
                break;
            }
        } while (entries[unitIdx].partyId == 0);
    }

    if (unitIdx >= 3) {
        battleSeq = entryPhase30;
        return 1;
    }

    loadNum = dataCdSync();
    unit = unitPlCreate(&entries[unitIdx].partyId);
    pCreateUnit = unit;
    unitIdx++;

    lastOccupiedIdx = 0;
    for (i = 0; i < 3; i++) {
        if (entries[i].partyId != 0) {
            lastOccupiedIdx = i;
        }
    }
    if (lastOccupiedIdx == unitIdx - 1) {
        menuStatDispCreate();
    }
    battleSeq = entryPhase26;
    return 1;
}

extern int sefCheckLoad(void);
extern void dataUnitFileLoad2(ObjectTask *unit);
static ObjectTask *pCreateUnit;
static EntryCamEvent camEv;

int entryPhase26(void)
{
    if (dataCdSync() <= loadNum && sefCheckLoad() == 0) {
        dataUnitFileLoad2(pCreateUnit);
        camEv.unit = (BattleUnit *)pCreateUnit;
        camEv.type = 0x17;
        camEventExec(&camEv);
        battleSeq = entryPhase25;
    }
    return 1;
}


extern void dataSndSeLoad(ObjectTask *unit, int seId);
extern int entryPhase31(void);

static ObjectTask *pCvUnit;

int entryPhase30(void) {
    ObjectTask *unit;

    loadNum = dataCdSync();
    unit = selectPL(0);
    pCvUnit = unit;
    if (unit != 0) {
        dataSndSeLoad(unit, 0x18);
    }
    battleSeq = &entryPhase31;
    return 1;
}

static int unitIdx;
extern void dataSndSeLoad2(ObjectTask *unit);
extern void sndSeTransPlay(ObjectTask *unit, int seId, int volume);
extern int entryPhase35(void);

int entryPhase31(void)
{
    if (dataCdSync() <= loadNum) {
        if (pCvUnit != 0) {
            dataSndSeLoad2(pCvUnit);
            sndSeTransPlay(pCvUnit, 0x18, 1);
        }
        loadNum = dataCdSync();
        unitIdx = 0;
        battleSeq = entryPhase35;
    }
    return 1;
}

/*
 * src/ov01/cmd.c declares and defines thinkMonsTblGet returning plain int
 * (thinkAdrGet's own return, an address); this TU's own call site only
 * ever uses that address as the base of its spawn-entry table, so it is
 * forward-declared here as its own view, ThinkMonsEntry *.
 */
extern ThinkMonsEntry *thinkMonsTblGet(int monsSetNo);
extern ObjectTask *unitEnCreate(short *pl);
extern int entryPhase40(void);
extern int entryPhase36(void);

int entryPhase35(void)
{
    ThinkMonsEntry *entries;
    ObjectTask *unit;

    entries = thinkMonsTblGet(monsSetNoGet());
    if (unitIdx >= 5 || entries[unitIdx].charaId == 0) {
        battleSeq = entryPhase40;
        return 1;
    }
    loadNum = dataCdSync();
    unit = unitEnCreate(&entries[unitIdx].charaId);
    pCreateUnit = unit;
    unitIdx++;
    battleSeq = entryPhase36;
    return 1;
}




static ObjectTask *pCreateUnit;

int entryPhase36(void)
{
    if (dataCdSync() <= loadNum && sefCheckLoad() == 0) {
        dataUnitFileLoad2(pCreateUnit);
        battleSeq = entryPhase35;
    }
    return 1;
}


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

extern void encountEffDisp(void);
int entryPhase50(void);

int entryPhase45(void)
{
    EntryBattleUnit **table;
    EntryBattleUnit **next;
    int count;
    int remaining;
    int dispOffCount;

    if (sefCheckLoad() != 0) {
        goto done;
    }

    count = unitTblGet(2, &table);
    dispOffCount = 0;
    if (count > 0) {
        remaining = count;
        next = table;
        do {
            EntryBattleUnit *unit = *next;

            next++;
            if (unit != 0 && (unit->actor->flags & ENTRY_ACTOR_FLAG_DISP_OFF) != 0) {
                dispOffCount++;
            }
            remaining--;
        } while (remaining != 0);
    }

    if (dispOffCount != 0) {
        goto done;
    }

    encountEffDisp();
    frameCnt = 0;
    battleSeq = entryPhase50;

done:
    return 1;
}

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
    ObjectTask *unit;

    if (battleMain() == 0) {
        loadNum = dataCdSync();
        if (battleRetCodeGet() == 1) {
            unit = selectPL((ObjectTask *)lastUnitGet());
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



int entryPhase60(void);

int entryPhase51(void)
{
    if (dataCdSync() <= loadNum) {
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

int entryPhase70(void) {
    extern int battleRetCodeGet(void);
    extern int cfEventGet(void);
    extern void calcPara2Org(void);
    extern void resultProcInit(void);
    extern int entryPhase80(void);
    extern int entryPhase100(void);
    extern void battleProcEnd(void);

    if (fadeEndChk() != 0) {
        int retCode = battleRetCodeGet();

        if ((retCode == 1 || retCode == 3) && (cfEventGet() & 0x10) == 0) {
            calcPara2Org();
            battleDispMode = 2;
            resultProcInit();
            battleSeq = entryPhase80;
        } else {
            battleDispMode = 0;
            battleSeq = entryPhase100;
        }
        battleProcEnd();
    }
    return 1;
}

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

extern void MBattleExec(void);
extern void xglStudioFlushActiveCamera(void);
extern void mapDisp(int mapNo);
extern void ACT_draw(void);
extern void GameDrawShadow(void);
extern void SeisanDisp(void);

int battleDisp(void)
{
    switch (battleDispMode) {
    case 0:
        break;
    case 1:
        MBattleExec();
        xglStudioFlushActiveCamera();
        mapDisp(mapNoGet());
        ACT_draw();
        GameDrawShadow();
        break;
    case 2:
        SeisanDisp();
        break;
    }
    return 1;
}

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

void fadeInObj(ObjectTask *task);
extern void fadeObjDraw(FadeObjTask *self);

int fadeIn(int speed, int delay) {
    FadeObjTask *self;

    self = (FadeObjTask *)objEntry2(fadeInObj, (void (*)(ObjectTask *))fadeObjDraw);
    self->delay = delay;
    self->speed = speed;
    self->life = 0;
    fadeFlag = 0;
    return (int)self;
}

void fadeOutObj(FadeObjTask *self);


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

extern void GS_REG_SET(void *packet, long long register_id, long long control);
extern int grFBAdrGet(int index);

void encEnvMake(void *packet) {
    int tbp0 = (grFBAdrGet(grBBIdxGet()) << 5) & 0xFFFF;

    GS_REG_SET(packet, 0xE, 0x1000000000008005LL);
    GS_REG_SET(packet, 0x3F, 1);
    GS_REG_SET(packet, 0x6, tbp0 | 0x2000000664020000LL);
    GS_REG_SET(packet, 0x42, 0);
    GS_REG_SET(packet, 0x47, 0x30000);
    GS_REG_SET(packet, 0x14, 0x60);
}

INCLUDE_ASM("asm/nonmatchings/ov01/entry_first_init", encEffObjInit);

INCLUDE_ASM("asm/nonmatchings/ov01/entry_first_init", encEffObj);

/*
 * This TU's own view of the shared transform helpers (src/ov01/gr_gp_init.c
 * declares and defines the same functions for its own calls, using its own
 * TU-local Matrix/FloatVector3/FloatVector4 types): here, the project's
 * shared Vector4/Matrix4 (include/shared.h) already match those layouts, so
 * no pointer cast is needed at any call site.
 */
extern void xglMatrixUnit(Matrix4 matrix);
extern void xglMatrixTrans(Matrix4 destination, const Matrix4 source,
                           Vector4 *translation);
extern void xglMatrixScale(Matrix4 destination, const Matrix4 source,
                           Vector4 *scale);
/*
 * grRotMatrix only reads angles->x/y/z (Y/X/Z Euler angles); the stack slot
 * this call site builds also carries a fourth word (the original .s stores
 * 1.0f at rotation+0xC, matching this TU's other two vectors' own unused .w
 * stores), so the argument stays a full Vector4 to keep that store, even
 * though grRotMatrix itself never reads angles->w.
 */
extern void grRotMatrix(Matrix4 destination, const Matrix4 source,
                        Vector4 *angles);

/* theta (original ELF symbol, src/ov01/entry_first_init.c's own .bss). */
extern float theta;

void effCalcMatrix(ObjectTask *task, Matrix4 matrix, int index) {
    Vector4 rotation;
    Vector4 translation;
    Vector4 scale;

    xglMatrixUnit(matrix);

    translation.x = 0.0f;
    translation.y = 0.0f;
    translation.z = 0.0f;
    translation.w = 1.0f;

    rotation.x = 0.0f;
    rotation.y = 0.0f;
    rotation.w = 1.0f;
    rotation.z = theta * (float)index * 0.125f;

    scale.x = 1.0f;
    scale.y = 1.0f;
    scale.z = 1.0f;
    scale.w = 1.0f;

    xglMatrixTrans(matrix, matrix, &translation);
    grRotMatrix(matrix, matrix, &rotation);
    xglMatrixScale(matrix, matrix, &scale);
}

INCLUDE_ASM("asm/nonmatchings/ov01/entry_first_init", encEffObjDraw);

extern void grGpInit(GrPacket *packet);
extern void grPacketSend(GrPacket *packet);

/*
 * Sends two GIF-tagged UV/RGBAQ/XYZ2 vertex pairs (regs 0x513), UV 0..
 * (0x1FF0, 0x1BF0), textured from grFBAdrGet(drawingIndex) through TEX0,
 * offset by +16 then -16 in 12.4 fixed point (one pixel) on both axes.
 * gp is the original's callee-saved packet pointer (`daddu $17,$29,$0` in
 * grGpInit's delay slot, every later data/count read goes through $17), and
 * depth its Z, loaded into $18 at entry (`lui/ori $18` in the first calls'
 * delay slots).
 */
void encEffAppend(void)
{
    GrPacket packet;
    GrPacket *gp;
    int drawingIndex;
    int currentIndex;
    int tbp0;
    int depth = 0xFFFFFD;
    int *count;
    int dx;
    int dy;
    int i;

    grDBIdxGet(&drawingIndex, &currentIndex);
    tbp0 = (grFBAdrGet(drawingIndex) << 5) & 0xFFFF;
    gp = &packet;
    grGpInit(&packet);
    GS_REG_SET(&packet, 0xE, 0x1000000000008005LL);
    GS_REG_SET(&packet, 0x3F, 1);
    GS_REG_SET(&packet, 0x6, tbp0 | 0x2000000664020000LL);
    GS_REG_SET(&packet, 0x42, 0x44);
    GS_REG_SET(&packet, 0x47, 0x30000);
    GS_REG_SET(&packet, 0x14, 0);

    for (i = 0; i < 2; i++) {
        if (i == 0) {
            dx = 1;
            dy = 1;
        } else {
            dx = -1;
            dy = -1;
        }
        GR_PACKET_QUAD(gp, gp->count)->tag.register_id = 0x513;
        count = &gp->count;
        GR_PACKET_QUAD(gp, (*count)++)->tag.control = 0x30AB400000008002LL;

        GR_PACKET_QUAD(gp, *count)->uv.u = 0;
        GR_PACKET_QUAD(gp, (*count)++)->uv.v = 0;
        GR_PACKET_QUAD(gp, *count)->rgba.r = 0x80;
        GR_PACKET_QUAD(gp, *count)->rgba.g = 0x80;
        GR_PACKET_QUAD(gp, *count)->rgba.b = 0x80;
        GR_PACKET_QUAD(gp, (*count)++)->rgba.a = 0x80;
        GR_PACKET_QUAD(gp, *count)->xyz.x = dx * 16 + 0x7000;
        GR_PACKET_QUAD(gp, *count)->xyz.y = dy * 16 + 0x7200;
        GR_PACKET_QUAD(gp, *count)->xyz.z = depth;
        GR_PACKET_QUAD(gp, (*count)++)->xyz.flags = 0;

        GR_PACKET_QUAD(gp, *count)->uv.u = 0x1FF0;
        GR_PACKET_QUAD(gp, (*count)++)->uv.v = 0x1BF0;
        GR_PACKET_QUAD(gp, *count)->rgba.r = 0x80;
        GR_PACKET_QUAD(gp, *count)->rgba.g = 0x80;
        GR_PACKET_QUAD(gp, *count)->rgba.b = 0x80;
        GR_PACKET_QUAD(gp, (*count)++)->rgba.a = 0x80;
        GR_PACKET_QUAD(gp, *count)->xyz.x = dx * 16 + 0x9000;
        GR_PACKET_QUAD(gp, *count)->xyz.y = dy * 16 + 0x8E00;
        GR_PACKET_QUAD(gp, *count)->xyz.z = depth;
        GR_PACKET_QUAD(gp, (*count)++)->xyz.flags = 0;
    }
    grPacketSend(gp);
}

INCLUDE_ASM("asm/nonmatchings/ov01/entry_first_init", encEffVramCopy);
