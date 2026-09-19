/*
 * OV01 original TU 3: 0x00a07770..0x00a10bb8 (150 functions)
 */
#include "common.h"
#include "shared.h"
#include "battle_init.h"
#include "ov01/calc.h"

#define CURSOR_STATE_OFF 0x10
#define STATE_FLAGS_OFF 0x00
#define STATE_UNK70_OFF 0x70
#define STATE_UNK74_OFF 0x74

extern int escapeFlag;
extern const char D_00A44080[];
extern int printf(const char *format, ...);

const char **dataItmNameGet(int index);
const char **dataWepNameGet(int index);
const char **dataBltNameGet(int index);
const char **dataAccNameGet(int index);

extern void battleMsgPut(int x, int y, int msgId, int frames);

/*
 * The battle actor object a unit's actor/weapon slots point to. src/main/
 * chr.h, src/main/near_dir.h, src/main/set_motion.h and src/ov01/unit_cmd.h
 * all restate this same 0x70-byte head verbatim (a TU-local header cannot be
 * included from another TU); this TU adds only the float its own code
 * touches, at +0xC0 (transWepIn/transWepOut set it to 0.0/1.0, the same
 * "transparency" member src/ov01/unit_cmd.h restates at that offset).
 */
typedef struct Actor {
    u32 flags;
    void (*update)(struct Actor *actor);
    void (*draw)(struct Actor *actor);
    u32 quadword_alignment_gap;
    Vector4 position;
    Vector4 previous_position;
    Vector4 velocity;
    Vector4 acceleration;
    Vector4 rotation;
    Vector4 scale;
    unsigned char unmodeled_70[0xC0 - 0x70];
    float transparency; /* +0xC0 */
} Actor;

#define WEAPON_HIDDEN_BIT 0x8

/*
 * The unit record scenarioMtd, transWepIn/transWepOut and spWepDispOn
 * (0x00a0e550, still INCLUDE_ASM here) all share: spWepDispOn receives the
 * same pointer scenarioMtd receives, dereferences it to reach this record,
 * then indexes its +0x1C table by (weapon-slot & 3) and reads +0x14, exactly
 * as transWepIn/transWepOut do on their own argument and scenarioMtd does on
 * *unitHandle. Bytes between the evidenced members are untouched here.
 */
typedef struct BattleUnit BattleUnit;
struct BattleUnit {
    unsigned char unmodeled_00[0x14];
    Actor *actor;      /* +0x14 */
    unsigned char unmodeled_18[0x1C - 0x18];
    Actor *weapon[4];  /* +0x1C */
};

/*
 * The scenario-battle exec record the scenarioBatExecPhase* handlers work on
 * (scenarioBatExecPhase20 passes the same pointer to etehrCVGet and to
 * scenarioMtd, and every unit call there loads its first word). Only +0x00,
 * the acting unit, is read by this TU's C; spWepDispOn (still INCLUDE_ASM)
 * also reads +0x04, +0x08 and +0x0C, which are not modeled here.
 */
typedef struct ScenarioBatExec {
    BattleUnit *unit; /* +0x00 */
} ScenarioBatExec;

int dataMtdRead(BattleUnit *unit, int trigger);
int dataDefWpnGet(BattleUnit *unit);

/*
 * Show (transWepIn) or hide (transWepOut) the weapon actor in the unit's
 * slot `slot & 3` by installing its fade draw callback.
 */
void transWepIn(BattleUnit *unit, int slot);
void transWepOut(BattleUnit *unit, int slot);

void spWepDispOn(ScenarioBatExec *exec, int weapon);
void spWepDispOff(ScenarioBatExec *exec, int weapon);
void hairWindOn(BattleUnit *unit);
void hairWindOff(BattleUnit *unit);
extern void ACT_setHand(Actor *actor, int hand);

extern void transWepInDraw(Actor *actor);
extern void transWepOutDraw(Actor *actor);

/*
 * The battle AI script identifier (data still asm-owned: the extern keeps
 * its splat name until the owning data TU recovers it, docs/naming.md).
 */
extern int D_00A57C80;

/*
 * The unit currently executing a battle action / the unit that most
 * recently acted (data still asm-owned; original ELF symbol names).
 */
extern BattleUnit *pActUnit;
extern BattleUnit *pLastUnit;
BattleUnit **tgtUnitAdrGet(void);

/*
 * manWorkGet's return type. Only the two fields menuKeyGuideObjDraw (src/
 * ov01/menu.c, ov01/tu009) consumes are evidenced there: unitTask at +0x10
 * and controlPhase at +0xc. This TU's own code only takes the record's
 * address (data still asm-owned; original ELF symbol name).
 */
typedef struct ManWork {
    unsigned char unmodeled_00[0xc];
    int controlPhase;     /* +0xc */
    ObjectTask *unitTask; /* +0x10 */
} ManWork;
extern ManWork manWk;

/*
 * Opaque battle-control record. batCtrlGet is its only reference in the
 * mapped functions so far; no dereference evidences any member layout
 * (data still asm-owned; original ELF symbol name).
 */
typedef struct BatCtrl BatCtrl;
extern BatCtrl batCtrl;

/*
 * Scenario-battle phase-dispatch record. scenarioBatInit and every
 * scenarioBatExecPhase* handler (still INCLUDE_ASM here) call
 * scenarioPtrFuncSet to install the next phase's handler and reset its
 * frame counter; only the two words that function writes are evidenced:
 *   +0x10 nextPhase: the phase callback the caller passes in $a0;
 *   +0x14 phaseTimer: zeroed on every phase transition.
 * scenarioBatInit itself also writes +0x00, +0x04, +0x08 and +0x0c, which
 * are not modeled here (data still asm-owned; original ELF symbol name).
 */
/*
 * scenarioBatNext (0x00a0ba30) evidences three more of this record's
 * words: it adds source->stride into index, clamps index to count once
 * their sum reaches it, and reports whether the clamp fired. +0x08 stays
 * unmodeled (scenarioBatInit writes it, but no claimed function here reads
 * it).
 */
/*
 * Opaque object ScenarioBatState.source points to; scenarioBatNext is the
 * only reference to it in this TU and touches just the one evidenced word.
 */
typedef struct ScenarioStep {
    unsigned char unmodeled_00[0x10];
    int stride; /* +0x10: added into ScenarioBatState.index by scenarioBatNext */
} ScenarioStep;
typedef void (*ScenarioPhaseFunc)(void);
typedef struct ScenarioBatState {
    int count;                    /* +0x00: index is clamped to this */
    int index;                    /* +0x04: advanced by source->stride each call */
    unsigned char unmodeled_08[0x0C - 0x08];
    ScenarioStep *source;         /* +0x0C */
    ScenarioPhaseFunc nextPhase; /* +0x10 */
    int phaseTimer;              /* +0x14 */
} ScenarioBatState;
extern ScenarioBatState scenarioPtr;

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", battleInit);

void battleCtrl(void);
void battleExec(void);
void battleEndChk(void);

/*
 * Set unconditionally at the start of every battleMain tick (data still
 * asm-owned; original ELF symbol name); no other write is evidenced in this TU.
 */
extern int batMainFlag;

void battleMain(void)
{
    /* Separate load keeps this store in its own register. */
    int flag = 1;
    batMainFlag = flag;
    battleCtrl();
    battleExec();
    battleEndChk();
}

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", battleAfter);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", battleCtrl);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", battleCtrlConfusion);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", battleCtrlJunk);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", actStockInit);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", actStockMaxGet);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", actStockMake);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", actStockPick);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", actStockRemove);

/*
 * The current action-stock count actStockInit/actStockMake/actStockPick/
 * actStockRemove (still INCLUDE_ASM here) read and write through the
 * address this returns (data still asm-owned; original ELF symbol name).
 */
extern int actStock;

int *actStockGet(void)
{
    return &actStock;
}

/*
 * The battle help page index, cycled 0..2 (data still asm-owned, splat
 * name kept).
 */
extern int helpFlag;

int battleCtrlHelp(void)
{
    helpFlag = (helpFlag + 1) % 3;
    return 1;
}

int battleCtrlHelpGet(void)
{
    return helpFlag;
}

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", battleCtrlBoostPl);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", battleCtrlBoostEn);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", battleCtrlMan);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", manInput);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", camEventTurnStartSet);

/*
 * The two command-list buffers cmdListSet/cmdListChange/unitCmdListSet/
 * cmdListCansel (still INCLUDE_ASM here) operate on (data still asm-owned,
 * original ELF symbol names); cmdListPtrGet is the only place both addresses are
 * taken together.
 */
extern int fifoCmd;
extern int cmdList;

void cmdListPtrGet(int **list, int **altList)
{
    *list = &fifoCmd;
    *altList = &cmdList;
}

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", cmdListSet);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", cmdListChange);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", convKey2unit);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", unitCmdListSet);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", cmdListCansel);

/*
 * Menu map-height toggle (ov01/tu009 src/ov01/menu.c, not yet published;
 * forward-declared here until that TU's header exists).
 */
extern void menuMapHSet(int enable);

/*
 * Enemy AI attack-selection entry point (ov01/tu006 src/ov01/cmd.c, still
 * asm there; forward-declared here until that TU publishes its header).
 * cmd.c's thinkUnitAtkExec passes its argument straight to calcUPGet, whose
 * published prototype (src/ov01/unit_cmd.h) takes ObjectTask *.
 */
extern void thinkUnitAtkExec(ObjectTask *unit);

int battleCtrlThink(ObjectTask *unit)
{
    menuMapHSet(1);
    thinkUnitAtkExec(unit);
    return 0;
}

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", battleExec);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", battleExecNorm);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", battleExecDead);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", stat1Proc);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", stat2Proc);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", keyCmdGet);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", keyCmdApGet);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", keyCmdChk);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", keyCmd2tid);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", keyCmd2wno);

/*
 * Buffered battle-input queue (data still asm-owned; original ELF symbol names):
 * keyBuffIdx is the current insertion index into the keyBuff queue.
 */
extern int keyBuffIdx;
extern int keyBuff[8];

void keyBuffClear(void)
{
    memset(keyBuff, 0, sizeof(keyBuff));
    keyBuffIdx = 0;
}

int keyBuffIdxGet(void)
{
    return keyBuffIdx;
}

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", keyBuffSet);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", keyBuffChk);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", busyUnitChk);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", actUnitTblMake);

/*
 * Defined in ov01/tu004 src/ov01/calc.c; not yet published there.
 */
extern int rnd(int max);

BattleUnit *tgtUnitPick(BattleUnit **table, int count)
{
    if (count > 0) {
        return table[rnd(count - 1)];
    }
    return 0;
}

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", battleEndChk);

/*
 * The battle result code consumed by phase transitions and shutdown
 * processing (data still asm-owned; original ELF symbol name).
 */
extern int batRetCode;

int battleRetCodeGet(void)
{
    return batRetCode;
}

void battleRetCodeSet(int retCode)
{
    batRetCode = retCode;
}

int thinkNoGet(void)
{
    return D_00A57C80;
}

void thinkNoSet(int thinkNo)
{
    D_00A57C80 = thinkNo;
}

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", plUnitTblGet);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", plUnitTblSet);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", monsSetNoGet);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", monsSetNoSet);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", mapNoGet);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", mapNoSet);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", cfEncountGet);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", cfEncountSet);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", cfEventGet);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", cfEventSet);

BattleUnit *actUnitGet(void)
{
    return pActUnit;
}

BattleUnit *lastUnitGet(void)
{
    return pLastUnit;
}

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", tgtUnitAdrGet);

BattleUnit *tgtUnitGet(void)
{
    return *tgtUnitAdrGet();
}

void tgtUnitSet(BattleUnit *unit)
{
    *tgtUnitAdrGet() = unit;
}

ManWork *manWorkGet(void)
{
    return &manWk;
}

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", menuChk);

BatCtrl *batCtrlGet(void)
{
    return &batCtrl;
}

void scenarioPtrFuncSet(ScenarioPhaseFunc func)
{
    scenarioPtr.nextPhase = func;
    scenarioPtr.phaseTimer = 0;
}

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", scenarioBatInit);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", scenarioSrcSet);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", scenarioDstSet);

/*
 * The scenario source index (data still asm-owned, splat name kept); a
 * different object from the ScenarioBatState record above.
 */
extern int D_00A57B64;

int scenarioSrcGet(void)
{
    return D_00A57B64;
}

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", scenarioDstGet);

int scenarioBatNext(void)
{
    int index;

    index = scenarioPtr.index + scenarioPtr.source->stride;
    scenarioPtr.index = index;
    if (index >= scenarioPtr.count) {
        scenarioPtr.index = scenarioPtr.count;
        return 1;
    }
    return 0;
}

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", scenarioCam);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", scenarioBatExec);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", scenarioBatDataSync);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", etehrCVGet);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", hairWindOn);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", hairWindOff);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", scenarioBatExecPhase10);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", scenarioBatExecPhase20);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", scenarioBatExecPhase25);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", scenarioBatExecPhase30);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", scenarioBatExecPhase40);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", scenarioBatExecPhase50);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", scenarioBatExecMain);

/*
 * The stolen-item state nusumeSet/nusumeGet share (data still asm-owned;
 * original ELF symbol name).
 */
extern int nusumuFlag;

void nusumeSet(int value)
{
    nusumuFlag = value;
}

int nusumeGet(void)
{
    return nusumuFlag;
}

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", flowSpecProcInit);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", flowSpecProc);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", syoukanFadeOut);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", syoukanFadeIn);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", syoukan1Sub);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", syoukan2Sub);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", specSyoukanProc);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", specHensinProc);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", specAnalyzeProc);

void specEscapeProc(void)
{
    escapeFlag = 1;
}

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", specInoriProc);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", specNusumuProc);

const char *itemNameGet(int group, int index)
{
    const char *name;

    name = 0;
    switch (group) {
    case 0:
        name = *dataItmNameGet(index);
        break;
    case 1:
        name = *dataWepNameGet(index);
        break;
    case 2:
        name = *dataBltNameGet(index);
        break;
    case 3:
        name = *dataAccNameGet(index);
        break;
    default:
        printf(D_00A44080, group, index);
        break;
    }
    return name;
}

void battleMsg(int msgId)
{
    battleMsgPut(0x10, 0x15C, msgId, 0x1E);
}

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", scenarioBatExecDst);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", decoEffCall);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", decoEffCallSub);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", scenarioEffCall);

void scenarioMtd(ScenarioBatExec *exec)
{
    if (dataMtdRead(exec->unit, 2) == 1) {
        spWepDispOn(exec, dataDefWpnGet(exec->unit));
    }
    if (dataMtdRead(exec->unit, 3) == 1) {
        spWepDispOff(exec, dataDefWpnGet(exec->unit));
    }
    if (dataMtdRead(exec->unit, 4) == 1) {
        ACT_setHand(exec->unit->actor, 0);
    }
    if (dataMtdRead(exec->unit, 5) == 1) {
        ACT_setHand(exec->unit->actor, 1);
    }
    if (dataMtdRead(exec->unit, 6) == 1) {
        spWepDispOn(exec, 1);
    }
    if (dataMtdRead(exec->unit, 7) == 1) {
        spWepDispOff(exec, 1);
    }
    if (dataMtdRead(exec->unit, 8) == 1) {
        hairWindOn(exec->unit);
    }
    if (dataMtdRead(exec->unit, 9) == 1) {
        hairWindOff(exec->unit);
    }
}

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", spWepDispOn);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", spWepDispOnSub);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", spWepDispOff);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", spWepDispOffSub);

void transWepIn(BattleUnit *unit, int slot)
{
    Actor *weapon;

    weapon = unit->weapon[slot & 3];
    if (weapon != 0) {
        weapon->flags &= ~WEAPON_HIDDEN_BIT;
        weapon->draw = transWepInDraw;
        weapon->transparency = 0.0f;
    }
}

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", transWepInDraw);

void transWepOut(BattleUnit *unit, int slot)
{
    Actor *weapon;

    weapon = unit->weapon[slot & 3];
    if (weapon != 0) {
        weapon->draw = transWepOutDraw;
        weapon->transparency = 1.0f;
    }
}

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", transWepOutDraw);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", scenarioSrcEndChk);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", scenarioBatEnd);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", counterProc);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", dmgValPut);

/*
 * Battle monster-gain tallies (original ELF symbol name; only its total size is
 * evidenced, by monsGainInit's own memset call).
 */
typedef struct MonsGain {
    unsigned char unmodeled_00[0x58];
} MonsGain;
extern MonsGain monsGain;

MonsGain *monsGainPtrGet(void)
{
    return &monsGain;
}

void monsGainInit(void)
{
    memset(&monsGain, 0, sizeof(MonsGain));
}

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", monsGainSet);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", itemGrpChk);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", statEffProc);

/*
 * calcUPGet's return type comes from its definer, ov01/tu004 calc.c
 * (published in include/ov01/calc.h); calcUPGet itself is still asm there.
 */
extern CalcUnitParam *calcUPGet(ObjectTask *unit);

#define UNIT_WORK_TABLE_COUNT 8
#define UNIT_WORK_STATUS_BLOCKED 0x100u
/* CalcUnitParam's own UnitWork* table base and per-slot pointer offset
 * (see the comment below); named to match this file's own CURSOR_STATE_OFF
 * convention instead of leaving the offsets as bare literals. */
#define UNIT_WORK_TABLE_BASE_OFF 0x150
#define UNIT_WORK_PTR_OFF 0xC

/*
 * Each battle unit's own large parameter block, reached only through the
 * per-unit table below; nothing else in this TU dereferences it.
 */
typedef struct UnitWork {
    unsigned char unmodeled_00[0xA8C];
    unsigned int flags; /* +0xA8C: UNIT_WORK_STATUS_BLOCKED suppresses
                          * status-effect ticks for this unit */
} UnitWork;

/*
 * calcUPGet's own CalcUnitParam (ov01/tu004 calc.c, still asm there, not
 * yet published to add this member -- reported as a shared-header need)
 * carries an 8-entry UnitWork* table at +0x15C. statEffOn/statEffOff each
 * re-read calcUPGet(unit)'s table slot for the current offset twice (once
 * to test it, once to use it) rather than caching it in a local: the
 * original calls calcUPGet a second time inside the body instead of
 * keeping the first call's result. The offset walks the table forward
 * (+4 each pass) while a separate trip counter counts the 8 passes down.
 */
void statEffOn(ObjectTask *unit)
{
    int offset;
    int i;

    offset = UNIT_WORK_TABLE_BASE_OFF;
    for (i = UNIT_WORK_TABLE_COUNT - 1; i >= 0; i--) {
        if (*(UnitWork **)((char *)calcUPGet(unit) + offset + UNIT_WORK_PTR_OFF) != 0) {
            (*(UnitWork **)((char *)calcUPGet(unit) + offset + UNIT_WORK_PTR_OFF))->flags
                &= ~UNIT_WORK_STATUS_BLOCKED;
        }
        offset += 4;
    }
}

void statEffOff(ObjectTask *unit)
{
    int offset;
    int i;

    offset = UNIT_WORK_TABLE_BASE_OFF;
    for (i = UNIT_WORK_TABLE_COUNT - 1; i >= 0; i--) {
        if (*(UnitWork **)((char *)calcUPGet(unit) + offset + UNIT_WORK_PTR_OFF) != 0) {
            (*(UnitWork **)((char *)calcUPGet(unit) + offset + UNIT_WORK_PTR_OFF))->flags
                |= UNIT_WORK_STATUS_BLOCKED;
        }
        offset += 4;
    }
}

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", objEnCurCreate);

/*
 * objRemove is defined by ov01/tu001 obj.c; menu.c already redeclares it
 * the same way to call it from a different TU (src/ov01/obj.h is that
 * TU-local header and is not includable here).
 */
extern void objRemove(ObjectTask *task);
extern ObjectTask *pTaskEnCur;

void objCurRemove(void)
{
    if (pTaskEnCur != 0) {
        objRemove(pTaskEnCur);
        pTaskEnCur = 0;
    }
}

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", objCur);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", objCurDraw);

/*
 * Evidenced word offsets on the opaque state/cursor handles. Each offset is
 * observed directly in the original body; the labels stay neutral because
 * the historical member names are unresolved:
 *   cursor + 0x10 : state pointer (lw s1,16(a0) in the callee delay slot);
 *   state  + 0x00 : flags word (read, OR mask 0x2, written back);
 *   state  + 0x70 : word zeroed on entry (sw zero,112(s1));
 *   state  + 0x74 : word zeroed on entry (sw zero,116(s1)).
 * Accesses are written inline (no helper calls) so the candidate emits only
 * the single curCmdSet extent; the macros keep the offsets reviewable.
 */

void curCmdSet(CursorObject *cursor, const CursorCommand *command)
{
    CursorCommandState *state;
    CursorCommand *tail;
    unsigned int flags;

    state = *(CursorCommandState **)((char *)cursor + CURSOR_STATE_OFF);
    tail = objCmdTailGet(cursor);
    *(unsigned int *)((char *)state + STATE_UNK70_OFF) = 0;
    *(unsigned int *)((char *)state + STATE_UNK74_OFF) = 0;
    *tail = *command;
    if (tail->selector != 0) {
        flags = *(unsigned int *)((char *)state + STATE_FLAGS_OFF);
        *(unsigned int *)((char *)state + STATE_FLAGS_OFF) = flags | 2;
    }
}

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", curSel);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", curGrpSel);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", curMove2Pos);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", curEnProcInit);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", curEnProc);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", curPosProcInit);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", curPosProc);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", curTgtProcInit);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", curTgtProc);

/*
 * The battle cursor's selection-group record (data still asm-owned, splat
 * name kept); curSelGrpSet is the only known writer of the evidenced word.
 */
typedef struct CurSelGrp {
    unsigned char unmodeled_00[0x58];
    u32 group; /* +0x58: written 0 or 1 by curSelGrpSet */
} CurSelGrp;
extern CurSelGrp curMove;

/*
 * Written -1 whenever curSelGrpSet derives the group from flags instead of
 * receiving it directly (data still asm-owned; original ELF symbol name); no other
 * reference is evidenced in this TU.
 */
extern int oldCurGrp;

void curSelGrpSet(u32 group, int flags)
{
    CurSelGrp *state = &curMove;

    if (group < 2U) {
        state->group = group;
        return;
    }
    if (flags & 8) {
        state->group = 0;
    } else if (flags & 0x40) {
        state->group = 0;
    } else {
        state->group = 1;
    }
    oldCurGrp = -1;
}

/*
 * The current selection group (data still asm-owned, splat name kept); a
 * different object from CurSelGrp above, whose writer is not in this TU.
 */
extern int D_00A57C48;

int curSelGrpGet(void)
{
    return D_00A57C48;
}

void objDispPlCur(void)
{
}

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", objDispEnCur);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", objDispCur);

void objDispCurSubPl(void)
{
}

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", objDispCurSubEn);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", curPutSub);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", curTexTrans);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", curEnvMake);
