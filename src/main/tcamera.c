#include "common.h"
#include "shared.h"
#include "main/xgl_studio.h"
#include "main/tcamera.h"
#include "camera.h"
#include "main/game_init_camera.h"
#include "main/nml_model_set.h"
#include "main/spl.h"
#include "main/xgl_2.h"
#include "main/xgl_font.h"
#include "main/fcv2.h"
#include "quadword.h"
#include "vu0_macro.h"

#define TCAMERA_ROTATION(camera)    (&((TCamera *)(camera))->rotation)
#define TCAMERA_TRANSLATION(camera) (&((TCamera *)(camera))->translation)

/*
 * TCAMERA_setFov/TCAMERA_fovSPL (this TU, VA 0x00308b58/0x00308f60) evidence
 * a field-of-view scalar at +0x94.
 */
#define TCAMERA_FOV(camera)         (((TCamera *)(camera))->fieldOfView)

#define TCAMERA_MPACK_SCRATCH ((void *)SCRATCHPAD_BASE)

/* Offset of the translation vector inside a StudioCamera. */
#define TCAMERA_TRANSLATION_OFFSET 0xd0

typedef unsigned int CameraVectorStorage __attribute__((mode(TI)));

typedef union {
    Vector4 vector;
    CameraVectorStorage storage;
} MpackInterest;

/*
 * TCAMERA_get indexes the engine's camera table `tcamera` (main:0x00465e10);
 * Java_xeno_Camera_create__I (src/main/camera.c) computes the same
 * `tcamera + camera_id * 0x12c0` address for the same table.
 */
extern CameraWork tcamera[];

extern PadPrefix PadData[];

extern float SPL_getValue(void *spline, int index, float frame);
extern StudioCamera *xglStudioGetActiveCamera(void);
extern void DB_incPos(int x, int y);
extern void FCV_resetPack(void *pack, void *source);
extern void FCV2_resetPack(void *pack, void *source);
extern void nmlModelSetGlobalFogCol(const Vector4 *color);

const char translationXFormat[] = "TX %7.3f";

const char translationYFormat[] = "TY %7.3f";

const char translationZFormat[] = "TZ %7.3f";

const char interestXFormat[] = "IX %7.3f";

const char interestYFormat[] = "IY %7.3f";

const char interestZFormat[] = "IZ %7.3f";

const char rotationXFormat[] = "RX %7.3f";

const char rotationYFormat[] = "RY %7.3f";

const char rotationZFormat[] = "RZ %7.3f";

const char fovValueFormat[] = "FOV%7.3f";

const char translationVectorFormat[24] = "trans (%f %f %f)";

const char rotationVectorFormat[24] = "rotate(%f %f %f)";

const char fovFormat[24] = "fov (%f)";

static int camDisp = 0;

static MpackInterest mpack_interest;

TCamera *TCAMERA_get(int camera_id)
{
    return (TCamera *)&tcamera[camera_id];
}

void TCAMERA_info(void)
{
    StudioCamera *camera = xglStudioGetActiveCamera();
    Vector4 *translation = &camera->position.vector;
    Vector4 *rotation = &camera->rotation.vector;
    Vector4 *interest = &mpack_interest.vector;

    DB_println(translationXFormat, (double)translation->x);
    DB_println(translationYFormat, (double)translation->y);
    DB_println(translationZFormat, (double)translation->z);
    DB_incPos(0, 6);
    if (interest->w > 0.0f) {
        DB_println(interestXFormat, (double)interest->x);
        DB_println(interestYFormat, (double)interest->y);
        DB_println(interestZFormat, (double)interest->z);
        interest->w = 0.0f;
    } else {
        DB_println(rotationXFormat, (double)(rotation->x / 3.1415927f * 180.0f));
        DB_println(rotationYFormat, (double)(rotation->y / 3.1415927f * 180.0f));
        DB_println(rotationZFormat, (double)(rotation->z / 3.1415927f * 180.0f));
    }
    DB_incPos(0, 6);
    DB_println(fovValueFormat, (double)(camera->fov / 3.1415927f * 180.0f));
}

/*
 * TCAMERA_transMPack samples eight consecutive values off the packed curve
 * `source` at `frame` through one FCV_resetPack/FCV_getPackValue cursor
 * (src/main/fcv2.c): translation xyz, an interest-point xyz (the same point
 * TCAMERA_mpackGetInterest reads, this TU), then roll and field of view. The
 * cursor lives in an FCVPack this function places at the EE scratchpad base
 * (docs/ee-reference/memory-dma.md); every call below addresses that same
 * fixed pack, matching the original's repeated `lui $4,0x7000` before each
 * call (FCVPack and FCV2Pack are declared in main/fcv2.h; FCV_resetPack's
 * source record stays an opaque pointer here).
 *
 * The interest point and the translation just written feed the same VU0
 * macro-mode subtraction TCAMERA_setView uses to turn a look-at point into
 * yaw/pitch: direction = translation - interest, yaw = atan2(dx, dz), pitch =
 * -atan2(dy, dx*sin(yaw) + dz*cos(yaw)). Roll and field of view are plain
 * degrees-to-radians conversions, each with its own .lit4 pi constant
 * (data_ownership in config/tu-build.json still owns that pool).
 */
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
    Vu0SubVectorXYZ(&direction, translation, interest);
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
    QuadwordCopy(&mpack_interest.storage, &interest.storage);
    Vu0SubVectorXYZ(&direction, translation, &interest.vector);
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
    Vu0SubVectorXYZ(&dir, translation, home);
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
    Vu0SubVectorXYZ(&direction, translation, point);
    yaw = xglAtan2(direction.x, direction.z);
    rotation->y = yaw;
    sine = xglSin(yaw);
    cosine = xglCos(rotation->y);
    pitchDenominator = direction.x * sine + direction.z * cosine;
    rotation->x = -xglAtan2(direction.y, pitchDenominator);
}

void TCAMERA_transCNS(TCamera *camera, Vector4 **reference_position, Vector4 *offset, int mode)
{
    Vector4 *translation = TCAMERA_TRANSLATION(camera);
    float point[4];

    if (mode == 0) {
        point[0] = (*reference_position)[1].x;
        point[1] = (*reference_position)[1].y;
        point[2] = (*reference_position)[1].z;
    } else if (mode == 1) {
        point[0] = (*reference_position)[0].x;
        point[1] = (*reference_position)[0].y;
        point[2] = (*reference_position)[0].z;
    } else {
        point[0] = (*reference_position)[1].x;
        point[1] = (*reference_position)[1].y;
        point[2] = (*reference_position)[1].z;
    }
    Vu0AddVectorXYZ(translation, point, offset);
}

void TCAMERA_viewCNS(TCamera *camera, Vector4 **reference_position, Vector4 *offset, int mode)
{
    Vector4 *rotation = TCAMERA_ROTATION(camera);
    float point[4];
    Vector4 direction;
    float yaw;
    float sine;
    float cosine;
    float pitchDenominator;

    if (mode == 0) {
        point[0] = (*reference_position)[1].x;
        point[1] = (*reference_position)[1].y;
        point[2] = (*reference_position)[1].z;
    } else if (mode == 1) {
        point[0] = (*reference_position)[0].x;
        point[1] = (*reference_position)[0].y;
        point[2] = (*reference_position)[0].z;
    } else {
        point[0] = (*reference_position)[1].x;
        point[1] = (*reference_position)[1].y;
        point[2] = (*reference_position)[1].z;
    }
    camera = (TCamera *)((u8 *)camera + TCAMERA_TRANSLATION_OFFSET);
    Vu0AddVectorXYZ(point, point, offset);
    Vu0SubVectorXYZ(&direction, camera, point);
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
    struct TCameraLoopState {
        unsigned char unmodeled_00[0xc8];
        unsigned short activeCameraIndex;
    };

    extern struct TCameraLoopState GameLoopState;
    int cameraIndex;
    float radiansPerHalfTurn;

    if ((PadData[0].half_2a & PAD_L1) != 0 && (PadData[0].half_28 & PAD_SELECT) != 0) {
        camDisp = (camDisp + 1) & 1;
    }

    radiansPerHalfTurn = 3.1415927f;
    for (cameraIndex = 0; cameraIndex < STUDIO_CAMERA_COUNT; cameraIndex++) {
        StudioCamera *studioCamera = xglStudioGetCamera2(cameraIndex);
        TCamera *camera = (TCamera *)studioCamera;
        Vector4 *position = &studioCamera->position.vector;
        Vector4 *rotation = &studioCamera->rotation.vector;

        if (camDisp != 0 && studioCamera->active != 0) {
            xglFontDebugPrintf(0, 0x10, translationVectorFormat, (double)position->x,
                               (double)position->y, (double)position->z);
            xglFontDebugPrintf(0, 0x18, rotationVectorFormat,
                               (double)(rotation->x / radiansPerHalfTurn * 180.0f),
                               (double)(rotation->y / radiansPerHalfTurn * 180.0f),
                               (double)(rotation->z / radiansPerHalfTurn * 180.0f));
            xglFontDebugPrintf(0, 0x20, fovFormat,
                               (double)(TCAMERA_FOV(studioCamera) /
                                        radiansPerHalfTurn * 180.0f));
        }

        if (studioCamera->state != STUDIO_CAMERA_STATE_EXTERNAL) {
            CameraWork *work = &tcamera[cameraIndex];

            switch (work->mode[CAMERA_CHANNEL_TRANSLATE]) {
            case CAMERA_MODE_MOTION_PACK2:
                TCAMERA_transMPack2(camera, work->translate.packed.source,
                                    work->translate.packed.frame);
                work->frame[CAMERA_CHANNEL_TRANSLATE]++;
                break;
            case CAMERA_MODE_MOTION_PACK:
                TCAMERA_transMPack(camera, work->translate.packed.source,
                                   (float)work->frame[CAMERA_CHANNEL_TRANSLATE]);
                work->frame[CAMERA_CHANNEL_TRANSLATE]++;
                break;
            case CAMERA_MODE_SPLINE:
                TCAMERA_transSPL(camera, &work->translate.spline,
                                 (float)work->frame[CAMERA_CHANNEL_TRANSLATE]);
                if (work->translate.spline.first_key >= 0) {
                    work->frame[CAMERA_CHANNEL_TRANSLATE]++;
                    if (work->translate.spline.last_key < work->frame[CAMERA_CHANNEL_TRANSLATE]) {
                        work->frame[CAMERA_CHANNEL_TRANSLATE] = work->translate.spline.last_key;
                    }
                } else {
                    work->frame[CAMERA_CHANNEL_TRANSLATE]++;
                }
                break;
            case CAMERA_MODE_CONSTANT:
                TCAMERA_transCNS(camera, &work->translate.constant.reference_position,
                                 &work->translate.constant.offset,
                                 work->translate.constant.reference_mode);
                break;
            }

            switch (work->mode[CAMERA_CHANNEL_VIEW]) {
            case CAMERA_MODE_MOTION_PACK:
                TCAMERA_viewMPack(camera, work->view.packed.source,
                                  (float)work->frame[CAMERA_CHANNEL_VIEW]);
                work->frame[CAMERA_CHANNEL_VIEW]++;
                break;
            case CAMERA_MODE_SPLINE:
                TCAMERA_viewSPL(camera, &work->view.spline,
                                (float)work->frame[CAMERA_CHANNEL_VIEW]);
                if (work->view.spline.first_key >= 0) {
                    work->frame[CAMERA_CHANNEL_VIEW]++;
                    if (work->view.spline.last_key < work->frame[CAMERA_CHANNEL_VIEW]) {
                        work->frame[CAMERA_CHANNEL_VIEW] = work->view.spline.last_key;
                    }
                } else {
                    work->frame[CAMERA_CHANNEL_VIEW]++;
                }
                break;
            case CAMERA_MODE_ROTATE_SPLINE:
                TCAMERA_rotateSPL(camera, &work->view.spline,
                                  (float)work->frame[CAMERA_CHANNEL_VIEW]);
                if (work->view.spline.first_key >= 0) {
                    work->frame[CAMERA_CHANNEL_VIEW]++;
                    if (work->view.spline.last_key < work->frame[CAMERA_CHANNEL_VIEW]) {
                        work->frame[CAMERA_CHANNEL_VIEW] = work->view.spline.last_key;
                    }
                } else {
                    work->frame[CAMERA_CHANNEL_VIEW]++;
                }
                break;
            case CAMERA_MODE_CONSTANT:
                TCAMERA_viewCNS(camera, &work->view.constant.reference_position,
                                &work->view.constant.offset,
                                work->view.constant.reference_mode);
                break;
            }

            if (work->mode[CAMERA_CHANNEL_ROLL] == CAMERA_MODE_SPLINE) {
                TCAMERA_rollSPL(camera, &work->roll.spline,
                                (float)work->frame[CAMERA_CHANNEL_ROLL]);
                work->frame[CAMERA_CHANNEL_ROLL]++;
            }
            if (work->mode[CAMERA_CHANNEL_FOV] == CAMERA_MODE_SPLINE) {
                TCAMERA_fovSPL(camera, &work->fov.spline,
                               (float)work->frame[CAMERA_CHANNEL_FOV]);
                work->frame[CAMERA_CHANNEL_FOV]++;
            }
        }

        if (studioCamera->active != 0) {
            float *fog = CfCameraDefine[GameLoopState.activeCameraIndex].fog_parameters;

            if (fog[0] != 0.0f && fog[1] != 0.0f) {
                nmlModelSetGlobalFogDist(fog[0], fog[1], fog[2], fog[3]);
                nmlModelSetGlobalFogCol(&CfCameraDefine[GameLoopState.activeCameraIndex].fog_color);
            }
        }
    }
}

void TCAMERA_init(void)
{
    CameraWork *cameras = tcamera;
    int cameraIndex;
    int modeIndex;

    for (cameraIndex = 0; cameraIndex < STUDIO_CAMERA_COUNT; cameraIndex++) {
        int *modes = cameras[cameraIndex].mode;
        for (modeIndex = 3; modeIndex >= 0; modeIndex--) {
            modes[modeIndex] = CAMERA_MODE_IDLE;
        }
    }
}

void TCAMERA_setFCurve(void)
{
}
