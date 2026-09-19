/*
 * OV12 original TU 4: 0x00a02510..0x00a09480 (153 functions)
 */
#include "common.h"
#include "shared.h"
#include "rg_robot.h"

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

static void _InitHomingThread(RgHomingThread *thread);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _InitHomingThread);

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

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _PassTimeHomingThread);

static float _CalcMaxSpeed(RgBody *body, float baseSpeed)
{
    float speedRating;
    float weight;

    speedRating = body->spec->speedRating;
    weight = RgGeomPointGetWeight(body->geometry);
    if (!(baseSpeed > 0.0f)) {
        assert_prog(D_00A51858, D_00A51840, 247);
    }
    return (baseSpeed * speedRating) / weight;
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _StopAllShotThread);

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

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _CreateStatue);

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

static int _StatusExecCmd(RgRobotStatus *status, RgBody *body, int param)
{
    RgStatusExecCmdFunc execCmdMethod;

    if (status == 0) {
        assert_prog(D_00A518F0, D_00A51840, 657);
    }
    execCmdMethod = status->execCmdMethod;
    if (execCmdMethod != 0) {
        return execCmdMethod(status, param, body);
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

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _AllowAllCmdInMoving);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _AllowDamageCmd);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _AllowDamageCmdNotVRDashHit);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _AllowDamageCmdNotPetrify);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _MovingPassTime);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _InitMovingStatus);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _ExecCmdInAttack);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _AttackPassTime);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _AttackExit);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _InitAttackStatus);

extern int _AllowDamageCmd(void);

static void _ExecCmdInHissatu(void)
{
    _AllowDamageCmd();
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _HissatuPassTime);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _HissatuExit);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _InitStatusHissatu);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _ExecCmdInShield);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _ShieldPassTime);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _ShieldExit);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _InitShieldStatus);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _ExecCmdInTargetting);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _TargettingPassTime);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _ExitTargettingStatus);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _InitTargettingStatus);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _ExecCmdInBreaking);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _BreakingPassTime);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _BreakingExit);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _InitBreakingStatus);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _DashExecCmd);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _DashPassTime);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _DashExit);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _InitDashStatus);

extern void _BodyPlayMotion(RgBody *body, int motion);

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

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _DashVRExecCmd);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _DashVRPassTime);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _DashVRExit);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _InitDashVRStatus);

extern int _AllowDamageCmdNotVRDashHit(void);
extern int _ExecDashCmd(RgRobotStatus *status, RgBody *body, RgCmd *command);

/*
 * Damage status's own scratch1 (see RgRobotStatus) is nonzero while a
 * dash-escape is still allowed, gating whether command type 5 (dash) is
 * forwarded to _ExecDashCmd.
 */
static int _DamageExecCmd(RgRobotStatus *status, RgBody *body, RgCmd *command)
{
    int result;

    result = _AllowDamageCmdNotVRDashHit();
    if (result == 0) {
        if (status->scratch1 != 0 && command->type == 5) {
            result = _ExecDashCmd(status, body, command);
        }
        return result;
    }
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _DamagePassTime);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _InitDamageStatus);

extern int _AllowDamageCmd(void);
static int _GetDropMotion(int eSide);
int _InitDropStatus(RgStatus *status, RgBody *body, int motion, int eSide);

/*
 * Drop status's own scratch0/scratch2 (see RgRobotStatus): scratch0 is the
 * equip side already being dropped and scratch2 is nonzero while that
 * side's spare weapon still needs removing (see _DropPassTime, which also
 * reads and clears it).
 */
static int _ExecCmdInDrop(RgRobotStatus *status, RgBody *body, RgCmd *command)
{
    int mask;

    mask = _AllowDamageCmd();
    if (command->type == 7
        && command->param != status->scratch0
        && command->param == 2
        && status->scratch2 != 0) {
        mask |= 0x200;
        _InitDropStatus((RgStatus *)status, body, _GetDropMotion(2), 2);
    }
    return mask;
}

extern int RgMotionInfoGetActionInTime(int motion, int actionIndex,
                                       float elapsedTime);
extern void _InitMovingStatus(RgRobotStatus *status, RgBody *body);

/*
 * Drop status's own scratch1 (see RgRobotStatus) is the drop motion id
 * (see _GetDropMotion) checked against elapsedTime, and scratch0/scratch2
 * are the same equip side and pending flag _ExecCmdInDrop evidences. Once
 * the drop motion's timed removal point is reached, this clears the
 * body's spare weapon at that side (see _BodySetSpareWeapon, RgBody+0x7c)
 * by handing it to _BodyEquipWeapon.
 */
static int _DropPassTime(RgRobotStatus *status, RgBody *body)
{
    float elapsedTime;
    int side;

    elapsedTime = status->elapsedTime;
    if (RgMotionInfoGetActionInTime(status->scratch1, 0, elapsedTime) != 0
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

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _InitDropStatus);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _EntryCmdQueue);

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

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _CreateCmdQueue);

static void _DisposeCmdQueue(RgCmdQueue *cmdQueue)
{
    if (cmdQueue == 0) {
        assert_prog(D_00A51A58, D_00A51840, 2697);
    }
    RgHeapFree(InstanceOfRgHeap(), cmdQueue, D_00A51840, 2698);
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _BodyPlayMotion);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _BodyPlayMotionLoop);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _BodyAdvanceForAttack);

/* ov12:0x00a51a68 contains the assertion expression "pBody != NIL". */
extern const char D_00A51A68[];

/* Opaque here: rg_geom_point.c (ov12/tu052) owns the RgPointVector definition. */
typedef struct RgPointVector RgPointVector;
extern void __RgGeomPointGetPos(RgGeomPoint *point, RgPointVector *destination,
                                const char *source_file, int source_line);

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

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _BodySetSpec);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _BodySetGeom);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _BodySetEyeGeom);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _BodySetAdvanceGeom);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _BodySetSound);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _BodySetActor);

static void _BodySetTarget(RgBody *body, void *target)
{
    if (body == 0) {
        assert_prog(D_00A51A68, D_00A51840, 2890);
    }
    body->target = target;
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _BodyDropWeapon);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _BodyEquipWeapon);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _BodySetSpareWeapon);

static void _BodySetAutoHomingEnv(RgBody *body, int autoHomingEnv)
{
    if (body == 0) {
        assert_prog(D_00A51A68, D_00A51840, 3142);
    }
    body->autoHomingEnv = autoHomingEnv;
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _BodyExecCmd);

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
    maxSpeed = _CalcMaxSpeed(body, spec->baseSpeed);
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

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _EyeGeomPassTime);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _AdvanceGeomPassTime);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _BodyAutoHomingPassTime);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _BodyPassTime);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _BodyDisp);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _InitBody);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _DestructBody);

void _InitBody(RgBody *body);

static RgBody *_CreateBody(void)
{
    RgBody *body;

    body = RgHeapAlloc(InstanceOfRgHeap(), sizeof(RgBody), D_00A51840, 3482);
    _InitBody(body);
    return body;
}

extern int RgHeapIsInvalidMemory(RgHeap *heap, void *ptr);
extern void RgError(const char *message, const char *source_file, int line,
                     ...);
void _DestructBody(RgBody *body);

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

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _ExecAccelarateCmd);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _ExecRotateCmd);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _IsBackWeaponReady);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _ExecShotOrAttackCmd);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _ExecTargettingCmd);

static int _ExecBreakCmd(RgStatus *status, RgBody *body, void *command)
{
    float brake_scale = 0.30000001192092895508f;

    (void)command;
    RgRobSubBreak(body->geometry, brake_scale, brake_scale);
    _InitBreakingStatus(status, body);
    return 0x40;
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _ExecDashCmd);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _ExecDashContinueCmd);

/* The format string "unknown equip type (drop weapon) %d". */
extern const char D_00A51B78[];

extern void RgError(const char *message, const char *source_file, int line,
                     ...);

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

int _InitDropStatus(RgStatus *status, RgBody *body, int motion,
                            int eSide);

static int _ExecDropWeaponCmd(RgStatus *status, RgBody *body,
                               RgCmd *command)
{
    return (_InitDropStatus(status, body, _GetDropMotion(command->param),
                             command->param) == 0)
               ? 0
               : 0x200;
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _ExecDamageCmd);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _ExecHitByBody);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _ExecWeakDamageCmd);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _DullFlagsPassTime);

extern int RgRobotIsDead(RgStatus *pRobot);
int _BodyExecCmd(RgRobotStatus *robotStatus, RgBody *body,
                         RgCmd *command);
int _BodyPassTime(RgRobotStatus *robotStatus, RgBody *body,
                          float deltaTime);
void _ClearCmdQueue(RgCmdQueue *cmdQueue);
void _DullFlagsPassTime(RgStatus *pRobot, float deltaTime);

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

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _DispRobot);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _DestructRobot);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _InitRobot);

void _InitRobot(RgStatus *pRobot);

RgStatus *CreateRgRobot(void)
{
    RgStatus *pRobot;

    pRobot = RgHeapAlloc(InstanceOfRgHeap(), sizeof(RgStatus), D_00A51840,
                          4041);
    _InitRobot(pRobot);
    return pRobot;
}

void _DestructRobot(RgStatus *pRobot);

void DisposeRgRobot(RgStatus *pRobot)
{
    if (pRobot == 0) {
        assert_prog(D_00A51BB0, D_00A51840, 4049);
    }
    _DestructRobot(pRobot);
    RgHeapFree(InstanceOfRgHeap(), pRobot, D_00A51840, 4051);
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", RgRobotPassTime);

void _DispRobot(RgStatus *pRobot);

void RgRobotDisp(RgStatus *pRobot)
{
    if (pRobot == 0) {
        assert_prog(D_00A51BB0, D_00A51840, 4109);
    }
    _DispRobot(pRobot);
}

void _BodySetSpec(RgBody *body, RgRobotSpec *spec);

void RgRobotSetSpec(RgStatus *pRobot, RgRobotSpec *spec)
{
    if (pRobot == 0) {
        assert_prog(D_00A51BB0, D_00A51840, 4124);
    }
    _BodySetSpec(pRobot->body, spec);
}

void _BodySetGeom(RgBody *body, RgGeomPoint *geometry);

void RgRobotSetGeom(RgStatus *pRobot, RgGeomPoint *geometry)
{
    if (pRobot == 0) {
        assert_prog(D_00A51BB0, D_00A51840, 4133);
    }
    _BodySetGeom(pRobot->body, geometry);
}

void _BodySetEyeGeom(RgBody *body, RgGeomPoint *eyeGeometry);

void RgRobotSetEyeGeom(RgStatus *pRobot, RgGeomPoint *eyeGeometry)
{
    if (pRobot == 0) {
        assert_prog(D_00A51BB0, D_00A51840, 4142);
    }
    _BodySetEyeGeom(pRobot->body, eyeGeometry);
}

void _BodySetAdvanceGeom(RgBody *body, RgGeomPoint *advanceGeometry);

void RgRobotSetAdvanceGeom(RgStatus *pRobot, RgGeomPoint *advanceGeometry)
{
    if (pRobot == 0) {
        assert_prog(D_00A51BB0, D_00A51840, 4152);
    }
    _BodySetAdvanceGeom(pRobot->body, advanceGeometry);
}

void _BodySetSound(RgBody *body, void *soundDriver);

void RgRobotSetSoundDriver(RgStatus *pRobot, void *soundDriver)
{
    if (pRobot == 0) {
        assert_prog(D_00A51BB0, D_00A51840, 4161);
    }
    _BodySetSound(pRobot->body, soundDriver);
}

void _BodySetActor(RgBody *body, void *actor);

void RgRobotSetActor(RgStatus *pRobot, void *actor)
{
    if (pRobot == 0) {
        assert_prog(D_00A51BB0, D_00A51840, 4169);
    }
    _BodySetActor(pRobot->body, actor);
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", RgRobotSetTarget);

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

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", RgRobotIsInvalidAttack);

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

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", RgRobotIsConfused);

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

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", RgRobotAccelarate);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", RgRobotAccelarateRotate);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", RgRobotShot);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", RgRobotTargetting);

void RgRobotBreak(RgStatus *pRobot)
{
    if (pRobot == 0) {
        assert_prog(D_00A51BB0, D_00A51840, 4419);
    }
    _EntryCmdQueue(pRobot->cmdQueue, 4); /* command type 4: break */
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", RgRobotDash);

void RgRobotDashContinue(RgStatus *pRobot)
{
    if (pRobot == 0) {
        assert_prog(D_00A51BB0, D_00A51840, 4440);
    }
    _EntryCmdQueue(pRobot->cmdQueue, 6); /* command type 6: continue dash */
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", RgRobotGiveDamage);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", RgRobotGiveWeakDamage);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", RgRobotHitByBody);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", RgRobotDropWeapon);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", RgRobotHitWeaponAttack);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", RgRobotInvalidAttack);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", RgRobotHitBG);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", RgRobotNearBG);
