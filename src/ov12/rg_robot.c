/*
 * OV12 original TU 4: 0x00a02510..0x00a09480 (153 functions)
 */
#include "common.h"
#include "shared.h"
#include "rg_robot.h"
#include "ov12/assert_prog.h"
#include "ov12/rg_actor_motid.h"
#include "ov12/rg_robot_effect.h"
#include "ov12/rg_equip_type.h"
#include "ov12/rg_geom_point.h"
#include "ov12/rg_weapon.h"
#include "ov12/rg_shot_thread.h"
#include "ov12/xrg_actor.h"
#include "ov12/xrg_rand_int.h"
#include "ov12/xrg_sound.h"
#include "ov12/rg_robot_subcon.h"
#include "ov12/create_rg.h"
#include "ov12/rg_drop_weapon.h"
#include "ov12/rg_dull_flag.h"
#include "ov12/rg_error.h"
#include "ov12/rg_geom.h"
#include "ov12/rg_geom_ball.h"
#include "ov12/rg_geom_robot.h"
#include "ov12/rg_heap.h"
#include "ov12/rg_hissatu.h"
#include "ov12/rg_motion_info_db.h"
#include "ov12/rg_robot_spec.h"
#include "ov12/rg_shot.h"
#include "ov12/xrg_v_log.h"
#include "ov12/rg_robot.h"

/* _AllowAllCmdInMoving supplies mode 1; _InitTargettingStatus tests this
 * mode as zero/nonzero and reads the actual target from body separately.
 * Its result is the 0x100 command-processing mask bit, not a command id. */
#define RG_CMD_RESULT_TARGETTING 0x100

/* RgShotThread is defined in tu011 and stays opaque at this call site. */
struct RgShotThread;

struct RgShot;
extern const char robotUnknownShieldStatusFormat[];
extern const char robotCannotTransitVrDashModeFormat[];
extern const char robotUnknownDashStatusFormat[];
extern const char robotUnknownDashModeFormat[];
extern const char robotUnknownVrDashCmdModeFormat[];
extern const char robotUnknownVrDashModeFormat[];
/* One priority level per command type (s_anCmdLevel, 0x3c bytes). */
#define RG_CMD_TYPE_NUM 15
static int s_anCmdLevel[15];
extern const char robotEmptyEquipDatabaseName[];
extern const char robotAssertEquipPosValid[];
extern const char robotAssertJointIdValid[];
extern const char robotCheckPakaPakaLog[];
extern const char robotCheckPakaPakaEndLog[];
extern const char robotNoLockOnLog[];
static RgRobotSpec s_inDefSpec_0;

static int _ExecAccelarateCmd(RgRobotStatus *status, RgBody *body, RgCmd *command);
static int _ExecRotateCmd(RgRobotStatus *status, RgBody *body, RgCmd *command);
static int _ExecShotOrAttackCmd(RgRobotStatus *status, RgBody *body, RgCmd *command);
static int _ExecTargettingCmd(RgRobotStatus *status, RgBody *body, RgCmd *command, int targettingMode);
static int _ExecBreakCmd(RgRobotStatus *status, RgBody *body, void *command);
static int _ExecDashCmd(RgRobotStatus *status, RgBody *body, RgCmd *command);
static int _ExecDashContinueCmd(RgRobotStatus *status, RgBody *body, RgCmd *command);
static int _ExecDropWeaponCmd(RgRobotStatus *status, RgBody *body, RgCmd *command);
static int _ExecDamageCmd(RgRobotStatus *status, RgBody *body, RgCmd *command);
static int _ExecWeakDamageCmd(RgRobotStatus *status, RgBody *body, RgCmd *command);
static int _ExecHitByBody(RgRobotStatus *status, RgBody *body, RgCmd *command);
static void _BodyPlayMotionLoop(RgBody *body, int motion);
static void _BodyPlayMotion(RgBody *body, int motion);
static int _AllowAllCmdInMoving(RgRobotStatus *status, RgBody *body, RgCmd *command);
static int _MovingPassTime(RgRobotStatus *status, RgBody *body, float deltaTime);
static RgRobotStatus *_InitAttackStatus(RgRobotStatus *status, RgBody *body, int motion, RgWeapon *weapon, int eSide);
static int _IsBackWeaponReady(RgBody *body, unsigned int eType);
static void _BodyAdvanceForAttack(RgBody *body, float attackPower, float attackFactor);
static void _StartHomingThread(RgHomingThread *thread, RgGeomPoint *geometry, RgGeomPoint *target, float actionTime);
static void _PassTimeHomingThread(RgHomingThread *thread, float deltaTime);
static float _CalcMaxSpeed(RgBody *body, float baseSpeed);
static int _StatusPassTime(RgRobotStatus *status, RgBody *body, float dt);
static void _BodyGeomPassTime(RgBody *body, float deltaTime);
static int _EyeGeomPassTime(RgBody *body, float deltaTime);
static void _AdvanceGeomPassTime(RgBody *body, float deltaTime);
static void _PassTimeRobot(RgStatus *pRobot, float deltaTime);
static int _ExecCmdInAttack(RgRobotStatus *status, RgBody *body, RgCmd *command);
static int _AllowDamageCmd(RgRobotStatus *status, RgBody *body, RgCmd *command);
static int _HissatuPassTime(RgRobotStatus *status, RgBody *body, float deltaTime);
static int _ShieldPassTime(RgRobotStatus *status, RgBody *body, float deltaTime);
static int _ExecCmdInShield(RgRobotStatus *status, RgBody *body, RgCmd *command);
static int _TargettingPassTime(RgRobotStatus *status, RgBody *body, float deltaTime);
static int _InitDashVRStatus(RgRobotStatus *status, RgBody *body, RgVector direction);
static void _InitShieldStatus(RgRobotStatus *status, RgBody *body, int motion, RgWeapon *shieldWeapon, int scratch2Arg);
static int _DashExecCmd(RgRobotStatus *status, RgBody *body, RgCmd *command);
static int _DashPassTime(RgRobotStatus *status, RgBody *body, float deltaTime);
static int _ExecCmdInTargetting(RgRobotStatus *status, RgBody *body, RgCmd *command);
static int _AllowDamageCmdNotVRDashHit(RgRobotStatus *status, RgBody *body, RgCmd *command);
static int _GetDropMotion(int eSide);
static int _InitDropStatus(RgRobotStatus *status, RgBody *body, int motion, int eSide);
static void _InitMovingStatus(RgRobotStatus *status, RgBody *body);
static void _BodySetSound(RgBody *body, XrgSound *sound);
static int _BodyDropWeapon(RgBody *body, unsigned int eType);
static void _BodyAutoHomingPassTime(RgBody *body, float deltaTime);
static void _InitBody(RgBody *body);
static void _DestructBody(RgBody *body);
static int _DashVRExecCmd(RgRobotStatus *status, RgBody *body, RgCmd *command);
static int _DashVRPassTime(RgRobotStatus *status, RgBody *body, float deltaTime);
static void _InitStatusHissatu(RgRobotStatus *status, RgBody *body);
static void _InitTargettingStatus(RgRobotStatus *status, RgBody *body, int jump);
static int _BodyExecCmd(RgRobotStatus *robotStatus, RgBody *body, RgCmd *command);
static int _BodyPassTime(RgRobotStatus *robotStatus, RgBody *body, float deltaTime);
static void _ClearCmdQueue(RgCmdQueue *cmdQueue);
static void _PassTimeRobot(RgStatus *pRobot, float deltaTime);
static int _ExecCmdInAttack(RgRobotStatus *status, RgBody *body, RgCmd *command);
static int _AllowDamageCmd(RgRobotStatus *status, RgBody *body, RgCmd *command);
static int _HissatuPassTime(RgRobotStatus *status, RgBody *body, float deltaTime);
static int _ShieldPassTime(RgRobotStatus *status, RgBody *body, float deltaTime);
static int _ExecCmdInShield(RgRobotStatus *status, RgBody *body, RgCmd *command);
static int _TargettingPassTime(RgRobotStatus *status, RgBody *body, float deltaTime);
static int _InitDashVRStatus(RgRobotStatus *status, RgBody *body, RgVector direction);
static void _InitShieldStatus(RgRobotStatus *status, RgBody *body, int motion, RgWeapon *shieldWeapon, int scratch2Arg);
static int _DashExecCmd(RgRobotStatus *status, RgBody *body, RgCmd *command);
static int _DashPassTime(RgRobotStatus *status, RgBody *body, float deltaTime);
static int _ExecCmdInTargetting(RgRobotStatus *status, RgBody *body, RgCmd *command);
static int _AllowDamageCmdNotVRDashHit(RgRobotStatus *status, RgBody *body, RgCmd *command);
static int _GetDropMotion(int eSide);
static int _InitDropStatus(RgRobotStatus *status, RgBody *body, int motion, int eSide);
static void _InitMovingStatus(RgRobotStatus *status, RgBody *body);
static void _BodySetSound(RgBody *body, XrgSound *sound);
static int _BodyDropWeapon(RgBody *body, unsigned int eType);
static void _BodyAutoHomingPassTime(RgBody *body, float deltaTime);
static void _InitBody(RgBody *body);
static void _DestructBody(RgBody *body);
static int _DashVRExecCmd(RgRobotStatus *status, RgBody *body, RgCmd *command);
static int _DashVRPassTime(RgRobotStatus *status, RgBody *body, float deltaTime);
static void _InitStatusHissatu(RgRobotStatus *status, RgBody *body);
static void _InitTargettingStatus(RgRobotStatus *status, RgBody *body, int jump);
static int _BodyExecCmd(RgRobotStatus *robotStatus, RgBody *body, RgCmd *command);
static int _BodyPassTime(RgRobotStatus *robotStatus, RgBody *body, float deltaTime);
static void _ClearCmdQueue(RgCmdQueue *cmdQueue);
static void _DullFlagsPassTime(RgStatus *pRobot, float deltaTime);
static void _InitRobot(RgStatus *pRobot);
static void _DestructRobot(RgStatus *pRobot);
static void _DispRobot(RgStatus *pRobot);
static void _BodySetSpec(RgBody *body, RgRobotSpec *spec);
static void _BodySetGeom(RgBody *body, RgGeomPoint *geometry);
static void _BodySetEyeGeom(RgBody *body, RgGeomPoint *eyeGeometry);
static void _BodySetAdvanceGeom(RgBody *body, RgGeomPoint *advanceGeometry);
static void _BodySetActor(RgBody *body, XrgActorEssence *actorSpec);

const char robotAssertThreadNotNil[] = "pThread != NIL";

const char robotSourcePath[] = "../rg_robot.euc.c";

const char robotAssertBaseSpeedPositive[] = "fBaseSpeed > RG_FCONST(0.0)";

const char robotAssertStatusNotNil[] = "pStatus != NIL";

const char robotAssertCmdQueueNotNil[] = "pQ != NIL";

const char robotAssertBodyNotNil[] = "pBody != NIL";

const char robotAssertGeomNotNil[] = "pGeom != NIL";

const char robotAssertEquipTypeInRange[] = "RG_EQUIP_TYPE_MIN <= (eType) && (eType) < RG_EQUIP_TYPE_NUM";

const char robotAssertCmdNotNil[] = "pCmd != NIL";

const char robotAlreadyDisposedBodyFormat[] = "already disposed body %p";

const char robotUnknownDropWeaponEquipTypeFormat[] = "unknown equip type (drop weapon) %d";

const char robotHitEffectName[] = "eff06.ptcl";

const char robotAssertRobotNotNil[] = "pRobot != NIL";

const char robotAlreadyDisposedRobotFormat[] = "already disposed robot %p\n";

const char robotAssertEquipSideInRange[] = "RG_EQUIP_TYPE_MIN <= (eSide) && (eSide) < RG_EQUIP_TYPE_NUM";

static void _InitHomingThread(RgHomingThread *thread)
{
    if (thread == 0) {
        assert_prog(robotAssertThreadNotNil, robotSourcePath, 73);
    }
    thread->geometry = 0;
    thread->target = 0;
    thread->actionTime = 0;
    thread->sleepTime = 0;
    thread->homingSpeed = RG_ROBOT_HOMING_TURN_SPEED;
    thread->state = 0;
}

static RgHomingThread *_CreateHomingThread(void)
{
    RgHomingThread *thread;

    thread = RgHeapAlloc(InstanceOfRgHeap(), sizeof(RgHomingThread),
                          robotSourcePath, 85);
    _InitHomingThread(thread);
    return thread;
}

static void _DisposeHomingThread(RgHomingThread *thread)
{
    if (thread == 0) {
        assert_prog(robotAssertThreadNotNil, robotSourcePath, 93);
    }
    RgHeapFree(InstanceOfRgHeap(), thread, robotSourcePath, 94);
}

static void _StartHomingThread(RgHomingThread *thread, RgGeomPoint *geometry,
                               RgGeomPoint *target, float actionTime)
{
    if (thread == 0) {
        assert_prog(robotAssertThreadNotNil, robotSourcePath, 100);
    }
    thread->actionTime = actionTime;
    thread->target = target;
    thread->geometry = geometry;
    thread->sleepTime = 0;
    if (geometry == 0) {
        thread->state = 0;
        return;
    }
    thread->state = 1;
}

static void _StopHomingThread(RgHomingThread *thread)
{
    if (thread == 0) {
        assert_prog(robotAssertThreadNotNil, robotSourcePath, 114);
    }
    if (thread->state == 1) {
        thread->sleepTime = 0;
        thread->state = 2;
    }
}

static int _GetStatHomingThread(RgHomingThread *thread)
{
    if (thread == 0) {
        assert_prog(robotAssertThreadNotNil, robotSourcePath, 124);
    }
    return thread->state;
}

static float _GetSleepTimeHomingThread(RgHomingThread *thread)
{
    float sleepTime;

    if (thread == 0) {
        assert_prog(robotAssertThreadNotNil, robotSourcePath, 131);
    }
    sleepTime = 99999.9f;
    if (thread->state == 2) {
        sleepTime = thread->sleepTime;
    }
    return sleepTime;
}

static void _PassTimeHomingThread(RgHomingThread *thread, float deltaTime)
{
    float actionTime;

    if (thread == 0) {
        assert_prog(robotAssertThreadNotNil, robotSourcePath, 141);
    }
    switch (thread->state) {
    case 1:
        actionTime = thread->actionTime - deltaTime;
        thread->actionTime = actionTime;
        if (actionTime <= 0.0f) {
            thread->actionTime = 0.0f;
            thread->state = 2;
        }
        RgRobSubHoming(thread->geometry, thread->target, thread->homingSpeed,
                       deltaTime);
        break;
    case 2:
        thread->sleepTime += deltaTime;
        break;
    }
}

static float _CalcMaxSpeed(RgBody *body, float baseSpeed)
{
    float specWeight;
    float geomWeight;

    specWeight = body->spec->weight;
    geomWeight = RgGeomPointGetWeight(body->geometry);
    if (!(baseSpeed > 0.0f)) {
        assert_prog(robotAssertBaseSpeedPositive, robotSourcePath, 247);
    }
    return (baseSpeed * specWeight) / geomWeight;
}

static void _StopAllShotThread(RgBody *body)
{
    unsigned int eSide;

    for (eSide = 0; eSide < RG_EQUIP_TYPE_NUM; eSide++) {
        RgShotThreadSleep(body->shotThread[eSide]);
    }
}

static void _BodyOnGround(RgBody *body)
{
    if (body->grounded == 0) {
        XrgSoundRing(body->landingSound, 0x32);
    }
    body->grounded = 1;
}

static void _InitStatus(RgRobotStatus *status)
{
    if (status == 0) {
        assert_prog(robotAssertStatusNotNil, robotSourcePath, 611);
    }
    status->type = 0;
    status->elapsedTime = 0;
    status->execCmdMethod = 0;
    status->passTimeMethod = 0;
    status->dispMethod = 0;
    status->exitMethod = 0;
}

static RgRobotStatus *_CreateStatue(void)
{
    RgRobotStatus *status;

    status = RgHeapAlloc(InstanceOfRgHeap(), 0x60U, robotSourcePath, 624);
    _InitStatus(status);
    return status;
}

static void _SetStatus(RgRobotStatus *status, int type, RgBody *body)
{
    RgStatusExitFunc exitMethod;

    if (status == 0) {
        assert_prog(robotAssertStatusNotNil, robotSourcePath, 633);
    }
    exitMethod = status->exitMethod;
    if (exitMethod != 0) {
        exitMethod(status, body);
    }
    _InitStatus(status);
    status->type = type;
}

static void _DisposeStatus(RgRobotStatus *status)
{
    if (status == 0) {
        assert_prog(robotAssertStatusNotNil, robotSourcePath, 647);
    }
    RgHeapFree(InstanceOfRgHeap(), status, robotSourcePath, 648);
}

static int _StatusExecCmd(RgRobotStatus *status, RgCmd *command, RgBody *body)
{
    RgStatusExecCmdFunc execCmdMethod;

    if (status == 0) {
        assert_prog(robotAssertStatusNotNil, robotSourcePath, 657);
    }
    execCmdMethod = status->execCmdMethod;
    if (execCmdMethod != 0) {
        return execCmdMethod(status, body, command);
    }
    return 0;
}

static int _StatusPassTime(RgRobotStatus *status, RgBody *body, float dt)
{
    RgStatusPassTimeFunc passTimeMethod;
    int result;

    result = 0;
    if (status == 0) {
        assert_prog(robotAssertStatusNotNil, robotSourcePath, 668);
    }
    passTimeMethod = status->passTimeMethod;
    if (passTimeMethod != 0) {
        result = passTimeMethod(status, body, dt);
    }
    status->elapsedTime += dt;
    return result;
}

static void _StatusDisp(RgRobotStatus *status, RgBody *body)
{
    RgStatusDispFunc dispMethod;

    if (status == 0) {
        assert_prog(robotAssertStatusNotNil, robotSourcePath, 678);
    }
    dispMethod = status->dispMethod;
    if (dispMethod != 0) {
        dispMethod(status, body);
    }
}

static int _AllowAllCmdInMoving(RgRobotStatus *status, RgBody *body, RgCmd *command)
{
    int result;

    result = 0;
    switch ((unsigned int) command->type) {
    case RG_CMD_ACCELARATE:
        result = _ExecAccelarateCmd(status, body, command);
        break;
    case RG_CMD_ACCELARATE_ROTATE:
        result = _ExecRotateCmd(status, body, command);
        break;
    case RG_CMD_SHOT:
        result = _ExecShotOrAttackCmd(status, body, command);
        break;
    case RG_CMD_TARGETTING:
        result = _ExecTargettingCmd(status, body, command, 1);
        break;
    case RG_CMD_BREAK:
        result = _ExecBreakCmd(status, body, command);
        break;
    case RG_CMD_DASH:
        result = _ExecDashCmd(status, body, command);
        break;
    case RG_CMD_DASH_CONTINUE:
        result = _ExecDashContinueCmd(status, body, command);
        break;
    case RG_CMD_DROP_WEAPON:
        result = _ExecDropWeaponCmd(status, body, command);
        break;
    case RG_CMD_DAMAGE:
        result = _ExecDamageCmd(status, body, command);
        break;
    case RG_CMD_WEAK_DAMAGE:
        result = _ExecWeakDamageCmd(status, body, command);
        break;
    case RG_CMD_HIT_BY_BODY:
        result = _ExecHitByBody(status, body, command);
        break;
    }
    return result;
}

/*
 * The original range-checks the command type with sltiu (ov12:0x00a02d78)
 * before splitting the 8/9/14 cases, which GCC only emits for an unsigned
 * switch operand.
 */
static int _AllowDamageCmd(RgRobotStatus *status, RgBody *body, RgCmd *command)
{
    int result;

    result = 0;
    switch ((unsigned int) command->type) {
    case RG_CMD_DAMAGE:
        result = _ExecDamageCmd(status, body, command);
        break;
    case RG_CMD_WEAK_DAMAGE:
        result = _ExecWeakDamageCmd(status, body, command);
        break;
    case RG_CMD_HIT_BY_BODY:
        result = _ExecHitByBody(status, body, command);
        break;
    }
    return result;
}

static int _AllowDamageCmdNotVRDashHit(RgRobotStatus *status, RgBody *body, RgCmd *command)
{
    int result;

    result = 0;
    switch (command->type) {
    case RG_CMD_DAMAGE:
        result = _ExecDamageCmd(status, body, command);
        break;
    case RG_CMD_WEAK_DAMAGE:
        result = _ExecWeakDamageCmd(status, body, command);
        break;
    }
    return result;
}

static int _AllowDamageCmdNotPetrify(RgRobotStatus *status, RgBody *body, RgCmd *command)
{
    int result;
    float life;

    (void)status;
    result = 0;
    if (command->type != RG_CMD_DAMAGE) {
        if (command->type == RG_CMD_WEAK_DAMAGE) {
            goto damage;
        }
    } else {
damage:
        life = body->life - command->scratch0;
        body->life = life;
        if (life < 0.0f) {
            body->life = 0.0f;
        }
        result = 0x80;
    }
    return result;
}

const char robotAssertMotionInfoNotNil[] = "pMotInfo != NIL";

static int _MovingPassTime(RgRobotStatus *status, RgBody *body, float deltaTime)
{
    RgVector velocity;
    RgRobotSpec *spec;
    float speed;
    float rotVel;
    int advanceMot;
    int rollMot;
    int motion;

    spec = body->spec;
    RgGeomPointGetVel(body->geometry, velocity);
    speed = __builtin_sqrtf(velocity[0] * velocity[0] + velocity[2] * velocity[2]);
    rotVel = RgGeomRobotGetRotVel(&body->geometry->geom);
    if (rotVel < 0.0f) {
        rotVel = -rotVel;
    }
    advanceMot = RgRobSubGetAdvanceMot(body->geometry, 0);
    rollMot = RgRobSubGetRollMotion(body->geometry);
    if (5.0f < speed && advanceMot != RG_MOT_NONE) {
        motion = advanceMot;
    } else if (0.39269908f < rotVel && rollMot != RG_MOT_NONE) {
        motion = rollMot;
    } else {
        motion = RG_MOT_STAND;
    }
    if (motion != status->scratch0) {
        status->scratch0 = motion;
        if (motion == RG_MOT_STAND) {
            _BodyPlayMotionLoop(body, RG_MOT_STAND);
        } else {
            _BodyPlayMotion(body, motion);
        }
    }
    if (body->dashTime < spec->dashTime) {
        body->dashTime = body->dashTime + deltaTime;
        if (spec->dashTime < body->dashTime) {
            body->dashTime = spec->dashTime;
        }
    }
    return 0;
}

static void _InitMovingStatus(RgRobotStatus *status, RgBody *body)
{
    _SetStatus(status, RG_STATUS_MOVING, body);
    status->execCmdMethod = _AllowAllCmdInMoving;
    status->passTimeMethod = _MovingPassTime;
    status->scratch0 = RG_MOT_NONE;
}

static int _ExecCmdInAttack(RgRobotStatus *status, RgBody *body, RgCmd *command)
{
    RgRobotStatus *nextStatus;
    RgMotionAction *action;
    RgMotionShotInfo *motionInfo;
    float elapsedTime;
    int result;

    nextStatus = status;
    result = _AllowDamageCmd(status, body, command);
    elapsedTime = status->elapsedTime;
    motionInfo = status->attackMotionInfo;
    if (result == 0 && command->type == RG_CMD_HIT_WEAPON_ATTACK) {
        action = RgMotionInfoGetActionInTime(motionInfo, 1, elapsedTime);
        if (action == 0) return 0;
        if (action->shiftMotionNo == -1) return 0;
        if (status->attackWeapon != command->weapon) return 0;
        nextStatus = _InitAttackStatus(status, body, action->shiftMotionNo,
                                       status->attackWeapon, status->eSide);
        if (nextStatus != 0) {
            nextStatus->scratch4 = 0;
            RgWeaponPlayMotion(command->weapon, action->weaponMotionNo);
        }
        result = 1024;
    }
    if (command->type == RG_CMD_SHOT && nextStatus->eSide != RG_EQUIP_TYPE_BACK &&
        _IsBackWeaponReady(body, command->param) &&
        nextStatus->attackActive != 0) {
        result |= _ExecShotOrAttackCmd(status, body, command);
    }
    return result;
}

static int _AttackPassTime(RgRobotStatus *status, RgBody *body, float deltaTime)
{
    RgWeapon *weapon;
    RgMotionShotInfo *motionInfo;
    float elapsedTime;
    int controlFlag;

    weapon = status->attackWeapon;
    elapsedTime = status->elapsedTime;
    motionInfo = status->attackMotionInfo;
    if (weapon != 0) {
        controlFlag = RgWeaponGetControlFlag(weapon);
    } else {
        controlFlag = 0;
    }
    if (RgMotionInfoGetActionInTime(motionInfo, 0, elapsedTime) != 0) {
        if (weapon != 0) RgWeaponShot(weapon);
        status->motionInfo = 0;
        _StopHomingThread(body->homingThread);
    } else if (XrgActorIsEndOfMotion(body->actor)) {
        if (weapon != 0) RgWeaponShotStop(weapon);
        _InitMovingStatus(status, body);
    } else if (status->motionInfo != 0) {
        if (status->scratch4 != 0 && (controlFlag & RG_WEAPON_CONTROL_ADV) != 0) {
            _BodyAdvanceForAttack(body, deltaTime, 1.0f);
        }
    } else {
        if (weapon != 0) RgWeaponShotStop(weapon);
        _StopHomingThread(body->homingThread);
    }
    return 32;
}

static void _AttackExit(RgRobotStatus *status, RgBody *body)
{
    RgWeapon *shotWeapon;
    RgWeapon *motionWeapon;
    RgWeaponEssence *ess;

    shotWeapon = status->attackWeapon;
    if (shotWeapon != 0) {
        RgWeaponShotStop(shotWeapon);
    }
    _StopHomingThread(body->homingThread);
    motionWeapon = status->attackWeapon;
    if (motionWeapon != 0) {
        ess = RgWeaponGetEss(motionWeapon);
        if (ess != 0) {
            RgWeaponPlayMotion(motionWeapon,
                               ess->shotEndMotionTable[body->charID][status->eSide]);
        }
    }
    if (status->attackLockonWeapon != 0) {
        RgWeaponRestartLockon(status->attackLockonWeapon);
    }
}

static RgRobotStatus *_InitAttackStatus(RgRobotStatus *status, RgBody *body,
                                        int motion, RgWeapon *weapon, int eSide)
{
    RgMotionShotInfo *motionInfo;
    RgMotionAction *action;

    motionInfo = RgMotionInfoDBGet(InstanceOfRgMotionInfoDB(), motion,
                                   body->charID);
    if (motionInfo == 0) assert_prog(robotAssertMotionInfoNotNil, robotSourcePath, 1009);
    _StopAllShotThread(body);
    if (body->actor == 0) return 0;
    _SetStatus(status, RG_STATUS_ATTACK, body);
    status->execCmdMethod = _ExecCmdInAttack;
    status->passTimeMethod = _AttackPassTime;
    status->exitMethod = _AttackExit;
    status->attackMotionInfo = motionInfo;
    status->attackWeapon = weapon;
    status->attackActive = 1;
    status->scratch4 = 1;
    status->eSide = eSide;
    switch (eSide) {
    case 0:
        status->attackLockonWeapon = body->weapon[RG_EQUIP_TYPE_RIGHT];
        break;
    case 1:
        status->attackLockonWeapon = body->weapon[RG_EQUIP_TYPE_LEFT];
        break;
    default:
        status->attackLockonWeapon = 0;
        break;
    }
    if (status->attackLockonWeapon != 0) {
        RgWeaponStopLockon(status->attackLockonWeapon);
    }
    _BodyPlayMotion(body, motion);
    if (weapon != 0) {
        RgWeaponPlayShotMotion(weapon);
        if ((RgWeaponGetControlFlag(weapon) & RG_WEAPON_CONTROL_TARGET_MASK)
                == RG_WEAPON_CONTROL_TARGET_WTH
            && RgWeaponIsTargetting(weapon)) {
            action = RgMotionInfoGetActionInTime(motionInfo, 0, RG_ROBOT_TIME_UNLIMITED);
            if (action != 0) {
                _StartHomingThread(body->homingThread, body->geometry,
                                   body->target, action->startTime);
            }
        }
    }
    return status;
}

static int _ExecCmdInHissatu(RgRobotStatus *status, RgBody *body, RgCmd *command)
{
    return _AllowDamageCmd(status, body, command);
}

static int _HissatuPassTime(RgRobotStatus *status, RgBody *body,
                            float deltaTime)
{
    RgRobotSpec *spec;
    RgMotionShotInfo *motionInfo;
    RgMotionShotInfo *nextMotionInfo;
    RgWeapon *weapon0;
    RgWeapon *weapon1;
    float moveMaxSpeed;
    float dashMaxSpeed;
    float elapsedTime;
    struct RgShot *shots[16];
    unsigned int shotCount;
    unsigned int i;
    int hit;
    int result;
    struct RgMotionAction *action;
    void *shotThread;

    spec = body->spec;
    motionInfo = status->hissatuMotionInfo;
    result = 0;
    elapsedTime = status->elapsedTime;
    moveMaxSpeed = _CalcMaxSpeed(body, spec->moveSpeedMax);
    dashMaxSpeed = _CalcMaxSpeed(body, spec->dashSpeedMax);
    weapon0 = body->weapon[RG_EQUIP_TYPE_LEFT];
    weapon1 = body->weapon[RG_EQUIP_TYPE_RIGHT];
    if (body->actor == 0 || XrgActorIsEndOfMotion(body->actor)) {
        _InitMovingStatus(status, body);
        return 0;
    }
    if (motionInfo == 0) {
        return 0x200;
    }
    action = RgMotionInfoGetActionInTime(motionInfo, 0, elapsedTime);
    if (action != status->currentAction && action != 0) {
        status->currentAction = action;
        status->scratch0 += 1;
        status->boosting = 0;
    }

    switch (body->charID) {
    case 0:
        if (status->boosting != 0) {
            _BodyAdvanceForAttack(body, deltaTime, 1.0f);
            RgGeomPointSetMaxXYSpd(body->geometry, dashMaxSpeed * 1.5f);
            if (status->effectStarted == 0) {
                RgRobotEffectStartDash(body->effect, 0.5f);
                status->effectStarted = 1;
            }
        } else {
            RgGeomPointSetMaxXYSpd(body->geometry, moveMaxSpeed);
        }
        if (RgWeaponIsTargetting(weapon0) != 0
            || RgWeaponIsTargetting(weapon1) != 0) {
            RgRobSubHoming(body->geometry, body->target, RG_ROBOT_HOMING_TURN_SPEED, deltaTime);
            body->flags |= RG_BODY_FLAG_FREE_TURN;
        }
        if (action != 0) {
            switch (status->scratch0) {
            case 1:
                result = 1024;
                break;
            case 2:
                RgWeaponShot(weapon1);
                break;
            }
        } else {
            RgWeaponShotStop(weapon1);
        }
        break;

    case 2:
        RgWeaponStopLockon(weapon1);
        RgWeaponStopLockon(weapon0);
        if (RgWeaponIsTargetting(weapon0) != 0
            || RgWeaponIsTargetting(weapon1) != 0) {
            RgRobSubHoming(body->geometry, body->target, RG_ROBOT_HOMING_TURN_SPEED, deltaTime);
            body->flags |= RG_BODY_FLAG_FREE_TURN;
        }
        if (action != 0) {
            if ((status->shotCounter & 3) == 0) {
                RgWeaponShot(weapon1);
            } else if ((status->shotCounter & 3) == 2) {
                RgWeaponShot(weapon0);
            }
            RgWeaponSetFree(weapon1);
            RgWeaponSetFree(weapon0);
            status->shotCounter++;
        } else {
            RgWeaponShotStop(weapon0);
            RgWeaponShotStop(weapon1);
        }
        break;

    case 4:
        if (status->shotPending != 0) {
            shotThread = body->shotThread[RG_EQUIP_TYPE_RIGHT];
            if (weapon1 != 0 && shotThread != 0) {
                RgShotThreadSleep(shotThread);
                RgWeaponShotStop(weapon1);
            }
        }
        if (status->boosting != 0) {
            _BodyAdvanceForAttack(body, deltaTime, 1.0f);
            if (RgWeaponIsTargetting(weapon0) != 0
                || RgWeaponIsTargetting(weapon1) != 0) {
                RgRobSubHoming(body->geometry, body->target, RG_ROBOT_HOMING_TURN_SPEED,
                               deltaTime);
                body->flags |= RG_BODY_FLAG_FREE_TURN;
            }
        } else {
            shotCount = RgWeaponGetAttachedShot(weapon0, shots);
            hit = 0;
            for (i = 0; i < shotCount; i++) {
                if (RgShotGetHitRobNum(shots[i]) != 0) {
                    hit = 1;
                }
            }
            if (status->scratch1 == RG_MOT_HISSATU && hit != 0) {
                nextMotionInfo = RgMotionInfoDBGet(InstanceOfRgMotionInfoDB(), RG_MOT_HISSATU_VX06_HIT,
                                                   body->charID);
                if (nextMotionInfo != 0) {
                    status->hissatuMotionInfo = nextMotionInfo;
                    status->scratch1 = RG_MOT_HISSATU_VX06_HIT;
                    status->elapsedTime = 0.0f;
                    _BodyPlayMotion(body, RG_MOT_HISSATU_VX06_HIT);
                }
            }
        }
        if (action != 0) {
            switch ((unsigned int) status->scratch0) {
            case 1:
                RgWeaponShot(weapon0);
                break;
            case 2:
            case 3:
            case 4:
                RgWeaponShot(weapon1);
                RgWeaponSetFree(weapon1);
                break;
            case 5:
                result = 1024;
                break;
            }
        } else {
            RgWeaponShotStop(weapon0);
            RgWeaponShotStop(weapon1);
            RgWeaponSetFree(weapon1);
        }
        break;

    case 5:
        if (status->shotPending != 0) {
            if (weapon1 != 0) {
                RgWeaponPlayMotion(weapon1, 2132);
            }
            if (weapon0 != 0) {
                RgWeaponStopLockon(weapon0);
            }
        }
        if (status->boosting != 0) {
            _BodyAdvanceForAttack(body, deltaTime, 1.0f);
            if (RgWeaponIsTargetting(weapon0) != 0
                || RgWeaponIsTargetting(weapon1) != 0) {
                RgRobSubHoming(body->geometry, body->target, RG_ROBOT_HOMING_TURN_SPEED, deltaTime);
                body->flags |= RG_BODY_FLAG_FREE_TURN;
            }
        }
        if (action != 0) {
            switch ((unsigned int) status->scratch0) {
            case 1:
            case 2:
                RgWeaponShot(weapon1);
                break;
            case 3:
                RgWeaponShot(weapon0);
                break;
            }
        } else {
            RgWeaponShotStop(weapon0);
            RgWeaponShotStop(weapon1);
            if (status->scratch0 == 2) {
                RgWeaponRestartLockon(weapon0);
            }
        }
        break;

    case 3:
        _BodyAdvanceForAttack(body, deltaTime, 1.0f);
        RgGeomPointSetMaxXYSpd(body->geometry, dashMaxSpeed);
        if (status->effectStarted == 0) {
            RgRobotEffectStartDash(body->effect, 0.8f);
            RgRobotEffectStartJet(body->effect, 0.8f);
            status->effectStarted = 1;
        }
        if (RgWeaponIsTargetting(weapon0) != 0
            || RgWeaponIsTargetting(weapon1) != 0) {
            RgRobSubHoming(body->geometry, body->target, RG_ROBOT_HOMING_TURN_SPEED, deltaTime);
            body->flags |= RG_BODY_FLAG_FREE_TURN;
        }
        if (action != 0) {
            result = 1024;
        }
        break;

    case 1:
        _BodyAdvanceForAttack(body, deltaTime, 1.0f);
        if (status->boosting != 0
            && (RgWeaponIsTargetting(weapon0) != 0
                || RgWeaponIsTargetting(weapon1) != 0)) {
            RgRobSubHoming(body->geometry, body->target, RG_ROBOT_HOMING_TURN_SPEED, deltaTime);
            body->flags |= RG_BODY_FLAG_FREE_TURN;
        }
        if (action != 0) {
            switch ((unsigned int) status->scratch0) {
            case 1:
                RgWeaponShot(weapon0);
                break;
            case 3:
                RgWeaponShot(weapon0);
            case 2:
                RgWeaponShot(weapon1);
                break;
            }
        } else {
            RgWeaponShotStop(weapon0);
            RgWeaponShotStop(weapon1);
        }
        break;

    default:
        break;
    }
    status->shotPending = 0;
    return result | 0x200;
}

static void _HissatuExit(RgRobotStatus *status, RgBody *body)
{
    float maxSpeed;
    void *weapon0;
    void *weapon1;

    (void)status;
    weapon0 = body->weapon[RG_EQUIP_TYPE_LEFT];
    weapon1 = body->weapon[RG_EQUIP_TYPE_RIGHT];
    maxSpeed = _CalcMaxSpeed(body, body->spec->moveSpeedMax);
    if (weapon0 != 0) {
        RgWeaponRestartLockon(weapon0);
        RgWeaponShotStop(weapon0);
    }
    if (weapon1 != 0) {
        RgWeaponRestartLockon(weapon1);
        RgWeaponShotStop(weapon1);
    }
    _StopHomingThread(body->homingThread);
    RgGeomPointSetMaxXYSpd(body->geometry, maxSpeed);
    RgRobotEffectTermDash(body->effect);
    RgRobotEffectTermJet(body->effect);
}

static void _InitStatusHissatu(RgRobotStatus *status, RgBody *body)
{
    RgMotionShotInfo *motionInfo;
    int motion;

    motion = RG_MOT_HISSATU;
    motionInfo = RgMotionInfoDBGet(InstanceOfRgMotionInfoDB(), motion, body->charID);
    _SetStatus(status, RG_STATUS_HISSATU, body);
    status->execCmdMethod = _ExecCmdInHissatu;
    status->scratch1 = motion;
    status->passTimeMethod = _HissatuPassTime;
    status->exitMethod = _HissatuExit;
    status->scratch0 = 0;
    status->scratchTimer = 0.0f;
    status->scratch10 = 0;
    status->scratch4 = 1;
    status->eSide = 1;
    status->scratch5 = 0;
    status->hissatuMotionInfo = motionInfo;
    _BodyPlayMotion(body, RG_MOT_HISSATU);
}

static int _ExecCmdInShield(RgRobotStatus *status, RgBody *body, RgCmd *command)
{
    int result;
    int type;
    long kind;
    void *weapon;

    result = _AllowDamageCmd(status, body, command);
    if (result != 0) {
        return result;
    }
    type = command->type;
    kind = type;
    if (type == RG_CMD_SHOT) {
        if (status->scratch2 == command->param) {
            if (status->scratch0 != type) {
                status->scratch1 = 1;
            }
        }
    }
    if (kind == RG_CMD_INVALID_ATTACK) {
        weapon = *(void **) &status->scratchTimer;
        if (weapon != 0) {
            RgWeaponSubShotNum(weapon, command->scratch0);
        }
        status->scratch4 = 1;
        result |= 0x400;
        XrgCopyVector((float *) &status->dashMotionPlayed, command->vector);
        type = command->type;
    }
    if (type == RG_CMD_SHOT) {
        if (status->scratch2 != 2) {
            if (status->scratch0 == 0) {
                if (_IsBackWeaponReady(body, command->param)) {
                    result |= _ExecShotOrAttackCmd(status, body, command);
                }
            }
        }
    }
    return result;
}

static int _ShieldPassTime(RgRobotStatus *status, RgBody *body, float deltaTime)
{
    void *actor;
    RgMatrix localFrame;
    struct RgMotionAction *action;
    RgWeapon *weapon;
    RgMotionShotInfo *motionInfo;
    float weight;
    unsigned int state;

    actor = body->actor;
    weapon = status->shieldWeapon;
    motionInfo = status->motionInfo;
    if (status->scratch4 != 0) {
        RgVector force;
        weight = RgGeomPointGetWeight(body->geometry);
        status->scratch4 = 0;
        action = RgMotionInfoGetActionInTime(motionInfo, 3, RG_ROBOT_TIME_UNLIMITED);
        XrgScaleVector(force, (float *) &status->dashMotionPlayed,
                       -weight * 7.0f / deltaTime);
        RgGeomPointAddForce(body->geometry, force);
        if (action != 0) {
            _BodyPlayMotion(body, action->shiftMotionNo);
        }
    }
    {
    state = *(unsigned int *)&status->scratch0;
    switch (state) {
    case 0:
        if (actor == 0) {
            status->scratch0 = 1;
        } else if (XrgActorIsEndOfMotion(actor)) {
            status->scratch0 = 1;
        }
        break;
    case 1:
        body->shieldActive = 1;
        RgGeomRobotCalcLocal(&body->geometry->geom, localFrame);
        XrgNegateVector(body->shieldDirection, &localFrame[8]);
        body->shieldDirection[1] = 0.0f;
        if (!status->scratch1 || RgWeaponGetShotNum(weapon) <= 0.0f) {
            action = RgMotionInfoGetActionInTime(motionInfo, 2, RG_ROBOT_TIME_UNLIMITED);
            if (action != 0) {
                _BodyPlayMotion(body, action->shiftMotionNo);
                if (weapon != 0) {
                    RgWeaponPlayMotion(weapon, action->weaponMotionNo);
                }
            }
            status->scratch0 = 2;
        }
        break;
    case 2:
        body->shieldActive = 0;
        if (actor == 0 || XrgActorIsEndOfMotion(actor)) {
            _InitMovingStatus(status, body);
        }
        break;
    default:
        RgError(robotUnknownShieldStatusFormat, robotSourcePath, 0x63C, state);
        break;
    }
    }
    status->scratch1 = 0;
    return 0x80;
}

static void _ShieldExit(RgRobotStatus *status, RgBody *body)
{
    void *weapon;

    (void)status;
    body->shieldActive = 0;
    weapon = body->weapon[RG_EQUIP_TYPE_LEFT];
    if (weapon != 0) {
        RgWeaponRestartLockon(weapon);
    }
    weapon = body->weapon[RG_EQUIP_TYPE_RIGHT];
    if (weapon != 0) {
        RgWeaponRestartLockon(weapon);
    }
}

static void _InitShieldStatus(RgRobotStatus *status, RgBody *body, int motion,
                              RgWeapon *shieldWeapon, int scratch2Arg)
{
    RgMotionShotInfo *motionInfo;
    void *actor;

    motionInfo = RgMotionInfoDBGet(InstanceOfRgMotionInfoDB(), motion,
                                   body->charID);
    if (motionInfo == 0) {
        assert_prog(robotAssertMotionInfoNotNil, robotSourcePath, 1629);
    }
    _StopAllShotThread(body);
    _SetStatus(status, RG_STATUS_SHIELD, body);
    status->execCmdMethod = _ExecCmdInShield;
    status->passTimeMethod = _ShieldPassTime;
    status->exitMethod = _ShieldExit;
    actor = body->actor;
    status->shieldWeapon = shieldWeapon;
    *(int *)&status->scratch1 = 1;
    *(int *)&status->scratch2 = scratch2Arg;
    *(RgMotionShotInfo **)&status->motionInfo = motionInfo;
    *(int *)&status->scratch0 = 0;
    *(int *)&status->scratch4 = 0;
    if (actor != 0) {
        _BodyPlayMotion(body, motion);
    }
    if (body->weapon[RG_EQUIP_TYPE_LEFT] != 0) {
        RgWeaponStopLockon(body->weapon[RG_EQUIP_TYPE_LEFT]);
    }
    if (body->weapon[RG_EQUIP_TYPE_RIGHT] != 0) {
        RgWeaponStopLockon(body->weapon[RG_EQUIP_TYPE_RIGHT]);
    }
}

static int _ExecCmdInTargetting(RgRobotStatus *status, RgBody *body, RgCmd *command)
{
    int mask;
    float elapsedTime;
    RgWeapon *weapon;

    elapsedTime = status->elapsedTime;
    mask = _AllowDamageCmd(status, body, command);
    if (status->scratch1AsFloat <= elapsedTime) {
        switch (command->type) {
        case RG_CMD_ACCELARATE_ROTATE:
            mask |= _AllowAllCmdInMoving(status, body, command);
            _StopHomingThread(body->homingThread);
            break;
        case RG_CMD_SHOT:
            weapon = body->weapon[command->param];
            if (weapon != 0 && RgWeaponGetShotNum(weapon) > 0.0f
                && (RgWeaponGetControlFlag(weapon) & RG_WEAPON_CONTROL_KIND_MASK) == 0) {
                mask |= _AllowAllCmdInMoving(status, body, command);
            }
            break;
        case RG_CMD_ACCELARATE:
        case RG_CMD_BREAK:
        case RG_CMD_DASH:
            mask |= _AllowAllCmdInMoving(status, body, command);
            break;
        }
    }
    return mask;
}

static int _TargettingPassTime(RgRobotStatus *status, RgBody *body, float deltaTime)
{
    float elapsedTime;
    int result;

    elapsedTime = status->elapsedTime;
    result = 0;
    if (_GetStatHomingThread(body->homingThread) == 1) {
        body->flags |= RG_BODY_FLAG_FREE_TURN | RG_BODY_FLAG_NO_AUTO_HOMING;
        result = 8;
    }
    if (elapsedTime < *(float *) &status->scratch2) {
        if (status->scratch0 == 0) {
            if (*(float *) &status->scratch2 - elapsedTime < 0.2f) {
                _BodyPlayMotion(body, RG_MOT_JUMP_END);
                status->scratch0 = 1;
            }
        }
    } else {
        RgRobSubBreak(body->geometry, 0.3f, 0.3f);
        status->exitMethod = 0;
        _InitMovingStatus(status, body);
        RgRobotEffectTermJet(body->effect);
        RgRobotEffectSetJetInertia(body->effect, 1);
    }
    return result;
}

static void _ExitTargettingStatus(RgRobotStatus *status, RgBody *body)
{
    /*
     * The original frame is 0x20 bytes with a 16-byte local area below the
     * saved $16/$31, and the last call is a jal, not a sibling jump. An
     * unreferenced 16-byte local reproduces both; its original role is not
     * evidenced.
     */
    Vector4 unused;

    (void)status;
    RgRobotEffectTermJet(body->effect);
    RgRobotEffectSetJetInertia(body->effect, 1);
}

static void _InitTargettingStatus(RgRobotStatus *status, RgBody *body, int jump)
{
    RgVector velocity;
    RgVector position;
    RgGeomPoint *geometry;
    RgGeomPoint *target;
    float gravity;
    XrgSound *sound;
    int startHoming;

    target = body->target;
    if (status == 0) {
        assert_prog(robotAssertStatusNotNil, robotSourcePath, 0x6F2);
    }
    _StopAllShotThread(body);
    startHoming = RgGeomGetEventFlag(&body->eyeGeometry->geom) == 0;
    if (target == 0) {
        startHoming = 0;
    }
    if (body->confuse > 0.0f) {
        startHoming = 0;
    }
    _SetStatus(status, RG_STATUS_TARGETTING, body);
    status->execCmdMethod = _ExecCmdInTargetting;
    status->passTimeMethod = _TargettingPassTime;
    status->exitMethod = _ExitTargettingStatus;
    if (jump != 0) {
        geometry = body->geometry;
        RgGeomPointGetVel(geometry, velocity);
        __RgGeomPointGetPos(geometry, position, robotSourcePath, 0x70D);
        gravity = RgGetGeomGravity();
        if (0.0f < 7.5f - position[1]) {
            velocity[1] = __builtin_sqrtf((gravity + gravity) * (7.5f - position[1]));
        }
        RgGeomPointSetVel(geometry, velocity);
        gravity = RgGetGeomGravity();
        sound = body->landingSound;
        status->scratch2AsFloat = (velocity[1] + velocity[1]) / gravity;
        XrgSoundRingStopMoving(sound);
        XrgSoundRingMoving(body->landingSound, 1, status->scratch2AsFloat);
        RgRobotEffectStartJet(body->effect, status->scratch2AsFloat * 0.4f);
        RgRobotEffectSetJetInertia(body->effect, 0);
        body->grounded = 0;
    } else {
        status->scratch2AsFloat = 0.8f;
    }
    status->scratch1AsFloat = status->scratch2AsFloat * 0.2f;
    if (startHoming) {
        _StartHomingThread(body->homingThread, body->geometry, body->target,
                           status->scratch1AsFloat);
    }
    _BodyPlayMotion(body, RG_MOT_JUMP);
    status->scratch0 = 0;
}

static int _ExecCmdInBreaking(RgRobotStatus *status, RgBody *body, RgCmd *command)
{
    int mask;
    float delta;
    int rotateSign;

    mask = _AllowDamageCmd(status, body, command);
    if (command->type == RG_CMD_ACCELARATE_ROTATE) {
        delta = command->rotate;
        if (delta > 0.0f) {
            rotateSign = 1;
        } else if (delta < 0.0f) {
            rotateSign = -1;
        } else {
            rotateSign = 0;
        }
        RgGeomRobotAddRotForce(&body->geometry->geom,
                               (float) rotateSign * body->spec->rotateForce * 0.1f);
        body->flags |= RG_BODY_FLAG_NO_AUTO_HOMING;
    }
    return mask;
}

static int _BreakingPassTime(RgRobotStatus *status, RgBody *body, float deltaTime)
{
    float remaining;

    remaining = status->scratch0AsFloat - deltaTime;
    status->scratch0AsFloat = remaining;
    if (remaining < 0.0f) {
        _InitMovingStatus(status, body);
    }
    return 0;
}

static void _BreakingExit(RgRobotStatus *status, RgBody *body)
{
    (void)status;
    _BodyPlayMotion(body, RG_MOT_STAND);
}

static void _InitBreakingStatus(RgRobotStatus *status, RgBody *body)
{
    _StopAllShotThread(body);
    _SetStatus(status, RG_STATUS_BREAKING, body);
    status->execCmdMethod = _ExecCmdInBreaking;
    status->passTimeMethod = _BreakingPassTime;
    status->exitMethod = _BreakingExit;
    status->scratch0AsFloat = 0.3f;
    _BodyPlayMotion(body, RG_MOT_BREAK);
}

static int _DashExecCmd(RgRobotStatus *status, RgBody *body, RgCmd *command)
{
    RgVector vector;
    int result;

    result = _AllowDamageCmd(status, body, command);
    if (status->dashVRAllowed != 0) {
        if (command->type == RG_CMD_DASH_CONTINUE) {
            XrgCopyVector(vector, (float *) &status->scratch10);
            if (_InitDashVRStatus(status, body, vector)) {
                result |= 4;
            }
        }
    }
    if (command->type == RG_CMD_SHOT) {
        result = _ExecShotOrAttackCmd(status, body, command);
        if (result != 0) {
            return result;
        }
    }
    return result;
}

static int _DashPassTime(RgRobotStatus *status, RgBody *body, float deltaTime)
{
    RgRobotSpec *spec;
    float tmpVector[3];
    float maxSpeed;
    float accel;
    int ret;
    int next;
    int cmdMask;

    ret = 0;
    spec = body->spec;
    cmdMask = body->cmdMask;
    maxSpeed = _CalcMaxSpeed(body, spec->dashSpeedMax);
    switch (status->scratch0) {
    case 0:
        body->dashTime = body->dashTime - deltaTime;
        if (body->dashTime < 0.0f) {
            body->dashTime = 0.0f;
        }
        status->dashTimeStart = status->dashTimeStart - deltaTime;
        if (status->dashTimeStart < 0.0f) {
            status->dashTimeStart = 0.0f;
        }
        ret = 4;
        accel = spec->enginePower * spec->dashPower;
        RgGeomPointSetMaxXYSpd(body->geometry, maxSpeed);
        RgRobSubAcceralate(body->geometry, (float *) &status->motionInfo, accel);
        if (status->dashMotionPlayed == 0) {
            _BodyPlayMotion(body, RgRobSubGetAdvanceMot(body->geometry, 1));
            status->dashMotionPlayed = 1;
        }
        next = 0;
        if (status->dashTimeStart <= 0.0f) {
            next = 1;
            if (cmdMask & 0x40) {
                if (status->dashLimited != 0) {
                    next = 2;
                }
            }
        }
        if (RgGeomGetEventFlag(&body->advanceGeometry->geom) != 0) {
            next = 1;
        }
        if (next == 2) {
            XrgCopyVector(tmpVector, (float *) &status->motionInfo);
            _InitDashVRStatus(status, body, tmpVector);
        } else if (next == 1) {
            status->scratch0 = next;
            status->dashPhaseTimer = 0.3f;
        }
        break;
    case 1:
        if (status->dashMotionPlayed != 0) {
            _BodyPlayMotionLoop(body, RgRobSubGetAdvanceMot(body->geometry, 0));
            status->dashMotionPlayed = 0;
            RgRobotEffectTermJet(body->effect);
            RgRobotEffectTermDash(body->effect);
        }
        status->dashPhaseTimer = status->dashPhaseTimer - deltaTime;
        if (status->dashPhaseTimer < 0.0f) {
            _InitMovingStatus(status, body);
        }
        break;
    default:
        RgError(robotUnknownDashStatusFormat, robotSourcePath, 0x7FE, ((int *) &status->scratch0)[0]);
        break;
    }
    return ret;
}

static void _DashExit(RgRobotStatus *status, RgBody *body)
{
    RgRobotSpec *spec;

    spec = body->spec;
    RgGeomPointSetMaxXYSpd(body->geometry, _CalcMaxSpeed(body, spec->moveSpeedMax));
    RgRobotEffectTermJet(body->effect);
    RgRobotEffectTermDash(body->effect);
    if (status->restoreDashTime != 0) {
        body->dashTime = spec->dashTime;
    }
    XrgSoundRingStopMoving(body->landingSound);
}

static int _InitDashStatus(RgRobotStatus *status, RgBody *body, RgVector direction)
{
    RgRobotSpec *spec;
    float limit;
    float dashTime;

    spec = body->spec;
    if (status == 0) {
        assert_prog(robotAssertStatusNotNil, robotSourcePath, 0x81E);
    }
    if (spec->type == RG_DASH_MODE_VR) {
        return 0;
    }
    if (spec->type != RG_DASH_MODE_NORMAL) {
        if ((unsigned int) spec->type < RG_DASH_MODE_NUM) {
            limit = spec->dashTime * 0.2f;
            if (body->dashTime < limit) {
                return 0;
            }
            body->dashTime = body->dashTime - limit * 0.1f;
        }
    }
    _StopAllShotThread(body);
    _SetStatus(status, RG_STATUS_DASH, body);
    status->execCmdMethod = _DashExecCmd;
    status->passTimeMethod = _DashPassTime;
    status->exitMethod = _DashExit;
    status->scratch0 = 0;
    status->dashMotionPlayed = 0;
    XrgCopyVector((float *) &status->motionInfo, direction);
    status->dashVRAllowed = 0;
    switch (spec->type) {
    case RG_DASH_MODE_NORMAL:
    case RG_DASH_MODE_VR:
        status->restoreDashTime = 1;
        status->dashTimeStart = body->dashTime;
        status->dashLimited = 0;
        break;
    case RG_DASH_MODE_NEO:
        status->dashLimited = 1;
        dashTime = body->dashTime;
        if (!(dashTime < 0.5f)) {
            dashTime = 0.5f;
        }
        status->dashTimeStart = dashTime;
        status->restoreDashTime = 0;
        break;
    case RG_DASH_MODE_NEO2:
    case RG_DASH_MODE_NEO3:
        status->dashLimited = 1;
        dashTime = body->dashTime;
        dashTime = (dashTime < 0.5f) ? dashTime : 0.5f;
        status->dashTimeStart = dashTime;
        status->dashVRAllowed = 1;
        status->restoreDashTime = 0;
        break;
    default:
        RgError(robotUnknownDashModeFormat, robotSourcePath, 0x85D, spec->type);
        break;
    }
    RgRobotEffectStartJet(body->effect, spec->dashTime);
    RgRobotEffectStartDash(body->effect, spec->dashTime);
    XrgSoundRingMoving(body->landingSound, 1, RG_ROBOT_TIME_UNLIMITED);
    return 1;
}

/*
 * Sets DashVR's own scratch0/scratchTimer (see RgRobotStatus): scratch0 is
 * the follow-state tag _DashVRPassTime (still asm, ov12:0x00a052b0)
 * compares against 1/2, and scratchTimer is a countdown _DashVRPassTime
 * subtracts deltaTime from every frame.
 */
static void _DashVRMainToFollow(RgRobotStatus *status, RgBody *body)
{
    status->scratch0 = 1;
    status->scratchTimer = 0.1f;
    _BodyPlayMotion(body, RG_MOT_STAND);
}

/*
 * Clamp value into [min, max] as one ternary chain, the same compare and
 * select shape as _ClampStiffTime.
 */
static inline float _ClampFloat(float value, float min, float max)
{
    return value < min ? min : (value > max ? max : value);
}

static int _DashVRExecCmd(RgRobotStatus *status, RgBody *body, RgCmd *command)
{
    int result;
    int flag;
    int sign;
    int type;
    float angle;
    float diff;
    float value;
    float step;
    float dot;
    RgWeapon *weapon;
    RgVector vec;
    RgVector scaled;

    result = _AllowDamageCmdNotPetrify(status, body, command);
    type = command->type;
    angle = ((float *) &status->scratch1)[0];
    if (type == RG_CMD_TARGETTING) {
        _InitTargettingStatus(status, body, 1);
        return result | 0x100;
    }
    if (type == RG_CMD_ACCELARATE_ROTATE) {
        if (command->vector[0] > 0.0f) {
            sign = 1;
        } else if (command->vector[0] < 0.0f) {
            sign = -1;
        } else {
            sign = 0;
        }
        value = status->scratch1AsFloat + (float) sign * 1.5707964f;
        if (3.1415927f < value) {
            value -= 6.2831855f;
            while (3.1415927f < value) {
                value -= 6.2831855f;
            }
        }
        if (value < -3.1415927f) {
            value += 6.2831855f;
            while (value < -3.1415927f) {
                value += 6.2831855f;
            }
        }
        angle = value;
    }
    if (type == RG_CMD_ACCELARATE) {
        angle = atan2f(command->vector[0], command->vector[2]);
    }
    type = command->type;
    if (type == RG_CMD_SHOT) {
        weapon = body->weapon[command->param];
        if (weapon != 0) {
            flag = RgWeaponGetControlFlag(weapon) & RG_WEAPON_CONTROL_KIND_MASK;
            if ((flag == type || flag == 4) && RgWeaponGetShotNum(weapon) > 0.0f) {
                result = _ExecShotOrAttackCmd(status, body, command);
                if (result != 0) {
                    return result;
                }
            } else if (status->scratch0 == 0) {
                _DashVRMainToFollow(status, body);
            }
        }
        type = command->type;
    }
    if (type == RG_CMD_HIT_BG) {
        vec[0] = sinf(status->scratch1AsFloat);
        vec[1] = 0.0f;
        vec[2] = cosf(status->scratch1AsFloat);
        dot = vec[0] * command->vector[0] + vec[1] * command->vector[1]
            + vec[2] * command->vector[2];
        XrgScaleVector(scaled, command->vector, -dot);
        XrgAddVector(vec, vec, scaled);
        angle = atan2f(vec[0], vec[2]);
        status->scratch1AsFloat = angle;
    }
    diff = angle - status->scratch1AsFloat;
    if (3.1415927f < diff) {
        diff -= 6.2831855f;
        while (3.1415927f < diff) {
            diff -= 6.2831855f;
        }
    }
    if (diff < -3.1415927f) {
        diff += 6.2831855f;
        while (diff < -3.1415927f) {
            diff += 6.2831855f;
        }
    }
    step = diff / 0.2f;
    step = _ClampFloat(step, -RG_ROBOT_HOMING_TURN_SPEED, RG_ROBOT_HOMING_TURN_SPEED);
    ((float *) &status->scratch2)[0] += step;
    return result;
}

static int _DashVRPassTime(RgRobotStatus *status, RgBody *body, float deltaTime)
{
    RgRobotSpec *spec;
    RgGeomPoint *geometry;
    RgVector vel;
    RgVector pos;
    float maxSpeed;
    float angle;
    int cmdMask;
    int ret;
    int flag;
    int rollMotion;

    ret = 4;
    flag = 0;
    geometry = body->geometry;
    spec = body->spec;
    cmdMask = body->cmdMask;
    maxSpeed = _CalcMaxSpeed(body, spec->dashSpeedMax);
    status->scratchTimer = status->scratchTimer - deltaTime;
    switch ((unsigned int) status->scratch0) {
    case 0:
        angle = status->scratch1AsFloat
            + ((float *) &status->scratch2)[0] * deltaTime;
        if (3.1415927f < angle) {
            angle -= 6.2831855f;
            while (3.1415927f < angle) {
                angle -= 6.2831855f;
            }
        }
        if (angle < -3.1415927f) {
            angle += 6.2831855f;
            while (angle < -3.1415927f) {
                angle += 6.2831855f;
            }
        }
        status->scratch1AsFloat = angle;
        RgGeomRobotSetRotate(&geometry->geom, angle);
        ((float *) &status->scratch2)[0] =
            ((float *) &status->scratch2)[0] * (deltaTime * 0.7f);
        RgGeomPointGetVel(geometry, vel);
        vel[0] = sinf(status->scratch1AsFloat) * maxSpeed;
        vel[2] = cosf(status->scratch1AsFloat) * maxSpeed;
        RgGeomPointSetVel(geometry, vel);
        body->dashTime = body->dashTime - deltaTime;
        if (body->dashTime <= 0.0f) {
            body->dashTime = 0.0f;
            flag = 1;
        }
        switch (status->dashVRMode) {
        case 0:
            if ((cmdMask & 0x40) == 0) {
                flag = 1;
            }
            break;
        case 1:
            if ((cmdMask & 0x10) != 0) {
                flag = 1;
            }
            break;
        default:
            RgError(robotUnknownVrDashCmdModeFormat, robotSourcePath, 0x900, status->dashVRMode);
        }
        if (flag != 0) {
            _DashVRMainToFollow(status, body);
        }
        ret = 260;
        break;
    case 1:
        RgRobotEffectTermJet(body->effect);
        RgRobotEffectTermDash(body->effect);
        if (status->scratchTimer < 0.0f) {
            status->scratch0 = 2;
            status->dashMotionPlayed = -1;
            status->scratchTimer = 0.2f;
        }
        /* falls through */
    case 2:
        if (status->scratchTimer < 0.0f) {
            _InitMovingStatus(status, body);
        } else {
            __RgGeomPointGetPos(body->geometry, pos,
                                robotSourcePath, 0x920);
            XrgAddVector(pos, pos, (float *) &status->motionInfo);
            RgRobSubTargetting(body->geometry, pos,
                               RG_ROBOT_HOMING_TURN_SPEED, deltaTime);
            body->flags |= RG_BODY_FLAG_FREE_TURN | RG_BODY_FLAG_NO_AUTO_HOMING;
        }
        rollMotion = RgRobSubGetRollMotion(body->geometry);
        if (rollMotion != status->dashMotionPlayed) {
            status->dashMotionPlayed = rollMotion;
            _BodyPlayMotion(body, rollMotion);
        }
        break;
    default:
        RgError(robotUnknownVrDashModeFormat, robotSourcePath, 0x931, ((int *) &status->scratch0)[0]);
    }
    return ret;
}

static void _DashVRExit(RgRobotStatus *status, RgBody *body)
{
    (void)status;
    RgGeomPointSetMaxXYSpd(body->geometry, _CalcMaxSpeed(body, body->spec->moveSpeedMax));
    RgRobotEffectTermJet(body->effect);
    RgRobotEffectTermDash(body->effect);
    XrgSoundRingStopMoving(body->landingSound);
}

static int _InitDashVRStatus(RgRobotStatus *status, RgBody *body, RgVector direction)
{
    RgRobotSpec *spec;
    RgGeomPoint *geometry;
    unsigned int type;
    float angle;
    float rotate;

    spec = body->spec;
    if (status == 0) {
        assert_prog(robotAssertStatusNotNil, robotSourcePath, 0x94D);
    }
    _StopAllShotThread(body);
    if (body->dashTime < spec->dashTime * 0.2f) {
        return 0;
    }
    _SetStatus(status, RG_STATUS_DASH_VR, body);
    status->execCmdMethod = _DashVRExecCmd;
    status->passTimeMethod = _DashVRPassTime;
    status->exitMethod = _DashVRExit;
    *(int *) &status->scratch0 = 0;
    angle = atan2f(direction[0], direction[2]);
    status->scratch2 = 0;
    geometry = body->geometry;
    status->scratch1AsFloat = angle;
    rotate = RgGeomRobotGetRotate(&geometry->geom);
    ((float *) &status->motionInfo)[0] = sinf(rotate) * 50.0f;
    status->scratch4 = 0;
    ((float *) &status->motionInfo)[2] = cosf(rotate) * 50.0f;
    type = spec->type;
    switch (type) {
    case RG_DASH_MODE_NEO2:
        status->dashVRMode = 1;
        break;
    case RG_DASH_MODE_NEO:
    case RG_DASH_MODE_NEO3:
        status->dashVRMode = 0;
        break;
    default:
        RgError(robotCannotTransitVrDashModeFormat, robotSourcePath, 0x971, type);
        break;
    }
    RgRobotEffectStartJet(body->effect, spec->dashTime);
    RgRobotEffectStartDash(body->effect, spec->dashTime);
    RgGeomPointSetMaxXYSpd(body->geometry, _CalcMaxSpeed(body, spec->dashSpeedMax));
    _BodyPlayMotion(body, 0x32);
    XrgSoundRingMoving(body->landingSound, 1, RG_ROBOT_TIME_UNLIMITED);
    return 1;
}

/*
 * Damage status's own scratch1 (see RgRobotStatus) is nonzero while a
 * dash-escape is still allowed, gating whether command type 5 (dash) is
 * forwarded to _ExecDashCmd.
 */
static int _DamageExecCmd(RgRobotStatus *status, RgBody *body, RgCmd *command)
{
    int result;

    result = _AllowDamageCmdNotVRDashHit(status, body, command);
    if (result == 0) {
        if (status->scratch1 != 0 && command->type == RG_CMD_DASH) {
            result = _ExecDashCmd(status, body, command);
        }
        return result;
    }
}

static int _DamagePassTime(RgRobotStatus *status, RgBody *body, float deltaTime)
{
    (void)deltaTime;
    if (status->elapsedTime >= status->scratch0AsFloat) {
        _InitMovingStatus(status, body);
    }
    return 0x10;
}

/*
 * stiffScale scales the spec's damage-stiff-time, clamped to 0.1..10 s
 * through the inline helper (the original keeps the bound in $f1, as
 * with RgDispLifeSetWin's clamp); a positive stiffTime overrides it, and
 * dashEscape becomes the Damage status's scratch1.
 *
 * The float argument comes before the flag: _ExecDamageCmd loads the
 * flag (lw $7, ov12:0x00a07c10) ahead of the float (lwc1 $f12 in the
 * jal delay slot), which GCC only schedules that way for this order.
 */
static inline float _ClampStiffTime(float time, float min, float max)
{
    return time < min ? min : (time > max ? max : time);
}

static void _InitDamageStatus(RgRobotStatus *status, RgBody *body,
                              RgCmd *command, float stiffScale, int dashEscape,
                              float stiffTime)
{
    RgRobotSpec *spec;
    float time;

    spec = body->spec;
    if (status == 0) {
        assert_prog(robotAssertStatusNotNil, robotSourcePath, 2478);
    }
    _StopAllShotThread(body);
    _SetStatus(status, RG_STATUS_DAMAGE, body);
    body->weakHitAccum = 0.0f;
    body->weakHitWindow = 0.0f;
    status->execCmdMethod = _DamageExecCmd;
    status->passTimeMethod = _DamagePassTime;
    status->scratch0AsFloat = _ClampStiffTime(stiffScale * spec->damageStiffTime,
                                              0.1f, 10.0f);
    status->scratch1 = dashEscape;
    if (stiffTime > 0.0f) {
        status->scratch0AsFloat = stiffTime;
    }
    _BodyPlayMotion(body, RgRobSubGetDamageMotion(body->geometry, command->vector));
}

/*
 * Drop status's own scratch0/scratch2 (see RgRobotStatus): scratch0 is the
 * equip side already being dropped and scratch2 is nonzero while that
 * side's spare weapon still needs removing (see _DropPassTime, which also
 * reads and clears it).
 */
static int _ExecCmdInDrop(RgRobotStatus *status, RgBody *body, RgCmd *command)
{
    int mask;

    mask = _AllowDamageCmd(status, body, command);
    if (command->type == RG_CMD_DROP_WEAPON
        && command->param != status->scratch0
        && command->param == 2
        && status->scratch2 != 0) {
        mask |= 0x200;
        _InitDropStatus(status, body, _GetDropMotion(2), 2);
    }
    return mask;
}

/*
 * Drop status's own scratch1 (see RgRobotStatus) is the drop motion id
 * (see _GetDropMotion) checked against elapsedTime, and scratch0/scratch2
 * are the same equip side and pending flag _ExecCmdInDrop evidences. Once
 * the drop motion's timed removal point is reached, this clears the
 * body's spare weapon at that side (see _BodySetSpareWeapon, RgBody+0x7c)
 * by handing it to _BodyEquipWeapon.
 */
static int _DropPassTime(RgRobotStatus *status, RgBody *body, float deltaTime)
{
    float elapsedTime;
    int side;

    (void)deltaTime;
    elapsedTime = status->elapsedTime;
    if (RgMotionInfoGetActionInTime(status->dropMotionInfo, 0, elapsedTime) != 0
        && status->scratch2 != 0) {
        side = status->scratch0;
        _BodyEquipWeapon(body, side, body->spareWeapon[side]);
        body->spareWeapon[side] = 0;
        status->scratch2 = 0;
    }
    if (status->scratch2 == 0 && elapsedTime > 0.15f) {
        _InitMovingStatus(status, body);
    }
    return 0;
}

static int _InitDropStatus(RgRobotStatus *status, RgBody *body, int motion,
                           int eSide)
{
    RgMotionShotInfo *motionInfo;

    motionInfo = RgMotionInfoDBGet(InstanceOfRgMotionInfoDB(), motion,
                                   body->charID);
    if (body->weapon[eSide] == 0) {
        return 0;
    }
    if (motionInfo == 0) {
        return 0;
    }
    if ((RgWeaponGetControlFlag(body->weapon[eSide]) & RG_WEAPON_CONTROL_KIND_MASK)
        == RG_WEAPON_CONTROL_KIND_UNARMED) {
        return 0;
    }
    if (RgWeaponIsAttachedShot(body->weapon[eSide]) != 0) {
        return 0;
    }
    _SetStatus(status, RG_STATUS_DROP, body);
    status->execCmdMethod = _ExecCmdInDrop;
    status->passTimeMethod = _DropPassTime;
    status->scratch0 = eSide;
    status->dropMotionInfo = motionInfo;
    status->scratch2 = 1;
    _BodyPlayMotion(body, motion);
    return 1;
}

static RgCmd *_EntryCmdQueue(void *cmdQueue, int cmdType)
{
    RgCmdQueue *pQ = cmdQueue;
    int level;
    int diff;
    RgCmd *command;

    if (pQ == 0) {
        assert_prog(robotAssertCmdQueueNotNil, robotSourcePath, 0xA41);
    }
    if (pQ->count >= RG_CMD_QUEUE_CAPACITY) {
        return 0;
    }
    if ((unsigned int) cmdType >= RG_CMD_TYPE_NUM) {
        return 0;
    }
    pQ->entryCount++;
    level = s_anCmdLevel[cmdType];
    if (pQ->count != 0) {
        diff = s_anCmdLevel[pQ->cmd[0].type] - level;
    } else {
        diff = 0;
    }
    if (diff > 0) {
        return 0;
    }
    if (diff < 0) {
        pQ->count = 0;
    }
    command = &pQ->cmd[pQ->count++];
    command->type = cmdType;
    return command;
}

/* ov12:0x00a51a58 contains the assertion expression "pQ != NIL". */

static void _InitCmdQueue(RgCmdQueue *cmdQueue)
{
    if (cmdQueue == 0) {
        assert_prog(robotAssertCmdQueueNotNil, robotSourcePath, 2673);
    }
    cmdQueue->count = 0;
    cmdQueue->entryCount = 0;
}

static void _ClearCmdQueue(RgCmdQueue *cmdQueue)
{
    if (cmdQueue == 0) {
        assert_prog(robotAssertCmdQueueNotNil, robotSourcePath, 2681);
    }
    cmdQueue->count = 0;
}

static RgCmdQueue *_CreateCmdQueue(void)
{
    RgCmdQueue *cmdQueue;

    cmdQueue = RgHeapAlloc(InstanceOfRgHeap(), sizeof(RgCmdQueue), robotSourcePath, 2689);
    _InitCmdQueue(cmdQueue);
    return cmdQueue;
}

static void _DisposeCmdQueue(RgCmdQueue *cmdQueue)
{
    if (cmdQueue == 0) {
        assert_prog(robotAssertCmdQueueNotNil, robotSourcePath, 2697);
    }
    RgHeapFree(InstanceOfRgHeap(), cmdQueue, robotSourcePath, 2698);
}

static void _BodyPlayMotion(RgBody *body, int motion)
{
    XrgActor *actor;

    actor = body->actor;
    if (actor != 0) {
        if (body->smoothSuppressed != 0) {
            XrgActorSetSmoothPlay(actor, 0);
        } else {
            XrgActorSetSmoothPlay(actor, body->motSmooth);
        }
        XrgActorSetMotion(actor, motion);
    }
}

static void _BodyPlayMotionLoop(RgBody *body, int motion)
{
    XrgActor *actor;

    actor = body->actor;
    if (actor != 0) {
        if (body->smoothSuppressed != 0) {
            XrgActorSetSmoothPlay(actor, 0);
        } else {
            XrgActorSetSmoothPlay(actor, body->motSmooth);
        }
        XrgActorSetLoopPlay(actor, 1);
        XrgActorSetMotion(actor, motion);
    }
}

static void _BodyAdvanceForAttack(RgBody *body, float attackPower,
                                  float attackFactor)
{
    RgGeomPoint *geometry;
    RgRobotSpec *spec;
    RgVector direction;
    RgMatrix localFrame;

    geometry = body->geometry;
    spec = body->spec;
    RgGeomRobotCalcLocal(&geometry->geom, localFrame);
    /* row 2 of the local frame: the body's forward (z) axis */
    XrgNegateVector(direction, &localFrame[8]);
    RgRobSubAcceralate(geometry, direction,
                       spec->enginePower * spec->dashPower * 0.3f * attackFactor);
}

/* ov12:0x00a51a68 contains the assertion expression "pBody != NIL". */

static int _BodyGetTargetPos(RgBody *body, RgVector position)
{
    if (body == 0) {
        assert_prog(robotAssertBodyNotNil, robotSourcePath, 2758);
    }
    if (body->target != 0) {
        __RgGeomPointGetPos(body->target, position, robotSourcePath, 2760);
        return 1;
    }
    return 0;
}

static void _BodySetSpec(RgBody *body, RgRobotSpec *spec)
{
    float life;

    if (body == 0) {
        assert_prog(robotAssertBodyNotNil, robotSourcePath, 2773);
    }
    if (spec != 0) {
        body->spec = spec;
        body->dashTime = spec->dashTime;
        body->lifeMax = body->life = spec->life;
        RgGeomPointSetMaxXYSpd(body->geometry, _CalcMaxSpeed(body, spec->moveSpeedMax));
        RgRobotEffectSetJetPtclName(body->effect, spec->jetEffect);
        RgRobotEffectSetDashPtclName(body->effect, spec->dashEffect);
    }
}

/* ov12:0x00a51a78 contains the assertion expression "pGeom != NIL". */

static void _BodySetGeom(RgBody *body, RgGeomPoint *geometry)
{
    RgGeomPoint *oldGeometry;
    RgRobotSpec *spec;

    if (body == 0) {
        assert_prog(robotAssertBodyNotNil, robotSourcePath, 0xAEA);
    }
    if (geometry == 0) {
        assert_prog(robotAssertGeomNotNil, robotSourcePath, 0xAEB);
    }
    oldGeometry = body->geometry;
    if (geometry != oldGeometry) {
        RgGeomFree(&oldGeometry->geom);
    }
    spec = body->spec;
    body->geometry = geometry;
    RgGeomPointSetWeight(geometry, spec->weight);
    RgGeomPointSetMoveResist(geometry, spec->moveResist);
    RgGeomRobotSetRotResist(&geometry->geom, spec->rotResist);
    body->rotate = RgGeomRobotGetRotate(&geometry->geom);
    RgRobotEffectSetGeom(body->effect, geometry);
    RgGeomPointSetMaxXYSpd(body->geometry, _CalcMaxSpeed(body, spec->moveSpeedMax));
}

static void _BodySetEyeGeom(RgBody *body, RgGeomPoint *eyeGeometry)
{
    RgGeomPoint *oldEyeGeometry;

    if (body == 0) {
        assert_prog(robotAssertBodyNotNil, robotSourcePath, 2816);
    }
    oldEyeGeometry = body->eyeGeometry;
    if (oldEyeGeometry != 0 && oldEyeGeometry != eyeGeometry) {
        RgGeomFree(&oldEyeGeometry->geom);
    }
    body->eyeGeometry = eyeGeometry;
}

/* ov12:0x00a51a78 contains the assertion expression "pGeom != NIL". */
static void _BodySetAdvanceGeom(RgBody *body, RgGeomPoint *advanceGeometry)
{
    RgGeomPoint *oldAdvanceGeometry;

    if (body == 0) {
        assert_prog(robotAssertBodyNotNil, robotSourcePath, 2826);
    }
    if (advanceGeometry == 0) {
        assert_prog(robotAssertGeomNotNil, robotSourcePath, 2827);
    }
    oldAdvanceGeometry = body->advanceGeometry;
    if (oldAdvanceGeometry != 0 && oldAdvanceGeometry != advanceGeometry) {
        RgGeomFree(&oldAdvanceGeometry->geom);
    }
    body->advanceGeometry = advanceGeometry;
}

static void _BodySetSound(RgBody *body, XrgSound *sound)
{
    unsigned int eSide;
    RgWeapon *weapon;

    if (body->landingSound != 0 && body->landingSound != sound) {
        DisposeXrgSound(body->landingSound);
    }
    body->landingSound = sound;
    eSide = 0;
    while (eSide < RG_EQUIP_TYPE_NUM) {
        if ((weapon = body->weapon[eSide]) != 0) {
            RgWeaponSetSound(weapon, body->landingSound);
        }
        if ((weapon = body->spareWeapon[eSide]) != 0) {
            RgWeaponSetSound(weapon, body->landingSound);
        }
        eSide++;
    }
}

static void _BodySetActor(RgBody *body, XrgActorEssence *actorSpec)
{
    XrgActor *oldActor;
    XrgActor *actor;

    if (body == 0) {
        assert_prog(robotAssertBodyNotNil, robotSourcePath, 0xB2A);
    }
    oldActor = body->actor;
    if (oldActor != 0) {
        DisposeXrgActor(oldActor);
    }
    body->actor = 0;
    if (actorSpec != 0) {
        actor = CreateXrgActor(actorSpec);
        body->actor = actor;
        body->charID = actorSpec->actorID;
        RgRobotEffectSetActor(body->effect, actor);
        _BodyPlayMotionLoop(body, RG_MOT_STAND);
    }
}

static void _BodySetTarget(RgBody *body, RgGeomPoint *target)
{
    if (body == 0) {
        assert_prog(robotAssertBodyNotNil, robotSourcePath, 2890);
    }
    body->target = target;
}

static int _BodyDropWeapon(RgBody *body, unsigned int eType)
{
    RgWeapon *weapon;
    XrgActor *actor;
    XrgActor *weaponActor;
    RgGeomPoint *geometry;
    float geomWeight;

    if (body == 0) {
        assert_prog(robotAssertBodyNotNil, robotSourcePath, 2900);
    }
    if (eType >= RG_EQUIP_TYPE_NUM) {
        assert_prog(robotAssertEquipTypeInRange, robotSourcePath, 2901);
    }
    weapon = body->weapon[eType];
    if (weapon != 0) {
        geometry = body->geometry;
        geomWeight = RgGeomPointGetWeight(geometry);
        RgGeomPointSetWeight(geometry, geomWeight - RgWeaponGetWeight(weapon));
        actor = body->actor;
        weaponActor = RgWeaponGetActor(weapon);
        if (actor != 0 && weaponActor != 0) {
            XrgActorDettachChild(actor, weaponActor);
        }
        RgWeaponShotStop(weapon);
        RgWeaponSetFree(weapon);
        RgWeaponDisposeLockon(weapon);
        RgWeaponSetdown(weapon);
        CreateRgDropWeapon(DisposeRgWeaponNotActor(weapon));
        body->weapon[eType] = 0;
        RgShotThreadInit(body->shotThread[eType], 0, -1, 0);
    }
    return 0;
}

static int _BodyEquipWeapon(RgBody *body, int side, RgWeapon *weapon)
{
    RgWeaponEssence *ess;
    RgEquip *equip;
    int mountPos;
    int jointID;
    XrgActor *weaponActor;
    XrgActor *lockonActor;
    RgWeapon *otherWeapon;
    RgShotThread *shotThread;
    float geomWeight;
    int result;

    if (body == 0) {
        assert_prog(robotAssertBodyNotNil, robotSourcePath, 0xBA9);
    }
    if ((unsigned int) side >= RG_EQUIP_TYPE_NUM) {
        assert_prog(robotAssertEquipTypeInRange, robotSourcePath, 0xBAA);
    }
    result = _BodyDropWeapon(body, side);
    if (weapon == 0) {
        return result;
    }
    ess = RgWeaponGetEss(weapon);
    if (ess != 0) {
        equip = RgWeaponGetEquip(weapon);
        mountPos = RgEquipMountPos(equip, side, body->charID, ess->databaseName);
    } else {
        equip = RgWeaponGetEquip(weapon);
        mountPos = RgEquipMountPos(equip, side, body->charID, robotEmptyEquipDatabaseName);
    }
    body->weapon[side] = weapon;
    geomWeight = RgGeomPointGetWeight(body->geometry);
    RgGeomPointSetWeight(body->geometry,
                         geomWeight + RgWeaponGetWeight(weapon));
    if (RgWeaponGetActor(weapon) != 0 && body->actor != 0) {
        lockonActor = 0;
        RgWeaponGetActor(weapon);
        jointID = RgEquipPosToJntID(mountPos);
        if (body->weapon[RG_EQUIP_TYPE_BACK] != 0) {
            lockonActor = RgWeaponGetActor(body->weapon[RG_EQUIP_TYPE_BACK]);
        }
        if (mountPos == -1) {
            assert_prog(robotAssertEquipPosValid, robotSourcePath, 0xBE3);
        }
        weaponActor = RgWeaponGetActor(weapon);
        XrgActorDeriveMotion(weaponActor, body->actor);
        if (jointID == -1) {
            assert_prog(robotAssertJointIdValid, robotSourcePath, 0xBEA);
        }
        XrgActorAttachChild(body->actor, weaponActor, jointID);

        if (side == 0 && lockonActor != 0) {
            if (RgWeaponIsActiveLockOnSys(weapon)) {
                if (XrgActorGetAttachedJoint(lockonActor) == 4) {
                    XrgActorCheckPakaPaka(lockonActor);
                }
            }
        } else if (side == 1 && lockonActor != 0) {
            if (RgWeaponIsActiveLockOnSys(weapon)) {
                if (XrgActorGetAttachedJoint(lockonActor) == 5) {
                    XrgActorCheckPakaPaka(lockonActor);
                }
            }
        } else if (side == RG_EQUIP_TYPE_BACK) {
            otherWeapon = 0;
            if (jointID == 4) {
                otherWeapon = body->weapon[RG_EQUIP_TYPE_LEFT];
            } else if (jointID == 5) {
                otherWeapon = body->weapon[RG_EQUIP_TYPE_RIGHT];
            }
            if (otherWeapon != 0 && RgWeaponIsActiveLockOnSys(otherWeapon)
                && (RgWeaponGetControlFlag(otherWeapon) & RG_WEAPON_CONTROL_LOCKON)) {
                XrgLog(robotCheckPakaPakaLog, robotSourcePath, 0xC0E);
                XrgActorCheckPakaPaka(weaponActor);
                XrgLog(robotCheckPakaPakaEndLog, robotSourcePath, 0xC10);
            } else {
                XrgLog(robotNoLockOnLog, robotSourcePath, 0xC12);
            }
        }
    }

    if ((RgWeaponGetControlFlag(weapon) & RG_WEAPON_CONTROL_KIND_MASK)
        == 0) {
        RgShotThreadInit(body->shotThread[side], weapon, side, body->actor);
    } else {
        RgShotThreadInit(body->shotThread[side], 0, -1, 0);
    }
    if (mountPos == 2 || mountPos == 5) {
        RgWeaponDisposeLockon(weapon);
        shotThread = body->shotThread[side];
        if (shotThread != 0) {
            RgShotThreadMotOff(shotThread);
        }
    }
    if ((unsigned int) (mountPos - 6) < 2) {
        RgWeaponDisposeLockon(weapon);
        shotThread = body->shotThread[side];
        if (shotThread != 0) {
            RgShotThreadMotOff(shotThread);
        }
    }
    _BodySetSound(body, body->landingSound);
    return result;
}

static void _BodySetSpareWeapon(RgBody *body, int eSide, RgWeapon *weapon)
{
    if (body == 0) {
        assert_prog(robotAssertBodyNotNil, robotSourcePath, 0xC3B);
    }
    if ((unsigned int) eSide >= RG_EQUIP_TYPE_NUM) {
        assert_prog(robotAssertEquipTypeInRange, robotSourcePath, 0xC3C);
    }
    if (body->spareWeapon[eSide] != 0 && body->spareWeapon[eSide] != weapon) {
        DisposeRgWeapon(body->spareWeapon[eSide]);
    }
    body->spareWeapon[eSide] = weapon;
}

static void _BodySetAutoHomingEnv(RgBody *body, int autoHomingEnv)
{
    if (body == 0) {
        assert_prog(robotAssertBodyNotNil, robotSourcePath, 3142);
    }
    body->autoHomingEnv = autoHomingEnv;
}

/* ov12:0x00a51b48 contains the assertion expression "pCmd != NIL". */

static int _BodyExecCmd(RgRobotStatus *robotStatus, RgBody *body,
                        RgCmd *command)
{
    if (body == 0) {
        assert_prog(robotAssertBodyNotNil, robotSourcePath, 3152);
    }
    if (command == 0) {
        assert_prog(robotAssertCmdNotNil, robotSourcePath, 3153);
    }
    body->cmdMask |= 1 << command->type;
    return _StatusExecCmd(robotStatus, command, body);
}

static void _BodyGeomPassTime(RgBody *body, float deltaTime)
{
    RgGeomPoint *geometry;
    RgRobotSpec *spec;
    float speed;
    float maxSpeed;
    float rotVelScale;

    geometry = body->geometry;
    spec = body->spec;
    speed = RgGeomPointGetSpeed(geometry);
    maxSpeed = _CalcMaxSpeed(body, spec->moveSpeedMax);
    if (!(body->flags & RG_BODY_FLAG_FREE_TURN)) {
        rotVelScale = (maxSpeed - speed) / maxSpeed;
        if (!(rotVelScale > 0.5f)) {
            rotVelScale = 0.5f;
        }
        RgGeomRobotSetMaxRotVel(&geometry->geom, spec->turnRate * rotVelScale);
    } else {
        RgGeomRobotSetMaxRotVel(&geometry->geom, RG_ROBOT_ROT_VEL_UNLIMITED);
    }
    body->flags &= ~RG_BODY_FLAG_FREE_TURN;
    RgGeomPassTime(&geometry->geom, deltaTime);
}

static int _EyeGeomPassTime(RgBody *body, float deltaTime)
{
    RgGeomPoint *eyeGeom;
    int eventFlag;
    RgVector position;

    eyeGeom = body->eyeGeometry;
    eventFlag = RgGeomGetEventFlag(&eyeGeom->geom);
    RgGeomPassTime(&eyeGeom->geom, deltaTime);
    __RgGeomPointGetPos(body->geometry, position, robotSourcePath, 0xC79);
    RgGeomPointSetPos(eyeGeom, position);
    if (body->target != 0) {
        __RgGeomPointGetPos(body->target, position, robotSourcePath, 0xC7E);
        RgGeomPointMovePos(eyeGeom, position);
    }
    return eventFlag;
}

static void _AdvanceGeomPassTime(RgBody *body, float deltaTime)
{
    RgGeomPoint *advanceGeom;
    RgGeomPoint *geometry;
    RgVector position;
    RgVector velocity;

    advanceGeom = body->advanceGeometry;
    geometry = body->geometry;
    RgGeomPassTime(&advanceGeom->geom, deltaTime);
    __RgGeomPointGetPos(geometry, position, robotSourcePath, 3213);
    RgGeomPointSetPos(advanceGeom, position);
    RgGeomPointGetVel(geometry, velocity);
    XrgScaleVector(velocity, velocity, 0.5f);
    XrgAddVector(position, position, velocity);
    RgGeomPointMovePos(advanceGeom, position);
}

static void _BodyAutoHomingPassTime(RgBody *body, float deltaTime)
{
    RgRobotSpec *spec;
    RgGeomPoint *geometry;
    unsigned int i;
    unsigned int flags;
    unsigned int homingFlags;

    if (body == 0) {
        assert_prog(robotAssertBodyNotNil, robotSourcePath, 3230);
    }
    flags = body->flags;
    spec = body->spec;
    geometry = body->geometry;
    if ((flags & RG_BODY_FLAG_NO_AUTO_HOMING) != 0) {
        body->flags = flags & ~RG_BODY_FLAG_NO_AUTO_HOMING;
        return;
    }
    for (i = 0; i < RG_AUTO_HOMING_ENTRY_NUM; i++) {
        homingFlags = spec->m_aAutoHoming[i].flags;
        if (0.0f < spec->m_aAutoHoming[i].value) {
            if (homingFlags != 0 && (body->autoHomingEnv & homingFlags) == homingFlags) {
                RgRobSubHoming(geometry, body->target, spec->m_aAutoHoming[i].value,
                               deltaTime);
                body->flags |= RG_BODY_FLAG_FREE_TURN;
            }
        }
    }
}

static int _BodyPassTime(RgRobotStatus *robotStatus, RgBody *body,
                         float deltaTime)
{
    unsigned int i;
    float frameTime = deltaTime;
    float passTime;
    int flags;

    flags = 0;
    passTime = frameTime;
    if (body == 0) {
        assert_prog(robotAssertBodyNotNil, robotSourcePath, 0xCBD);
    }
    body->landingSoundTime += passTime;
    XrgSoundPassTime(body->landingSound, passTime);
    if (body->confuse > 0.0f) {
        body->confuse = body->confuse - passTime;
    }
    if (!(robotStatus->type == RG_STATUS_DASH_VR || robotStatus->type == RG_STATUS_DASH)) {
        body->rotate = RgGeomRobotGetRotate(&body->geometry->geom);
    }
    RgRobotEffectPassTime(body->effect, frameTime);
    if (body->weakHitMotion > 0.0f) {
        body->weakHitMotion = body->weakHitMotion - frameTime;
        flags = 0x10;
        passTime = frameTime * body->weakHitPower;
    }
    flags |= _StatusPassTime(robotStatus, body, passTime);
    for (i = 0; i < RG_EQUIP_TYPE_NUM; i++) {
        RgShotThreadPassTime(body->shotThread[i], passTime);
    }
    _PassTimeHomingThread(body->homingThread, passTime);
    if (_GetStatHomingThread(body->homingThread) == 1) {
        body->flags |= RG_BODY_FLAG_FREE_TURN;
    }
    _BodyAutoHomingPassTime(body, passTime);
    _BodyGeomPassTime(body, passTime);
    if (body->actor != 0) {
        XrgActorPassTime(body->actor, passTime);
    }
    for (i = 0; i < RG_EQUIP_TYPE_NUM; i++) {
        if (body->weapon[i] != 0) {
            RgWeaponPassTime(body->weapon[i], passTime);
            if (RgWeaponIsLockedOn(body->weapon[i])) {
                flags |= 2;
            }
        }
    }
    if (_EyeGeomPassTime(body, passTime) == 0) {
        if (0.1f < body->landingSoundTime) {
            flags |= 1;
        }
    }
    _AdvanceGeomPassTime(body, passTime);
    body->smoothSuppressed = 0;
    return flags;
}

static void _BodyDisp(RgRobotStatus *robotStatus, RgBody *body)
{
    RgMatrix localFrame;
    unsigned int eSide;

    if (body == 0) {
        assert_prog(robotAssertBodyNotNil, robotSourcePath, 0xD0E);
    }
    _StatusDisp(robotStatus, body);
    RgGeomRobotCalcLocal(&body->geometry->geom, localFrame);
    if (body->actor != 0) {
        XrgActorSetLocal(body->actor, localFrame);
        XrgActorDraw(body->actor);
    }
    for (eSide = 0; eSide < RG_EQUIP_TYPE_NUM; eSide++) {
        if (body->weapon[eSide] != 0) {
            RgWeaponDisp(body->weapon[eSide]);
        }
    }
    RgRobotEffectDisp(body->effect);
}

static void _InitBody(RgBody *body)
{
    unsigned int eType;

    InitRgRobotSpec(&s_inDefSpec_0);
    if (body == 0) {
        assert_prog(robotAssertBodyNotNil, robotSourcePath, 0xD37);
    }
    body->charID = -1;
    body->life = 100.0f;
    body->lifeMax = 100.0f;
    body->spec = &s_inDefSpec_0;
    body->grounded = 1;
    body->flags = 0;
    body->autoHomingEnv = 0;
    body->cmdMask = 0;
    body->prevCmdMask = 0;
    body->weakHitMotion = 0.0f;
    body->weakHitPower = 1.0f;
    body->weakHitAccum = 0.0f;
    body->weakHitWindow = 0.0f;
    body->shieldActive = 0;
    body->target = 0;
    body->actor = 0;
    body->geometry = (RgGeomPoint *) CreateRgGeomRobot();
    body->eyeGeometry = &CreateRgGeomBall(1.0f, 1.0f)->point;
    body->advanceGeometry = &CreateRgGeomBall(0.3f, 0.3f)->point;
    RgGeomPointSetMaxXYSpd(body->geometry, _CalcMaxSpeed(body, body->spec->moveSpeedMax));
    body->dashTime = body->spec->dashTime;
    for (eType = 0; eType < RG_EQUIP_TYPE_NUM; eType++) {
        body->weapon[eType] = 0;
        body->spareWeapon[eType] = 0;
        body->shotThread[eType] = CreateRgShotThread();
    }
    body->homingThread = _CreateHomingThread();
    body->effect = CreateRgRobotEffect();
    RgRobotEffectSetGeom(body->effect, body->geometry);
    do {
        body->smoothSuppressed = 1;
        body->landingSoundTime = 0.0f;
    } while (0);
    do {
        body->motSmooth = 1;
        body->landingSound = 0;
        body->confuse = 0.0f;
    } while (0);
}

static void _DestructBody(RgBody *body)
{
    unsigned int i;

    if (body == 0) {
        assert_prog(robotAssertBodyNotNil, robotSourcePath, 3454);
    }
    RgGeomFree(&body->geometry->geom);
    if (body->eyeGeometry != 0) {
        RgGeomFree(&body->eyeGeometry->geom);
    }
    if (body->advanceGeometry != 0) {
        RgGeomFree(&body->advanceGeometry->geom);
    }
    for (i = 0; i < RG_EQUIP_TYPE_NUM; i++) {
        if (body->weapon[i] != 0) {
            DisposeRgWeapon(body->weapon[i]);
        }
        if (body->spareWeapon[i] != 0) {
            DisposeRgWeapon(body->spareWeapon[i]);
        }
        DisposeRgShotThread(body->shotThread[i]);
    }
    _DisposeHomingThread(body->homingThread);
    DisposeRgRobotEffect(body->effect);
    if (body->actor != 0) {
        DisposeXrgActor(body->actor);
    }
    if (body->landingSound != 0) {
        DisposeXrgSound(body->landingSound);
    }
}

static RgBody *_CreateBody(void)
{
    RgBody *body;

    body = RgHeapAlloc(InstanceOfRgHeap(), sizeof(RgBody), robotSourcePath, 3482);
    _InitBody(body);
    return body;
}

/* ov12:0x00a51b58 contains the format string "already disposed body %p". */

static void _DisposeBody(RgBody *body)
{
    if (body == 0) {
        assert_prog(robotAssertBodyNotNil, robotSourcePath, 3490);
    }
    if (RgHeapIsInvalidMemory(InstanceOfRgHeap(), body) != 0) {
        RgError(robotAlreadyDisposedBodyFormat, robotSourcePath, 3492, body);
    }
    _DestructBody(body);
    RgHeapFree(InstanceOfRgHeap(), body, robotSourcePath, 3494);
}

static int _ExecAccelarateCmd(RgRobotStatus *status, RgBody *body, RgCmd *command)
{
    RgRobotSpec *spec;
    RgGeomPoint *geometry;

    spec = body->spec;
    geometry = body->geometry;
    RgGeomPointSetMaxXYSpd(geometry, _CalcMaxSpeed(body, spec->moveSpeedMax));
    RgRobSubAcceralate(geometry, command->vector, spec->enginePower);
    XrgSoundRingMoving(body->landingSound, 0, 0.2f);
    return 1;
}

/*
 * The queued command reuses its own +0x00 slot (RgCmd.param) as a plain
 * float rotate delta for this command type (see RgCmd's own doc on that
 * reuse); only its sign is used.
 */
static int _ExecRotateCmd(RgRobotStatus *status, RgBody *body, RgCmd *command)
{
    float delta;
    int rotateSign;

    delta = command->rotate;
    if (delta > 0.0f) {
        rotateSign = 1;
    } else if (delta < 0.0f) {
        rotateSign = -1;
    } else {
        rotateSign = 0;
    }
    if ((float) rotateSign == 0.0f) {
        return 0;
    }
    RgGeomRobotAddRotForce(&body->geometry->geom, (float) rotateSign * body->spec->rotateForce);
    body->flags |= RG_BODY_FLAG_NO_AUTO_HOMING;
    XrgSoundRingMoving(body->landingSound, 0, 0.2f);
    return 2;
}

static int _IsBackWeaponReady(RgBody *body, unsigned int eType)
{
    if (eType >= RG_EQUIP_TYPE_NUM) {
        assert_prog(robotAssertEquipTypeInRange, robotSourcePath, 0xDD7);
    }
    if (eType != RG_EQUIP_TYPE_BACK) {
        return 0;
    }
    if (body->weapon[RG_EQUIP_TYPE_BACK] == 0) {
        return 0;
    }
    if (RgWeaponGetShotNum(body->weapon[RG_EQUIP_TYPE_BACK]) <= 0.0f) {
        return 0;
    }
    return 1;
}

static int _ExecShotOrAttackCmd(RgRobotStatus *status, RgBody *body, RgCmd *command)
{
    RgShotMotStartInfo startParam;
    RgMotionShotInfo *motionInfo;
    RgMotionAction *action;
    RgWeapon *weapon;
    void *shotThread;
    int eSide;
    int controlFlag;
    int shotType;
    int motion;

    eSide = command->param;
    weapon = body->weapon[eSide];
    shotThread = body->shotThread[eSide];
    if (eSide == RG_EQUIP_TYPE_BACK && RgHissatuCheck(body->charID, body->weapon)) {
        _InitStatusHissatu(status, body);
        return 8;
    }
    if (weapon == 0) {
        return 0;
    }
    if (RgWeaponGetShotNum(weapon) <= 0.0f) {
        motion = _GetDropMotion(eSide);
        if (_InitDropStatus(status, body, motion, eSide) != 0) {
            return 0x200;
        }
        return 0;
    }
    if (RgWeaponIsNotBusy(weapon) == 0 && RgWeaponIsActiveLockOnSys(weapon) == 0) {
        return 0;
    }
    controlFlag = RgWeaponGetControlFlag(weapon);
    motion = RgWeaponRobotShotMotion(weapon, body->charID);
    shotType = controlFlag & RG_WEAPON_CONTROL_KIND_MASK;
    motionInfo = RgMotionInfoDBGet(InstanceOfRgMotionInfoDB(), motion, body->charID);
    if (eSide == RG_EQUIP_TYPE_BACK) {
        RgShotThreadSleep(body->shotThread[RG_EQUIP_TYPE_LEFT]);
        RgShotThreadSleep(body->shotThread[RG_EQUIP_TYPE_RIGHT]);
    }
    if (shotType == 0 && RgShotThreadGetStatus(shotThread) != RG_SHOT_THREAD_DISABLED) {
        action = RgMotionInfoGetActionInTime(motionInfo, 0, RG_ROBOT_TIME_UNLIMITED);
        if ((controlFlag & RG_WEAPON_CONTROL_LOCKON) != 0 || motion == -1) {
            startParam.motion = RG_SHOT_THREAD_NO_MOTION;
            startParam.frame = 0.18f;
            startParam.duration = 0.4f;
        } else {
            startParam.motion = motion;
            if (action == 0) {
                startParam.frame = 0.18f;
                startParam.duration = 0.4f;
            } else {
                startParam.frame = action->startTime;
                startParam.duration = action->endTime;
            }
        }
        RgShotThreadStart(shotThread, &startParam);
        RgWeaponPlayShotMotion(weapon);
        return 8;
    }
    if (shotType == 3) {
        _InitShieldStatus(status, body, motion, weapon, eSide);
        return 0x20;
    }
    _InitAttackStatus(status, body, (motion == -1) ? 0 : motion, weapon, eSide);
    return 0x10;
}

static int _ExecTargettingCmd(RgRobotStatus *status, RgBody *body, RgCmd *command, int targettingMode)
{
    (void)command;
    _InitTargettingStatus(status, body, targettingMode);
    return RG_CMD_RESULT_TARGETTING;
}

static int _ExecBreakCmd(RgRobotStatus *status, RgBody *body, void *command)
{
    float brake_scale = 0.3f;

    (void)command;
    RgRobSubBreak(body->geometry, brake_scale, brake_scale);
    _InitBreakingStatus(status, body);
    return 0x40;
}

static int _ExecDashCmd(RgRobotStatus *status, RgBody *body, RgCmd *command)
{
    int result;
    unsigned int type;

    result = 0;
    type = body->spec->type;
    if (type != 1) {
        if (type == 0) {
            goto normal;
        }
        if (type >= 5) {
            goto done;
        }
normal:
        _InitDashStatus(status, body, command->vector);
        result = 4;
    } else {
        _InitDashVRStatus(status, body, command->vector);
        result = 4;
    }
done:
    return result;
}

static int _ExecDashContinueCmd(RgRobotStatus *status, RgBody *body, RgCmd *command)
{
    int result;

    result = 0;
    if (body->spec->type == RG_DASH_MODE_VR) {
        /* The original call passes two extra, ignored arguments. */
        ((int (*)()) _InitDashVRStatus)(status, body, command->vector, 1, 0);
        result = 4;
    }
    return result;
}

/* The format string "unknown equip type (drop weapon) %d". */

static int _GetDropMotion(int eSide)
{
    int motion;

    motion = RG_MOT_STAND;
    switch (eSide) {
    case RG_EQUIP_TYPE_BACK:
        break;
    case RG_EQUIP_TYPE_LEFT:
        motion = RG_MOT_DROP_LEFT;
        break;
    case RG_EQUIP_TYPE_RIGHT:
        motion = RG_MOT_DROP_RIGHT;
        break;
    default:
        RgError(robotUnknownDropWeaponEquipTypeFormat, robotSourcePath, 3752, eSide);
        break;
    }
    return motion;
}

static int _ExecDropWeaponCmd(RgRobotStatus *status, RgBody *body,
                               RgCmd *command)
{
    return (_InitDropStatus(status, body, _GetDropMotion(command->param),
                             command->param) == 0)
               ? 0
               : 0x200;
}

static int _ExecDamageCmd(RgRobotStatus *status, RgBody *body, RgCmd *command)
{
    float life;

    life = body->life - command->scratch0;
    body->life = life;
    if (life < 0.0f) {
        body->life = 0.0f;
    }
    _InitDamageStatus(status, body, command, command->scratch0,
                      command->scratch1, 0.0f);
    return 0x80;
}

static int _ExecHitByBody(RgRobotStatus *status, RgBody *body, RgCmd *command)
{
    RgVector force;

    CreateRgHitEffectPosDir(&command->scratch0, command->vector,
                            robotHitEffectName, 0);
    XrgNormalizeVector(force, command->vector);
    XrgScaleVector(force, force, command->scratch5);
    RgGeomPointAddForce(body->geometry, force);
    body->life -= command->scratch4;
    if (body->life < 0.0f) {
        body->life = 0.0f;
    }
    _InitDamageStatus(status, body, command, 0.0f, 1, 0.5f);
    return 0x400;
}

static int _ExecWeakDamageCmd(RgRobotStatus *status, RgBody *body, RgCmd *command)
{
    float windowSeed;
    float accum;

    body->life -= command->scratch0;
    if (body->life < 0.0f) {
        body->life = 0.0f;
    }
    if (body->weakHitWindow <= 0.0f) {
        windowSeed = command->scratch2;
        body->weakHitAccum = 0.0f;
        body->weakHitWindow = windowSeed;
    }
    accum = body->weakHitAccum + command->scratch0;
    body->weakHitAccum = accum;
    if (*(float *) &command->scratch1 <= accum) {
        _InitDamageStatus(status, body, command, accum, 1, 0.0f);
        body->weakHitWindow = 0.0f;
        body->weakHitAccum = 0.0f;
    } else {
        body->weakHitMotion = command->scratch3;
        body->weakHitPower = command->scratch4;
    }
    return 0x80;
}

static void _DullFlagsPassTime(RgStatus *pRobot, float deltaTime)
{
    RgVector targetPos;
    unsigned int flags;
    unsigned int mask;

    flags = pRobot->statusFlags;
    mask = pRobot->acceptedCommandMask;
    if ((flags & 1) != 0) {
        RgDullFlagSetFlag(&pRobot->dullFlag0);
    }
    if ((flags & 2) != 0) {
        RgDullFlagSetFlag(&pRobot->dullFlag1);
    }
    if ((mask & 8) != 0) {
        RgDullFlagSetFlag(&pRobot->dullFlag2);
    }
    RgDullFlagPassTime(&pRobot->dullFlag0, deltaTime);
    RgDullFlagPassTime(&pRobot->dullFlag1, deltaTime);
    RgDullFlagPassTime(&pRobot->dullFlag2, deltaTime);

    flags = 0;
    if (RgDullFlagGet(&pRobot->dullFlag2)) {
        flags = 4;
    }
    if (RgDullFlagGet(&pRobot->dullFlag0)) {
        flags |= 1;
    }
    if (RgDullFlagGet(&pRobot->dullFlag1)) {
        flags |= 2;
    }
    if (_BodyGetTargetPos(pRobot->body, targetPos) != 0) {
        if (pRobot->drawView != 0
            && RgDrawViewIsPointInView(pRobot->drawView, targetPos, 0.0f)) {
            flags |= 8;
        }
    }
    _BodySetAutoHomingEnv(pRobot->body, flags);
}

static void _PassTimeRobot(RgStatus *pRobot, float deltaTime)
{
    RgBody *body;
    RgCmdQueue *cmdQueue;
    unsigned int savedCmdMask;
    unsigned int i;

    cmdQueue = pRobot->cmdQueue;
    body = pRobot->body;
    if (!RgRobotIsDead(pRobot)) {
        pRobot->acceptedCommandMask = 0;
        savedCmdMask = body->cmdMask;
        body->cmdMask = 0;
        body->prevCmdMask = savedCmdMask;
        for (i = 0; i < cmdQueue->count; i++) {
            pRobot->acceptedCommandMask |=
                _BodyExecCmd(pRobot->robotStatus, body, &cmdQueue->cmd[i]);
        }
    }
    _ClearCmdQueue(cmdQueue);
    pRobot->statusFlags =
        _BodyPassTime(pRobot->robotStatus, body, deltaTime);
    _DullFlagsPassTime(pRobot, deltaTime);
}

static void _DispRobot(RgStatus *pRobot)
{
    if (pRobot == 0) {
        assert_prog(robotAssertRobotNotNil, robotSourcePath, 3976);
    }
    _BodyDisp(pRobot->robotStatus, pRobot->body);
}

/* ov12:0x00a51bc0 contains the format string "already disposed robot %p\n". */

static void _DestructRobot(RgStatus *pRobot)
{
    if (pRobot == 0) {
        assert_prog(robotAssertRobotNotNil, robotSourcePath, 3985);
    }
    if (RgHeapIsInvalidMemory(InstanceOfRgHeap(), pRobot) != 0) {
        RgError(robotAlreadyDisposedRobotFormat, robotSourcePath, 3987, pRobot);
    }
    _DisposeBody(pRobot->body);
    _DisposeStatus(pRobot->robotStatus);
    _DisposeCmdQueue(pRobot->cmdQueue);
}

static void _InitRobot(RgStatus *pRobot)
{
    RgRobotStatus *status;
    RgDullFlag *dullFlag0 = &pRobot->dullFlag0;
    RgDullFlag *dullFlag1 = &pRobot->dullFlag1;
    RgDullFlag *dullFlag2 = &pRobot->dullFlag2;

    if (pRobot == 0) {
        assert_prog(robotAssertRobotNotNil, robotSourcePath, 4002);
    }
    pRobot->body = _CreateBody();
    status = _CreateStatue();
    pRobot->robotStatus = status;
    _InitMovingStatus(status, pRobot->body);
    pRobot->cmdQueue = _CreateCmdQueue();
    pRobot->acceptedCommandMask = 0;
    pRobot->statusFlags = 0;
    InitRgDullFlag(dullFlag0);
    InitRgDullFlag(dullFlag1);
    InitRgDullFlag(dullFlag2);
    RgDullFlagSetDullTime(dullFlag0, 0.5f);
    RgDullFlagSetDullTime(dullFlag1, 0.5f);
    RgDullFlagSetDullTime(dullFlag2, 0.4f);
    pRobot->drawView = 0;
    pRobot->passTimePhase = 0;
    pRobot->passTimeTimer = 0.0f;
}

RgStatus *CreateRgRobot(void)
{
    RgStatus *pRobot;

    pRobot = RgHeapAlloc(InstanceOfRgHeap(), sizeof(RgStatus), robotSourcePath,
                          4041);
    _InitRobot(pRobot);
    return pRobot;
}

void DisposeRgRobot(RgStatus *pRobot)
{
    if (pRobot == 0) {
        assert_prog(robotAssertRobotNotNil, robotSourcePath, 4049);
    }
    _DestructRobot(pRobot);
    RgHeapFree(InstanceOfRgHeap(), pRobot, robotSourcePath, 4051);
}

void RgRobotPassTime(RgStatus *pRobot, float deltaTime)
{
    RgVector position;
    RgRobotStatus *status;

    if (pRobot == 0) {
        assert_prog(robotAssertRobotNotNil, robotSourcePath, 0xFDC);
    }
    _PassTimeRobot(pRobot, deltaTime);
    if (pRobot->body->life <= 0.0f) {
        status = pRobot->robotStatus;
        if (status->type != RG_STATUS_MOVING) {
            _InitMovingStatus(status, pRobot->body);
        }
    }
    pRobot->passTimeTimer = pRobot->passTimeTimer - deltaTime;
    if (pRobot->passTimePhase != 1) {
        if (pRobot->passTimePhase == 0 && pRobot->body->life <= 0.0f) {
            pRobot->passTimePhase = 1;
            pRobot->passTimeTimer = 5.0f;
        }
    } else {
        if (pRobot->passTimeTimer < 0.0f) {
            pRobot->passTimePhase = 2;
        } else {
            if (((int) (pRobot->passTimeTimer * 15.0f) & 1) == 0) {
                __RgGeomPointGetPos(pRobot->body->geometry, position,
                                    robotSourcePath, 0xFF7);
                position[0] = position[0] + XrgRand(-1.7f, 1.7f);
                position[1] = position[1] + XrgRand(-1.0f, 3.5f);
                position[2] = position[2] + XrgRand(-1.7f, 1.7f);
                CreateRgHitEffectPos(position, robotHitEffectName, robotHitEffectName);
            }
            if (((int) (pRobot->passTimeTimer * 20.0f) & 1) == 0) {
                XrgSoundRing(pRobot->body->landingSound, 21);
            }
        }
    }
}

void RgRobotDisp(RgStatus *pRobot)
{
    if (pRobot == 0) {
        assert_prog(robotAssertRobotNotNil, robotSourcePath, 4109);
    }
    _DispRobot(pRobot);
}

void RgRobotSetSpec(RgStatus *pRobot, RgRobotSpec *spec)
{
    if (pRobot == 0) {
        assert_prog(robotAssertRobotNotNil, robotSourcePath, 4124);
    }
    _BodySetSpec(pRobot->body, spec);
}

void RgRobotSetGeom(RgStatus *pRobot, RgGeomPoint *geometry)
{
    if (pRobot == 0) {
        assert_prog(robotAssertRobotNotNil, robotSourcePath, 4133);
    }
    _BodySetGeom(pRobot->body, geometry);
}

void RgRobotSetEyeGeom(RgStatus *pRobot, RgGeomPoint *eyeGeometry)
{
    if (pRobot == 0) {
        assert_prog(robotAssertRobotNotNil, robotSourcePath, 4142);
    }
    _BodySetEyeGeom(pRobot->body, eyeGeometry);
}

void RgRobotSetAdvanceGeom(RgStatus *pRobot, RgGeomPoint *advanceGeometry)
{
    if (pRobot == 0) {
        assert_prog(robotAssertRobotNotNil, robotSourcePath, 4152);
    }
    _BodySetAdvanceGeom(pRobot->body, advanceGeometry);
}

void RgRobotSetSoundDriver(RgStatus *pRobot, XrgSound *sound)
{
    if (pRobot == 0) {
        assert_prog(robotAssertRobotNotNil, robotSourcePath, 4161);
    }
    _BodySetSound(pRobot->body, sound);
}

void RgRobotSetActor(RgStatus *pRobot, XrgActorEssence *actor)
{
    if (pRobot == 0) {
        assert_prog(robotAssertRobotNotNil, robotSourcePath, 4169);
    }
    _BodySetActor(pRobot->body, actor);
}

void RgRobotSetTarget(RgStatus *pRobot, RgGeomPoint *target)
{
    if (pRobot == 0) {
        assert_prog(robotAssertRobotNotNil, robotSourcePath, 4177);
    }
    _BodySetTarget(pRobot->body, target);
}

void RgRobotSetRgDrawView(RgStatus *pRobot, RgDrawView *view)
{
    if (pRobot == 0) {
        assert_prog(robotAssertRobotNotNil, robotSourcePath, 4187);
    }
    pRobot->drawView = view;
}

void RgRobotSetMotSmooth(RgStatus *pRobot, int motSmooth)
{
    if (pRobot == 0) {
        assert_prog(robotAssertRobotNotNil, robotSourcePath, 4195);
    }
    pRobot->body->motSmooth = motSmooth;
}

void RgRobotSetConfuse(RgStatus *pRobot, float confuse)
{
    if (pRobot == 0) {
        assert_prog(robotAssertRobotNotNil, robotSourcePath, 4203);
    }
    pRobot->body->confuse = confuse;
}

void RgRobotSetWeapon(RgStatus *pRobot, int eSide, RgWeapon *weapon)
{
    if (pRobot == 0) {
        assert_prog(robotAssertRobotNotNil, robotSourcePath, 4217);
    }
    _BodyEquipWeapon(pRobot->body, eSide, weapon);
}

void RgRobotSetSpareWeapon(RgStatus *pRobot, int eSide, RgWeapon *weapon)
{
    if (pRobot == 0) {
        assert_prog(robotAssertRobotNotNil, robotSourcePath, 4227);
    }
    _BodySetSpareWeapon(pRobot->body, eSide, weapon);
}

RgGeomPoint *RgRobotGetGeom(RgStatus *pRobot)
{
    if (pRobot == 0) {
        assert_prog(robotAssertRobotNotNil, robotSourcePath, 4239);
    }
    return pRobot->body->geometry;
}

RgWeapon *RgRobotGetWeapon(RgStatus *pRobot, unsigned int eSide)
{
    if (pRobot == 0) {
        assert_prog(robotAssertRobotNotNil, robotSourcePath, 4247);
    }
    if (eSide >= RG_EQUIP_TYPE_NUM) {
        assert_prog(robotAssertEquipSideInRange, robotSourcePath, 4248);
    }
    return pRobot->body->weapon[eSide];
}

RgGeomPoint *RgRobotGetTarget(RgStatus *pRobot)
{
    if (pRobot == 0) {
        assert_prog(robotAssertRobotNotNil, robotSourcePath, 4256);
    }
    return pRobot->body->target;
}

int RgRobotIsDead(RgStatus *pRobot)
{
    if (pRobot == 0) {
        assert_prog(robotAssertRobotNotNil, robotSourcePath, 4264);
    }
    return pRobot->body->life <= 0.0f;
}

float RgRobotGetLife(RgStatus *pRobot)
{
    if (pRobot == 0) {
        assert_prog(robotAssertRobotNotNil, robotSourcePath, 4272);
    }
    return pRobot->body->life;
}

float RgRobotGetLifeMax(RgStatus *pRobot)
{
    if (pRobot == 0) {
        assert_prog(robotAssertRobotNotNil, robotSourcePath, 4278);
    }
    return pRobot->body->lifeMax;
}

XrgActor *RgRobotGetActor(RgStatus *pRobot)
{
    if (pRobot == 0) {
        assert_prog(robotAssertRobotNotNil, robotSourcePath, 4286);
    }
    return pRobot->body->actor;
}

int RgRobotGetCharID(RgStatus *pRobot)
{
    if (pRobot == 0) {
        assert_prog(robotAssertRobotNotNil, robotSourcePath, 4294);
    }
    return pRobot->body->charID;
}

int RgRobotIsInvalidAttack(RgStatus *pRobot, RgVector direction)
{
    float *shield;

    if (pRobot == 0) {
        assert_prog(robotAssertRobotNotNil, robotSourcePath, 0x10CF);
    }
    if (pRobot->body->shieldActive != 0) {
        shield = pRobot->body->shieldDirection;
        /* !(x < 0) keeps the original's c.lt.s/bc1f; x >= 0 compiles to c.le.s. */
        if (!(direction[0] * shield[0] + direction[1] * shield[1] + direction[2] * shield[2] < 0.0f)) {
            return 1;
        }
    }
    return 0;
}

float RgRobotGetDashTime(RgStatus *pRobot)
{
    if (pRobot == 0) {
        assert_prog(robotAssertRobotNotNil, robotSourcePath, 4321);
    }
    return pRobot->body->dashTime;
}

RgRobotSpec *RgRobotGetSpec(RgStatus *pRobot)
{
    if (pRobot == 0) {
        assert_prog(robotAssertRobotNotNil, robotSourcePath, 4329);
    }
    return pRobot->body->spec;
}

int RgRobotIsConfused(RgStatus *pRobot)
{
    if (pRobot == 0) {
        assert_prog(robotAssertRobotNotNil, robotSourcePath, 4337);
    }
    if (pRobot->body->confuse > 0.0f)
        return 1;
    return 0;
}

int RgRobotIsAcceptedCommand(RgStatus *pRobot, int command)
{
    if (pRobot == 0) {
        assert_prog(robotAssertRobotNotNil, robotSourcePath, 4348);
    }
    return (pRobot->acceptedCommandMask & (1 << command)) != 0;
}

unsigned int RgRobotGetStatusFlags(RgStatus *pRobot)
{
    if (pRobot == 0) {
        assert_prog(robotAssertRobotNotNil, robotSourcePath, 4356);
    }
    return pRobot->statusFlags;
}

void RgRobotAccelarate(RgStatus *pRobot, RgVector velocity)
{
    RgCmd *command;

    if (pRobot == 0) {
        assert_prog(robotAssertRobotNotNil, robotSourcePath, 4370);
    }
    command = _EntryCmdQueue(pRobot->cmdQueue, RG_CMD_ACCELARATE);
    if (command != 0) {
        XrgCopyVector(command->vector, velocity);
    }
}

void RgRobotAccelarateRotate(RgStatus *pRobot, float rotate)
{
    RgCmd *command;

    if (pRobot == 0) {
        assert_prog(robotAssertRobotNotNil, robotSourcePath, 4382);
    }
    command = _EntryCmdQueue(pRobot->cmdQueue, RG_CMD_ACCELARATE_ROTATE);
    if (command != 0) {
        command->rotate = rotate;
    }
}

/*
 * ov12:0x00a51a88 contains the range expression
 * "RG_EQUIP_TYPE_MIN <= (eType) && (eType) < RG_EQUIP_TYPE_NUM".
 */

void RgRobotShot(RgStatus *pRobot, unsigned int eType)
{
    RgCmd *command;

    if (pRobot == 0) {
        assert_prog(robotAssertRobotNotNil, robotSourcePath, 4394);
    }
    if (eType >= RG_EQUIP_TYPE_NUM) {
        assert_prog(robotAssertEquipTypeInRange, robotSourcePath, 4395);
    }
    command = _EntryCmdQueue(pRobot->cmdQueue, RG_CMD_SHOT);
    if (command != 0) {
        command->param = eType;
    }
}

void RgRobotTargetting(RgStatus *pRobot)
{
    RgCmd *command;

    if (pRobot == 0) {
        assert_prog(robotAssertRobotNotNil, robotSourcePath, 4407);
    }
    command = _EntryCmdQueue(pRobot->cmdQueue, RG_CMD_TARGETTING);
    if (command != 0) {
        command->param = (int) pRobot->body->target;
    }
}

void RgRobotBreak(RgStatus *pRobot)
{
    if (pRobot == 0) {
        assert_prog(robotAssertRobotNotNil, robotSourcePath, 4419);
    }
    _EntryCmdQueue(pRobot->cmdQueue, RG_CMD_BREAK);
}

void RgRobotDash(RgStatus *pRobot, RgVector direction)
{
    RgCmd *command;

    if (pRobot == 0) {
        assert_prog(robotAssertRobotNotNil, robotSourcePath, 4428);
    }
    command = _EntryCmdQueue(pRobot->cmdQueue, RG_CMD_DASH);
    if (command != 0) {
        XrgCopyVector(command->vector, direction);
    }
}

void RgRobotDashContinue(RgStatus *pRobot)
{
    if (pRobot == 0) {
        assert_prog(robotAssertRobotNotNil, robotSourcePath, 4440);
    }
    _EntryCmdQueue(pRobot->cmdQueue, RG_CMD_DASH_CONTINUE);
}

void RgRobotGiveDamage(RgStatus *pRobot, RgVector direction, int damageType,
                       float damage)
{
    RgCmd *command;

    if (pRobot == 0) {
        assert_prog(robotAssertRobotNotNil, robotSourcePath, 4449);
    }
    command = _EntryCmdQueue(pRobot->cmdQueue, RG_CMD_DAMAGE);
    if (command != 0) {
        command->scratch0 = damage;
        command->scratch1 = damageType;
        XrgCopyVector(command->vector, direction);
    }
}

void RgRobotGiveWeakDamage(RgStatus *pRobot, RgVector direction, float damage,
                           float threshold, float windowSeed,
                           float weakHitMotion, float weakHitPower)
{
    RgCmd *command;

    if (pRobot == 0) {
        assert_prog(robotAssertRobotNotNil, robotSourcePath, 0x117A);
    }
    command = _EntryCmdQueue(pRobot->cmdQueue, RG_CMD_WEAK_DAMAGE);
    if (command != 0) {
        command->scratch0 = damage;
        XrgCopyVector(command->vector, direction);
        *(float *) &command->scratch1 = threshold;
        command->scratch2 = windowSeed;
        command->scratch3 = weakHitMotion;
        command->scratch4 = weakHitPower;
    }
}

void RgRobotHitByBody(RgStatus *pRobot, RgVector direction, RgVector position,
                      float damage, float scale)
{
    RgCmd *command;

    if (pRobot == 0) {
        assert_prog(robotAssertRobotNotNil, robotSourcePath, 0x118C);
    }
    command = _EntryCmdQueue(pRobot->cmdQueue, RG_CMD_HIT_BY_BODY);
    if (command != 0) {
        XrgCopyVector(&command->scratch0, direction);
        XrgCopyVector(command->vector, position);
        command->scratch4 = damage;
        command->scratch5 = scale;
    }
}

void RgRobotDropWeapon(RgStatus *pRobot, unsigned int eType)
{
    RgCmd *command;

    if (pRobot == 0) {
        assert_prog(robotAssertRobotNotNil, robotSourcePath, 4508);
    }
    if (eType >= RG_EQUIP_TYPE_NUM) {
        assert_prog(robotAssertEquipTypeInRange, robotSourcePath, 4509);
    }
    command = _EntryCmdQueue(pRobot->cmdQueue, RG_CMD_DROP_WEAPON);
    if (command != 0) {
        command->param = eType;
    }
}

void RgRobotHitWeaponAttack(RgStatus *pRobot, int param)
{
    RgCmd *command;

    if (pRobot == 0) {
        assert_prog(robotAssertRobotNotNil, robotSourcePath, 4522);
    }
    command = _EntryCmdQueue(pRobot->cmdQueue, RG_CMD_HIT_WEAPON_ATTACK);
    if (command != 0) {
        command->param = param;
    }
}

void RgRobotInvalidAttack(RgStatus *pRobot, RgVector direction, float scale)
{
    RgCmd *command;

    if (pRobot == 0) {
        assert_prog(robotAssertRobotNotNil, robotSourcePath, 4536);
    }
    command = _EntryCmdQueue(pRobot->cmdQueue, RG_CMD_INVALID_ATTACK);
    if (command != 0) {
        XrgNormalizeVector(command->vector, direction);
        command->scratch0 = scale;
    }
}

void RgRobotHitBG(RgStatus *pRobot, RgVector direction)
{
    RgCmd *command;

    if (pRobot == 0) {
        assert_prog(robotAssertRobotNotNil, robotSourcePath, 4550);
    }
    command = _EntryCmdQueue(pRobot->cmdQueue, RG_CMD_HIT_BG);
    if (command != 0) {
        XrgCopyVector(command->vector, direction);
    }
    if (!(direction[1] > 0.9f)) {
        return;
    }
    _BodyOnGround(pRobot->body);
}

void RgRobotNearBG(RgStatus *pRobot, RgVector direction)
{
    RgCmd *command;

    if (pRobot == 0) {
        assert_prog(robotAssertRobotNotNil, robotSourcePath, 4568);
    }
    command = _EntryCmdQueue(pRobot->cmdQueue, RG_CMD_NEAR_BG);
    if (command != 0) {
        XrgCopyVector(command->vector, direction);
    }
}

extern const char robotCommandNameAccelarate[];
extern const char robotCommandNameAccelarateRotate[];
extern const char robotCommandNameShot[];
extern const char robotCommandNameTargetting[];
extern const char robotCommandNameBreak[];
extern const char robotCommandNameDash[];
extern const char robotCommandNameDashContinue[];
extern const char robotCommandNameDropWeapon[];
extern const char robotCommandNameDamage[];
extern const char robotCommandNameWeakDamage[];
extern const char robotCommandNameHitWeaponAttack[];
extern const char robotCommandNameInvalidAttack[];
extern const char robotCommandNameHitBg[];
extern const char robotCommandNameNearBg[];
extern const char robotCommandNameHitByBody[];

static const char *s_aszCommandName[15] = {
    robotCommandNameAccelarate,
    robotCommandNameAccelarateRotate,
    robotCommandNameShot,
    robotCommandNameTargetting,
    robotCommandNameBreak,
    robotCommandNameDash,
    robotCommandNameDashContinue,
    robotCommandNameDropWeapon,
    robotCommandNameDamage,
    robotCommandNameWeakDamage,
    robotCommandNameHitWeaponAttack,
    robotCommandNameInvalidAttack,
    robotCommandNameHitBg,
    robotCommandNameNearBg,
    robotCommandNameHitByBody
};

static int s_anCmdLevel[15] = { 1, 1, 1, 1, 1, 1, 1, 2, 3, 3, 2, 1, 1, 1, 2 };

const char robotCommandNameHitByBody[8] = "H_BB";

const char robotCommandNameNearBg[8] = "B_NB";

const char robotCommandNameHitBg[8] = "B_HB";

const char robotCommandNameInvalidAttack[8] = "INVA";

const char robotCommandNameHitWeaponAttack[8] = "HITW";

const char robotCommandNameWeakDamage[8] = "WEAK";

const char robotCommandNameDamage[8] = "DMG ";

const char robotCommandNameDropWeapon[8] = "DROP";

const char robotCommandNameDashContinue[8] = "DSHC";

const char robotCommandNameDash[8] = "DASH";

const char robotCommandNameBreak[8] = "BRK ";

const char robotCommandNameTargetting[8] = "TGT ";

const char robotCommandNameShot[8] = "ATK ";

const char robotCommandNameAccelarateRotate[8] = "ROT ";

const char robotCommandNameAccelarate[8] = "ACC ";

const char robotUnknownShieldStatusFormat[40] = "unknown shield status %d";

const char robotUnknownDashStatusFormat[24] = "unknown dash status %d";

const char robotUnknownDashModeFormat[32] = "unknown dash mode %d";

const char robotUnknownVrDashCmdModeFormat[32] = "unknown VRDASH cmd mode %d";

const char robotUnknownVrDashModeFormat[24] = "unknown VR dash mode %d";

const char robotCannotTransitVrDashModeFormat[40] = "cannot transit to VR dash mode %d";

const char robotEmptyEquipDatabaseName[8] = "";

const char robotAssertEquipPosValid[40] = "eEquipPos != RG_EQUIP_POS_INVALID";

const char robotAssertJointIdValid[32] = "eJntID != RG_ACTOR_JNT_NUL";

const char robotCheckPakaPakaLog[16] = "check pakapaka\n";

const char robotCheckPakaPakaEndLog[24] = "check pakapaka end\n";

const char robotNoLockOnLog[8] = "\?\?\?\n";

static RgRobotSpec s_inDefSpec_0;
