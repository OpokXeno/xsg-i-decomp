#include "common.h"

#include "shared.h"

extern float xglAtan2(float x, float y);

extern float xglSin(float angle);

extern float xglCos(float angle);

/* Partial camera view: TCAMERA_setFov/fovSPL use fieldOfView at +0x94;
 * TCAMERA_setView uses rotation at +0xa0 and translation at +0xd0.
 * Unevidenced spans remain explicitly unmodeled. */

typedef struct TCamera {
    unsigned char unmodeled_00[0x94];
    float fieldOfView;
    unsigned char unmodeled_98[8];
    Vector4 rotation;
    unsigned char unmodeled_b0[0x20];
    Vector4 translation;
} TCamera;

#define TCAMERA_ROTATION(camera)    (&((TCamera *)(camera))->rotation)

#define TCAMERA_TRANSLATION(camera) (&((TCamera *)(camera))->translation)

/*
 * TCAMERA_setFov/TCAMERA_fovSPL (this TU, VA 0x00308b58/0x00308f60) evidence
 * a field-of-view scalar at +0x94.
 */

#define TCAMERA_FOV(camera)         (((TCamera *)(camera))->fieldOfView)

/* Every accepted call site of SPL_getValueXYZ (near_dir.c, spl.h) spells this
 * prototype; SPL_getValue has no accepted definition yet, so its second
 * parameter is named from the one value every recovered call site here
 * passes it, literal 0, and its third from the frame argument TCAMERA_update
 * (VA 0x0030931c, 0x00309348) loads into $f12 before calling TCAMERA_rollSPL/
 * TCAMERA_fovSPL, which forward it untouched into SPL_getValue, whose body
 * reads $f12 (c.le.s at 0x0030c0d8). */

extern void SPL_getValueXYZ(float *destination, void *spline, float frame);

extern float SPL_getValue(void *spline, int index, float frame);

/*
 * TCAMERA_get indexes the engine's camera table `tcamera` (main:0x00465e10);
 * Java_xeno_Camera_create__I (src/main/camera.c) computes the same
 * `tcamera + camera_id * 0x12c0` address for the same table.
 */

extern unsigned char tcamera[];

extern StudioCamera *xglStudioGetActiveCamera(void);

const char D_004D1838[] = "TX %7.3f";

const char D_004D1848[] = "TY %7.3f";

const char D_004D1858[] = "TZ %7.3f";

const char D_004D1868[] = "IX %7.3f";

const char D_004D1878[] = "IY %7.3f";

const char D_004D1888[] = "IZ %7.3f";

const char D_004D1898[] = "RX %7.3f";

const char D_004D18A8[] = "RY %7.3f";

const char D_004D18B8[] = "RZ %7.3f";

const char D_004D18C8[] = "FOV%7.3f";

extern void DB_incPos(int x, int y);

extern void DB_println(const char *format, ...);

typedef unsigned int CameraVectorStorage __attribute__((mode(TI)));

typedef union {
    Vector4 vector;
    CameraVectorStorage storage;
} MpackInterest;

static MpackInterest mpack_interest;

/*
 * TCAMERA_transMPack samples eight consecutive values off the packed curve
 * `source` at `frame` through one FCV_resetPack/FCV_getPackValue cursor
 * (src/main/fcv2.c): translation xyz, an interest-point xyz (the same point
 * TCAMERA_mpackGetInterest reads, this TU), then roll and field of view. The
 * cursor lives in an FCVPack this function places at the EE scratchpad base
 * (docs/ee-reference/memory-dma.md); every call below addresses that same
 * fixed pack, matching the original's repeated `lui $4,0x7000` before each
 * call (FCV_resetPack/FCV_getPackValue's own record types are TU-local to
 * fcv2.c, so the pack and source are reached as opaque pointers here).
 *
 * The interest point and the translation just written feed the same VU0
 * macro-mode subtraction TCAMERA_setView uses to turn a look-at point into
 * yaw/pitch: direction = translation - interest, yaw = atan2(dx, dz), pitch =
 * -atan2(dy, dx*sin(yaw) + dz*cos(yaw)). Roll and field of view are plain
 * degrees-to-radians conversions, each with its own .lit4 pi constant
 * (data_ownership in config/tu-build.json still owns that pool).
 */

#define TCAMERA_MPACK_SCRATCH ((void *)0x70000000)

extern void FCV_resetPack(void *pack, void *source);

extern float FCV_getPackValue(void *pack, float frame);

typedef struct {
    unsigned char unmodeled_00[12];
    int mode[4];
    unsigned char unmodeled_1c[0x12c0 - 28];
} CameraInitRecord;

#include "main/xgl_studio.h"

extern PadPrefix PadData[];

extern void FCV2_resetPack(void *pack, void *source);

extern float FCV2_getPackValue(void *pack, float frame);

const char D_004D18D8[24] = "trans (%f %f %f)";

const char D_004D18F0[24] = "rotate(%f %f %f)";

const char D_004D1908[24] = "fov (%f)";

static int camDisp __attribute__((section(".sdata"))) = 0;

extern void xglFontDebugPrintf(int x, int y, const char *format, ...);

extern void nmlModelSetGlobalFogDist(float start, float end,
                                     float parameter0, float parameter1);

extern void nmlModelSetGlobalFogCol(const Vector4 *color);

TCamera *TCAMERA_get(int camera_id)
{
    return (TCamera *)(tcamera + camera_id * 0x12c0);
}

void TCAMERA_info(void)
{
    TCamera *camera = (TCamera *)xglStudioGetActiveCamera();
    Vector4 *translation = TCAMERA_TRANSLATION(camera);
    Vector4 *rotation = TCAMERA_ROTATION(camera);
    Vector4 *interest = &mpack_interest.vector;

    DB_println(D_004D1838, (double)translation->x);
    DB_println(D_004D1848, (double)translation->y);
    DB_println(D_004D1858, (double)translation->z);
    DB_incPos(0, 6);
    if (interest->w > 0.0f) {
        DB_println(D_004D1868, (double)interest->x);
        DB_println(D_004D1878, (double)interest->y);
        DB_println(D_004D1888, (double)interest->z);
        interest->w = 0.0f;
    } else {
        DB_println(D_004D1898, (double)(rotation->x / 3.1415927f * 180.0f));
        DB_println(D_004D18A8, (double)(rotation->y / 3.1415927f * 180.0f));
        DB_println(D_004D18B8, (double)(rotation->z / 3.1415927f * 180.0f));
    }
    DB_incPos(0, 6);
    DB_println(D_004D18C8, (double)(TCAMERA_FOV(camera) / 3.1415927f * 180.0f));
}

void TCAMERA_transMPack(TCamera *camera, void *source, float frame)
{
    Vector4 *translation = TCAMERA_TRANSLATION(camera);
    Vector4 *rotation = TCAMERA_ROTATION(camera);
    float interest[4];
    Vector4 direction;
    float yaw;
    float radiansPerDegree;
    float positionScale;

    FCV_resetPack(TCAMERA_MPACK_SCRATCH, source);
    positionScale = 0.1f;
    translation->x = FCV_getPackValue(TCAMERA_MPACK_SCRATCH, frame) * positionScale;
    translation->y = FCV_getPackValue(TCAMERA_MPACK_SCRATCH, frame) * positionScale;
    translation->z = FCV_getPackValue(TCAMERA_MPACK_SCRATCH, frame) * positionScale;
    interest[0] = FCV_getPackValue(TCAMERA_MPACK_SCRATCH, frame) * positionScale;
    interest[1] = FCV_getPackValue(TCAMERA_MPACK_SCRATCH, frame) * positionScale;
    interest[2] = FCV_getPackValue(TCAMERA_MPACK_SCRATCH, frame) * positionScale;
    __asm__ __volatile__(
        "lqc2 $vf3, 0(%0)\n\t"
        "lqc2 $vf2, 0(%1)\n\t"
        "vsub.xyz $vf2xyz, $vf2xyz, $vf3xyz\n\t"
        "sqc2 $vf2, 0(%2)"
        :
        : "r"(interest), "r"(translation), "r"(&direction)
        : "memory");
    yaw = xglAtan2(direction.x, direction.z);
    radiansPerDegree = 3.1415927f;
    rotation->y = yaw;
    rotation->x = -xglAtan2(direction.y,
                             direction.x * xglSin(yaw) + direction.z * xglCos(rotation->y));
    rotation->z = FCV_getPackValue(TCAMERA_MPACK_SCRATCH, frame) / 180.0f * radiansPerDegree;
    TCAMERA_FOV(camera) = FCV_getPackValue(TCAMERA_MPACK_SCRATCH, frame) / 180.0f * radiansPerDegree;
}

CameraVectorStorage TCAMERA_mpackGetInterest(volatile MpackInterest *interest)
{
    CameraVectorStorage value = mpack_interest.storage;
    interest->storage = value;
    return value;
}

void TCAMERA_transMPack2(TCamera *camera, void *source, float frame)
{
    Vector4 *translation = TCAMERA_TRANSLATION(camera);
    Vector4 *rotation = TCAMERA_ROTATION(camera);
    MpackInterest interest;
    Vector4 direction;

    FCV2_resetPack(TCAMERA_MPACK_SCRATCH, source);
    translation->x = FCV2_getPackValue(TCAMERA_MPACK_SCRATCH, frame);
    translation->y = FCV2_getPackValue(TCAMERA_MPACK_SCRATCH, frame);
    translation->z = FCV2_getPackValue(TCAMERA_MPACK_SCRATCH, frame);
    interest.vector.x = FCV2_getPackValue(TCAMERA_MPACK_SCRATCH, frame);
    interest.vector.y = FCV2_getPackValue(TCAMERA_MPACK_SCRATCH, frame);
    interest.vector.z = FCV2_getPackValue(TCAMERA_MPACK_SCRATCH, frame);
    interest.vector.w = 1.0f;
    __asm__ __volatile__(
        "lq $2, 0(%1)\n\t"
        "sq $2, 0(%0)"
        :
        : "r"(&mpack_interest.storage), "r"(&interest.storage)
        : "$2", "memory");
    __asm__ __volatile__(
        "lqc2 $vf3, 0(%0)\n\t"
        "lqc2 $vf2, 0(%1)\n\t"
        "vsub.xyz $vf2xyz, $vf2xyz, $vf3xyz\n\t"
        "sqc2 $vf2, 0(%2)"
        :
        : "r"(&interest.vector), "r"(translation), "r"(&direction)
        : "memory");
    rotation->y = xglAtan2(direction.x, direction.z);
    rotation->x = -xglAtan2(direction.y,
                            direction.x * xglSin(rotation->y) +
                                direction.z * xglCos(rotation->y));
    rotation->z = FCV2_getPackValue(TCAMERA_MPACK_SCRATCH, frame);
    TCAMERA_FOV(camera) = FCV2_getPackValue(TCAMERA_MPACK_SCRATCH, frame);
}

void TCAMERA_viewMPack(TCamera *camera, void *source, float frame)
{
}

void TCAMERA_setTranslate(TCamera *camera, float x, float y, float z)
{
    Vector4 *translation = TCAMERA_TRANSLATION(camera);

    translation->x = x;
    translation->y = y;
    translation->z = z;
}

void TCAMERA_setFov(TCamera *camera, float degrees)
{
    TCAMERA_FOV(camera) = degrees / 180.0f * 3.1415927f;
}

void TCAMERA_setRoll(TCamera *camera, float degrees)
{
    TCAMERA_ROTATION(camera)->z = degrees / 180.0f * 3.1415927f;
}

void TCAMERA_setRotate(TCamera *camera, float pitchDegrees, float yawDegrees, float rollDegrees)
{
    Vector4 *rotation = TCAMERA_ROTATION(camera);

    rotation->x = pitchDegrees / 180.0f * 3.1415927f;
    rotation->y = yawDegrees / 180.0f * 3.1415927f;
    rotation->z = rollDegrees / 180.0f * 3.1415927f;
}

void TCAMERA_setView(TCamera *camera, float x, float y, float z)
{
    float home[4];
    Vector4 dir;
    Vector4 *rotation;
    Vector4 *translation;
    float yaw;
    float sine;
    float cosine;
    float denom;

    translation = TCAMERA_TRANSLATION(camera);
    rotation = TCAMERA_ROTATION(camera);
    home[0] = x;
    home[1] = y;
    home[2] = z;
    __asm__ __volatile__(
        "lqc2 $vf3, 0(%0)\n\t"
        "lqc2 $vf2, 0(%1)\n\t"
        "vsub.xyz $vf2xyz, $vf2xyz, $vf3xyz\n\t"
        "sqc2 $vf2, 0(%2)"
        :
        : "r"(home), "r"(translation), "r"(&dir)
        : "memory");
    yaw = xglAtan2(dir.x, dir.z);
    rotation->y = yaw;
    sine = xglSin(yaw);
    cosine = xglCos(rotation->y);
    denom = dir.x * sine + dir.z * cosine;
    rotation->x = -xglAtan2(dir.y, denom);
}

void TCAMERA_transSPL(TCamera *camera, void *spline, float frame)
{
    SPL_getValueXYZ(&TCAMERA_TRANSLATION(camera)->x, spline, frame);
}

void TCAMERA_rotateSPL(TCamera *camera, void *spline, float frame)
{
    Vector4 *rotation = TCAMERA_ROTATION(camera);
    float angles[3];

    SPL_getValueXYZ(angles, spline, frame);
    rotation->x = (angles[0] / 180.0f) * 3.1415927f;
    rotation->y = (angles[1] / 180.0f) * 3.1415927f;
    rotation->z = (angles[2] / 180.0f) * 3.1415927f;
}

void TCAMERA_viewSPL(TCamera *camera, void *spline, float frame)
{
    Vector4 *translation = TCAMERA_TRANSLATION(camera);
    Vector4 *rotation = TCAMERA_ROTATION(camera);
    float point[4];
    Vector4 direction;
    float yaw;
    float sine;
    float cosine;
    float pitchDenominator;

    SPL_getValueXYZ(point, spline, frame);
    __asm__ __volatile__(
        "lqc2 $vf3, 0(%0)\n\t"
        "lqc2 $vf2, 0(%1)\n\t"
        "vsub.xyz $vf2xyz, $vf2xyz, $vf3xyz\n\t"
        "sqc2 $vf2, 0(%2)"
        :
        : "r"(point), "r"(translation), "r"(&direction)
        : "memory");
    yaw = xglAtan2(direction.x, direction.z);
    rotation->y = yaw;
    sine = xglSin(yaw);
    cosine = xglCos(rotation->y);
    pitchDenominator = direction.x * sine + direction.z * cosine;
    rotation->x = -xglAtan2(direction.y, pitchDenominator);
}

void TCAMERA_transCNS(TCamera *camera, float **constraint, Vector4 *offset, int mode)
{
    Vector4 *translation = TCAMERA_TRANSLATION(camera);
    float point[4];

    if (mode == 0) {
        point[0] = (*constraint)[4];
        point[1] = (*constraint)[5];
        point[2] = (*constraint)[6];
    } else if (mode == 1) {
        point[0] = (*constraint)[0];
        point[1] = (*constraint)[1];
        point[2] = (*constraint)[2];
    } else {
        point[0] = (*constraint)[4];
        point[1] = (*constraint)[5];
        point[2] = (*constraint)[6];
    }
    __asm__ __volatile__(
        "lqc2 $vf3, 0(%0)\n\t"
        "lqc2 $vf2, 0(%1)\n\t"
        "vadd.xyz $vf2xyz, $vf2xyz, $vf3xyz\n\t"
        "sqc2 $vf2, 0(%2)"
        :
        : "r"(offset), "r"(point), "r"(translation)
        : "memory");
}

void TCAMERA_viewCNS(TCamera *camera, float **constraint, Vector4 *offset, int mode)
{
    Vector4 *rotation = TCAMERA_ROTATION(camera);
    float point[4];
    Vector4 direction;
    float yaw;
    float sine;
    float cosine;
    float pitchDenominator;

    if (mode == 0) {
        point[0] = (*constraint)[4];
        point[1] = (*constraint)[5];
        point[2] = (*constraint)[6];
    } else if (mode == 1) {
        point[0] = (*constraint)[0];
        point[1] = (*constraint)[1];
        point[2] = (*constraint)[2];
    } else {
        point[0] = (*constraint)[4];
        point[1] = (*constraint)[5];
        point[2] = (*constraint)[6];
    }
    camera = (TCamera *)((u8 *)camera + 0xd0);
    __asm__ __volatile__(
        "lqc2 $vf3, 0(%0)\n\t"
        "lqc2 $vf2, 0(%1)\n\t"
        "vadd.xyz $vf2xyz, $vf2xyz, $vf3xyz\n\t"
        "sqc2 $vf2, 0(%1)"
        :
        : "r"(offset), "r"(point)
        : "memory");
    __asm__ __volatile__(
        "lqc2 $vf3, 0(%0)\n\t"
        "lqc2 $vf2, 0(%1)\n\t"
        "vsub.xyz $vf2xyz, $vf2xyz, $vf3xyz\n\t"
        "sqc2 $vf2, 0(%2)"
        :
        : "r"(point), "r"(camera), "r"(&direction)
        : "memory");
    yaw = xglAtan2(direction.x, direction.z);
    rotation->y = yaw;
    sine = xglSin(yaw);
    cosine = xglCos(rotation->y);
    pitchDenominator = direction.x * sine + direction.z * cosine;
    rotation->x = -xglAtan2(direction.y, pitchDenominator);
}

void TCAMERA_rollSPL(TCamera *camera, void *spline, float frame)
{
    Vector4 *rotation = TCAMERA_ROTATION(camera);

    rotation->z = SPL_getValue(spline, 0, frame) / 180.0f * 3.1415927f;
}

void TCAMERA_fovSPL(TCamera *camera, void *spline, float frame)
{
    TCAMERA_FOV(camera) = SPL_getValue(spline, 0, frame) / 180.0f * 3.1415927f;
}

void TCAMERA_update(void)
{
    struct TCameraSpline {
        unsigned char unmodeled_00[6];
        short firstKey;
        short lastKey;
    };
    struct TCameraConstraint {
        Vector4 offset;
        int mode;
        float *point;
    };
    struct TCameraMotionPack {
        void *source;
        float frame;
    };
    /* One control track: translation, view, roll or field of view. */
    union TCameraTrack {
        struct TCameraSpline spline;
        struct TCameraMotionPack packed;
        struct TCameraConstraint constraint;
        unsigned char storage[0x4a0];
    };
    struct TCameraWorkView {
        unsigned char unmodeled_00[4];
        int camera_id;
        unsigned char unmodeled_08[4];
        int mode[4];
        int frame[4];
        unsigned char unmodeled_2c[4];
        union TCameraTrack track[4];
        unsigned char unmodeled_12b0[0x10];
    };
    struct TCameraLoopState {
        unsigned char unmodeled_00[0xc8];
        unsigned short activeCameraIndex;
    };
    struct TCameraFogEntry {
        float start;
        float end;
        float parameter0;
        float parameter1;
        Vector4 color;
        unsigned char unmodeled_20[0x80];
    };
    struct TCameraFogTable {
        unsigned char unmodeled_00[0x60];
        struct TCameraFogEntry camera[32];
        unsigned char unmodeled_1460[0x40];
    };

    extern struct TCameraLoopState GameLoopState;
    extern struct TCameraFogTable CfCameraDefine;
    int cameraIndex;
    int cameraOffset;
    float radiansPerHalfTurn;

    if ((PadData[0].half_2a & 4) != 0 && (PadData[0].half_28 & 0x100) != 0) {
        camDisp = (camDisp + 1) & 1;
    }

    radiansPerHalfTurn = 3.1415927f;
    for (cameraIndex = 0, cameraOffset = 0; cameraIndex < 8;
         cameraIndex++, cameraOffset += 0x12c0) {
        StudioCamera *studioCamera = xglStudioGetCamera2(cameraIndex);
        TCamera *camera = (TCamera *)studioCamera;
        Vector4 *position = &studioCamera->position;
        Vector4 *rotation = &studioCamera->rotation;

        if (camDisp != 0 && studioCamera->active != 0) {
            xglFontDebugPrintf(0, 0x10, D_004D18D8, (double)position->x,
                               (double)position->y, (double)position->z);
            xglFontDebugPrintf(0, 0x18, D_004D18F0,
                               (double)(rotation->x / radiansPerHalfTurn * 180.0f),
                               (double)(rotation->y / radiansPerHalfTurn * 180.0f),
                               (double)(rotation->z / radiansPerHalfTurn * 180.0f));
            xglFontDebugPrintf(0, 0x20, D_004D1908,
                               (double)(TCAMERA_FOV(studioCamera) /
                                        radiansPerHalfTurn * 180.0f));
        }

        if (studioCamera->state != 2) {
            struct TCameraWorkView *work =
                (struct TCameraWorkView *)(tcamera + cameraOffset);

            switch (work->mode[0]) {
            case 0x15:
                TCAMERA_transMPack2(camera, work->track[0].packed.source,
                                    work->track[0].packed.frame);
                work->frame[0]++;
                break;
            case 0x11:
                TCAMERA_transMPack(camera, work->track[0].packed.source,
                                   (float)work->frame[0]);
                work->frame[0]++;
                break;
            case 0x12:
                TCAMERA_transSPL(camera, &work->track[0].spline,
                                 (float)work->frame[0]);
                if (work->track[0].spline.firstKey >= 0) {
                    work->frame[0]++;
                    if (work->track[0].spline.lastKey < work->frame[0]) {
                        work->frame[0] = work->track[0].spline.lastKey;
                    }
                } else {
                    work->frame[0]++;
                }
                break;
            case 0x14:
                TCAMERA_transCNS(camera, &work->track[0].constraint.point,
                                 &work->track[0].constraint.offset,
                                 work->track[0].constraint.mode);
                break;
            }

            switch (work->mode[1]) {
            case 0x11:
                TCAMERA_viewMPack(camera, work->track[1].packed.source,
                                  (float)work->frame[1]);
                work->frame[1]++;
                break;
            case 0x12:
                TCAMERA_viewSPL(camera, &work->track[1].spline,
                                (float)work->frame[1]);
                if (work->track[1].spline.firstKey >= 0) {
                    work->frame[1]++;
                    if (work->track[1].spline.lastKey < work->frame[1]) {
                        work->frame[1] = work->track[1].spline.lastKey;
                    }
                } else {
                    work->frame[1]++;
                }
                break;
            case 0x13:
                TCAMERA_rotateSPL(camera, &work->track[1].spline,
                                  (float)work->frame[1]);
                if (work->track[1].spline.firstKey >= 0) {
                    work->frame[1]++;
                    if (work->track[1].spline.lastKey < work->frame[1]) {
                        work->frame[1] = work->track[1].spline.lastKey;
                    }
                } else {
                    work->frame[1]++;
                }
                break;
            case 0x14:
                TCAMERA_viewCNS(camera, &work->track[1].constraint.point,
                                &work->track[1].constraint.offset,
                                work->track[1].constraint.mode);
                break;
            }

            if (work->mode[2] == 0x12) {
                TCAMERA_rollSPL(camera, &work->track[2].spline,
                                (float)work->frame[2]);
                work->frame[2]++;
            }
            if (work->mode[3] == 0x12) {
                TCAMERA_fovSPL(camera, &work->track[3].spline,
                               (float)work->frame[3]);
                work->frame[3]++;
            }
        }

        if (studioCamera->active != 0) {
            struct TCameraFogEntry *fog =
                &CfCameraDefine.camera[GameLoopState.activeCameraIndex];

            if (fog->start != 0.0f && fog->end != 0.0f) {
                nmlModelSetGlobalFogDist(fog->start, fog->end, fog->parameter0,
                                         fog->parameter1);
                nmlModelSetGlobalFogCol(
                    &CfCameraDefine.camera[GameLoopState.activeCameraIndex].color);
            }
        }
    }
}

void TCAMERA_init(void)
{
    CameraInitRecord *cameras = (CameraInitRecord *)tcamera;
    int cameraIndex;
    int modeIndex;

    for (cameraIndex = 0; cameraIndex < 8; cameraIndex++) {
        int *modes = cameras[cameraIndex].mode;
        for (modeIndex = 3; modeIndex >= 0; modeIndex--) {
            modes[modeIndex] = 16;
        }
    }
}

void TCAMERA_setFCurve(void)
{
}
