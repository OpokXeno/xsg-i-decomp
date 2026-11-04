/*
 * OV12 original TU 16: 0x00a10f20..0x00a129c8 (43 functions)
 */
#include "common.h"
#include "rg_camera.h"
#include "ov12/rg_text_info.h"

static int _SetBit(int *flags, int bit, int value);
static void _PassTimeVersion3(RgCamera *camera, float elapsed);
static int _GetVer3TimerID(char *timerName);
extern int strcmp(const char *left, const char *right);
extern const char D_00A527F0[];
extern const char D_00A527E8[];
extern const char D_00A527E0[];
extern const char D_00A527D0[];
extern const char D_00A527C8[];
extern const char D_00A527C0[];
extern const char D_00A527B8[];
extern const char D_00A527A8[];
extern const char D_00A52798[];
extern const char D_00A52788[];

const char D_00A52750[] = "pCam != NIL";

const char D_00A52760[] = "../rg_camera.euc.c";

const char D_00A52778[] = "pStudio != NIL";

static const char *s_aszVer3TimerName[10] = {
    D_00A527F0, D_00A527E8, D_00A527E0, D_00A527D0, D_00A527C8,
    D_00A527C0, D_00A527B8, D_00A527A8, D_00A52798, D_00A52788
};

extern void *RgHeapAlloc(RgHeap *heap, unsigned int size, const char *source_file, int line);
extern void RgError(const char *message, const char *source_file, int line, ...);
extern void XrgCopyVectorXYZ(RgVector destination, RgVector source);
extern float XrgLengthVector(RgVector vector);
extern float atan2f(float y, float x);
extern RgDrawView *RgDrawStudioGetView(RgDrawStudio *pStudio);
extern void RgDrawViewSetPosition(RgDrawView *view, RgVector position);
extern void RgDrawViewSetRotateX(RgDrawView *view, float angle);
extern void RgDrawViewSetRotateY(RgDrawView *view, float angle);
extern void RgDrawViewSetRotateZ(RgDrawView *view, float angle);
extern void RgDrawViewInit(RgDrawView *view);

const char D_00A52920[] = "camera version error (%d)";

const char D_00A52940[] = "0";

const char D_00A52948[] = "pRgCam != NIL";

const char D_00A52958[] = "pEssence != NIL";

extern RgHeap *InstanceOfRgHeap(void);
extern void RgHeapFree(RgHeap *heap, void *ptr, const char *source_file, int line);
extern void RgCalcLocalForChar(RgMatrix dest_matrix, RgVector position,
                               RgVector facing, RgVector up_ref);

/* Opaque here: rg_geom_point.c (ov12/tu052) owns the RgPointVector definition. */
typedef struct RgPointVector RgPointVector;

extern void __RgGeomPointGetPos(RgGeomPoint *point, RgPointVector *destination,
                                const char *source_file, int source_line);
extern void XrgLog(const char *format, const char *source_file, int line, ...);

/* ov12:0x00a52948 "pRgCam != NIL" */
/* ov12:0x00a52968 "pReader != NIL" */
const char D_00A52968[] = "pReader != NIL";

/* ov12:0x00a52978 "---------- camera dump %p\n" */
const char D_00A52978[] = "---------- camera dump %p\n";

/* ov12:0x00a52998 "  me=%p enemy=%p\n" */
const char D_00A52998[] = "  me=%p enemy=%p\n";

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

extern void assert_prog(const char *expression, const char *source_file, int line);
extern void XrgClearVector(RgVector vector);
extern void XrgCopyVector(RgVector destination, RgVector source);
extern float *XrgVectorY(void);

static void _InitAbstructCamera(RgCamera *camera, RgDrawStudio *studio)
{
    if (camera == 0) {
        assert_prog(D_00A52750, D_00A52760, 86);
    }
    if (studio == 0) {
        assert_prog(D_00A52778, D_00A52760, 87);
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
        camera->actionState[0] = 0;
        camera->actionState[1] = 0;
        camera->actionState[4] = 1;
        camera->actionState[2] = 0;
        camera->actionState[3] = 0;
    } while (0);
    camera->actionState[5] = 0;
    camera->actionState[6] = 0;
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
    yaw = atan2f(direction[0], direction[2]) + 3.1415927f;
    if (yaw > 3.1415927f) {
        yaw -= 6.2831855f;
        while (yaw > 3.1415927f) {
            yaw -= 6.2831855f;
        }
    }
    if (yaw < -3.1415927f) {
        do {
            yaw += 6.2831855f;
        } while (yaw < -3.1415927f);
    }
    __builtin_memcpy(&roll, &camera->actionState[0], sizeof(roll));
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
    return _CheckBit(RG_CAMERA_ACTION_FLAGS(camera), 2);
}

static int _IsActionAdvance(RgCamera *camera)
{
    return _CheckBit(RG_CAMERA_ACTION_FLAGS(camera), 1);
}

static int _IsActionAttack(RgCamera *camera)
{
    return _CheckBit(RG_CAMERA_ACTION_FLAGS(camera), 4);
}

static int _IsActionSeeTarget(RgCamera *camera)
{
    return _CheckBit(RG_CAMERA_ACTION_FLAGS(camera), 5);
}

static int _IsActionLockAndAttack(RgCamera *camera)
{
    int result;

    result = 0;
    if (_CheckBit(RG_CAMERA_ACTION_FLAGS(camera), 4)) {
        result = _CheckBit(RG_CAMERA_ACTION_FLAGS(camera), 0) != 0;
    }
    return result;
}

static int _IsActionLockOn(RgCamera *camera)
{
    return _CheckBit(RG_CAMERA_ACTION_FLAGS(camera), 0);
}

static int _IsActionDash(RgCamera *camera)
{
    return _CheckBit(RG_CAMERA_ACTION_FLAGS(camera), 3);
}

extern int RgDrawViewIsPointInView(RgDrawView *view, RgPointVector *point, float margin);

static void _IsActionTargetInScrn(RgCamera *camera)
{
    /*
     * A RgPointVector-sized scratch buffer (rg_geom_point.c owns the type);
     * __RgGeomPointGetPos fills it and RgDrawViewIsPointInView reads it back.
     */
    unsigned char position[sizeof(RgVector)];

    /* actionState[3] (+0x4c) is the "enemy" geometry point of RgCameraDump's log. */
    __RgGeomPointGetPos((RgGeomPoint *)camera->actionState[3],
                         (RgPointVector *)position, D_00A52760, 220);
    RgDrawViewIsPointInView(RgDrawStudioGetView(camera->studio),
                             (RgPointVector *)position, 0.0f);
}

static int _IsActionTrue(void)
{
    return 1;
}

static int _IsActionFalse(void)
{
    return 0;
}

static void _InitVer3Timer(RgVer3Timer *timer, int (*predicate)(int), float duration)
{
    timer->predicate = predicate;
    timer->duration = duration;
    timer->remaining = 0.0f;
}

static void _PreJobVer3Timer(RgVer3Timer *timer, int value)
{
    if (timer->predicate != 0 && timer->predicate(value) != 0) {
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

static void _SettingVersion3(void)
{
}

extern int (*s_apConditionTbl_0[10])(int);

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
    pi4 = 0.7853982f;
    point3 = 0.3f;
    camera->cam74 = one;
    camera->cam78 = one;
    camera->farDistance = twelve;
    camera->cam74 = one;
    camera->cam78 = one;
    camera->cam7c = half;
    RG_CAMERA_SELECTABLE_FAR(camera) = -1;
    camera->cam90 = pi4;
    camera->cam_a0 = 0;
    camera->cam140 = 0;
    camera->cam8c = ten;
    camera->cam94 = ten;
    camera->cam9c = point3;
    camera->cam98 = 0;
    for (timerIndex = 0; timerIndex < 10; timerIndex++) {
        _InitVer3Timer(&camera->timers[timerIndex], s_apConditionTbl_0[timerIndex], 0.5f);
    }
    three = 3;
    five = 5;
    do {
        camera->cam13c = three;
        camera->cam130 = five;
    } while (0);
    do {
        camera->cam144 = 0;
        camera->cam148 = 0;
    } while (0);
    do {
        camera->cam134 = three;
        camera->cam138 = 0;
    } while (0);
}


extern void *memcpy(void *dest, const void *src, unsigned int n);
extern unsigned int strlen(const char *s);
extern const char D_00A527F8[];
extern const char D_00A52808[];
extern const char D_00A52818[];
extern const char D_00A52828[];
extern const char D_00A52838[];
extern const char D_00A52848[];
extern const char D_00A52858[];
extern const char D_00A52860[];
extern const char D_00A52868[];
extern const char D_00A52870[];
extern const char D_00A52878[];
extern const char D_00A52880[];
extern const char D_00A52888[];
extern const char D_00A52898[];
extern const char D_00A528A8[];
extern const char D_00A528B8[];
extern const char D_00A528C8[];
extern const char D_00A528D0[];
extern const char D_00A528D8[];

static void _LoadVersion3(RgCamera *camera, void *reader)
{
    char text[6][64];
    char *operatorText = text[4];
    struct RgTextInfoEntry table[] = {
        { RG_TEXT_INFO_FLOAT_OFFSET, D_00A527F8, (void *) 0x70, 0 },
        { RG_TEXT_INFO_FLOAT_OFFSET, D_00A52808, (void *) 0x74, 0 },
        { RG_TEXT_INFO_FLOAT_OFFSET, D_00A52818, (void *) 0x78, 0 },
        { RG_TEXT_INFO_FLOAT_OFFSET, D_00A52828, (void *) 0x7C, 0 },
        { RG_TEXT_INFO_BOOL_OFFSET, D_00A52838, (void *) 0xA0, 0 },
        { RG_TEXT_INFO_FLOAT_OFFSET, s_aszVer3TimerName[0], (void *) 0xb4, 0 },
        { RG_TEXT_INFO_FLOAT_OFFSET, s_aszVer3TimerName[1], (void *) 0xc0, 0 },
        { RG_TEXT_INFO_FLOAT_OFFSET, s_aszVer3TimerName[2], (void *) 0xcc, 0 },
        { RG_TEXT_INFO_FLOAT_OFFSET, s_aszVer3TimerName[3], (void *) 0xd8, 0 },
        { RG_TEXT_INFO_FLOAT_OFFSET, s_aszVer3TimerName[4], (void *) 0xe4, 0 },
        { RG_TEXT_INFO_FLOAT_OFFSET, s_aszVer3TimerName[5], (void *) 0xf0, 0 },
        { RG_TEXT_INFO_FLOAT_OFFSET, s_aszVer3TimerName[6], (void *) 0xfc, 0 },
        { RG_TEXT_INFO_FLOAT_OFFSET, s_aszVer3TimerName[7], (void *) 0x108, 0 },
        { RG_TEXT_INFO_FLOAT_OFFSET, D_00A52848, (void *) 0xB4, 0 },
        { RG_TEXT_INFO_STRING_ADDRESS, D_00A52858, text[0], 0 },
        { RG_TEXT_INFO_STRING_ADDRESS, D_00A52860, text[1], 0 },
        { RG_TEXT_INFO_STRING_ADDRESS, D_00A52868, text[2], 0 },
        { RG_TEXT_INFO_STRING_ADDRESS, D_00A52870, text[3], 0 },
        { RG_TEXT_INFO_STRING_ADDRESS, D_00A52878, text[4], 0 },
        { RG_TEXT_INFO_STRING_ADDRESS, D_00A52880, text[5], 0 },
        { RG_TEXT_INFO_FLOAT_OFFSET, D_00A52888, (void *) 0x8C, 0 },
        { RG_TEXT_INFO_FLOAT_OFFSET, D_00A52898, (void *) 0x90, 0 },
        { RG_TEXT_INFO_FLOAT_OFFSET, D_00A528A8, (void *) 0x94, 0 },
        { RG_TEXT_INFO_FLOAT_OFFSET, D_00A528B8, (void *) 0x9C, 0 },
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

    camera->cam90 = camera->cam90 * 3.1415927f / 180.0f;

    timerId = &camera->cam130;
    for (i = 0; i < 4; i++) {
        name = text[i];

        if (name[0] == '!') {
            name++;
            camera->cam140 |= 1 << i;
        }
        id = _GetVer3TimerID(name);
        if (id >= 0) {
            timerId[i] = id;
        }
    }

    operatorIsAnd = &camera->cam144;
    operator = operatorText;
    for (i = 0; i < 2; i++) {
        if (strcmp(operator, D_00A528C8) == 0) {
            *operatorIsAnd = 1;
        } else if (strcmp(operator, D_00A528D0) == 0) {
            *operatorIsAnd = 0;
        } else if (strlen(operator) != 0) {
            RgError(D_00A528D8, D_00A52760, 429, operator);
        }
        operatorIsAnd++;
        operator += 64;
    }
}


extern float sinf(float x);
extern float cosf(float x);
extern void XrgPadSetID(int pad_id);
extern int XrgPadIsSelect(void);
extern void XrgSubVector(RgVector destination, RgVector first, RgVector second);
extern void XrgOuterVector(RgVector destination, RgVector first, RgVector second);
extern float XrgInnerVector(RgVector first, RgVector second);
extern void RgGeomPointGetVel(RgGeomPoint *point, RgVector velocity);
extern float RgGeomRobotGetRotate(const RgGeom *geom);
extern void RgCameraResetAction(RgCamera *pRgCam);
extern const char D_00A528F8[];

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

    me = (RgGeomPoint *)camera->actionState[2];
    enemy = (RgGeomPoint *)camera->actionState[3];
    prevInt50 = RG_CAMERA_INT_50(camera);
    RG_CAMERA_INT_50(camera) = 0;

    if (RG_CAMERA_SELECTABLE_FAR(camera) >= 0) {
        XrgPadSetID(RG_CAMERA_SELECTABLE_FAR(camera));
        if (XrgPadIsSelect()) {
            if (camera->farDistance >= 10.0f) {
                camera->farDistance = 8.0f;
            } else {
                camera->farDistance = 14.0f;
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
    smoothVel = RG_CAMERA_FLOAT_40(camera);

    targetDist = camera->farDistance;
    __RgGeomPointGetPos(me, (RgPointVector *)pos, D_00A52760, 494);
    if (enemy != 0) {
        __RgGeomPointGetPos(enemy, (RgPointVector *)enemyPos, D_00A52760, 496);
    } else {
        XrgClearVector(enemyPos);
    }
    XrgSubVector(delta, enemyPos, pos);
    dist = XrgLengthVector(delta);
    robotRot = RgGeomRobotGetRotate(me);
    enemyYaw = atan2f(delta[0], delta[2]);

    timers = RG_CAMERA_VER3_TIMERS(camera);
    for (i = 0; i < 10; i++) {
        _PreJobVer3Timer(&timers[i], (int)camera);
        timerActive[i] = _IsVer3TimerActive(&timers[i]);
    }

    mask = RG_CAMERA_INT_140(camera);
    for (i = 0; i <= 3; i++) {
        int bit = 1 << i;
        int active = timerActive[RG_CAMERA_TIMER_IDS(camera)[i]];

        localFlag[i] = active;
        if (mask & bit) {
            localFlag[i] = (active == 0);
        }
    }

    for (i = 0; i < 2; i++) {
        switch (RG_CAMERA_FLAGS_144(camera)[i]) {
        case 1:
            outFlag[i] = localFlag[i * 2] && localFlag[i * 2 + 1];
            break;
        case 0:
            outFlag[i] = localFlag[i * 2] || localFlag[i * 2 + 1];
            break;
        default:
            RgError(D_00A528F8, D_00A52760, 525, RG_CAMERA_FLAGS_144(camera)[i], i);
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
    } else if (RG_CAMERA_USE_MOVE_DIR(camera) != 0) {
        RgGeomPointGetVel(me, dirVec);
        desiredYaw = atan2f(dirVec[0], dirVec[2]);
    } else {
        desiredYaw = RgGeomRobotGetRotate(me);
    }

    XrgCopyVector(yAxis, XrgVectorY());

    ratio = (RG_CAMERA_FLOAT_8C(camera) - dist) / RG_CAMERA_FLOAT_8C(camera);
    diff = enemyYaw - robotRot;
    while (diff > 3.1415927f) {
        diff -= 6.2831855f;
    }
    while (diff < -3.1415927f) {
        diff += 6.2831855f;
    }
    if (diff < 0.0f) {
        diff = -diff;
    }
    limit = RG_CAMERA_FLOAT_90(camera);
    limit = limit / (3.1415927f - limit);
    excess = diff - limit;
    if (ratio > 0.0f) {
        if (excess > 0.0f) {
            targetDist = camera->farDistance + RG_CAMERA_FLOAT_94(camera) * ratio;
        }
    }

    turnRate = 0.0f;
    if (timerActive[4] != 0) {
        RgGeomPointGetVel(me, vel);
        XrgOuterVector(outer, *target, *up);
        if (0.0f < XrgInnerVector(outer, vel)) {
            turnRate = 0.039269908f;
        } else {
            turnRate = -0.039269908f;
        }
    }

    if (prevInt50 != 0) {
        RG_CAMERA_FLOAT_98(camera) = targetDist;
        yaw = desiredYaw;
        smoothVel = smoothVel * 0.9f + turnRate * 0.1f;
        eyeCopy[0] = mePos[0] - sinf(yaw) * RG_CAMERA_FLOAT_98(camera);
        eyeCopy[1] = mePos[1] + RG_CAMERA_UPPER_OFFSET(camera);
        eyeCopy[2] = mePos[2] - cosf(yaw) * RG_CAMERA_FLOAT_98(camera);
        XrgCopyVector(upCopy, yAxis);
    } else {
        yawDelta = desiredYaw - yaw;
        while (yawDelta > 3.1415927f) {
            yawDelta -= 6.2831855f;
        }
        while (yawDelta < -3.1415927f) {
            yawDelta += 6.2831855f;
        }
        yaw += yawDelta * RG_CAMERA_ROLL_COEF(camera);
        smoothedDist = targetDist * RG_CAMERA_FLOAT_9C(camera)
            + RG_CAMERA_FLOAT_98(camera) * (1.0f - RG_CAMERA_FLOAT_9C(camera));
        RG_CAMERA_FLOAT_98(camera) = smoothedDist;
        smoothVel = smoothVel * 0.9f + turnRate * 0.1f;
        eyeCopy[0] = mePos[0] - sinf(yaw) * RG_CAMERA_FLOAT_98(camera);
        eyeCopy[1] = mePos[1] + RG_CAMERA_UPPER_OFFSET(camera);
        eyeCopy[2] = mePos[2] - cosf(yaw) * RG_CAMERA_FLOAT_98(camera);
        XrgCopyVector(upCopy, yAxis);
    }

    delta[0] = sinf(yaw) * camera->farDistance;
    delta[1] = -RG_CAMERA_LOOK_DOWN(camera);
    delta[2] = cosf(yaw) * camera->farDistance;
    XrgCopyVectorXYZ(*target, delta);
    XrgCopyVectorXYZ(*eye, eyeCopy);
    XrgCopyVectorXYZ(*up, upCopy);
    RG_CAMERA_FLOAT_40(camera) = smoothVel;

    _SetView(camera);
    for (i = 0; i < 10; i++) {
        _PastVer3Timer(&RG_CAMERA_VER3_TIMERS(camera)[i], elapsed);
    }
    RgCameraResetAction(camera);
}

static void _SetVersion3Extent(RgCamera *camera);
static void _LoadVersion3(RgCamera *camera, void *reader);

static void _InitVersion3(RgCamera *camera, RgDrawStudio *studio)
{
    if (camera == 0) {
        assert_prog(D_00A52750, D_00A52760, 652);
    }
    _InitAbstructCamera(camera, studio);
    camera->actionState[5] = (int) _SettingVersion3;
    camera->actionState[6] = (int) _LoadVersion3;
    _SetVersion3Extent(camera);
}

RgCamera *CreateRgCamera(int cameraVersion, RgDrawStudio *studio)
{
    RgCamera *camera;

    camera = 0;
    if (cameraVersion == 0) {
        RgError(D_00A52920, D_00A52760, 676, cameraVersion);
    } else if (cameraVersion == 1) {
        RgError(D_00A52920, D_00A52760, 679, cameraVersion);
    } else if (cameraVersion == 2) {
        RgCamera *allocatedCamera;

        allocatedCamera = RgHeapAlloc(InstanceOfRgHeap(), 0x150, D_00A52760, 683);
        if (allocatedCamera == 0) {
            assert_prog(D_00A52750, D_00A52760, 684);
        }
        _InitVersion3(allocatedCamera, studio);
        camera = allocatedCamera;
    } else {
        assert_prog(D_00A52940, D_00A52760, 689);
    }
    return camera;
}

void DisposeRgCamera(RgCamera *pRgCam)
{
    if (pRgCam == 0) {
        assert_prog(D_00A52948, D_00A52760, 697);
    }
    RgHeapFree(InstanceOfRgHeap(), pRgCam, D_00A52760, 698);
}

typedef struct RgCameraEssence {
    int me_point_address;
    int enemy_point_address;
} RgCameraEssence;

void RgCameraSetting(RgCamera *camera, RgCameraEssence *essence)
{
    void (*setCamera)(RgCamera *, RgCameraEssence *);

    if (camera == 0) {
        assert_prog(D_00A52750, D_00A52760, 709);
    }
    if (essence == 0) {
        assert_prog(D_00A52958, D_00A52760, 710);
    }
    setCamera = (void (*)(RgCamera *, RgCameraEssence *))camera->actionState[5];
    camera->actionState[2] = essence->me_point_address;
    camera->actionState[3] = essence->enemy_point_address;
    if (setCamera != 0) {
        setCamera(camera, essence);
    }
    RgDrawViewInit(RgDrawStudioGetView(camera->studio));
}

void RgCameraLoadText(RgCamera *pCam, void *pReader)
{
    void (*loadText)(RgCamera *, void *);

    if (pCam == 0) {
        assert_prog(D_00A52750, D_00A52760, 726);
    }
    if (pReader == 0) {
        assert_prog(D_00A52968, D_00A52760, 727);
    }
    /*
     * actionState[6] (+0x58) holds a per-mode text-load callback; the array's
     * other indices are unrelated (see RgCamera's own definition).
     */
    loadText = (void (*)(RgCamera *, void *))pCam->actionState[6];
    if (loadText != 0) {
        loadText(pCam, pReader);
    }
}

void RgCameraSetFarMode(RgCamera *pCam, int enable)
{
    if (pCam == 0) {
        assert_prog(D_00A52750, D_00A52760, 737);
    }
    if (enable != 0) {
        pCam->farDistance = 14.0f;
    } else {
        pCam->farDistance = 8.0f;
    }
}

void RgCameraSelectableFar(RgCamera *pCam, int level)
{
    if (pCam == 0) {
        assert_prog(D_00A52750, D_00A52760, 750);
    }
    if (level >= 2) {
        level = -1;
    }
    RG_CAMERA_SELECTABLE_FAR(pCam) = level;
}

void RgCameraLocal(RgCamera *pCam, RgMatrix matrix)
{
    if (pCam == 0) {
        assert_prog(D_00A52750, D_00A52760, 762);
    }
    RgCalcLocalForChar(matrix, pCam->eye, pCam->target, pCam->up);
}

void RgCameraGetTarget(RgCamera *pCam, RgPointVector *position)
{
    if (pCam == 0) {
        assert_prog(D_00A52750, D_00A52760, 771);
    }
    /* actionState[2] (+0x48) is the "me" geometry point of RgCameraDump's log. */
    __RgGeomPointGetPos((RgGeomPoint *)pCam->actionState[2], position, D_00A52760, 772);
}

RgDrawStudio *RgCameraGetStudio(RgCamera *pCam)
{
    if (pCam == 0) {
        assert_prog(D_00A52750, D_00A52760, 779);
    }
    return pCam->studio;
}

void RgCameraResetAction(RgCamera *pRgCam)
{
    if (pRgCam == 0) {
        assert_prog(D_00A52948, D_00A52760, 789);
    }
    pRgCam->actionState[1] = 0;
}

void RgCameraSetActionLockedOn(RgCamera *pRgCam, int enable)
{
    if (pRgCam == 0) {
        assert_prog(D_00A52948, D_00A52760, 797);
    }
    _SetBit(&pRgCam->actionState[1], 0, enable);
}

void RgCameraSetActionAdvance(RgCamera *pRgCam, int enable)
{
    if (pRgCam == 0) {
        assert_prog(D_00A52948, D_00A52760, 805);
    }
    _SetBit(&pRgCam->actionState[1], 1, enable);
}

void RgCameraSetActionRoll(RgCamera *pRgCam, int enable)
{
    if (pRgCam == 0) {
        assert_prog(D_00A52948, D_00A52760, 813);
    }
    _SetBit(&pRgCam->actionState[1], 2, enable);
}

void RgCameraSetActionDash(RgCamera *pRgCam, int enable)
{
    if (pRgCam == 0) {
        assert_prog(D_00A52948, D_00A52760, 821);
    }
    _SetBit(&pRgCam->actionState[1], 3, enable);
}

void RgCameraSetActionAttack(RgCamera *pRgCam, int enable)
{
    if (pRgCam == 0) {
        assert_prog(D_00A52948, D_00A52760, 829);
    }
    _SetBit(&pRgCam->actionState[1], 4, enable);
}

void RgCameraSetActionSeeTarget(RgCamera *pRgCam, int enable)
{
    if (pRgCam == 0) {
        assert_prog(D_00A52948, D_00A52760, 837);
    }
    _SetBit(&pRgCam->actionState[1], 5, enable);
}

void RgCameraControl(RgCamera *pCam, float elapsed)
{
    if (pCam == 0) {
        assert_prog(D_00A52750, D_00A52760, 846);
    }
    _PassTimeVersion3(pCam, elapsed);
}

void RgCameraDump(RgCamera *pRgCam)
{
    if (pRgCam == 0) {
        assert_prog(D_00A52948, D_00A52760, 854);
    }
    XrgLog(D_00A52978, D_00A52760, 858, pRgCam);
    XrgLog(D_00A52998, D_00A52760, 859,
           (void *)pRgCam->actionState[2], (void *)pRgCam->actionState[3]);
}

const char D_00A52788[16] = "all-false";

const char D_00A52798[16] = "all-true";

const char D_00A527A8[16] = "see-target";

const char D_00A527B8[8] = "lock";

const char D_00A527C0[8] = "in-scrn";

const char D_00A527C8[8] = "dash";

const char D_00A527D0[16] = "lock-attack";

const char D_00A527E0[8] = "attack";

const char D_00A527E8[8] = "advance";

const char D_00A527F0[8] = "roll";
