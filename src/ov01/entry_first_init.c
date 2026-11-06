/*
 * OV01 original TU 7: 0x00a22628..0x00a24a98 (50 functions)
 */
#include "common.h"
#include "entry_first_init.h"
#include "main/act_1.h"
#include "main/xgl_packet.h"
#include "main/xgl_studio.h"
#include "ov01/battle_init.h"
#include "ov01/calc.h"

/* The player side's party slots (unitTblGet(0, ...) and plUnitTblGet). */
#define BATTLE_PARTY_SLOT_COUNT 3
/* Encounter grid: POINTS lattice points per side, CELLS quads between them. */
#define ENC_GRID_POINTS 65
#define ENC_GRID_CELLS 64
/* Fade countdown life at full cover; fadeInObj ends once it passes this. */
#define FADE_LIFE_FULL 0x80
/* Screen in pixels (512x448), its centre, and the colour bounds of encEffObj. */
#define ENC_SCREEN_WIDTH 0x200
#define ENC_SCREEN_HEIGHT 0x1C0
#define ENC_SCREEN_CENTER_X (ENC_SCREEN_WIDTH / 2)
#define ENC_SCREEN_CENTER_Y (ENC_SCREEN_HEIGHT / 2)
#define ENC_COLOR_FADE_MAX 0xF8
#define ENC_COLOR_FADE_MIN 0x80
#define ENC_COLOR_FADE_STEP 4

/* plUnitTblGet's own PlUnitEntry (src/ov01/battle_init.c): this TU reads
 * only partyId (+0x00), the slot-occupied flag entryPhase25's own search
 * and status-display match both test (`lh $x,0x0($y)`). */
typedef struct EntryPlUnit {
    short partyId;              /* +0x00 */
    unsigned char unmodeled_02[6];
} EntryPlUnit;

extern int printf(const char *format, ...);
extern void dataLeaderChk(void *table);
extern void dataEnemyMdlChk(void *table);
extern EntryPlUnit *plUnitTblGet(void);
extern ThinkMonsEntry *thinkMonsTblGet(int monsSetNo);
extern void calcPara2Org(void);
extern void resultProcInit(void);
extern int entryPhase80(void);
extern int entryPhase100(void);

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

/* Battle main loop: the sequencer pointer and its work area, pause flag, pad
 * record and GameLoopState flag word this TU reads (scaffold data, declared
 * here by their accurate types). */
extern const char entryFirstInitBattleStartText[];
extern const char entryFirstInitPAUSEText[];
extern const char entryFirstInitBattleEndText[];
static int pauseFlag;
static unsigned char battleSeqWk[32];
extern int sndMuStop(void);
extern void xglRenderClearOn(void);
extern void xglClockRead(XglClock *clock);
extern void objExec2(void);
extern void thinkProcessExec(void);
extern void objExec(void);
extern void ACT_update(void);
extern void sefExecEffect(void);
extern void xglFontDebugPrintf(int x, int y, const char *format, ...);
extern void ACT_pauseUpdate(void);
extern void nmlModelSendPauseSignal(int flag);
extern int battleDisp(void);
extern void GameSnapShotCheck(void);
extern void xglSleep(void);
extern void battleRetCodeSet(int code);
extern void battleProcEnd(void);
extern int entryPhase00(void);
extern int pauseProc(void);
extern int rnd(int max);

/* The render state's object-update hook slot (sRender word at +0x3C), set
 * for the battle and cleared by battleProcEnd. */
typedef struct EntryRenderState {
    unsigned char unmodeled_00[0x34];
    int unmodeled_34;           /* +0x34 cleared when the entry sequence starts */
    unsigned char unmodeled_38[0x3C - 0x38];
    void (*objExecHook)(void);  /* +0x3C */
    unsigned char unmodeled_40[0x5C - 0x40];
} EntryRenderState;
extern EntryRenderState sRender;

/* GameLoopState's flag word at +0x10 (GAME_FLAG_* bits of include/main/game.h). */
typedef struct EntryGameLoopState {
    unsigned char unmodeled_00[0x10];
    unsigned int flags;  /* +0x10 */
} EntryGameLoopState;
extern EntryGameLoopState GameLoopState;

/* PadPrefix (include/shared.h) fields this loop reads: half_2a holds the
 * buttons pressed this frame, half_30 the buttons held. */
extern PadPrefix PadData;

/* Held buttons that, with START pressed, abandon the battle. */
#define BATTLE_ABORT_HOLD_MASK (PAD_SELECT | PAD_L1 | PAD_R1)
/* GameLoopState flag bit that leaves the game loop (GAME_FLAG_EXIT). */
#define BATTLE_LOOP_EXIT_BIT 0x10000000u

void battleProc(void) {
    XglClock clock;
    int mask;

    printf(entryFirstInitBattleStartText);
    xglClockRead(&clock);
    clock.second = clock.second * 100;
    while (--clock.second != 0xFF) {
        rnd(0);
    }
    xglRenderClearOn();
    sRender.objExecHook = objExec2;
    pauseFlag = 0;
    battleSeq = entryPhase00;
    memset(battleSeqWk, 0, 0x20);
    for (;;) {
        mask = BATTLE_ABORT_HOLD_MASK;
        if ((PadData.half_30 & mask) == mask && (PadData.half_2a & PAD_START)) {
            GameLoopState.flags |= BATTLE_LOOP_EXIT_BIT;
            battleRetCodeSet(1);
            battleProcEnd();
            sndMuStop();
            break;
        }
        if (pauseProc() != 0) {
            if (battleSeq() == 0) {
                goto end;
            }
            thinkProcessExec();
            objExec();
            ACT_update();
            sefExecEffect();
        } else {
            xglFontDebugPrintf(0x6C, 0, entryFirstInitPAUSEText);
            ACT_pauseUpdate();
        }
        nmlModelSendPauseSignal(pauseFlag);
        battleDisp();
        GameSnapShotCheck();
        frameCnt++;
        xglSleep();
    }
end:
    battleSeq = entryPhase00;
    printf(entryFirstInitBattleEndText);
}

extern int cameraFlagGet(void);
extern void xglRenderClearOff(void);
extern void thinkSysInit(void);
extern void sefDestroyEffect(void);
extern void MBattleDestroy(void);
extern void MCamReleaseControl(void);
extern void JNT_offSmoothHair(void);
extern void xglCullingIgnoreOff(void);
extern void ACT_init(void);
extern void ACT_initSequence(void);
extern void xglSoundEffectStopBank(int bank);

void battleProcEnd(void) {
    int i;

    xglRenderClearOff();
    sRender.objExecHook = 0;
    pauseFlag = 0;
    thinkSysInit();
    sefDestroyEffect();
    MBattleDestroy();
    if (cameraFlagGet() != 0) {
        MCamReleaseControl();
    }
    JNT_offSmoothHair();
    xglCullingIgnoreOff();
    ACT_init();
    ACT_initSequence();
    for (i = 1; i < 15; i++) {
        xglSoundSendEffect(0, 0, i);
    }
    xglSoundEffectStopBank(0);
}

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

extern int calcBattleChk(BattleUnit *unit);
extern int unitLiveNumGet2(int side);
extern int unitTblGet(int side, BattleUnit ***table);

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
BattleUnit *selectPL(BattleUnit *excludeUnit)
{
    BattleUnit **table;
    BattleUnit *picked[BATTLE_PARTY_SLOT_COUNT];
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
            if (calcBattleChk(table[i]) != 0
                && calcUPGet(table[i])->charaId < SELECT_PL_CHARA_ID_LIMIT) {
                picked[pickedCount] = table[i];
                pickedCount++;
            }
        }
    }

    if (pickedCount == 0) {
        return 0;
    }

    idx = rnd(pickedCount - 1);
    return picked[idx];
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
const char entryFirstInitMapMonsSetThinkCfEncountCfEventText[64] = "** map=%d monsSet=%d/%d Think=%d cfEncount=%X cfEvent=%X**\n";
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
    printf(entryFirstInitMapMonsSetThinkCfEncountCfEventText, mapNo, monsSetNo, thinkMonsTblNum, thinkNo, cfEncount, cfEventGet());
    GameDefocusQuickSet(0, 1, 0xFFFFFF, 0);
    sndSysSePlay(0x25);
    battleSeq = entryPhase10;
    return 1;
}

extern int dataSndMuLoad(int kind);
static int loadNum;
extern void encEffObjInit(void);
extern int lastBossBtlChk(void);
extern int bossBtlChk(void);
extern int entryPhase15(void);
extern FadeObjTask *fadeIn(int speed, int delay);
static FadeObjTask *pFadeIn;

/*
 * The battle entry sequence's first phase: clears the render state's
 * +0x34 word, loads the leader, enemy model and sound data for the battle's
 * map, starts the fade-in and the encounter effect objects, then advances
 * to entryPhase15.
 */
int entryPhase10(void)
{
    int kind;

    sRender.unmodeled_34 = 0;
    dataLeaderChk(plUnitTblGet());
    dataEnemyMdlChk(thinkMonsTblGet(monsSetNoGet()));
    loadNum = dataCdSync();
    if (lastBossBtlChk() != 0) {
        kind = 2;
    } else {
        kind = bossBtlChk() != 0;
    }
    dataSndMuLoad(kind);
    pFadeIn = fadeIn(0x80, 3);
    encEffObjInit();
    battleSeq = entryPhase15;
    return 1;
}

extern int dataCdSync(void);
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
 * Tentative static definitions of the phase state below. Their position is
 * part of the data order of this TU: keep them where they are.
 */
static BattleUnit *pCreateUnit;

/*
 * camEv is a second camera-event request record: camEventExec's other
 * caller (src/ov01/battle_init.c's camEv) fills the same two fields,
 * a type code at +0x00 and the acting unit at +0x08, before the call.
 */
typedef struct EntryCamEvent {
    int type;                  /* +0x00 */
    unsigned char unmodeled_04[4];
    BattleUnit *unit;          /* +0x08 */
    unsigned char unmodeled_0c[4];
} EntryCamEvent;
static EntryCamEvent camEv;
static BattleUnit *pCvUnit;
static int unitIdx;

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
static int encEffCmd;

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

extern BattleUnit *unitPlCreate(s16 *pl);
extern void menuStatDispCreate(void);
extern int entryPhase30(void);
extern int entryPhase26(void);

int entryPhase25(void)
{
    EntryPlUnit *entries;
    BattleUnit *unit;
    int i;
    int lastOccupiedIdx;

    /* Advance unitIdx to the next occupied party slot, if any. */
    entries = plUnitTblGet();
    if (unitIdx < BATTLE_PARTY_SLOT_COUNT && entries[unitIdx].partyId == 0) {
        do {
            unitIdx++;
            if (unitIdx >= BATTLE_PARTY_SLOT_COUNT) {
                break;
            }
        } while (entries[unitIdx].partyId == 0);
    }

    if (unitIdx >= BATTLE_PARTY_SLOT_COUNT) {
        battleSeq = entryPhase30;
        return 1;
    }

    loadNum = dataCdSync();
    unit = unitPlCreate(&entries[unitIdx].partyId);
    pCreateUnit = unit;
    unitIdx++;

    lastOccupiedIdx = 0;
    for (i = 0; i < BATTLE_PARTY_SLOT_COUNT; i++) {
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
extern void dataUnitFileLoad2(BattleUnit *unit);
static BattleUnit *pCreateUnit;
static EntryCamEvent camEv;

int entryPhase26(void)
{
    if (dataCdSync() <= loadNum && sefCheckLoad() == 0) {
        dataUnitFileLoad2(pCreateUnit);
        camEv.unit = pCreateUnit;
        camEv.type = 0x17;
        camEventExec(&camEv);
        battleSeq = entryPhase25;
    }
    return 1;
}


extern void dataSndSeLoad(BattleUnit *unit, int seId);
extern int entryPhase31(void);

static BattleUnit *pCvUnit;

int entryPhase30(void) {
    BattleUnit *unit;

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
extern void dataSndSeLoad2(BattleUnit *unit);
extern void sndSeTransPlay(BattleUnit *unit, int seId, int volume);
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

extern BattleUnit *unitEnCreate(short *pl);
extern int entryPhase40(void);
extern int entryPhase36(void);

int entryPhase35(void)
{
    ThinkMonsEntry *entries;
    BattleUnit *unit;

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
    BattleUnit **table;
    BattleUnit **next;
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
            BattleUnit *unit = *next;

            next++;
            if (unit != 0 && (unit->actor->flags & ACTOR_FLAG_HIDDEN) != 0) {
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
extern void GameDefocusSet(int first, int second, int third);
int entryPhase51(void);

int entryPhase50(void)
{
    BattleUnit *unit;

    if (battleMain() == 0) {
        loadNum = dataCdSync();
        if (battleRetCodeGet() == 1) {
            unit = selectPL(lastUnitGet());
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
extern FadeObjTask *fadeOut(int speed, int delay);
extern int entryPhase70(void);

int entryPhase60(void) {
    if (battleAfter() == 0) {
        fadeOut(4, 2);
        battleSeq = entryPhase70;
    }
    return 1;
}

int entryPhase70(void) {
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

extern const char entryFirstInitExpMoneyText[];
extern const char entryFirstInitTpEpSpText[];
extern const char entryFirstInitGrpItemText[];
extern int entryPhase90(void);

/*
 * The monster-gain record monsGainPtrGet returns (battle_init.c keeps its
 * own opaque MonsGain): the experience, money and TP/EP/SP totals, then the
 * per-monster group and item lists and their count.
 */
typedef struct EntryMonsGain {
    int exp;                /* +0x00 */
    int tp;                 /* +0x04 */
    int ep;                 /* +0x08 */
    int sp;                 /* +0x0C */
    int money;              /* +0x10 */
    int grp[8];             /* +0x14 */
    int item[8];            /* +0x34 */
    int num;                /* +0x54 */
} EntryMonsGain;
extern EntryMonsGain *monsGainPtrGet(void);

/*
 * Prints the battle's gain totals and each monster's group and item, then
 * advances to entryPhase90.
 */
int entryPhase80(void)
{
    int i;
    EntryMonsGain *gain;

    i = 0;
    gain = monsGainPtrGet();
    printf(entryFirstInitExpMoneyText, gain->exp, gain->money);
    printf(entryFirstInitTpEpSpText, gain->tp, gain->ep, gain->sp);
    for (; i < gain->num; i++) {
        printf(entryFirstInitGrpItemText, i, gain->grp[i], gain->item[i]);
    }
    battleSeq = entryPhase90;
    return 1;
}

extern int resultProc(void);

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

extern void sefCreateEffect(EncounterEffectRequest *params);

void encountEffDisp(void) {
    EncounterEffectRequest params;

    memset(&params, 0, sizeof(params));
    if (cfEncountGet() & 1) {
        params.subId = 0xA42;
        sefCreateEffect(&params);
    }
    if (cfEncountGet() & 2) {
        params.subId = 0xA43;
        sefCreateEffect(&params);
    }
    if (cfEncountGet() & 4) {
        params.subId = 0xA44;
        sefCreateEffect(&params);
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

FadeObjTask *fadeIn(int speed, int delay) {
    FadeObjTask *self;

    self = (FadeObjTask *)objEntry2(fadeInObj, (void (*)(ObjectTask *))fadeObjDraw);
    self->delay = delay;
    self->speed = speed;
    self->life = 0;
    fadeFlag = 0;
    return self;
}

void fadeOutObj(FadeObjTask *self);


FadeObjTask *fadeOut(int speed, int delay) {
    FadeObjTask *self;

    self = (FadeObjTask *)objEntry2(fadeOutObj, (void (*)(ObjectTask *))fadeObjDraw);
    self->delay = delay;
    self->speed = speed;
    self->life = FADE_LIFE_FULL;
    fadeFlag = 0;
    return self;
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
    if ((unsigned int)self->life > FADE_LIFE_FULL) {
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

extern void grGpInit(GrPacket *packet);
extern void grPacketSend(GrPacket *packet);

/*
 * Sends the fade quad: the GIF tags for the register writes 0x0E, 0x42 (the
 * fade's life in the high word), 0x47 and 5 (XYZ2), then two XYZ vertices
 * (0x7000,0x7200) and (0x9000,0x8E00) at depth 0xFFFFFC.
 */
void fadeObjDraw(FadeObjTask *self)
{
    GrPacket packet;
    int life;

    life = self->life;
    grGpInit(&packet);

    GR_PACKET_QUAD(&packet, packet.count)->tag.register_id = SCE_GS_A_D;
    GR_PACKET_QUAD(&packet, packet.count++)->tag.control = 0x1000000000008002ULL;
    GR_PACKET_QUAD(&packet, packet.count)->tag.register_id = SCE_GS_ALPHA_1;
    GR_PACKET_QUAD(&packet, packet.count++)->tag.control = ((u64)life << 32) | 0xA9;
    GR_PACKET_QUAD(&packet, packet.count)->tag.register_id = SCE_GS_TEST_1;
    GR_PACKET_QUAD(&packet, packet.count++)->tag.control = 0x50000;
    GR_PACKET_QUAD(&packet, packet.count)->tag.register_id = SCE_GS_XYZ2;
    GR_PACKET_QUAD(&packet, packet.count++)->tag.control = 0x1023400000008002ULL;

    GR_PACKET_QUAD(&packet, packet.count)->xyz.x = 0x7000;
    GR_PACKET_QUAD(&packet, packet.count)->xyz.y = 0x7200;
    GR_PACKET_QUAD(&packet, packet.count)->xyz.z = 0xFFFFFC;
    GR_PACKET_QUAD(&packet, packet.count++)->xyz.flags = 0;

    GR_PACKET_QUAD(&packet, packet.count)->xyz.x = 0x9000;
    GR_PACKET_QUAD(&packet, packet.count)->xyz.y = 0x8E00;
    GR_PACKET_QUAD(&packet, packet.count)->xyz.z = 0xFFFFFC;
    GR_PACKET_QUAD(&packet, packet.count++)->xyz.flags = 0;

    grPacketSend(&packet);
}

extern void GS_REG_SET(void *packet, long long register_id, long long control);
extern int grFBAdrGet(int index);

/*
 * encEffObjDraw passes arg->env as a second argument; encEnvMake never reads
 * it (the original callee ignores $5).
 */
void encEnvMake(void *packet, int env) {
    int tbp0 = (grFBAdrGet(grBBIdxGet()) << 5) & 0xFFFF;

    GS_REG_SET(packet, SCE_GS_A_D, 0x1000000000008005LL);
    GS_REG_SET(packet, SCE_GS_TEXFLUSH, 1);
    GS_REG_SET(packet, SCE_GS_TEX0_1, tbp0 | 0x2000000664020000LL);
    GS_REG_SET(packet, SCE_GS_ALPHA_1, 0);
    GS_REG_SET(packet, SCE_GS_TEST_1, 0x30000);
    GS_REG_SET(packet, SCE_GS_TEX1_1, 0x60);
}

/* Encounter effect objects: one 0xC0-byte record per cell, at this address. */
#define ENC_EFF_OBJ_BASE ((void *)0x01DBF840)

/* The colour word of a FT4 primitive: its bytes are the red/green/blue fade
 * levels encEffObj clamps, the word is the default copied in encEffObjInit. */
typedef union EncEffColor {
    int word;                           /* +0x00 */
    struct {
        unsigned char r;                /* +0x00 */
        unsigned char g;                /* +0x01 */
        unsigned char b;                /* +0x02 */
        unsigned char a;                /* +0x03 */
    } ch;
} EncEffColor;

/* One transformed corner: x, y, z and a word that the FT4 packet ignores. */
typedef struct EncEffVertex {
    float x;                            /* +0x00 */
    float y;                            /* +0x04 */
    float z;                            /* +0x08 */
    int pad;                            /* +0x0C */
} EncEffVertex;

/* The FT4 primitive grPutFT4 reads from an object (+0x50, 0x70 bytes): its
 * first 0x40 bytes are the four transformed corners. */
typedef struct EncEffPrim {
    EncEffVertex vtx[4];                /* +0x00 */
    EncEffColor color;                  /* +0x40 */
    float u[4];                         /* +0x44 */
    float v[4];                         /* +0x54 */
    unsigned char unmodeled_64[0x0C];
} EncEffPrim;

/* One encounter effect object: its active flag, four corner indices into the
 * grid, four object-space corners, and the primitive. */
typedef struct EncEffObj {
    short active;                       /* +0x00 */
    short idx[4];                       /* +0x02 */
    short unmodeled_0A[3];
    Vector4 v[4];                       /* +0x10 */
    EncEffPrim prim;                    /* +0x50 */
} EncEffObj;

/* encEffObjDraw's argument: the environment word at +0x1C, the copy size at +0x20. */
typedef struct EncEffDrawArg {
    int unmodeled_00[7];
    int env;                            /* +0x1C */
    int size;                           /* +0x20 */
} EncEffDrawArg;
void encEffObjDraw(EncEffDrawArg *arg);

static int encObjCnt;
extern void encEffVramCopy(int size);
extern void encEffAppend(void);
extern void grOpenFT4(GrPacket *packet);
extern void grPutFT4(GrPacket *packet, void *prim, int flag);
extern void grCloseFT4(void);
extern XglPacket *xglPacketGetCurrent(void);
extern void sceVif1PkAddDirectDataN(XglPacket *packet, const void *data, int count);
extern void sceVif1PkTerminate(XglPacket *packet);
extern void encEnvMake(void *packet, int env);

/* One lattice point of the encounter grid: its position, a unit w, and the
 * distance class n that selects the corner index of each object. */
typedef struct EncGridPt {
    float x;                            /* +0x00 */
    float y;                            /* +0x04 */
    float z;                            /* +0x08 */
    float w;                            /* +0x0C */
    int n;                              /* +0x10 */
    int unmodeled_14[3];
} EncGridPt;

#define ENC_EFF_GRID_BASE ((void *)0x01D9E000)

static EncEffPrim ft4Base;
extern const char entryFirstInitFreeTopEndText[];
static float theta;
static float thetaInc;
extern void encEffObj(ObjectTask *task);


void effCalcMatrix(ObjectTask *task, Matrix4 matrix, int index);
extern void xglVectorMulMat(Vector4 *destination, Matrix4 matrix, const Vector4 *vector);


/*
 * This TU's own view of the shared transform helpers (src/ov01/gr_gp_init.c
 * declares and defines the same functions for its own calls, using its own
 * TU-local Matrix/FloatVector3/FloatVector4 types): here, the project's
 * shared Vector4/Matrix4 (include/shared.h) already match those layouts, so
 * no pointer cast is needed at any call site.
 */
extern void xglMatrixUnit(Matrix4 matrix);
extern void xglMatrixTrans(Matrix4 destination, Matrix4 source,
                           Vector4 *translation);
extern void xglMatrixScale(Matrix4 destination, Matrix4 source,
                           Vector4 *scale);
/*
 * grRotMatrix only reads angles->x/y/z (Y/X/Z Euler angles); the stack slot
 * this call site builds also carries a fourth word (the original .s stores
 * 1.0f at rotation+0xC, matching this TU's other two vectors' own unused .w
 * stores), so the argument stays a full Vector4 to keep that store, even
 * though grRotMatrix itself never reads angles->w.
 */
extern void grRotMatrix(Matrix4 destination, Matrix4 source,
                        Vector4 *angles);

/* theta (original ELF symbol, src/ov01/entry_first_init.c's own .bss). */
static float theta;

void encEffObjInit(void)
{
    EncGridPt (*grid)[ENC_GRID_POINTS];
    EncEffObj *obj;
    EncGridPt *g;
    FadeObjTask *task;
    int i;
    int j;
    int l;
    int t;
    int p;
    int u;
    int a;
    int b;
    int n;
    int m;
    float dx;
    float dy;
    float *wp;
    int *w40p;

    obj = (EncEffObj *)ENC_EFF_OBJ_BASE;
    grid = (EncGridPt (*)[ENC_GRID_POINTS])ENC_EFF_GRID_BASE;
    encObjCnt = 0;
    theta = 0.0f;
    thetaInc = 0.032724924f;
    encEffCmd = 0;

    for (i = 0; i < ENC_GRID_POINTS; i++) {
        for (j = 0; j < ENC_GRID_POINTS; j++) {
            g = &grid[j][i];
            g->y = (i * 8) - 256;
            g->z = 1.0f;
            g->w = 1.0f;
            g->x = (j * 8) - 256;
            if ((i == 0) || (i == ENC_GRID_CELLS)) {
                dy = 0.0f;
            } else {
                p = j - 32;
                if (p < 0) {
                    m = -j;
                    p = m + 32;
                }
                if (i >= 32) {
                    dy = -(p / 2);
                } else {
                    dy = p / 2;
                }
            }
            if ((j == 0) || (j == ENC_GRID_CELLS)) {
                dx = 0.0f;
            } else {
                u = i - 32;
                if (u < 0) {
                    n = -i;
                    u = n + 32;
                }
                if (j >= 32) {
                    dx = -(u / 2);
                } else {
                    dx = u / 2;
                }
            }
            g->y += dy;
            g->x += dx;
        }
    }

    for (i = 0; i < 33; i++) {
        for (j = 0; j < ENC_GRID_POINTS; j++) {
            if (!((j < i) || (j > (ENC_GRID_CELLS - i)))) {
                t = i;
            } else {
                t = (j <= 32) ? (j) : (ENC_GRID_CELLS - j);
            }
            grid[j][i].n = t;
            grid[j][ENC_GRID_CELLS - i].n = t;
        }
    }

    for (i = 0; i < ENC_GRID_CELLS; i++) {
        for (j = 0; j < ENC_GRID_CELLS; j++) {
            for (l = 0; l < 4; l++) {
                switch (l) {
                case 0:
                default:
                    a = 0;
                    b = 0;
                    break;
                case 1:
                    a = 1;
                    b = 0;
                    break;
                case 2:
                    a = 0;
                    b = 1;
                    break;
                case 3:
                    a = 1;
                    b = 1;
                    break;
                }

                g = &grid[j + a][i + b];
                wp = &obj->v[l].w;
                wp[-3] = g->x;
                wp[-2] = g->y;
                wp[-1] = g->z;
                wp[0] = 1.0f;
                obj->prim.u[l] = ft4Base.u[l] + ((float)(j * 8));
                obj->prim.v[l] = ft4Base.v[l] + ((float)(i * 7));
                obj->idx[l] = g->n;
            }

            obj->active = 1;
            w40p = &ft4Base.color.word;
            obj->prim.color.word = *w40p;
            encObjCnt++;
            obj++;
        }
    }

    printf(entryFirstInitFreeTopEndText, ENC_EFF_GRID_BASE, obj);
    task = (FadeObjTask *)objEntry2(encEffObj, (void (*)(ObjectTask *))encEffObjDraw);
    task->life = FADE_LIFE_FULL;
    task->speed = 8;
    task->delay = 0;
}

void encEffObj(ObjectTask *task)
{
    FadeObjTask *self = (FadeObjTask *)task;
    float mats[ENC_GRID_POINTS][4][4];
    Vector4 transformed;
    EncEffObj *obj;
    int i;
    int objIndex;
    int cx = ENC_SCREEN_CENTER_X;
    int cy = ENC_SCREEN_CENTER_Y;
    float *cornerZ;
    Vector4 *objVec;
    short *idxp;

    obj = (EncEffObj *)ENC_EFF_OBJ_BASE;
    switch (encEffCmd) {
    case 0:
        theta += thetaInc;
        if (theta > 6.2831855f) {
            theta -= 6.2831855f;
        }
        break;
    case 1:
        theta -= thetaInc;
        if (theta <= 0.0f) {
            encEffCmd = 2;
            theta = 0.0f;
            self->delay = 4;
        }
        break;
    default:
        if (--self->delay == 0) {
            objRemove(task);
            return;
        }
        break;
    }
    self->life = FADE_LIFE_FULL;
    if (encEffCmd != 0) {
        self->speed += 0x10;
    }
    for (i = 0; i < ENC_GRID_POINTS; i++) {
        effCalcMatrix(task, mats[i], i);
    }
    for (objIndex = 0; objIndex < encObjCnt; objIndex++, obj++) {
        cornerZ = &obj->prim.vtx[0].z;
        objVec = obj->v;
        idxp = obj->idx;
        for (i = 3; i >= 0; i--) {
            xglVectorMulMat(&transformed, mats[*idxp++], objVec++);
            cornerZ[-2] = (int)(cx + transformed.x);
            cornerZ[-1] = (int)(cy + transformed.y);
            cornerZ[0] = 16777213.0f;
            cornerZ += 4;
        }
        switch (encEffCmd) {
        case 0:
            obj->prim.color.ch.r += ENC_COLOR_FADE_STEP;
            obj->prim.color.ch.g += ENC_COLOR_FADE_STEP;
            obj->prim.color.ch.b += ENC_COLOR_FADE_STEP;
            if (obj->prim.color.ch.r > ENC_COLOR_FADE_MAX) obj->prim.color.ch.r = ENC_COLOR_FADE_MAX;
            if (obj->prim.color.ch.g > ENC_COLOR_FADE_MAX) obj->prim.color.ch.g = ENC_COLOR_FADE_MAX;
            if (obj->prim.color.ch.b > ENC_COLOR_FADE_MAX) obj->prim.color.ch.b = ENC_COLOR_FADE_MAX;
            break;
        case 1:
            obj->prim.color.ch.r -= ENC_COLOR_FADE_STEP;
            obj->prim.color.ch.g -= ENC_COLOR_FADE_STEP;
            obj->prim.color.ch.b -= ENC_COLOR_FADE_STEP;
            if (obj->prim.color.ch.r < ENC_COLOR_FADE_MIN) obj->prim.color.ch.r = ENC_COLOR_FADE_MIN;
            if (obj->prim.color.ch.g < ENC_COLOR_FADE_MIN) obj->prim.color.ch.g = ENC_COLOR_FADE_MIN;
            if (obj->prim.color.ch.b < ENC_COLOR_FADE_MIN) obj->prim.color.ch.b = ENC_COLOR_FADE_MIN;
            break;
        }
    }
}

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

/*
 * Draws the encounter effect objects that are active: their FT4 primitives go
 * into a GIF packet, flushed every 50 objects, then the grid's VRAM copy and
 * the packet's append run when the effect is fading.
 */
void encEffObjDraw(EncEffDrawArg *arg)
{
    GrPacket packet;
    XglPacket *pk;
    EncEffObj *obj;
    int i;

    obj = (EncEffObj *)ENC_EFF_OBJ_BASE;
    if (encEffCmd != 0) {
        encEffVramCopy(arg->size);
    }
    pk = xglPacketGetCurrent();
    grGpInit(&packet);
    sceVif1PkCnt(pk, 0);
    sceVif1PkAddCode(pk, 0x11000000);
    sceVif1PkOpenDirectHLCode(pk, 0);
    encEnvMake(&packet, arg->env);
    for (i = 0; i < encObjCnt; i++, obj++) {
        if (obj->active == 1) {
            grOpenFT4(&packet);
            grPutFT4(&packet, &obj->prim, 0);
            grCloseFT4();
            if (i % 50 == 0) {
                sceVif1PkAddDirectDataN(pk, packet.data, packet.count);
                packet.count = 0;
            }
        }
    }
    sceVif1PkAddDirectDataN(pk, packet.data, packet.count);
    sceVif1PkCloseDirectHLCode(pk);
    sceVif1PkTerminate(pk);
    encEffAppend();
}



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
    GS_REG_SET(&packet, SCE_GS_TEXFLUSH, 1);
    GS_REG_SET(&packet, SCE_GS_TEX0_1, tbp0 | 0x2000000664020000LL);
    GS_REG_SET(&packet, SCE_GS_ALPHA_1, 0x44);
    GS_REG_SET(&packet, SCE_GS_TEST_1, 0x30000);
    GS_REG_SET(&packet, SCE_GS_TEX1_1, 0);

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

/* The two frame-buffer base halfwords (indexed by the display and the back
 * buffer index) and the VRAM copy's parameter block grVramCopy reads. */
typedef struct EncFrameBaseTable {
    unsigned short base[3];
} EncFrameBaseTable;

typedef struct EncVramCopyParams {
    unsigned short source_base;        /* +0x00 */
    unsigned short unmodeled_02;
    unsigned short destination_base;   /* +0x04 */
    unsigned short unmodeled_06;
    unsigned short source_x;           /* +0x08 */
    unsigned short source_y;           /* +0x0A */
    unsigned short destination_x;      /* +0x0C */
    unsigned short destination_y;      /* +0x0E */
    unsigned short width;              /* +0x10 */
    unsigned short height;             /* +0x12 */
} EncVramCopyParams;

extern const EncFrameBaseTable encFrameBaseTable;
extern const EncVramCopyParams encVramCopyParams;
extern void grVramCopy(EncVramCopyParams *params);
extern void grGsRegSet(GrPacket *packet, unsigned long long register_id, unsigned long long value);

/*
 * Copies the centred size x size window (clamped to 0x200 x 0x1C0) from the
 * display buffer to the back buffer, then sends a GS register write
 * (0x3F = 1). The table copies are whole-struct reads of the TU's constant
 * template, so their unnamed halves come along unchanged.
 */
void encEffVramCopy(int size)
{
    GrPacket packet;
    EncFrameBaseTable bases = encFrameBaseTable;
    EncVramCopyParams params = encVramCopyParams;
    int dispIdx;
    int unused;
    int bb;
    int w;
    int h;

    grDBIdxGet(&dispIdx, &unused);
    bb = grBBIdxGet();
    w = size;
    if (size > ENC_SCREEN_WIDTH) {
        w = ENC_SCREEN_WIDTH;
    }
    h = size;
    if (size > ENC_SCREEN_HEIGHT) {
        h = ENC_SCREEN_HEIGHT;
    }
    params.source_base = bases.base[unused];
    params.destination_base = bases.base[bb];
    params.destination_x = ENC_SCREEN_CENTER_X - w / 2;
    params.destination_y = ENC_SCREEN_CENTER_Y - h / 2;
    params.source_x = ENC_SCREEN_CENTER_X - w / 2;
    params.source_y = ENC_SCREEN_CENTER_Y - h / 2;
    params.width = w;
    params.height = h;
    grVramCopy(&params);
    grGpInit(&packet);
    grGsRegSet(&packet, SCE_GS_TEXFLUSH, 1);
    grPacketSend(&packet);
}



static int pauseFlag = 0;

static EncEffPrim ft4Base = { .vtx = { { .x = 0.0f, .y = 0.0f, .z = 0.0f, .pad = 1065353216 }, { .x = 0.0f, .y = 0.0f, .z = 0.0f, .pad = 1065353216 }, { .x = 0.0f, .y = 0.0f, .z = 0.0f, .pad = 1065353216 }, { .x = 0.0f, .y = 0.0f, .z = 0.0f, .pad = 1065353216 } }, .color = { .word = -2139062144 }, .u = { 0.0f, 8.0f, 0.0f, 8.0f }, .v = { 0.0f, 0.0f, 7.0f, 7.0f }, .unmodeled_64 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } };

const char entryFirstInitBattleStartText[24] = "** Battle Start! **\n";

const char entryFirstInitPAUSEText[16] = "\013\r\000PAUSE";

const char entryFirstInitBattleEndText[24] = "** Battle End! **\n";

const char entryFirstInitExpMoneyText[24] = "** Exp = %d Money = %d\n";

const char entryFirstInitTpEpSpText[32] = "** Tp = %d Ep = %d Sp = %d\n";

const char entryFirstInitGrpItemText[24] = "** %d: grp=%d item=%d\n";

const char entryFirstInitFreeTopEndText[24] = "** free top=%X end=%X\n";

const EncFrameBaseTable encFrameBaseTable = { { 0xe00, 0x1c00, 0x2a00 } };

const EncVramCopyParams encVramCopyParams = { 0, 0x8, 0, 0x8, 0, 0, 0, 0, 0x200, 0x1c0 };

static unsigned char battleSeqWk[32];

static int encEffCmd;

static int encObjCnt;

static float theta;

static float thetaInc;
