/*
 * OV01 original TU 2: 0x00a00d60..0x00a07770 (119 functions)
 */
#include "common.h"
#include "shared.h"
#include "unit_cmd.h"

/* The same eight unit pointers have full-record and scheduler-prefix views. */
typedef union UnitTableStorage {
    UnitRecord *records[8];
    ObjectTask *tasks[8];
} UnitTableStorage;
static UnitTableStorage unitTbl;
static int unitPlNum;
static int unitEnNum;

void unitInit(void)
{
    memset(&unitTbl, 0, sizeof(unitTbl));
    unitPlNum = 0;
    unitEnNum = 0;
}

int unitTblGet(int side, UnitRecord ***table)
{
    if (side == 0) {
        *table = unitTbl.records;
        return 3;
    }
    if (side == 1) {
        *table = &unitTbl.records[3];
        return 5;
    }
    *table = unitTbl.records;
    return 8;
}

/*
 * The side's first slot is its own local, copied into the loop index where
 * the two side paths meet (daddu $6,$2,$0 at 0x00a00e18); starting the loop
 * index itself in each branch allocates it to $6 directly.
 */
int unitTblSet(int side, ObjectTask *unit)
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
        if (unitTbl.tasks[i] == 0) {
            unitTbl.tasks[i] = unit;
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

int unitTblRemove(ObjectTask *unit)
{
    ObjectTask **units = unitTbl.tasks;
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

int unitTblChg(ObjectTask *oldUnit, ObjectTask *newUnit)
{
    ObjectTask **units = unitTbl.tasks;
    int i;

    for (i = 0; i < 8; i++) {
        if (units[i] == oldUnit) {
            units[i] = newUnit;
            return 1;
        }
    }
    return 0;
}

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

int unitLiveNumGet(int side)
{
    UnitRecord **table;
    UnitRecord **cur;
    UnitRecord *unit;
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
                if (!(((Actor *)unit->task.work)->flags & 0x80)) {
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
 * 0 or 4 (calcStatGet at 0x00a01064 and 0x00a01080).
 */
int unitLiveNumGet2(int side)
{
    UnitRecord **table;
    UnitRecord *unit;
    int count;
    int live;
    int i;

    live = 0;
    count = unitTblGet(side, &table);
    for (i = 0; i < count; i++) {
        unit = table[i];
        if (unit != 0 && !(((Actor *)unit->task.work)->flags & ACTOR_FLAG_DOWN)) {
            if (calcStatGet(&unit->task, 0, 0x2000) == 0
                && calcStatGet(&table[i]->task, 4, 0x2000) == 0) {
                live++;
            }
        }
    }
    return live;
}

int unitCidGet(ObjectTask *unit)
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
void unitPlFunc(ObjectTask *unit);

/* Original OV01 formatting strings, with their full 32-byte slots retained. */
static const char UnitPlCreateAgwsFormat[32] =
    "unitPlCreate: AGWS (pilot %d)\n";
#define D_00A438F0 UnitPlCreateAgwsFormat
#define D_00A43BD8 UnitNoGetFailureFormat
#define D_00A43BF8 UnitPtrGetFailureFormat

/*
 * unitCreate/unitTblSet/unitLoad are unitPlCreate's own siblings; unitCreate
 * is still asm in this TU (also forward-declared below for unitEnCreate's
 * own use), unitTblSet is defined above and unitLoad later in this file.
 * unitTblSet returns int, not void: it leaves 1 or 0 in $2 (addiu $2,$0,0x1
 * at 0x00a00e80, daddu $2,$0,$0 at 0x00a00e98).
 */
extern ObjectTask *unitCreate(s16 *pl, int side, ObjectTaskCallback func);
void unitLoad(ObjectTask *unit, int mode);

ObjectTask *unitPlCreate(s16 *pl)
{
    ObjectTask *unit;
    ObjectTask *pilotUnit;
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
        printf(D_00A438F0, pilotId);
    }
    return unit;
}

/*
 * unitCreate is unitEnCreate's own sibling, still asm in this TU;
 * forward-declared above for the call it makes before its definition
 * appears later in the file.
 */

void unitEnFunc(ObjectTask *unit);

ObjectTask *unitEnCreate(s16 *pl)
{
    ObjectTask *unit;

    unit = unitCreate(pl, 1, unitEnFunc);
    if (unit != 0) {
        unitTblSet(1, unit);
        unitLoad(unit, *pl);
    }
    return unit;
}

/*
 * The player-character origin record dataUnitOrgGet (src/ov01/data_unit_org_get.c)
 * returns, as far as unitAgwsPilotGet reads it: the equipped AGWS at +0x54
 * (`lh $3,0x54($2)`, 0x00a012a8), CalcCharParaData.agwsId in src/ov01/calc.h.
 */
typedef struct UnitOrgAgws {
    unsigned char unmodeled_00[0x54];
    short agwsId; /* +0x54 */
} UnitOrgAgws;

extern UnitOrgAgws *dataUnitOrgGet(int charaId);

/*
 * The AGWS character ids (`addiu $2,$17,-0x11`/`sltiu $2,$2,0x10`) and the
 * player characters that can pilot one (`slti $2,$16,0x11`).
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

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCreate);

/*
 * Disposes the unit's actors: the optional sub-actor, the three weapon slots
 * (indexed `slot & 3` like every other wpnActor access) and finally the
 * motion actor. The weapon slot is reached as pointer arithmetic on the
 * array: the subscript form makes the compiler hoist unit+12 as a loop base
 * (+16 displacement), the original keeps unit itself with +0x1C.
 */
void unitRemoveActor(UnitRecord *unit)
{
    Actor *motionActor;
    Actor *actor;
    int i;

    motionActor = unit->motionActor;
    actor = unit->subActor;
    if (actor != 0) {
        ACT_dispose(actor);
    }
    for (i = 0; i < 3; i++) {
        actor = *(unit->wpnActor + (i & 3));
        if (actor != 0) {
            ACT_dispose(actor);
        }
    }
    ACT_dispose(motionActor);
}

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitRemove);

void unitLoad(ObjectTask *unit, int mode)
{
    CalcUnitParam *up;

    up = calcUPGet(unit);
    if (calcUPGet(unit)->charaId < 0xBB || calcUPGet(unit)->charaId >= 0xC3) {
        if (!(((Actor *)unit->work)->flags & ACTOR_FLAG_ENEMY)) {
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

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitParaInit);

/*
 * plTbl's stride (8 bytes per entry, three entries) and its leading short are
 * the only bytes unitPlInit demonstrates; unitPlCreate is called with the
 * entry pointer itself whenever that leading short is nonzero.
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

extern const char D_00A43968[];

void unitEnInit(s16 *pl)
{
    int count;

    count = 0;
    for (;;) {
        if (pl[0] == 0) {
            break;
        }
        if (count >= 5) {
            printf(D_00A43968);
            break;
        }
        count++;
        unitEnCreate(pl);
        pl += 4;
    }
}

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitClipSet);

void unitPlFunc(ObjectTask *unit)
{
    unitCmdExec(unit);
    statEffProc(unit);
    unitClipSet(unit);
}

void unitEnFunc(ObjectTask *unit)
{
    unitCmdExec(unit);
    statEffProc(unit);
    unitClipSet(unit);
}

/*
 * Defined in a different translation unit (src/ov01/obj.c, ov01/tu001), its
 * published exact_c; unitCmdClear forwards its own unit argument straight
 * through to it.
 */
extern void objCmdClear(ObjectTask *unit);

/*
 * unitCmdClear also nulls out its unit's Actor.update callback (declared
 * above), alongside clearing the shared object command queue.
 */
void unitCmdClear(ObjectTask *unit)
{
    Actor *actor;

    objCmdClear(unit);
    actor = unit->work;
    actor->update = 0;
}

void *unitCmdTailGet(ObjectTask *unit)
{
    return objCmdTailGet(unit);
}

void *unitCmdPtrGet(ObjectTask *unit)
{
    return objCmdPtrGet(unit);
}

int unitCmdNext(ObjectTask *unit)
{
    return objCmdNext(unit);
}

void unitCmdSet(ObjectTask *unit, CmdEntry *cmd)
{
    Actor *actor;
    CmdEntry *entry;

    actor = unit->work;
    entry = objCmdPush(unit);
    actor->runFrameCounter = 0;
    actor->moveState = 0;
    *entry = *cmd;
    if (entry->selector != 0) {
        actor->flags |= ACTOR_FLAG_CMD_PENDING;
    }
}

void unitCmdExec(ObjectTask *unit)
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

int unitCmdFree(ObjectTask *unit)
{
    Actor *actor;

    actor = unit->work;
    actor->flags &= ~ACTOR_FLAG_CMD_PENDING;
    return 0;
}

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdAtk);

extern const char D_00A43980[];

/*
 * The error report is wrapped the way a debug-print macro wraps it, in a
 * do/while (0) block: the original calls printf with jal and returns
 * normally, while a bare trailing printf call is turned into a sibling call.
 * The two range tests stay nested: the original branches on each one (lb),
 * where a single `&&` folds them into one unsigned compare.
 */
#define UNIT_ERROR_PRINT(args) do { printf args; } while (0)

void unitCmdAtkSub(ObjectTask *unit, UnitActRequest *request)
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
    UNIT_ERROR_PRINT((D_00A43980, tec->jobType, request->id));
}

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitJobTypeAtk);

/*
 * unitCmdAtkEnd's own siblings, both defined later in this TU; forward-
 * declared for its calls (an implicit declaration would mismatch the
 * definitions' void return), as the existing unitMotSet declaration before
 * unitCmdEva does for that function.
 */
extern void unitMotSet(ObjectTask *unit, int motion);
extern void unitWpnMotSet(ObjectTask *unit, int weaponIndex, int animGroup, int motion);

void unitCmdAtkEnd(ObjectTask *unit)
{
    CmdEntry cmd;

    unitCmdClear(unit);
    if (unitMotGet(unit) == 0x20) {
        cmd.selector = 0xD;
        cmd.argument = 0x21;
        cmd.extra = 0;
        unitCmdSet(unit, &cmd);
        unitWpnMotSet(unit, dataDefWpnGet((UnitRecord *) unit), 0, 0x32);
        if (calcUPGet(unit)->charaId == 7) {
            unitWpnMotSet(unit, 1, 0, 0x32);
        }
    } else {
        unitMotSet(unit, 1);
    }
    unitFaceMotSet(unit, 0x35);
}

int unitCmdDeadSet(ObjectTask *unit)
{
    CmdEntry cmd;

    if (unit == 0 || (((Actor *) unit->work)->flags & 0x80)) {
        return 0;
    }
    if (*(short *) &calcUPGet(unit)->currentHp != 0) {
        return 0;
    }
    cmd.selector = 0xC;
    unitCmdSet(unit, &cmd);
    return 1;
}

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdEther);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdItem);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdItemSub);

int unitCmdMove(UnitRecord *unit)
{
    Actor *actor;
    CalcUnitParam *calcParam;
    int useWarp;

    actor = unit->task.work;
    unitCmdPtrGet(&unit->task);
    if (!(*(u32 *)unit->task.work & ACTOR_FLAG_ENEMY)) {
        calcParam = calcUPGet(&unit->task);
        useWarp = calcParam->flags & 0x40;
    } else {
        calcParam = calcUPGet(&unit->task);
        useWarp = calcParam->moveKindId == 4;
    }
    switch (actor->moveState) {
    case 0:
        unitCmdDstSet(&unit->task, 3, 1);
        if (!useWarp) {
            unitCmdDirSet(unit);
        }
        calcUPGet(&unit->task);
        unitCmdVASet(&unit->task, 3, 0);
        actor->movePhase = 0;
        actor->moveState++;
        break;
    case 1:
        if (!useWarp) {
            if (unitCmdJump(&unit->task, 3, 1, 1) != 0) {
                unitCmdMoveNext(&unit->task, 1);
            }
        } else if (unitCmdWarp(&unit->task, 3, 1) != 0) {
            unitCmdMoveNext(&unit->task, 1);
        }
        break;
    }
    return 0;
}

/*
 * Guard: plays motion 0x1B and marks the unit as guarding
 * (ACTOR_FLAG_GUARD), then stands the unit once the motion ends.
 */
int unitCmdGuard(ObjectTask *unit)
{
    Actor *actor;

    actor = unit->work;
    unitCmdPtrGet(unit);
    if (actor->moveState == 0) {
        unitMotSet(unit, 0x1B);
        /* The work slot is loaded again after the call (lw $2,0x10($16) at
         * 0x00a0275c), not read through the cached `actor`. */
        ((Actor *)unit->work)->flags |= ACTOR_FLAG_GUARD;
        actor->moveState++;
    } else if (((UnitRecord *)unit)->motionActor->moveFlags & 0x1000) {
        unitMotStandSet(unit);
        return unitCmdNext(unit);
    }
    return 0;
}

/*
 * Queues the AGWS boarding pair: command 0x2B (with the queued argument and
 * the unit's character id: the linked unit's when CalcUnitParam flag 0x40 is
 * set, lw $2,0x50/lh $17,0x38($2) at 0x00a0280c, else baseCharaId,
 * lh $17,0x54($3) at 0x00a02818) and then command 0x2D from the same entry.
 */
int unitCmdAgws(ObjectTask *unit)
{
    CmdEntry entry;
    CmdEntry *cmd;
    CalcUnitParam *param;
    int charaId;
    int effectId;
    int soundId;

    cmd = unitCmdPtrGet(unit);
    param = calcUPGet(unit);
    if (param->flags & 0x40) {
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
    if (calcUPGet(unit)->flags & 0x40) {
        effectId = 0x67;
        soundId = 0x20;
    } else {
        /* Both set before the call: addiu $17,0x66 and the delay-slot
         * addiu $18,0x1F around jal sndSeTransPlay (0x00a02888). */
        effectId = 0x66;
        soundId = 0x1F;
        sndSeTransPlay(unit, 0x17, 3);
    }
    decoEffCall(unit, effectId);
    sndSysSePlay(soundId);
    return unitCmdNext(unit);
}

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdUnitChange);

int unitCmdStand(ObjectTask *unit)
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

int unitCmdEnd(ObjectTask *unit)
{
    return unitCmdStand(unit);
}

/*
 * Damage reaction: the queued argument picks the hit motion and face (1 and
 * 2 the heavy ones, face 0x3B), and a kind-2 hit on a front position
 * (CalcUnitParam.posId < 4, lh $3,0x44($2) at 0x00a02df8) knocks the unit
 * three positions back and snaps its motion actor onto the new position.
 */
int unitCmdDmg(ObjectTask *unit)
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
        case 0: /* its own case: slti $2,$17,2 at 0x00a02d7c */
        default:
            motion = 4;
            face = 0x3A;
            break;
        }
        unitMotSet(unit, motion);
        unitFaceMotSet(unit, face);
        sndSeTransPlay(unit, 2, kind + 1);
        actor->moveState++;
    } else if (((UnitRecord *)unit)->motionActor->moveFlags & 0x1000) {
        if (kind == 2 && calcUPGet(unit)->posId < 4) {
            param = calcUPGet(unit);
            param->posId += 3;
            pos = dataPosTblGet(param->posId);
            motionActor = ((UnitRecord *)unit)->motionActor;
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

/*
 * unitMotSet's own sibling, still asm in this TU; forward-declared for the
 * call unitCmdEva makes before its own definition appears later in the
 * file.
 */
extern void unitMotSet(ObjectTask *unit, int motion);

int unitCmdEva(ObjectTask *unit)
{
    Actor *actor;
    Actor *motionActor;

    actor = unit->work;
    if (actor->moveState == 0) {
        unitMotSet(unit, 0x1A);
        actor->moveState++;
    } else {
        motionActor = ((UnitRecord *) unit)->motionActor;
        if (motionActor->moveFlags & 0x1000) {
            unitMotStandSet(unit);
            return unitCmdNext(unit);
        }
    }
    return 0;
}

/*
 * Defence: plays motion 0x1E, then, once it ends, motion 0x1C unless the
 * unit is guarding (Actor.flags 0x800, andi at 0x00a02fac), and stands the
 * unit when the last motion ends.
 */
int unitCmdDef(ObjectTask *unit)
{
    Actor *actor;

    actor = unit->work;
    switch (actor->moveState) {
    case 0:
        unitMotSet(unit, 0x1E);
        actor->moveState++;
        break;
    case 1:
        if (((UnitRecord *)unit)->motionActor->moveFlags & 0x1000) {
            if (!(actor->flags & ACTOR_FLAG_GUARD)) {
                unitMotSet(unit, 0x1C);
            }
            actor->moveState++;
        }
        break;
    default:
        if (((UnitRecord *)unit)->motionActor->moveFlags & 0x1000) {
            unitMotStandSet(unit);
            return unitCmdNext(unit);
        }
        break;
    }
    return 0;
}

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdDead);

int unitDeadSpecChk(ObjectTask *unit)
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
 * (extra == 0: the motion actor's moveFlags bit 0x1000) or for extra frames
 * of Actor.runFrameCounter (bne $3,$2 at 0x00a033b0).
 */
int unitCmdMot(ObjectTask *unit)
{
    Actor *actor;
    CmdEntry *cmd;

    actor = unit->work;
    cmd = unitCmdPtrGet(unit);
    if (actor->moveState == 0) {
        unitMotSet(unit, cmd->argument);
        actor->moveState++;
    } else if (cmd->extra == 0 ? (((UnitRecord *)unit)->motionActor->moveFlags & 0x1000) != 0
                               : cmd->extra == actor->runFrameCounter) {
        unitMotStandSet(unit);
        return unitCmdNext(unit);
    }
    unitMtdProc((UnitRecord *)unit);
    return 0;
}

extern const char D_00A43AB8[];

int unitCmdTgtSet(ObjectTask *unit)
{
    Actor *actor;
    CmdEntry *cmd;
    TgtCmd tgtCmd;
    ObjectTask *target;

    actor = unit->work;
    cmd = unitCmdPtrGet(unit);
    if (actor->moveState != 0) {
        return 0;
    }
    tgtCmd.tecId = cmd->argument;
    tgtCmd.target = (ObjectTask *) cmd->extra;
    tgtCmd.itmId = cmd->itmId;
    tgtCmd.mode = cmd->mode;
    target = tgtFindFirst(unit, &tgtCmd);
    if (target != 0) {
        /* The work slot is loaded again here, not read through the cached
         * `actor`: reusing the local trades two callee-saved registers and
         * the OV01.OVL gate no longer matches (tried, unitCmdTgtSet). */
        ((Actor *) unit->work)->targetUnit = target;
        return unitCmdNext(unit);
    }
    printf(D_00A43AB8);
    cmdListCansel(unit);
    unitCmdClear(unit);
    return unitCmdNext(unit);
}

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdTurn);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdTurnSub);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdTurnSubInit);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdMoveCha);

/*
 * calcUPGet(unit)+0x44 (ov01/tu004 calc.c's own unmodeled span) is the same
 * position-table-index half unitCmdDstSet and unitCmdDirAdj read later in
 * this file (CALC_UNIT_PARAM_UNK44_OFF below); unitCmdMoveNext is its only
 * claimed writer, copying it from the queued command entry's own index at
 * cmdEntry+0x4 (CMD_ENTRY_X_OR_IDX_OFF below, also read there). Offset
 * 0x4C is a float this TU does not otherwise claim, restored into the
 * motion actor's yaw on every call.
 */
#define CALC_UNIT_PARAM_UNK44_OFF 0x44
#define CMD_ENTRY_X_OR_IDX_OFF 0x4
#define CALC_UNIT_PARAM_DEFAULT_YAW_OFF 0x4C

int unitCmdMoveNext(ObjectTask *unit, int mode)
{
    Actor *actor;
    void *cmdEntry;

    actor = ((UnitRecord *) unit)->motionActor;
    cmdEntry = unitCmdPtrGet(unit);
    actor->rotation.y = *(float *)((char *)calcUPGet(unit) + CALC_UNIT_PARAM_DEFAULT_YAW_OFF);
    if (mode == 1) {
        *(u16 *)((char *)calcUPGet(unit) + CALC_UNIT_PARAM_UNK44_OFF) =
            *(u16 *)((char *)cmdEntry + CMD_ENTRY_X_OR_IDX_OFF);
    } else {
        unitCmdDstSet(unit, 0, 0);
        unitCmdDirSet((UnitRecord *) unit);
    }
    if (mode == 1) {
        unitMotStandSet(unit);
    }
    return unitCmdNext(unit);
}

/*
 * unitCmdMoveChaNorm's, unitCmdMoveChaJump2's and unitCmdMovePosNorm's own
 * siblings, defined later in this TU and forward-declared for their
 * calls. unitCmdLenGet returns its distance in $f0, which all three hand on as
 * unitCmdDivSet's distance (mov.s $f12,$f0 at 0x00a03eb0,
 * 0x00a044f4 and 0x00a0416c).
 */
extern float unitCmdLenGet(ObjectTask *unit);
extern void unitCmdDivSet(UnitRecord *unit, float distance, float step);

/*
 * A run up to the target unit followed by a jump onto it: step 0 hands
 * unitCmdDivSet the distance unitCmdLenGet returned and that distance less
 * 255.0f as the step (0x437F0000 through lui/mtc1, sub.s $f13,$f12,$f13 at
 * 0x00a03ebc), step 1 waits for the run, step 2 starts the jump and the last
 * step waits for it to land.
 */
int unitCmdMoveChaNorm(ObjectTask *unit)
{
    Actor *actor;
    float length;

    actor = unit->work;
    switch (actor->moveState) {
    case 0:
        length = unitCmdLenGet(unit);
        unitCmdDivSet((UnitRecord *)unit, length, length - 255.0f);
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

int unitCmdMoveChaRun(ObjectTask *unit)
{
    if (unitCmdRun(unit, 1, 0) != 0) {
        unitCmdMoveNext(unit, 0);
    }
    return 0;
}

int unitCmdMoveChaJump(ObjectTask *unit)
{
    if (unitCmdJump(unit, 3, 0, 0) != 0) {
        unitCmdMoveNext(unit, 0);
    }
    return 0;
}

int unitCmdMoveChaHover(ObjectTask *unit)
{
    if (unitCmdHover(unit, 5, 0) != 0) {
        unitCmdMoveNext(unit, 0);
    }
    return 0;
}

int unitCmdMoveChaWarp(ObjectTask *unit)
{
    if (unitCmdWarp(unit, 7, 0) != 0) {
        unitCmdMoveNext(unit, 0);
    }
    return 0;
}

/*
 * A three-hop jump to the command's target: step 0 stores distance / 3 in
 * hopLength and passes unitCmdDivSet the whole distance and hopLength as the
 * step; steps 1..3 each wait for unitCmdJump to land, aim the next hop and
 * pass unitCmdDivSet the remaining distance with hopLength as the step, or
 * that remaining distance itself as the step for the last hop (beq $2,$18
 * at 0x00a04168 skips the lwc1 $f0,0($19) at 0x00a04170); the step after
 * the last hop finishes the move. Nothing is divided by hopLength: the only
 * division is the one by 3 cited below. The work slot points at the Actor record;
 * it is read again after unitCmdPtrGet (lw $2,0x10($16) at 0x00a040d0) for
 * the hopLength address. hops is an int local, not a 3.0f literal: the
 * original keeps 3 in $18 and converts it for the division (mtc1 $18,$f13;
 * cvt.s.w $f13,$f13; div.s $f13,$f0,$f13 at 0x00a040f0..0x00a0410c), while a
 * float literal is loaded ready-made with lui/mtc1.
 */
int unitCmdMoveChaJump2(ObjectTask *unit)
{
    Actor *actor;
    float *hopLength;
    int hops;
    float length;

    hops = 3;
    actor = unit->work;
    unitCmdPtrGet(unit);
    hopLength = &((Actor *)unit->work)->hopLength;
    if (actor->moveState == 0) {
        length = unitCmdLenGet(unit);
        *hopLength = length / hops;
        unitCmdDivSet((UnitRecord *)unit, length, *hopLength);
        unitCmdVASet(unit, 3, 0);
        actor->moveState++;
    } else if (actor->moveState > hops) {
        unitCmdMoveNext(unit, 0);
    } else if (unitCmdJump(unit, 3, 2, 0) != 0) {
        actor->movePhase = 0;
        unitCmdDstSet(unit, 3, 0);
        length = unitCmdLenGet(unit);
        unitCmdDivSet((UnitRecord *)unit, length,
                      actor->moveState == hops ? length : *hopLength);
        unitCmdVASet(unit, 3, 0);
        actor->moveState++;
    }
    return 0;
}

int unitCmdMoveChaHover2(ObjectTask *unit)
{
    if (unitCmdJump(unit, 9, 0, 0) != 0) {
        unitCmdMoveNext(unit, 0);
    }
    return 0;
}

int unitCmdMoveChaFloat(ObjectTask *unit)
{
    if (unitCmdFloat(unit, 0xB, 0) != 0) {
        unitCmdMoveNext(unit, 0);
    }
    return 0;
}

int unitCmdMoveChaHover3(ObjectTask *unit)
{
    if (unitCmdJump(unit, 0xD, 0, 0) != 0) {
        unitCmdMoveNext(unit, 0);
    }
    return 0;
}

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdMovePos);

/*
 * Step 0 passes unitCmdDivSet the distance unitCmdLenGet returned (mov.s
 * $f12,$f0 at 0x00a044f4) and the step 255.0f (0x437F0000 through lui/mtc1
 * into $f13 at 0x00a044e8/0x00a044ec); nothing here divides by 255.0f.
 * The queued command's argument word is parked in Actor.targetUnit for the jump
 * (lw $2,0x4($4); sw $2,0x80($17) at 0x00a044b4/0x00a044e0) and written back
 * before the run's unitCmdDstSet reads it (sw $2,0x4($4) at 0x00a04568).
 */
int unitCmdMovePosNorm(ObjectTask *unit)
{
    Actor *actor;
    CmdEntry *cmd;

    actor = unit->work;
    cmd = unitCmdPtrGet(unit);
    switch (actor->moveState) {
    case 0:
        actor->targetUnit = (ObjectTask *)cmd->argument;
        unitCmdDivSet((UnitRecord *)unit, unitCmdLenGet(unit), 255.0f);
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
        unitCmdDirSet((UnitRecord *)unit);
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

int unitCmdMovePosRun(ObjectTask *unit)
{
    if (unitCmdRun(unit, 2, 1) != 0) {
        unitCmdMoveNext(unit, 1);
    }
    return 0;
}

int unitCmdMovePosJump(ObjectTask *unit)
{
    if (unitCmdJump(unit, 4, 1, 0) != 0) {
        unitCmdMoveNext(unit, 1);
    }
    return 0;
}

int unitCmdMovePosHover(ObjectTask *unit)
{
    if (unitCmdHover(unit, 6, 1) != 0) {
        unitCmdMoveNext(unit, 1);
    }
    return 0;
}

int unitCmdMovePosWarp(ObjectTask *unit)
{
    if (unitCmdWarp(unit, 8, 1) != 0) {
        unitCmdMoveNext(unit, 1);
    }
    return 0;
}

int unitCmdMovePosHover2(ObjectTask *unit)
{
    if (unitCmdJump(unit, 0xA, 1, 0) != 0) {
        unitCmdMoveNext(unit, 1);
    }
    return 0;
}

int unitCmdMovePosFloat(ObjectTask *unit)
{
    if (unitCmdFloat(unit, 0xC, 1) != 0) {
        unitCmdMoveNext(unit, 1);
    }
    return 0;
}

int unitCmdMovePosHover3(ObjectTask *unit)
{
    if (unitCmdJump(unit, 0xE, 1, 0) != 0) {
        unitCmdMoveNext(unit, 1);
    }
    return 0;
}

/*
 * ov01/tu004 calc.c owns CalcUnitParam and does not model this signed half;
 * unitCmdDirAdj is the only claimed reader, using its sign to flip which side
 * of the move target the retreat offset below is placed on.
 */
#define CALC_UNIT_PARAM_UNK44_OFF 0x44

void unitCmdDirAdj(ObjectTask *unit, int cmdId, int flip)
{
    Actor *actor = unit->work;
    Actor *motionActor = ((UnitRecord *)unit)->motionActor;
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
 * Moves the unit along the authored root-motion curve of its current step
 * motion: the per-frame offset is the curve's value at the motion actor's
 * motionFrame less the previous frame's (lwc1 $f0,-0x4($2), sub.s at
 * 0x00a04a9c/0x00a04aa0), clamped to the distance left to the command target.
 * The target is blended by unitCmdDivSet, copied into the motion actor's
 * position and then restored (two ld/sd pairs, 0x00a04aa4..0x00a04af8).
 * The calcUPGet call before the curve read has no use of its result
 * (jal calcUPGet at 0x00a04a84, $2 overwritten at 0x00a04a90).
 */
void unitCmdPosAdj(ObjectTask *unit)
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
    motionActor = ((UnitRecord *)unit)->motionActor;
    target = &actor->cmdTarget;
    frame = motionActor->motionFrame;
    index = 0;
    if (!(actor->flags & ACTOR_FLAG_ENEMY) &&
        !(calcUPGet(unit)->flags & 0x40) &&
        (calcUPGet(unit)->charaId < 0xBB || calcUPGet(unit)->charaId >= 0xC3)) {
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
    unitCmdDivSet((UnitRecord *)unit, length, offset);
    motionActor->position.x = target->vector.x;
    motionActor->position.z = target->vector.z;
    *target = savedTarget;
}

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdRun);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdJump);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdHover);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdFloat);

int unitCmdWarp(ObjectTask *unit, int cmdId, int flag)
{
    Actor *actor = unit->work;
    Actor *motionActor = ((UnitRecord *)unit)->motionActor;
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
 * curCmdSet's queued command (src/ov01/battle_init.c, this TU's own
 * ObjectCommandEntry-shaped record via unitCmdPtrGet) stores either a raw
 * coordinate pair at +4/+0xC or a position-table index at +4, selected by
 * mode; ov01/tu001 obj.c owns the record and types both as int, so the float
 * reads below reinterpret the same bytes it does not model as float.
 */
#define CMD_ENTRY_X_OR_IDX_OFF 0x4
#define CMD_ENTRY_Z_OFF 0xC

void unitCmdDstSet(ObjectTask *unit, int cmdId, int mode)
{
    Actor *actor;
    Vector4 *target;
    void *cmdEntry;
    int idx;

    cmdEntry = unitCmdPtrGet(unit);
    actor = unit->work;
    target = &actor->cmdTarget.vector;

    if (mode == 0) {
        ObjectTask *targetUnit = actor->targetUnit;

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

float unitCmdDirSet(UnitRecord *unit)
{
    float zero = 0.0f;
    Actor *self = unit->task.work;
    Vector4 *target = &self->cmdTarget.vector;
    Actor *motionActor = unit->motionActor;
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
 * target (cmdTarget x/z, lwc1 0x0/0x8 off the hoisted +0x90 base at
 * 0x00a05b2c).
 */
float unitCmdLenGet(ObjectTask *unit)
{
    Vector4 *target;
    Actor *motionActor;
    float dx;
    float dz;

    target = &((Actor *)unit->work)->cmdTarget.vector;
    motionActor = ((UnitRecord *)unit)->motionActor;
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

void unitCmdDivSet(UnitRecord *unit, float distance, float step)
{
    Actor *self = unit->task.work;
    Vector4 *target = &self->cmdTarget.vector;
    float ratio = step / distance;
    Actor *motionActor = unit->motionActor;

    target->x = motionActor->position.x + (target->x - motionActor->position.x) * ratio;
    target->z = motionActor->position.z + (target->z - motionActor->position.z) * ratio;
}

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdVASet);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdMoveCalc);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitTelIn);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitTelOut);

/*
 * Battle start: step 0 counts the queued command's argument down as a delay,
 * step 1 plays the battle-ready motions (unitMotSet 0x23, weapon motion 0x2F)
 * for the same player units unitCmdBted poses, or the stand motion for any
 * other unit, step 2 waits for unitTelIn, and the last step waits for the
 * ready motion to end before putting the weapon away.
 */
int unitCmdBtst(ObjectTask *unit)
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
        if (!(((Actor *)unit->work)->flags & ACTOR_FLAG_ENEMY) &&
            !(calcUPGet(unit)->flags & 0x40) &&
            (calcUPGet(unit)->charaId < 0xBB || calcUPGet(unit)->charaId >= 0xC3)) {
            unitMotSet(unit, 0x23);
            unitWpnMotSet(unit, 0, 0, 0x2F);
        } else {
            unitMotStandSet(unit);
        }
        unitMtdProc((UnitRecord *)unit);
        actor->movePhaseTimer = 0;
        actor->moveState++;
    } else if (actor->moveState == 2) {
        unitMtdProc((UnitRecord *)unit);
        if (unitTelIn(unit) != 0) {
            actor->moveState++;
        }
    } else if (!(((Actor *)unit->work)->flags & ACTOR_FLAG_ENEMY) &&
               !(calcUPGet(unit)->flags & 0x40) &&
               (calcUPGet(unit)->charaId < 0xBB || calcUPGet(unit)->charaId >= 0xC3)) {
        unitMtdProc((UnitRecord *)unit);
        if (((UnitRecord *)unit)->motionActor->moveFlags & 0x1000) {
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
 * motion 0x30, face motion 0x39) for a player unit that is neither flagged
 * 0x40 in CalcUnitParam.flags nor one of the charaId 0xBB..0xC2 units
 * unitLoad also skips, then turns the status effects off; the next steps
 * wait for the motion to end and put the weapon away.
 */
int unitCmdBted(ObjectTask *unit)
{
    Actor *actor;

    actor = unit->work;
    unitCmdPtrGet(unit);
    if (actor->moveState == 0) {
        if (!(((Actor *)unit->work)->flags & ACTOR_FLAG_ENEMY) &&
            !(calcUPGet(unit)->flags & 0x40) &&
            (calcUPGet(unit)->charaId < 0xBB || calcUPGet(unit)->charaId >= 0xC3)) {
            unitMotSet(unit, 0x24);
            unitWpnMotSet(unit, 0, 0, 0x30);
            unitFaceMotSet(unit, 0x39);
        }
        unitMtdProc((UnitRecord *)unit);
        statEffOff(unit);
        actor->moveState++;
    } else {
        unitMtdProc((UnitRecord *)unit);
        if (((UnitRecord *)unit)->motionActor->moveFlags & 0x1000) {
            if (calcUPGet(unit)->flags & 0x40) {
                unitWpnMotSet(unit, 0, 0, 5);
            } else if (!(((Actor *)unit->work)->flags & ACTOR_FLAG_ENEMY) &&
                       !(calcUPGet(unit)->flags & 0x40) &&
                       (calcUPGet(unit)->charaId < 0xBB ||
                        calcUPGet(unit)->charaId >= 0xC3)) {
                unitWpnMotSet(unit, 0, 0, 0xFF);
            }
            return unitCmdNext(unit);
        }
    }
    return 0;
}

void unitMtdProc(UnitRecord *unit)
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
 * unitCmdRec/unitCmdStat both read a flag word of the queued command entry
 * (unitCmdPtrGet; unitCmdDstSet's own cmdEntry above reads its own command's
 * first argument at this same offset): nonzero skips playing the stand
 * motion below.
 */
typedef struct {
    unsigned char unmodeled_00[0x4];
    int flag; /* +0x4 */
} UnitCmdRecEntry;

void unitCmdRec(ObjectTask *unit)
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

int unitCmdWake(ObjectTask *unit)
{
    Actor *actor;

    actor = unit->work;
    unitCmdPtrGet(unit);
    if (actor->moveState == 0) {
        unitMotSet(unit, 0x25);
        sndSeTransPlay(unit, 2, 5);
        actor->moveState++;
    } else if (((UnitRecord *)unit)->motionActor->moveFlags & 0x1000) {
        actor->flags &= ~ACTOR_FLAG_DOWN;
        unitMotStandSet(unit);
        return unitCmdNext(unit);
    }
    return 0;
}

void unitCmdStat(ObjectTask *unit)
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

int unitCmdReadWait(ObjectTask *unit)
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
 * Step 0 calls the AGWS effect (0x67 with CalcUnitParam flag 0x40, else 0x66,
 * movz at 0x00a06c14), step 1 stands the unit and clears the teleport timer,
 * then it waits for unitTelIn.
 */
int unitCmdAgws2(ObjectTask *unit)
{
    Actor *actor;

    actor = unit->work;
    unitCmdPtrGet(unit);
    if (actor->moveState == 0) {
        decoEffCall(unit, (calcUPGet(unit)->flags & 0x40) ? 0x67 : 0x66);
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

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdEscape);

/*
 * The reverse of unitCmdFadeOut: teleports the unit in, then unblocks the
 * unit's effect record and restarts its status effects.
 */
int unitCmdFadeIn(ObjectTask *unit)
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
int unitCmdFadeOut(ObjectTask *unit)
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

/*
 * unitMotSetEx is defined later in this TU, still asm; forward-declared for
 * the tail call unitMotSet makes to it, always with a zero third argument.
 */
extern void unitMotSetEx(ObjectTask *unit, int motion, int);

void unitMotSet(ObjectTask *unit, int motion)
{
    unitMotSetEx(unit, motion, 0);
}

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitMotSetEx);

/*
 * unitSetInterp enables interpolation on the actor object owned by a unit.
 * The surrounding Unit layout is not established here; the two scalar
 * offsets below are the members directly demonstrated by this function and
 * its callers.
 */
void unitSetInterp(void *unit)
{
    void *object = *(void **)((unsigned char *)unit + 0x10);
    *(int *)((unsigned char *)object + 0x100) = 1;
}

void unitSetInterpTime(ObjectTask *unit, float time)
{
    ((UnitRecord *)unit)->motionActor->interpTime = time;
}

void unitWpnMotSet(ObjectTask *unit, int weaponIndex, int animGroup, int motion)
{
    Actor *actor;
    unsigned int dataId;

    if (weaponIndex >= 0) {
        actor = ((UnitRecord *)unit)->wpnActor[weaponIndex & 3];
        if (actor != 0) {
            dataId = (animGroup << 8) + motion - 1;
            if (ACT_animGetData(actor, dataId) != 0 || motion == 0xFF) {
                ACT_setMotion(actor, dataId);
            }
        }
    }
}

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitFaceMotSet);

int unitMotGet(ObjectTask *unit)
{
    return ((UnitRecord *)unit)->motionActor->motion + 1;
}

/*
 * Bits of Actor.flags (unit->work, lw 0x10 then lw 0x0) that unitMotStandSet
 * tests, named by what this function does with them: 0x800 (andi at
 * 0x00a072b8) selects motion 0x1D outright; 0x8 (andi at 0x00a072dc) is
 * tested only after calcIdol2Chk returned nonzero, and when set gates motion
 * 2 on unitMotGet returning 0x1C or 0x23. This TU is not the only reader of
 * 0x800: calcHitRate (src/ov01/calc.c on the calc branch) tests the same bit
 * of the target's actor flags as CALC_ACTOR_FLAG_BIT11 and calcTurnStart
 * tests and clears it; what the bit means for them is not evidenced here.
 */
#define ACTOR_FLAG_STAND_MOTION_1D 0x800u
#define ACTOR_FLAG_STAND_MOTION_2_GATED 0x8u

/*
 * One condition for motion 2, as the original branches: separate if/else
 * arms for the last test turn its bnel (0x00a07308) into a bne.
 */
void unitMotStandSet(ObjectTask *unit)
{
    int motion;

    if (((Actor *)unit->work)->flags & ACTOR_FLAG_STAND_MOTION_1D) {
        motion = 0x1D;
    } else if (calcIdol2Chk(unit) != 0
               && ((((Actor *)unit->work)->flags & ACTOR_FLAG_STAND_MOTION_2_GATED) == 0
                   || unitMotGet(unit) == 0x1C || unitMotGet(unit) == 0x23)) {
        motion = 2;
    } else {
        motion = 1;
    }
    unitMotSet(unit, motion);
}

static const char UnitNoGetFailureFormat[32] = "** unitNoGet: err -> %X\n";

int unitNoGet(ObjectTask *unit)
{
    ObjectTask **units = unitTbl.tasks;
    int i;

    for (i = 0; i < 8; i++) {
        if (units[i] == unit) {
            return i;
        }
    }
    printf(D_00A43BD8, unit);
    return -1;
}

extern int printf(const char *format, ...);

/* This legacy accessor returns a unit pointer as its 32-bit address. */
static const char UnitPtrGetFailureFormat[32] = "** unitPtrGet: err -> %d\n";

int unitPtrGet(int index)
{
    if (index < 8) {
        return (int)unitTbl.tasks[index];
    }
    printf(D_00A43BF8, index);
    return 0;
}

/*
 * unitDispOn's own sibling, still asm in this TU; this TU's own UnitRecord
 * view of unitTblGet's table (already published identically as BattleUnit
 * by src/ov01/battle_init.c and src/ov01/debug_entry.c, both ov01/tu003).
 */
extern int unitTblGet(int side, UnitRecord ***table);

void unitDispOn(int side)
{
    UnitRecord **table;
    int count;
    UnitRecord **cur;
    int remaining;
    UnitRecord *unit;

    count = unitTblGet(side, &table);
    if (count > 0) {
        cur = table;
        remaining = count;
        do {
            unit = *cur;
            cur++;
            if (unit != 0) {
                /* Clears the motion actor's hidden-draw flag (bit 0x8 of
                 * Actor.flags); no other claimed function here touches it. */
                unit->motionActor->flags &= ~8;
            }
            remaining--;
        } while (remaining != 0);
    }
}

void unitDispOff(int side)
{
    UnitRecord **table;
    int count;
    UnitRecord **cur;
    int remaining;
    UnitRecord *unit;

    count = unitTblGet(side, &table);
    if (count > 0) {
        cur = table;
        remaining = count;
        do {
            unit = *cur;
            cur++;
            if (unit != 0) {
                unit->motionActor->flags |= 8;
            }
            remaining--;
        } while (remaining != 0);
    }
}

/*
 * Combined collision distance of a unit approaching targetUnit, in the
 * position table's units (/ 100). The first term is converted and kept in a
 * callee-saved FP register across the second calcUPGet call, as the original
 * does.
 */
float unitColiGet(ObjectTask *unit, ObjectTask *targetUnit)
{
    float distance;

    distance = calcUPGet(unit)->coliMove;
    distance += calcUPGet(targetUnit)->coliTarget;
    return distance / 100.0f;
}

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitAgwsChk);

/*
 * Counts the units on one side whose CalcUnitParam.charaId equals charaId.
 * unitTblGet is defined at the top of this TU with `UnitRecord ***table`, so
 * the table is a UnitRecord ** here and each entry is cast to ObjectTask *
 * for calcUPGet.
 */
int unitIdChk(int side, int charaId)
{
    UnitRecord **table;
    int count;
    int matchCount;
    int i;

    matchCount = 0;
    count = unitTblGet(side, &table);
    for (i = 0; i < count; i++) {
        if (table[i] != 0) {
            if (calcUPGet((ObjectTask *)table[i])->charaId == charaId) {
                matchCount++;
            }
        }
    }
    return matchCount != 0;
}

/* Defined later in this TU (a local sibling still in asm). */
extern void unitDispOnOff(int flag);

void unitDispOnAll(void)
{
    unitDispOnOff(0);
}

void unitDispOffAll(void)
{
    unitDispOnOff(1);
}

extern void statEffOn(ObjectTask *unit);

/*
 * calcUPGet(unit)+0x158, immediately before ov01/tu004 calc.c's own
 * published statEffUnits[8] table at +0x15C, holds a pointer to the same
 * kind of per-unit status-effect record statEffOn/statEffOff walk through
 * that table (src/ov01/battle_init.c, ov01/tu003's own published UnitWork,
 * include/ov01/battle_init.h). This TU does not own that record and cannot
 * complete a second UnitWork of its own, so only the touched flag word is
 * reached through a raw offset here.
 */
#define CALC_UNIT_PARAM_STAT_EFF_TARGET_OFF 0x158
#define UNIT_WORK_FLAGS_OFF 0xA8C

void unitDispOnOff(int flag)
{
    UnitRecord **table;
    int index;
    int count;
    UnitRecord *unit;
    void *statEffTarget;

    index = 0;
    count = unitTblGet(2, &table);
    if (count > 0) {
        do {
            unit = table[index];
            if (unit != 0) {
                if (flag == 0) {
                    unit->motionActor->flags &= ~8;
                    statEffTarget = *(void **)((char *)calcUPGet((ObjectTask *) unit) + CALC_UNIT_PARAM_STAT_EFF_TARGET_OFF);
                    if (statEffTarget != 0) {
                        *(unsigned int *)((char *)statEffTarget + UNIT_WORK_FLAGS_OFF) &= ~UNIT_WORK_STATUS_BLOCKED;
                    }
                    statEffOn((ObjectTask *) table[index]);
                } else {
                    unit->motionActor->flags |= 8;
                    statEffTarget = *(void **)((char *)calcUPGet((ObjectTask *) unit) + CALC_UNIT_PARAM_STAT_EFF_TARGET_OFF);
                    if (statEffTarget != 0) {
                        *(unsigned int *)((char *)statEffTarget + UNIT_WORK_FLAGS_OFF) |= UNIT_WORK_STATUS_BLOCKED;
                    }
                    statEffOff((ObjectTask *) table[index]);
                }
            }
            index++;
        } while (index < count);
    }
}
