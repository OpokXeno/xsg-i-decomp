#ifndef INCLUDE_OV01_BATTLE_INIT_H
#define INCLUDE_OV01_BATTLE_INIT_H

#include "shared.h"

typedef struct Actor Actor;
typedef struct BattleUnit BattleUnit;
struct CalcUnitParam;

/* Hides the actor: unit_cmd.c sets and clears it on the motion actor and tests
 * it on the work actor (unitCmdSet, unitTelIn). include/main/act_1.h defines the
 * same macro, so this bit is not an enumerator. */
#define ACTOR_FLAG_HIDDEN 0x8

/* Bits of Actor.flags, the battle actor record every OV01 TU shares. */
enum ActorFlag {
    /* unitCmdSet sets it when it queues a command whose selector is nonzero
     * (ori 0x2 at 0x00a01d5c); unitCmdFree clears it (and 0x00a01e08). */
    ACTOR_FLAG_CMD_PENDING = 0x2,
    /* The enemy-controlled bit: unitLoad picks sefSetupEnemy on it, and
     * unitActupdate recomputes the ground height only when it is set. */
    ACTOR_FLAG_ENEMY = 0x40,
    /* Set by unitCmdDead (0x00a0307c, 0x00a03270) and unitCmdEscape; unitLiveNumGet
     * skips it as not alive, unitCmdDeadSet refuses to set it again and unitCmdWake
     * clears it. calcDeadUnitNum counts it, calcBackLineChk skips it. */
    ACTOR_FLAG_DEAD = 0x80,
    /* unitCmdGuard sets it (ori 0x800 at 0x00a02764) and unitCmdDef tests it
     * to skip its follow-up motion 0x1C (andi at 0x00a02fac). */
    ACTOR_FLAG_GUARD = 0x800
};

/*
 * The unit record scenarioMtd, transWepIn/transWepOut and spWepDispOn
 * (0x00a0e550) all share: spWepDispOn receives the
 * same pointer scenarioMtd receives, dereferences it to reach this record,
 * then indexes its +0x1C table by (weapon-slot & 3) and reads +0x14, exactly
 * as transWepIn/transWepOut do on their own argument and scenarioMtd does on
 * *unitHandle. Bytes between the evidenced members are untouched here.
 */
struct BattleUnit {
    XglTaskPrefix task;   /* +0x00: the scheduler header (include/shared.h) */
    Actor *work;          /* +0x10: the unit's battle actor (ObjectTask.work) */
    Actor *actor;         /* +0x14: the motion actor */
    unsigned char unmodeled_18[0x1C - 0x18];
    Actor *weapon[4];     /* +0x1C: one actor per weapon slot (slot & 3) */
    Actor *subActor;      /* +0x2C: unitRemoveActor disposes it first when set */
};

/*
 * Show (transWepIn) or hide (transWepOut) the weapon actor in the unit's
 * slot `slot & 3` by installing its fade draw callback.
 */
void transWepIn(BattleUnit *unit, int slot);

void transWepOut(BattleUnit *unit, int slot);

/*
 * manWorkGet's return type. Only the two fields menuKeyGuideObjDraw (src/
 * ov01/menu.c, ov01/tu009) consumes are evidenced there: unitTask at +0x10
 * and controlPhase at +0xc. This TU's own code only takes the record's
 * address (data still asm-owned; original ELF symbol name).
 */
typedef struct ManWork {
    int tgtSelect;        /* +0x0: picks pTgtUnit2 (values 6, 7, 9, 10, 11) or
                           * pTgtUnit in tgtUnitAdrGet */
    int field04;          /* +0x4: cleared by battleInit */
    int selection;        /* +0x8 */
    int controlPhase;     /* +0xc */
    BattleUnit *unitTask; /* +0x10 */
    unsigned char unmodeled_14[4]; /* original named span continues to +0x18 */
} ManWork;

/*
 * Each battle unit's own large parameter block, reached only through the
 * per-unit table below; nothing else in this TU dereferences it.
 */
typedef struct UnitWork {
    unsigned char unmodeled_00[0xA8C];
    unsigned int flags; /* +0xA8C: UNIT_WORK_STATUS_BLOCKED suppresses
                          * status-effect ticks for this unit */
} UnitWork;

ManWork *manWorkGet(void);

/* Functions used by other translation units (ov01/tu003, battle_init.c). */
int unitCmdListSet(BattleUnit *unit, int *command);
int keyCmdApGet(BattleUnit *unit, int command);
void decoEffCall(BattleUnit *unit, int type);

#endif /* INCLUDE_OV01_BATTLE_INIT_H */
