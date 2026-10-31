/*
 * OV01 original TU 3: 0x00a07770..0x00a10bb8 (150 functions)
 */
#include "common.h"
#include "shared.h"
#include "battle_init.h"
#include "ov01/calc.h"
#include "main/party.h"

/* Define this before TU-local INCLUDE_ASM output so cc1 emits its storage
 * outside their #APP blocks. */
const char s_battleCtrlBoostEnFormat[32] = "** BOOST EN=%d %d%% (%d)\n";

#define CURSOR_STATE_OFF 0x10
#define STATE_FLAGS_OFF 0x00
#define STATE_UNK70_OFF 0x70
#define STATE_UNK74_OFF 0x74

/*
 * The enemy AI's own boost chance percentage: a signed byte at CalcUnitParam
 * +0x20 (`lb` at 0x00a08808 in battleCtrlBoostEn), inside calc.h's
 * unmodeled_1e span; calc.h is owned by a different TU and is not completed
 * here. The boost level both functions log is calc.h's bcValue (+0x3C).
 */
#define CALC_PARAM_BOOST_CHANCE_OFF 0x20

static int escapeFlag;
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
 * included from another TU); this TU adds only the words its own code
 * touches: the float at +0xC0 (transWepIn/transWepOut set it to 0.0/1.0, the
 * same "transparency" member src/ov01/unit_cmd.h restates at that offset)
 * and the flags word at +0x6F0 (hairWindOn ORs in HAIR_WIND_BIT there for
 * every other unit's actor and hairWindOff clears it again).
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
    unsigned char unmodeled_c4[0x6F0 - 0xC4];
    u32 windFlags; /* +0x6F0 */
} Actor;

#define HAIR_WIND_BIT 0x100000

#define WEAPON_HIDDEN_BIT 0x8

/*
 * The unit record scenarioMtd, transWepIn/transWepOut and spWepDispOn
 * (0x00a0e550) all share: spWepDispOn receives the
 * same pointer scenarioMtd receives, dereferences it to reach this record,
 * then indexes its +0x1C table by (weapon-slot & 3) and reads +0x14, exactly
 * as transWepIn/transWepOut do on their own argument and scenarioMtd does on
 * *unitHandle. Bytes between the evidenced members are untouched here.
 */
typedef struct BattleUnit BattleUnit;
struct BattleUnit {
    unsigned char unmodeled_00[0x10];
    Actor *work;       /* +0x10 */
    Actor *actor;      /* +0x14 */
    unsigned char unmodeled_18[0x1C - 0x18];
    Actor *weapon[4];  /* +0x1C */
};

/*
 * BattleUnit's own ObjectTask.work slot at +0x10 (inside unmodeled_00 above,
 * not completed here): camEventTurnStartSet reads it as the same Actor
 * pointer src/ov01/unit_cmd.c and src/ov01/snd.c already read through
 * ObjectTask.work on their own unit records, testing bit 0x40 of
 * Actor.flags (the enemy bit src/ov01/unit_cmd.h names ACTOR_FLAG_ENEMY).
 *
 * BattleUnit is the battle-side view of the unit's ObjectTask, the type
 * calcUPGet takes. scenarioCam and spWepDispOn pass ScenarioBatExec.unit to
 * calcUPGet unconverted (the first word of the exec record goes straight
 * into the argument register: `jal calcUPGet` at 0x00a0ba90 with `lw $4,
 * 0x0($16)` in its delay slot, and at 0x00a0e5d8 and 0x00a0e664 in
 * spWepDispOn), so they write the same (ObjectTask *) conversion that
 * battleCtrlBoostPl already uses on the unitTblGet() entries. The view
 * stays partial: only work (+0x10), actor (+0x14) and weapon[] (+0x1C) are
 * evidenced, and no other BattleUnit member is claimed here.
 */
#define BATTLE_UNIT_WORK_OFF 0x10

/*
 * The camera-event request record camEventExec (defined elsewhere, still
 * INCLUDE_ASM there) consumes: camEventTurnStartSet writes type 6, the
 * acting/target units and, in param, whether the acting unit is an enemy.
 * battleCtrlMan writes type 8 with only unit/target, leaving param at
 * whatever the record already held. Data still asm-owned; the original ELF
 * local symbol is "camEv", not yet in config/symbols/ov01.txt, so the splat
 * placeholder name is kept here.
 */
typedef struct CamEvent {
    int type;           /* +0x00 */
    int param;           /* +0x04 */
    BattleUnit *unit;     /* +0x08 */
    BattleUnit *target;   /* +0x0C */
} CamEvent;

/*
 * One argument: camEventExec reads only $4, and scenarioCam's item branch
 * calls it without loading $5; the former (event, unit) spelling added
 * `lw $5,0x0($16)` before that call (the original's `j camEventExec` at
 * 0x00a0bb10 has no $5 set-up). entry_first_init.c declares it with one
 * argument as well.
 */
extern void camEventExec(CamEvent *event);

/*
 * The scenario-battle exec record the scenarioBatExecPhase* handlers work on
 * (scenarioBatExecPhase20 passes the same pointer to etehrCVGet and to
 * scenarioMtd, and every unit call there loads its first word). scenarioCam
 * reads the action type (`lw $2,0x4($16)` at 0x00a0baa8, compared with 3),
 * the technique id (`lw $4,0x8($16)` at 0x00a0bab4, passed to dataTecGet;
 * `lw $2,0x8($16)` at 0x00a0bae8) and the item id (`lw $4,0xC($16)` at
 * 0x00a0bad4, passed to dataItmGet). spWepDispOn reads the same type
 * (`lw $2,0x4($16)` at 0x00a0e568, compared with 1), the technique id
 * (`lw $4,0x8($16)` at 0x00a0e578 and 0x00a0e5cc; reloaded at 0x00a0e5d4
 * and compared with 8 and 0xB) and, for a technique action, +0x0C as a
 * weapon slot (`lw $5,0xC($16)` at 0x00a0e5b4 and 0x00a0e66c).
 */
typedef struct ScenarioBatExec {
    BattleUnit *unit; /* +0x00 */
    int type;         /* +0x04: 3 for an item action */
    int tecId;        /* +0x08 */
    int slotOrItemId; /* +0x0C: the item id for an item action (type 3),
                       * the weapon slot for a technique (type 1) */
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
void spWepDispOnSub(BattleUnit *unit, int slot, int fade);
void hairWindOn(BattleUnit *unit);
void hairWindOff(BattleUnit *unit);
extern void ACT_setHand(Actor *actor, int hand);
extern Actor *ACT_setArms(Actor *actor, Actor *parent, unsigned int accessoryId, int flags);
/* Defined in data_unit_org_get.c; spWepDispOn only tests the returned
 * special-weapon entry against zero, so the entry type stays incomplete. */
extern struct WepSpEntry *dataTid2WepSp(int tecId);

extern void transWepInDraw(Actor *actor);
extern void transWepOutDraw(Actor *actor);

/*
 * The battle AI script identifier (data still asm-owned: the extern keeps
 * its splat name until the owning data TU recovers it, docs/naming.md).
 */
static int D_00A57C80;

/*
 * The unit currently executing a battle action / the unit that most
 * recently acted (data still asm-owned; original ELF symbol names).
 */
static BattleUnit *pLastUnit;
BattleUnit **tgtUnitAdrGet(void);
BattleUnit *tgtUnitGet(void);

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
    unsigned char unmodeled_14[4]; /* original named span continues to +0x18 */
} ManWork;
static ManWork manWk;

/*
 * Opaque battle-control record. batCtrlGet is its only reference in the
 * mapped functions so far; no dereference evidences any member layout
 * (data still asm-owned; original ELF symbol name).
 */
typedef struct BatCtrl {
    unsigned char unmodeled_00[4];
} BatCtrl;
static BatCtrl batCtrl;

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
/*
 * Two-parameter spelling of the phase callback: a compiler-specific matching
 * workaround for scenarioBatExec's indirect call, not standards-correct
 * prototyping. That call (`jalr $3` at 0x00a0bc1c) sets $a0 to
 * scenarioSrcGet(0)'s return and $a1 to scenarioDstGet(0)'s record (`daddu
 * $5,$16,$0` at 0x00a0bc08, `daddu $4,$2,$0` at 0x00a0bc10); only this
 * parameter list makes the compiler emit that set-up. scenarioBatExecPhase10
 * reads `source` as the ScenarioBatExec record (`lw $4,0x0($17)` at
 * 0x00a0bee4, `lw $3,0x4($17)` at 0x00a0bef4) and hands `dest` to
 * scenarioCam as its target list (`daddu $5,$18,$0` at 0x00a0bef0).
 */
typedef struct ScenarioDstEntry ScenarioDstEntry;
typedef void (*ScenarioPhaseFunc)(int source, ScenarioDstEntry *dest);
/*
 * One destination record. scenarioDstSet appends a whole 0x28-byte
 * record built by unitJobTypeAtk/unitCmdItemSub (src/ov01/unit_cmd.c) at
 * dest[count]; scenarioDstGet hands out &dest[index + offset]. Only the words
 * this TU's callers of scenarioDstGet touch are modeled:
 *   +0x00 unit: scenarioBatEnd reads unit->work (+0x10) and clears bit 0x4000
 *         in its first word; scenarioBatExecMain passes the word at unit+0x14
 *         to sefIsSeSignal.
 *   +0x08: battleExec copies it into D_00A57B78.
 *   +0x14: scenarioBatExecPhase30 zeroes it after scenarioEffCall.
 */
struct ScenarioDstEntry {
    ObjectTask *unit;                 /* +0x00 */
    unsigned char unmodeled_04[4];
    int word08;                       /* +0x08 */
    unsigned char unmodeled_0c[8];
    int word14;                       /* +0x14 */
    unsigned char unmodeled_18[0x10];
};

/*
 * The record limit scenarioDstSet and scenarioDstGet both test (`slti 0xD`).
 * scenarioDst itself spans 0x1E0 bytes (0x00a57978..0x00a57b58), twelve
 * records; a thirteenth accepted record would land on scenarioPtr.
 */
#define SCENARIO_DST_LIMIT 13

/*
 * scenarioBatInit also writes +0x08, the destination buffer scenarioBatInit
 * pairs with source: both are set from the fixed scenarioSrc/scenarioDst
 * arrays.
 */
typedef struct ScenarioBatState {
    int count;                    /* +0x00: index is clamped to this */
    int index;                    /* +0x04: advanced by source->stride each call */
    ScenarioDstEntry *dest;       /* +0x08: scenarioDstSet appends at dest[count] */
    ScenarioStep *source;         /* +0x0C */
    ScenarioPhaseFunc nextPhase; /* +0x10 */
    int phaseTimer;              /* +0x14 */
} ScenarioBatState;

/*
 * Private battle state, in original BSS address order. Keeping these local
 * definitions together lets the guarded overlay linker map each exact object
 * to its original storage span.
 */
static ObjectTask *pTaskEnCur;
static BattleUnit *pActUnit;
static int oldCurGrp;
static int keyBuffIdx;
static int keyBuff[8];
static int batMainFlag;
static ScenarioStep scenarioSrc[8];
static ScenarioDstEntry scenarioDst[12];
static ScenarioBatState scenarioPtr;
static int helpFlag;
static CamEvent D_00A57B80;
static int batRetCode;

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", battleInit);

void battleCtrl(void);
void battleExec(void);
void battleEndChk(void);

/*
 * Set unconditionally at the start of every battleMain tick (data still
 * asm-owned; original ELF symbol name); no other write is evidenced in this TU.
 */
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

/*
 * The action-stock record (0x24 bytes, the span the published `int *` of
 * actStockGet indexes). actStockRemove compares the unit against the boost
 * slots (`lw $3,0x10($7)` at 0x00a085a8, `lw $2,0x14($7)` at 0x00a085c0) and
 * the first `count` (`lw $2,0x18($7)` at 0x00a085c8) stock words from +0x00.
 * The actor words are pointers: only with pointer-typed loads does the
 * scheduler hoist the +0x10 load above the `*index = -1` int store, as the
 * original does (`sw` in the first `bne` delay slot, 0x00a085b4).
 * calc.c (CalcActStock, src/ov01/calc.h) and menu.c (actStockGet()[4]/[5])
 * read the same storage through views of their own.
 */
typedef struct ActStock {
    ObjectTask *stock[4]; /* +0x00 */
    ObjectTask *boost[2]; /* +0x10: calcBoost's per-mode boosting actor */
    int count;            /* +0x18 */
    unsigned char unmodeled_1c[8];
} ActStock;
static ActStock actStock;

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", actStockInit);

extern int calcBattleChk(ObjectTask *unit);

/*
 * The action-stock limit: half the units on side 2 still able to fight,
 * rounded up, capped at 4.
 */
int actStockMaxGet(void)
{
    BattleUnit **table;
    int count;
    int i;
    int fighters;

    fighters = 0;
    count = unitTblGet(2, &table);
    for (i = 0; i < count; i++) {
        if (table[i] != 0 && calcBattleChk((ObjectTask *)table[i])) {
            fighters++;
        }
    }
    if (fighters > 0) {
        if (fighters * 5 % 10 >= 5) {
            fighters = fighters / 2 + 1;
        } else {
            fighters = fighters / 2;
        }
    }
    if (fighters > 4) {
        fighters = 4;
    }
    return fighters;
}

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", actStockMake);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", actStockPick);

/*
 * Drop `unit` from the boost slots and the stock; *index receives its stock
 * position, or -1. Nonzero when any slot held it.
 */
int actStockRemove(ObjectTask *unit, int *index)
{
    int removed = 0;
    int i;

    *index = -1;
    if (actStock.boost[0] == unit) {
        actStock.boost[0] = 0;
        removed = 1;
    }
    if (actStock.boost[1] == unit) {
        actStock.boost[1] = 0;
        removed = 1;
    }
    for (i = 0; i < actStock.count; i++) {
        if (actStock.stock[i] == unit) {
            actStock.stock[i] = 0;
            removed = 1;
            *index = i;
            break;
        }
    }
    return removed;
}

/*
 * The action-stock record actStockInit/actStockMake/actStockPick (still
 * INCLUDE_ASM here) and actStockRemove work on; menu.c and calc.c declare
 * this as returning `int *`, so that published type is kept.
 */
int *actStockGet(void)
{
    return (int *)&actStock;
}

/*
 * The battle help page index, cycled 0..2 (data still asm-owned, splat
 * name kept).
 */
int battleCtrlHelp(void)
{
    helpFlag = (helpFlag + 1) % 3;
    return 1;
}

int battleCtrlHelpGet(void)
{
    return helpFlag;
}

extern CalcUnitParam *calcUPGet(ObjectTask *unit);
extern int unitTblGet(int side, BattleUnit ***table);
extern int calcBoostChk(BattleUnit *unit, int mode);
extern int calcBoost(BattleUnit *unit, int mode);

/* "** BOOST PL=%d (%d)\n" (data still asm-owned; original ELF symbol name). */
extern const char D_00A43D78[];

extern int dataPadRead(int padNo);
extern void menuBoostDisp(int enabled);

int battleCtrlBoostPl(void)
{
    BattleUnit **table;
    int selection;
    int held;

    held = dataPadRead(1);
    if (dataPadRead(0) & 0xA) {
        menuBoostDisp(1);
        selection = -1;
        if (held & 0x80) {
            selection = 0;
        } else if (held & 0x10) {
            selection = 1;
        } else if (held & 0x20) {
            selection = 2;
        }

        unitTblGet(0, &table);
        if (selection != -1 && table[selection] != 0 &&
            calcBoostChk(table[selection], 1) != 0) {
            printf(D_00A43D78, calcUPGet((ObjectTask *)table[selection])->charaId,
                   calcUPGet((ObjectTask *)table[selection])->bcValue);
            calcBoost(table[selection], 1);
            return 1;
        }
        return 0;
    }

    menuBoostDisp(0);
    return 0;
}

extern int rnd(int max);

int battleCtrlBoostEn(void)
{
    BattleUnit **table;
    int count;
    int chance;
    int i;

    count = unitTblGet(1, &table);
    for (i = 0; i < count; i++) {
        if (table[i] != 0) {
            chance = ((signed char *)calcUPGet((ObjectTask *)table[i]))[CALC_PARAM_BOOST_CHANCE_OFF];
            if (calcBoostChk(table[i], 1) != 0 && rnd(100) < chance) {
                printf(s_battleCtrlBoostEnFormat, calcUPGet((ObjectTask *)table[i])->charaId, chance,
                       calcUPGet((ObjectTask *)table[i])->bcValue);
                calcBoost(table[i], 1);
                return 1;
            }
        }
    }
    return 0;
}

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", battleCtrlMan);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", manInput);

void camEventTurnStartSet(void)
{
    D_00A57B80.type = 6;
    D_00A57B80.unit = pActUnit;
    D_00A57B80.target = tgtUnitGet();
    D_00A57B80.param = (*(int *)*(void **)((char *)pActUnit + BATTLE_UNIT_WORK_OFF) >> 6) & 1;
    camEventExec(&D_00A57B80);
}

/*
 * The two command-list buffers cmdListSet/cmdListChange/unitCmdListSet/
 * cmdListCansel (still INCLUDE_ASM here) operate on (data still asm-owned,
 * original ELF symbol names); cmdListPtrGet is the only place both addresses are
 * taken together.
 */
static int fifoCmd[6];
static int cmdList[0xC0];

void cmdListPtrGet(int **list, int **altList)
{
    *list = fifoCmd;
    *altList = cmdList;
}

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", cmdListSet);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", cmdListChange);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", convKey2unit);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", unitCmdListSet);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", cmdListCansel);

/* Menu map-height toggle, defined by ov01/tu009 (src/ov01/menu.c). */
extern void menuMapHSet(int enable);

/* Enemy AI attack-selection entry point, defined by ov01/tu006.
 * thinkUnitAtkExec forwards this argument to calcUPGet as ObjectTask *. */
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
void keyBuffClear(void)
{
    memset(keyBuff, 0, sizeof(keyBuff));
    keyBuffIdx = 0;
}

int keyBuffIdxGet(void)
{
    return keyBuffIdx;
}

/*
 * Sentinel-terminated (0) command-key tables, one per calcUPGet() flags bit
 * 0x40 (the enemy-side bit objEnCurCreate/objCurDraw already read): data
 * still asm-owned, original ELF symbol names.
 */
extern const unsigned short keyItemTblAgws[];
extern const unsigned short keyItemTblCha[];

/* "** keyBuffSet: buff full\n"-style diagnostic; data still asm-owned. */
extern const char D_00A43F60[];

/*
 * Looks key up in the unit's own command-key table (enemy or character,
 * chosen by calcUPGet()'s flags bit 0x40) and, if it is a recognised key,
 * appends it to the shared keyBuff queue. Returns whether it both matched a
 * table entry and fit in the queue.
 */
int keyBuffSet(ObjectTask *unit, unsigned short key)
{
    const unsigned short *table;
    int idx;
    unsigned short entry;
    int found;

    found = 0;
    if (calcUPGet(unit)->flags & 0x40) {
        table = keyItemTblAgws;
    } else {
        table = keyItemTblCha;
    }

    idx = 0;
    entry = table[idx];
    while (entry != 0) {
        if (key == entry) {
            found = 1;
            break;
        }
        idx++;
        entry = table[idx];
    }

    if (!found) {
        return 0;
    }

    if (keyBuffIdx < 8) {
        keyBuff[keyBuffIdx] = key;
        keyBuffIdx++;
        return 1;
    }
    printf(D_00A43F60);
    return 0;
}

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", keyBuffChk);

int busyUnitChk(int side)
{
    BattleUnit **units;
    int count = unitTblGet(side, &units);
    int busyCount = 0;
    int i;

    for (i = 0; i < count; i++) {
        BattleUnit *unit = units[i];

        if (unit != 0 && (unit->work->flags & 2) != 0) {
            busyCount++;
        }
    }
    return busyCount;
}

/*
 * Collects the units of one side that are still taking part in the battle
 * and returns how many there are. The test is on the flags word of the
 * unit's own actor (the same word transWepIn and camEventTurnStartSet
 * read): bit 0x8 must be set and bit 0x80 clear.
 * A null actUnits only counts them; otherwise the collected units are
 * stored in it in table order.
 */
int actUnitTblMake(int side, BattleUnit **actUnits)
{
    BattleUnit **table;
    BattleUnit **next;
    BattleUnit **out;
    BattleUnit *unit;
    u32 flags;
    int remaining;
    int count;
    int made;

    count = unitTblGet(side, &table);
    made = 0;
    if (count > 0) {
        next = table;
        remaining = count;
        out = actUnits;
        do {
            unit = *next;
            next++;
            if (unit != 0) {
                flags = unit->work->flags;
                if (!(flags & 0x80)) {
                    if (flags & 8) {
                        if (actUnits != 0) {
                            *out = unit;
                        }
                        out++;
                        made++;
                    }
                }
            }
            remaining--;
        } while (remaining != 0);
    }
    return made;
}

/* Defined by ov01/tu004 (src/ov01/calc.c). */


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

/*
 * The party's per-slot attack assignment (src/main/party.h
 * PartyAttackPosition: that TU-local header cannot be included from here).
 * Only the 4-byte stride and the two fields plUnitTblGet copies are modeled.
 */
typedef struct PartyAttackSlot {
    unsigned short partyId;      /* +0x0 */
    signed char attackPosition;  /* +0x2 */
} PartyAttackSlot;

/*
 * plTbl (data still asm-owned; original ELF symbol name): three 8-byte
 * entries. plUnitTblGet and plUnitTblSet write only the first two fields of
 * each; nothing in this TU's C touches the remaining four bytes.
 */
typedef struct PlUnitEntry {
    short partyId;         /* +0x0 */
    short attackPosition;  /* +0x2 */
    unsigned char unmodeled_04[4];
} PlUnitEntry;
static PlUnitEntry plTbl[3];

PlUnitEntry *plUnitTblGet(void) {
    PartyAttackSlot *slot;
    PlUnitEntry *entry;
    int i;

    slot = (PartyAttackSlot *)(PartyDataGet() + 0x30);
    entry = plTbl;
    for (i = 0; i < 3; i++) {
        entry->partyId = (short)slot->partyId;
        entry->attackPosition = (short)slot->attackPosition;
        slot++;
        entry++;
    }
    return plTbl;
}

/*
 * The party record's attack-slot table (src/main/party.h PartyDataPrefix:
 * attack_slots at +0x30), viewed with the offset inside the struct so that
 * index*4 + 0x30 is added before the record base (`addiu $4,$4,0x30` at
 * 0x00a0b6b8, `addu $2,$2,$4` at 0x00a0b6c0); partyId is stored first (`sb`
 * at 0x00a0b6cc ahead of the `sh`).
 */
typedef struct PartyAttackView {
    unsigned char unmodeled_00[0x30];
    PartyAttackSlot attackSlots[3]; /* +0x30 */
} PartyAttackView;

/* Assign a party member and its attack position to slot `index`, both in the
 * party record and in plTbl. */
void plUnitTblSet(int index, int partyId, int attackPosition)
{
    PartyAttackView *party;

    party = (PartyAttackView *)PartyDataGet();
    party->attackSlots[index].partyId = partyId;
    party->attackSlots[index].attackPosition = attackPosition;
    plTbl[index].partyId = partyId;
    plTbl[index].attackPosition = attackPosition;
}

/*
 * GameLoopState (config/symbols/main.txt, data still asm-owned) is a
 * 0x2A030-byte record several TUs each model only where they touch it (for
 * example src/main/script.h's GameLoopState+0x29F40 event byte); the active
 * monster-set number monsSetNoGet/monsSetNoSet share lives at +0x29F90 as a
 * signed halfword. The rest of the record is not recovered here.
 */
typedef struct GameLoopMonsSetView {
    unsigned char unmodeled_00[0x29F90];
    short monsSetNo; /* +0x29F90 */
} GameLoopMonsSetView;
extern GameLoopMonsSetView GameLoopState;

int monsSetNoGet(void)
{
    return GameLoopState.monsSetNo;
}

void monsSetNoSet(int monsSetNo)
{
    GameLoopState.monsSetNo = monsSetNo;
}

/*
 * GameLoopState also carries the current map number at +0x29F92, directly
 * after the monster-set number above; the published GameLoopMonsSetView
 * cannot be extended to reach it, so this TU models it through its own
 * prefix view of the same object; the rest of the record is not recovered
 * here.
 */
typedef struct GameLoopMapNoPrefix {
    unsigned char unmodeled_00[0x29F92];
    short mapNo; /* +0x29F92 */
} GameLoopMapNoPrefix;

int mapNoGet(void)
{
    GameLoopMapNoPrefix *mapNoView = (GameLoopMapNoPrefix *)&GameLoopState;
    return mapNoView->mapNo;
}

void mapNoSet(int mapNo)
{
    GameLoopMapNoPrefix *mapNoView = (GameLoopMapNoPrefix *)&GameLoopState;
    mapNoView->mapNo = mapNo;
}

/*
 * GameLoopState also carries the encounter/event configuration bytes at
 * +0x29F96 and +0x29F97 (lbu/sb against 0x00362616/0x00362617, base
 * 0x338680 + 0x29F96/+0x29F97, the same base the monster-set view above
 * uses). The published GameLoopMonsSetView above cannot be extended to
 * reach these bytes, so this TU models them through their own prefix view
 * of the same object; the rest of the record is not recovered here.
 */
typedef struct GameLoopCfConfigPrefix {
    unsigned char unmodeled_00[0x29F96];
    u8 cfEncount; /* +0x29F96 */
    u8 cfEvent;   /* +0x29F97 */
} GameLoopCfConfigPrefix;

u8 cfEncountGet(void)
{
    GameLoopCfConfigPrefix *cfConfig = (GameLoopCfConfigPrefix *)&GameLoopState;
    return cfConfig->cfEncount;
}

void cfEncountSet(u8 cfEncount)
{
    GameLoopCfConfigPrefix *cfConfig = (GameLoopCfConfigPrefix *)&GameLoopState;
    cfConfig->cfEncount = cfEncount;
}

u8 cfEventGet(void)
{
    GameLoopCfConfigPrefix *cfConfig = (GameLoopCfConfigPrefix *)&GameLoopState;
    return cfConfig->cfEvent;
}

void cfEventSet(u8 cfEvent)
{
    GameLoopCfConfigPrefix *cfConfig = (GameLoopCfConfigPrefix *)&GameLoopState;
    cfConfig->cfEvent = cfEvent;
}

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

extern int menuOpenChkBat(void);
extern int menuFlag;

int menuChk(void)
{
    if (menuOpenChkBat() != 0) {
        return 1;
    }
    return menuFlag == 1;
}

BatCtrl *batCtrlGet(void)
{
    return &batCtrl;
}

void scenarioPtrFuncSet(ScenarioPhaseFunc func)
{
    scenarioPtr.nextPhase = func;
    scenarioPtr.phaseTimer = 0;
}

void scenarioBatExecPhase10(int source, ScenarioDstEntry *dest);
extern void sndInit2(void);
void scenarioBatInit(void)
{
    sndInit2();
    scenarioPtr.source = (ScenarioStep *)scenarioSrc;
    scenarioPtr.count = 0;
    scenarioPtr.index = 0;
    scenarioPtr.dest = scenarioDst;
    scenarioPtrFuncSet(scenarioBatExecPhase10);
}

int scenarioSrcSet(const ScenarioStep *source)
{
    ScenarioStep *destination = scenarioPtr.source;

    *destination = *source;
    return 1;
}

/* Append a copy of `entry` at dest[count]; 0 once the record limit is reached. */
int scenarioDstSet(const ScenarioDstEntry *entry)
{
    if (scenarioPtr.count >= SCENARIO_DST_LIMIT) {
        return 0;
    }
    scenarioPtr.dest[scenarioPtr.count] = *entry;
    scenarioPtr.count++;
    return 1;
}

/*
 * The scenario source index (data still asm-owned, splat name kept); a
 * different object from the ScenarioBatState record above.
 */
int scenarioSrcGet(int unused)
{
    return (int)scenarioPtr.source;
}

/* The destination record `offset` steps past the current index, or NULL. */
ScenarioDstEntry *scenarioDstGet(int offset)
{
    int index = scenarioPtr.index + offset;

    if (index >= SCENARIO_DST_LIMIT || index >= scenarioPtr.count) {
        return 0;
    }
    return &scenarioPtr.dest[index];
}

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

/*
 * dataTecGet's return, as far as this TU reads it: a byte at +0x00 whose
 * bit 0x10 selects between the selector-0x2C and selector-0xF commands in
 * syoukan1Sub and ends scenarioCam (`andi $3,$3,0x10` at 0x00a0bb40), and
 * whose low two bits are scenarioCam's range class (`andi $3,$3,0x3` at
 * 0x00a0bb58); +0x11 is the signed byte data_unit_org_get.c's TecRecord
 * names `kind` (`lb $3,0x11($2)` at 0x00a0bb20). data_unit_org_get.c,
 * dataTecGet's own TU, owns the layout.
 */
typedef struct TecFlagView {
    unsigned char flags;                      /* +0x00 */
    unsigned char unmodeled_01[0x02 - 0x01];
    u16 procFlags; /* +0x02: with bit 0x20 (TEC_PROC_FLOW_SPEC) set,
                    * scenarioSrcEndChk returns the caller's result (`lhu
                    * $3,0x2($2)`/`andi $3,$3,0x20` at 0x00a0ea54); calc.c's
                    * TecKindView names the same u16 flags */
    unsigned char unmodeled_04[0x11 - 0x04];
    signed char kind;                         /* +0x11 */
    unsigned char unmodeled_12[0x16 - 0x12];
    short effectNo; /* +0x16: scenarioBatExecPhase50 waits on this effect
                     * (`lh $4,0x16($2)` at 0x00a0cb8c into sefIsFinishEffect) */
} TecFlagView;

extern TecFlagView *dataTecGet(int tecId);

/*
 * dataItmGet's return as far as scenarioCam reads it: the item's
 * target-selection flags byte (`lbu $3,0x8($2)` at 0x00a0bae0); calc.c's
 * ItmTrgView names the same member.
 */
typedef struct ItmTrgView {
    unsigned char unmodeled_00[0x08];
    u8 trgFlags; /* +0x08 */
    unsigned char unmodeled_09[0x10 - 0x09];
    short effectNo; /* +0x10: the item's effect (`lh $4,0x10($2)` at
                     * 0x00a0cb98 into sefIsFinishEffect) */
} ItmTrgView;

extern ItmTrgView *dataItmGet(int itmId);

/*
 * Queues the camera event for a scenario-battle action: items (type 3) use
 * event 0x14 with the item's target-selection flags (bit 0x10 added when
 * tecId is set); techniques of kind 2 or 6, or with flag bit 0x10, get
 * none, the rest use event 2 or 3 by their range class. The CalcUnitParam
 * bit tested first (`lhu $3,0x1C($2)`/`andi $3,$3,0x40` at
 * 0x00a0ba98..0x00a0ba9c) is calc.h's CALC_UNIT_FLAG_IS_AGWS, which the
 * generated include/ov01/calc.h does not carry yet.
 */
void scenarioCam(ScenarioBatExec *exec, BattleUnit **targets)
{
    int tecKind;
    int range;

    if (calcUPGet((ObjectTask *)exec->unit)->flags & 0x40) {
        return;
    }
    if (exec->type == 3) {
        D_00A57B80.type = 0x14;
        D_00A57B80.unit = exec->unit;
        D_00A57B80.target = *targets;
        D_00A57B80.param = dataItmGet(exec->slotOrItemId)->trgFlags;
        if (exec->tecId != 0) {
            D_00A57B80.param |= 0x10;
        }
        camEventExec(&D_00A57B80);
        return;
    }
    tecKind = dataTecGet(exec->tecId)->kind;
    if (tecKind == 2 || tecKind == 6) {
        return;
    }
    if (dataTecGet(exec->tecId)->flags & 0x10) {
        return;
    }
    range = dataTecGet(exec->tecId)->flags & 0x3;
    if (range == 0) {
        D_00A57B80.type = 2;
        D_00A57B80.unit = exec->unit;
        D_00A57B80.param = exec->tecId;
        D_00A57B80.target = *targets;
        camEventExec(&D_00A57B80);
    } else if (range == 1) {
        D_00A57B80.type = 3;
        D_00A57B80.unit = exec->unit;
        D_00A57B80.param = exec->tecId;
        D_00A57B80.target = *targets;
        camEventExec(&D_00A57B80);
    }
}

void scenarioBatEnd(void);

/*
 * Run the installed scenario phase on the current destination record; 1 once
 * no record is left (scenarioBatEnd has run), 0 while records remain.
 */
int scenarioBatExec(void)
{
    ScenarioDstEntry *dest = scenarioDstGet(0);
    int source = scenarioSrcGet(0);

    if (dest) {
        scenarioPtr.nextPhase(source, dest);
        if (scenarioDstGet(0)) {
            return 0;
        }
    }
    scenarioBatEnd();
    return 1;
}

int sefCheckLoad(void);
int dataCdSync(void);
void dataSndSeLoad2(int unit);

int scenarioBatDataSync(int *sourceIndex)
{
    if (sefCheckLoad() != 0) {
        return 0;
    }
    if (dataCdSync() > 0) {
        return 0;
    }
    dataSndSeLoad2(*sourceIndex);
    return 1;
}

/*
 * The ether voice for a scenario action: 4 by default, 5 or 6 for the
 * listed techniques of characters 3, 6 and 7.
 */
int etehrCVGet(ScenarioBatExec *exec)
{
    int voice = 4;

    switch (calcUPGet((ObjectTask *)exec->unit)->charaId) {
    case 3:
        if (exec->tecId >= 0x236 && exec->tecId <= 0x239) {
            voice = 5;
        }
        break;
    case 6:
        if (exec->tecId == 0x254) {
            voice = 5;
        } else if (exec->tecId == 0x257) {
            voice = 6;
        }
        break;
    case 7:
        if (exec->tecId >= 0x262 && exec->tecId <= 0x263) {
            voice = 5;
        }
        break;
    }
    return voice;
}

extern void EXM_SetDirectionalWind(Vector4 *direction);
extern void EXM_SetShakePower(float power);
extern void EXM_SetShakeTime(float time);

/*
 * Blows a fixed directional wind and camera shake, then sets HAIR_WIND_BIT
 * on every other unit's actor (hairWindOff is this bit's pair; unit itself
 * is excluded).
 */
void hairWindOn(BattleUnit *unit)
{
    Vector4 direction;
    BattleUnit **table;
    BattleUnit **next;
    BattleUnit *cur;
    int remaining;
    int count;

    direction.x = 0.0f;
    direction.y = 1.0f;
    direction.z = 0.0f;
    direction.w = 0.03f;
    EXM_SetDirectionalWind(&direction);
    EXM_SetShakePower(1.0f);
    EXM_SetShakeTime(0.75f);

    count = unitTblGet(0, &table);
    if (count > 0) {
        next = table;
        remaining = count;
        do {
            cur = *next;
            next++;
            if (cur != 0 && cur != unit) {
                cur->actor->windFlags |= HAIR_WIND_BIT;
            }
            remaining--;
        } while (remaining != 0);
    }
}

extern void EXM_StopWind(void);

/*
 * hairWindOn's pair: stops the wind and clears HAIR_WIND_BIT on every other
 * unit's actor again (unit itself is excluded).
 */
void hairWindOff(BattleUnit *unit)
{
    BattleUnit **table;
    BattleUnit **next;
    BattleUnit *cur;
    int remaining;
    int count;

    EXM_StopWind();
    count = unitTblGet(0, &table);
    if (count > 0) {
        next = table;
        remaining = count;
        do {
            cur = *next;
            next++;
            if (cur != 0 && cur != unit) {
                cur->actor->windFlags &= ~HAIR_WIND_BIT;
            }
            remaining--;
        } while (remaining != 0);
    }
}

void statEffOff(ObjectTask *unit);
/* Defined in src/main/sef.c and src/ov01/snd.c. */
extern void sefLoadEffect(int effect_no, int character_id);
extern int sndConvSpid(ObjectTask *unit, int mode, int tecId, int itmId);
extern int sndSeRegChk(ObjectTask *unit, int seType);
extern void dataSndSeLoad(ObjectTask *unit, int seId);
void scenarioBatExecPhase20(int source, ScenarioDstEntry *dest);

/*
 * First phase of a scenario action. `source` is the record scenarioSrcGet
 * hands out as an int (the action: unit, type, technique id, item id or
 * weapon slot, read at +0x0/+0x4/+0x8/+0xC); `dest` is the current
 * destination record, whose first word scenarioCam reads as the target.
 * On the phase's first frame it suspends the unit's status-effect ticks,
 * queues the action camera, loads the action's effect and its sound effect,
 * then hands over to scenarioBatExecPhase20 within the same frame.
 */
void scenarioBatExecPhase10(int source, ScenarioDstEntry *dest)
{
    ScenarioBatExec *exec = (ScenarioBatExec *)source;
    int effectNo;
    int seType;

    if (scenarioPtr.phaseTimer == 0) {
        statEffOff((ObjectTask *)exec->unit);
        scenarioCam(exec, (BattleUnit **)dest);
        if (exec->type != 3) {
            effectNo = dataTecGet(exec->tecId)->effectNo;
        } else {
            effectNo = dataItmGet(exec->slotOrItemId)->effectNo;
        }
        sefLoadEffect(effectNo, calcUPGet((ObjectTask *)exec->unit)->charaId);
        seType = sndConvSpid((ObjectTask *)exec->unit, exec->type, exec->tecId,
                             exec->slotOrItemId);
        if (!sndSeRegChk((ObjectTask *)exec->unit, seType)) {
            dataSndSeLoad((ObjectTask *)exec->unit, seType);
        }
    }
    scenarioPtrFuncSet(scenarioBatExecPhase20);
    scenarioBatExecPhase20(source, dest);
}

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", scenarioBatExecPhase20);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", scenarioBatExecPhase25);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", scenarioBatExecPhase30);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", scenarioBatExecPhase40);

/* Defined in src/main/sef.c; nonzero once the effect has finished. */
extern int sefIsFinishEffect(int effect_no);
void statEffOn(ObjectTask *unit);

/*
 * Wait for the action's effect (the item's for an item action, else the
 * technique's) to finish, then queue camera event 1 for the acting unit,
 * re-enable its status-effect ticks and advance the scenario.
 */
void scenarioBatExecPhase50(int source, ScenarioDstEntry *dest)
{
    ScenarioBatExec *exec = (ScenarioBatExec *)source;
    int effectNo;

    if (exec->type != 3) {
        effectNo = dataTecGet(exec->tecId)->effectNo;
    } else {
        effectNo = dataItmGet(exec->slotOrItemId)->effectNo;
    }
    if (sefIsFinishEffect(effectNo)) {
        D_00A57B80.type = 1;
        D_00A57B80.unit = exec->unit;
        D_00A57B80.param = exec->tecId;
        camEventExec(&D_00A57B80);
        statEffOn((ObjectTask *)exec->unit);
        scenarioBatNext();
    }
}

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", scenarioBatExecMain);

/*
 * The stolen-item state nusumeSet/nusumeGet share (data still asm-owned;
 * original ELF symbol name).
 */
static int nusumuFlag;

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

/*
 * The queued-command record unitCmdSet writes (src/ov01/unit_cmd.c, still
 * asm there); src/ov01/calc.c restates the same layout as CalcCmdEntry for
 * its own call into unitCmdSet, defined in that TU.
 */
typedef struct SyoukanCmdEntry SyoukanCmdEntry;

struct SyoukanCmdEntry {
    int selector;      /* +0x00 */
    int argument;      /* +0x04 */
    int extra;         /* +0x08 */
    unsigned char unmodeled_0c[8]; /* +0x0C: unwritten by any call here */
};

extern void unitCmdClear(ObjectTask *unit);
extern void unitCmdSet(ObjectTask *unit, SyoukanCmdEntry *cmd);

/*
 * syoukanFadeOut clears and re-queues (selector 0x30) every live side-0
 * unit except the acting unit itself; syoukanFadeIn (next) is its
 * counterpart with a different selector.
 */
void syoukanFadeOut(ObjectTask *unit)
{
    BattleUnit **table;
    int count;
    int i;

    count = unitTblGet(0, &table);
    for (i = 0; i < count; i++) {
        if (table[i] != 0 && (ObjectTask *)table[i] != unit) {
            SyoukanCmdEntry cmd;

            cmd.selector = 0x30;
            unitCmdClear((ObjectTask *)table[i]);
            unitCmdSet((ObjectTask *)table[i], &cmd);
        }
    }
}

/*
 * syoukanFadeIn is syoukanFadeOut's counterpart (selector 0x2F instead of
 * 0x30, same loop shape).
 */
void syoukanFadeIn(ObjectTask *unit)
{
    BattleUnit **table;
    int count;
    int i;

    count = unitTblGet(0, &table);
    for (i = 0; i < count; i++) {
        if (table[i] != 0 && (ObjectTask *)table[i] != unit) {
            SyoukanCmdEntry cmd;

            cmd.selector = 0x2F;
            unitCmdClear((ObjectTask *)table[i]);
            unitCmdSet((ObjectTask *)table[i], &cmd);
        }
    }
}

/*
 * syoukan1Sub's first argument: the two words the body touches (src/ov01/
 * data_unit_org_get.c owns the rest of whatever record this comes from,
 * still asm there). The second argument is never read by this body; the
 * third is the character id of the unit the summon replaces, carried in the
 * 0x2B command's extra word (syoukan2Sub fills the same word from
 * linkedUnit->charaId).
 */
typedef struct Syoukan1SubArgs {
    ObjectTask *unit; /* +0x00 */
    unsigned char unmodeled_04[4];
    int tecId;         /* +0x08 */
} Syoukan1SubArgs;

void syoukan1Sub(Syoukan1SubArgs *args, int reserved, int replacementCharaId)
{
    SyoukanCmdEntry cmd;

    unitCmdClear(args->unit);

    cmd.selector = 0x2B;
    cmd.argument = 8;
    cmd.extra = replacementCharaId;
    unitCmdSet(args->unit, &cmd);

    if (dataTecGet(args->tecId)->flags & 0x10) {
        cmd.selector = 0x2C;
    } else {
        cmd.selector = 0xF;
    }
    unitCmdSet(args->unit, &cmd);

    cmd.selector = 0x25;
    cmd.argument = 0;
    unitCmdSet(args->unit, &cmd);

    cmd.selector = 0xD;
    cmd.argument = 7;
    cmd.extra = 0;
    unitCmdSet(args->unit, &cmd);
}

/*
 * calcUPGet(unit)->linkedUnit's own CalcUnitParam (ov01/tu004 calc.c, still
 * asm there, not yet published to add these members) carries a short at
 * +0x44 inside calc.h's own unmodeled_44 span; the short at +0x38 is
 * already published as charaId.
 */
#define CALC_PARAM_LINKED_UNK44_OFF 0x44

void syoukan2Sub(ObjectTask **unit)
{
    SyoukanCmdEntry cmd;

    unitCmdClear(*unit);

    cmd.selector = 0x2B;
    cmd.argument = *(short *)((char *)calcUPGet(*unit)->linkedUnit + CALC_PARAM_LINKED_UNK44_OFF);
    cmd.extra = calcUPGet(*unit)->linkedUnit->charaId;
    unitCmdSet(*unit, &cmd);

    cmd.selector = 0xD;
    cmd.argument = 1;
    cmd.extra = 1;
    unitCmdSet(*unit, &cmd);

    cmd.selector = 0x2F;
    unitCmdSet(*unit, &cmd);
}

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
        printf("itemNameGet: err=%d %d\n", group, index);
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

/*
 * decoEffCallSub's own signed byte inside calc.h's unmodeled_10 span (that
 * header is owned by ov01/tu004 and not completed here); shared-header need.
 */
#define CALC_PARAM_DECO_EFF_OFF 0x15

/*
 * calc.h's own charaId (+0x38) is signed; decoEffCallSub reads it here with
 * an unsigned halfword load (lhu, not the lh every other charaId read in
 * this TU uses), so it is read through this raw offset instead of the
 * shared ->charaId member.
 */
#define CALC_PARAM_CHARA_ID_OFF 0x38

extern void sefCreateEffect(int *params);

/*
 * The local effect-request block sefCreateEffect reads (evidenced only by
 * this call: only the fields decoEffCallSub itself stores are named).
 */
typedef struct DecoEffParams {
    Actor *actor;              /* +0x00 */
    unsigned char unmodeled_04[0xC - 0x4];
    short moveVariant;         /* +0x0C */
    unsigned char unmodeled_0e[0x10 - 0xE];
    short charaId;             /* +0x10 */
    unsigned char unmodeled_12[0x14 - 0x12];
    short subId;               /* +0x14 */
    unsigned char unmodeled_16[0x1A - 0x16];
    short flags;               /* +0x1A */
    unsigned char unmodeled_1c[0x24 - 0x1C];
} DecoEffParams;

void decoEffCallSub(BattleUnit *unit, int subId) {
    DecoEffParams params;
    unsigned short charaId;

    memset(&params, 0, sizeof(params));
    params.actor = unit->actor;
    params.moveVariant = (signed char)((signed char *)calcUPGet((ObjectTask *)unit))[CALC_PARAM_DECO_EFF_OFF];
    charaId = *(unsigned short *)((unsigned char *)calcUPGet((ObjectTask *)unit) + CALC_PARAM_CHARA_ID_OFF);
    params.subId = (short)subId;
    params.charaId = charaId;
    params.flags = (short)calcUPGet((ObjectTask *)unit)->flags;
    sefCreateEffect((int *)&params);
}

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

/*
 * The weapon ids 0xD..0x10 (`addiu $2,$2,-0xD`/`sltiu $2,$2,0x4` at
 * 0x00a0e5e4..0x00a0e5e8, on weaponId[2], `lh $2,0x62($2)` at 0x00a0e5e0)
 * and 0xC (`addiu $4,$0,0xC` at 0x00a0e670), the accessory id 0x108
 * ACT_setArms gets (`addiu $6,$0,0x108` at 0x00a0e620) and the technique
 * ids 8 and 0xB (0x00a0e650/0x00a0e658) are compared as the original does;
 * what each id is, is not established here.
 */
void spWepDispOn(ScenarioBatExec *exec, int weapon)
{
    int tecKind;
    struct WepSpEntry *wepSp;
    int weaponId;
    Actor *weaponActor;

    if (exec->type == 1) {
        tecKind = dataTecGet(exec->tecId)->kind;
    } else {
        spWepDispOnSub(exec->unit, weapon, 0);
        return;
    }

    if (tecKind == 6) {
        spWepDispOnSub(exec->unit, exec->slotOrItemId, 1);
    } else {
        wepSp = dataTid2WepSp(exec->tecId);
        if (wepSp != 0) {
            weaponId = calcUPGet((ObjectTask *)exec->unit)->weaponId[2];
            if ((unsigned int)(weaponId - 0xD) >= 4) {
                weaponActor = exec->unit->weapon[weapon & 3];
                if (weaponActor != 0) {
                    /* `ori $2,$2,0x8` at 0x00a0e614: the same bit spWepDispOffSub sets. */
                    weaponActor->flags |= WEAPON_HIDDEN_BIT;
                }
            } else {
                ACT_setArms(exec->unit->weapon[2], exec->unit->actor, 0x108, 0);
            }
            spWepDispOnSub(exec->unit, 2, 0);
        } else if (exec->tecId == 8 || exec->tecId == 0xB) {
            weaponId = calcUPGet((ObjectTask *)exec->unit)->weaponId[exec->slotOrItemId];
            if (weaponId == 0xC) {
                spWepDispOnSub(exec->unit, exec->slotOrItemId, 0);
            }
        }
    }
}

/*
 * spWepDispOn's worker: with fade clear the weapon actor in slot `slot & 3`
 * is revealed at once, otherwise transWepIn fades it in.
 */
void spWepDispOnSub(BattleUnit *unit, int slot, int fade)
{
    if (fade == 0) {
        unit->weapon[slot & 3]->flags &= ~WEAPON_HIDDEN_BIT;
        return;
    }
    transWepIn(unit, slot);
}

void spWepDispOffSub(BattleUnit *unit, int slot, int fade);

/*
 * spWepDispOn's counterpart at the end of an action: hide the weapon the
 * action showed again (fading for kind-6 techniques and for slot-2 weapons
 * 0x11 and 0xB), or put a special weapon's actor back.
 */
void spWepDispOff(ScenarioBatExec *exec, int weapon)
{
    int tecKind;
    struct WepSpEntry *wepSp;
    int weaponId;
    Actor *weaponActor;

    if (exec->type == 1) {
        tecKind = dataTecGet(exec->tecId)->kind;
    } else {
        spWepDispOffSub(exec->unit, weapon, 0);
        return;
    }

    weaponId = calcUPGet((ObjectTask *)exec->unit)->weaponId[2];
    if (tecKind == 6) {
        spWepDispOffSub(exec->unit, exec->slotOrItemId, 1);
    } else if (weaponId == 0x11 || weaponId == 0xB) {
        spWepDispOffSub(exec->unit, 2, 1);
    } else {
        wepSp = dataTid2WepSp(exec->tecId);
        if (wepSp != 0) {
            weaponId = calcUPGet((ObjectTask *)exec->unit)->weaponId[2];
            if ((unsigned int)(weaponId - 0xD) >= 4) {
                weaponActor = exec->unit->weapon[weapon & 3];
                if (weaponActor != 0 && weaponId != 0x11) {
                    weaponActor->flags &= ~WEAPON_HIDDEN_BIT;
                }
            } else {
                ACT_setHand(exec->unit->actor, 0);
            }
            spWepDispOffSub(exec->unit, 2, 0);
        } else if (exec->tecId == 8 || exec->tecId == 0xB) {
            weaponId = calcUPGet((ObjectTask *)exec->unit)->weaponId[exec->slotOrItemId];
            if (weaponId == 0xC) {
                spWepDispOffSub(exec->unit, exec->slotOrItemId, 0);
            }
        }
    }
}

/*
 * spWepDispOff's worker: with fade clear the weapon actor in slot
 * `slot & 3` is hidden at once, otherwise transWepOut fades it out.
 */
void spWepDispOffSub(BattleUnit *unit, int slot, int fade)
{
    if (fade == 0) {
        unit->weapon[slot & 3]->flags |= WEAPON_HIDDEN_BIT;
        return;
    }
    transWepOut(unit, slot);
}

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

extern void unitActdraw(void);

void transWepInDraw(Actor *actor) {
    float transparency;

    unitActdraw();
    transparency = actor->transparency + 0.1f;
    actor->transparency = transparency;
    if (transparency >= 1.0f) {
        actor->transparency = 1.0f;
        actor->draw = 0;
    }
}

void transWepOut(BattleUnit *unit, int slot)
{
    Actor *weapon;

    weapon = unit->weapon[slot & 3];
    if (weapon != 0) {
        weapon->draw = transWepOutDraw;
        weapon->transparency = 1.0f;
    }
}

void transWepOutDraw(Actor *actor) {
    float transparency;

    unitActdraw();
    transparency = actor->transparency - 0.1f;
    actor->transparency = transparency;
    if (transparency <= 0.0f) {
        actor->transparency = 0.0f;
        actor->draw = 0;
        actor->flags |= WEAPON_HIDDEN_BIT;
    }
}

#define TEC_PROC_FLOW_SPEC 0x20
/* Actor.windFlags bit 0x1000 (`andi $2,$2,0x1000` at 0x00a0eab0 and
 * 0x00a0eae0): set, the unit is still held in its action motion. */
#define ACTOR_MOTION_HOLD_BIT 0x1000

extern void unitMotStandSet(ObjectTask *unit);

/*
 * Whether the scenario action has finished: a special-flow technique
 * reports `done` as its flow handler left it; techniques and items of
 * types 2 and 3 wait for their effect (an item also returns its unit to
 * the standing motion while it is held); anything else waits for the
 * motion hold to be set.
 */
int scenarioSrcEndChk(ScenarioBatExec *exec, ScenarioDstEntry *dest, int done)
{
    int effectNo;
    unsigned int held;

    if (exec->type != 3 && (dataTecGet(exec->tecId)->procFlags & TEC_PROC_FLOW_SPEC)) {
        return done;
    }
    if (exec->type == 2 || exec->type == 3) {
        if (exec->type != 3) {
            effectNo = dataTecGet(exec->tecId)->effectNo;
        } else {
            effectNo = dataItmGet(exec->slotOrItemId)->effectNo;
            if (exec->unit->actor->windFlags & ACTOR_MOTION_HOLD_BIT) {
                unitMotStandSet((ObjectTask *)exec->unit);
            }
        }
        return sefIsFinishEffect(effectNo) != 0;
    }
    held = exec->unit->actor->windFlags & ACTOR_MOTION_HOLD_BIT;
    return held != 0;
}

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
static MonsGain monsGain;

MonsGain *monsGainPtrGet(void)
{
    return &monsGain;
}

void monsGainInit(void)
{
    memset(&monsGain, 0, sizeof(MonsGain));
}

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", monsGainSet);

int itemGrpChk(int group, int item)
{
    switch (group) {
    case 0:
        if (item < 73) break;
        return 0;
    case 1:
        if (item < 116) break;
        return 0;
    case 2:
        if (item < 179) break;
        return 0;
    case 3:
        if (item < 181) break;
        return 0;
    default:
        return 0;
    }
    return 1;
}

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

/*
 * objEntry2 and objStdInit are defined by ov01/tu001 obj.c (src/ov01/obj.h
 * is that TU-local header and is not includable here);
 * src/ov01/entry_first_init.c already redeclares them the same way.
 */
extern ObjectTask *objEntry2(void *argument, void (*callback)(ObjectTask *task));
extern void objStdInit(ObjectTask *task);

/*
 * The enemy cursor's own task callbacks, both defined further down in this
 * TU: objCur (still asm) reaches the cursor's state record through
 * CURSOR_STATE_OFF exactly as objCurDraw right after it does.
 */
void objCur(CursorObject *cursor);
void objCurDraw(CursorObject *cursor);

/*
 * Further evidenced words on the cursor's state record (CURSOR_STATE_OFF,
 * the object's own work pointer):
 *   state + 0x04 and state + 0x78 are cleared when the cursor is created;
 *   state + 0x80 is the target address curSel below compares and updates;
 *   state + 0x90 starts the three coordinate floats the caller supplies.
 * The 0x40 bit objEnCurCreate sets in the flags word at STATE_FLAGS_OFF is
 * the enemy-side bit objCurDraw reads back to pick objDispEnCur.
 */
#define STATE_UNK04_OFF 0x04
#define STATE_UNK78_OFF 0x78
#define STATE_TARGET_OFF 0x80
#define STATE_POS_OFF 0x90

ObjectTask *objEnCurCreate(float x, float y, float z)
{
    ObjectTask *task;
    CursorCommandState *state;
    float *pos;

    task = objEntry2(objCur, (void (*)(ObjectTask *))objCurDraw);
    state = task->work;
    objStdInit(task);
    pos = (float *)((char *)state + STATE_POS_OFF);
    pos[0] = x;
    pos[1] = y;
    pos[2] = z;
    *(unsigned int *)((char *)state + STATE_FLAGS_OFF) |= 0x40;
    *(int *)((char *)state + STATE_UNK04_OFF) = 0;
    *(void **)((char *)state + STATE_TARGET_OFF) = 0;
    *(int *)((char *)state + STATE_UNK78_OFF) = 0;
    return task;
}

/*
 * objRemove is defined by ov01/tu001 obj.c; menu.c already redeclares it
 * the same way to call it from a different TU (src/ov01/obj.h is that
 * TU-local header and is not includable here).
 */
extern void objRemove(ObjectTask *task);
void objCurRemove(void)
{
    if (pTaskEnCur != 0) {
        objRemove(pTaskEnCur);
        pTaskEnCur = 0;
    }
}

/* src/ov01/obj.c; menu.c redeclares them the same way. */
extern void *objCmdPtrGet(ObjectTask *task);
extern void objHatoVec(ObjectTask *task, int steps);
extern int objStdMove(ObjectTask *task);
/* src/main/sef.c, spelled as src/ov01/menu.c and src/ov01/m_cam.c do. */
extern void sefGetNullPosition(Vector4 *position, void *actor, int part, int flag);

/* curCmdSet selectors: 1 moves the cursor to a position, 2 to a unit; 3 is
 * accepted by objCur alongside 2. */
#define CURSOR_CMD_POSITION 1
#define CURSOR_CMD_UNIT 2
/* Cursor state flag bits: 0x2 is set by curCmdSet while a command is
 * queued; 0x4 hides the cursor (objCurDraw); 0x8 marks a unit selection. */
#define CURSOR_STATE_FLAG_QUEUED 0x2
#define CURSOR_STATE_FLAG_HIDDEN 0x4

/*
 * The cursor task's per-frame step: run the queued command. A position
 * move steps the cursor until objStdMove reports arrival; a unit command
 * snaps it to the unit's null position unless the cursor is hidden.
 * Either way the queued flag is cleared once the command is done.
 */
void objCur(CursorObject *cursor)
{
    CursorCommandState *state = cursor->state;
    CursorCommand *cmd = objCmdPtrGet((ObjectTask *)cursor);
    Vector4 *position = &state->transform.fields.position;
    BattleUnit *unit;
    unsigned int flags;

    switch (cmd->selector) {
    case CURSOR_CMD_UNIT:
    case 3:
        unit = (BattleUnit *)cmd->argument;
        flags = state->flags;
        if (!(flags & CURSOR_STATE_FLAG_HIDDEN)) {
            sefGetNullPosition(position, unit->actor, 0, 0);
            flags = state->flags;
        }
        state->flags = flags & ~CURSOR_STATE_FLAG_QUEUED;
        break;
    case CURSOR_CMD_POSITION:
        objHatoVec((ObjectTask *)cursor, 2);
        if (objStdMove((ObjectTask *)cursor)) {
            state->flags &= ~CURSOR_STATE_FLAG_QUEUED;
            cmd->selector = 0;
        }
        break;
    }
}

extern void objDispPlCur(void);
extern void objDispEnCur(CursorObject *cursor);

/*
 * cursor + 0x10 (CURSOR_STATE_OFF) reaches the same state record curCmdSet
 * writes; this function reads back state + 0x00 (STATE_FLAGS_OFF) and
 * returns without drawing when bit 0x4 is set, otherwise dispatches to the
 * enemy-side draw callee (bit 0x40 set) or the player-side one (objDispPlCur,
 * which takes no argument).
 */
void objCurDraw(CursorObject *cursor)
{
    CursorCommandState *state;
    unsigned int flags;

    state = *(CursorCommandState **)((char *)cursor + CURSOR_STATE_OFF);
    flags = *(unsigned int *)((char *)state + STATE_FLAGS_OFF);
    if (flags & 4) {
        return;
    }
    if (!(flags & 0x40)) {
        objDispPlCur();
        return;
    }
    objDispEnCur(cursor);
}

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

/*
 * Battle cursor selection list. entries[] holds the target address of each
 * selectable item, indexed by curIdx (word width, stride 4); count bounds
 * curIdx on both ends; prevIdx is a snapshot of curIdx taken on entry and
 * compared again after the target update to decide whether to play the
 * selection sound. units[] and entries[] are each sized to the gap their
 * offsets leave (0x00..0x23 and 0x24..0x47, stride 4 = 9 words).
 */
typedef struct CursorSelectList {
    BattleUnit *units[9]; /* +0x00..+0x23: the selectable units;
                           * curMove2Pos reads units[curIdx]->actor (`lw
                           * $3,0x0($2)`/`lw $5,0x14($3)` at 0x00a0fb48) */
    void *entries[9]; /* +0x24..+0x47: unit pointers, or position ids
                       * (cast) when posMode is set (curMove2Pos) */
    int count;         /* +0x48 */
    int curIdx;         /* +0x4C */
    int prevIdx;         /* +0x50 */
    int posMode;         /* +0x54: nonzero, curMove2Pos targets a field
                          * position from entries[] instead of a unit
                          * (`lw $2,0x54($16)` at 0x00a0fb24) */
    int groupFlag;       /* +0x58: toggled by curGrpSel, set by curSelGrpSet */
} CursorSelectList;

/*
 * The battle cursor's selection record (original ELF symbol name):
 * curSel/curMove2Pos work on it and curSelGrpSet/curSelGrpGet
 * own its group word.
 */
static CursorSelectList curMove;

/*
 * dataPadRead is defined by ov01/tu005 data_unit_org_get.c (still asm);
 * declared here to call it from this TU.
 */
extern int dataPadRead(int padNo);

/*
 * curMove2Pos is defined later in this TU (still asm); forward-declared to
 * call it here.
 */
extern void curMove2Pos(CursorObject *cursor, CursorSelectList *list);

/*
 * sndSysSePlay is defined by ov01/tu013 snd.c (still asm); other TUs already
 * redeclare it the same way to call it from a different TU.
 */
extern int sndSysSePlay(int soundId);

/*
 * +0x80 on the cursor's state record (CURSOR_STATE_OFF above): the target
 * address curMove2Pos last moved the cursor toward.
 */
#define STATE_TARGET_OFF 0x80

int curSel(CursorObject *cursor, CursorSelectList *list, int nextMask, int prevMask)
{
    CursorCommandState *state;
    void *target;

    list->prevIdx = list->curIdx;
    if (dataPadRead(1) & nextMask) {
        list->curIdx++;
        if (list->curIdx >= list->count) {
            list->curIdx = 0;
        }
    } else if (dataPadRead(1) & prevMask) {
        list->curIdx--;
        if (list->curIdx < 0) {
            list->curIdx = list->count - 1;
        }
    }

    state = *(CursorCommandState **)((char *)cursor + CURSOR_STATE_OFF);
    target = list->entries[list->curIdx];
    if (*(void **)((char *)state + STATE_TARGET_OFF) != target) {
        *(void **)((char *)state + STATE_TARGET_OFF) = target;
        curMove2Pos(cursor, list);
        if (list->curIdx != list->prevIdx) {
            sndSysSePlay(0xA);
        }
    }
    return list->curIdx;
}

int curGrpSel(CursorObject *cursor, CursorSelectList *list, int nextMask, int prevMask)
{
    if (!(dataPadRead(1) & nextMask) && !(dataPadRead(1) & prevMask)) {
        return 0;
    }
    if (list->groupFlag == 0) {
        list->groupFlag = 1;
    } else {
        list->groupFlag = 0;
    }
    sndSysSePlay(0xA);
    return 1;
}

/* src/ov01/data_unit_org_get.c, spelled as src/ov01/unit_cmd.h does. */
extern Vector4 *dataPosTblGet(int index);

/*
 * Send the cursor to the selected entry: a unit's null position, raised by
 * the unit's size class (1.0 below class 3, none for class 3, 2.5 above),
 * or a field position from dataPosTblGet (negative ids mirror x). A hidden
 * cursor jumps there at once; the move itself is queued as command 1.
 */
void curMove2Pos(CursorObject *cursor, CursorSelectList *list)
{
    CursorCommand cmd;
    CursorCommandState *state = cursor->state;
    CursorTransform *transform = &state->transform;
    Vector4 *pos;
    int posId;
    int sizeClass;
    float x;

    if (list->posMode == 0) {
        sefGetNullPosition(&transform->fields.goal, list->units[list->curIdx]->actor, 0, 0);
        sizeClass = calcUPGet((ObjectTask *)list->units[list->curIdx])->sizeClass;
        if (sizeClass < 3) {
            transform->fields.offset.y = 1.0f;
        } else if (sizeClass < 4) {
            transform->fields.offset.y = 0.0f;
        } else {
            transform->fields.offset.y = 2.5f;
        }
    } else {
        posId = (int)list->entries[list->curIdx];
        pos = dataPosTblGet(posId);
        x = pos->x;
        transform->fields.goal.x = x;
        transform->fields.goal.y = pos->y;
        transform->fields.goal.z = pos->z;
        if (posId < 0) {
            transform->fields.goal.x = -x;
        }
        transform->fields.offset.y = 0.0f;
    }
    if (state->flags & CURSOR_STATE_FLAG_HIDDEN) {
        transform->fields.position.x = transform->fields.goal.x;
        transform->fields.position.y = transform->fields.goal.y;
        transform->fields.position.z = transform->fields.goal.z;
    }
    cmd.selector = CURSOR_CMD_POSITION;
    curCmdSet(cursor, &cmd);
}

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", curEnProcInit);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", curEnProc);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", curPosProcInit);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", curPosProc);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", curTgtProcInit);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", curTgtProc);

/*
 * Written -1 whenever curSelGrpSet derives the group from flags instead of
 * receiving it directly (data still asm-owned; original ELF symbol name); no other
 * reference is evidenced in this TU.
 */
void curSelGrpSet(u32 group, int flags)
{
    CursorSelectList *state = &curMove;

    if (group < 2U) {
        state->groupFlag = group;
        return;
    }
    if (flags & 8) {
        state->groupFlag = 0;
    } else if (flags & 0x40) {
        state->groupFlag = 0;
    } else {
        state->groupFlag = 1;
    }
    oldCurGrp = -1;
}

/*
 * The current selection group.
 */
int curSelGrpGet(void)
{
    return curMove.groupFlag;
}

void objDispPlCur(void)
{
}

extern void curTexTrans(int unused);
extern void objDispCur(CursorObject *cursor, int isPlayer);
extern void menuStatTexTrans(void);
extern void grGpInit(void *packet);
extern void grPacketSend(void *packet);
extern int frameCntGet(void);
/* src/ov01/menu.c takes a GrPacket *; this file passes grGpInit's
 * `int packet[2]` record as void *, as objDispCur does. */
extern void menuActCurPut(void *packet, ObjectTask *cursor, ObjectTask *unit, int frame);
extern void monsNameDisp(void *packet, ObjectTask *unitTask, int x, int y, int isEnemy);

/*
 * Bit 0x40 of the target's work-record flags word (`lw $3,0x10($2)`/`lw
 * $2,0x0($3)`/`andi $2,$2,0x40` at 0x00a106e8..0x00a106f0); the name is the
 * one src/ov01/data_unit_org_get.h gives the same Actor.flags bit (that
 * header is not included here).
 */
#define BATTLE_ACTOR_FLAG_ENEMY 0x40
/* Bit 0x80 of the same word: objDispEnCur skips the name of a target that
 * has it (`andi $2,$2,0x80` at 0x00a10620). */
#define BATTLE_ACTOR_FLAG_DEAD 0x80

/*
 * Draw the enemy-side cursor: the cursor itself, the acting unit's marker
 * when the target is an enemy, and the target's name while a selection is
 * active and help mode 2 is off (an enemy target only while it is alive).
 */
void objDispEnCur(CursorObject *cursor)
{
    int packet[2]; /* grGpInit's record, as in curTexTrans */
    int selectActive;
    unsigned int enemyBit;
    int isEnemy;

    curTexTrans(0);
    objDispCur(cursor, 0);
    menuStatTexTrans();
    if (tgtUnitGet() != 0 && (tgtUnitGet()->work->flags & BATTLE_ACTOR_FLAG_ENEMY)) {
        grGpInit(packet);
        menuActCurPut(packet, (ObjectTask *)cursor, (ObjectTask *)actUnitGet(),
                      ((unsigned int)frameCntGet() >> 1) & 7);
    }
    if (tgtUnitGet() == 0) {
        return;
    }
    selectActive = cursor->state->selectActive;
    if (selectActive != 1 || battleCtrlHelpGet() == 2) {
        return;
    }
    enemyBit = tgtUnitGet()->work->flags & BATTLE_ACTOR_FLAG_ENEMY;
    isEnemy = enemyBit != 0;
    if (isEnemy == selectActive) {
        if (tgtUnitGet() == 0 || (tgtUnitGet()->work->flags & BATTLE_ACTOR_FLAG_DEAD)) {
            return;
        }
    }
    grGpInit(packet);
    monsNameDisp(packet, (ObjectTask *)tgtUnitGet(), 8, 0x5C, isEnemy);
    grPacketSend(packet);
}

/*
 * Bit 0x8 of the cursor state's flags word (`andi $2,$2,0x8` at
 * 0x00a106ac): set, objDispCur runs the tgtUnitGet/calcLineChk test below.
 * Nothing here shows what sets it, so it is named by the bit only.
 */
#define CURSOR_STATE_FLAG_BIT3 0x8

/*
 * objDispCur calls it with $a0 = packet (`daddu $4,$29,$0` at 0x00a1072c, the
 * delay slot of the `jal` at 0x00a10728) and $a1 = cursor (`daddu $5,$17,$0`
 * at 0x00a10698); the original body is empty, so neither is read.
 */
void objDispCurSubPl(void *packet, CursorObject *cursor);
/* Defined by ov01/tu008 (src/ov01/gr_gp_init.c). Declared here because
 * objDispCur calls them ahead of the declarations further down; without them
 * gcc 2.96 reports "type mismatch with previous implicit declaration". */
extern void grGpInit(void *packet);
extern void grPacketSend(void *packet);
extern void curEnvMake(void *packet, int isAlly);
extern void objDispCurSubEn(void *packet, CursorObject *cursor, int outOfLine);
extern void curPutSub(void *packet, CursorObject *cursor);
extern void *objCmdPtrGet(ObjectTask *task);
extern int calcLineChk(ObjectTask *unit);

/*
 * Draw the battle cursor into one GS packet: the player-side cursor, or the
 * enemy-side one, coloured by the target's side and marked when the state's
 * 0x8 flag is set and the target fails calcLineChk.
 */
void objDispCur(CursorObject *cursor, int isPlayer)
{
    int packet[2]; /* grGpInit's record, as in curTexTrans */
    int outOfLine;
    int isAlly;

    grGpInit(packet);
    if (!isPlayer) {
        /* The original calls it (0x00a1069c) and ignores the result. */
        objCmdPtrGet((ObjectTask *)cursor);
        if ((cursor->state->flags & CURSOR_STATE_FLAG_BIT3) &&
            tgtUnitGet() != 0 && calcLineChk((ObjectTask *)tgtUnitGet()) == 0) {
            outOfLine = 1;
            isAlly = 0;
        } else {
            unsigned int isEnemy;

            outOfLine = 0;
            /*
             * The original performs an unguarded read through tgtUnitGet()'s
             * return here (`lw $3,0x10($2)` at 0x00a106e8 straight after the
             * `jal tgtUnitGet` at 0x00a106e0): no null test exists on this
             * path, so none is written.
             */
            isEnemy = tgtUnitGet()->work->flags & BATTLE_ACTOR_FLAG_ENEMY;
            isAlly = isEnemy == 0;
        }
        curEnvMake(packet, isAlly);
        objDispCurSubEn(packet, cursor, outOfLine);
        curPutSub(packet, cursor);
    } else {
        objDispCurSubPl(packet, cursor);
    }
    grPacketSend(packet);
}

void objDispCurSubPl(void *packet, CursorObject *cursor)
{
}

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", objDispCurSubEn);

/*
 * grPutFT4STQ's quad record (src/ov01/gr_gp_init.c, TU-local there: four
 * corners, one shared RGBA, per-corner S/T); restated for the cursor quad.
 */
typedef struct CursorQuad {
    Vector4 points[4];       /* +0x00 */
    unsigned char color[4];  /* +0x40: R, G, B, A */
    float s[4];              /* +0x44 */
    float t[4];              /* +0x54 */
} CursorQuad;
/* The enemy-target cursor quad (data still asm-owned; original ELF symbol name). */
extern CursorQuad cur1;

extern void grCalcMatrix(CursorObject *cursor, Matrix4 matrix);
extern void grGsRegSet(void *packet, int reg, int value);
extern void grOpenFT4STQ(void *packet);
extern void grPutFT4STQ(void *packet, CursorQuad *quad, Matrix4 matrix);
extern void grCloseFT4STQ(void);

/*
 * Draw the spinning cursor quad: the cursor's transform is flattened onto
 * the ground, given a unit scale and a one-turn-per-360-frames Y rotation,
 * turned into a matrix and restored afterwards.
 */
void curPutSub(void *packet, CursorObject *cursor)
{
    CursorTransform saved;
    Matrix4 matrix;
    CursorTransform *transform;
    float degToRad;

    transform = &cursor->state->transform;
    saved = *transform;
    transform->fields.scale.x = 1.0f;
    transform->fields.offset.y = 0.01f;
    transform->fields.scale.y = 1.0f;
    transform->fields.scale.z = 1.0f;
    transform->fields.position.y = 0.0f;
    degToRad = 0.017453292f; /* pi/180 */
    transform->fields.rotation.y = degToRad * (float)((unsigned int)frameCntGet() % 360);
    grCalcMatrix(cursor, matrix);
    grGsRegSet(packet, 0x14, 0x60);
    grGsRegSet(packet, 0x42, 0x44);
    grOpenFT4STQ(packet);
    grPutFT4STQ(packet, &cur1, matrix);
    grCloseFT4STQ();
    *transform = saved;
}

/*
 * grGpInit's own record (src/ov01/gr_gp_init.c, still asm there; that TU
 * owns the layout and has not published it through a header yet -- shared-
 * header need). curTexTrans only ever takes this local's address and never
 * reads it back, so the storage is left untyped here rather than restating
 * the owner's struct.
 */
extern void grGpInit(void *packet);
extern void grGsRegSet(void *packet, int reg, int value);
extern void grPacketSend(void *packet);

/*
 * dataXtxFileGet's own table entry (src/ov01/data_unit_org_get.c, still
 * asm); only the word at +0x4 is evidenced here, forwarded unchanged to
 * xglPacketTextureTrans.
 */
typedef struct XtxFile {
    unsigned char unmodeled_00[4];
    int textureId; /* +0x4 */
} XtxFile;

extern XtxFile *dataXtxFileGet(int index);
extern void xglPacketTextureTrans(int textureId);

#define CURSOR_XTX_SLOT 2
/* GS register 0x3F (TEXFLUSH): any write flushes the texture cache after a
 * texture upload, matching the dataXtxFileGet(2) transfer just above it. */
#define GS_REG_TEXFLUSH 0x3F

/*
 * objDispEnCur, the only caller, passes 0 (`daddu $4,$0,$0` at 0x00a10538,
 * the delay slot of `jal curTexTrans`); the body never reads $a0.
 */
void curTexTrans(int unused)
{
    int packet[2]; /* grGpInit's own record: a data pointer and an element
                     * count (this call site's own evidence for the size). */

    xglPacketTextureTrans(dataXtxFileGet(CURSOR_XTX_SLOT)->textureId);
    grGpInit(packet);
    grGsRegSet(packet, GS_REG_TEXFLUSH, 1);
    grPacketSend(packet);
}

/*
 * Local compatibility spelling of GS_REG_SET: the definition
 * (src/ov01/gr_gp_init.c) takes (GrPacket *, long long, long long); `void *`
 * stands in for GrPacket here because this file passes the `int packet[]`
 * record grGpInit fills, the same spelling src/ov01/entry_first_init.c's
 * encEnvMake already uses for that idiom.
 */
extern void GS_REG_SET(void *packet, long long register_id, long long control);
extern unsigned short dataXtxAdrGet(int index);

void curEnvMake(void *packet, int isAlly)
{
    unsigned short tbp0, tbp1;

    if (!isAlly) {
        tbp0 = dataXtxAdrGet(0xF);
        tbp1 = dataXtxAdrGet(0x10);
    } else {
        tbp0 = dataXtxAdrGet(0xF);
        tbp1 = dataXtxAdrGet(0x12);
    }

    GS_REG_SET(packet, 0xE, 0x1000000000008005LL);
    GS_REG_SET(packet, 0x6,
               tbp0 | ((long long)tbp1 << 37) | 0x20000005DD340000LL);
    GS_REG_SET(packet, 0x42, 0);
    GS_REG_SET(packet, 0x47, 0x5000D);
    GS_REG_SET(packet, 0x8, 0);
    GS_REG_SET(packet, 0x14, 0);
}
