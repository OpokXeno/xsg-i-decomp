/*
 * OV12 original TU 16: 0x00a10f20..0x00a129c8 (43 functions)
 */
#include "common.h"
#include "rg_camera.h"

static int _SetBit(int *flags, int bit, int value);
static void _PassTimeVersion3(RgCamera *camera, float elapsed);
static int _GetVer3TimerID(char *timerName);
extern int strcmp(const char *left, const char *right);
extern char *s_aszVer3TimerName[10];
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
extern const char D_00A52920[];
extern const char D_00A52940[];
extern const char D_00A52958[];

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
extern const char D_00A52948[];
/* ov12:0x00a52968 "pReader != NIL" */
extern const char D_00A52968[];
/* ov12:0x00a52978 "---------- camera dump %p\n" */
extern const char D_00A52978[];
/* ov12:0x00a52998 "  me=%p enemy=%p\n" */
extern const char D_00A52998[];

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

/* ov12:0x00a52750 "pCam != NIL" */
extern const char D_00A52750[];
/* ov12:0x00a52760 "../rg_camera.euc.c" */
extern const char D_00A52760[];
/* ov12:0x00a52778 "pStudio != NIL" */
extern const char D_00A52778[];

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

extern RgDrawView *RgDrawStudioGetView(RgDrawStudio *pStudio);
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

INCLUDE_ASM("asm/nonmatchings/ov12/rg_camera", _SetVersion3Extent);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_camera", _LoadVersion3);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_camera", _PassTimeVersion3);

static int _SetVersion3Extent(RgCamera *camera);
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
