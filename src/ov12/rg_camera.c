/*
 * OV12 original TU 16: 0x00a10f20..0x00a129c8 (43 functions)
 */
#include "common.h"
#include "shared.h"
#include "ov12/assert_prog.h"
#include "ov12/rg_camera.h"
#include "ov12/rg_char.h"
#include "ov12/rg_draw.h"
#include "ov12/rg_error.h"
#include "ov12/rg_geom.h"
#include "ov12/rg_geom_point.h"
#include "ov12/rg_geom_robot.h"
#include "ov12/rg_heap.h"
#include "ov12/rg_text_info.h"
#include "ov12/xrg_pad.h"
#include "ov12/xrg_rand_int.h"
#include "ov12/xrg_v_log.h"

/* Follow distances RgCameraSetFarMode and the SELECT toggle switch between. */
#define RG_CAMERA_NEAR_DISTANCE 8.0f
#define RG_CAMERA_FAR_DISTANCE 14.0f
#define RG_CAMERA_FAR_THRESHOLD 10.0f

/* Largest sideways drift per frame of the camera yaw (pi / 80). */
#define RG_CAMERA_TURN_STEP 0.039269908f

#define offsetof(type, member) ((unsigned int) &((type *) 0)->member)

extern const char cameraDumpRollText[];
extern const char cameraDumpAdvanceText[];
extern const char cameraDumpAttackText[];
extern const char cameraDumpLockAttackText[];
extern const char cameraDumpDashText[];
extern const char cameraDumpInScreenText[];
extern const char cameraDumpLockText[];
extern const char cameraDumpSeeTargetText[];
extern const char cameraDumpAllTrueText[];
extern const char cameraDumpAllFalseText[];
extern const char cameraTextKeyBehindOffset[];
extern const char cameraTextKeyLookDown[];
extern const char cameraTextKeyUpperOffset[];
extern const char cameraTextKeyRollCoef[];
extern const char cameraTextKeyUseMoveDir[];
extern const char cameraTextKeyRollTime[];
extern const char cameraTextKeyCondition1[];
extern const char cameraTextKeyCondition2[];
extern const char cameraTextKeyCondition3[];
extern const char cameraTextKeyCondition4[];
extern const char cameraTextKeyOperator1[];
extern const char cameraTextKeyOperator2[];
extern const char cameraTextKeySubslideStart[];
extern const char cameraTextKeySubslideDegree[];
extern const char cameraTextKeySubslideLength[];
extern const char cameraTextKeySubslideCoef[];
extern const char cameraOperatorAndText[];
extern const char cameraOperatorOrText[];
extern const char cameraUnknownOperatorFormat[];
extern const char cameraUnknownBoolOperatorFormat[];

static int _SetBit(int *flags, int bit, int value);
static void _PassTimeVersion3(RgCamera *camera, float elapsed);
static int _GetVer3TimerID(char *timerName);
static int (*s_apConditionTbl_0[RG_CAMERA_TIMER_COUNT])(RgCamera *);
static void _SetVersion3Extent(RgCamera *camera);
static void _LoadVersion3(RgCamera *camera, struct RgReadText *reader);

const char cameraAssertCamNotNil[] = "pCam != NIL";

const char cameraSourcePath[] = "../rg_camera.euc.c";

const char cameraAssertStudioNotNil[] = "pStudio != NIL";

static const char *s_aszVer3TimerName[RG_CAMERA_TIMER_COUNT] = {
    cameraDumpRollText, cameraDumpAdvanceText, cameraDumpAttackText, cameraDumpLockAttackText, cameraDumpDashText,
    cameraDumpInScreenText, cameraDumpLockText, cameraDumpSeeTargetText, cameraDumpAllTrueText, cameraDumpAllFalseText
};

const char cameraVersionErrorFormat[] = "camera version error (%d)";

const char cameraAssertZero[] = "0";

const char cameraAssertRgCamNotNil[] = "pRgCam != NIL";

const char cameraAssertEssenceNotNil[] = "pEssence != NIL";

const char cameraAssertReaderNotNil[] = "pReader != NIL";

/* ov12:0x00a52978 "---------- camera dump %p\n" */
const char cameraDumpHeaderFormat[] = "---------- camera dump %p\n";

/* ov12:0x00a52998 "  me=%p enemy=%p\n" */
const char cameraDumpMeEnemyFormat[] = "  me=%p enemy=%p\n";

static int _SetBit(int *flags, int bit, int value)
{
    if (value != 0) {
        *flags |= 1 << bit;
    } else {
        *flags &= ~(1 << bit);
    }
    return *flags;
}

static int _CheckBit(int flags, int bit)
{
    int isSet;

    isSet = flags & (1 << bit);
    return isSet != 0;
}

static void _NonJobMethod(void)
{
}

static void _InitAbstructCamera(RgCamera *camera, RgDrawStudio *studio)
{
    if (camera == 0) {
        assert_prog(cameraAssertCamNotNil, cameraSourcePath, 86);
    }
    if (studio == 0) {
        assert_prog(cameraAssertStudioNotNil, cameraSourcePath, 87);
    }
    camera->studio = studio;
    XrgClearVector(camera->eye);
    /*
     * cc1 2.96 -O2 reorders an unbroken run of independent stores to the
     * same base register by moving the last one to the front; grouping the
     * leading calls and stores in their own scope keeps this in the
     * original order (docs/compiler-patterns.md CP-0234/CP-0242).
     */
    do {
        XrgClearVector(camera->target);
        XrgCopyVector(camera->up, XrgVectorY());
        camera->turnRate = 0.0f;
        camera->actionFlags = 0;
        camera->snapToTarget = 1;
        camera->me = 0;
        camera->enemy = 0;
    } while (0);
    camera->setCamera = 0;
    camera->loadText = 0;
}

static void _SetView(RgCamera *camera)
{
    float direction[3];
    RgDrawView *view;
    float pitch;
    float yaw;
    float roll;

    XrgCopyVectorXYZ(direction, camera->target);
    direction[1] = 0.0f;
    pitch = atan2f(camera->target[1], XrgLengthVector(direction));
    yaw = atan2f(direction[0], direction[2]) + XRG_PI;
    if (yaw > XRG_PI) {
        yaw -= XRG_TWO_PI;
        while (yaw > XRG_PI) {
            yaw -= XRG_TWO_PI;
        }
    }
    if (yaw < -XRG_PI) {
        do {
            yaw += XRG_TWO_PI;
        } while (yaw < -XRG_PI);
    }
    roll = camera->turnRate;
    view = RgDrawStudioGetView(camera->studio);
    RgDrawViewSetPosition(view, camera->eye);
    RgDrawViewSetRotateX(view, pitch);
    RgDrawViewSetRotateY(view, yaw);
    RgDrawViewSetRotateZ(view, roll);
}

static int _GetVer3TimerID(char *timerName)
{
    unsigned int timerIndex;

    for (timerIndex = 0; timerIndex < 10; timerIndex++) {
        if (strcmp(s_aszVer3TimerName[timerIndex], timerName) == 0) {
            return timerIndex;
        }
    }
    return -1;
}

static int _IsActionRoll(RgCamera *camera)
{
    return _CheckBit(camera->actionFlags, RG_CAMERA_ACTION_ROLL);
}

static int _IsActionAdvance(RgCamera *camera)
{
    return _CheckBit(camera->actionFlags, RG_CAMERA_ACTION_ADVANCE);
}

static int _IsActionAttack(RgCamera *camera)
{
    return _CheckBit(camera->actionFlags, RG_CAMERA_ACTION_ATTACK);
}

static int _IsActionSeeTarget(RgCamera *camera)
{
    return _CheckBit(camera->actionFlags, RG_CAMERA_ACTION_SEE_TARGET);
}

static int _IsActionLockAndAttack(RgCamera *camera)
{
    int result;

    result = 0;
    if (_CheckBit(camera->actionFlags, RG_CAMERA_ACTION_ATTACK)) {
        result = _CheckBit(camera->actionFlags, RG_CAMERA_ACTION_LOCKED_ON) != 0;
    }
    return result;
}

static int _IsActionLockOn(RgCamera *camera)
{
    return _CheckBit(camera->actionFlags, RG_CAMERA_ACTION_LOCKED_ON);
}

static int _IsActionDash(RgCamera *camera)
{
    return _CheckBit(camera->actionFlags, RG_CAMERA_ACTION_DASH);
}

static void _IsActionTargetInScrn(RgCamera *camera)
{
    RgVector position;

    __RgGeomPointGetPos(camera->enemy, position, cameraSourcePath, 220);
    RgDrawViewIsPointInView(RgDrawStudioGetView(camera->studio), position,
                            0.0f);
}

static int _IsActionTrue(RgCamera *camera)
{
    return 1;
}

static int _IsActionFalse(RgCamera *camera)
{
    return 0;
}

static void _InitVer3Timer(RgVer3Timer *timer, int (*predicate)(RgCamera *), float duration)
{
    timer->predicate = predicate;
    timer->duration = duration;
    timer->remaining = 0.0f;
}

static void _PreJobVer3Timer(RgVer3Timer *timer, RgCamera *camera)
{
    if (timer->predicate != 0 && timer->predicate(camera) != 0) {
        timer->remaining = timer->duration;
    }
}

static void _PastVer3Timer(RgVer3Timer *timer, float elapsed)
{
    float remaining;

    remaining = timer->remaining - elapsed;
    timer->remaining = remaining;
    if (remaining <= 0.0f) {
        timer->remaining = 0.0f;
    }
}

static int _IsVer3TimerActive(RgVer3Timer *timer)
{
    return timer->remaining > 0.0f;
}

static void _SettingVersion3(RgCamera *camera, RgGeomPoint **points)
{
}

static void _SetVersion3Extent(RgCamera *camera)
{
    int timerIndex;
    int three;
    int five;
    float one;
    float ten;
    float twelve;
    float half;
    float pi4;
    float point3;

    XrgClearVector(camera->extent);
    twelve = 12.0f;
    one = 1.0f;
    ten = 10.0f;
    half = 0.5f;
    pi4 = XRG_QUARTER_PI;
    point3 = 0.3f;
    camera->lookDown = one;
    camera->upperOffset = one;
    camera->farDistance = twelve;
    camera->lookDown = one;
    camera->upperOffset = one;
    camera->rollCoef = half;
    camera->selectableFar = -1;
    camera->lockAngle = pi4;
    camera->useMoveDir = 0;
    camera->conditionInvertMask = 0;
    camera->enemyRange = ten;
    camera->pullBack = ten;
    camera->distanceSmoothing = point3;
    camera->distance = 0;
    for (timerIndex = 0; timerIndex < RG_CAMERA_TIMER_COUNT; timerIndex++) {
        _InitVer3Timer(&camera->timers[timerIndex], s_apConditionTbl_0[timerIndex], 0.5f);
    }
    three = 3;
    five = 5;
    do {
        camera->conditionTimer[3] = three;
        camera->conditionTimer[0] = five;
    } while (0);
    do {
        camera->decisionIsAnd[0] = 0;
        camera->decisionIsAnd[1] = 0;
    } while (0);
    do {
        camera->conditionTimer[1] = three;
        camera->conditionTimer[2] = 0;
    } while (0);
}

static void _LoadVersion3(RgCamera *camera, struct RgReadText *reader)
{
    char text[6][64];
    char *operatorText = text[4];
    struct RgTextInfoEntry table[] = {
        { RG_TEXT_INFO_FLOAT_OFFSET, cameraTextKeyBehindOffset, (void *) offsetof(RgCamera, farDistance), 0 },
        { RG_TEXT_INFO_FLOAT_OFFSET, cameraTextKeyLookDown, (void *) offsetof(RgCamera, lookDown), 0 },
        { RG_TEXT_INFO_FLOAT_OFFSET, cameraTextKeyUpperOffset, (void *) offsetof(RgCamera, upperOffset), 0 },
        { RG_TEXT_INFO_FLOAT_OFFSET, cameraTextKeyRollCoef, (void *) offsetof(RgCamera, rollCoef), 0 },
        { RG_TEXT_INFO_BOOL_OFFSET, cameraTextKeyUseMoveDir, (void *) offsetof(RgCamera, useMoveDir), 0 },
        { RG_TEXT_INFO_FLOAT_OFFSET, s_aszVer3TimerName[0], (void *) offsetof(RgCamera, timers[0].duration), 0 },
        { RG_TEXT_INFO_FLOAT_OFFSET, s_aszVer3TimerName[1], (void *) offsetof(RgCamera, timers[1].duration), 0 },
        { RG_TEXT_INFO_FLOAT_OFFSET, s_aszVer3TimerName[2], (void *) offsetof(RgCamera, timers[2].duration), 0 },
        { RG_TEXT_INFO_FLOAT_OFFSET, s_aszVer3TimerName[3], (void *) offsetof(RgCamera, timers[3].duration), 0 },
        { RG_TEXT_INFO_FLOAT_OFFSET, s_aszVer3TimerName[4], (void *) offsetof(RgCamera, timers[4].duration), 0 },
        { RG_TEXT_INFO_FLOAT_OFFSET, s_aszVer3TimerName[5], (void *) offsetof(RgCamera, timers[5].duration), 0 },
        { RG_TEXT_INFO_FLOAT_OFFSET, s_aszVer3TimerName[6], (void *) offsetof(RgCamera, timers[6].duration), 0 },
        { RG_TEXT_INFO_FLOAT_OFFSET, s_aszVer3TimerName[7], (void *) offsetof(RgCamera, timers[7].duration), 0 },
        { RG_TEXT_INFO_FLOAT_OFFSET, cameraTextKeyRollTime, (void *) offsetof(RgCamera, timers[0].duration), 0 },
        { RG_TEXT_INFO_STRING_ADDRESS, cameraTextKeyCondition1, text[0], 0 },
        { RG_TEXT_INFO_STRING_ADDRESS, cameraTextKeyCondition2, text[1], 0 },
        { RG_TEXT_INFO_STRING_ADDRESS, cameraTextKeyCondition3, text[2], 0 },
        { RG_TEXT_INFO_STRING_ADDRESS, cameraTextKeyCondition4, text[3], 0 },
        { RG_TEXT_INFO_STRING_ADDRESS, cameraTextKeyOperator1, text[4], 0 },
        { RG_TEXT_INFO_STRING_ADDRESS, cameraTextKeyOperator2, text[5], 0 },
        { RG_TEXT_INFO_FLOAT_OFFSET, cameraTextKeySubslideStart, (void *) offsetof(RgCamera, enemyRange), 0 },
        { RG_TEXT_INFO_FLOAT_OFFSET, cameraTextKeySubslideDegree, (void *) offsetof(RgCamera, lockAngle), 0 },
        { RG_TEXT_INFO_FLOAT_OFFSET, cameraTextKeySubslideLength, (void *) offsetof(RgCamera, pullBack), 0 },
        { RG_TEXT_INFO_FLOAT_OFFSET, cameraTextKeySubslideCoef, (void *) offsetof(RgCamera, distanceSmoothing), 0 },
        { RG_TEXT_INFO_END }
    };
    int *timerId;
    int *operatorIsAnd;
    int i;
    int id;
    char *name;
    char *operator;

    for (i = 3; i >= 0; i--) {
        text[i][0] = 0;
    }
    operatorText[64] = 0;
    operatorText[0] = 0;
    RgTextInfoRead(reader, table, (char *)camera);

    camera->lockAngle = camera->lockAngle * XRG_PI / 180.0f;

    timerId = camera->conditionTimer;
    for (i = 0; i < 4; i++) {
        name = text[i];

        if (name[0] == '!') {
            name++;
            camera->conditionInvertMask |= 1 << i;
        }
        id = _GetVer3TimerID(name);
        if (id >= 0) {
            timerId[i] = id;
        }
    }

    operatorIsAnd = camera->decisionIsAnd;
    operator = operatorText;
    for (i = 0; i < 2; i++) {
        if (strcmp(operator, cameraOperatorAndText) == 0) {
            *operatorIsAnd = 1;
        } else if (strcmp(operator, cameraOperatorOrText) == 0) {
            *operatorIsAnd = 0;
        } else if (strlen(operator) != 0) {
            RgError(cameraUnknownOperatorFormat, cameraSourcePath, 429, operator);
        }
        operatorIsAnd++;
        operator += 64;
    }
}

static void _PassTimeVersion3(RgCamera *camera, float elapsed)
{
    RgGeomPoint *me;
    RgGeomPoint *enemy;
    RgVector pos;
    RgVector enemyPos;
    RgVector mePos;
    RgVector yAxis;
    RgVector eyeCopy;
    RgVector upCopy;
    RgVector targetCopy;
    int timerActive[10];
    int localFlag[4];
    int outFlag[2];
    int mask;
    RgVector dirVec;
    RgVector delta;
    RgVector vel;
    RgVector outer;
    int prevInt50;
    RgVector *eye;
    RgVector *up;
    RgVector *target;
    RgVer3Timer *timers;
    int i;
    float yaw;
    float desiredYaw;
    float enemyYaw;
    float robotRot;
    float dist;
    float ratio;
    float diff;
    float limit;
    float targetDist;
    float excess;
    float yawDelta;
    float turnRate;
    float smoothVel;
    float smoothedDist;

    me = camera->me;
    enemy = camera->enemy;
    prevInt50 = camera->snapToTarget;
    camera->snapToTarget = 0;

    if (camera->selectableFar >= 0) {
        XrgPadSetID(camera->selectableFar);
        if (XrgPadIsSelect()) {
            if (camera->farDistance >= RG_CAMERA_FAR_THRESHOLD) {
                camera->farDistance = RG_CAMERA_NEAR_DISTANCE;
            } else {
                camera->farDistance = RG_CAMERA_FAR_DISTANCE;
            }
        }
    }

    eye = &camera->eye;
    XrgCopyVector(eyeCopy, *eye);
    up = &camera->up;
    XrgCopyVector(upCopy, *up);
    target = &camera->target;
    XrgCopyVector(targetCopy, *target);
    yaw = atan2f(targetCopy[0], targetCopy[2]);
    smoothVel = camera->turnRate;

    targetDist = camera->farDistance;
    __RgGeomPointGetPos(me, pos, cameraSourcePath, 494);
    if (enemy != 0) {
        __RgGeomPointGetPos(enemy, enemyPos, cameraSourcePath, 496);
    } else {
        XrgClearVector(enemyPos);
    }
    XrgSubVector(delta, enemyPos, pos);
    dist = XrgLengthVector(delta);
    robotRot = RgGeomRobotGetRotate(&me->geom);
    enemyYaw = atan2f(delta[0], delta[2]);

    timers = camera->timers;
    for (i = 0; i < RG_CAMERA_TIMER_COUNT; i++) {
        _PreJobVer3Timer(&timers[i], camera);
        timerActive[i] = _IsVer3TimerActive(&timers[i]);
    }

    mask = camera->conditionInvertMask;
    for (i = 0; i <= 3; i++) {
        int bit = 1 << i;
        int active = timerActive[camera->conditionTimer[i]];

        localFlag[i] = active;
        if (mask & bit) {
            localFlag[i] = (active == 0);
        }
    }

    for (i = 0; i < 2; i++) {
        switch (camera->decisionIsAnd[i]) {
        case 1:
            outFlag[i] = localFlag[i * 2] && localFlag[i * 2 + 1];
            break;
        case 0:
            outFlag[i] = localFlag[i * 2] || localFlag[i * 2 + 1];
            break;
        default:
            RgError(cameraUnknownBoolOperatorFormat, cameraSourcePath, 525, camera->decisionIsAnd[i], i);
            break;
        }
    }

    XrgCopyVector(mePos, pos);
    if (outFlag[0] != 0) {
        if (outFlag[1] != 0) {
            XrgSubVector(dirVec, enemyPos, mePos);
            desiredYaw = atan2f(dirVec[0], dirVec[2]);
        } else {
            desiredYaw = yaw;
        }
    } else if (camera->useMoveDir != 0) {
        RgGeomPointGetVel(me, dirVec);
        desiredYaw = atan2f(dirVec[0], dirVec[2]);
    } else {
        desiredYaw = RgGeomRobotGetRotate(&me->geom);
    }

    XrgCopyVector(yAxis, XrgVectorY());

    ratio = (camera->enemyRange - dist) / camera->enemyRange;
    diff = enemyYaw - robotRot;
    while (diff > XRG_PI) {
        diff -= XRG_TWO_PI;
    }
    while (diff < -XRG_PI) {
        diff += XRG_TWO_PI;
    }
    if (diff < 0.0f) {
        diff = -diff;
    }
    limit = camera->lockAngle;
    limit = limit / (XRG_PI - limit);
    excess = diff - limit;
    if (ratio > 0.0f) {
        if (excess > 0.0f) {
            targetDist = camera->farDistance + camera->pullBack * ratio;
        }
    }

    turnRate = 0.0f;
    if (timerActive[4] != 0) {
        RgGeomPointGetVel(me, vel);
        XrgOuterVector(outer, *target, *up);
        if (0.0f < XrgInnerVector(outer, vel)) {
            turnRate = RG_CAMERA_TURN_STEP;
        } else {
            turnRate = -RG_CAMERA_TURN_STEP;
        }
    }

    if (prevInt50 != 0) {
        camera->distance = targetDist;
        yaw = desiredYaw;
        smoothVel = smoothVel * 0.9f + turnRate * 0.1f;
        eyeCopy[0] = mePos[0] - sinf(yaw) * camera->distance;
        eyeCopy[1] = mePos[1] + camera->upperOffset;
        eyeCopy[2] = mePos[2] - cosf(yaw) * camera->distance;
        XrgCopyVector(upCopy, yAxis);
    } else {
        yawDelta = desiredYaw - yaw;
        while (yawDelta > XRG_PI) {
            yawDelta -= XRG_TWO_PI;
        }
        while (yawDelta < -XRG_PI) {
            yawDelta += XRG_TWO_PI;
        }
        yaw += yawDelta * camera->rollCoef;
        smoothedDist = targetDist * camera->distanceSmoothing
            + camera->distance * (1.0f - camera->distanceSmoothing);
        camera->distance = smoothedDist;
        smoothVel = smoothVel * 0.9f + turnRate * 0.1f;
        eyeCopy[0] = mePos[0] - sinf(yaw) * camera->distance;
        eyeCopy[1] = mePos[1] + camera->upperOffset;
        eyeCopy[2] = mePos[2] - cosf(yaw) * camera->distance;
        XrgCopyVector(upCopy, yAxis);
    }

    delta[0] = sinf(yaw) * camera->farDistance;
    delta[1] = -camera->lookDown;
    delta[2] = cosf(yaw) * camera->farDistance;
    XrgCopyVectorXYZ(*target, delta);
    XrgCopyVectorXYZ(*eye, eyeCopy);
    XrgCopyVectorXYZ(*up, upCopy);
    camera->turnRate = smoothVel;

    _SetView(camera);
    for (i = 0; i < RG_CAMERA_TIMER_COUNT; i++) {
        _PastVer3Timer(&camera->timers[i], elapsed);
    }
    RgCameraResetAction(camera);
}

static void _InitVersion3(RgCamera *camera, RgDrawStudio *studio)
{
    if (camera == 0) {
        assert_prog(cameraAssertCamNotNil, cameraSourcePath, 652);
    }
    _InitAbstructCamera(camera, studio);
    camera->setCamera = _SettingVersion3;
    camera->loadText = _LoadVersion3;
    _SetVersion3Extent(camera);
}

RgCamera *CreateRgCamera(int cameraVersion, RgDrawStudio *studio)
{
    RgCamera *camera;

    camera = 0;
    if (cameraVersion == 0) {
        RgError(cameraVersionErrorFormat, cameraSourcePath, 676, cameraVersion);
    } else if (cameraVersion == 1) {
        RgError(cameraVersionErrorFormat, cameraSourcePath, 679, cameraVersion);
    } else if (cameraVersion == RG_CAMERA_VERSION3) {
        RgCamera *allocatedCamera;

        allocatedCamera = RgHeapAlloc(InstanceOfRgHeap(), sizeof(RgCamera), cameraSourcePath, 683);
        if (allocatedCamera == 0) {
            assert_prog(cameraAssertCamNotNil, cameraSourcePath, 684);
        }
        _InitVersion3(allocatedCamera, studio);
        camera = allocatedCamera;
    } else {
        assert_prog(cameraAssertZero, cameraSourcePath, 689);
    }
    return camera;
}

void DisposeRgCamera(RgCamera *pRgCam)
{
    if (pRgCam == 0) {
        assert_prog(cameraAssertRgCamNotNil, cameraSourcePath, 697);
    }
    RgHeapFree(InstanceOfRgHeap(), pRgCam, cameraSourcePath, 698);
}

void RgCameraSetting(RgCamera *camera, RgGeomPoint **essence)
{
    void (*setCamera)(RgCamera *, RgGeomPoint **);

    if (camera == 0) {
        assert_prog(cameraAssertCamNotNil, cameraSourcePath, 709);
    }
    if (essence == 0) {
        assert_prog(cameraAssertEssenceNotNil, cameraSourcePath, 710);
    }
    setCamera = camera->setCamera;
    camera->me = essence[0];
    camera->enemy = essence[1];
    if (setCamera != 0) {
        setCamera(camera, essence);
    }
    RgDrawViewInit(RgDrawStudioGetView(camera->studio));
}

void RgCameraLoadText(RgCamera *pCam, struct RgReadText *pReader)
{
    void (*loadText)(RgCamera *, struct RgReadText *);

    if (pCam == 0) {
        assert_prog(cameraAssertCamNotNil, cameraSourcePath, 726);
    }
    if (pReader == 0) {
        assert_prog(cameraAssertReaderNotNil, cameraSourcePath, 727);
    }
    loadText = pCam->loadText;
    if (loadText != 0) {
        loadText(pCam, pReader);
    }
}

void RgCameraSetFarMode(RgCamera *pCam, int enable)
{
    if (pCam == 0) {
        assert_prog(cameraAssertCamNotNil, cameraSourcePath, 737);
    }
    if (enable != 0) {
        pCam->farDistance = RG_CAMERA_FAR_DISTANCE;
    } else {
        pCam->farDistance = RG_CAMERA_NEAR_DISTANCE;
    }
}

void RgCameraSelectableFar(RgCamera *pCam, int level)
{
    if (pCam == 0) {
        assert_prog(cameraAssertCamNotNil, cameraSourcePath, 750);
    }
    if (level >= 2) {
        level = -1;
    }
    pCam->selectableFar = level;
}

void RgCameraLocal(RgCamera *pCam, RgMatrix matrix)
{
    if (pCam == 0) {
        assert_prog(cameraAssertCamNotNil, cameraSourcePath, 762);
    }
    RgCalcLocalForChar(matrix, pCam->eye, pCam->target, pCam->up);
}

void RgCameraGetTarget(RgCamera *pCam, RgVector position)
{
    if (pCam == 0) {
        assert_prog(cameraAssertCamNotNil, cameraSourcePath, 771);
    }
    __RgGeomPointGetPos(pCam->me, position, cameraSourcePath, 772);
}

RgDrawStudio *RgCameraGetStudio(RgCamera *pCam)
{
    if (pCam == 0) {
        assert_prog(cameraAssertCamNotNil, cameraSourcePath, 779);
    }
    return pCam->studio;
}

void RgCameraResetAction(RgCamera *pRgCam)
{
    if (pRgCam == 0) {
        assert_prog(cameraAssertRgCamNotNil, cameraSourcePath, 789);
    }
    pRgCam->actionFlags = 0;
}

void RgCameraSetActionLockedOn(RgCamera *pRgCam, int enable)
{
    if (pRgCam == 0) {
        assert_prog(cameraAssertRgCamNotNil, cameraSourcePath, 797);
    }
    _SetBit(&pRgCam->actionFlags, RG_CAMERA_ACTION_LOCKED_ON, enable);
}

void RgCameraSetActionAdvance(RgCamera *pRgCam, int enable)
{
    if (pRgCam == 0) {
        assert_prog(cameraAssertRgCamNotNil, cameraSourcePath, 805);
    }
    _SetBit(&pRgCam->actionFlags, RG_CAMERA_ACTION_ADVANCE, enable);
}

void RgCameraSetActionRoll(RgCamera *pRgCam, int enable)
{
    if (pRgCam == 0) {
        assert_prog(cameraAssertRgCamNotNil, cameraSourcePath, 813);
    }
    _SetBit(&pRgCam->actionFlags, RG_CAMERA_ACTION_ROLL, enable);
}

void RgCameraSetActionDash(RgCamera *pRgCam, int enable)
{
    if (pRgCam == 0) {
        assert_prog(cameraAssertRgCamNotNil, cameraSourcePath, 821);
    }
    _SetBit(&pRgCam->actionFlags, RG_CAMERA_ACTION_DASH, enable);
}

void RgCameraSetActionAttack(RgCamera *pRgCam, int enable)
{
    if (pRgCam == 0) {
        assert_prog(cameraAssertRgCamNotNil, cameraSourcePath, 829);
    }
    _SetBit(&pRgCam->actionFlags, RG_CAMERA_ACTION_ATTACK, enable);
}

void RgCameraSetActionSeeTarget(RgCamera *pRgCam, int enable)
{
    if (pRgCam == 0) {
        assert_prog(cameraAssertRgCamNotNil, cameraSourcePath, 837);
    }
    _SetBit(&pRgCam->actionFlags, RG_CAMERA_ACTION_SEE_TARGET, enable);
}

void RgCameraControl(RgCamera *pCam, float elapsed)
{
    if (pCam == 0) {
        assert_prog(cameraAssertCamNotNil, cameraSourcePath, 846);
    }
    _PassTimeVersion3(pCam, elapsed);
}

void RgCameraDump(RgCamera *pRgCam)
{
    if (pRgCam == 0) {
        assert_prog(cameraAssertRgCamNotNil, cameraSourcePath, 854);
    }
    XrgLog(cameraDumpHeaderFormat, cameraSourcePath, 858, pRgCam);
    XrgLog(cameraDumpMeEnemyFormat, cameraSourcePath, 859,
           pRgCam->me, pRgCam->enemy);
}

const char cameraDumpAllFalseText[16] = "all-false";

const char cameraDumpAllTrueText[16] = "all-true";

const char cameraDumpSeeTargetText[16] = "see-target";

const char cameraDumpLockText[8] = "lock";

const char cameraDumpInScreenText[8] = "in-scrn";

const char cameraDumpDashText[8] = "dash";

const char cameraDumpLockAttackText[16] = "lock-attack";

const char cameraDumpAttackText[8] = "attack";

const char cameraDumpAdvanceText[8] = "advance";

const char cameraDumpRollText[8] = "roll";



static int (*s_apConditionTbl_0[RG_CAMERA_TIMER_COUNT])(RgCamera *) = {
    _IsActionRoll,
    _IsActionAdvance,
    _IsActionAttack,
    _IsActionLockAndAttack,
    _IsActionDash,
    (int (*)(RgCamera *))_IsActionTargetInScrn,
    _IsActionLockOn,
    _IsActionSeeTarget,
    _IsActionTrue,
    _IsActionFalse
};

const char cameraTextKeyBehindOffset[16] = "behind-offset";

const char cameraTextKeyLookDown[16] = "look-down";

const char cameraTextKeyUpperOffset[16] = "upper-offset";

const char cameraTextKeyRollCoef[16] = "roll-coef";

const char cameraTextKeyUseMoveDir[16] = "use-move-dir";

const char cameraTextKeyRollTime[16] = "roll-time";

const char cameraTextKeyCondition1[8] = "cond-1";

const char cameraTextKeyCondition2[8] = "cond-2";

const char cameraTextKeyCondition3[8] = "cond-3";

const char cameraTextKeyCondition4[8] = "cond-4";

const char cameraTextKeyOperator1[8] = "ope-1";

const char cameraTextKeyOperator2[8] = "ope-2";

const char cameraTextKeySubslideStart[16] = "subslide-start";

const char cameraTextKeySubslideDegree[16] = "subslide-degree";

const char cameraTextKeySubslideLength[16] = "subslide-len";

const char cameraTextKeySubslideCoef[16] = "subslide-coef";

const char cameraOperatorAndText[8] = "and";

const char cameraOperatorOrText[8] = "or";

const char cameraUnknownOperatorFormat[32] = "unknown operator name '%s'";

const char cameraUnknownBoolOperatorFormat[40] = "unknown bool operator %d (index %d)";
