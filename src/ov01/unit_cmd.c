/*
 * OV01 original TU 2: 0x00a00d60..0x00a07770 (119 functions)
 */
#include "common.h"
#include "shared.h"
#include "unit_cmd.h"

static BattleUnit *unitTbl[8];
static int unitPlNum;
static int unitEnNum;

void unitInit(void)
{
    memset(&unitTbl, 0, sizeof(unitTbl));
    unitPlNum = 0;
    unitEnNum = 0;
}

/*
 * Returns the number of slots for side (0 players, 1 enemies, anything else
 * all 8) and points *table at the side's first slot.
 */
int unitTblGet(int side, BattleUnit ***table)
{
    if (side == 0) {
        *table = unitTbl;
        return 3;
    }
    if (side == 1) {
        *table = &unitTbl[3];
        return 5;
    }
    *table = unitTbl;
    return 8;
}

/*
 * Stores unit in the first free slot of its side (0 players, 1 enemies) and
 * returns 1, or returns 0 when no slot is free or the side is invalid.
 *
 * The side's first slot is its own local, copied into the loop index where
 * the two side paths meet; starting the loop index itself in each branch
 * keeps the copy out of the loop.
 */
int unitTblSet(int side, BattleUnit *unit)
{
    int first;
    int end;
    int i;

    if (side == 0) {
        first = 0;
        end = 3;
    } else if (side == 1) {
        first = 3;
        end = 8;
    } else {
        return 0;
    }
    for (i = first; i < end; i++) {
        if (unitTbl[i] == 0) {
            unitTbl[i] = unit;
            if (i < 3) {
                unitPlNum++;
            } else {
                unitEnNum++;
            }
            return 1;
        }
    }
    return 0;
}

/* Clears the slot holding unit and updates the side count. Returns 1 when found. */
int unitTblRemove(BattleUnit *unit)
{
    BattleUnit **units = unitTbl;
    int i;

    for (i = 0; i < 8; i++) {
        if (units[i] == unit) {
            units[i] = 0;
            if (i < 3) {
                unitPlNum--;
            } else {
                unitEnNum--;
            }
            return 1;
        }
    }
    return 0;
}

/* Puts newUnit in the slot of oldUnit. Returns 1 when oldUnit is found. */
int unitTblChg(BattleUnit *oldUnit, BattleUnit *newUnit)
{
    BattleUnit **units = unitTbl;
    int i;

    for (i = 0; i < 8; i++) {
        if (units[i] == oldUnit) {
            units[i] = newUnit;
            return 1;
        }
    }
    return 0;
}

/* Returns the number of units on side (0 players, 1 enemies, anything else both). */
int unitNumGet(int side)
{
    int count;

    if (side == 0) {
        count = unitPlNum;
    } else if (side == 1) {
        count = unitEnNum;
    } else {
        count = unitPlNum + unitEnNum;
    }
    return count;
}

/* Counts the units on side that are not dead. */
int unitLiveNumGet(int side)
{
    BattleUnit **table;
    BattleUnit **cur;
    BattleUnit *unit;
    int count;
    int remaining;
    int live;

    count = unitTblGet(side, &table);
    live = 0;
    if (count > 0) {
        cur = table;
        remaining = count;
        do {
            unit = *cur;
            cur++;
            if (unit != 0) {
                if (!(unit->work->flags & ACTOR_FLAG_DEAD)) {
                    live++;
                }
            }
            remaining--;
        } while (remaining != 0);
    }
    return live;
}

/*
 * unitLiveNumGet that also skips units carrying status 0x2000 in category
 * 0 or 4.
 */
int unitLiveNumGet2(int side)
{
    BattleUnit **table;
    BattleUnit *unit;
    int count;
    int live;
    int i;

    live = 0;
    count = unitTblGet(side, &table);
    for (i = 0; i < count; i++) {
        unit = table[i];
        if (unit != 0 && !(unit->work->flags & ACTOR_FLAG_DEAD)) {
            if (calcStatGet(unit,0, 0x2000) == 0
                && calcStatGet(table[i], 4, 0x2000) == 0) {
                live++;
            }
        }
    }
    return live;
}

/*
 * Returns the character id for the unit's CID: 0x14 when calcStatGet category
 * 7 flag 2 is set, 0x15 when flag 1 is set, otherwise the id from dataCidGet.
 */
int unitCidGet(BattleUnit *unit)
{
    int cid;

    cid = dataCidGet(calcUPGet(unit)->charaId);
    if (calcStatGet(unit, 7, 2) != 0) {
        cid = 0x14;
    } else if (calcStatGet(unit, 7, 1) != 0) {
        cid = 0x15;
    }
    return cid;
}

extern int unitAgwsPilotGet(int agwsId);
extern int printf(const char *format, ...);
void unitPlFunc(ObjectTask *task);

/* Original OV01 formatting strings, with their full 32-byte slots retained. */
static const char UnitPlCreateAgwsFormat[32] =
    "unitPlCreate: AGWS (pilot %d)\n";

/* unitCreate and unitLoad are defined later in this file. */
extern BattleUnit *unitCreate(s16 *pl, int side, ObjectTaskCallback func);
void unitLoad(BattleUnit *unit, int mode);

/* Creates a player unit from the placement record pl; AGWS characters also get a linked pilot unit. */
BattleUnit *unitPlCreate(s16 *pl)
{
    BattleUnit *unit;
    BattleUnit *pilotUnit;
    int pilotId;
    s16 pilotPl[2];

    unit = unitCreate(pl, 0, unitPlFunc);
    if (unit != 0) {
        unitTblSet(0, unit);
        unitLoad(unit, pl[0]);
    }
    if (pl[0] >= 0x11) {
        /* AGWS characters (charaId >= 0x11) also get a linked pilot unit,
         * whose own CalcUnitParam is chained through linkedUnit. */
        pilotId = unitAgwsPilotGet(pl[0]);
        if (pilotId != 0) {
            pilotPl[0] = (s16) pilotId;
            pilotPl[1] = 1;
            pilotUnit = unitCreate(pilotPl, 0, unitPlFunc);
            calcUPGet(unit)->linkedUnit = calcUPGet(pilotUnit);
        }
        printf(UnitPlCreateAgwsFormat, pilotId);
    }
    return unit;
}

void unitEnFunc(ObjectTask *task);

BattleUnit *unitEnCreate(s16 *pl)
{
    BattleUnit *unit;

    unit = unitCreate(pl, 1, unitEnFunc);
    if (unit != 0) {
        unitTblSet(1, unit);
        unitLoad(unit, *pl);
    }
    return unit;
}

/*
 * The player-character origin record dataUnitOrgGet (src/ov01/data_unit_org_get.c)
 * returns, as far as unitAgwsPilotGet reads it: the equipped AGWS at +0x54,
 * CalcCharParaData.agwsId in src/ov01/calc.h.
 */
typedef struct UnitOrgAgws {
    unsigned char unmodeled_00[0x54];
    short agwsId; /* +0x54 */
} UnitOrgAgws;

extern UnitOrgAgws *dataUnitOrgGet(int charaId);

/*
 * The AGWS character ids (0x11 to 0x20) and the player character ids that
 * can pilot one (1 to 0x10).
 */
#define UNIT_AGWS_FIRST  0x11
#define UNIT_AGWS_COUNT  0x10
#define UNIT_PILOT_FIRST 1
#define UNIT_PILOT_LAST  0x10

/*
 * The player character whose origin record has AGWS agwsId equipped, or 0
 * when agwsId is not an AGWS or no character pilots it. The parameter is a
 * full int: the original reads $a0 without the sign extension an s16
 * parameter would add.
 */
int unitAgwsPilotGet(int agwsId)
{
    int charaId;

    if (agwsId < UNIT_AGWS_FIRST || agwsId >= UNIT_AGWS_FIRST + UNIT_AGWS_COUNT) {
        return 0;
    }
    for (charaId = UNIT_PILOT_FIRST; charaId <= UNIT_PILOT_LAST; charaId++) {
        if (dataUnitOrgGet(charaId)->agwsId == agwsId) {
            return charaId;
        }
    }
    return 0;
}

extern const char unitCmdCreateIDUAPOText[];
typedef union {
    float f;
    int i;
} UnitFW;

/*
 * Actor.flags written through a union that also carries a pointer member:
 * the store is not assumed to leave the ObjectTask.work pointer unchanged,
 * so the reads of unit->work that follow it reload the pointer (unitCmdDead).
 */
typedef union {
    int i;
    void *p;
} ActorFlagsWord;
extern Actor *ACT_create(int slot, int id);
extern void ACT_initMotion(Actor *actor);
extern void objStdInit(ObjectTask *task);
extern ObjectTask *objEntry(ObjectTaskCallback callback);
void unitActupdate(Actor *actor);
void unitActdraw(Actor *actor);
void unitParaInit(BattleUnit *unit, int charaId, int flag);

/*
 * Creates a unit on side 0 (player) or 1 (enemy) from the placement record pl
 * (pl[0] character id, pl[1] position id). Returns the unit, or 0 when no
 * object or actor could be created.
 */
BattleUnit *unitCreate(s16 *pl, int side, ObjectTaskCallback func)
{
    BattleUnit *unit;
    CalcUnitParam *up;
    Actor *act;
    int charaId;
    int pos;
    int ang;
    int i;
    double zero = 0.0;

    charaId = pl[0];
    unit = (BattleUnit *)objEntry(func);
    if (unit == 0) {
        return 0;
    }
    objStdInit((ObjectTask *)unit);
    unitParaInit(unit, charaId, side);
    up = calcUPGet(unit);
    pos = pl[1];
    ang = 270;
    if (side != 0) {
        ang = 90;
    }
    if (side != 0) {
        pos = -pos;
    }
    up->posId = pos;
    *(float *)((char *)up + 0x4C) = ((float)ang / 180.0f) * 3.1415927f;
    if (side == 0) {
        *(void **)((char *)up + 0x48) = 0;
    } else {
        *(s16 **)((char *)up + 0x48) = pl;
        ((Actor *)*(int *)((char *)unit + 0x10))->flags |= 0x40;
        ((Actor *)*(int *)((char *)unit + 0x10))->flags |= 0x20;
    }
    unitCmdClear(unit);
    act = ACT_create(-1, unitCidGet(unit));
    if (act == 0) {
        return 0;
    }
    ACT_initMotion(act);
    unit->actor = act;
    /* Position from the placement table; the x sign follows the placement id. */
    act->position.x = dataPosTblGet(pl[1])->x;
    act->position.y = dataPosTblGet(pl[1])->y;
    act->position.z = dataPosTblGet(pl[1])->z;
    act->rotation.z = zero;
    act->rotation.x = zero;
    act->rotation.y = *(float *)((char *)up + 0x4C);
    if (pos < 0) {
        act->position.x = -act->position.x;
    }
    if (charaId != 0xB0) {
        *(unsigned char *)((char *)act + 0x90) = 9;   /* shadow_kind */
        act->flags |= 0x20;
        *(unsigned char *)((char *)act + 0x91) = 0x3C; /* shadowSize */
        act->drawEnvFlags |= 0x40;
    }
    if (charaId == 0xAB) {
        act->renderLevel |= 0x80;
        act->cameraCullDistance = 10.0f;
    }
    act->flags |= 0x300;
    act->renderLevel |= 0x200;
    ((Actor *)*(int *)((char *)unit + 0x14))->flags |= 8;
    ((UnitFW *)&act->transparency)->f = zero;
    ((UnitFW *)&act->posSnapX)->f = 0.1f;
    act->motionFrame = 0;
    act->motion = 0;
    /* Clears the five words at +0x100 of the motion actor. */
    i = 0;
loop:
    ((int *)((char *)act + 0x100))[i] = 0;
    i++;
    if (i < 5) {
        goto loop;
    }
    act->update = unitActupdate;
    act->draw = unitActdraw;
    unit->subActor = 0;
    for (i = 0; i < 3; i++) {
        *(int *)((char *)unit + 0x1C + (i & 3) * 4) = 0;
    }
    printf(unitCmdCreateIDUAPOText, charaId, unit, act, calcUPGet(unit), unit->work);
    return unit;
}

/*
 * Disposes the unit's actors: the optional sub-actor, the three weapon slots
 * (indexed `slot & 3` like every other wpnActor access) and finally the
 * motion actor. The weapon slot is reached as pointer arithmetic on the
 * array: the subscript form makes the compiler hoist unit+12 as a loop base
 * (+16 displacement), the original keeps unit itself with +0x1C.
 */
void unitRemoveActor(BattleUnit *unit)
{
    Actor *motionActor;
    Actor *actor;
    int i;

    motionActor = unit->actor;
    actor = unit->subActor;
    if (actor != 0) {
        ACT_dispose(actor);
    }
    for (i = 0; i < 3; i++) {
        actor = *(unit->weapon + (i & 3));
        if (actor != 0) {
            ACT_dispose(actor);
        }
    }
    ACT_dispose(motionActor);
}

typedef struct UnitRemoveEffects {
    struct UnitWork *units[8];
} UnitRemoveEffects;

extern const char unitCmdRemoveUAPOText[];

/*
 * Removes a unit: its actors, then its scheduler object. A plain remove
 * (mode 1) stops there. Otherwise an enemy unit releases its sound and
 * effect slots only when no other living unit shares its character id, and
 * a player unit releases them unconditionally.
 */
void unitRemove(BattleUnit *unit, int mode)
{
    Actor *actor;
    short cid;
    int flag;
    struct UnitWork *effect;
    UnitRemoveEffects effects;
    BattleUnit **table;
    BattleUnit *other;
    int count;
    int i;
    int n;

    actor = unit->actor;
    printf(unitCmdRemoveUAPOText, calcUPGet(unit)->charaId, unit, actor,
           calcUPGet(unit), unit->work);
    unit->actor->flags |= ACTOR_FLAG_HIDDEN;
    cid = calcUPGet(unit)->charaId;
    flag = unit->work->flags & ACTOR_FLAG_ENEMY;
    effect = calcUPGet(unit)->effectUnit;
    effects = *(UnitRemoveEffects *)calcUPGet(unit)->statEffUnits;
    if ((unit->work->flags & ACTOR_FLAG_ENEMY) ||
        (calcUPGet(unit)->charaId >= CALC_SPECIAL_MODEL_CHARACTER_ID_BEGIN && calcUPGet(unit)->charaId < CALC_SPECIAL_MODEL_CHARACTER_ID_END)) {
        calcUPGet(unit)->charaId = 0;
    }
    unitRemoveActor(unit);
    objRemove((ObjectTask *)unit);
    if (mode == 1) {
        return;
    }
    n = 0;
    if (flag) {
        count = unitTblGet(2, &table);
        for (i = 0; i < count; i++) {
            other = table[i];
            if (other != 0 && !(other->work->flags & ACTOR_FLAG_DEAD)) {
                if (calcUPGet(other)->charaId == cid) {
                    n++;
                }
            }
        }
        if (n == 0) {
            sefReleaseID(cid);
            sndSeRegRemove(cid);
            if (effect != 0) {
                sefDeleteEffect2((SchedulerState *)effect);
            }
        }
        for (i = 0; i < 8; i++) {
            if (effects.units[i] != 0) {
                sefDeleteEffect2((SchedulerState *)effects.units[i]);
            }
        }
    } else {
        sefReleaseID(cid);
        sndSeRegRemove(cid);
        if (effect != 0) {
            sefDeleteEffectWait(cid);
        }
    }
}

/*
 * Sets up the player or enemy sound and effect slots for a unit whose character
 * id is outside the special model range, then loads its unit files.
 */
void unitLoad(BattleUnit *unit, int mode)
{
    CalcUnitParam *up;

    up = calcUPGet(unit);
    if (calcUPGet(unit)->charaId < CALC_SPECIAL_MODEL_CHARACTER_ID_BEGIN || calcUPGet(unit)->charaId >= CALC_SPECIAL_MODEL_CHARACTER_ID_END) {
        if (!(unit->work->flags & ACTOR_FLAG_ENEMY)) {
            sefSetupPlayer(up->charaId, up->weaponId[0],
                           up->weaponId[1], up->weaponId[2]);
        } else {
            sefSetupEnemy(up->charaId);
        }
    }
    dataUnitFileLoad(unit, mode);
    dataSndSeRegLoad(unit);
}

void unitActupdate(Actor *actor)
{
    if (actor->flags & 0x8) { /* motion update does not run while set. */
        return;
    }
    if (actor->flags & ACTOR_FLAG_ENEMY) {
        actor->position.y = UnduGet(actor->position.x, actor->position.z);
    }
    actor->motionFrame += 1;
    ACT_updateMotion(actor);
}

void unitActdraw(Actor *actor)
{
    float transparency;

    transparency = actor->transparency;
    if (transparency < 1.0f) {
        nmlModelSetTransparency(transparency);
        nmlModelSetToumei(1);
        nmlModelSetZwrite(1);
    }
}

/*
 * Resets the unit's parameter record: the character id, the wait counters
 * from calcWct, and the three weapon-slot values taken from each equipped
 * weapon whose attachment is set. bcLimit is 3. For flag 0 (the player side)
 * bcValue is 1 when stat 0x1000 of category 7 is set and 0 otherwise; for
 * other flags it is the stored byte unmodeled_1e[3].
 */
void unitParaInit(BattleUnit *unit, int charaId, int flag)
{
    CalcUnitParam *up;
    short *dst;
    short *wp;
    int wct;
    int i;

    i = 2;
    dataUnitInitSet(unit, charaId);
    up = calcUPGet(unit);
    up->charaId = charaId;
    dst = up->wpnSlotValue;
    wp = up->weaponId;
    wct = calcWct(unit);
    up->dblActKind = 0;
    up->waitCount = wct;
    up->waitCountBase = wct;
    up->turnBias = 0;
    do {
        if (*wp != 0 && dst[-3] != 0 && calcAttWpnChk(*wp) != 0) {
            *dst = dataWpnGet(*wp)[0x10];
        }
        i--;
        dst++;
        wp++;
    } while (i >= 0);
    up->bpGauge = 0;
    up->bcLimit = 3;
    if (flag == 0) {
        if (calcStatGet(unit, 7, 0x1000) == 0) {
            up->bcValue = 0;
            return;
        }
        up->bcValue = 1;
    } else {
        up->bcValue = up->unmodeled_1e[3];
    }
}

/*
 * Creates a player unit for each entry of plTbl (three entries, 8 bytes apart)
 * whose leading short is nonzero.
 */
void unitPlInit(short *plTbl)
{
    int count;

    count = 2;
    do {
        if (*plTbl != 0) {
            unitPlCreate(plTbl);
        }
        count -= 1;
        plTbl += 4;
    } while (count >= 0);
}

extern const char unitCmdUnitEnInitENOverText[];

/*
 * Creates an enemy unit for each entry of pl (4 shorts apart) until an entry
 * with a zero leading short, and at most five.
 */
void unitEnInit(s16 *pl)
{
    int count;

    count = 0;
    for (;;) {
        if (pl[0] == 0) {
            break;
        }
        if (count >= 5) {
            printf(unitCmdUnitEnInitENOverText);
            break;
        }
        count++;
        unitEnCreate(pl);
        pl += 4;
    }
}

/*
 * Sets the motion actor's clipping bit 0x200 for a unit whose character is
 * 0xB4 or whose work actor is hidden, and clears it otherwise.
 */
void unitClipSet(BattleUnit *unit)
{
    Actor *motionActor;

    motionActor = unit->actor;
    if (calcUPGet(unit)->charaId == 0xB4) {
        motionActor->drawEnvFlags |= 0x200;
    } else if (unit->work->flags & ACTOR_FLAG_HIDDEN) {
        motionActor->drawEnvFlags |= 0x200;
    } else {
        motionActor->drawEnvFlags &= ~0x200;
    }
}

void unitPlFunc(ObjectTask *task)
{
    BattleUnit *unit = (BattleUnit *)task;

    unitCmdExec(unit);
    statEffProc(unit);
    unitClipSet(unit);
}

void unitEnFunc(ObjectTask *task)
{
    BattleUnit *unit = (BattleUnit *)task;

    unitCmdExec(unit);
    statEffProc(unit);
    unitClipSet(unit);
}

/* Defined in src/ov01/obj.c; unitCmdClear passes its unit argument straight through to it. */
extern void objCmdClear(ObjectTask *task);

/*
 * unitCmdClear clears the shared object command queue, then nulls out the
 * unit's Actor.update callback.
 */
void unitCmdClear(BattleUnit *unit)
{
    Actor *actor;

    objCmdClear((ObjectTask *)unit);
    actor = unit->work;
    actor->update = 0;
}

void *unitCmdTailGet(BattleUnit *unit)
{
    return objCmdTailGet((ObjectTask *)unit);
}

void *unitCmdPtrGet(BattleUnit *unit)
{
    return objCmdPtrGet((ObjectTask *)unit);
}

int unitCmdNext(BattleUnit *unit)
{
    return objCmdNext((ObjectTask *)unit);
}

/*
 * Pushes cmd onto the unit's command queue and resets its run counter and move
 * state. A nonzero selector marks the command pending.
 */
void unitCmdSet(BattleUnit *unit, CmdEntry *cmd)
{
    Actor *actor;
    CmdEntry *entry;

    actor = unit->work;
    entry = objCmdPush((ObjectTask *)unit);
    actor->runFrameCounter = 0;
    actor->moveState = 0;
    *entry = *cmd;
    if (entry->selector != 0) {
        actor->flags |= ACTOR_FLAG_CMD_PENDING;
    }
}

/*
 * Runs the handler for the unit's current command; the run frame counter
 * advances while the handler returns 0.
 */
void unitCmdExec(BattleUnit *unit)
{
    Actor *actor;
    CmdEntry *cmd;

    actor = unit->work;
    cmd = unitCmdPtrGet(unit);
    if (actor->runFrameCounter == 0 && cmd->selector != 0 && cmd->selector != 7) {
        debugCmdPrint(unit, cmd->selector);
    }
    if (unitCmdTbl[cmd->selector](unit) == 0) {
        actor->runFrameCounter++;
    }
}

int unitCmdFree(BattleUnit *unit)
{
    Actor *actor;

    actor = unit->work;
    actor->flags &= ~ACTOR_FLAG_CMD_PENDING;
    return 0;
}

/*
 * Runs the attack once, while the action's move state is still 0: the
 * request takes the technique and the target from the queued entry and the
 * work actor, runs the attack, then pays the technique's ep and bp cost, and
 * the special-ability counter of the attacker's spec record advances.
 */
int unitCmdAtk(BattleUnit *unit)
{
    UnitActRequest request;
    Actor *actor;
    CmdEntry *cmd;
    BattleUnit *target;
    unsigned short *spec;
    int value;

    actor = unit->work;
    cmd = unitCmdPtrGet(unit);
    if (actor->moveState == 0) {
        value = cmd->argument;
        /* The word is read as an int: a pointer-typed read schedules the argument stores differently. */
        target = (BattleUnit *)*(int *)&unit->work->targetUnit;
        request.id = value;
        request.target = target;
        value = cmd->itmId;
        request.mode = 1;
        request.itmId = value;
        unitCmdAtkSub(unit, &request);
        calcEp(unit, -calcTakeEpGet(unit, request.id));
        calcBp2Tid(unit, request.id);
        spec = dataSpecDataGet(calcUPGet(unit)->charaId, request.id);
        if (spec != 0) {
            spec[4] = spec[4] + 1;
        }
        actor->moveState++;
    }
    return 0;
}

extern const char unitCmdJobTypeErrTidText[];

/*
 * The error report is wrapped the way a debug-print macro wraps it, in a
 * do/while (0) block: the original calls printf with jal and returns
 * normally, while a bare trailing printf call is turned into a sibling call.
 * The two range tests stay nested: the original branches on each one (lb),
 * where a single `&&` folds them into one unsigned compare.
 */
#define UNIT_ERROR_PRINT(args) do { printf args; } while (0)

/*
 * Starts the attack for technique request->id: a valid job type goes to
 * unitJobTypeAtk, and an unknown job type prints an error.
 */
void unitCmdAtkSub(BattleUnit *unit, UnitActRequest *request)
{
    TecJobView *tec;

    scenarioBatInit();
    tec = dataTecGet(request->id);
    if (tec->jobType < 8) {
        if (tec->jobType >= 0) {
            unitJobTypeAtk(unit, request, tec);
            return;
        }
    }
    UNIT_ERROR_PRINT((unitCmdJobTypeErrTidText, tec->jobType, request->id));
}

/*
 * The hit record calcHitDmg fills for one target: hit mode and side in
 * `result`, then the damage and the status clear/set words, as CalcHitInfo
 * lays them out in src/ov01/calc.c.
 */
typedef struct UnitJtaHit {
    int result;        /* +0x00: hit mode in the low byte, side above it */
    int damage;        /* +0x04 */
    int unk08;         /* +0x08: unmodeled by CalcHitInfo */
    int setCategory;   /* +0x0C */
    int setMask;       /* +0x10 */
    int clearCategory; /* +0x14 */
    int clearMask;     /* +0x18 */
} UnitJtaHit;

/* The source step that scenarioSrcSet receives. */
typedef struct UnitJtaStep {
    BattleUnit *unit; /* +0x00 */
    int mode;         /* +0x04: request->mode */
    int id;           /* +0x08: request->id */
    int itmId;        /* +0x0C: request->itmId */
    int stride;       /* +0x10: the number of targets visited */
} UnitJtaStep;

/* One scenario destination record, appended by scenarioDstSet. */
typedef struct UnitJtaDst {
    BattleUnit *unit;      /* +0x00: the target being hit */
    int kind;              /* +0x04: the animation/effect kind */
    int damage;            /* +0x08 */
    int hit;               /* +0x0C: the hit mode/side word */
    int unk08;             /* +0x10 */
    unsigned char unmodeled_14[4];
    int setCategory;       /* +0x18 */
    int setMask;           /* +0x1C */
    int clearCategory;     /* +0x20 */
    int clearMask;         /* +0x24 */
} UnitJtaDst;

extern int calcHitDmg(BattleUnit *user, UnitActRequest *cmd, UnitJtaHit *hit);
extern int calcPlusAglGet(BattleUnit *unit, int tecId);
extern BattleUnit *tgtFindNext(void);
extern int scenarioSrcSet(UnitJtaStep *source);
extern int scenarioDstSet(UnitJtaDst *entry);

/*
 * Runs an attack over the chain of targets from tgtFindNext. Each target gets
 * a destination record from calcHitDmg (job type 7 uses a fixed record) passed
 * to scenarioDstSet; when no target is left the source step is stored and the
 * unit's turn bias grows by calcPlusAglGet.
 */
int unitJobTypeAtk(BattleUnit *unit, UnitActRequest *request, TecJobView *tec)
{
    UnitJtaStep step;
    UnitJtaDst rec;
    UnitJtaHit out;
    CalcUnitParam *up;
    int stride = 0;
    int kind;
    int hitKind;
    int hitMode;
    int hitSide;
    int p;
    int t;
    BattleUnit *nextTarget;

    t = tec->jobType;
    for (;;) {
        if (t != 7) {
            calcHitDmg(unit, request, &out);
            hitKind = out.result;
            hitMode = hitKind & 0xFF;
            hitSide = (hitKind >> 8) & 0xFF;
            /* Effect kind for each hit mode. */
            switch (hitMode) {
            case 0:
                kind = 0xA;
                break;
            case 1:
            case 3:
            case 7:
            case 8:
            case 9:
            case 10:
                kind = (hitSide & 2) ? 0xB : 9;
                break;
            case 2:
            case 4:
                kind = (hitSide & 8) ? 0x28 : 0x27;
                break;
            case 5:
            case 6:
                kind = 0x29;
                break;
            default:
                printf("** Hit Type err JTYPE_ATK -> %d\n", hitMode);
                return 0;
            }
        } else {
            kind = 0;
            memset(&out, 0, 0x1C);
            hitKind = 0xB;
            out.result = 0xB;
        }
        stride += 1;
        rec.unit = request->target;
        rec.kind = kind;
        rec.damage = out.damage;
        rec.hit = hitKind;
        rec.unk08 = out.unk08;
        rec.setCategory = out.setCategory;
        rec.setMask = out.setMask;
        rec.clearCategory = out.clearCategory;
        rec.clearMask = out.clearMask;
        scenarioDstSet(&rec);
        *(BattleUnit **)((char *)calcUPGet((BattleUnit *)*(int *)&request->target) + 0x150) = unit;
        *(int *)((char *)calcUPGet((BattleUnit *)*(int *)&request->target) + 0x154) = request->id;
        nextTarget = tgtFindNext();
        request->target = nextTarget;
        if (nextTarget == 0) {
            step.unit = unit;
            step.mode = request->mode;
            step.id = request->id;
            step.itmId = request->itmId;
            step.stride = stride;
            scenarioSrcSet(&step);
            up = calcUPGet(unit);
            p = calcPlusAglGet(unit, request->id);
            up->turnBias = (short)(up->turnBias + p);
            return 0;
        }
    }
}

/*
 * Ends an attack: clears the command queue, then either queues command 0xD
 * with weapon motions (when the current motion is 0x20) or plays motion 1.
 * Face motion 0x35 is set last.
 */
void unitCmdAtkEnd(BattleUnit *unit)
{
    CmdEntry cmd;

    unitCmdClear(unit);
    if (unitMotGet(unit) == 0x20) {
        cmd.selector = 0xD;
        cmd.argument = 0x21;
        cmd.extra = 0;
        unitCmdSet(unit, &cmd);
        unitWpnMotSet(unit, dataDefWpnGet(unit), 0, 0x32);
        if (calcUPGet(unit)->charaId == 7) {
            unitWpnMotSet(unit, 1, 0, 0x32);
        }
    } else {
        unitMotSet(unit, 1);
    }
    unitFaceMotSet(unit, 0x35);
}

/* Queues the dead command (selector 0xC) when the unit's current HP is zero. Returns 1 if it was queued. */
int unitCmdDeadSet(BattleUnit *unit)
{
    CmdEntry cmd;

    if (unit == 0 || (unit->work->flags & ACTOR_FLAG_DEAD)) {
        return 0;
    }
    if (*(short *) &calcUPGet(unit)->currentHp != 0) {
        return 0;
    }
    cmd.selector = 0xC;
    unitCmdSet(unit, &cmd);
    return 1;
}

/*
 * The ether record's halfword at +2 carries the 0x2000 bit that unitCmdEther
 * tests; the technique id sits at +6 (dataEthGet, data_unit_org_get).
 */
typedef struct EtherRecord {
    unsigned char unmodeled_00[2];
    unsigned short flags;
    unsigned char unmodeled_04[2];
    short tecId;
} EtherRecord;

typedef struct EtherView {
    unsigned char unmodeled_00[0x8E];
    short etherId[12];
} EtherView;

extern EtherRecord *dataEthGet(int etherId);
extern void calcEtherDisableSet(BattleUnit *unit, int bit);

/*
 * Runs the ether command once, while the action's move state is still 0: the
 * attack request runs in mode 2, the technique's ep cost is paid, and the
 * equipped slot holding the requested ether is disabled. Skips to the next
 * command when stat 0x1000 of category 1 is set.
 */
int unitCmdEther(BattleUnit *unit)
{
    UnitActRequest request;
    Actor *actor;
    CmdEntry *cmd;
    int i;
    int value;
    BattleUnit *target;
    int itmId;

    actor = unit->work;
    cmd = unitCmdPtrGet(unit);
    if (calcStatGet(unit, 1, 0x1000) != 0) {
        return unitCmdNext(unit);
    }
    if (actor->moveState == 0) {
        target = (BattleUnit *)*(int *)&((Actor *)*(int *)&unit->work)->targetUnit;
        value = cmd->argument;
        request.id = value;
        request.target = target;
        itmId = cmd->itmId;
        request.mode = 2;
        request.itmId = itmId;
        unitCmdAtkSub(unit, &request);
        calcEp(unit, -calcTakeEpGet(unit, request.id));
        for (i = 0; i < 12; i++) {
            if (dataEthGet(request.itmId)->flags & 0x2000) {
                if (((EtherView *)calcUPGet(unit))->etherId[i] == request.itmId) {
                    calcEtherDisableSet(unit, i);
                    break;
                }
            }
        }
        actor->moveState++;
    }
    return 0;
}

/*
 * Runs the item command once, while the action's move state is still 0:
 * the queued entry supplies the argument and item id, the work actor
 * supplies the target unit, and the item sub-routine runs with mode 3.
 */
int unitCmdItem(BattleUnit *unit)
{
    ItemCmd buf;
    Actor *actor;
    CmdEntry *cmd;
    int value;

    actor = unit->work;
    cmd = unitCmdPtrGet(unit);
    if (actor->moveState == 0) {
        value = cmd->argument;
        buf.argument = value;
        buf.target = (int)unit->work->targetUnit;
        value = cmd->itmId;
        buf.itmId = value;
        buf.mode = 3;
        unitCmdItemSub(unit, &buf);
        actor->moveState++;
    }
    return 0;
}

extern int calcHitItem(BattleUnit *unit, ItemCmd *cmd, UnitJtaHit *hit);
extern int calcDmgItem(BattleUnit *unit, ItemCmd *cmd);

/*
 * Applies an item command to each target in turn: calcHitItem picks the hit
 * kind, scenarioDstSet records the target, and scenarioSrcSet stores the
 * source step once no targets remain.
 */
void unitCmdItemSub(BattleUnit *unit, ItemCmd *buf)
{
    UnitJtaStep src;
    UnitJtaDst dst;
    UnitJtaHit out;
    int count;
    int hit;
    int kind;
    int damage;
    int t;
    int b2;
    int neg;
    ItemCmd *cb;
    int *po;

    count = 0;
    scenarioBatInit();
    for (;;) {
        hit = calcHitItem(unit, buf, &out);
        t = hit & 0xFF;
        b2 = (hit >> 8) & 0xFF;
        /* Hit types 1 and 3 store the damage as its absolute value; 2 and 4 keep it signed; 5 and 6 deal none. */
        switch (t) {
        case 1:
        case 3:
            kind = 9;
            damage = calcDmgItem(unit, buf);
            out.unk08 = 0;
            neg = -damage;
            damage = (damage < 0) ? neg : damage;
            break;
        case 2:
        case 4:
            damage = calcDmgItem(unit, buf);
            kind = (b2 & 8) ? 0x28 : 0x27;
            break;
        case 5:
        case 6:
            damage = 0;
            kind = 0x29;
            break;
        default:
            printf("** Hit Type err -> %d\n", t);
            return;
        }
        /*
         * The destination words are stored through a pointer-typed view:
         * the original's record is written word by word through void *, and
         * that alias class is what lets the out reads below schedule ahead
         * of the stores (the record fill is otherwise reordered differently).
         */
        ((void **)&dst)[1] = (void *)kind;
        ((void **)&dst)[0] = (void *)buf->target;
        ((void **)&dst)[2] = (void *)damage;
        ((void **)&dst)[3] = (void *)hit;
        po = (int *)&out;
        ((void **)&dst)[4] = (void *)*(int *)((char *)po + 8);
        ((void **)&dst)[6] = (void *)*(int *)((char *)po + 12);
        ((void **)&dst)[7] = (void *)*(int *)((char *)po + 16);
        ((void **)&dst)[8] = (void *)*(int *)((char *)po + 20);
        ((void **)&dst)[9] = (void *)*(int *)((char *)po + 24);
        count++;
        scenarioDstSet(&dst);
        *(BattleUnit **)((char *)calcUPGet((BattleUnit *)*(int *)&buf->target) + 0x150) = unit;
        buf->target = (int)tgtFindNext();
        if (buf->target != 0) {
            continue;
        }
        break;
    }
    src.unit = unit;
    src.mode = buf->mode;
    src.id = buf->argument;
    src.itmId = buf->itmId;
    src.stride = count;
    scenarioSrcSet(&src);
}

/*
 * Moves the unit to its target: state 0 sets the destination and, unless the
 * move warps, the facing; state 1 jumps or warps, then calls unitCmdMoveNext.
 * Warping applies to AGWS player units and to enemies with moveKindId 4.
 */
int unitCmdMove(BattleUnit *unit)
{
    Actor *actor;
    CalcUnitParam *calcParam;
    int useWarp;

    actor = unit->work;
    unitCmdPtrGet(unit);
    if (!(*(u32 *)unit->work & ACTOR_FLAG_ENEMY)) {
        calcParam = calcUPGet(unit);
        useWarp = calcParam->flags & CALC_UNIT_FLAG_IS_AGWS;
    } else {
        calcParam = calcUPGet(unit);
        useWarp = calcParam->moveKindId == 4;
    }
    switch (actor->moveState) {
    case 0:
        unitCmdDstSet(unit, 3, 1);
        if (!useWarp) {
            unitCmdDirSet(unit);
        }
        calcUPGet(unit);
        unitCmdVASet(unit, 3, 0);
        actor->movePhase = 0;
        actor->moveState++;
        break;
    case 1:
        if (!useWarp) {
            if (unitCmdJump(unit, 3, 1, 1) != 0) {
                unitCmdMoveNext(unit, 1);
            }
        } else if (unitCmdWarp(unit, 3, 1) != 0) {
            unitCmdMoveNext(unit, 1);
        }
        break;
    }
    return 0;
}

/*
 * Guard: plays motion 0x1B and marks the unit as guarding
 * (ACTOR_FLAG_GUARD), then stands the unit once the motion ends.
 */
int unitCmdGuard(BattleUnit *unit)
{
    Actor *actor;

    actor = unit->work;
    unitCmdPtrGet(unit);
    if (actor->moveState == 0) {
        unitMotSet(unit, 0x1B);
        /* The work slot is loaded again after the call, not read through the
         * cached `actor`. */
        unit->work->flags |= ACTOR_FLAG_GUARD;
        actor->moveState++;
    } else if (unit->actor->moveFlags & MOVE_FLAG_REACHED) {
        unitMotStandSet(unit);
        return unitCmdNext(unit);
    }
    return 0;
}

/*
 * Queues the AGWS boarding pair: command 0x2B (with the queued argument and
 * the unit's character id: the linked unit's when CALC_UNIT_FLAG_IS_AGWS is
 * set, else baseCharaId) and then command 0x2D from the same entry.
 */
int unitCmdAgws(BattleUnit *unit)
{
    CmdEntry entry;
    CmdEntry *cmd;
    CalcUnitParam *param;
    int charaId;
    int effectId;
    int soundId;

    cmd = unitCmdPtrGet(unit);
    param = calcUPGet(unit);
    if (param->flags & CALC_UNIT_FLAG_IS_AGWS) {
        charaId = param->linkedUnit->charaId;
    } else {
        charaId = param->baseCharaId;
    }
    calcStatResetAll(unit);
    entry.selector = 0x2B;
    entry.argument = cmd->argument;
    entry.extra = charaId;
    unitCmdSet(unit, &entry);
    entry.selector = 0x2D;
    unitCmdSet(unit, &entry);
    if (calcUPGet(unit)->flags & CALC_UNIT_FLAG_IS_AGWS) {
        effectId = 0x67;
        soundId = 0x20;
    } else {
        /* Both values are assigned around the sndSeTransPlay call, as in the original. */
        effectId = 0x66;
        soundId = 0x1F;
        sndSeTransPlay(unit, 0x17, 3);
    }
    decoEffCall(unit, effectId);
    sndSysSePlay(soundId);
    return unitCmdNext(unit);
}

/* Defined in src/ov01/data_unit_org_get.c. */
extern void dataFileInfoInitChg(BattleUnit *unit, s16 charaId);

/*
 * Replaces the unit with a new unit of another character, built from the
 * record in the command. The old unit's remaining commands are copied to the
 * new unit, which is linked to the old one and takes over its gauge values,
 * then queues a change-in command (0x2A) and the saved commands.
 */
int unitCmdUnitChange(BattleUnit *unit)
{
    Actor *actor;
    CmdEntry *cmd;
    CmdEntry sp;
    u16 hdr[4];
    CmdEntry list[5];
    int n;
    int wflags;
    void *spA0;
    BattleUnit *savedTarget;
    int hiddenFlag;
    CalcUnitParam *up;
    BattleUnit *nu;
    CalcUnitParam *np;
    int removeMode;
    int k;
    int cdSync;
    signed char savedBcValue;
    signed char savedBpGauge;
    s16 cid;
    s16 ep;
    CmdEntry *lp;
    int playerSide;
    u16 h0;
    u16 h1;

    actor = unit->work;
    cmd = unitCmdPtrGet(unit);
    switch (actor->moveState) {
    case 0:
        actor->moveState = 1;
        return 0;
    case 1:
        if (unitTelOut(unit) != 0) {
            actor->moveState++;
        }
        return 0;
    case 2:
        up = calcUPGet(unit);
        n = 0;
        wflags = unit->work->flags;
        h0 = ((u16 *)cmd)[4];
        lp = list - 1;
        spA0 = *(void **)((char *)up + 0x48);
        h1 = ((u16 *)cmd)[2];
        hiddenFlag = wflags & 8;
        playerSide = ((wflags >> 6) ^ 1) & 1;
        hdr[0] = h0;
        hdr[1] = h1;
        hdr[2] = 0;
        hdr[3] = 0;
        /* Copies the old unit's queued commands into list. */
        while (unitCmdNext(unit) != 0) {
            lp++;
            *lp = *(CmdEntry *)unitCmdPtrGet(unit);
            n++;
        }
        cid = up->charaId;
        ep = up->currentEp;
        savedTarget = unit->work->targetUnit;
        savedBcValue = up->bcValue;
        savedBpGauge = up->bpGauge;
        dataFileInfoInitChg(unit, cid);
        if ((s16)hdr[0] == 6 && (calcStatGet(unit, 7, 2) != 0 || calcStatGet(unit, 7, 1) != 0)) {
            removeMode = 1;
        } else {
            removeMode = 0;
        }
        unitRemove(unit, removeMode);
        if (playerSide) {
            nu = unitCreate((s16 *)hdr, 0, unitPlFunc);
        } else {
            nu = unitCreate((s16 *)hdr, 1, unitEnFunc);
        }
        cdSync = dataCdSync();
        unitLoad(nu, cid);
        unitTblChg(unit, nu);
        if (calcUPGet(nu)->flags & CALC_UNIT_FLAG_IS_AGWS) {
            calcUPGet(nu)->linkedUnit = up;
            calcUPGet(nu)->currentEp = ep;
            calcUPGet(nu)->bcValue = savedBcValue;
            calcUPGet(nu)->bpGauge = savedBpGauge;
        } else if (!(nu->work->flags & ACTOR_FLAG_ENEMY) && !(calcUPGet(nu)->flags & CALC_UNIT_FLAG_IS_AGWS)
                   && !(calcUPGet(nu)->charaId >= CALC_SPECIAL_MODEL_CHARACTER_ID_BEGIN && calcUPGet(nu)->charaId < CALC_SPECIAL_MODEL_CHARACTER_ID_END)) {
            calcUPGet(nu)->linkedUnit = up;
            calcUPGet(nu)->currentEp = ep;
            calcUPGet(nu)->bcValue = savedBcValue;
            calcUPGet(nu)->bpGauge = savedBpGauge;
        } else if (calcUPGet(nu)->charaId >= CALC_SPECIAL_MODEL_CHARACTER_ID_BEGIN && calcUPGet(nu)->charaId < CALC_SPECIAL_MODEL_CHARACTER_ID_END) {
            calcUPGet(nu)->linkedUnit = up;
            calcUPGet(nu)->bcValue = savedBcValue;
            calcUPGet(nu)->bpGauge = savedBpGauge;
        }
        /* Carries the hidden state over, then queues the change-in command and the saved commands. */
        nu->work->flags |= hiddenFlag;
        np = calcUPGet(nu);
        sp.selector = 0x2A;
        *(void **)((char *)np + 0x48) = spA0;
        sp.argument = cdSync;
        unitCmdSet(nu, &sp);
        for (k = 0; k < n; k++) {
            sp = list[k];
            unitCmdSet(nu, &sp);
        }
        nu->work->targetUnit = savedTarget;
        return 1;
    }
    return 0;
}

int unitCmdStand(BattleUnit *unit)
{
    Actor *actor;

    actor = unit->work;
    if (actor->moveState == 0) {
        unitMotStandSet(unit);
        actor->flags &= ~ACTOR_FLAG_CMD_PENDING;
        actor->moveState++;
    }
    return unitCmdNext(unit);
}

int unitCmdEnd(BattleUnit *unit)
{
    return unitCmdStand(unit);
}

/*
 * Damage reaction: the queued argument picks the hit motion and face (1 uses
 * motion 5 and 2 uses motion 0x26, both with face 0x3B; anything else uses
 * motion 4 and face 0x3A). A kind-2 hit on a front position (posId < 4) adds 3
 * to CalcUnitParam.posId and snaps the motion actor onto that position.
 */
int unitCmdDmg(BattleUnit *unit)
{
    Actor *actor;
    CmdEntry *cmd;
    int kind;
    int motion;
    int face;
    CalcUnitParam *param;
    Vector4 *pos;
    Actor *motionActor;

    actor = unit->work;
    cmd = unitCmdPtrGet(unit);
    kind = cmd->argument;
    if (actor->moveState == 0) {
        switch (kind) {
        case 1:
            motion = 5;
            face = 0x3B;
            break;
        case 2:
            motion = 0x26;
            face = 0x3B;
            break;
        case 0: /* Kept as its own case, separate from the default. */
        default:
            motion = 4;
            face = 0x3A;
            break;
        }
        unitMotSet(unit, motion);
        unitFaceMotSet(unit, face);
        sndSeTransPlay(unit, 2, kind + 1);
        actor->moveState++;
    } else if (unit->actor->moveFlags & MOVE_FLAG_REACHED) {
        if (kind == 2 && calcUPGet(unit)->posId < 4) {
            param = calcUPGet(unit);
            param->posId += 3;
            pos = dataPosTblGet(param->posId);
            motionActor = unit->actor;
            motionActor->position.x = pos->x;
            motionActor->position.y = pos->y;
            motionActor->position.z = pos->z;
        }
        unitMotStandSet(unit);
        if (calcStatGet(unit, 1, 0x2000) != 0) {
            unitFaceMotSet(unit, 0x37);
        } else {
            unitFaceMotSet(unit, 0x35);
        }
        return unitCmdNext(unit);
    }
    return 0;
}

/* Evasion: plays motion 0x1A, then stands the unit once that motion reaches its end. */
int unitCmdEva(BattleUnit *unit)
{
    Actor *actor;
    Actor *motionActor;

    actor = unit->work;
    if (actor->moveState == 0) {
        unitMotSet(unit, 0x1A);
        actor->moveState++;
    } else {
        motionActor = unit->actor;
        if (motionActor->moveFlags & MOVE_FLAG_REACHED) {
            unitMotStandSet(unit);
            return unitCmdNext(unit);
        }
    }
    return 0;
}

/*
 * Defence: plays motion 0x1E, then, once it ends, motion 0x1C unless the
 * unit is guarding (ACTOR_FLAG_GUARD), and stands the unit when the last
 * motion ends.
 */
int unitCmdDef(BattleUnit *unit)
{
    Actor *actor;

    actor = unit->work;
    switch (actor->moveState) {
    case 0:
        unitMotSet(unit, 0x1E);
        actor->moveState++;
        break;
    case 1:
        if (unit->actor->moveFlags & MOVE_FLAG_REACHED) {
            if (!(actor->flags & ACTOR_FLAG_GUARD)) {
                unitMotSet(unit, 0x1C);
            }
            actor->moveState++;
        }
        break;
    default:
        if (unit->actor->moveFlags & MOVE_FLAG_REACHED) {
            unitMotStandSet(unit);
            return unitCmdNext(unit);
        }
        break;
    }
    return 0;
}

extern void calcTurnEnd(BattleUnit *unit);
extern int unitDeadSpecChk(BattleUnit *unit);

/*
 * Death sequence over several move states: the death motion and sound, a
 * reset of the stat masks that keeps three bits, and for enemies the removal
 * from the unit table once the death spec check passes.
 */
int unitCmdDead(BattleUnit *unit)
{
    Actor *actor;
    int i;
    int statKeep2;
    int statKeep3;
    int statKeep7;

    actor = unit->work;
    switch (actor->moveState) {
    case 0:
        if (actor->flags & ACTOR_FLAG_ENEMY) {
            ((ActorFlagsWord *)actor)->i = actor->flags | ACTOR_FLAG_DEAD;
        }
        if (unit->work->flags & ACTOR_FLAG_GUARD) {
            unitMotSet(unit, 0x1C);
            actor->moveState++;
        } else {
            actor->moveState = 2;
        }
        break;
    case 1:
        if (unit->actor->moveFlags & MOVE_FLAG_REACHED) {
            actor->moveState++;
        }
        break;
    case 2:
        unitMotSet(unit, 6);
        calcTurnEnd(unit);
        if (unit->work->flags & ACTOR_FLAG_ENEMY) {
            int se;

            if (calcUPGet(unit)->flags & 0x1000) {
                se = 0x23;
            } else {
                se = (calcUPGet(unit)->flags & 0x2000) ? 0x22 : 0x21;
            }
            sndSysSePlay(se);
        }
        sndSeTransPlay(unit, 2, 4);
        actor->moveState++;
        break;
    case 3:
        if (unit->actor->moveFlags & MOVE_FLAG_REACHED) {
            actor->movePhaseTimer = 0;
            actor->moveState++;
        }
        break;
    case 4:
        statKeep2 = calcUPGet(unit)->statActiveMask[2] & 0x4000;
        statKeep3 = calcUPGet(unit)->statActiveMask[3] & 0x800;
        statKeep7 = calcUPGet(unit)->statActiveMask[7] & 7;
        /* Clears every stat mask, then restores the three kept bits. */
        for (i = 0; i < 8; i++) {
            calcUPGet(unit)->statActiveMask[i] = 0;
        }
        calcUPGet(unit)->statActiveMask[2] |= statKeep2;
        calcUPGet(unit)->statActiveMask[3] |= statKeep3;
        calcUPGet(unit)->statActiveMask[7] |= statKeep7;
        if (unit->work->flags & ACTOR_FLAG_ENEMY) {
            if (unitDeadSpecChk(unit) != 0) {
                if (unitTelOut(unit) != 0) {
                    actor->moveState++;
                }
            } else {
                actor->moveState++;
            }
        } else {
            actor->moveState++;
        }
        break;
    case 5:
        ((ActorFlagsWord *)actor)->i = (actor->flags & 0x61) | 0x80;
        if (unit->work->flags & ACTOR_FLAG_ENEMY) {
            unitTblRemove(unit);
            if (unitDeadSpecChk(unit) != 0) {
                unitRemove(unit, 0);
            }
        } else {
            actor->moveState++;
        }
        break;
    }
    return 0;
}

/* Returns 0 for the character ids listed in the switch and 1 for every other unit. */
int unitDeadSpecChk(BattleUnit *unit)
{
    short charaId;

    charaId = calcUPGet(unit)->charaId;
    switch (charaId) {
    case 0xA2:
    case 0xAA:
    case 0xAB:
    case 0xB0:
    case 0xB2:
    case 0xB3:
    case 0xB4:
    case 0xB7:
        return 0;
    default:
        return 1;
    }
}

/*
 * Plays the queued motion (argument), then waits either for it to end
 * (extra == 0: the motion actor's MOVE_FLAG_REACHED) or for extra frames
 * of Actor.runFrameCounter.
 */
int unitCmdMot(BattleUnit *unit)
{
    Actor *actor;
    CmdEntry *cmd;

    actor = unit->work;
    cmd = unitCmdPtrGet(unit);
    if (actor->moveState == 0) {
        unitMotSet(unit, cmd->argument);
        actor->moveState++;
    } else if (cmd->extra == 0 ? (unit->actor->moveFlags & MOVE_FLAG_REACHED) != 0
                               : cmd->extra == actor->runFrameCounter) {
        unitMotStandSet(unit);
        return unitCmdNext(unit);
    }
    unitMtdProc(unit);
    return 0;
}

extern const char unitCmdTgtSetErrText[];

/*
 * Sets the target from the queued command's technique, target, item and mode.
 * When no target is found, prints an error and cancels the queued commands.
 */
int unitCmdTgtSet(BattleUnit *unit)
{
    Actor *actor;
    CmdEntry *cmd;
    TgtCmd tgtCmd;
    BattleUnit *target;

    actor = unit->work;
    cmd = unitCmdPtrGet(unit);
    if (actor->moveState != 0) {
        return 0;
    }
    tgtCmd.tecId = cmd->argument;
    tgtCmd.target = (BattleUnit *) cmd->extra;
    tgtCmd.itmId = cmd->itmId;
    tgtCmd.mode = cmd->mode;
    target = tgtFindFirst(unit, &tgtCmd);
    if (target != 0) {
        /* The work slot is loaded again here, not read through the cached
         * `actor`, as in the original. */
        unit->work->targetUnit = target;
        return unitCmdNext(unit);
    }
    printf(unitCmdTgtSetErrText);
    cmdListCansel(unit);
    unitCmdClear(unit);
    return unitCmdNext(unit);
}

/*
 * Turns the unit toward its target. Move state 0 picks the target point: the
 * target unit for cmd[0] 15, a position-table entry for 16, otherwise the
 * average of the living units on the other side. unitCmdTurnSub then rotates
 * the unit, and the command ends when that turn is done.
 */
void unitCmdTurn(BattleUnit *unit)
{
    Actor *actor;
    Actor *w;
    int *cmd;
    Vector4 *tgt;
    struct { BattleUnit **p; } tb;
    BattleUnit *u;
    Actor *m;
    int count;
    int i;
    int n;
    int idx;
    float f;

    actor = unit->work;
    cmd = unitCmdPtrGet(unit);
    w = unit->work;
    tgt = (Vector4 *)&w->motionFrame;
    if (actor->moveState == 0) {
        if (cmd[0] == 15) {
            if (unit == w->targetUnit) {
                unitCmdNext(unit);
                return;
            }
            m = w->targetUnit->actor;
            idx = 0;
            tgt->x = m->position.x;
            tgt->z = m->position.z;
        } else if (cmd[0] == 16) {
            idx = cmd[1];
            tgt->x = dataPosTblGet(idx)->x;
            tgt->z = dataPosTblGet(idx)->z;
        } else {
            n = 0;
            idx = 0;
            tgt->x = tgt->z = 0.0f;
            count = unitTblGet((((int)w->flags >> 6) ^ 1) & 1, &tb.p);
            for (i = 0; i < count; i++) {
                u = tb.p[i];
                if (u != 0) {
                    n++;
                    tgt->x += u->actor->position.x;
                    tgt->z += u->actor->position.z;
                }
            }
            if (n > 0) {
                f = n;
                tgt->x = tgt->x / f;
                tgt->z = tgt->z / f;
            }
        }
        if (idx < 0) {
            tgt->x = -tgt->x;
        }
        actor->movePhaseTimer = 0;
        actor->moveState++;
    }
    if (unitCmdTurnSub(unit, 0) != 0) {
        unitCmdNext(unit);
    }
}

extern void unitSetInterp(BattleUnit *unit);
extern void unitSetInterpTime(BattleUnit *unit, float time);

/*
 * Rotates the unit's yaw toward its turn target, at a rate set by the size
 * class; mode 0 also sets the interpolation time and the turn motion. Returns
 * 1 when the turn is complete.
 */
int unitCmdTurnSub(BattleUnit *unit, int mode)
{
    Actor *actor;
    Actor *m;
    CalcUnitParam *param;
    Vector4 *dst;
    Vector4 *tgt;
    float rate;
    float d;
    float nd;
    int done;

    done = 0;
    rate = 0.0f;
    actor = unit->work;
    m = unit->actor;
    param = calcUPGet(unit);
    tgt = (Vector4 *)&unit->work->motionFrame;
    dst = (Vector4 *)((char *)unit->work + 0xB0);
    if (actor->movePhaseTimer == 0) {
        if (tgt->x == m->position.x && tgt->z == m->position.z) {
            dst->y = *(float *)((char *)param + 0x4C);
        } else {
            dst->y = atan2f(tgt->x - m->position.x, tgt->z - m->position.z);
        }
        if (dst->y < 0.0f) {
            dst->y += 6.2831855f;
        }
        tgt->y = dst->y - m->rotation.y;
        if (tgt->y == 0.0f) {
            return 1;
        }
        if (mode == 0) {
            if (calcUPGet(unit)->sizeClass >= 2 &&
                (calcUPGet(unit)->charaId < CALC_SPECIAL_MODEL_CHARACTER_ID_BEGIN || calcUPGet(unit)->charaId >= CALC_SPECIAL_MODEL_CHARACTER_ID_END)) {
                rate = 16.0f;
            } else {
                rate = 5.0f;
            }
        } else {
            if (calcUPGet(unit)->sizeClass >= 2 &&
                (calcUPGet(unit)->charaId < CALC_SPECIAL_MODEL_CHARACTER_ID_BEGIN || calcUPGet(unit)->charaId >= CALC_SPECIAL_MODEL_CHARACTER_ID_END)) {
                rate = 10.0f;
            } else {
                rate = 5.0f;
            }
        }
        /* Takes the shorter way round the circle. */
        if (tgt->y < 0.0f) {
            nd = -tgt->y;
            if (nd > 3.1415927f) {
                goto wrap;
            }
        } else if (tgt->y > 3.1415927f) {
        wrap:
            if (tgt->y < 0.0f) {
                tgt->y = tgt->y + 6.2831855f;
                dst->y = dst->y + 6.2831855f;
            } else {
                tgt->y = tgt->y - 6.2831855f;
                dst->y = dst->y + 6.2831854820251465;
            }
        }
        tgt->y = tgt->y / rate;
        if (mode == 0) {
            if (calcUPGet(unit)->charaId < CALC_SPECIAL_MODEL_CHARACTER_ID_BEGIN || calcUPGet(unit)->charaId >= CALC_SPECIAL_MODEL_CHARACTER_ID_END) {
                unitSetInterp(unit);
                unitSetInterpTime(unit, rate * 0.033333335f);
            }
            d = m->rotation.y - dst->y;
            if (d < 0.0f) {
                d = -d;
                if (d > 0.000001f) {
                    goto mot;
                }
            } else if (d > 0.000001f) {
            mot:
                if (calcUPGet(unit)->sizeClass >= 2) {
                    unitMotSet(unit, 0x17);
                }
            }
        }
        actor->movePhaseTimer++;
    } else {
        if (m->rotation.y != dst->y) {
            d = m->rotation.y + tgt->y;
            m->rotation.y = d;
            if ((tgt->y > 0.0f && dst->y < m->rotation.y) ||
                (tgt->y < 0.0f && m->rotation.y < dst->y)) {
                goto snap;
            }
            d = m->rotation.y - dst->y;
            if (d < 0.0f) {
                d = -d;
                if (d <= 0.000001f) {
                    goto snap;
                }
            } else if (d <= 0.000001f) {
            snap:
                m->rotation.y = dst->y;
                goto set;
            }
        } else {
        set:
            done = 1;
        }
        if (mode == 0 && done) {
            unitSetInterpTime(unit, 0.26666668f);
        }
    }
    return done;
}

/* A vector copied as two doublewords, whose alias set holds int: the store may
 * overwrite the record's work word, which is read again after it. */
typedef union UnitTurnVector {
    int word[4];
    u64 pair[2];
} UnitTurnVector;

/* The record's work word, read as a word each time. */
#define TURN_WORK(unit) ((Actor *)((int *)(unit))[4])

/*
 * Starts a turn toward cmdTarget; mode 1 turns away: the target is mirrored
 * through the unit's own position.
 */
void unitCmdTurnSubInit(BattleUnit *unit, int mode)
{
    Actor *motion = unit->actor;
    float dx;
    float dz;

    TURN_WORK(unit)->movePhaseTimer = 0;
    *(UnitTurnVector *)&TURN_WORK(unit)->motionFrame = *(UnitTurnVector *)&TURN_WORK(unit)->cmdTarget;
    if (mode == 1) {
        Actor *work = TURN_WORK(unit);

        dx = ((Vector4 *)&work->motionFrame)->x - motion->position.x;
        dz = ((Vector4 *)&work->motionFrame)->z - motion->position.z;
        ((Vector4 *)&work->motionFrame)->x = motion->position.x - dx;
        ((Vector4 *)&work->motionFrame)->z = motion->position.z - dz;
    }
}

extern float unitCmdLenGet(BattleUnit *unit);

/* Format for the error printed when unitCmdMoveCha gets an unknown move kind. */
extern const char unitCmdMoveChaErrText[];

/*
 * Starts the move for the unit's moveKindId. Kind 0 jumps over short distances
 * and walks over long ones; the other kinds each select their own movement
 * command. An unknown kind prints an error.
 */
int unitCmdMoveCha(BattleUnit *unit)
{
    Actor *actor;
    CmdEntry *cmd;
    float len;

    actor = unit->work;
    cmd = unitCmdPtrGet(unit);
    unitCmdDstSet(unit, 3, 0);
    len = unitCmdLenGet(unit);
    actor->movePhase = 0;
    switch (calcUPGet(unit)->moveKindId) {
    case 0:
        if (len <= 255.0f) {
            unitCmdVASet(unit, 3, 0);
            cmd->selector = 0x17;
            unitCmdMoveChaJump(unit);
        } else {
            cmd->selector = 0x13;
            unitCmdMoveChaNorm(unit);
        }
        break;
    case 1:
        unitCmdVASet(unit, 3, 0);
        cmd->selector = 0x17;
        unitCmdMoveChaJump(unit);
        break;
    case 3:
        cmd->selector = 0x19;
        unitCmdMoveChaHover(unit);
        break;
    case 4:
        cmd->selector = 0x1B;
        unitCmdMoveChaWarp(unit);
        break;
    case 5:
        cmd->selector = 0x1D;
        unitCmdMoveChaJump2(unit);
        break;
    case 2:
    case 6:
        unitCmdVASet(unit, 1, 0);
        cmd->selector = 0x15;
        unitCmdMoveChaRun(unit);
        break;
    case 7:
        unitCmdVASet(unit, 9, 0);
        cmd->selector = 0x1F;
        unitCmdMoveChaHover2(unit);
        break;
    case 8:
        cmd->selector = 0x21;
        unitCmdMoveChaFloat(unit);
        break;
    case 9:
        unitCmdVASet(unit, 9, 0);
        cmd->selector = 0x23;
        unitCmdMoveChaHover3(unit);
        break;
    default:
        printf(unitCmdMoveChaErrText, calcUPGet(unit)->moveKindId);
        break;
    }
    return 0;
}

/*
 * calcUPGet(unit)+0x44 holds the position-table index that unitCmdDstSet
 * reads; unitCmdMoveNext's mode 1 copies it from the queued command entry's
 * halfword at +0x4. The float at +0x4C is the unit's default yaw, restored
 * into the motion actor on every call.
 */
#define CALC_UNIT_PARAM_UNK44_OFF 0x44
#define CMD_ENTRY_X_OR_IDX_OFF 0x4
#define CALC_UNIT_PARAM_DEFAULT_YAW_OFF 0x4C

/*
 * Restores the default yaw and moves on to the next command. Mode 1 saves the
 * queued position index and stands the unit; any other mode sets the
 * destination and direction first.
 */
int unitCmdMoveNext(BattleUnit *unit, int mode)
{
    Actor *actor;
    void *cmdEntry;

    actor = unit->actor;
    cmdEntry = unitCmdPtrGet(unit);
    actor->rotation.y = *(float *)((char *)calcUPGet(unit) + CALC_UNIT_PARAM_DEFAULT_YAW_OFF);
    if (mode == 1) {
        *(u16 *)((char *)calcUPGet(unit) + CALC_UNIT_PARAM_UNK44_OFF) =
            *(u16 *)((char *)cmdEntry + CMD_ENTRY_X_OR_IDX_OFF);
    } else {
        unitCmdDstSet(unit, 0, 0);
        unitCmdDirSet(unit);
    }
    if (mode == 1) {
        unitMotStandSet(unit);
    }
    return unitCmdNext(unit);
}

/*
 * unitCmdLenGet returns the distance that unitCmdMoveChaNorm, unitCmdMoveChaJump2
 * and unitCmdMovePosNorm pass on to unitCmdDivSet.
 */
extern void unitCmdDivSet(BattleUnit *unit, float distance, float step);

/*
 * A run up to the target unit, then a jump onto it: step 0 passes
 * unitCmdDivSet the distance and that distance less 255.0f as the step; step 1
 * waits for the run, step 2 starts the jump, and the last step waits for it to
 * land.
 */
int unitCmdMoveChaNorm(BattleUnit *unit)
{
    Actor *actor;
    float length;

    actor = unit->work;
    switch (actor->moveState) {
    case 0:
        length = unitCmdLenGet(unit);
        unitCmdDivSet(unit, length, length - 255.0f);
        unitCmdVASet(unit, 1, 0);
        unitCmdRun(unit, 1, 2);
        actor->moveState++;
        break;
    case 1:
        if (unitCmdRun(unit, 1, 2) != 0) {
            actor->moveState++;
        }
        break;
    case 2:
        actor->movePhase = 0;
        unitCmdDstSet(unit, 3, 0);
        unitCmdVASet(unit, 3, 0);
        unitCmdJump(unit, 3, 0, 0);
        actor->moveState++;
        break;
    default:
        if (unitCmdJump(unit, 3, 0, 0) != 0) {
            unitCmdMoveNext(unit, 0);
        }
        break;
    }
    return 0;
}

int unitCmdMoveChaRun(BattleUnit *unit)
{
    if (unitCmdRun(unit, 1, 0) != 0) {
        unitCmdMoveNext(unit, 0);
    }
    return 0;
}

int unitCmdMoveChaJump(BattleUnit *unit)
{
    if (unitCmdJump(unit, 3, 0, 0) != 0) {
        unitCmdMoveNext(unit, 0);
    }
    return 0;
}

int unitCmdMoveChaHover(BattleUnit *unit)
{
    if (unitCmdHover(unit, 5, 0) != 0) {
        unitCmdMoveNext(unit, 0);
    }
    return 0;
}

int unitCmdMoveChaWarp(BattleUnit *unit)
{
    if (unitCmdWarp(unit, 7, 0) != 0) {
        unitCmdMoveNext(unit, 0);
    }
    return 0;
}

/*
 * A three-hop jump to the command's target: step 0 stores distance / 3 in
 * hopLength and passes unitCmdDivSet the whole distance and hopLength as the
 * step; steps 1..3 each wait for unitCmdJump to land, then aim the next hop,
 * passing the remaining distance with hopLength as the step, or the remaining
 * distance itself on the last hop. The step after the last hop finishes the
 * move. The work slot is read again after unitCmdPtrGet, as in the original.
 * hops stays an int rather than a 3.0f literal, which the original converts
 * at the division.
 */
int unitCmdMoveChaJump2(BattleUnit *unit)
{
    Actor *actor;
    float *hopLength;
    int hops;
    float length;

    hops = 3;
    actor = unit->work;
    unitCmdPtrGet(unit);
    hopLength = &unit->work->hopLength;
    if (actor->moveState == 0) {
        length = unitCmdLenGet(unit);
        *hopLength = length / hops;
        unitCmdDivSet(unit, length, *hopLength);
        unitCmdVASet(unit, 3, 0);
        actor->moveState++;
    } else if (actor->moveState > hops) {
        unitCmdMoveNext(unit, 0);
    } else if (unitCmdJump(unit, 3, 2, 0) != 0) {
        actor->movePhase = 0;
        unitCmdDstSet(unit, 3, 0);
        length = unitCmdLenGet(unit);
        unitCmdDivSet(unit, length,
                      actor->moveState == hops ? length : *hopLength);
        unitCmdVASet(unit, 3, 0);
        actor->moveState++;
    }
    return 0;
}

int unitCmdMoveChaHover2(BattleUnit *unit)
{
    if (unitCmdJump(unit, 9, 0, 0) != 0) {
        unitCmdMoveNext(unit, 0);
    }
    return 0;
}

int unitCmdMoveChaFloat(BattleUnit *unit)
{
    if (unitCmdFloat(unit, 0xB, 0) != 0) {
        unitCmdMoveNext(unit, 0);
    }
    return 0;
}

int unitCmdMoveChaHover3(BattleUnit *unit)
{
    if (unitCmdJump(unit, 0xD, 0, 0) != 0) {
        unitCmdMoveNext(unit, 0);
    }
    return 0;
}

extern void unitCmdDstSet(BattleUnit *unit, int cmdId, int mode);
extern int unitCmdMovePosNorm(BattleUnit *unit);
extern int unitCmdMovePosRun(BattleUnit *unit);
extern int unitCmdMovePosJump(BattleUnit *unit);
extern int unitCmdMovePosHover(BattleUnit *unit);
extern int unitCmdMovePosWarp(BattleUnit *unit);
extern int unitCmdMovePosHover2(BattleUnit *unit);
extern int unitCmdMovePosFloat(BattleUnit *unit);
extern int unitCmdMovePosHover3(BattleUnit *unit);

/*
 * Starts a move toward a position, chosen by the unit's moveKindId. Kind 0
 * jumps for short distances and uses the normal move for long ones. An
 * unknown kind prints an error.
 */
int unitCmdMovePos(BattleUnit *unit)
{
    Actor *actor;
    int *cmd;
    float len;

    actor = unit->work;
    cmd = unitCmdPtrGet(unit);
    unitCmdDstSet(unit, 4, 1);
    len = unitCmdLenGet(unit);
    actor->movePhase = 0;
    switch (calcUPGet(unit)->moveKindId) {
    case 0:
        if (!(len <= 255.0f)) {
            *cmd = 0x14;
            unitCmdMovePosNorm(unit);
        } else {
            goto block_10;
        }
        break;
    case 2:
        unitCmdVASet(unit, 2, 0);
        *cmd = 0x16;
        unitCmdMovePosRun(unit);
        break;
    case 3:
        *cmd = 0x1A;
        unitCmdMovePosHover(unit);
        break;
    case 4:
        *cmd = 0x1C;
        unitCmdMovePosWarp(unit);
        break;
    case 5:
        *cmd = 0x1E;
        unitCmdMovePosJump(unit);
        break;
    case 1:
    case 6:
    block_10:
        unitCmdVASet(unit, 4, 0);
        *cmd = 0x18;
        unitCmdMovePosJump(unit);
        break;
    case 7:
        unitCmdVASet(unit, 0xA, 0);
        *cmd = 0x20;
        unitCmdMovePosHover2(unit);
        break;
    case 8:
        *cmd = 0x22;
        unitCmdMovePosFloat(unit);
        break;
    case 9:
        unitCmdVASet(unit, 0xA, 0);
        *cmd = 0x24;
        unitCmdMovePosHover3(unit);
        break;
    default:
        printf("** unitCmdMovePos: err -> %d\n", calcUPGet(unit)->moveKindId);
        break;
    }
    return 0;
}

/*
 * Step 0 parks the queued command's argument word in Actor.targetUnit for the
 * jump and passes unitCmdDivSet the distance with 255.0f as the step. Step 2
 * writes the word back into the command before the run's unitCmdDstSet reads it.
 */
int unitCmdMovePosNorm(BattleUnit *unit)
{
    Actor *actor;
    CmdEntry *cmd;

    actor = unit->work;
    cmd = unitCmdPtrGet(unit);
    switch (actor->moveState) {
    case 0:
        actor->targetUnit = (BattleUnit *)cmd->argument;
        unitCmdDivSet(unit, unitCmdLenGet(unit), 255.0f);
        unitCmdVASet(unit, 4, 0);
        unitCmdJump(unit, 4, 2, 0);
        actor->moveState++;
        break;
    case 1:
        if (unitCmdJump(unit, 4, 2, 0) != 0) {
            actor->moveState++;
        }
        break;
    case 2:
        actor->movePhase = 0;
        cmd->argument = (int)actor->targetUnit;
        unitCmdDstSet(unit, 2, 1);
        unitCmdVASet(unit, 2, 0);
        unitCmdDirSet(unit);
        unitCmdRun(unit, 2, 1);
        actor->moveState++;
        break;
    case 3:
        if (unitCmdRun(unit, 2, 1) != 0) {
            unitCmdMoveNext(unit, 1);
        }
        break;
    }
    return 0;
}

int unitCmdMovePosRun(BattleUnit *unit)
{
    if (unitCmdRun(unit, 2, 1) != 0) {
        unitCmdMoveNext(unit, 1);
    }
    return 0;
}

int unitCmdMovePosJump(BattleUnit *unit)
{
    if (unitCmdJump(unit, 4, 1, 0) != 0) {
        unitCmdMoveNext(unit, 1);
    }
    return 0;
}

int unitCmdMovePosHover(BattleUnit *unit)
{
    if (unitCmdHover(unit, 6, 1) != 0) {
        unitCmdMoveNext(unit, 1);
    }
    return 0;
}

int unitCmdMovePosWarp(BattleUnit *unit)
{
    if (unitCmdWarp(unit, 8, 1) != 0) {
        unitCmdMoveNext(unit, 1);
    }
    return 0;
}

int unitCmdMovePosHover2(BattleUnit *unit)
{
    if (unitCmdJump(unit, 0xA, 1, 0) != 0) {
        unitCmdMoveNext(unit, 1);
    }
    return 0;
}

int unitCmdMovePosFloat(BattleUnit *unit)
{
    if (unitCmdFloat(unit, 0xC, 1) != 0) {
        unitCmdMoveNext(unit, 1);
    }
    return 0;
}

int unitCmdMovePosHover3(BattleUnit *unit)
{
    if (unitCmdJump(unit, 0xE, 1, 0) != 0) {
        unitCmdMoveNext(unit, 1);
    }
    return 0;
}

/*
 * unitCmdDirAdj reads the position-table index at calcUPGet(unit)+0x44 as a
 * signed half: a negative index flips the side of the target that the offset
 * is placed on.
 */

void unitCmdDirAdj(BattleUnit *unit, int cmdId, int flip)
{
    Actor *actor = unit->work;
    Actor *motionActor = unit->actor;
    Vector4 *target = &actor->cmdTarget.vector;
    float offset = !flip ? -1.0f : 1.0f;

    if (*(short *)((char *)calcUPGet(unit) + CALC_UNIT_PARAM_UNK44_OFF) < 0) {
        offset = -offset;
    }
    motionActor->rotation.y = atan2f(target->x + offset - motionActor->position.x,
                                      target->z - motionActor->position.z);
    if (cmdId == 4 || cmdId == 2 || cmdId == 6 || cmdId == 0xA || cmdId == 0xE) {
        motionActor->rotation.y += 3.14159274f;
    }
}

/*
 * Moves the unit along the root-motion curve of its current step motion: the
 * per-frame offset is the curve's value at the motion actor's motionFrame less
 * the previous frame's, clamped to the distance left to the command target.
 * The target is blended by unitCmdDivSet, copied into the motion actor's
 * position and then restored. The calcUPGet call before the curve read has an
 * unused result, as in the original.
 */
void unitCmdPosAdj(BattleUnit *unit, int cmdId)
{
    Actor *actor;
    Actor *motionActor;
    UnitVector *target;
    int frame;
    int index;
    UnitPosAdj *table;
    int i;
    int motion;
    float *curve;
    float offset;
    float length;
    UnitVector savedTarget;

    actor = unit->work;
    motionActor = unit->actor;
    target = &actor->cmdTarget;
    frame = motionActor->motionFrame;
    index = 0;
    if (!(actor->flags & ACTOR_FLAG_ENEMY) &&
        !(calcUPGet(unit)->flags & CALC_UNIT_FLAG_IS_AGWS) &&
        (calcUPGet(unit)->charaId < CALC_SPECIAL_MODEL_CHARACTER_ID_BEGIN || calcUPGet(unit)->charaId >= CALC_SPECIAL_MODEL_CHARACTER_ID_END)) {
        table = adjTblPl;
        for (i = 0; table[i].charaId != 0; i++) {
            if (table[i].charaId == calcUPGet(unit)->charaId) {
                index = i;
                break;
            }
        }
    } else {
        table = adjTblEn;
        for (i = 0; table[i].charaId != 0; i++) {
            if (table[i].charaId == calcUPGet(unit)->charaId) {
                index = i;
                break;
            }
        }
    }
    motion = unitMotGet(unit);
    if (motion == 0xD) {
        curve = table[index].front1;
    } else if (motion == 0x11 || motion == 0xF) {
        curve = table[index].front5;
    } else if (motion == 0x12) {
        curve = table[index].back1;
    } else if (motion == 0x16 || motion == 0x14) {
        curve = table[index].back5;
    } else {
        return;
    }
    if (curve == 0) {
        return;
    }
    calcUPGet(unit);
    offset = curve[frame];
    if (frame != 0) {
        offset -= curve[frame - 1];
    }
    savedTarget = *target;
    length = unitCmdLenGet(unit);
    if (length < offset) {
        offset = length;
    }
    unitCmdDivSet(unit, length, offset);
    motionActor->position.x = target->vector.x;
    motionActor->position.z = target->vector.z;
    *target = savedTarget;
}

extern int unitCmdTurnSub(BattleUnit *unit, int mode);
extern void unitCmdTurnSubInit(BattleUnit *unit, int mode);
extern void unitCmdDirAdj(BattleUnit *unit, int cmdId, int flip);
extern int unitCmdMoveCalc(BattleUnit *unit, int freeX, int freeY, int freeZ);

/*
 * Runs the unit toward cmdTarget in three move phases: turn (cmdId 1 uses
 * motion 0xD, other ids use the backward motion 0x12), run the last stretch
 * with a step limited by size class, then stand facing the target. Returns 1
 * when the run is finished.
 */
int unitCmdRun(BattleUnit *unit, int cmdId, int flip)
{
    Actor *actor;
    Actor *motionActor;
    Vector4 *cmdTgt;
    float *saved;
    int motionId;
    int seKind;
    int turnMode;
    float stepLimit;
    float len;

    actor = unit->work;
    motionActor = unit->actor;
    cmdTgt = &actor->cmdTarget.vector;
    saved = (float *)&actor->motion;

    switch (actor->movePhase) {
    case 0:
        if (cmdId == 1) {
            motionId = 0xD;
            seKind = 1;
            turnMode = 0;
        } else {
            motionId = 0x12;
            seKind = 3;
            turnMode = 1;
        }
        unitMotSet(unit, motionId);
        /* The target is saved here and restored after the run step. */
        saved[0] = cmdTgt->x;
        saved[1] = cmdTgt->y;
        saved[2] = cmdTgt->z;
        sndSeTransPlay(unit, 1, seKind);
        actor->movePhase++;
        unitCmdTurnSubInit(unit, turnMode);
        /* fall through */
    case 1:
        unitCmdTurnSub(unit, 1);
        if (unit->work->runFrameCounter % 3 == 0) {
            decoEffCall(unit, 0x65);
        }
        if (unit->actor->moveFlags & MOVE_FLAG_REACHED) {
            unitMotSet(unit, (cmdId == 1) ? 0xE : 0x13);
            stepLimit = 0.8f;
            if (calcUPGet(unit)->sizeClass != 0) {
                stepLimit = 1.0f;
                if (calcUPGet(unit)->sizeClass != 1) {
                    stepLimit = 1.5f;
                }
            }
            len = unitCmdLenGet(unit);
            if (len < stepLimit) {
                stepLimit = len;
            }
            unitCmdDivSet(unit, len, stepLimit);
            motionActor->position.x = cmdTgt->x;
            motionActor->position.z = cmdTgt->z;
            cmdTgt->x = saved[0];
            cmdTgt->y = saved[1];
            cmdTgt->z = saved[2];
            unitCmdVASet(unit, cmdId, 0);
            actor->movePhase++;
        }
        break;
    case 2:
        unitCmdDirAdj(unit, cmdId, flip);
        if (unit->work->runFrameCounter % 3 == 0) {
            decoEffCall(unit, 0x65);
        }
        if (unitCmdMoveCalc(unit, 0, 0, 0) != 0) {
            sndSeTransPlay(unit, 1, ((cmdId ^ 1) == 0) ? 2 : 4);
            unitMotStandSet(unit);
            return 1;
        }
        break;
    }
    return 0;
}

/*
 * Moves the unit toward cmdTarget through move phases 0 to 5, choosing motions
 * and sounds from cmdId; faceFlag 0 enables the direction updates.
 * Returns 1 when the jump is finished.
 */
int unitCmdJump(BattleUnit *unit, int cmdId, int flip, int faceFlag)
{
    Actor *work = unit->work;
    Vector4 *cmdTgt = &work->cmdTarget.vector;
    float *saved = (float *)&work->motion;
    Actor *motionActor = unit->actor;
    int mot;
    int snd;
    int pre;
    int dir;
    float f20;
    float len;

    switch (work->movePhase) {
    case 0:
    case 1:
        /* While motion 2 plays, switch to motion 1 and wait nine frames. */
        if (work->movePhase == 0) {
            if (unitMotGet(unit) == 2) {
                unitMotSet(unit, 1);
                work->movePhase++;
                goto _sw_end;
            }
            work->movePhase += 2;
        } else {
            if ((int)((motionActor->moveElapsed / 0.033333335f) + 0.5f) >= 9) {
                work->movePhase = work->movePhase + 1;
            } else {
                goto _sw_end;
            }
        }
        /* fallthrough */
    case 2:
        if (cmdId == 3 || cmdId == 9 || cmdId == 0xD) {
            mot = 0xD;
            snd = 1;
            dir = 0;
        } else {
            mot = 0x12;
            snd = 3;
            dir = 1;
        }
        unitMotSet(unit, mot);
        work->movePhase++;
        saved[0] = cmdTgt->x;
        saved[1] = cmdTgt->y;
        saved[2] = cmdTgt->z;
        decoEffCall(unit, 0x65);
        sndSeTransPlay(unit,1, snd);
        unitCmdTurnSubInit(unit, dir);
        /* fallthrough */
    case 3:
        unitCmdTurnSub(unit, 1);
        if (!(unit->actor->moveFlags & MOVE_FLAG_REACHED)) {
            goto block_62;
        }
        if (cmdId == 3 || cmdId == 9 || cmdId == 0xD) {
            mot = 0xE;
        } else {
            mot = 0x13;
        }
        unitMotSet(unit, mot);
        f20 = 1.0f;
        if (calcUPGet(unit)->sizeClass < 2) {
            f20 = 0.8f;
        }
        len = unitCmdLenGet(unit);
        if ((unsigned)(cmdId - 0xD) < 2U) {
            unit->actor->moveStepRate = (1.0f / (len / 10.0f)) * 0.033333335f;
        }
        if (len < f20) {
            f20 = len;
        }
        unitCmdDivSet(unit, len, f20);
        motionActor->position.x = cmdTgt->x;
        work->movePhase++;
        motionActor->position.z = cmdTgt->z;
        goto _sw_end;
    case 4:
        /* Restores the saved target, then sets the approach step. */
        cmdTgt->x = saved[0];
        cmdTgt->y = saved[1];
        cmdTgt->z = saved[2];
        f20 = 1.0f;
        if (calcUPGet(unit)->sizeClass < 2) {
            f20 = 0.8f;
        }
        len = unitCmdLenGet(unit);
        if (len < f20) {
            f20 = len;
        } else {
            f20 = len - f20;
        }
        unitCmdDivSet(unit, len, f20);
        unitCmdVASet(unit, cmdId, 0);
        unitCmdMoveCalc(unit, 0, 1, 0);
        mot = (cmdId == 3 || cmdId == 9) ? 0xF : 0x14;
        if ((unsigned)(cmdId - 0xD) >= 2U) {
            unitSetInterp(unit);
            unitMotSet(unit, mot);
        }
        work->movePhase++;
        goto _sw_end;
    case 5:
        if (faceFlag == 0) {
            unitCmdDirAdj(unit, cmdId, flip);
        }
        pre = (motionActor->velocity.y < 0.0f) ? -1 : 1;
        if (unitCmdMoveCalc(unit, 0, 1, 0) != 0) {
            if (cmdId == 3 || cmdId == 9) {
                mot = 0x11;
                snd = 2;
            } else {
                mot = 0x16;
                snd = 4;
            }
            if (cmdId == 0xD) {
                mot = 0xF;
                snd = 2;
            } else if (cmdId == 0xE) {
                mot = 0x14;
                snd = 4;
            }
            unitMotSet(unit, mot);
            unit->actor->moveStepRate = 0.033333335f;
            work->movePhase++;
            cmdTgt->x = saved[0];
            cmdTgt->y = saved[1];
            cmdTgt->z = saved[2];
            decoEffCall(unit, 0x65);
            sndSeTransPlay(unit,1, snd);
        block_62:
            unitCmdPosAdj(unit, cmdId);
            break;
        }
        if ((unsigned)(cmdId - 3) < 2U) {
            if (pre != ((motionActor->velocity.y < 0.0f) ? -1 : 1)) {
                goto motion;
            }
        }
        if ((unsigned)(cmdId - 9) >= 2U) {
            goto _sw_end;
        }
        if ((int)unit->work->posSnapX != (int)(unit->work->posSnapZ * 0.5f)) {
            goto _sw_end;
        }
    motion:
        mot = (cmdId == 3 || cmdId == 9) ? 0x10 : 0x15;
        unitSetInterp(unit);
        unitMotSet(unit, mot);
        goto _sw_end;
    default:
        if (unit->actor->moveFlags & MOVE_FLAG_REACHED) {
            motionActor->position.x = saved[0];
            motionActor->position.y = saved[1];
            motionActor->position.z = saved[2];
            unitSetInterpTime(unit, 0.26666668f);
            return 1;
        }
        if (faceFlag == 0) {
            unitCmdDirAdj(unit, cmdId, flip);
        }
        unitCmdPosAdj(unit, cmdId);
        break;
    }
_sw_end:
    return 0;
}

static unsigned char sSp[24];
extern int calcSpline(void *spline, float *out, int mode);

/*
 * Hovers the unit toward cmdTarget: turns to face it, moves along the sSp
 * spline, then settles on the target. Returns 1 when the hover is finished.
 */
int unitCmdHover(BattleUnit *unit, int cmdId, int flip)
{
    Actor *work = unit->work;
    Actor *motionActor = unit->actor;
    Vector4 *cmdTgt = &work->cmdTarget.vector;
    int motionId;
    int seKind;
    int turnMode;

    switch (work->movePhase) {
    case 0:
        if (cmdId == 5) {
            motionId = 0xD;
            seKind = 1;
            turnMode = 0;
        } else {
            motionId = 0x12;
            seKind = 3;
            turnMode = 1;
        }
        unitMotSet(unit, motionId);
        sndSeTransPlay(unit,1, seKind);
        decoEffCall(unit, 0x65);
        work->movePhase++;
        unitCmdTurnSubInit(unit, turnMode);
        /* fallthrough */
    case 1:
        unitCmdTurnSub(unit, 1);
        if (unit->actor->moveFlags & MOVE_FLAG_REACHED) {
            unitMotSet(unit, (cmdId == 5) ? 0xE : 0x13);
            unitCmdVASet(unit, cmdId, 0);
            work->movePhase++;
        }
        return 0;
    case 2:
        unitCmdDirAdj(unit, cmdId, flip);
        if (calcSpline(sSp, (float *)&motionActor->position, 3) != 0) {
            work->movePhase++;
        }
        if (cmdId == 5) {
            if (unitMotGet(unit) != 0xF) {
                unitSetInterp(unit);
                unitMotSet(unit, 0xF);
            }
        } else if (unitMotGet(unit) != 0x14) {
            unitSetInterp(unit);
            unitMotSet(unit, 0x14);
        }
        return 0;
    default:
        if (motionActor->moveFlags & MOVE_FLAG_REACHED) {
            motionActor->position.x = cmdTgt->x;
            motionActor->position.z = cmdTgt->z;
            unitSetInterpTime(unit, 0.26666668f);
            decoEffCall(unit, 0x65);
            return 1;
        }
        unitCmdDirAdj(unit, cmdId, flip);
        return 0;
    }
}

/*
 * Floats the unit toward cmdTarget: turns to face it, lowers the motion actor
 * by a fixed offset, follows the sSp spline and then settles on the target.
 * Returns 1 when finished.
 */
int unitCmdFloat(BattleUnit *unit, int cmdId, int flip)
{
    Actor *work = unit->work;
    Actor *motionActor = unit->actor;
    Vector4 *cmdTgt = &work->cmdTarget.vector;
    int turnMode;
    int m0;
    int m3;
    float y;
    float yOffset;
    int floatMotion;

    switch (work->movePhase) {
    case 0:
        if (cmdId == 0xB) {
            m0 = 0xD;
            turnMode = 0;
        } else {
            m0 = 0x12;
            turnMode = 1;
        }
        unitMotSet(unit, m0);
        work->movePhase++;
        unitCmdTurnSubInit(unit, turnMode);
        /* fallthrough */
    case 1:
        unitCmdTurnSub(unit, 1);
        if (unit->actor->moveFlags & MOVE_FLAG_REACHED) {
            motionActor->position.x += motionActor->floatStepX;
            motionActor->position.z += motionActor->floatStepZ;
            if (cmdId == 0xB) {
                y = motionActor->position.y;
                yOffset = 1.3f;
                floatMotion = 0xE;
                motionActor->position.y = y - yOffset;
            } else {
                y = motionActor->position.y;
                floatMotion = 0x13;
                yOffset = 0.8f;
                motionActor->position.y = y - yOffset;
            }
            unitMotSet(unit, floatMotion);
            unit->actor->moveFlags &= ~8;
            unit->actor->moveStepRate = (1.0f / (unitCmdLenGet(unit) / 10.0f)) * 0.033333335f;
            unitCmdVASet(unit, cmdId, 0);
            work->movePhase++;
        }
        break;
    case 2:
        if (cmdId == 0xB) {
            unitCmdDirAdj(unit, cmdId, flip);
        }
        if (calcSpline(sSp, (float *)&motionActor->position, 3) != 0) {
            work->movePhase++;
        }
        break;
    case 3:
        m3 = 0x14;
        if (cmdId == 0xB) {
            m3 = 0xF;
        }
        unitMotSet(unit, m3);
        unit->actor->moveStepRate = 0.033333335f;
        motionActor->position.y = 0.0f;
        motionActor->position.x += motionActor->floatStepX;
        work->movePhase++;
        motionActor->position.z += motionActor->floatStepZ;
        break;
    default:
        if (unit->actor->moveFlags & MOVE_FLAG_REACHED) {
            motionActor->position.x = cmdTgt->x;
            motionActor->position.z = cmdTgt->z;
            unitSetInterpTime(unit, 0.26666668f);
            return 1;
        }
        break;
    }
    return 0;
}

/*
 * Warps the unit to cmdTarget: stands it, fades it out, moves the motion actor
 * to the target and fades it back in. Returns 1 when finished.
 */
int unitCmdWarp(BattleUnit *unit, int cmdId, int flag)
{
    Actor *actor = unit->work;
    Actor *motionActor = unit->actor;
    Vector4 *target = &actor->cmdTarget.vector;

    switch (actor->movePhase) {
    case 0:
        unitMotStandSet(unit);
        actor->movePhaseTimer = 0;
        actor->movePhase++;
        break;
    case 1:
        if (unitTelOut(unit) != 0) {
            actor->movePhase++;
        }
        break;
    case 2:
        actor->movePhase = 3;
        motionActor->position.x = target->x;
        actor->movePhaseTimer = 0;
        motionActor->position.y = target->y;
        motionActor->position.z = target->z;
        break;
    default:
        if (unitTelIn(unit) == 0) {
            break;
        }
        return 1;
    }
    return 0;
}

/*
 * The queued command (unitCmdPtrGet) holds either a raw coordinate pair at
 * +0x4 and +0xC or a position-table index at +0x4, chosen by mode. obj.c types
 * these fields as int, so the float reads below reinterpret the same bytes.
 */
#define CMD_ENTRY_Z_OFF 0xC

void unitCmdDstSet(BattleUnit *unit, int cmdId, int mode)
{
    Actor *actor;
    Vector4 *target;
    void *cmdEntry;
    int idx;

    cmdEntry = unitCmdPtrGet(unit);
    actor = unit->work;
    target = &actor->cmdTarget.vector;

    if (mode == 0) {
        BattleUnit *targetUnit = actor->targetUnit;

        idx = *(short *)((char *)calcUPGet(targetUnit) + CALC_UNIT_PARAM_UNK44_OFF);
        target->x = dataPosTblGet(idx)->x;
        target->z = dataPosTblGet(idx)->z;
        if (cmdId != 0) {
            target->x -= unitColiGet(unit, targetUnit);
        }
        if (idx < 0) {
            target->x = -target->x;
        }
    } else if (mode == 1) {
        idx = *(int *)((char *)cmdEntry + CMD_ENTRY_X_OR_IDX_OFF);
        target->x = dataPosTblGet(idx)->x;
        target->z = dataPosTblGet(idx)->z;
        if (idx < 0) {
            target->x = -target->x;
        }
    } else {
        target->x = *(float *)((char *)cmdEntry + CMD_ENTRY_X_OR_IDX_OFF);
        target->z = *(float *)((char *)cmdEntry + CMD_ENTRY_Z_OFF);
    }
    target->y = 0.0f;
}

float unitCmdDirSet(BattleUnit *unit)
{
    float zero = 0.0f;
    Actor *self = unit->work;
    Vector4 *target = &self->cmdTarget.vector;
    Actor *motionActor = unit->actor;
    float dx = target->x - motionActor->position.x;

    if (dx != zero || (target->z - motionActor->position.z) != zero) {
        motionActor->rotation.y = atan2f(dx, target->z - motionActor->position.z);
        if (motionActor->rotation.y < zero) {
            motionActor->rotation.y += 6.2831855f;
        }
    }
    return motionActor->rotation.y;
}

/*
 * Ground-plane distance from the motion actor to the pending move command's
 * target (cmdTarget x and z).
 */
float unitCmdLenGet(BattleUnit *unit)
{
    Vector4 *target;
    Actor *motionActor;
    float dx;
    float dz;

    target = &unit->work->cmdTarget.vector;
    motionActor = unit->actor;
    dx = target->x - motionActor->position.x;
    if (dx < 0.0f) {
        dx = -dx;
    }
    dz = target->z - motionActor->position.z;
    if (dz < 0.0f) {
        dz = -dz;
    }
    return __builtin_sqrtf(dx * dx + dz * dz);
}

void unitCmdDivSet(BattleUnit *unit, float distance, float step)
{
    Actor *self = unit->work;
    Vector4 *target = &self->cmdTarget.vector;
    float ratio = step / distance;
    Actor *motionActor = unit->actor;

    target->x = motionActor->position.x + (target->x - motionActor->position.x) * ratio;
    target->z = motionActor->position.z + (target->z - motionActor->position.z) * ratio;
}

static float cPoint[32];
extern void calcSplineInit(void *spline, float *points, int count, int duration);

/*
 * Prepares a move toward cmdTarget: sets the interpolation time and the path
 * length in the floats at +0xD0 and +0xD8. Ids 5, 6, 11 and 12 build a
 * five-point spline in sSp; other ids take a straight-line velocity, and ids
 * 3 and 4 also set a vertical acceleration and velocity.
 */
void unitCmdVASet(BattleUnit *unit, int cmdId, int arg2)
{
    float q = 0.25f;
    Actor *act = unit->actor;
    float *tgt = (float *)(((unsigned char *)unit->work) + 0x90);
    float *p1 = (float *)(((unsigned char *)unit->work) + 0xD0);
    float *p2 = (float *)(((unsigned char *)unit->work) + 0xD8);
    float len = unitCmdLenGet(unit) / 0.4f;
    int n = (int)len;
    float *arr;

    unitSetInterpTime(unit, (((float)n) * q) * 0.033333335f);
    *p1 = len;
    *p2 = len;
    if (((unsigned int)(cmdId - 5)) < 2U) {
        arr = cPoint;
        arr[0] = act->position.x;
        arr[1] = act->position.y;
        arr[2] = act->position.z;
        arr[4] = act->position.x + ((tgt[0] - act->position.x) * 0.125f);
        arr[5] = act->position.y + ((tgt[1] - act->position.y) * 0.125f);
        arr[6] = act->position.z + ((tgt[2] - act->position.z) * 0.125f);
        arr[8] = act->position.x + ((tgt[0] - act->position.x) * 0.5f);
        arr[9] = act->position.y + ((tgt[1] - act->position.y) * 0.5f);
        arr[10] = act->position.z + ((tgt[2] - act->position.z) * 0.5f);
        arr[12] = act->position.x + (((tgt[0] - act->position.x) * 0.125f) * 7.0f);
        arr[13] = act->position.y + (((tgt[1] - act->position.y) * 0.125f) * 7.0f);
        arr[14] = act->position.z + (((tgt[2] - act->position.z) * 0.125f) * 7.0f);
        arr[16] = tgt[0];
        arr[17] = tgt[1];
        arr[18] = tgt[2];
        calcSplineInit(sSp, arr, 5, n);
        goto after_switch;
    }
    /* Ids 11 and 12 build the same points; 11 first sets the target height to the actor's. */
    if (cmdId == 11) {
        tgt[1] = act->position.y;
        cPoint[0] = act->position.x;
        cPoint[1] = act->position.y;
        cPoint[2] = act->position.z;
        cPoint[4] = act->position.x + ((tgt[0] - act->position.x) * q);
        cPoint[5] = (act->position.y + ((tgt[1] - act->position.y) * q)) - 0.125f;
        cPoint[6] = act->position.z + ((tgt[2] - act->position.z) * q);
        cPoint[8] = act->position.x + ((tgt[0] - act->position.x) * 0.5f);
        cPoint[9] = (act->position.y + ((tgt[1] - act->position.y) * 0.5f)) - q;
        cPoint[10] = act->position.z + ((tgt[2] - act->position.z) * 0.5f);
        cPoint[12] = act->position.x + (((tgt[0] - act->position.x) * q) * 3.0f);
        cPoint[13] = (act->position.y + (((tgt[1] - act->position.y) * q) * 3.0f)) - 0.125f;
        cPoint[14] = act->position.z + (((tgt[2] - act->position.z) * q) * 3.0f);
        cPoint[16] = tgt[0];
        cPoint[17] = tgt[1];
        cPoint[18] = tgt[2];
        calcSplineInit(sSp, cPoint, 5, n);
    } else if (cmdId == 12) {
        cPoint[0] = act->position.x;
        cPoint[1] = act->position.y;
        cPoint[2] = act->position.z;
        cPoint[4] = act->position.x + ((tgt[0] - act->position.x) * q);
        cPoint[5] = (act->position.y + ((tgt[1] - act->position.y) * q)) - 0.125f;
        cPoint[6] = act->position.z + ((tgt[2] - act->position.z) * q);
        cPoint[8] = act->position.x + ((tgt[0] - act->position.x) * 0.5f);
        cPoint[9] = (act->position.y + ((tgt[1] - act->position.y) * 0.5f)) - q;
        cPoint[10] = act->position.z + ((tgt[2] - act->position.z) * 0.5f);
        cPoint[12] = act->position.x + (((tgt[0] - act->position.x) * q) * 3.0f);
        cPoint[13] = (act->position.y + (((tgt[1] - act->position.y) * q) * 3.0f)) - 0.125f;
        cPoint[14] = act->position.z + (((tgt[2] - act->position.z) * q) * 3.0f);
        cPoint[16] = tgt[0];
        cPoint[17] = tgt[1];
        cPoint[18] = tgt[2];
        calcSplineInit(sSp, cPoint, 5, n);
    } else {
        float az = act->position.z;
        float ax = act->position.x;
        float t0 = tgt[0];

        act->velocity.x = (t0 - ax) / len;
        act->velocity.z = (tgt[2] - az) / len;
        act->acceleration.x = 0.0f;
        act->acceleration.z = 0.0f;
    }
after_switch:
    if (((unsigned int)(cmdId - 3)) < 2U) {
        act->acceleration.y = -0.0204375f;
        act->velocity.y = (len * 0.0204375f) * 0.5f;
    } else {
        *((int *)(&act->acceleration.y)) = 0;
        *((int *)(&act->velocity.y)) = 0;
    }
}

/*
 * One step of a move command toward cmdTarget: each axis adds the motion
 * actor's acceleration into its velocity and the velocity into its position.
 * An axis whose free flag is 1 keeps moving while at least one step remains
 * in posSnapX (the step count unitCmdVASet stores there, decremented here by
 * one per call); any other axis counts as arrived when it already equals the
 * target, or snaps onto the target once it passes it or comes within 1e-6.
 * Returns 1 when all three axes have arrived.
 *
 * Forms that moved the bytes: velocity/position locals keep both in FPRs
 * across the stores (no reload of +0x30/+0x10); the block-scoped zero is
 * materialized once per axis after the velocity/acceleration loads and shared
 * by the three sign tests; the || form of the overshoot test allocates arrived
 * to $9 and the target base to $8; the magnitude negates distance inside the
 * comparison, which keeps neg.s and the duplicated 1e-6 test, where a separate
 * `if (distance < zero) distance = -distance;` gives a branch-likely neg and
 * a ternary without the assignment folds into a compare against -1e-6.
 */
int unitCmdMoveCalc(BattleUnit *unit, int freeX, int freeY, int freeZ)
{
    Actor *actor = unit->work;
    Actor *motionActor = unit->actor;
    int arrived = 0;
    Vector4 *target = &actor->cmdTarget.vector;
    float *steps = &actor->posSnapX;
    float velocity;
    float position;
    float distance;

    if (freeX == 1 && *steps >= 1.0f) {
        motionActor->velocity.x += motionActor->acceleration.x;
        motionActor->position.x += motionActor->velocity.x;
    } else if (motionActor->position.x == target->x) {
        arrived++;
    } else {
        float zero = 0.0f;

        velocity = motionActor->velocity.x + motionActor->acceleration.x;
        position = motionActor->position.x + velocity;
        motionActor->velocity.x = velocity;
        motionActor->position.x = position;
        if ((velocity > zero && position > target->x)
            || (velocity < zero && position < target->x)) {
            motionActor->position.x = target->x;
            arrived++;
        } else {
            distance = target->x - position;
            if ((distance < zero ? (distance = -distance) : distance) <= 1e-6f) {
                motionActor->position.x = target->x;
                arrived++;
            }
        }
    }

    if (freeY == 1 && *steps >= 1.0f) {
        motionActor->velocity.y += motionActor->acceleration.y;
        motionActor->position.y += motionActor->velocity.y;
    } else if (motionActor->position.y == target->y) {
        arrived++;
    } else {
        float zero = 0.0f;

        velocity = motionActor->velocity.y + motionActor->acceleration.y;
        position = motionActor->position.y + velocity;
        motionActor->velocity.y = velocity;
        motionActor->position.y = position;
        if ((velocity > zero && position > target->y)
            || (velocity < zero && position < target->y)) {
            motionActor->position.y = target->y;
            arrived++;
        } else {
            distance = target->y - position;
            if ((distance < zero ? (distance = -distance) : distance) <= 1e-6f) {
                motionActor->position.y = target->y;
                arrived++;
            }
        }
    }

    if (freeZ == 1 && *steps >= 1.0f) {
        motionActor->velocity.z += motionActor->acceleration.z;
        motionActor->position.z += motionActor->velocity.z;
    } else if (motionActor->position.z == target->z) {
        arrived++;
    } else {
        float zero = 0.0f;

        velocity = motionActor->velocity.z + motionActor->acceleration.z;
        position = motionActor->position.z + velocity;
        motionActor->velocity.z = velocity;
        motionActor->position.z = position;
        if ((velocity > zero && position > target->z)
            || (velocity < zero && position < target->z)) {
            motionActor->position.z = target->z;
            arrived++;
        } else {
            distance = target->z - position;
            if ((distance < zero ? (distance = -distance) : distance) <= 1e-6f) {
                motionActor->position.z = target->z;
                arrived++;
            }
        }
    }

    *steps -= 1.0f;
    return arrived == 3;
}

/*
 * Teleport fade-in: the first step clears the motion actor's transparency
 * and its hidden-draw flag (bit 0x8, the one unitDispOn clears); later steps
 * raise transparency by posSnapX until it reaches 1.0 and report 1.
 */
int unitTelIn(BattleUnit *unit)
{
    Actor *actor = unit->work;
    Actor *motionActor = unit->actor;

    if (actor->movePhaseTimer == 0) {
        motionActor->transparency = 0.0f;
        unit->actor->flags &= ~ACTOR_FLAG_HIDDEN;
        actor->movePhaseTimer++;
    } else {
        motionActor->transparency += motionActor->posSnapX;
        if (motionActor->transparency >= 1.0f) {
            motionActor->transparency = 1.0f;
            return 1;
        }
    }
    return 0;
}

/*
 * Teleport fade-out, unitTelIn reversed: the first step only starts the
 * phase timer; after that transparency falls by posSnapX each step, and at
 * 0.0 the hidden-draw flag is set and the step reports 1.
 */
int unitTelOut(BattleUnit *unit)
{
    Actor *actor = unit->work;
    Actor *motionActor = unit->actor;

    if (actor->movePhaseTimer == 0) {
        actor->movePhaseTimer++;
    } else {
        motionActor->transparency -= motionActor->posSnapX;
        if (motionActor->transparency <= 0.0f) {
            motionActor->transparency = 0.0f;
            unit->actor->flags |= ACTOR_FLAG_HIDDEN;
            return 1;
        }
    }
    return 0;
}

/*
 * Battle start: step 0 counts the queued command's argument down as a delay,
 * step 1 plays the battle-ready motions (unitMotSet 0x23, weapon motion 0x2F)
 * for the same player units unitCmdBted poses, or the stand motion for any
 * other unit, step 2 waits for unitTelIn, and the last step waits for the
 * ready motion to end before putting the weapon away.
 */
int unitCmdBtst(BattleUnit *unit)
{
    Actor *actor;
    CmdEntry *cmd;

    actor = unit->work;
    cmd = unitCmdPtrGet(unit);
    if (actor->moveState == 0) {
        if (cmd->argument > 0) {
            cmd->argument--;
        } else {
            actor->moveState = 1;
        }
    } else if (actor->moveState == 1) {
        if (!(unit->work->flags & ACTOR_FLAG_ENEMY) &&
            !(calcUPGet(unit)->flags & CALC_UNIT_FLAG_IS_AGWS) &&
            (calcUPGet(unit)->charaId < CALC_SPECIAL_MODEL_CHARACTER_ID_BEGIN || calcUPGet(unit)->charaId >= CALC_SPECIAL_MODEL_CHARACTER_ID_END)) {
            unitMotSet(unit, 0x23);
            unitWpnMotSet(unit, 0, 0, 0x2F);
        } else {
            unitMotStandSet(unit);
        }
        unitMtdProc(unit);
        actor->movePhaseTimer = 0;
        actor->moveState++;
    } else if (actor->moveState == 2) {
        unitMtdProc(unit);
        if (unitTelIn(unit) != 0) {
            actor->moveState++;
        }
    } else if (!(unit->work->flags & ACTOR_FLAG_ENEMY) &&
               !(calcUPGet(unit)->flags & CALC_UNIT_FLAG_IS_AGWS) &&
               (calcUPGet(unit)->charaId < CALC_SPECIAL_MODEL_CHARACTER_ID_BEGIN || calcUPGet(unit)->charaId >= CALC_SPECIAL_MODEL_CHARACTER_ID_END)) {
        unitMtdProc(unit);
        if (unit->actor->moveFlags & MOVE_FLAG_REACHED) {
            unitMotStandSet(unit);
            unitWpnMotSet(unit, 0, 0, 0xFF);
            return unitCmdNext(unit);
        }
    } else {
        return unitCmdNext(unit);
    }
    return 0;
}

/*
 * Battle end: step 0 plays the victory motions (unitMotSet 0x24, weapon
 * motion 0x30, face motion 0x39) for a player unit that is neither an enemy,
 * nor an AGWS (CALC_UNIT_FLAG_IS_AGWS), nor a special-model character, then
 * turns the status effects off; the next step waits for the motion to end and
 * puts the weapon away.
 */
int unitCmdBted(BattleUnit *unit)
{
    Actor *actor;

    actor = unit->work;
    unitCmdPtrGet(unit);
    if (actor->moveState == 0) {
        if (!(unit->work->flags & ACTOR_FLAG_ENEMY) &&
            !(calcUPGet(unit)->flags & CALC_UNIT_FLAG_IS_AGWS) &&
            (calcUPGet(unit)->charaId < CALC_SPECIAL_MODEL_CHARACTER_ID_BEGIN || calcUPGet(unit)->charaId >= CALC_SPECIAL_MODEL_CHARACTER_ID_END)) {
            unitMotSet(unit, 0x24);
            unitWpnMotSet(unit, 0, 0, 0x30);
            unitFaceMotSet(unit, 0x39);
        }
        unitMtdProc(unit);
        statEffOff(unit);
        actor->moveState++;
    } else {
        unitMtdProc(unit);
        if (unit->actor->moveFlags & MOVE_FLAG_REACHED) {
            if (calcUPGet(unit)->flags & CALC_UNIT_FLAG_IS_AGWS) {
                unitWpnMotSet(unit, 0, 0, 5);
            } else if (!(unit->work->flags & ACTOR_FLAG_ENEMY) &&
                       !(calcUPGet(unit)->flags & CALC_UNIT_FLAG_IS_AGWS) &&
                       (calcUPGet(unit)->charaId < CALC_SPECIAL_MODEL_CHARACTER_ID_BEGIN ||
                        calcUPGet(unit)->charaId >= CALC_SPECIAL_MODEL_CHARACTER_ID_END)) {
                unitWpnMotSet(unit, 0, 0, 0xFF);
            }
            return unitCmdNext(unit);
        }
    }
    return 0;
}

/* Applies the weapon changes that the unit's motion data requests. */
void unitMtdProc(BattleUnit *unit)
{
    int defaultWeapon;

    defaultWeapon = dataDefWpnGet(unit);
    if (dataMtdRead(unit, 2) == 1) {
        transWepIn(unit, defaultWeapon);
    }
    if (dataMtdRead(unit, 3) == 1) {
        transWepOut(unit, defaultWeapon);
    }
    if (dataMtdRead(unit, 6) == 1) {
        transWepIn(unit, defaultWeapon);
    }
    if (dataMtdRead(unit, 6) == 1) {
        transWepIn(unit, 1);
    }
    if (dataMtdRead(unit, 7) != 1) {
        return;
    }
    transWepOut(unit, 1);
}

/*
 * unitCmdRec and unitCmdStat read the flag word at +0x4 of the queued command
 * entry; when it is nonzero the stand motion is skipped.
 */
typedef struct {
    unsigned char unmodeled_00[0x4];
    int flag; /* +0x4 */
} UnitCmdRecEntry;

void unitCmdRec(BattleUnit *unit)
{
    Actor *actor;
    UnitCmdRecEntry *cmd;

    actor = unit->work;
    cmd = unitCmdPtrGet(unit);
    if (actor->moveState == 0) {
        if (cmd->flag == 0) {
            unitMotStandSet(unit);
        }
        actor->moveState++;
    }
    unitCmdNext(unit);
}

/* Wakes the unit: plays motion 0x25 and its sound, then clears the dead flag and stands it once the motion ends. */
int unitCmdWake(BattleUnit *unit)
{
    Actor *actor;

    actor = unit->work;
    unitCmdPtrGet(unit);
    if (actor->moveState == 0) {
        unitMotSet(unit, 0x25);
        sndSeTransPlay(unit, 2, 5);
        actor->moveState++;
    } else if (unit->actor->moveFlags & MOVE_FLAG_REACHED) {
        actor->flags &= ~ACTOR_FLAG_DEAD;
        unitMotStandSet(unit);
        return unitCmdNext(unit);
    }
    return 0;
}

void unitCmdStat(BattleUnit *unit)
{
    Actor *actor;
    UnitCmdRecEntry *cmd;

    actor = unit->work;
    cmd = unitCmdPtrGet(unit);
    if (actor->moveState == 0) {
        if (cmd->flag == 0) {
            unitMotStandSet(unit);
        }
        actor->moveState++;
    }
    unitCmdNext(unit);
}

/* Waits until the CD sync counter reaches the command's argument, then loads the unit's file data. */
int unitCmdReadWait(BattleUnit *unit)
{
    Actor *actor;
    CmdEntry *cmd;

    actor = unit->work;
    cmd = unitCmdPtrGet(unit);
    if (actor->moveState == 0) {
        if (dataCdSync() <= cmd->argument) {
            dataUnitFileLoad2Sub(unit);
            actor->moveState++;
        }
    } else {
        return unitCmdNext(unit);
    }
    return 0;
}

/*
 * Step 0 calls effect 0x67 for an AGWS unit (CALC_UNIT_FLAG_IS_AGWS), else
 * effect 0x66. Step 1 stands the unit and clears the teleport timer, then
 * step 2 waits for unitTelIn.
 */
int unitCmdAgws2(BattleUnit *unit)
{
    Actor *actor;

    actor = unit->work;
    unitCmdPtrGet(unit);
    if (actor->moveState == 0) {
        decoEffCall(unit, (calcUPGet(unit)->flags & CALC_UNIT_FLAG_IS_AGWS) ? 0x67 : 0x66);
        actor->moveState++;
    } else if (actor->moveState == 1) {
        unitMotStandSet(unit);
        actor->movePhaseTimer = 0;
        actor->moveState++;
    } else if (unitTelIn(unit) != 0) {
        return unitCmdNext(unit);
    }
    return 0;
}

/* Teleports the unit out; an enemy is then taken out of the unit table and removed. */
int unitCmdEscape(BattleUnit *unit)
{
    Actor *actor;

    actor = unit->work;
    switch (actor->moveState) {
    case 0:
        actor->moveState = 1;
        break;
    case 1:
        if (unitTelOut(unit) != 0) {
            actor->moveState++;
        }
        break;
    case 2:
        ((ActorFlagsWord *)actor)->i = (actor->flags & 0x61) | 0x80;
        if (unit->work->flags & ACTOR_FLAG_ENEMY) {
            unitTblRemove(unit);
            unitRemove(unit, 0);
        }
        break;
    }
    return 0;
}

/*
 * The reverse of unitCmdFadeOut: teleports the unit in, then unblocks the
 * unit's effect record and restarts its status effects.
 */
int unitCmdFadeIn(BattleUnit *unit)
{
    Actor *actor;
    CalcUnitParam *param;

    actor = unit->work;
    switch (actor->moveState) {
    case 0:
        actor->moveState++;
        actor->movePhaseTimer = 0;
        break;
    case 1:
        if (unitTelIn(unit) != 0) {
            actor->moveState++;
        }
        break;
    case 2:
        param = calcUPGet(unit);
        if (param->effectUnit != 0) {
            param->effectUnit->flags &= ~UNIT_WORK_STATUS_BLOCKED;
        }
        statEffOn(unit);
        return unitCmdNext(unit);
    }
    return 0;
}

/*
 * Blocks the unit's effect record and stops its status effects, then
 * teleports the unit out.
 */
int unitCmdFadeOut(BattleUnit *unit)
{
    Actor *actor;
    CalcUnitParam *param;

    actor = unit->work;
    switch (actor->moveState) {
    case 0:
        param = calcUPGet(unit);
        if (param->effectUnit != 0) {
            param->effectUnit->flags |= UNIT_WORK_STATUS_BLOCKED;
        }
        statEffOff(unit);
        actor->movePhaseTimer = 0;
        actor->moveState++;
        break;
    case 1:
        if (unitTelOut(unit) != 0) {
            actor->moveState++;
        }
        break;
    case 2:
        return unitCmdNext(unit);
    }
    return 0;
}

/* unitMotSet forwards to unitMotSetEx with a zero third argument. */
extern void unitMotSetEx(BattleUnit *unit, int motion, int);

void unitMotSet(BattleUnit *unit, int motion)
{
    unitMotSetEx(unit, motion, 0);
}

extern const char unitCmdMotSetMotPackFlagText[];
extern const char unitCmdIntrText[];

/*
 * Starts a motion on the unit's actor when animation data exists for
 * (arg2 << 8) + motion - 1. When arg2 is 0 the flags passed to ACT_setMotion2
 * (interpolation, 0x8, 0x20000000) are chosen from the motion id, and the effect
 * unit's UNIT_WORK_STATUS_BLOCKED bit is cleared for motions 1 and 2 and set for
 * all others.
 */
void unitMotSetEx(BattleUnit *unit, int motion, int arg2)
{
    Actor *actor;
    int flags;
    unsigned int m;
    UnitWork *eff;

    actor = unit->actor;
    if (ACT_animGetData(actor, (arg2 << 8) + motion - 1) != 0) {
        if (actor->moveFlags & 0x80000) {
            if ((motion != 6) && (motion != 0x1C)) {
                actor->moveFlags |= 0x1000;
                return;
            }
            actor->moveStepRate = 0.033333335f;
            actor->moveFlags &= 0xFFF7FFFF;
        }
        actor->motionFrame = 0;
        flags = *(unsigned int *)((char *)unit->work + 0x100) & 1;
        if (arg2 == 0) {
            if (((unsigned int)(motion - 1) < 2U) || (motion == 0x1D) || (motion == 0x17) || (motion == 0x20) || (motion == 0xE) || (motion == 0x13)) {
                flags |= 8;
            }
            m = motion - 1;
            if ((motion == 2) || (unitMotGet(unit) == 2)) {
                flags |= 1;
                unitSetInterpTime(unit, 0.26666668f);
            }
            if ((unsigned int)(motion - 4) < 2U || motion == 0x1E || motion == 0x1A) {
                flags |= 0x20000000;
            }
            eff = calcUPGet(unit)->effectUnit;
            if (m < 2U) {
                if (eff != 0) {
                    eff->flags &= ~UNIT_WORK_STATUS_BLOCKED;
                }
            } else if (eff != 0) {
                eff->flags |= UNIT_WORK_STATUS_BLOCKED;
            }
        }
        if (calcUPGet(unit)->charaId == 0xB4) {
            flags |= 0xC0000000;
        }
        printf(unitCmdMotSetMotPackFlagText, motion, arg2, flags, unit);
        if ((flags & 1) != 0) {
            printf(unitCmdIntrText, actor->interpTime);
        }
        actor->moveFlags &= 0xDFFFFFF6;
        ACT_setMotion2(actor, (arg2 << 8) + motion - 1, flags);
        actor->motion = motion - 1;
        *(int *)((char *)unit->work + 0x100) = 0;
    }
}

/*
 * unitSetInterp enables interpolation on the actor object owned by a unit
 * (the word at +0x100 of its work actor, the same slot unitMotSetEx clears).
 */
void unitSetInterp(BattleUnit *unit)
{
    Actor *object = unit->work;
    *(int *)((unsigned char *)object + 0x100) = 1;
}

void unitSetInterpTime(BattleUnit *unit, float time)
{
    unit->actor->interpTime = time;
}

/* Plays a motion on the weapon actor at weaponIndex when its animation exists, or always for motion 0xFF. */
void unitWpnMotSet(BattleUnit *unit, int weaponIndex, int animGroup, int motion)
{
    Actor *actor;
    unsigned int dataId;

    if (weaponIndex >= 0) {
        actor = unit->weapon[weaponIndex & 3];
        if (actor != 0) {
            dataId = (animGroup << 8) + motion - 1;
            if (ACT_animGetData(actor, dataId) != 0 || motion == 0xFF) {
                ACT_setMotion(actor, dataId);
            }
        }
    }
}

/*
 * Plays a motion on the unit's sub-actor (BattleUnit.subActor), when it has
 * the animation. The ACT_setMotion2 flags are 0 for motions 0x3A and 0x3B and
 * 8 otherwise. The flags start at 0 and the if sets 8, as in the original (a
 * ternary generates a different sequence); `motion - 1` is written out in both
 * calls, which keeps the original's register assignment (a named dataId local
 * swaps the two).
 */
void unitFaceMotSet(BattleUnit *unit, int motion)
{
    Actor *faceActor;
    unsigned int flags;

    flags = 0;
    if (motion != 0x3A && motion != 0x3B) {
        flags = 8; /* ACT_setMotion2's flag, not ACTOR_FLAG_HIDDEN */
    }
    faceActor = unit->subActor;
    if (faceActor != 0 && ACT_animGetData(faceActor, motion - 1) != 0) {
        ACT_setMotion2(faceActor, motion - 1, flags);
    }
}

int unitMotGet(BattleUnit *unit)
{
    return unit->actor->motion + 1;
}

/*
 * Bits of Actor.flags (unit->work) that unitMotStandSet tests: 0x800 selects
 * motion 0x1D outright; 0x8 is tested only after calcIdol2Chk returned nonzero,
 * and when set gates motion 2 on unitMotGet returning 0x1C or 0x23. This TU is
 * not the only reader of 0x800: calcHitRate (src/ov01/calc.c) tests the same
 * bit of the target's actor flags and calcTurnStart tests and clears it; what
 * the bit means for them is not evidenced here.
 */
#define ACTOR_FLAG_STAND_MOTION_1D 0x800u
#define ACTOR_FLAG_STAND_MOTION_2_GATED 0x8u

/*
 * Picks the stand motion: 0x1D when the 0x800 flag is set, motion 2 when
 * calcIdol2Chk passes and the 0x8 gate allows it, otherwise motion 1. The
 * motion 2 test is written with separate if/else arms, as the original
 * branches on it.
 */
void unitMotStandSet(BattleUnit *unit)
{
    int motion;

    if (unit->work->flags & ACTOR_FLAG_STAND_MOTION_1D) {
        motion = 0x1D;
    } else if (calcIdol2Chk(unit) != 0
               && ((unit->work->flags & ACTOR_FLAG_STAND_MOTION_2_GATED) == 0
                   || unitMotGet(unit) == 0x1C || unitMotGet(unit) == 0x23)) {
        motion = 2;
    } else {
        motion = 1;
    }
    unitMotSet(unit, motion);
}

static const char UnitNoGetFailureFormat[32] = "** unitNoGet: err -> %X\n";

/* Returns the table index of unit, or -1 after printing an error when it is not in the table. */
int unitNoGet(BattleUnit *unit)
{
    BattleUnit **units = unitTbl;
    int i;

    for (i = 0; i < 8; i++) {
        if (units[i] == unit) {
            return i;
        }
    }
    printf(UnitNoGetFailureFormat, unit);
    return -1;
}

/* This legacy accessor returns a unit pointer as its 32-bit address. */
static const char UnitPtrGetFailureFormat[32] = "** unitPtrGet: err -> %d\n";

int unitPtrGet(int index)
{
    if (index < 8) {
        return (int)unitTbl[index];
    }
    printf(UnitPtrGetFailureFormat, index);
    return 0;
}

/* unitTblGet is defined at the top of this file. */
extern int unitTblGet(int side, BattleUnit ***table);

/* Clears the hidden flag on the motion actors of the units on side. */
void unitDispOn(int side)
{
    BattleUnit **table;
    int count;
    BattleUnit **cur;
    int remaining;
    BattleUnit *unit;

    count = unitTblGet(side, &table);
    if (count > 0) {
        cur = table;
        remaining = count;
        do {
            unit = *cur;
            cur++;
            if (unit != 0) {
                /* Clears the hidden-draw flag (bit 0x8 of Actor.flags) on the motion actor. */
                unit->actor->flags &= ~ACTOR_FLAG_HIDDEN;
            }
            remaining--;
        } while (remaining != 0);
    }
}

/* Sets the hidden flag on the motion actors of the units on side. */
void unitDispOff(int side)
{
    BattleUnit **table;
    int count;
    BattleUnit **cur;
    int remaining;
    BattleUnit *unit;

    count = unitTblGet(side, &table);
    if (count > 0) {
        cur = table;
        remaining = count;
        do {
            unit = *cur;
            cur++;
            if (unit != 0) {
                unit->actor->flags |= ACTOR_FLAG_HIDDEN;
            }
            remaining--;
        } while (remaining != 0);
    }
}

/*
 * Collision distance between a unit and its target: the unit's coliMove plus
 * the target's coliTarget, divided by 100. The first term is held in a
 * callee-saved FP register across the second calcUPGet call, as the original
 * does.
 */
float unitColiGet(BattleUnit *unit, BattleUnit *targetUnit)
{
    float distance;

    distance = calcUPGet(unit)->coliMove;
    distance += calcUPGet(targetUnit)->coliTarget;
    return distance / 100.0f;
}

/*
 * Whether any unit on the side carries the CALC_UNIT_FLAG_IS_AGWS bit (0x40) in
 * CalcUnitParam.flags, the bit unitCmdMove tests to pick a warp move for a
 * player unit.
 */
int unitAgwsChk(int side)
{
    BattleUnit **table;
    int count;
    int found;
    int i;

    found = 0;
    count = unitTblGet(side, &table);
    for (i = 0; i < count; i++) {
        if (table[i] != 0 && (calcUPGet(table[i])->flags & CALC_UNIT_FLAG_IS_AGWS)) {
            found++;
        }
    }
    return found != 0;
}

/*
 * Returns 1 when a unit on the side has CalcUnitParam.charaId equal to
 * charaId, otherwise 0.
 */
int unitIdChk(int side, int charaId)
{
    BattleUnit **table;
    int count;
    int matchCount;
    int i;

    matchCount = 0;
    count = unitTblGet(side, &table);
    for (i = 0; i < count; i++) {
        if (table[i] != 0) {
            if (calcUPGet(table[i])->charaId == charaId) {
                matchCount++;
            }
        }
    }
    return matchCount != 0;
}

/* unitDispOnOff is defined below. */
extern void unitDispOnOff(int flag);

void unitDispOnAll(void)
{
    unitDispOnOff(0);
}

void unitDispOffAll(void)
{
    unitDispOnOff(1);
}

/*
 * Shows every unit with its status effects (flag 0), or hides them (any other
 * flag). The hidden flag and the effect unit's blocked bit change with it, and
 * statEffOn or statEffOff is called for each unit.
 */
void unitDispOnOff(int flag)
{
    BattleUnit **table;
    int index;
    int count;
    BattleUnit *unit;
    struct UnitWork *effectUnit;

    index = 0;
    count = unitTblGet(2, &table);
    if (count > 0) {
        do {
            unit = table[index];
            if (unit != 0) {
                if (flag == 0) {
                    unit->actor->flags &= ~ACTOR_FLAG_HIDDEN;
                    effectUnit = calcUPGet(unit)->effectUnit;
                    if (effectUnit != 0) {
                        effectUnit->flags &= ~UNIT_WORK_STATUS_BLOCKED;
                    }
                    statEffOn(table[index]);
                } else {
                    unit->actor->flags |= ACTOR_FLAG_HIDDEN;
                    effectUnit = calcUPGet(unit)->effectUnit;
                    if (effectUnit != 0) {
                        effectUnit->flags |= UNIT_WORK_STATUS_BLOCKED;
                    }
                    statEffOff(table[index]);
                }
            }
            index++;
        } while (index < count);
    }
}



static int (*unitCmdTbl[49])(BattleUnit *unit) = {
    unitCmdFree,
    unitCmdAtk,
    unitCmdEther,
    unitCmdItem,
    unitCmdGuard,
    unitCmdAgws,
    unitCmdMove,
    unitCmdStand,
    unitCmdEnd,
    unitCmdDmg,
    unitCmdEva,
    unitCmdDef,
    unitCmdDead,
    unitCmdMot,
    unitCmdTgtSet,
    (int (*)(BattleUnit *))unitCmdTurn,
    (int (*)(BattleUnit *))unitCmdTurn,
    unitCmdMoveCha,
    unitCmdMovePos,
    unitCmdMoveChaNorm,
    unitCmdMovePosNorm,
    unitCmdMoveChaRun,
    unitCmdMovePosRun,
    unitCmdMoveChaJump,
    unitCmdMovePosJump,
    unitCmdMoveChaHover,
    unitCmdMovePosHover,
    unitCmdMoveChaWarp,
    unitCmdMovePosWarp,
    unitCmdMoveChaJump2,
    unitCmdMovePosJump,
    unitCmdMoveChaHover2,
    unitCmdMovePosHover2,
    unitCmdMoveChaFloat,
    unitCmdMovePosFloat,
    unitCmdMoveChaHover3,
    unitCmdMovePosHover3,
    unitCmdBtst,
    unitCmdBted,
    (int (*)(BattleUnit *))unitCmdRec,
    unitCmdWake,
    (int (*)(BattleUnit *))unitCmdStat,
    unitCmdReadWait,
    unitCmdUnitChange,
    (int (*)(BattleUnit *))unitCmdTurn,
    unitCmdAgws2,
    unitCmdEscape,
    unitCmdFadeIn,
    unitCmdFadeOut
};

float chaosF1[5] = {
    -0.02f, -0.05f, 0.0f, 0.18f, 0.49f
};

float chaosF5[9] = {
    0.21f, 0.38f, 0.51f, 0.61f, 0.69f, 0.74f, 0.77f, 0.79f,
    0.8f
};

float chaosB1[5] = {
    0.02f, 0.07f, 0.16f, 0.31f, 0.52f
};

float chaosB5[9] = {
    0.3f, 0.52f, 0.64f, 0.71f, 0.75f, 0.78f, 0.79f, 0.8f,
    0.8f
};

float kosmosF1[5] = {
    0.0f, 0.03f, 0.09f, 0.21f, 0.44f
};

float kosmosF5[9] = {
    0.22f, 0.39f, 0.53f, 0.63f, 0.7f, 0.75f, 0.78f, 0.79f,
    0.8f
};

float kosmosB1[5] = {
    0.01f, 0.06f, 0.16f, 0.34f, 0.57f
};

float kosmosB5[9] = {
    0.25f, 0.45f, 0.59f, 0.67f, 0.72f, 0.76f, 0.78f, 0.79f,
    0.8f
};

float shionF1[5] = {
    0.01f, 0.03f, 0.07f, 0.14f, 0.26f
};

float shionF5[9] = {
    0.2f, 0.37f, 0.53f, 0.63f, 0.69f, 0.73f, 0.76f, 0.79f,
    0.8f
};

float shionB1[5] = {
    0.03f, 0.1f, 0.2f, 0.32f, 0.45f
};

float shionB5[9] = {
    0.21f, 0.35f, 0.47f, 0.57f, 0.65f, 0.71f, 0.75f, 0.78f,
    0.8f
};

float ziggyF1[5] = {
    0.01f, 0.03f, 0.08f, 0.15f, 0.26f
};

float ziggyF5[9] = {
    0.18f, 0.34f, 0.5f, 0.6f, 0.67f, 0.72f, 0.75f, 0.78f,
    0.8f
};

float ziggyB1[5] = {
    0.01f, 0.04f, 0.09f, 0.18f, 0.3f
};

float ziggyB5[9] = {
    0.17f, 0.33f, 0.48f, 0.6f, 0.7f, 0.76f, 0.79f, 0.8f,
    0.8f
};

float momoF1[5] = {
    0.02f, 0.07f, 0.17f, 0.33f, 0.5f
};

float momoF5[9] = {
    0.21f, 0.39f, 0.52f, 0.62f, 0.7f, 0.75f, 0.77f, 0.79f,
    0.8f
};

float momoB1[5] = {
    0.04f, 0.13f, 0.26f, 0.41f, 0.58f
};

float momoB5[9] = {
    0.21f, 0.38f, 0.51f, 0.62f, 0.69f, 0.74f, 0.77f, 0.79f,
    0.8f
};

float jrF1[5] = {
    0.01f, 0.06f, 0.13f, 0.24f, 0.56f
};

float jrF5[9] = {
    0.26f, 0.44f, 0.56f, 0.64f, 0.69f, 0.73f, 0.76f, 0.78f,
    0.8f
};

float jrB1[5] = {
    0.04f, 0.14f, 0.28f, 0.45f, 0.63f
};

float jrB5[9] = {
    0.21f, 0.37f, 0.48f, 0.56f, 0.63f, 0.69f, 0.74f, 0.77f,
    0.79f
};

float virgilF1[5] = {
    -0.08f, -0.05f, 0.08f, 0.27f, 0.52f
};

float virgilF5[10] = {
    0.34f, 0.59f, 0.75f, 0.85f, 0.89f, 0.91f, 0.89f, 0.85f,
    0.82f, 0.82f
};

float virgilB1[5] = {
    -0.02f, -0.05f, 0.01f, 0.18f, 0.44f
};

float virgilB5[10] = {
    0.32f, 0.52f, 0.63f, 0.69f, 0.74f, 0.77f, 0.79f, 0.8f,
    0.8f, 0.8f
};

float vx01F1[37] = {
    -0.01f, -0.02f, -0.05f, -0.09f, -0.13f, -0.18f, -0.23f, -0.28f,
    -0.34f, -0.39f, -0.44f, -0.49f, -0.54f, -0.58f, -0.61f, -0.64f,
    -0.66f, -0.67f, -0.68f, -0.69f, -0.69f, -0.69f, -0.69f, -0.69f,
    -0.69f, -0.68f, -0.66f, -0.64f, -0.6f, -0.54f, -0.47f, -0.38f,
    -0.26f, -0.1f, 0.09f, 0.33f, 0.63f
};

float vx01F5[23] = {
    0.3f, 0.5f, 0.66f, 0.79f, 0.89f, 0.96f, 1.0f, 1.04f,
    1.05f, 1.06f, 1.06f, 1.06f, 1.06f, 1.05f, 1.05f, 1.04f,
    1.03f, 1.02f, 1.01f, 1.0f, 1.0f, 1.0f, 1.0f
};

float vx01B1[27] = {
    0.0f, -0.02f, -0.03f, -0.06f, -0.08f, -0.11f, -0.14f, -0.17f,
    -0.19f, -0.21f, -0.22f, -0.24f, -0.25f, -0.25f, -0.26f, -0.26f,
    -0.26f, -0.26f, -0.26f, -0.26f, -0.23f, -0.19f, -0.11f, -0.01f,
    0.18f, 0.39f, 0.66f
};

float vx01B5[24] = {
    0.34f, 0.59f, 0.75f, 0.88f, 0.98f, 1.06f, 1.12f, 1.16f,
    1.19f, 1.21f, 1.22f, 1.22f, 1.22f, 1.22f, 1.21f, 1.19f,
    1.17f, 1.14f, 1.11f, 1.08f, 1.06f, 1.03f, 1.02f, 1.0f
};

float vx02F1[13] = {
    0.01f, 0.01f, 0.03f, 0.04f, 0.07f, 0.1f, 0.14f, 0.19f,
    0.26f, 0.33f, 0.43f, 0.55f, 0.73f
};

float vx02F5[32] = {
    0.17f, 0.5f, 0.79f, 1.03f, 1.24f, 1.4f, 1.54f, 1.64f,
    1.71f, 1.76f, 1.79f, 1.8f, 1.79f, 1.75f, 1.7f, 1.64f,
    1.57f, 1.49f, 1.41f, 1.33f, 1.26f, 1.19f, 1.12f, 1.06f,
    0.99f, 0.93f, 0.88f, 0.84f, 0.81f, 0.79f, 0.78f, 0.79f
};

float vx02B1[13] = {
    -0.02f, -0.08f, -0.14f, -0.21f, -0.25f, -0.27f, -0.27f, -0.25f,
    -0.18f, -0.06f, 0.12f, 0.35f, 0.64f
};

float vx02B5[32] = {
    0.17f, 0.46f, 0.71f, 0.92f, 1.08f, 1.22f, 1.31f, 1.39f,
    1.44f, 1.47f, 1.48f, 1.49f, 1.49f, 1.49f, 1.47f, 1.44f,
    1.41f, 1.37f, 1.32f, 1.27f, 1.22f, 1.17f, 1.11f, 1.06f,
    1.01f, 0.96f, 0.92f, 0.88f, 0.85f, 0.83f, 0.81f, 0.8f
};

float vx03F1[34] = {
    0.0f, -0.02f, -0.05f, -0.1f, -0.18f, -0.27f, -0.37f, -0.48f,
    -0.59f, -0.7f, -0.79f, -0.88f, -0.94f, -0.98f, -1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -0.97f, -0.94f,
    -0.9f, -0.84f, -0.77f, -0.67f, -0.54f, -0.39f, -0.2f, 0.03f,
    0.3f, 0.63f
};

float vx03F5[29] = {
    0.32f, 0.5f, 0.61f, 0.7f, 0.77f, 0.84f, 0.89f, 0.92f,
    0.95f, 0.97f, 0.99f, 0.99f, 1.0f, 1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f, 1.0f, 1.0f
};

float vx03B1[29] = {
    0.0f, -0.02f, -0.05f, -0.1f, -0.18f, -0.27f, -0.36f, -0.47f,
    -0.57f, -0.67f, -0.77f, -0.85f, -0.92f, -0.97f, -1.0f, -1.01f,
    -1.0f, -0.98f, -0.95f, -0.9f, -0.84f, -0.77f, -0.67f, -0.54f,
    -0.39f, -0.2f, 0.03f, 0.31f, 0.63f
};

float vx03B5[29] = {
    0.37f, 0.67f, 0.91f, 1.1f, 1.25f, 1.36f, 1.43f, 1.47f,
    1.49f, 1.5f, 1.5f, 1.5f, 1.5f, 1.48f, 1.44f, 1.37f,
    1.29f, 1.21f, 1.13f, 1.06f, 1.02f, 1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f, 1.0f, 1.0f
};

float vx06F1[29] = {
    -0.03f, -0.1f, -0.2f, -0.32f, -0.47f, -0.63f, -0.8f, -0.97f,
    -1.14f, -1.3f, -1.42f, -1.48f, -1.5f, -1.5f, -1.5f, -1.49f,
    -1.47f, -1.43f, -1.37f, -1.3f, -1.2f, -1.07f, -0.92f, -0.74f,
    -0.54f, -0.3f, -0.03f, 0.28f, 0.62f
};

float vx06F5[29] = {
    0.36f, 0.64f, 0.85f, 1.02f, 1.04f, 1.24f, 1.32f, 1.4f,
    1.47f, 1.52f, 1.56f, 1.58f, 1.59f, 1.6f, 1.6f, 1.59f,
    1.57f, 1.54f, 1.5f, 1.45f, 1.4f, 1.34f, 1.28f, 1.23f,
    1.17f, 1.12f, 1.08f, 1.04f, 1.01f
};

float vx06B1[29] = {
    -0.01f, -0.05f, -0.11f, -0.18f, -0.26f, -0.35f, -0.44f, -0.52f,
    -0.59f, -0.65f, -0.69f, -0.72f, -0.74f, -0.75f, -0.75f, -0.75f,
    -0.75f, -0.75f, -0.74f, -0.73f, -0.7f, -0.65f, -0.57f, -0.47f,
    -0.33f, -0.16f, 0.05f, 0.32f, 0.63f
};

float vx06B5[29] = {
    0.36f, 0.65f, 0.88f, 1.05f, 1.17f, 1.26f, 1.32f, 1.35f,
    1.37f, 1.4f, 1.42f, 1.43f, 1.45f, 1.46f, 1.47f, 1.48f,
    1.49f, 1.5f, 1.5f, 1.5f, 1.49f, 1.45f, 1.39f, 1.32f,
    1.25f, 1.18f, 1.11f, 1.05f, 1.01f
};

float ag04F1[55] = {
    0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, -0.02f, -0.07f, -0.14f, -0.22f, -0.29f, -0.34f,
    -0.38f, -0.41f, -0.43f, -0.44f, -0.46f, -0.47f, -0.48f, -0.48f,
    -0.49f, -0.49f, -0.49f, -0.49f, -0.49f, -0.49f, -0.48f, -0.47f,
    -0.45f, -0.42f, -0.37f, -0.3f, -0.24f, -0.16f, -0.09f, -0.04f,
    0.0f, 0.02f, 0.03f, 0.03f, 0.03f, 0.03f, 0.02f, 0.01f,
    0.0f, 0.0f, 0.02f, 0.08f, 0.2f, 0.39f, 0.65f
};

float ag04F5[10] = {
    0.33f, 0.56f, 0.7f, 0.8f, 0.89f, 0.94f, 0.97f, 0.99f,
    1.0f, 1.0f
};

float ag04B1[15] = {
    0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 0.0f, 0.04f, 0.17f, 0.43f
};

float ag04B5[44] = {
    0.36f, 0.64f, 0.84f, 0.96f, 1.0f, 1.02f, 1.07f, 1.14f,
    1.22f, 1.29f, 1.34f, 1.38f, 1.41f, 1.43f, 1.44f, 1.46f,
    1.47f, 1.47f, 1.48f, 1.49f, 1.49f, 1.49f, 1.49f, 1.49f,
    1.49f, 1.49f, 1.48f, 1.46f, 1.44f, 1.41f, 1.39f, 1.35f,
    1.32f, 1.28f, 1.25f, 1.21f, 1.17f, 1.14f, 1.11f, 1.08f,
    1.05f, 1.03f, 1.01f, 1.0f
};

float fema005F1[34] = {
    -0.01f, -0.04f, -0.1f, -0.17f, -0.26f, -0.36f, -0.47f, -0.59f,
    -0.72f, -0.85f, -0.97f, -1.1f, -1.22f, -1.33f, -1.43f, -1.52f,
    -1.6f, -1.65f, -1.69f, -1.7f, -1.7f, -1.7f, -1.7f, -1.68f,
    -1.61f, -1.5f, -1.36f, -1.17f, -0.95f, -0.7f, -0.41f, -0.1f,
    0.24f, 0.61f
};

float fema005F5[34] = {
    0.37f, 0.7f, 1.0f, 1.21f, 1.4f, 1.57f, 1.7f, 1.8f,
    1.87f, 1.93f, 1.96f, 1.98f, 2.0f, 2.0f, 2.0f, 2.0f,
    2.0f, 1.97f, 1.94f, 1.9f, 1.86f, 1.8f, 1.73f, 1.65f,
    1.57f, 1.5f, 1.4f, 1.33f, 1.25f, 1.18f, 1.12f, 1.07f,
    1.03f, 1.0f
};

float fema005B1[34] = {
    -0.01f, -0.04f, -0.1f, -0.17f, -0.26f, -0.36f, -0.47f, -0.59f,
    -0.72f, -0.85f, -0.97f, -1.1f, -1.22f, -1.33f, -1.43f, -1.52f,
    -1.6f, -1.65f, -1.69f, -1.7f, -1.7f, -1.7f, -1.7f, -1.68f,
    -1.61f, -1.5f, -1.36f, -1.17f, -0.95f, -0.7f, -0.41f, -0.1f,
    0.24f, 0.61f
};

float fema005B5[34] = {
    0.37f, 0.7f, 0.98f, 1.2f, 1.4f, 1.55f, 1.66f, 1.74f,
    1.79f, 1.8f, 1.8f, 1.79f, 1.78f, 1.76f, 1.73f, 1.7f,
    1.66f, 1.62f, 1.58f, 1.52f, 1.46f, 1.4f, 1.34f, 1.28f,
    1.23f, 1.18f, 1.14f, 1.1f, 1.08f, 1.05f, 1.03f, 1.02f,
    1.01f, 1.0f
};

float guno001F1[11] = {
    -0.03f, -0.09f, -0.15f, -0.18f, -0.17f, -0.15f, -0.09f, 0.0f,
    0.14f, 0.35f, 0.62f
};

float guno001F5[11] = {
    0.23f, 0.52f, 0.72f, 0.84f, 0.87f, 0.87f, 0.86f, 0.84f,
    0.83f, 0.81f, 0.8f
};

float guno001B1[11] = {
    -0.02f, -0.06f, -0.12f, -0.16f, -0.18f, -0.18f, -0.17f, -0.11f,
    0.01f, 0.23f, 0.57f
};

float guno001B5[11] = {
    0.24f, 0.6f, 0.85f, 1.0f, 1.08f, 1.1f, 1.08f, 1.02f,
    0.95f, 0.88f, 0.82f
};

float guno004B1[5] = {
    -0.03f, -0.06f, -0.08f, 0.06f, 0.39f
};

float guno004B5[9] = {
    0.35f, 0.6f, 0.77f, 0.86f, 0.9f, 0.9f, 0.87f, 0.84f,
    0.81f
};

float guno005B1[11] = {
    -0.01f, -0.04f, -0.08f, -0.11f, -0.12f, -0.12f, -0.1f, -0.04f,
    0.07f, 0.27f, 0.57f
};

float guno005B5[11] = {
    0.41f, 0.66f, 0.8f, 0.88f, 0.94f, 0.98f, 1.01f, 1.02f,
    1.02f, 1.01f, 1.0f
};

float guno006B1[11] = {
    -0.03f, -0.09f, -0.15f, -0.18f, -0.18f, -0.15f, -0.1f, -0.01f,
    0.14f, 0.35f, 0.64f
};

float guno006B5[11] = {
    0.17f, 0.42f, 0.62f, 0.77f, 0.87f, 0.91f, 0.9f, 0.88f,
    0.85f, 0.83f, 0.81f
};

float guno010B1[12] = {
    0.0f, 0.0f, -0.01f, -0.02f, -0.03f, -0.04f, -0.04f, -0.03f,
    -0.01f, 0.03f, 0.08f, 0.15f
};

float guno010B5[11] = {
    0.2f, 0.36f, 0.5f, 0.62f, 0.72f, 0.8f, 0.87f, 0.92f,
    0.96f, 0.98f, 1.0f
};

float guno013F1[34] = {
    0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 0.0f, -0.04f, -0.16f, -0.3f, -0.44f,
    -0.55f, -0.63f, -0.68f, -0.7f, -0.65f, -0.53f, -0.32f, -0.06f,
    0.25f, 0.6f
};

float guno013F5[29] = {
    0.36f, 0.64f, 0.84f, 0.96f, 1.0f, 0.99f, 0.95f, 0.9f,
    0.85f, 0.81f, 0.8f, 0.8f, 0.82f, 0.85f, 0.88f, 0.92f,
    0.95f, 0.97f, 0.99f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f, 1.0f, 1.0f
};

float guno013B1[34] = {
    0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 0.0f, -0.04f, -0.16f, -0.3f, -0.44f,
    -0.55f, -0.63f, -0.68f, -0.7f, -0.65f, -0.53f, -0.32f, -0.06f,
    0.25f, 0.6f
};

float guno013B5[29] = {
    0.36f, 0.64f, 0.84f, 0.96f, 1.0f, 0.99f, 0.95f, 0.9f,
    0.85f, 0.81f, 0.8f, 0.8f, 0.82f, 0.85f, 0.88f, 0.92f,
    0.95f, 0.97f, 0.99f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f, 1.0f, 1.0f
};

float guno015B1[11] = {
    0.01f, 0.03f, 0.06f, 0.11f, 0.16f, 0.22f, 0.29f, 0.36f,
    0.44f, 0.52f, 0.61f
};

float guno015B5[11] = {
    0.19f, 0.35f, 0.49f, 0.61f, 0.71f, 0.79f, 0.85f, 0.9f,
    0.95f, 0.98f, 0.99f
};

float guno017F1[11] = {
    -0.03f, -0.09f, -0.15f, -0.18f, -0.17f, -0.15f, -0.09f, 0.0f,
    0.14f, 0.35f, 0.62f
};

float guno017F5[11] = {
    0.23f, 0.52f, 0.72f, 0.84f, 0.87f, 0.87f, 0.86f, 0.84f,
    0.83f, 0.81f, 0.8f
};

float guno017B1[11] = {
    -0.02f, -0.06f, -0.12f, -0.16f, -0.18f, -0.18f, -0.17f, -0.11f,
    0.01f, 0.23f, 0.57f
};

float guno017B5[11] = {
    0.24f, 0.6f, 0.85f, 1.0f, 1.08f, 1.1f, 1.08f, 1.02f,
    0.95f, 0.88f, 0.82f
};

float guno018F1[11] = {
    -0.02f, -0.06f, -0.12f, -0.18f, -0.22f, -0.24f, -0.24f, -0.21f,
    -0.13f, 0.03f, 0.31f
};

float guno018F5[11] = {
    0.76f, 1.2f, 1.41f, 1.45f, 1.43f, 1.38f, 1.31f, 1.23f,
    1.14f, 1.07f, 1.02f
};

float guno018B1[11] = {
    -0.03f, -0.09f, -0.18f, -0.27f, -0.33f, -0.36f, -0.35f, -0.32f,
    -0.22f, -0.07f, 0.28f
};

float guno018B5[11] = {
    0.76f, 1.14f, 1.27f, 1.28f, 1.27f, 1.23f, 1.2f, 1.14f,
    1.09f, 1.04f, 1.01f
};

float guno025B1[11] = {
    0.01f, 0.03f, 0.06f, 0.11f, 0.16f, 0.22f, 0.29f, 0.37f,
    0.45f, 0.53f, 0.61f
};

float guno025B5[11] = {
    0.29f, 0.5f, 0.66f, 0.77f, 0.86f, 0.92f, 0.96f, 0.99f,
    1.0f, 1.0f, 1.0f
};

float utma006F1[29] = {
    0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f, -0.04f, -0.14f, -0.27f, -0.42f, -0.56f,
    -0.66f, -0.69f, -0.7f, -0.69f, -0.69f, -0.66f, -0.62f, -0.55f,
    -0.44f, -0.3f, -0.11f, 0.13f, 0.43f
};

float utma006F5[28] = {
    0.39f, 0.73f, 1.0f, 1.18f, 1.24f, 1.19f, 1.06f, 0.9f,
    0.77f, 0.72f, 0.73f, 0.76f, 0.79f, 0.8f, 0.8f, 0.8f,
    0.8f, 0.8f, 0.8f, 0.8f, 0.8f, 0.8f, 0.8f, 0.8f,
    0.8f, 0.8f, 0.8f, 0.8f
};

float utma006B1[29] = {
    0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -0.04f, -0.16f, -0.3f,
    -0.45f, -0.56f, -0.6f, -0.61f, -0.6f, -0.59f, -0.55f, -0.5f,
    -0.4f, -0.27f, -0.09f, 0.14f, 0.44f
};

float utma006B5[28] = {
    0.37f, 0.68f, 0.92f, 1.07f, 1.12f, 1.09f, 1.0f, 0.9f,
    0.78f, 0.7f, 0.66f, 0.67f, 0.7f, 0.73f, 0.76f, 0.79f,
    0.8f, 0.8f, 0.8f, 0.8f, 0.8f, 0.8f, 0.8f, 0.8f,
    0.8f, 0.8f, 0.8f, 0.8f
};

float utma007B1[5] = {
    0.03f, 0.08f, 0.16f, 0.28f, 0.44f
};

float utma007B5[9] = {
    0.46f, 0.76f, 0.9f, 0.94f, 0.93f, 0.91f, 0.87f, 0.84f,
    0.81f
};

float utre002B1[5] = {
    -0.01f, -0.01f, 0.05f, 0.2f, 0.45f
};

float utre002B5[5] = {
    0.36f, 0.62f, 0.75f, 0.79f, 0.8f
};

float utre003B1[5] = {
    -0.07f, -0.16f, -0.19f, -0.08f, 0.27f
};

float utre003B5[9] = {
    0.52f, 0.82f, 0.94f, 0.96f, 0.95f, 0.92f, 0.88f, 0.84f,
    0.81f
};

float utre005B1[5] = {
    0.0f, 0.02f, 0.07f, 0.16f, 0.33f
};

float utre005B5[9] = {
    0.34f, 0.57f, 0.71f, 0.78f, 0.8f, 0.8f, 0.8f, 0.8f,
    0.8f
};

float utro001B1[11] = {
    0.0f, 0.02f, 0.04f, 0.08f, 0.13f, 0.19f, 0.27f, 0.37f,
    0.48f, 0.62f, 0.85f
};

float utro001B5[11] = {
    0.23f, 0.38f, 0.52f, 0.66f, 0.77f, 0.86f, 0.92f, 0.96f,
    0.98f, 0.99f, 1.0f
};

float utro002F1[11] = {
    0.0f, 0.01f, 0.02f, 0.05f, 0.08f, 0.12f, 0.18f, 0.26f,
    0.35f, 0.46f, 0.66f
};

float utro002F5[11] = {
    0.16f, 0.3f, 0.44f, 0.57f, 0.69f, 0.78f, 0.85f, 0.9f,
    0.95f, 0.98f, 0.99f
};

float utro002B1[11] = {
    0.0f, 0.02f, 0.04f, 0.08f, 0.13f, 0.19f, 0.27f, 0.37f,
    0.48f, 0.62f, 0.85f
};

float utro002B5[11] = {
    0.23f, 0.38f, 0.52f, 0.66f, 0.77f, 0.86f, 0.92f, 0.96f,
    0.98f, 0.99f, 1.0f
};

float utso006B1[5] = {
    0.0f, 0.01f, 0.06f, 0.16f, 0.33f
};

float utso006B5[9] = {
    0.34f, 0.58f, 0.73f, 0.8f, 0.83f, 0.82f, 0.82f, 0.81f,
    0.81f
};

float boss001B1[11] = {
    0.01f, 0.02f, 0.05f, 0.09f, 0.14f, 0.2f, 0.26f, 0.33f,
    0.4f, 0.48f, 0.56f
};

float boss001B5[11] = {
    0.27f, 0.48f, 0.64f, 0.76f, 0.84f, 0.9f, 0.94f, 0.97f,
    0.99f, 1.0f, 1.0f
};

float boss004F1[9] = {
    -0.02f, -0.06f, -0.09f, -0.09f, -0.07f, 0.0f, 0.15f, 0.37f,
    0.66f
};

float boss004F5[14] = {
    0.24f, 0.42f, 0.55f, 0.66f, 0.74f, 0.81f, 0.87f, 0.91f,
    0.94f, 0.97f, 0.98f, 0.99f, 1.0f, 1.0f
};

float boss004B1[11] = {
    0.0f, -0.01f, -0.02f, -0.03f, -0.02f, 0.01f, 0.06f, 0.15f,
    0.28f, 0.46f, 0.7f
};

float boss004B5[14] = {
    0.28f, 0.48f, 0.61f, 0.7f, 0.79f, 0.85f, 0.9f, 0.94f,
    0.96f, 0.98f, 0.99f, 1.0f, 1.0f, 1.0f
};

float boss007B1[11] = {
    -0.02f, -0.06f, -0.12f, -0.17f, -0.21f, -0.23f, -0.2f, -0.11f,
    0.07f, 0.31f, 0.63f
};

float boss007B5[11] = {
    0.35f, 0.61f, 0.8f, 0.92f, 0.98f, 1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f
};

float boss012F1[24] = {
    -0.17f, -0.31f, -0.44f, -0.55f, -0.64f, -0.72f, -0.79f, -0.85f,
    -0.89f, -0.92f, -0.94f, -0.95f, -0.96f, -0.95f, -0.94f, -0.9f,
    -0.85f, -0.77f, -0.64f, -0.48f, -0.27f, -0.02f, 0.28f, 0.62f
};

float boss012F5[29] = {
    0.23f, 0.37f, 0.49f, 0.59f, 0.67f, 0.74f, 0.79f, 0.83f,
    0.88f, 0.9f, 0.93f, 0.95f, 0.96f, 0.97f, 0.98f, 0.99f,
    1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f, 1.0f, 1.0f
};

float boss012B1[24] = {
    -0.01f, -0.02f, -0.05f, -0.08f, -0.11f, -0.15f, -0.2f, -0.24f,
    -0.28f, -0.31f, -0.35f, -0.37f, -0.39f, -0.39f, -0.39f, -0.37f,
    -0.33f, -0.28f, -0.2f, -0.09f, 0.04f, 0.19f, 0.39f, 0.65f
};

float boss012B5[39] = {
    0.38f, 0.71f, 1.0f, 1.26f, 1.47f, 1.65f, 1.81f, 1.95f,
    2.07f, 2.18f, 2.27f, 2.33f, 2.38f, 2.41f, 2.42f, 2.42f,
    2.42f, 2.41f, 2.4f, 2.37f, 2.34f, 2.31f, 2.27f, 2.22f,
    2.15f, 2.08f, 1.99f, 1.9f, 1.8f, 1.7f, 1.6f, 1.5f,
    1.4f, 1.31f, 1.22f, 1.15f, 1.09f, 1.04f, 1.01f
};

float boss013B1[11] = {
    0.02f, -0.03f, -0.1f, -0.17f, -0.23f, -0.25f, -0.21f, -0.1f,
    0.08f, 0.32f, 0.63f
};

float boss013B5[11] = {
    0.37f, 0.68f, 0.93f, 1.1f, 1.2f, 1.25f, 1.23f, 1.2f,
    1.14f, 1.09f, 1.05f
};

float boss014F1[11] = {
    -0.01f, -0.04f, -0.07f, -0.1f, -0.12f, -0.11f, -0.06f, 0.03f,
    0.17f, 0.37f, 0.64f
};

float boss014F5[11] = {
    0.44f, 0.93f, 1.39f, 1.75f, 1.94f, 2.0f, 2.0f, 1.75f,
    1.38f, 1.15f, 1.03f
};

float boss014B1[11] = {
    -0.01f, -0.05f, -0.1f, -0.15f, -0.2f, -0.24f, -0.25f, -0.18f,
    0.0f, 0.27f, 0.61f
};

float boss014B5[11] = {
    0.43f, 0.9f, 1.34f, 1.7f, 1.94f, 2.0f, 1.89f, 1.69f,
    1.46f, 1.23f, 1.06f
};

float boss016F1[25] = {
    -0.01f, -0.02f, -0.05f, -0.08f, -0.12f, -0.16f, -0.21f, -0.25f,
    -0.3f, -0.34f, -0.38f, -0.41f, -0.43f, -0.45f, -0.46f, -0.46f,
    -0.45f, -0.43f, -0.39f, -0.32f, -0.22f, -0.08f, 0.1f, 0.34f,
    0.64f
};

float boss016F5[26] = {
    0.37f, 0.67f, 0.92f, 1.1f, 1.22f, 1.28f, 1.3f, 1.31f,
    1.3f, 1.27f, 1.24f, 1.19f, 1.15f, 1.1f, 1.06f, 1.02f,
    1.0f, 0.99f, 0.98f, 0.98f, 0.98f, 0.98f, 0.99f, 0.99f,
    1.0f, 1.0f
};

float boss016B1[24] = {
    0.0f, -0.01f, -0.02f, -0.04f, -0.05f, -0.07f, -0.09f, -0.11f,
    -0.13f, -0.15f, -0.17f, -0.18f, -0.19f, -0.2f, -0.2f, -0.2f,
    -0.21f, -0.2f, -0.16f, -0.1f, -0.0f, 0.15f, 0.36f, 0.64f
};

float boss016B5[27] = {
    0.39f, 0.76f, 1.08f, 1.34f, 1.5f, 1.56f, 1.56f, 1.53f,
    1.49f, 1.44f, 1.38f, 1.32f, 1.26f, 1.19f, 1.13f, 1.08f,
    1.03f, 1.0f, 0.98f, 0.97f, 0.96f, 0.97f, 0.97f, 0.98f,
    0.99f, 1.0f, 1.0f
};

float boss017F1[24] = {
    -0.16f, -0.31f, -0.45f, -0.57f, -0.68f, -0.77f, -0.85f, -0.91f,
    -0.95f, -0.98f, -0.99f, -0.99f, -0.96f, -0.92f, -0.87f, -0.8f,
    -0.72f, -0.62f, -0.5f, -0.35f, -0.17f, 0.04f, 0.3f, 0.63f
};

float boss017F5[24] = {
    0.33f, 0.6f, 0.81f, 0.95f, 1.04f, 1.1f, 1.13f, 1.15f,
    1.16f, 1.16f, 1.16f, 1.16f, 1.15f, 1.14f, 1.12f, 1.1f,
    1.09f, 1.07f, 1.06f, 1.04f, 1.03f, 1.02f, 1.01f, 1.0f
};

float boss017B1[24] = {
    -0.01f, -0.04f, -0.08f, -0.13f, -0.19f, -0.26f, -0.33f, -0.4f,
    -0.46f, -0.52f, -0.58f, -0.62f, -0.64f, -0.65f, -0.64f, -0.61f,
    -0.55f, -0.47f, -0.37f, -0.23f, -0.08f, 0.1f, 0.33f, 0.63f
};

float boss017B5[24] = {
    0.31f, 0.56f, 0.76f, 0.91f, 1.02f, 1.11f, 1.17f, 1.23f,
    1.26f, 1.28f, 1.3f, 1.3f, 1.29f, 1.28f, 1.26f, 1.23f,
    1.2f, 1.17f, 1.13f, 1.1f, 1.07f, 1.04f, 1.02f, 1.01f
};

float boss019F1[54] = {
    0.01f, 0.03f, 0.07f, 0.12f, 0.17f, 0.22f, 0.26f, 0.3f,
    0.33f, 0.34f, 0.32f, 0.29f, 0.23f, 0.13f, 0.0f, -0.21f,
    -0.53f, -0.93f, -1.39f, -1.87f, -2.37f, -2.84f, -3.28f, -3.64f,
    -3.91f, -4.12f, -4.3f, -4.45f, -4.58f, -4.67f, -4.74f, -4.78f,
    -4.79f, -4.76f, -4.71f, -4.62f, -4.5f, -4.34f, -4.14f, -3.91f,
    -3.65f, -3.38f, -3.08f, -2.77f, -2.45f, -2.11f, -1.77f, -1.42f,
    -1.07f, -0.72f, -0.37f, -0.03f, 0.32f, 0.65f
};

float boss019F5[29] = {
    0.23f, 0.44f, 0.66f, 0.86f, 1.0f, 1.1f, 1.18f, 1.24f,
    1.3f, 1.34f, 1.36f, 1.38f, 1.39f, 1.39f, 1.38f, 1.36f,
    1.34f, 1.32f, 1.29f, 1.25f, 1.22f, 1.18f, 1.15f, 1.12f,
    1.08f, 1.06f, 1.03f, 1.02f, 1.0f
};

float boss019B1[19] = {
    -0.01f, -0.05f, -0.11f, -0.17f, -0.25f, -0.32f, -0.4f, -0.46f,
    -0.5f, -0.52f, -0.51f, -0.45f, -0.35f, -0.22f, -0.06f, 0.13f,
    0.33f, 0.55f, 0.78f
};

float boss019B5[19] = {
    0.38f, 0.74f, 1.07f, 1.32f, 1.48f, 1.55f, 1.59f, 1.6f,
    1.59f, 1.56f, 1.51f, 1.45f, 1.37f, 1.3f, 1.23f, 1.16f,
    1.09f, 1.04f, 1.01f
};

float boss020F1[5] = {
    0.0f, 0.02f, 0.07f, 0.14f, 0.25f
};

float boss020F5[9] = {
    0.23f, 0.47f, 0.66f, 0.75f, 0.77f, 0.79f, 0.8f, 0.8f,
    0.8f
};

float boss020B1[5] = {
    0.0f, 0.01f, 0.03f, 0.09f, 0.2f
};

float boss020B5[9] = {
    0.17f, 0.39f, 0.61f, 0.73f, 0.76f, 0.79f, 0.8f, 0.8f,
    0.8f
};

float boss029F1[19] = {
    0.01f, 0.01f, 0.01f, 0.02f, 0.02f, 0.03f, 0.03f, 0.04f,
    0.04f, 0.03f, 0.01f, -0.01f, -0.02f, 0.0f, 0.06f, 0.22f,
    0.46f, 0.67f, 0.77f
};

float boss029F5[29] = {
    0.1f, 0.25f, 0.4f, 0.55f, 0.72f, 0.88f, 1.0f, 1.07f,
    1.12f, 1.14f, 1.15f, 1.14f, 1.11f, 1.08f, 1.05f, 1.01f,
    0.97f, 0.94f, 0.92f, 0.91f, 0.9f, 0.91f, 0.92f, 0.93f,
    0.95f, 0.96f, 0.98f, 0.99f, 1.0f
};

float boss029B1[19] = {
    -0.01f, -0.02f, -0.03f, -0.04f, -0.05f, -0.06f, -0.06f, -0.06f,
    -0.04f, -0.01f, 0.02f, 0.07f, 0.14f, 0.21f, 0.31f, 0.43f,
    0.57f, 0.73f, 0.89f
};

float boss029B5[29] = {
    0.25f, 0.41f, 0.55f, 0.68f, 0.71f, 0.86f, 0.92f, 0.97f,
    1.01f, 1.04f, 1.06f, 1.07f, 1.07f, 1.07f, 1.07f, 1.07f,
    1.06f, 1.06f, 1.06f, 1.07f, 1.07f, 1.07f, 1.07f, 1.06f,
    1.04f, 1.03f, 1.02f, 1.01f, 1.0f
};

UnitPosAdj adjTblPl[8] = {
    { 1, chaosF1, chaosF5, chaosB1, chaosB5 },
    { 2, kosmosF1, kosmosF5, kosmosB1, kosmosB5 },
    { 3, shionF1, shionF5, shionB1, shionB5 },
    { 5, ziggyF1, ziggyF5, ziggyB1, ziggyB5 },
    { 6, momoF1, momoF5, momoB1, momoB5 },
    { 7, jrF1, jrF5, jrB1, jrB5 },
    { 10, virgilF1, virgilF5, virgilB1, virgilB5 },
    { 0, 0, 0, 0, 0 }
};

UnitPosAdj adjTblEn[42] = {
    { 17, vx01F1, vx01F5, vx01B1, vx01B5 },
    { 18, vx02F1, vx02F5, vx02B1, vx02B5 },
    { 19, vx03F1, vx03F5, vx03B1, vx03B5 },
    { 22, vx06F1, vx06F5, vx06B1, vx06B5 },
    { 26, ag04F1, ag04F5, ag04B1, ag04B5 },
    { 67, fema005F1, fema005F5, fema005B1, fema005B5 },
    { 34, guno001F1, guno001F5, guno001B1, guno001B5 },
    { 50, guno001F1, guno001F5, guno001B1, guno001B5 },
    { 59, guno001F1, guno001F5, guno001B1, guno001B5 },
    { 37, 0, 0, guno004B1, guno004B5 },
    { 38, 0, 0, guno005B1, guno005B5 },
    { 39, 0, 0, guno006B1, guno006B5 },
    { 43, 0, 0, guno010B1, guno010B5 },
    { 46, guno013F1, guno013F5, guno013B1, guno013B5 },
    { 48, 0, 0, guno015B1, guno015B5 },
    { 50, guno017F1, guno017F5, guno017B1, guno017B5 },
    { 51, guno018F1, guno018F5, guno018B1, guno018B5 },
    { 58, 0, 0, guno025B1, guno025B5 },
    { 82, utma006F1, utma006F5, utma006B1, utma006B5 },
    { 83, 0, 0, utma007B1, utma007B5 },
    { 92, 0, 0, utre002B1, utre002B5 },
    { 93, 0, 0, utre003B1, utre003B5 },
    { 95, 0, 0, utre005B1, utre005B5 },
    { 97, 0, 0, utro001B1, utro001B5 },
    { 98, utro002F1, utro002F5, utro002B1, utro002B5 },
    { 104, 0, 0, utso006B1, utso006B5 },
    { 151, 0, 0, boss001B1, boss001B5 },
    { 154, boss004F1, boss004F5, boss004B1, boss004B5 },
    { 159, boss004F1, boss004F5, boss004B1, boss004B5 },
    { 160, boss004F1, boss004F5, boss004B1, boss004B5 },
    { 161, boss004F1, boss004F5, boss004B1, boss004B5 },
    { 185, boss004F1, boss004F5, boss004B1, boss004B5 },
    { 157, 0, 0, boss007B1, boss007B5 },
    { 162, boss012F1, boss012F5, boss012B1, boss012B5 },
    { 163, 0, 0, boss013B1, boss013B5 },
    { 164, boss014F1, boss014F5, boss014B1, boss014B5 },
    { 166, boss016F1, boss016F5, boss016B1, boss016B5 },
    { 167, boss017F1, boss017F5, boss017B1, boss017B5 },
    { 169, boss019F1, boss019F5, boss019B1, boss019B5 },
    { 170, boss020F1, boss020F5, boss020B1, boss020B5 },
    { 179, boss029F1, boss029F5, boss029B1, boss029B5 },
    { 0, 0, 0, 0, 0 }
};

const char unitCmdCreateIDUAPOText[48] = "** Create (ID=%d) U=%X A=%X P=%X O=%X **\n";

const char unitCmdRemoveUAPOText[40] = "** Remove (%d) U=%X A=%X P=%X O=%X **\n";

const char unitCmdUnitEnInitENOverText[24] = "** unitEnInit: EN over\n";

const char unitCmdJobTypeErrTidText[32] = "** JobType err -> %d tid=%d\n";

const char unitCmdTgtSetErrText[24] = "\n** unitCmdTgtSet: err\n";

const char unitCmdMoveChaErrText[40] = "** unitCmdMoveCha: err -> %d\n";

const char unitCmdMotSetMotPackFlagText[48] = "** MotSet: Mot=%d Pack=%d flag=%X (%X) **\n";

const char unitCmdIntrText[16] = "** Intr=%f\n";

static unsigned char sSp[24];

static float cPoint[32];
