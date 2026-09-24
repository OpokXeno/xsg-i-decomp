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

static int _ExecDamageCmd(RgRobotStatus *status, RgBody *body, RgCmd *command);
static int _ExecWeakDamageCmd(RgRobotStatus *status, RgBody *body, RgCmd *command);

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

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _MovingPassTime);

static int _AllowAllCmdInMoving(RgRobotStatus *status, int param, RgBody *body);
static int _MovingPassTime(RgRobotStatus *status, RgBody *body, float deltaTime);

static void _InitMovingStatus(RgRobotStatus *status, RgBody *body)
{
    _SetStatus(status, 1, body);
    status->execCmdMethod = _AllowAllCmdInMoving;
    status->passTimeMethod = _MovingPassTime;
    status->scratch0 = -1;
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _ExecCmdInAttack);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _AttackPassTime);

extern void RgWeaponShotStop(void *weapon);
extern char *RgWeaponGetEss(void *weapon);
extern void RgWeaponPlayMotion(void *weapon, int motion);
extern void RgWeaponRestartLockon(void *weapon);

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

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _InitAttackStatus);

static int _AllowDamageCmd(RgRobotStatus *status, RgBody *body, RgCmd *command);

static int _ExecCmdInHissatu(RgRobotStatus *status, RgBody *body, RgCmd *command)
{
    return _AllowDamageCmd(status, body, command);
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

static void _BodyPlayMotion(RgBody *body, int motion);

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

extern void RgGeomPointSetMaxXYSpd(RgGeomPoint *point, float maxSpeed);
extern void RgRobotEffectTermJet(RgRobotEffect *effect);
extern void RgRobotEffectTermDash(RgRobotEffect *effect);
extern void XrgSoundRingStopMoving(int soundID);

static void _DashVRExit(RgRobotStatus *status, RgBody *body)
{
    (void)status;
    RgGeomPointSetMaxXYSpd(body->geometry, _CalcMaxSpeed(body, body->spec->baseSpeed));
    RgRobotEffectTermJet(body->effect);
    RgRobotEffectTermDash(body->effect);
    XrgSoundRingStopMoving(body->landingSoundID);
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _InitDashVRStatus);

static int _AllowDamageCmdNotVRDashHit(RgRobotStatus *status, RgBody *body, RgCmd *command);
static int _ExecDashCmd(RgRobotStatus *status, RgBody *body, RgCmd *command);

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

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _DamagePassTime);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _InitDamageStatus);

static int _AllowDamageCmd(RgRobotStatus *status, RgBody *body, RgCmd *command);
static int _GetDropMotion(int eSide);
static int _InitDropStatus(RgStatus *status, RgBody *body, int motion, int eSide);

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
        _InitDropStatus((RgStatus *)status, body, _GetDropMotion(2), 2);
    }
    return mask;
}

extern int RgMotionInfoGetActionInTime(int motion, int actionIndex,
                                       float elapsedTime);
static void _InitMovingStatus(RgRobotStatus *status, RgBody *body);

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

extern void RgGeomRobotCalcLocal(RgGeomPoint *geometry, RgVector local[4]);
extern void RgRobSubAcceralate(RgGeomPoint *geometry, RgVector direction,
                               float scale);
extern void XrgNegateVector(RgVector destination, RgVector source);

static void _BodyAdvanceForAttack(RgBody *body, float attackPower,
                                  float attackFactor)
{
    RgGeomPoint *geometry;
    RgRobotSpec *spec;
    RgVector direction;
    RgVector localFrame[4];

    geometry = body->geometry;
    spec = body->spec;
    RgGeomRobotCalcLocal(geometry, localFrame);
    XrgNegateVector(direction, localFrame[2]);
    RgRobSubAcceralate(geometry, direction,
                       spec->accelRate * spec->attackAdvanceRate * 0.3f * attackFactor);
}

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

extern void RgGeomFree(RgGeomPoint *point);
extern void RgGeomPointSetMoveResist(RgGeomPoint *point, float resist);
extern void RgGeomPointSetWeight(RgGeomPoint *point, float weight);
extern float RgGeomRobotGetRotate(RgGeomPoint *point);
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
    RgGeomPointSetWeight(geometry, spec->speedRating);
    RgGeomPointSetMoveResist(geometry, spec->moveResist);
    RgGeomRobotSetRotResist(geometry, spec->rotResist);
    body->rotate = RgGeomRobotGetRotate(geometry);
    RgRobotEffectSetGeom(body->effect, geometry);
    RgGeomPointSetMaxXYSpd(body->geometry, _CalcMaxSpeed(body, spec->baseSpeed));
}

extern void RgGeomFree(RgGeomPoint *point);

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
extern const char D_00A51A78[];

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

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _BodySetSound);

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
    return _StatusExecCmd(robotStatus, (RgBody *) command, (int) body);
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

extern int RgGeomGetEventFlag(RgGeom *geom);
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

extern void RgGeomPointGetVel(RgGeomPoint *point, RgVector velocity);
extern void RgGeomPointSetPos(RgGeomPoint *point, RgVector position);
extern void RgGeomPointMovePos(RgGeomPoint *point, RgVector position);
extern void XrgAddVector(RgVector destination, RgVector first, RgVector second);
extern void XrgScaleVector(RgVector destination, RgVector source, float scale);

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

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _BodyAutoHomingPassTime);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _BodyPassTime);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _BodyDisp);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _InitBody);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _DestructBody);

static void _InitBody(RgBody *body);

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

extern void RgGeomPointSetMaxXYSpd(RgGeomPoint *point, float maxSpeed);
extern void XrgSoundRingMoving(int soundID, int unused, float volume);

static int _ExecAccelarateCmd(RgStatus *status, RgBody *body, RgCmd *command)
{
    RgRobotSpec *spec;
    RgGeomPoint *geometry;

    spec = body->spec;
    geometry = body->geometry;
    RgGeomPointSetMaxXYSpd(geometry, _CalcMaxSpeed(body, spec->baseSpeed));
    RgRobSubAcceralate(geometry, (float *) command, spec->accelRate);
    XrgSoundRingMoving(body->landingSoundID, 0, 0.2f);
    return 1;
}

extern void RgGeomRobotAddRotForce(RgGeomPoint *point, float force);

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

static int _InitDashVRStatus(RgRobotStatus *status, RgBody *body, RgCmd *command,
                             int param1, int param2);

static int _ExecDashContinueCmd(RgRobotStatus *status, RgBody *body, RgCmd *command)
{
    int result;

    result = 0;
    if (body->spec->type == 1) {
        _InitDashVRStatus(status, body, command, 1, 0);
        result = 4;
    }
    return result;
}

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

static int _InitDropStatus(RgStatus *status, RgBody *body, int motion,
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

extern void CreateRgHitEffectPosDir(RgVector position, RgVector direction,
                                    char *name1, char *name2);
extern void RgGeomPointAddForce(RgGeomPoint *point, RgVector force);
extern float XrgNormalizeVector(RgVector destination, RgVector source);
static void _InitDamageStatus(RgRobotStatus *status, RgBody *body,
                              RgCmd *command, int flag, float p5, float p6);
extern unsigned char D_00A51BA0[];

static int _ExecHitByBody(RgRobotStatus *status, RgBody *body, RgCmd *command)
{
    RgVector force;

    CreateRgHitEffectPosDir(&command->scratch0, (float *) command,
                            D_00A51BA0, 0);
    XrgNormalizeVector(force, (float *) command);
    XrgScaleVector(force, force, command->scratch5);
    RgGeomPointAddForce(body->geometry, force);
    body->life -= command->scratch4;
    if (body->life < 0.0f) {
        body->life = 0.0f;
    }
    _InitDamageStatus(status, body, command, 1, 0.0f, 0.5f);
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
        _InitDamageStatus(status, body, command, 1, accum, 0.0f);
        body->weakHitWindow = 0.0f;
        body->weakHitAccum = 0.0f;
    } else {
        body->weakHitMotion = command->scratch3;
        body->weakHitPower = command->scratch4;
    }
    return 0x80;
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _DullFlagsPassTime);

extern int RgRobotIsDead(RgStatus *pRobot);
static int _BodyExecCmd(RgRobotStatus *robotStatus, RgBody *body,
                         RgCmd *command);
static int _BodyPassTime(RgRobotStatus *robotStatus, RgBody *body,
                          float deltaTime);
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

static void _BodyDisp(RgRobotStatus *robotStatus, RgBody *body);

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

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _InitRobot);

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

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", RgRobotPassTime);

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

static void _BodySetSound(RgBody *body, void *soundDriver);

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

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", RgRobotAccelarateRotate);

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

extern float XrgNormalizeVector(RgVector destination, RgVector source);

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
