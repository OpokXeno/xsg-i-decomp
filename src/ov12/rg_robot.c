/*
 * OV12 original TU 4: 0x00a02510..0x00a09480 (153 functions)
 */
#include "common.h"
#include "shared.h"
#include "rg_robot.h"

const char D_00A51830[] = "pThread != NIL";

const char D_00A51840[] = "../rg_robot.euc.c";

const char D_00A51858[] = "fBaseSpeed > RG_FCONST(0.0)";

const char D_00A518F0[] = "pStatus != NIL";

const char D_00A51A58[] = "pQ != NIL";

const char D_00A51A68[] = "pBody != NIL";

const char D_00A51A78[] = "pGeom != NIL";

const char D_00A51A88[] = "RG_EQUIP_TYPE_MIN <= (eType) && (eType) < RG_EQUIP_TYPE_NUM";

const char D_00A51B48[] = "pCmd != NIL";

const char D_00A51B58[] = "already disposed body %p";

const char D_00A51B78[] = "unknown equip type (drop weapon) %d";

const char D_00A51BA0[] = "eff06.ptcl";

const char D_00A51BB0[] = "pRobot != NIL";

const char D_00A51BC0[] = "already disposed robot %p\n";

const char D_00A51BE0[] = "RG_EQUIP_TYPE_MIN <= (eSide) && (eSide) < RG_EQUIP_TYPE_NUM";

/* RgShotThread is defined in tu011 and stays opaque at this call site. */
struct RgShotThread;
extern void RgShotThreadSleep(struct RgShotThread *thread);
/*
 * These are external file-backed witnesses in the TU's still asm-owned
 * .rodata (config/tu/ov12/tu004.json data_ownership), not candidate-emitted
 * data. They keep their splat names per docs/naming.md ("Scaffold-owned data
 * keeps its splat name") until that data becomes C-owned.
 *
 * ov12:0x00a51840 (D_00A51840), 18 bytes, SHA-256
 * b8693e915c6e9d84b2f020600f866bdc3ba67025729e8a7566ebdf621f4380ca
 * contains the source filename "../rg_robot.euc.c".
 * ov12:0x00a51bb0 (D_00A51BB0), 14 bytes, SHA-256
 * d7e89b8cbef744999f0635dbf5f1111a85f0daeb1ad5c69e912cc14f4c1096a9
 * contains the assertion expression "pRobot != NIL".
 * ov12:0x00a51be0 (D_00A51BE0), 60 bytes, SHA-256
 * 1eafc9773d62673cff56b70f703e51d2bc76ff912cea41f18140919845228eb2
 * contains the range expression
 * "RG_EQUIP_TYPE_MIN <= (eSide) && (eSide) < RG_EQUIP_TYPE_NUM".
 */
extern void assert_prog(const char *expression, const char *source_file,
                        int line);
extern const char D_00A51840[];
extern const char D_00A51BB0[];
extern const char D_00A51BE0[];
/*
 * ov12:0x00a51830 (D_00A51830) contains the assertion expression
 * "pThread != NIL".
 * ov12:0x00a518f0 (D_00A518F0) contains the assertion expression
 * "pStatus != NIL".
 * ov12:0x00a51858 (D_00A51858) contains the assertion expression
 * "fBaseSpeed > RG_FCONST(0.0)".
 */
extern const char D_00A51830[];
extern const char D_00A518F0[];
extern const char D_00A51858[];
extern RgHeap *InstanceOfRgHeap(void);
extern void *RgHeapAlloc(void *heap, unsigned int size,
                         const char *source_file, int line);
extern void RgHeapFree(void *heap, void *pointer, const char *source_file,
                       int line);
extern void XrgSoundRing(int soundID, int count);
extern float RgGeomPointGetWeight(RgGeomPoint *point);

static void _InitHomingThread(RgHomingThread *thread)
{
    if (thread == 0) {
        assert_prog(D_00A51830, D_00A51840, 73);
    }
    thread->geometry = 0;
    thread->target = 0;
    thread->actionTime = 0;
    thread->sleepTime = 0;
    thread->homingSpeed = 4.0f * 3.1415927f;
    thread->state = 0;
}

static RgHomingThread *_CreateHomingThread(void)
{
    RgHomingThread *thread;

    thread = RgHeapAlloc(InstanceOfRgHeap(), sizeof(RgHomingThread),
                          D_00A51840, 85);
    _InitHomingThread(thread);
    return thread;
}

static void _DisposeHomingThread(RgHomingThread *thread)
{
    if (thread == 0) {
        assert_prog(D_00A51830, D_00A51840, 93);
    }
    RgHeapFree(InstanceOfRgHeap(), thread, D_00A51840, 94);
}

static void _StartHomingThread(RgHomingThread *thread, RgGeomPoint *geometry,
                               void *target, float actionTime)
{
    if (thread == 0) {
        assert_prog(D_00A51830, D_00A51840, 100);
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
        assert_prog(D_00A51830, D_00A51840, 114);
    }
    if (thread->state == 1) {
        thread->sleepTime = 0;
        thread->state = 2;
    }
}

static int _GetStatHomingThread(RgHomingThread *thread)
{
    if (thread == 0) {
        assert_prog(D_00A51830, D_00A51840, 124);
    }
    return thread->state;
}

static float _GetSleepTimeHomingThread(RgHomingThread *thread)
{
    float sleepTime;

    if (thread == 0) {
        assert_prog(D_00A51830, D_00A51840, 131);
    }
    sleepTime = 99999.9f;
    if (thread->state == 2) {
        sleepTime = thread->sleepTime;
    }
    return sleepTime;
}

extern float RgRobSubHoming(RgGeomPoint *geometry, RgGeomPoint *targetGeometry,
                            float speed, float deltaTime);

static void _PassTimeHomingThread(RgHomingThread *thread, float deltaTime)
{
    float actionTime;

    if (thread == 0) {
        assert_prog(D_00A51830, D_00A51840, 141);
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
        assert_prog(D_00A51858, D_00A51840, 247);
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
        XrgSoundRing(body->landingSoundID, 0x32);
    }
    body->grounded = 1;
}

static void _InitStatus(RgRobotStatus *status)
{
    if (status == 0) {
        assert_prog(D_00A518F0, D_00A51840, 611);
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

    status = RgHeapAlloc(InstanceOfRgHeap(), 0x60U, D_00A51840, 624);
    _InitStatus(status);
    return status;
}

static void _SetStatus(RgRobotStatus *status, int type, RgBody *body)
{
    RgStatusExitFunc exitMethod;

    if (status == 0) {
        assert_prog(D_00A518F0, D_00A51840, 633);
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
        assert_prog(D_00A518F0, D_00A51840, 647);
    }
    RgHeapFree(InstanceOfRgHeap(), status, D_00A51840, 648);
}

static int _StatusExecCmd(RgRobotStatus *status, RgCmd *command, RgBody *body)
{
    RgStatusExecCmdFunc execCmdMethod;

    if (status == 0) {
        assert_prog(D_00A518F0, D_00A51840, 657);
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
        assert_prog(D_00A518F0, D_00A51840, 668);
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
        assert_prog(D_00A518F0, D_00A51840, 678);
    }
    dispMethod = status->dispMethod;
    if (dispMethod != 0) {
        dispMethod(status, body);
    }
}

static int _ExecAccelarateCmd(RgStatus *status, RgBody *body, RgCmd *command);
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

static int _AllowAllCmdInMoving(RgRobotStatus *status, RgBody *body, RgCmd *command)
{
    int result;

    result = 0;
    switch ((unsigned int) command->type) {
    case 0:
        result = _ExecAccelarateCmd((RgStatus *) status, body, command);
        break;
    case 1:
        result = _ExecRotateCmd(status, body, command);
        break;
    case 2:
        result = _ExecShotOrAttackCmd(status, body, command);
        break;
    case 3:
        result = _ExecTargettingCmd(status, body, command, 1);
        break;
    case 4:
        result = _ExecBreakCmd(status, body, command);
        break;
    case 5:
        result = _ExecDashCmd(status, body, command);
        break;
    case 6:
        result = _ExecDashContinueCmd(status, body, command);
        break;
    case 7:
        result = _ExecDropWeaponCmd(status, body, command);
        break;
    case 8:
        result = _ExecDamageCmd(status, body, command);
        break;
    case 9:
        result = _ExecWeakDamageCmd(status, body, command);
        break;
    case 14:
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
    case 8:
        result = _ExecDamageCmd(status, body, command);
        break;
    case 9:
        result = _ExecWeakDamageCmd(status, body, command);
        break;
    case 14:
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
    case 8:
        result = _ExecDamageCmd(status, body, command);
        break;
    case 9:
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
    if (command->type != 8) {
        if (command->type == 9) {
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

const char D_00A51940[] = "pMotInfo != NIL";

extern void RgGeomPointGetVel(RgGeomPoint *point, RgVector velocity);
extern float RgGeomRobotGetRotVel(RgGeomPoint *geom);
extern int RgRobSubGetAdvanceMot(RgGeomPoint *geom, int flag);
extern int RgRobSubGetRollMotion(RgGeomPoint *geom);
static void _BodyPlayMotionLoop(RgBody *body, int motion);
static void _BodyPlayMotion(RgBody *body, int motion);

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
    rotVel = RgGeomRobotGetRotVel(body->geometry);
    if (rotVel < 0.0f) {
        rotVel = -rotVel;
    }
    advanceMot = RgRobSubGetAdvanceMot(body->geometry, 0);
    rollMot = RgRobSubGetRollMotion(body->geometry);
    if (5.0f < speed && advanceMot != -1) {
        motion = advanceMot;
    } else if (0.39269908f < rotVel && rollMot != -1) {
        motion = rollMot;
    } else {
        motion = 0;
    }
    if (motion != status->scratch0) {
        status->scratch0 = motion;
        if (motion == 0) {
            _BodyPlayMotionLoop(body, 0);
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

static int _AllowAllCmdInMoving(RgRobotStatus *status, RgBody *body, RgCmd *command);
static int _MovingPassTime(RgRobotStatus *status, RgBody *body, float deltaTime);

static void _InitMovingStatus(RgRobotStatus *status, RgBody *body)
{
    _SetStatus(status, 1, body);
    status->execCmdMethod = _AllowAllCmdInMoving;
    status->passTimeMethod = _MovingPassTime;
    status->scratch0 = -1;
}

static RgRobotStatus *_InitAttackStatus(RgRobotStatus *status, RgBody *body, int motion, RgWeapon *weapon, int eSide);

typedef struct RgMotionAction RgMotionAction;

struct RgMotionAction
{
    int actionNo;
    float startTime;
    float endTime;
    int shiftMotionNo;
    int weaponMotionNo;
};

extern RgMotionAction *RgMotionInfoGetActionInTime(RgMotionShotInfo *info,
                                                   int actionNo, float time);
static int _IsBackWeaponReady(RgBody *body, unsigned int eType);
extern void RgWeaponPlayMotion(RgWeapon *weapon, int motion);

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
    if (result == 0 && command->type == 10) {
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
    if (command->type == 2 && nextStatus->eSide != 2 &&
        _IsBackWeaponReady(body, command->param) &&
        nextStatus->attackActive != 0) {
        result |= _ExecShotOrAttackCmd(status, body, command);
    }
    return result;
}

extern int RgWeaponGetControlFlag(RgWeapon *weapon);
extern void RgWeaponShot(RgWeapon *weapon);
extern void RgWeaponShotStop(RgWeapon *weapon);
extern int XrgActorIsEndOfMotion(void *actor);
static void _BodyAdvanceForAttack(RgBody *body, float attackPower,
                                  float attackFactor);

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
        if (status->scratch4 != 0 && (controlFlag & 0x40) != 0) {
            _BodyAdvanceForAttack(body, deltaTime, 1.0f);
        }
    } else {
        if (weapon != 0) RgWeaponShotStop(weapon);
        _StopHomingThread(body->homingThread);
    }
    return 32;
}

extern char *RgWeaponGetEss(void *weapon);
extern void RgWeaponRestartLockon(RgWeapon *weapon);

/*
 * Motion-ID table offset within the weapon-essence record RgWeaponGetEss
 * returns; only this field is evidenced here (docs/style.md, "Struct
 * fields, not offset casts" -- narrowly evidenced scalar access).
 */
#define WEP_ESSENCE_ATTACK_MOTION_OFFSET 0x140

static void _AttackExit(RgRobotStatus *status, RgBody *body)
{
    void *shotWeapon;
    void *motionWeapon;
    char *ess;

    shotWeapon = (void *) status->scratch1;
    if (shotWeapon != 0) {
        RgWeaponShotStop(shotWeapon);
    }
    _StopHomingThread(body->homingThread);
    motionWeapon = (void *) status->scratch1;
    if (motionWeapon != 0) {
        ess = RgWeaponGetEss(motionWeapon);
        if (ess != 0) {
            RgWeaponPlayMotion(motionWeapon,
                *(int *) (ess + WEP_ESSENCE_ATTACK_MOTION_OFFSET
                          + (body->charID * RG_EQUIP_TYPE_NUM + status->eSide) * 4));
        }
    }
    if (status->scratch2 != 0) {
        RgWeaponRestartLockon((void *) status->scratch2);
    }
}

extern void RgRobotEffectStartJet(RgRobotEffect *effect, float dashTime);
extern void RgRobotEffectStartDash(RgRobotEffect *effect, float dashTime);
extern void RgGeomPointSetMaxXYSpd(RgGeomPoint *point, float maxSpeed);
extern void XrgSoundRingMoving(int soundID, int unused, float volume);
extern void RgGeomPointSetWeight(RgGeomPoint *point, float weight);
extern float RgWeaponGetWeight(RgWeapon *weapon);
extern void XrgSoundPassTime(int soundID, float dt);
extern float RgGeomRobotGetRotate(RgGeomPoint *point);
extern void RgRobotEffectPassTime(RgRobotEffect *effect, float time);
extern void RgShotThreadPassTime(void *thread, float deltaTime);
extern void XrgActorPassTime(void *actor, float dt);
extern void RgWeaponPassTime(void *weapon, float dt);
extern RgGeomPoint *CreateRgGeomBall(float radius, float weight);
extern float RgWeaponGetShotNum(RgWeapon *weapon);
extern void RgDullFlagPassTime(RgDullFlag *pDull, float fTime);
extern float XrgRand(float lower, float upper);
extern void RgGeomRobotAddRotForce(RgGeomPoint *point, float force);
extern void XrgScaleVector(RgVector destination, RgVector source, float scale);
extern float RgGetGeomGravity(void);
extern void RgRobSubAcceralate(RgGeomPoint *geometry, RgVector direction, float scale);
static void _StartHomingThread(RgHomingThread *thread, RgGeomPoint *geometry,
                               void *target, float actionTime);
static void _PassTimeHomingThread(RgHomingThread *thread, float deltaTime);
static float _CalcMaxSpeed(RgBody *body, float baseSpeed);
static int _StatusPassTime(RgRobotStatus *status, RgBody *body, float dt);
static void _BodyGeomPassTime(RgBody *body, float deltaTime);
static int _EyeGeomPassTime(RgBody *body, float deltaTime);
static void _AdvanceGeomPassTime(RgBody *body, float deltaTime);
static void _PassTimeRobot(RgStatus *pRobot, float deltaTime);
extern void RgWeaponPlayShotMotion(RgWeapon *weapon);
extern int RgWeaponIsTargetting(RgWeapon *weapon);

typedef struct RgMotionInfoDB RgMotionInfoDB;

extern RgMotionInfoDB *InstanceOfRgMotionInfoDB(void);
extern RgMotionShotInfo *RgMotionInfoDBGet(RgMotionInfoDB *db, int motionNo,
                                           int charId);
static int _ExecCmdInAttack(RgRobotStatus *status, RgBody *body, RgCmd *command);
extern void RgWeaponStopLockon(RgWeapon *weapon);

static RgRobotStatus *_InitAttackStatus(RgRobotStatus *status, RgBody *body,
                                        int motion, RgWeapon *weapon, int eSide)
{
    RgMotionShotInfo *motionInfo;
    RgMotionAction *action;

    motionInfo = RgMotionInfoDBGet(InstanceOfRgMotionInfoDB(), motion,
                                   body->charID);
    if (motionInfo == 0) assert_prog(D_00A51940, D_00A51840, 1009);
    _StopAllShotThread(body);
    if (body->actor == 0) return 0;
    _SetStatus(status, 2, body);
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
        status->attackLockonWeapon = body->weapon[1];
        break;
    case 1:
        status->attackLockonWeapon = body->weapon[0];
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
        if ((RgWeaponGetControlFlag(weapon) & 0x18) == 8
            && RgWeaponIsTargetting(weapon)) {
            action = RgMotionInfoGetActionInTime(motionInfo, 0, 100000000.0f);
            if (action != 0) {
                _StartHomingThread(body->homingThread, body->geometry,
                                   body->target, action->startTime);
            }
        }
    }
    return status;
}

static int _AllowDamageCmd(RgRobotStatus *status, RgBody *body, RgCmd *command);

static int _ExecCmdInHissatu(RgRobotStatus *status, RgBody *body, RgCmd *command)
{
    return _AllowDamageCmd(status, body, command);
}

struct RgShot;
extern void RgWeaponSetFree(RgWeapon *weapon);
extern unsigned int RgWeaponGetAttachedShot(RgWeapon *weapon,
                                            struct RgShot **shots);
extern int RgShotGetHitRobNum(struct RgShot *shot);

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
    weapon0 = (RgWeapon *) body->weapon[0];
    weapon1 = (RgWeapon *) body->weapon[1];
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
            RgRobSubHoming(body->geometry, body->target, 12.566371f, deltaTime);
            body->flags |= 1;
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
            RgRobSubHoming(body->geometry, body->target, 12.566371f, deltaTime);
            body->flags |= 1;
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
            shotThread = body->shotThread[1];
            if (weapon1 != 0 && shotThread != 0) {
                RgShotThreadSleep(shotThread);
                RgWeaponShotStop(weapon1);
            }
        }
        if (status->boosting != 0) {
            _BodyAdvanceForAttack(body, deltaTime, 1.0f);
            if (RgWeaponIsTargetting(weapon0) != 0
                || RgWeaponIsTargetting(weapon1) != 0) {
                RgRobSubHoming(body->geometry, body->target, 12.566371f,
                               deltaTime);
                body->flags |= 1;
            }
        } else {
            shotCount = RgWeaponGetAttachedShot(weapon0, shots);
            hit = 0;
            for (i = 0; i < shotCount; i++) {
                if (RgShotGetHitRobNum(shots[i]) != 0) {
                    hit = 1;
                }
            }
            if (status->scratch1 == 78 && hit != 0) {
                nextMotionInfo = RgMotionInfoDBGet(InstanceOfRgMotionInfoDB(), 79,
                                                   body->charID);
                if (nextMotionInfo != 0) {
                    status->hissatuMotionInfo = nextMotionInfo;
                    status->scratch1 = 79;
                    status->elapsedTime = 0.0f;
                    _BodyPlayMotion(body, 79);
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
                RgRobSubHoming(body->geometry, body->target, 12.566371f, deltaTime);
                body->flags |= 1;
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
            RgRobSubHoming(body->geometry, body->target, 12.566371f, deltaTime);
            body->flags |= 1;
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
            RgRobSubHoming(body->geometry, body->target, 12.566371f, deltaTime);
            body->flags |= 1;
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

extern void RgRobotEffectTermDash(RgRobotEffect *effect);
extern void XrgSoundRingStopMoving(int soundID);

static void _HissatuExit(RgRobotStatus *status, RgBody *body)
{
    float maxSpeed;
    void *weapon0;
    void *weapon1;

    (void)status;
    weapon0 = body->weapon[0];
    weapon1 = body->weapon[1];
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

static int _HissatuPassTime(RgRobotStatus *status, RgBody *body, float deltaTime);

static void _InitStatusHissatu(RgRobotStatus *status, RgBody *body)
{
    RgMotionShotInfo *motionInfo;
    int motion;

    motion = 78;
    motionInfo = RgMotionInfoDBGet(InstanceOfRgMotionInfoDB(), motion, body->charID);
    _SetStatus(status, 10, body);
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
    status->scratch2 = (int) motionInfo;
    _BodyPlayMotion(body, 78);
}

extern void RgWeaponSubShotNum(void *weapon, float shotNum);

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
    if (type == 2) {
        if (status->scratch2 == command->param) {
            if (status->scratch0 != type) {
                status->scratch1 = 1;
            }
        }
    }
    if (kind == 11) {
        weapon = *(void **) &status->scratchTimer;
        if (weapon != 0) {
            RgWeaponSubShotNum(weapon, command->scratch0);
        }
        status->scratch4 = 1;
        result |= 0x400;
        XrgCopyVector((float *) &status->dashMotionPlayed, (float *) command);
        type = command->type;
    }
    if (type == 2) {
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

extern void RgGeomRobotCalcLocal(RgGeomPoint *geometry, RgMatrix local);
extern void XrgNegateVector(RgVector destination, RgVector source);
extern void RgGeomPointAddForce(RgGeomPoint *point, RgVector force);
extern void RgError(const char *message, const char *source_file, int line,
                    ...);
extern const char D_00A51968[];

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
        action = RgMotionInfoGetActionInTime(motionInfo, 3, 100000000.0f);
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
        RgGeomRobotCalcLocal(body->geometry, localFrame);
        XrgNegateVector(body->shieldDirection, &localFrame[8]);
        body->shieldDirection[1] = 0.0f;
        if (!status->scratch1 || RgWeaponGetShotNum(weapon) <= 0.0f) {
            action = RgMotionInfoGetActionInTime(motionInfo, 2, 100000000.0f);
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
        RgError(D_00A51968, D_00A51840, 0x63C, state);
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
    weapon = body->weapon[0];
    if (weapon != 0) {
        RgWeaponRestartLockon(weapon);
    }
    weapon = body->weapon[1];
    if (weapon != 0) {
        RgWeaponRestartLockon(weapon);
    }
}

static int _ShieldPassTime(RgRobotStatus *status, RgBody *body, float deltaTime);
static int _ExecCmdInShield(RgRobotStatus *status, RgBody *body, RgCmd *command);

static void _InitShieldStatus(RgRobotStatus *status, RgBody *body, int motion,
                              RgWeapon *shieldWeapon, int scratch2Arg)
{
    RgMotionShotInfo *motionInfo;
    void *actor;

    motionInfo = RgMotionInfoDBGet(InstanceOfRgMotionInfoDB(), motion,
                                   body->charID);
    if (motionInfo == 0) {
        assert_prog(D_00A51940, D_00A51840, 1629);
    }
    _StopAllShotThread(body);
    _SetStatus(status, 3, body);
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
    if (body->weapon[0] != 0) {
        RgWeaponStopLockon(body->weapon[0]);
    }
    if (body->weapon[1] != 0) {
        RgWeaponStopLockon(body->weapon[1]);
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
        case 1:
            mask |= _AllowAllCmdInMoving(status, body, command);
            _StopHomingThread(body->homingThread);
            break;
        case 2:
            weapon = body->weapon[command->param];
            if (weapon != 0 && RgWeaponGetShotNum(weapon) > 0.0f
                && (RgWeaponGetControlFlag(weapon) & 7) == 0) {
                mask |= _AllowAllCmdInMoving(status, body, command);
            }
            break;
        case 0:
        case 4:
        case 5:
            mask |= _AllowAllCmdInMoving(status, body, command);
            break;
        }
    }
    return mask;
}

extern void RgRobotEffectTermJet(RgRobotEffect *effect);
extern void RgRobotEffectSetJetInertia(RgRobotEffect *effect, int inertia);

static int _TargettingPassTime(RgRobotStatus *status, RgBody *body, float deltaTime)
{
    float elapsedTime;
    int result;

    elapsedTime = status->elapsedTime;
    result = 0;
    if (_GetStatHomingThread(body->homingThread) == 1) {
        body->flags |= 3;
        result = 8;
    }
    if (elapsedTime < *(float *) &status->scratch2) {
        if (status->scratch0 == 0) {
            if (*(float *) &status->scratch2 - elapsedTime < 0.2f) {
                _BodyPlayMotion(body, 75);
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

static int _TargettingPassTime(RgRobotStatus *status, RgBody *body, float deltaTime);
extern void __RgGeomPointGetPos(RgGeomPoint *point, RgPointVector *destination,
                                const char *source_file, int source_line);
extern void RgGeomPointSetVel(RgGeomPoint *point, RgVector velocity);
extern int RgGeomGetEventFlag(RgGeom *geom);

static void _InitTargettingStatus(RgRobotStatus *status, RgBody *body, int jump)
{
    RgVector velocity;
    RgVector position;
    RgGeomPoint *geometry;
    void *target;
    float gravity;
    int soundID;
    int startHoming;

    target = body->target;
    if (status == 0) {
        assert_prog(D_00A518F0, D_00A51840, 0x6F2);
    }
    _StopAllShotThread(body);
    startHoming = RgGeomGetEventFlag((RgGeom *) body->eyeGeometry) == 0;
    if (target == 0) {
        startHoming = 0;
    }
    if (body->confuse > 0.0f) {
        startHoming = 0;
    }
    _SetStatus(status, 4, body);
    status->execCmdMethod = _ExecCmdInTargetting;
    status->passTimeMethod = _TargettingPassTime;
    status->exitMethod = _ExitTargettingStatus;
    if (jump != 0) {
        geometry = body->geometry;
        RgGeomPointGetVel(geometry, velocity);
        __RgGeomPointGetPos(geometry, (RgPointVector *) position, D_00A51840, 0x70D);
        gravity = RgGetGeomGravity();
        if (0.0f < 7.5f - position[1]) {
            velocity[1] = __builtin_sqrtf((gravity + gravity) * (7.5f - position[1]));
        }
        RgGeomPointSetVel(geometry, velocity);
        gravity = RgGetGeomGravity();
        soundID = body->landingSoundID;
        status->scratch2AsFloat = (velocity[1] + velocity[1]) / gravity;
        XrgSoundRingStopMoving(soundID);
        XrgSoundRingMoving(body->landingSoundID, 1, status->scratch2AsFloat);
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
    _BodyPlayMotion(body, 0x13);
    status->scratch0 = 0;
}

static int _ExecCmdInBreaking(RgRobotStatus *status, RgBody *body, RgCmd *command)
{
    int mask;
    float delta;
    int rotateSign;

    mask = _AllowDamageCmd(status, body, command);
    if (command->type == 1) {
        delta = *(float *) command;
        if (delta > 0.0f) {
            rotateSign = 1;
        } else if (delta < 0.0f) {
            rotateSign = -1;
        } else {
            rotateSign = 0;
        }
        RgGeomRobotAddRotForce(body->geometry,
                               (float) rotateSign * body->spec->rotateForce * 0.1f);
        body->flags |= 2;
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
    _BodyPlayMotion(body, 0);
}

static void _InitBreakingStatus(RgRobotStatus *status, RgBody *body)
{
    _StopAllShotThread(body);
    _SetStatus(status, 8, body);
    status->execCmdMethod = _ExecCmdInBreaking;
    status->passTimeMethod = _BreakingPassTime;
    status->exitMethod = _BreakingExit;
    status->scratch0AsFloat = 0.3f;
    _BodyPlayMotion(body, 0x14);
}

extern const char D_00A51A30[];
static int _InitDashVRStatus();

static int _DashExecCmd(RgRobotStatus *status, RgBody *body, RgCmd *command)
{
    RgVector vector;
    int result;

    result = _AllowDamageCmd(status, body, command);
    if (status->dashVRAllowed != 0) {
        if (command->type == 6) {
            XrgCopyVector(vector, &status->scratch10);
            if (_InitDashVRStatus(status, body, (RgCmd *) vector)) {
                result |= 4;
            }
        }
    }
    if (command->type == 2) {
        result = _ExecShotOrAttackCmd(status, body, command);
        if (result != 0) {
            return result;
        }
    }
    return result;
}

extern const char D_00A519A8[];

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
        if (RgGeomGetEventFlag((RgGeom *) body->advanceGeometry) != 0) {
            next = 1;
        }
        if (next == 2) {
            XrgCopyVector(tmpVector, (float *) &status->motionInfo);
            _InitDashVRStatus(status, body, (RgCmd *) tmpVector);
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
        RgError(D_00A519A8, D_00A51840, 0x7FE, ((int *) &status->scratch0)[0]);
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
    XrgSoundRingStopMoving(body->landingSoundID);
}

static void _InitShieldStatus(RgRobotStatus *status, RgBody *body, int motion, RgWeapon *shieldWeapon, int scratch2Arg);
extern const char D_00A519C0[];
static int _DashExecCmd(RgRobotStatus *status, RgBody *body, RgCmd *command);
static int _DashPassTime(RgRobotStatus *status, RgBody *body, float deltaTime);

static int _InitDashStatus(RgRobotStatus *status, RgBody *body, RgVector vector)
{
    RgRobotSpec *spec;
    float limit;
    float dashTime;

    spec = body->spec;
    if (status == 0) {
        assert_prog(D_00A518F0, D_00A51840, 0x81E);
    }
    if (spec->type == 1) {
        return 0;
    }
    if (spec->type != 0) {
        if ((unsigned int) spec->type < 5) {
            limit = spec->dashTime * 0.2f;
            if (body->dashTime < limit) {
                return 0;
            }
            body->dashTime = body->dashTime - limit * 0.1f;
        }
    }
    _StopAllShotThread(body);
    _SetStatus(status, 5, body);
    status->execCmdMethod = _DashExecCmd;
    status->passTimeMethod = _DashPassTime;
    status->exitMethod = _DashExit;
    status->scratch0 = 0;
    status->dashMotionPlayed = 0;
    XrgCopyVector((float *) &status->motionInfo, vector);
    status->dashVRAllowed = 0;
    switch (spec->type) {
    case 0:
    case 1:
        status->restoreDashTime = 1;
        status->dashTimeStart = body->dashTime;
        status->dashLimited = 0;
        break;
    case 2:
        status->dashLimited = 1;
        dashTime = body->dashTime;
        if (!(dashTime < 0.5f)) {
            dashTime = 0.5f;
        }
        status->dashTimeStart = dashTime;
        status->restoreDashTime = 0;
        break;
    case 3:
    case 4:
        status->dashLimited = 1;
        dashTime = body->dashTime;
        dashTime = (dashTime < 0.5f) ? dashTime : 0.5f;
        status->dashTimeStart = dashTime;
        status->dashVRAllowed = 1;
        status->restoreDashTime = 0;
        break;
    default:
        RgError(D_00A519C0, D_00A51840, 0x85D, spec->type);
        break;
    }
    RgRobotEffectStartJet(body->effect, spec->dashTime);
    RgRobotEffectStartDash(body->effect, spec->dashTime);
    XrgSoundRingMoving(body->landingSoundID, 1, 100000000.0f);
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
    _BodyPlayMotion(body, 0);
}

struct RgPointVector;
static int _ExecCmdInTargetting(RgRobotStatus *status, RgBody *body, RgCmd *command);
extern float atan2f(float y, float x);
extern float sinf(float angle);
extern float cosf(float angle);
extern void XrgAddVector(RgVector destination, RgVector first, RgVector second);

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
    if (type == 3) {
        _InitTargettingStatus(status, body, 1);
        return result | 0x100;
    }
    if (type == 1) {
        if (((float *) command)[0] > 0.0f) {
            sign = 1;
        } else if (((float *) command)[0] < 0.0f) {
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
    if (type == 0) {
        angle = atan2f(((float *) command)[0], ((float *) command)[2]);
    }
    type = command->type;
    if (type == 2) {
        weapon = body->weapon[command->param];
        if (weapon != 0) {
            flag = RgWeaponGetControlFlag(weapon) & 0x7;
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
    if (type == 12) {
        vec[0] = sinf(status->scratch1AsFloat);
        vec[1] = 0.0f;
        vec[2] = cosf(status->scratch1AsFloat);
        dot = vec[0] * ((float *) command)[0] + vec[1] * ((float *) command)[1]
            + vec[2] * ((float *) command)[2];
        XrgScaleVector(scaled, (float *) command, -dot);
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
    step = _ClampFloat(step, -12.566371f, 12.566371f);
    ((float *) &status->scratch2)[0] += step;
    return result;
}

extern const char D_00A519F8[];
extern const char D_00A51A18[];
extern void RgGeomRobotSetRotate(RgGeom *geom, float rotate);
extern float RgRobSubTargetting(RgGeomPoint *geometry,
                                struct RgPointVector *targetPosition,
                                float speed, float turnRate);

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
        RgGeomRobotSetRotate((RgGeom *) geometry, angle);
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
            RgError(D_00A519F8, D_00A51840, 0x900, status->dashVRMode);
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
            __RgGeomPointGetPos(body->geometry, (struct RgPointVector *) pos,
                                D_00A51840, 0x920);
            XrgAddVector(pos, pos, (float *) &status->motionInfo);
            RgRobSubTargetting(body->geometry, (struct RgPointVector *) pos,
                               12.566371f, deltaTime);
            body->flags |= 3;
        }
        rollMotion = RgRobSubGetRollMotion((RgGeom *) body->geometry);
        if (rollMotion != status->dashMotionPlayed) {
            status->dashMotionPlayed = rollMotion;
            _BodyPlayMotion(body, rollMotion);
        }
        break;
    default:
        RgError(D_00A51A18, D_00A51840, 0x931, ((int *) &status->scratch0)[0]);
    }
    return ret;
}

static void _DashVRExit(RgRobotStatus *status, RgBody *body)
{
    (void)status;
    RgGeomPointSetMaxXYSpd(body->geometry, _CalcMaxSpeed(body, body->spec->moveSpeedMax));
    RgRobotEffectTermJet(body->effect);
    RgRobotEffectTermDash(body->effect);
    XrgSoundRingStopMoving(body->landingSoundID);
}

static int _InitDashVRStatus(RgRobotStatus *status, RgBody *body, RgCmd *command)
{
    RgRobotSpec *spec;
    RgGeomPoint *geometry;
    unsigned int type;
    float angle;
    float rotate;

    spec = body->spec;
    if (status == 0) {
        assert_prog(D_00A518F0, D_00A51840, 0x94D);
    }
    _StopAllShotThread(body);
    if (body->dashTime < spec->dashTime * 0.2f) {
        return 0;
    }
    _SetStatus(status, 7, body);
    status->execCmdMethod = _DashVRExecCmd;
    status->passTimeMethod = _DashVRPassTime;
    status->exitMethod = _DashVRExit;
    *(int *) &status->scratch0 = 0;
    angle = atan2f(((float *) command)[0], ((float *) command)[2]);
    status->scratch2 = 0;
    geometry = body->geometry;
    status->scratch1AsFloat = angle;
    rotate = RgGeomRobotGetRotate(geometry);
    ((float *) &status->motionInfo)[0] = sinf(rotate) * 50.0f;
    status->scratch4 = 0;
    ((float *) &status->motionInfo)[2] = cosf(rotate) * 50.0f;
    type = spec->type;
    switch (type) {
    case 3:
        status->dashVRMode = 1;
        break;
    case 2:
    case 4:
        status->dashVRMode = 0;
        break;
    default:
        RgError(D_00A51A30, D_00A51840, 0x971, type);
        break;
    }
    RgRobotEffectStartJet(body->effect, spec->dashTime);
    RgRobotEffectStartDash(body->effect, spec->dashTime);
    RgGeomPointSetMaxXYSpd(body->geometry, _CalcMaxSpeed(body, spec->dashSpeedMax));
    _BodyPlayMotion(body, 0x32);
    XrgSoundRingMoving(body->landingSoundID, 1, 100000000.0f);
    return 1;
}

static int _AllowDamageCmdNotVRDashHit(RgRobotStatus *status, RgBody *body, RgCmd *command);

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
        if (status->scratch1 != 0 && command->type == 5) {
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

extern int RgRobSubGetDamageMotion(RgGeomPoint *geometry, RgVector direction);

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
        assert_prog(D_00A518F0, D_00A51840, 2478);
    }
    _StopAllShotThread(body);
    _SetStatus(status, 6, body);
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
    _BodyPlayMotion(body, RgRobSubGetDamageMotion(body->geometry, (float *) command));
}

static int _GetDropMotion(int eSide);
static int _InitDropStatus(RgRobotStatus *status, RgBody *body, int motion, int eSide);

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
    if (command->type == 7
        && command->param != status->scratch0
        && command->param == 2
        && status->scratch2 != 0) {
        mask |= 0x200;
        _InitDropStatus(status, body, _GetDropMotion(2), 2);
    }
    return mask;
}

static void _InitMovingStatus(RgRobotStatus *status, RgBody *body);

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

extern int RgWeaponIsAttachedShot(RgWeapon *weapon);

/*
 * The weapon control flag's low three bits give its type; 4 is the unarmed
 * type (InitRgWeaponUnArmedEssence sets the mode word to 76, whose low bits
 * are 4; rg_hissatu.c's _IsUnarmed tests the same value).
 */
#define RG_WEAPON_CONTROL_TYPE_MASK 7
#define RG_WEAPON_CONTROL_UNARMED 4

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
    if ((RgWeaponGetControlFlag(body->weapon[eSide]) & RG_WEAPON_CONTROL_TYPE_MASK)
        == RG_WEAPON_CONTROL_UNARMED) {
        return 0;
    }
    if (RgWeaponIsAttachedShot(body->weapon[eSide]) != 0) {
        return 0;
    }
    _SetStatus(status, 9, body);
    status->execCmdMethod = _ExecCmdInDrop;
    status->passTimeMethod = _DropPassTime;
    status->scratch0 = eSide;
    status->dropMotionInfo = motionInfo;
    status->scratch2 = 1;
    _BodyPlayMotion(body, motion);
    return 1;
}

/* ov12:0x00a4f560, 0x3c bytes: one priority level per command type. */
#define RG_CMD_TYPE_NUM 15

extern int s_anCmdLevel[RG_CMD_TYPE_NUM];

static RgCmd *_EntryCmdQueue(void *cmdQueue, int cmdType)
{
    RgCmdQueue *pQ = cmdQueue;
    int level;
    int diff;
    RgCmd *command;

    if (pQ == 0) {
        assert_prog(D_00A51A58, D_00A51840, 0xA41);
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
extern const char D_00A51A58[];

static void _InitCmdQueue(RgCmdQueue *cmdQueue)
{
    if (cmdQueue == 0) {
        assert_prog(D_00A51A58, D_00A51840, 2673);
    }
    cmdQueue->count = 0;
    cmdQueue->entryCount = 0;
}

static void _ClearCmdQueue(RgCmdQueue *cmdQueue)
{
    if (cmdQueue == 0) {
        assert_prog(D_00A51A58, D_00A51840, 2681);
    }
    cmdQueue->count = 0;
}

static RgCmdQueue *_CreateCmdQueue(void)
{
    RgCmdQueue *cmdQueue;

    cmdQueue = RgHeapAlloc(InstanceOfRgHeap(), sizeof(RgCmdQueue), D_00A51840, 2689);
    _InitCmdQueue(cmdQueue);
    return cmdQueue;
}

static void _DisposeCmdQueue(RgCmdQueue *cmdQueue)
{
    if (cmdQueue == 0) {
        assert_prog(D_00A51A58, D_00A51840, 2697);
    }
    RgHeapFree(InstanceOfRgHeap(), cmdQueue, D_00A51840, 2698);
}

typedef struct XrgActor XrgActor;

extern void XrgActorSetMotion(XrgActor *actor, int motion);
extern void XrgActorSetSmoothPlay(XrgActor *actor, int smoothPlay);

static void _BodyPlayMotion(RgBody *body, int motion)
{
    XrgActor *actor;

    actor = (XrgActor *) body->actor;
    if (actor != 0) {
        if (body->smoothSuppressed != 0) {
            XrgActorSetSmoothPlay(actor, 0);
        } else {
            XrgActorSetSmoothPlay(actor, body->motSmooth);
        }
        XrgActorSetMotion(actor, motion);
    }
}

extern void XrgActorSetLoopPlay(XrgActor *actor, int loopPlay);

static void _BodyPlayMotionLoop(RgBody *body, int motion)
{
    XrgActor *actor;

    actor = (XrgActor *) body->actor;
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
    RgGeomRobotCalcLocal(geometry, localFrame);
    /* row 2 of the local frame: the body's forward (z) axis */
    XrgNegateVector(direction, &localFrame[8]);
    RgRobSubAcceralate(geometry, direction,
                       spec->enginePower * spec->dashPower * 0.3f * attackFactor);
}

/* ov12:0x00a51a68 contains the assertion expression "pBody != NIL". */
extern const char D_00A51A68[];

/* Opaque here: rg_geom_point.c (ov12/tu052) owns the RgPointVector definition. */
typedef struct RgPointVector RgPointVector;

static int _BodyGetTargetPos(RgBody *body, RgPointVector *position)
{
    if (body == 0) {
        assert_prog(D_00A51A68, D_00A51840, 2758);
    }
    if (body->target != 0) {
        __RgGeomPointGetPos(body->target, position, D_00A51840, 2760);
        return 1;
    }
    return 0;
}

static void _BodySetSpec(RgBody *body, RgRobotSpec *spec)
{
    float life;

    if (body == 0) {
        assert_prog(D_00A51A68, D_00A51840, 2773);
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

extern void RgGeomFree(RgGeomPoint *point);
extern void RgGeomPointSetMoveResist(RgGeomPoint *point, float resist);
extern void RgGeomRobotSetRotResist(RgGeomPoint *point, float resist);
extern void RgRobotEffectSetGeom(RgRobotEffect *effect, RgGeomPoint *geometry);
/* ov12:0x00a51a78 contains the assertion expression "pGeom != NIL". */
extern const char D_00A51A78[];

static void _BodySetGeom(RgBody *body, RgGeomPoint *geometry)
{
    RgGeomPoint *oldGeometry;
    RgRobotSpec *spec;

    if (body == 0) {
        assert_prog(D_00A51A68, D_00A51840, 0xAEA);
    }
    if (geometry == 0) {
        assert_prog(D_00A51A78, D_00A51840, 0xAEB);
    }
    oldGeometry = body->geometry;
    if (geometry != oldGeometry) {
        RgGeomFree(oldGeometry);
    }
    spec = body->spec;
    body->geometry = geometry;
    RgGeomPointSetWeight(geometry, spec->weight);
    RgGeomPointSetMoveResist(geometry, spec->moveResist);
    RgGeomRobotSetRotResist(geometry, spec->rotResist);
    body->rotate = RgGeomRobotGetRotate(geometry);
    RgRobotEffectSetGeom(body->effect, geometry);
    RgGeomPointSetMaxXYSpd(body->geometry, _CalcMaxSpeed(body, spec->moveSpeedMax));
}

static void _BodySetEyeGeom(RgBody *body, RgGeomPoint *eyeGeometry)
{
    RgGeomPoint *oldEyeGeometry;

    if (body == 0) {
        assert_prog(D_00A51A68, D_00A51840, 2816);
    }
    oldEyeGeometry = body->eyeGeometry;
    if (oldEyeGeometry != 0 && oldEyeGeometry != eyeGeometry) {
        RgGeomFree(oldEyeGeometry);
    }
    body->eyeGeometry = eyeGeometry;
}

/* ov12:0x00a51a78 contains the assertion expression "pGeom != NIL". */
static void _BodySetAdvanceGeom(RgBody *body, RgGeomPoint *advanceGeometry)
{
    RgGeomPoint *oldAdvanceGeometry;

    if (body == 0) {
        assert_prog(D_00A51A68, D_00A51840, 2826);
    }
    if (advanceGeometry == 0) {
        assert_prog(D_00A51A78, D_00A51840, 2827);
    }
    oldAdvanceGeometry = body->advanceGeometry;
    if (oldAdvanceGeometry != 0 && oldAdvanceGeometry != advanceGeometry) {
        RgGeomFree(oldAdvanceGeometry);
    }
    body->advanceGeometry = advanceGeometry;
}

extern void DisposeXrgSound(int soundID);
extern void RgWeaponSetSound(RgWeapon *weapon, int soundID);

static void _BodySetSound(RgBody *body, void *soundDriver)
{
    unsigned int eSide;
    RgWeapon *weapon;

    if (body->landingSoundID != 0 && body->landingSoundID != (int) soundDriver) {
        DisposeXrgSound(body->landingSoundID);
    }
    body->landingSoundID = (int) soundDriver;
    eSide = 0;
    while (eSide < RG_EQUIP_TYPE_NUM) {
        if ((weapon = body->weapon[eSide]) != 0) {
            RgWeaponSetSound(weapon, body->landingSoundID);
        }
        if ((weapon = (RgWeapon *) body->spareWeapon[eSide]) != 0) {
            RgWeaponSetSound(weapon, body->landingSoundID);
        }
        eSide++;
    }
}

/*
 * CreateXrgActor's published parameter is `int actorID` (src/ov12/xrg_actor.c,
 * ov12/tu080); this caller passes the actorSpec pointer's own raw value as
 * that argument (ov12:0x00a06438, daddu $4, $17, $0 straight into the call,
 * no load first), then reads charID back from *actorSpec afterward
 * (ov12:0x00a06440), so actorSpec is reinterpreted as actorID here.
 */
extern XrgActor *CreateXrgActor(int actorID);
extern void DisposeXrgActor(XrgActor *actor);
extern void RgRobotEffectSetActor(RgRobotEffect *effect, void *actor);

static void _BodySetActor(RgBody *body, void *actorSpec)
{
    XrgActor *oldActor;
    XrgActor *actor;

    if (body == 0) {
        assert_prog(D_00A51A68, D_00A51840, 0xB2A);
    }
    oldActor = (XrgActor *) body->actor;
    if (oldActor != 0) {
        DisposeXrgActor(oldActor);
    }
    body->actor = 0;
    if (actorSpec != 0) {
        actor = CreateXrgActor((int) actorSpec);
        body->actor = actor;
        body->charID = *(int *) actorSpec;
        RgRobotEffectSetActor(body->effect, actor);
        _BodyPlayMotionLoop(body, 0);
    }
}

static void _BodySetTarget(RgBody *body, void *target)
{
    if (body == 0) {
        assert_prog(D_00A51A68, D_00A51840, 2890);
    }
    body->target = target;
}

static int _BodyDropWeapon(RgBody *body, unsigned int eType)
{
    RgWeapon *weapon;
    void *actor;
    void *weaponActor;
    RgGeomPoint *geometry;
    float geomWeight;

    if (body == 0) {
        assert_prog(D_00A51A68, D_00A51840, 2900);
    }
    if (eType >= RG_EQUIP_TYPE_NUM) {
        assert_prog(D_00A51A88, D_00A51840, 2901);
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

extern void *RgWeaponGetActor(RgWeapon *weapon);
extern void XrgActorDettachChild(void *parent, void *child);
extern void RgWeaponDisposeLockon(RgWeapon *weapon);
extern void RgWeaponSetdown(RgWeapon *weapon);
extern void *DisposeRgWeaponNotActor(RgWeapon *weapon);
extern void CreateRgDropWeapon(void *actor);
extern void RgShotThreadInit(struct RgShotThread *thread, void *weapon,
                             int side, void *actor);
extern void *RgWeaponGetEquip(RgWeapon *weapon);
extern int RgEquipMountPos(void *equip, unsigned int side, int charID,
                           const void *mountTable);
extern int RgEquipPosToJntID(int mountPos);
extern void XrgActorDeriveMotion(void *weaponActor, void *actor);
extern void XrgActorAttachChild(void *parent, void *child, int jointID);
extern int RgWeaponIsActiveLockOnSys(RgWeapon *weapon);
extern int XrgActorGetAttachedJoint(void *actor);
extern void XrgActorCheckPakaPaka(void *actor);
extern void RgShotThreadMotOff(struct RgShotThread *thread);
extern void XrgLog(const char *message, const char *sourceFile, int line);
extern const char D_00A51AC8[];
extern const char D_00A51AD0[];
extern const char D_00A51AF8[];
extern const char D_00A51B18[];
extern const char D_00A51B28[];
extern const char D_00A51B40[];
static void _BodySetSound(RgBody *body, void *soundDriver);

static int _BodyEquipWeapon(RgBody *body, int side, RgWeapon *weapon)
{
    char *ess;
    void *equip;
    int mountPos;
    int jointID;
    void *weaponActor;
    void *lockonActor;
    RgWeapon *otherWeapon;
    void *shotThread;
    float geomWeight;
    int result;

    if (body == 0) {
        assert_prog(D_00A51A68, D_00A51840, 0xBA9);
    }
    if ((unsigned int) side >= RG_EQUIP_TYPE_NUM) {
        assert_prog(D_00A51A88, D_00A51840, 0xBAA);
    }
    result = _BodyDropWeapon(body, side);
    if (weapon == 0) {
        return result;
    }
    ess = RgWeaponGetEss(weapon);
    if (ess != 0) {
        equip = RgWeaponGetEquip(weapon);
        mountPos = RgEquipMountPos(equip, side, body->charID, ess + 0x330);
    } else {
        equip = RgWeaponGetEquip(weapon);
        mountPos = RgEquipMountPos(equip, side, body->charID, D_00A51AC8);
    }
    body->weapon[side] = weapon;
    geomWeight = RgGeomPointGetWeight(body->geometry);
    RgGeomPointSetWeight(body->geometry,
                         geomWeight + RgWeaponGetWeight(weapon));
    if (RgWeaponGetActor(weapon) != 0 && body->actor != 0) {
        lockonActor = 0;
        RgWeaponGetActor(weapon);
        jointID = RgEquipPosToJntID(mountPos);
        if (body->weapon[2] != 0) {
            lockonActor = RgWeaponGetActor(body->weapon[2]);
        }
        if (mountPos == -1) {
            assert_prog(D_00A51AD0, D_00A51840, 0xBE3);
        }
        weaponActor = RgWeaponGetActor(weapon);
        XrgActorDeriveMotion(weaponActor, body->actor);
        if (jointID == -1) {
            assert_prog(D_00A51AF8, D_00A51840, 0xBEA);
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
        } else if (side == 2) {
            otherWeapon = 0;
            if (jointID == 4) {
                otherWeapon = body->weapon[0];
            } else if (jointID == 5) {
                otherWeapon = body->weapon[1];
            }
            if (otherWeapon != 0 && RgWeaponIsActiveLockOnSys(otherWeapon)
                && (RgWeaponGetControlFlag(otherWeapon) & 0x20)) {
                XrgLog(D_00A51B18, D_00A51840, 0xC0E);
                XrgActorCheckPakaPaka(weaponActor);
                XrgLog(D_00A51B28, D_00A51840, 0xC10);
            } else {
                XrgLog(D_00A51B40, D_00A51840, 0xC12);
            }
        }
    }

    if ((RgWeaponGetControlFlag(weapon) & RG_WEAPON_CONTROL_TYPE_MASK)
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
    _BodySetSound(body, body->landingSoundID);
    return result;
}

/*
 * spareWeapon[] holds the weapon handle as int (RgRobotSetSpareWeapon's
 * weaponID); the slot being replaced is disposed as the weapon it is.
 */
extern void DisposeRgWeapon(RgWeapon *weapon);

static void _BodySetSpareWeapon(RgBody *body, int eSide, int weaponID)
{
    if (body == 0) {
        assert_prog(D_00A51A68, D_00A51840, 0xC3B);
    }
    if ((unsigned int) eSide >= RG_EQUIP_TYPE_NUM) {
        assert_prog(D_00A51A88, D_00A51840, 0xC3C);
    }
    if (body->spareWeapon[eSide] != 0 && body->spareWeapon[eSide] != weaponID) {
        DisposeRgWeapon((RgWeapon *) body->spareWeapon[eSide]);
    }
    body->spareWeapon[eSide] = weaponID;
}

static void _BodySetAutoHomingEnv(RgBody *body, int autoHomingEnv)
{
    if (body == 0) {
        assert_prog(D_00A51A68, D_00A51840, 3142);
    }
    body->autoHomingEnv = autoHomingEnv;
}

/* ov12:0x00a51b48 contains the assertion expression "pCmd != NIL". */
extern const char D_00A51B48[];

static int _BodyExecCmd(RgRobotStatus *robotStatus, RgBody *body,
                        RgCmd *command)
{
    if (body == 0) {
        assert_prog(D_00A51A68, D_00A51840, 3152);
    }
    if (command == 0) {
        assert_prog(D_00A51B48, D_00A51840, 3153);
    }
    body->cmdMask |= 1 << command->type;
    return _StatusExecCmd(robotStatus, command, body);
}

extern float RgGeomPointGetSpeed(RgGeomPoint *point);
extern void RgGeomRobotSetMaxRotVel(RgGeom *geom, float maxRotVel);
extern void RgGeomPassTime(RgGeom *geom, float deltaTime);

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
    if (!(body->flags & 1)) {
        rotVelScale = (maxSpeed - speed) / maxSpeed;
        if (!(rotVelScale > 0.5f)) {
            rotVelScale = 0.5f;
        }
        RgGeomRobotSetMaxRotVel((RgGeom *)geometry, spec->turnRate * rotVelScale);
    } else {
        RgGeomRobotSetMaxRotVel((RgGeom *)geometry, 1e8f);
    }
    body->flags &= ~1;
    RgGeomPassTime((RgGeom *)geometry, deltaTime);
}

extern void RgGeomPointSetPos(RgGeomPoint *point, RgVector position);
extern void RgGeomPointMovePos(RgGeomPoint *point, RgVector position);

static int _EyeGeomPassTime(RgBody *body, float deltaTime)
{
    RgGeom *eyeGeom;
    int eventFlag;
    RgVector position;

    eyeGeom = (RgGeom *) body->eyeGeometry;
    eventFlag = RgGeomGetEventFlag(eyeGeom);
    RgGeomPassTime(eyeGeom, deltaTime);
    __RgGeomPointGetPos(body->geometry, (RgPointVector *) position, D_00A51840, 0xC79);
    RgGeomPointSetPos((RgGeomPoint *) eyeGeom, position);
    if (body->target != 0) {
        __RgGeomPointGetPos(body->target, (RgPointVector *) position, D_00A51840, 0xC7E);
        RgGeomPointMovePos((RgGeomPoint *) eyeGeom, position);
    }
    return eventFlag;
}

static void _AdvanceGeomPassTime(RgBody *body, float deltaTime)
{
    RgGeom *advanceGeom;
    RgGeomPoint *geometry;
    RgVector position;
    RgVector velocity;

    advanceGeom = (RgGeom *) body->advanceGeometry;
    geometry = body->geometry;
    RgGeomPassTime(advanceGeom, deltaTime);
    __RgGeomPointGetPos(geometry, (RgPointVector *) position, D_00A51840, 3213);
    RgGeomPointSetPos((RgGeomPoint *) advanceGeom, position);
    RgGeomPointGetVel(geometry, velocity);
    XrgScaleVector(velocity, velocity, 0.5f);
    XrgAddVector(position, position, velocity);
    RgGeomPointMovePos((RgGeomPoint *) advanceGeom, position);
}

static void _BodyAutoHomingPassTime(RgBody *body, float deltaTime)
{
    RgRobotSpec *spec;
    RgGeomPoint *geometry;
    unsigned int i;
    unsigned int flags;
    unsigned int homingFlags;

    if (body == 0) {
        assert_prog(D_00A51A68, D_00A51840, 3230);
    }
    flags = body->flags;
    spec = body->spec;
    geometry = body->geometry;
    if ((flags & 2) != 0) {
        body->flags = flags & ~2U;
        return;
    }
    for (i = 0; i < 2; i++) {
        homingFlags = spec->m_aAutoHoming[i].flags;
        if (0.0f < spec->m_aAutoHoming[i].value) {
            if (homingFlags != 0 && (body->autoHomingEnv & homingFlags) == homingFlags) {
                RgRobSubHoming(geometry, body->target, spec->m_aAutoHoming[i].value,
                               deltaTime);
                body->flags |= 1;
            }
        }
    }
}

static int _BodyDropWeapon(RgBody *body, unsigned int eType);
extern int RgWeaponIsLockedOn(void *weapon);
static void _BodyAutoHomingPassTime(RgBody *body, float deltaTime);

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
        assert_prog(D_00A51A68, D_00A51840, 0xCBD);
    }
    body->landingSoundTime += passTime;
    XrgSoundPassTime(body->landingSoundID, passTime);
    if (body->confuse > 0.0f) {
        body->confuse = body->confuse - passTime;
    }
    if (!(robotStatus->type == 7 || robotStatus->type == 5)) {
        body->rotate = RgGeomRobotGetRotate(body->geometry);
    }
    RgRobotEffectPassTime(body->effect, frameTime);
    if (body->weakHitMotion > 0.0f) {
        body->weakHitMotion = body->weakHitMotion - frameTime;
        flags = 0x10;
        passTime = frameTime * body->weakHitPower;
    }
    flags |= _StatusPassTime(robotStatus, body, passTime);
    for (i = 0; i < 3; i++) {
        RgShotThreadPassTime(body->shotThread[i], passTime);
    }
    _PassTimeHomingThread(body->homingThread, passTime);
    if (_GetStatHomingThread(body->homingThread) == 1) {
        body->flags |= 1;
    }
    _BodyAutoHomingPassTime(body, passTime);
    _BodyGeomPassTime(body, passTime);
    if (body->actor != 0) {
        XrgActorPassTime(body->actor, passTime);
    }
    for (i = 0; i < 3; i++) {
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

extern void XrgActorSetLocal(XrgActor *actor, const RgMatrix matrix);
extern void XrgActorDraw(XrgActor *actor);
extern void RgWeaponDisp(RgWeapon *weapon);
extern void RgRobotEffectDisp(RgRobotEffect *effect);

static void _BodyDisp(RgRobotStatus *robotStatus, RgBody *body)
{
    RgMatrix localFrame;
    unsigned int eSide;

    if (body == 0) {
        assert_prog(D_00A51A68, D_00A51840, 0xD0E);
    }
    _StatusDisp(robotStatus, body);
    RgGeomRobotCalcLocal(body->geometry, localFrame);
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

extern RgRobotSpec D_00A59920;
extern void InitRgRobotSpec(RgRobotSpec *pSpec);
extern RgGeomPoint *CreateRgGeomRobot(void);
extern void *CreateRgShotThread(void);
extern RgRobotEffect *CreateRgRobotEffect(void);

static void _InitBody(RgBody *body)
{
    unsigned int eType;

    InitRgRobotSpec(&D_00A59920);
    if (body == 0) {
        assert_prog(D_00A51A68, D_00A51840, 0xD37);
    }
    body->charID = -1;
    body->life = 100.0f;
    body->lifeMax = 100.0f;
    body->spec = &D_00A59920;
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
    body->geometry = CreateRgGeomRobot();
    body->eyeGeometry = CreateRgGeomBall(1.0f, 1.0f);
    body->advanceGeometry = CreateRgGeomBall(0.3f, 0.3f);
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
        body->landingSoundID = 0;
        body->confuse = 0.0f;
    } while (0);
}

extern void DisposeRgShotThread(struct RgShotThread *thread);
struct XrgSound;
extern void DisposeRgRobotEffect(RgRobotEffect *effect);

static void _DestructBody(RgBody *body)
{
    unsigned int i;

    if (body == 0) {
        assert_prog(D_00A51A68, D_00A51840, 3454);
    }
    RgGeomFree(body->geometry);
    if (body->eyeGeometry != 0) {
        RgGeomFree(body->eyeGeometry);
    }
    if (body->advanceGeometry != 0) {
        RgGeomFree(body->advanceGeometry);
    }
    for (i = 0; i < RG_EQUIP_TYPE_NUM; i++) {
        if (body->weapon[i] != 0) {
            DisposeRgWeapon(body->weapon[i]);
        }
        if (body->spareWeapon[i] != 0) {
            DisposeRgWeapon((RgWeapon *) body->spareWeapon[i]);
        }
        DisposeRgShotThread(body->shotThread[i]);
    }
    _DisposeHomingThread(body->homingThread);
    DisposeRgRobotEffect(body->effect);
    if (body->actor != 0) {
        DisposeXrgActor(body->actor);
    }
    if (body->landingSoundID != 0) {
        DisposeXrgSound((struct XrgSound *) body->landingSoundID);
    }
}

static void _InitBody(RgBody *body);

static RgBody *_CreateBody(void)
{
    RgBody *body;

    body = RgHeapAlloc(InstanceOfRgHeap(), sizeof(RgBody), D_00A51840, 3482);
    _InitBody(body);
    return body;
}

extern int RgHeapIsInvalidMemory(RgHeap *heap, void *ptr);
static void _DestructBody(RgBody *body);
/* ov12:0x00a51b58 contains the format string "already disposed body %p". */
extern const char D_00A51B58[];

static void _DisposeBody(RgBody *body)
{
    if (body == 0) {
        assert_prog(D_00A51A68, D_00A51840, 3490);
    }
    if (RgHeapIsInvalidMemory(InstanceOfRgHeap(), body) != 0) {
        RgError(D_00A51B58, D_00A51840, 3492, body);
    }
    _DestructBody(body);
    RgHeapFree(InstanceOfRgHeap(), body, D_00A51840, 3494);
}

static int _ExecAccelarateCmd(RgStatus *status, RgBody *body, RgCmd *command)
{
    RgRobotSpec *spec;
    RgGeomPoint *geometry;

    spec = body->spec;
    geometry = body->geometry;
    RgGeomPointSetMaxXYSpd(geometry, _CalcMaxSpeed(body, spec->moveSpeedMax));
    RgRobSubAcceralate(geometry, (float *) command, spec->enginePower);
    XrgSoundRingMoving(body->landingSoundID, 0, 0.2f);
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

    delta = *(float *) command;
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
    RgGeomRobotAddRotForce(body->geometry, (float) rotateSign * body->spec->rotateForce);
    body->flags |= 2;
    XrgSoundRingMoving(body->landingSoundID, 0, 0.2f);
    return 2;
}

static int _IsBackWeaponReady(RgBody *body, unsigned int eType)
{
    if (eType >= RG_EQUIP_TYPE_NUM) {
        assert_prog(D_00A51A88, D_00A51840, 0xDD7);
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

static int _DashVRExecCmd(RgRobotStatus *status, RgBody *body, RgCmd *command);
static int _DashVRPassTime(RgRobotStatus *status, RgBody *body, float deltaTime);
extern int RgHissatuCheck(int charID, void **weapon);
extern void RgShotThreadStart(void *thread, void *params);
extern int RgShotThreadGetStatus(void *thread);
extern int RgWeaponRobotShotMotion(RgWeapon *weapon, int charID);
extern int RgWeaponIsNotBusy(RgWeapon *weapon);
static void _InitStatusHissatu(RgRobotStatus *status, RgBody *body);

static int _ExecShotOrAttackCmd(RgRobotStatus *status, RgBody *body, RgCmd *command)
{
    struct {
        int motion;
        float startTime;
        float endTime;
    } startParam;
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
    if (eSide == 2 && RgHissatuCheck(body->charID, body->weapon)) {
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
    shotType = controlFlag & 0x7;
    motionInfo = RgMotionInfoDBGet(InstanceOfRgMotionInfoDB(), motion, body->charID);
    if (eSide == 2) {
        RgShotThreadSleep(body->shotThread[0]);
        RgShotThreadSleep(body->shotThread[1]);
    }
    if (shotType == 0 && RgShotThreadGetStatus(shotThread) != 5) {
        action = RgMotionInfoGetActionInTime(motionInfo, 0, 100000000.0f);
        if ((controlFlag & 0x20) != 0 || motion == -1) {
            startParam.motion = -1;
            startParam.startTime = 0.18f;
            startParam.endTime = 0.4f;
        } else {
            startParam.motion = motion;
            if (action == 0) {
                startParam.startTime = 0.18f;
                startParam.endTime = 0.4f;
            } else {
                startParam.startTime = action->startTime;
                startParam.endTime = action->endTime;
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

static void _InitTargettingStatus(RgRobotStatus *status, RgBody *body, int jump);

/* _AllowAllCmdInMoving supplies mode 1; _InitTargettingStatus tests this
 * mode as zero/nonzero and reads the actual target from body separately.
 * Its result is the 0x100 command-processing mask bit, not a command id. */
#define RG_CMD_RESULT_TARGETTING 0x100

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
        _InitDashStatus(status, body, command);
        result = 4;
    } else {
        _InitDashVRStatus(status, body, command);
        result = 4;
    }
done:
    return result;
}

static int _ExecDashContinueCmd(RgRobotStatus *status, RgBody *body, RgCmd *command)
{
    int result;

    result = 0;
    if (body->spec->type == 1) {
        /* The original call passes two extra, ignored arguments. */
        ((int (*)()) _InitDashVRStatus)(status, body, command, 1, 0);
        result = 4;
    }
    return result;
}

/* The format string "unknown equip type (drop weapon) %d". */
extern const char D_00A51B78[];

static int _GetDropMotion(int eSide)
{
    int motion;

    motion = 0;
    switch (eSide) {
    case 2:
        break;
    case 0:
        motion = 0x1F;
        break;
    case 1:
        motion = 0x20;
        break;
    default:
        RgError(D_00A51B78, D_00A51840, 3752, eSide);
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

extern void CreateRgHitEffectPosDir(RgVector position, RgVector direction,
                                    char *name1, char *name2);
extern float XrgNormalizeVector(RgVector destination, RgVector source);

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

extern const char D_00A51BA0[];

static int _ExecHitByBody(RgRobotStatus *status, RgBody *body, RgCmd *command)
{
    RgVector force;

    /* The legacy API accepts char * but only reads this effect name. */
    CreateRgHitEffectPosDir(&command->scratch0, (float *) command,
                            (char *) D_00A51BA0, 0);
    XrgNormalizeVector(force, (float *) command);
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

extern void RgDullFlagSetFlag(RgDullFlag *pDull);
extern int RgDullFlagGet(RgDullFlag *pDull);
extern int RgDrawViewIsPointInView(RgDrawView *view, RgPointVector *point, float margin);

static void _DullFlagsPassTime(RgStatus *pRobot, float deltaTime)
{
    RgPointVector targetPos;
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
    if (_BodyGetTargetPos(pRobot->body, &targetPos) != 0) {
        if (pRobot->drawView != 0
            && RgDrawViewIsPointInView(pRobot->drawView, &targetPos, 0.0f)) {
            flags |= 8;
        }
    }
    _BodySetAutoHomingEnv(pRobot->body, flags);
}

extern int RgRobotIsDead(RgStatus *pRobot);
static int _BodyExecCmd(RgRobotStatus *robotStatus, RgBody *body,
                         RgCmd *command);
static int _BodyPassTime(RgRobotStatus *robotStatus, RgBody *body, float deltaTime);
static void _ClearCmdQueue(RgCmdQueue *cmdQueue);
static void _DullFlagsPassTime(RgStatus *pRobot, float deltaTime);

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
        assert_prog(D_00A51BB0, D_00A51840, 3976);
    }
    _BodyDisp(pRobot->robotStatus, pRobot->body);
}

/* ov12:0x00a51bc0 contains the format string "already disposed robot %p\n". */
extern const char D_00A51BC0[];

static void _DestructRobot(RgStatus *pRobot)
{
    if (pRobot == 0) {
        assert_prog(D_00A51BB0, D_00A51840, 3985);
    }
    if (RgHeapIsInvalidMemory(InstanceOfRgHeap(), pRobot) != 0) {
        RgError(D_00A51BC0, D_00A51840, 3987, pRobot);
    }
    _DisposeBody(pRobot->body);
    _DisposeStatus(pRobot->robotStatus);
    _DisposeCmdQueue(pRobot->cmdQueue);
}

static int _InitDashStatus(RgRobotStatus *status, RgBody *body, RgVector vector);

/*
 * RgDullFlag is defined by ov12/tu031 (src/ov12/rg_dull_flag.h); this TU
 * only passes the pointer, at RgStatus's dull-flag offsets.
 */
typedef struct RgDullFlag RgDullFlag;

extern void InitRgDullFlag(RgDullFlag *pDull);
extern void RgDullFlagSetDullTime(RgDullFlag *pDull, float fTime);

static void _InitRobot(RgStatus *pRobot)
{
    RgRobotStatus *status;
    RgDullFlag *dullFlag0 = (RgDullFlag *) (pRobot->unmodeled_14 + 0x0c);
    RgDullFlag *dullFlag1 = (RgDullFlag *) (pRobot->unmodeled_14 + 0x1c);
    RgDullFlag *dullFlag2 = (RgDullFlag *) (pRobot->unmodeled_14 + 0x2c);

    if (pRobot == 0) {
        assert_prog(D_00A51BB0, D_00A51840, 4002);
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
    *(int *) &pRobot->passTimeTimer = 0;
}

static void _InitRobot(RgStatus *pRobot);

RgStatus *CreateRgRobot(void)
{
    RgStatus *pRobot;

    pRobot = RgHeapAlloc(InstanceOfRgHeap(), sizeof(RgStatus), D_00A51840,
                          4041);
    _InitRobot(pRobot);
    return pRobot;
}

static void _DestructRobot(RgStatus *pRobot);

void DisposeRgRobot(RgStatus *pRobot)
{
    if (pRobot == 0) {
        assert_prog(D_00A51BB0, D_00A51840, 4049);
    }
    _DestructRobot(pRobot);
    RgHeapFree(InstanceOfRgHeap(), pRobot, D_00A51840, 4051);
}

extern void CreateRgHitEffectPos(RgVector position, char *name1, char *name2);

void RgRobotPassTime(RgStatus *pRobot, float deltaTime)
{
    RgVector position;
    RgRobotStatus *status;

    if (pRobot == 0) {
        assert_prog(D_00A51BB0, D_00A51840, 0xFDC);
    }
    _PassTimeRobot(pRobot, deltaTime);
    if (pRobot->body->life <= 0.0f) {
        status = pRobot->robotStatus;
        if (status->type != 1) {
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
                __RgGeomPointGetPos(pRobot->body->geometry, (RgPointVector *) position,
                                    D_00A51840, 0xFF7);
                position[0] = position[0] + XrgRand(-1.7f, 1.7f);
                position[1] = position[1] + XrgRand(-1.0f, 3.5f);
                position[2] = position[2] + XrgRand(-1.7f, 1.7f);
                CreateRgHitEffectPos(position, (char *) D_00A51BA0,
                                     (char *) D_00A51BA0);
            }
            if (((int) (pRobot->passTimeTimer * 20.0f) & 1) == 0) {
                XrgSoundRing(pRobot->body->landingSoundID, 21);
            }
        }
    }
}

static void _DispRobot(RgStatus *pRobot);

void RgRobotDisp(RgStatus *pRobot)
{
    if (pRobot == 0) {
        assert_prog(D_00A51BB0, D_00A51840, 4109);
    }
    _DispRobot(pRobot);
}

static void _BodySetSpec(RgBody *body, RgRobotSpec *spec);

void RgRobotSetSpec(RgStatus *pRobot, RgRobotSpec *spec)
{
    if (pRobot == 0) {
        assert_prog(D_00A51BB0, D_00A51840, 4124);
    }
    _BodySetSpec(pRobot->body, spec);
}

static void _BodySetGeom(RgBody *body, RgGeomPoint *geometry);

void RgRobotSetGeom(RgStatus *pRobot, RgGeomPoint *geometry)
{
    if (pRobot == 0) {
        assert_prog(D_00A51BB0, D_00A51840, 4133);
    }
    _BodySetGeom(pRobot->body, geometry);
}

static void _BodySetEyeGeom(RgBody *body, RgGeomPoint *eyeGeometry);

void RgRobotSetEyeGeom(RgStatus *pRobot, RgGeomPoint *eyeGeometry)
{
    if (pRobot == 0) {
        assert_prog(D_00A51BB0, D_00A51840, 4142);
    }
    _BodySetEyeGeom(pRobot->body, eyeGeometry);
}

static void _BodySetAdvanceGeom(RgBody *body, RgGeomPoint *advanceGeometry);

void RgRobotSetAdvanceGeom(RgStatus *pRobot, RgGeomPoint *advanceGeometry)
{
    if (pRobot == 0) {
        assert_prog(D_00A51BB0, D_00A51840, 4152);
    }
    _BodySetAdvanceGeom(pRobot->body, advanceGeometry);
}

void RgRobotSetSoundDriver(RgStatus *pRobot, void *soundDriver)
{
    if (pRobot == 0) {
        assert_prog(D_00A51BB0, D_00A51840, 4161);
    }
    _BodySetSound(pRobot->body, soundDriver);
}

static void _BodySetActor(RgBody *body, void *actor);

void RgRobotSetActor(RgStatus *pRobot, void *actor)
{
    if (pRobot == 0) {
        assert_prog(D_00A51BB0, D_00A51840, 4169);
    }
    _BodySetActor(pRobot->body, actor);
}

void RgRobotSetTarget(RgStatus *pRobot, void *target)
{
    if (pRobot == 0) {
        assert_prog(D_00A51BB0, D_00A51840, 4177);
    }
    _BodySetTarget(pRobot->body, target);
}

void RgRobotSetRgDrawView(RgStatus *pRobot, RgDrawView *view)
{
    if (pRobot == 0) {
        assert_prog(D_00A51BB0, D_00A51840, 4187);
    }
    pRobot->drawView = view;
}

void RgRobotSetMotSmooth(RgStatus *pRobot, int motSmooth)
{
    if (pRobot == 0) {
        assert_prog(D_00A51BB0, D_00A51840, 4195);
    }
    pRobot->body->motSmooth = motSmooth;
}

void RgRobotSetConfuse(RgStatus *pRobot, float confuse)
{
    if (pRobot == 0) {
        assert_prog(D_00A51BB0, D_00A51840, 4203);
    }
    pRobot->body->confuse = confuse;
}

void RgRobotSetWeapon(RgStatus *pRobot, int eSide, int weaponID)
{
    if (pRobot == 0) {
        assert_prog(D_00A51BB0, D_00A51840, 4217);
    }
    _BodyEquipWeapon(pRobot->body, eSide, weaponID);
}

void RgRobotSetSpareWeapon(RgStatus *pRobot, int eSide, int weaponID)
{
    if (pRobot == 0) {
        assert_prog(D_00A51BB0, D_00A51840, 4227);
    }
    _BodySetSpareWeapon(pRobot->body, eSide, weaponID);
}

RgGeomPoint *RgRobotGetGeom(RgStatus *pRobot)
{
    if (pRobot == 0) {
        assert_prog(D_00A51BB0, D_00A51840, 4239);
    }
    return pRobot->body->geometry;
}

void *RgRobotGetWeapon(RgStatus *pRobot, unsigned int eSide)
{
    if (pRobot == 0) {
        assert_prog(D_00A51BB0, D_00A51840, 4247);
    }
    if (eSide >= RG_EQUIP_TYPE_NUM) {
        assert_prog(D_00A51BE0, D_00A51840, 4248);
    }
    return pRobot->body->weapon[eSide];
}

void *RgRobotGetTarget(RgStatus *pRobot)
{
    if (pRobot == 0) {
        assert_prog(D_00A51BB0, D_00A51840, 4256);
    }
    return pRobot->body->target;
}

int RgRobotIsDead(RgStatus *pRobot)
{
    if (pRobot == 0) {
        assert_prog(D_00A51BB0, D_00A51840, 4264);
    }
    return pRobot->body->life <= 0.0f;
}

float RgRobotGetLife(RgStatus *pRobot)
{
    if (pRobot == 0) {
        assert_prog(D_00A51BB0, D_00A51840, 4272);
    }
    return pRobot->body->life;
}

float RgRobotGetLifeMax(RgStatus *pRobot)
{
    if (pRobot == 0) {
        assert_prog(D_00A51BB0, D_00A51840, 4278);
    }
    return pRobot->body->lifeMax;
}

void *RgRobotGetActor(RgStatus *pRobot)
{
    if (pRobot == 0) {
        assert_prog(D_00A51BB0, D_00A51840, 4286);
    }
    return pRobot->body->actor;
}

int RgRobotGetCharID(RgStatus *pRobot)
{
    if (pRobot == 0) {
        assert_prog(D_00A51BB0, D_00A51840, 4294);
    }
    return pRobot->body->charID;
}

int RgRobotIsInvalidAttack(RgStatus *pRobot, RgVector direction)
{
    float *shield;

    if (pRobot == 0) {
        assert_prog(D_00A51BB0, D_00A51840, 0x10CF);
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
        assert_prog(D_00A51BB0, D_00A51840, 4321);
    }
    return pRobot->body->dashTime;
}

void *RgRobotGetSpec(RgStatus *pRobot)
{
    if (pRobot == 0) {
        assert_prog(D_00A51BB0, D_00A51840, 4329);
    }
    return pRobot->body->spec;
}

int RgRobotIsConfused(RgStatus *pRobot)
{
    if (pRobot == 0) {
        assert_prog(D_00A51BB0, D_00A51840, 4337);
    }
    if (pRobot->body->confuse > 0.0f)
        return 1;
    return 0;
}

int RgRobotIsAcceptedCommand(RgStatus *pRobot, int command)
{
    if (pRobot == 0) {
        assert_prog(D_00A51BB0, D_00A51840, 4348);
    }
    return (pRobot->acceptedCommandMask & (1 << command)) != 0;
}

unsigned int RgRobotGetStatusFlags(RgStatus *pRobot)
{
    if (pRobot == 0) {
        assert_prog(D_00A51BB0, D_00A51840, 4356);
    }
    return pRobot->statusFlags;
}

void RgRobotAccelarate(RgStatus *pRobot, RgVector velocity)
{
    RgCmd *command;

    if (pRobot == 0) {
        assert_prog(D_00A51BB0, D_00A51840, 4370);
    }
    command = _EntryCmdQueue(pRobot->cmdQueue, 0);
    if (command != 0) {
        XrgCopyVector((float *) command, velocity);
    }
}

void RgRobotAccelarateRotate(RgStatus *pRobot, float rotate)
{
    RgCmd *command;

    if (pRobot == 0) {
        assert_prog(D_00A51BB0, D_00A51840, 4382);
    }
    command = _EntryCmdQueue(pRobot->cmdQueue, 1);
    if (command != 0) {
        *(float *) command = rotate;
    }
}

/*
 * ov12:0x00a51a88 contains the range expression
 * "RG_EQUIP_TYPE_MIN <= (eType) && (eType) < RG_EQUIP_TYPE_NUM".
 */
extern const char D_00A51A88[];

void RgRobotShot(RgStatus *pRobot, unsigned int eType)
{
    RgCmd *command;

    if (pRobot == 0) {
        assert_prog(D_00A51BB0, D_00A51840, 4394);
    }
    if (eType >= RG_EQUIP_TYPE_NUM) {
        assert_prog(D_00A51A88, D_00A51840, 4395);
    }
    command = _EntryCmdQueue(pRobot->cmdQueue, 2);
    if (command != 0) {
        command->param = eType;
    }
}

void RgRobotTargetting(RgStatus *pRobot)
{
    RgCmd *command;

    if (pRobot == 0) {
        assert_prog(D_00A51BB0, D_00A51840, 4407);
    }
    command = _EntryCmdQueue(pRobot->cmdQueue, 3);
    if (command != 0) {
        command->param = (int) pRobot->body->target;
    }
}

void RgRobotBreak(RgStatus *pRobot)
{
    if (pRobot == 0) {
        assert_prog(D_00A51BB0, D_00A51840, 4419);
    }
    _EntryCmdQueue(pRobot->cmdQueue, 4); /* command type 4: break */
}

void RgRobotDash(RgStatus *pRobot, RgVector direction)
{
    RgCmd *command;

    if (pRobot == 0) {
        assert_prog(D_00A51BB0, D_00A51840, 4428);
    }
    command = _EntryCmdQueue(pRobot->cmdQueue, 5);
    if (command != 0) {
        XrgCopyVector((float *) command, direction);
    }
}

void RgRobotDashContinue(RgStatus *pRobot)
{
    if (pRobot == 0) {
        assert_prog(D_00A51BB0, D_00A51840, 4440);
    }
    _EntryCmdQueue(pRobot->cmdQueue, 6); /* command type 6: continue dash */
}

void RgRobotGiveDamage(RgStatus *pRobot, RgVector direction, int damageType,
                       float damage)
{
    RgCmd *command;

    if (pRobot == 0) {
        assert_prog(D_00A51BB0, D_00A51840, 4449);
    }
    command = _EntryCmdQueue(pRobot->cmdQueue, 8);
    if (command != 0) {
        command->scratch0 = damage;
        command->scratch1 = damageType;
        XrgCopyVector((float *) command, direction);
    }
}

void RgRobotGiveWeakDamage(RgStatus *pRobot, RgVector direction, float damage,
                           float threshold, float windowSeed,
                           float weakHitMotion, float weakHitPower)
{
    RgCmd *command;

    if (pRobot == 0) {
        assert_prog(D_00A51BB0, D_00A51840, 0x117A);
    }
    command = _EntryCmdQueue(pRobot->cmdQueue, 9);
    if (command != 0) {
        command->scratch0 = damage;
        XrgCopyVector((float *) command, direction);
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
        assert_prog(D_00A51BB0, D_00A51840, 0x118C);
    }
    command = _EntryCmdQueue(pRobot->cmdQueue, 0xE);
    if (command != 0) {
        XrgCopyVector(&command->scratch0, direction);
        XrgCopyVector((float *) command, position);
        command->scratch4 = damage;
        command->scratch5 = scale;
    }
}

void RgRobotDropWeapon(RgStatus *pRobot, unsigned int eType)
{
    RgCmd *command;

    if (pRobot == 0) {
        assert_prog(D_00A51BB0, D_00A51840, 4508);
    }
    if (eType >= RG_EQUIP_TYPE_NUM) {
        assert_prog(D_00A51A88, D_00A51840, 4509);
    }
    command = _EntryCmdQueue(pRobot->cmdQueue, 7);
    if (command != 0) {
        command->param = eType;
    }
}

void RgRobotHitWeaponAttack(RgStatus *pRobot, int param)
{
    RgCmd *command;

    if (pRobot == 0) {
        assert_prog(D_00A51BB0, D_00A51840, 4522);
    }
    command = _EntryCmdQueue(pRobot->cmdQueue, 0xA);
    if (command != 0) {
        command->param = param;
    }
}

void RgRobotInvalidAttack(RgStatus *pRobot, RgVector direction, float scale)
{
    RgCmd *command;

    if (pRobot == 0) {
        assert_prog(D_00A51BB0, D_00A51840, 4536);
    }
    command = _EntryCmdQueue(pRobot->cmdQueue, 0xB);
    if (command != 0) {
        XrgNormalizeVector((float *) command, direction);
        command->scratch0 = scale;
    }
}

void RgRobotHitBG(RgStatus *pRobot, RgVector direction)
{
    RgCmd *command;

    if (pRobot == 0) {
        assert_prog(D_00A51BB0, D_00A51840, 4550);
    }
    command = _EntryCmdQueue(pRobot->cmdQueue, 0xC);
    if (command != 0) {
        XrgCopyVector((float *) command, direction);
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
        assert_prog(D_00A51BB0, D_00A51840, 4568);
    }
    command = _EntryCmdQueue(pRobot->cmdQueue, 0xD);
    if (command != 0) {
        XrgCopyVector((float *) command, direction);
    }
}
