/*
 * OV01 original TU 2: 0x00a00d60..0x00a07770 (119 functions)
 */
#include "common.h"
#include "shared.h"
#include "unit_cmd.h"

extern unsigned char unitTbl[0x20];
extern int unitPlNum;
extern int unitEnNum;

void unitInit(void)
{
    memset(unitTbl, 0, 0x20);
    unitPlNum = 0;
    unitEnNum = 0;
}

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitTblGet);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitTblSet);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitTblRemove);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitTblChg);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitNumGet);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitLiveNumGet);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitLiveNumGet2);

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

extern int unitAgwsPilotGet(s16 charaId);
extern const char D_00A438F0[];
extern int printf(const char *format, ...);
void unitPlFunc(ObjectTask *unit);

/*
 * unitCreate/unitTblSet/unitLoad are unitPlCreate's own siblings; unitCreate
 * and unitTblSet are still asm in this TU (also forward-declared below for
 * unitEnCreate's own use) and unitLoad is defined later in this file.
 */
extern ObjectTask *unitCreate(s16 *pl, int side, ObjectTaskCallback func);
extern void unitTblSet(int side, ObjectTask *unit);
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
 * unitCreate/unitTblSet are unitEnCreate's own siblings, still asm in this
 * TU; forward-declared for the calls it makes before their definitions
 * appear later in the file.
 */
extern ObjectTask *unitCreate(s16 *pl, int side, ObjectTaskCallback func);
extern void unitTblSet(int side, ObjectTask *unit);
void unitLoad(ObjectTask *unit, int mode);
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

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitAgwsPilotGet);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCreate);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitRemoveActor);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitRemove);

void unitLoad(ObjectTask *unit, int mode)
{
    CalcUnitParam *up;

    up = calcUPGet(unit);
    if (calcUPGet(unit)->charaId < 0xBB || calcUPGet(unit)->charaId >= 0xC3) {
        if (!(((Actor *)unit->work)->flags & ACTOR_FLAG_ENEMY)) {
            sefSetupPlayer(up->charaId, up->sefSetupParams[0],
                           up->sefSetupParams[1], up->sefSetupParams[2]);
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

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitEnInit);

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

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdSet);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdExec);

int unitCmdFree(ObjectTask *unit)
{
    Actor *actor;

    actor = unit->work;
    actor->flags &= ~ACTOR_FLAG_CMD_PENDING;
    return 0;
}

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdAtk);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdAtkSub);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitJobTypeAtk);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdAtkEnd);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdDeadSet);

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

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdGuard);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdAgws);

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

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdDmg);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdEva);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdDef);

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

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdMot);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdTgtSet);

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

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdMoveChaNorm);

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

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdMoveChaJump2);

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

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdMovePosNorm);

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
    Vector4 *target = &actor->cmdTarget;
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

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdPosAdj);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdRun);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdJump);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdHover);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdFloat);

int unitCmdWarp(ObjectTask *unit, int cmdId, int flag)
{
    Actor *actor = unit->work;
    Actor *motionActor = ((UnitRecord *)unit)->motionActor;
    Vector4 *target = &actor->cmdTarget;

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
    target = &actor->cmdTarget;

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
    Vector4 *target = &self->cmdTarget;
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

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdLenGet);

void unitCmdDivSet(UnitRecord *unit, float distance, float step)
{
    Actor *self = unit->task.work;
    Vector4 *target = &self->cmdTarget;
    float ratio = step / distance;
    Actor *motionActor = unit->motionActor;

    target->x = motionActor->position.x + (target->x - motionActor->position.x) * ratio;
    target->z = motionActor->position.z + (target->z - motionActor->position.z) * ratio;
}

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdVASet);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdMoveCalc);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitTelIn);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitTelOut);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdBtst);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdBted);

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

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdWake);

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

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdReadWait);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdAgws2);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdEscape);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdFadeIn);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdFadeOut);

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

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitMotStandSet);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitNoGet);

extern const char D_00A43BF8[];
extern int printf(const char *format, ...);

/*
 * unitTbl (declared above) is read here as a table of eight 4-byte slots;
 * unitPtrGet is the only claimed reader of it besides unitInit's own clear.
 */
int unitPtrGet(int index)
{
    if (index < 8) {
        return ((int *)unitTbl)[index];
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

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitDispOff);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitColiGet);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitAgwsChk);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitIdChk);

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
extern void statEffOff(ObjectTask *unit);

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
#define UNIT_WORK_STATUS_BLOCKED 0x100u

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
