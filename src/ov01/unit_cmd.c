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

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitPlCreate);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitEnCreate);

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

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitPlInit);

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

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdClear);

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

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdStand);

int unitCmdEnd(ObjectTask *unit)
{
    return unitCmdStand(unit);
}

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdDmg);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdEva);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdDef);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdDead);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitDeadSpecChk);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdMot);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdTgtSet);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdTurn);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdTurnSub);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdTurnSubInit);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdMoveCha);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdMoveNext);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdMoveChaNorm);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdMoveChaRun);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdMoveChaJump);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdMoveChaHover);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdMoveChaWarp);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdMoveChaJump2);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdMoveChaHover2);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdMoveChaFloat);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdMoveChaHover3);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdMovePos);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdMovePosNorm);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdMovePosRun);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdMovePosJump);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdMovePosHover);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdMovePosWarp);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdMovePosHover2);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdMovePosFloat);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdMovePosHover3);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdDirAdj);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdPosAdj);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdRun);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdJump);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdHover);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdFloat);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdWarp);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdDstSet);

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

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdRec);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdWake);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdStat);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdReadWait);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdAgws2);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdEscape);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdFadeIn);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdFadeOut);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitMotSet);

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

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitPtrGet);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitDispOn);

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

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitDispOnOff);
