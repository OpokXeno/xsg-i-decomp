#include "common.h"
#include "shared.h"
#include "quadword.h"
#include "vu0_macro.h"

/* Studio camera states handled by xglCameraControl. */
#define CAMERA_STATE_TRAVEL_MANUAL 2 /* pad-flown free camera */
#define CAMERA_STATE_TRAVEL_CHASE 3 /* follows chase_reference at chase_distance */

/* The camera draws into the GS 4096x4096 primitive space centred on 2048. */
#define CAMERA_GS_COORD_SIZE 4096.0f
#define CAMERA_GS_COORD_CENTER 2048.0f
/* 24-bit depth buffer. */
#define CAMERA_DEPTH_MAX 0xffffff

/* Free-camera tuning: focus steps between the limits and 5.0f is the neutral focus. */
#define CAMERA_FOCUS_DEFAULT 5.0f
#define CAMERA_FOCUS_MAX 9.0f
#define CAMERA_FOCUS_MIN 1.0f
#define CAMERA_FOV_DEFAULT 0.69813174f
#define CAMERA_FOV_STEP 0.0011170107f
#define CAMERA_BOOST_SCALE 4.0f
#define CAMERA_TURN_STEP_PITCH_YAW 6.981317e-05f
#define CAMERA_TURN_STEP_ROLL 0.0022340214f
#define CAMERA_MOVE_STEP_STICK 0.0002f
#define CAMERA_MOVE_STEP_HEIGHT 0.0064f
#define CAMERA_VELOCITY_DAMPING 0.8f
/* Chase camera distance and height limits. */
#define CAMERA_CHASE_DISTANCE_MIN 0.4f
#define CAMERA_CHASE_DISTANCE_MAX 100.0f
#define CAMERA_CHASE_HEIGHT_MIN 0.4f
#define CAMERA_CHASE_HEIGHT_MAX 100.0f
#define CAMERA_CHASE_FOLLOW_RATE 0.2f
/* Default clip planes. */
#define CAMERA_NEAR_CLIP_DEFAULT 0.01f
#define CAMERA_FAR_CLIP_DEFAULT 99000.0f

typedef struct XglCamera XglCamera;

typedef struct XglCameraScreen XglCameraScreen;

/* The screen subobject begins at +0x10; its viewport occupies +0x60..+0x6f. */
typedef struct CameraScreen {
    u8 unmodeled_00[0x50];
    Vector4 window;
} CameraScreen;

typedef struct CameraWindowData {
    u8 unmodeled_00[0x10];
    CameraScreen screen;
} CameraWindowData;

/* sRender is 0x5c bytes; its signed screen dimensions are at +4 and +6. */
typedef struct RenderWindowLimits {
    u8 unmodeled_00[4];
    union {
        struct {
            short width;
            short height;
        };
        int packed_size;
    };
    u8 unmodeled_08[0x54];
} RenderWindowLimits;

/* Two 0x68-byte pad records; TravelScale reads port 1's held mask. */
struct CameraPadRecord {
    u8 unmodeled_00[0x28];
    union {
        u16 held_buttons;
        u8 held_bytes[2];
    };
    u16 pressed_buttons;
    u8 unmodeled_2c[0x34];
    u8 analog_magnitude[4];
    signed char analog_signed[4];
};

/* DISPLAY register of the GS display environment at +0x18. */
typedef struct CameraDisplayEnv {
    u8 unmodeled_00[0x18];
    long display;
    u8 unmodeled_20[0x30];
} CameraDisplayEnv;

extern RenderWindowLimits sRender;
extern struct CameraPadRecord PadData[2];
extern CameraDisplayEnv DispEnv;

static void xglCameraScreenInit(StudioCamera *studio);
static void xglCameraTravelInit(StudioCamera *studio);

/* Move passes an unused second register argument; the callee reads only a0. */
static void xglCameraViewScreen(StudioCamera *studio, const Vector4 *offset);
static void xglCameraViewTravel(StudioCamera *studio, Vector4 *offset);
static void xglCameraViewVolume(StudioCamera *studio, Vector4 *offset);

extern void xglVectorScaleXYZ(Vector4 *destination, const Vector4 *source,
                              float scale);

extern void xglVectorNormal(Vector4 *destination, const Vector4 *source);
extern float atan2f(float y, float x);
extern float tanf(float angle);
extern void xglMatrixStackInverse(void);
extern void xglMatrixStackMul(Matrix4 matrix);

extern void xglMatrixStackFrustum(float left, float right, float bottom,
                                  float top, float near, float far);

extern void xglVectorMulMat(Vector4 *destination, Matrix4 matrix,
                            const Vector4 *vector);

extern void xglPlaneParameter(Vector4 *destination, const Vector4 *p0,
                              const Vector4 *p1, const Vector4 *p2);

extern void xglVectorScale(Vector4 *destination, const Vector4 *source,
                           float scale);

extern float xglSin(float angle);
extern float xglCos(float angle);

extern void xglVectorScaleAddXYZ(Vector4 *destination, const Vector4 *source,
                                 const Vector4 *other, float scale);

static Vector4 sClipRange_0;

static Vector4 sPortEffect_1;

static Vector4 sShootAngleR_2;

static Vector4 sShootAngleV_3;

static Vector4 sShootAngleA_4;

static Vector4 sShootPlaceR_5;

static Vector4 sShootPlaceV_6;

static Vector4 sShootPlaceA_7;

static Vector4 sChaseRefPos_8;

static Vector4 sChaseIntPos_9;

static Vector4 sShootAngleA_10 = {0.0f, 0.0f, 0.0f, 1.0f};

static Vector4 sShootPlaceA_11 = {0.0f, 0.0f, 0.0f, 1.0f};

static signed char aaiVertexIndex[6][3];

static Vector4 sClipRange_0 = { .x = 0.0099999997764825821f, .y = 99000.0f, .z = 0.0f, .w = 0.0f };

static Vector4 sPortEffect_1 = { .x = 1.0f, .y = 1.0f, .z = 1.0f, .w = 100.0f };

static Vector4 sShootAngleR_2 = { .x = 0.0f, .y = 0.0f, .z = 0.0f, .w = 1.0f };

static Vector4 sShootAngleV_3 = { .x = 0.0f, .y = 0.0f, .z = 0.0f, .w = 1.0f };

static Vector4 sShootAngleA_4 = { .x = 0.0f, .y = 0.0f, .z = 0.0f, .w = 1.0f };

static Vector4 sShootPlaceR_5 = { .x = 0.0f, .y = 1.0f, .z = 4.0f, .w = 1.0f };

static Vector4 sShootPlaceV_6 = { .x = 0.0f, .y = 0.0f, .z = 0.0f, .w = 1.0f };

static Vector4 sShootPlaceA_7 = { .x = 0.0f, .y = 0.0f, .z = 0.0f, .w = 1.0f };

static Vector4 sChaseRefPos_8 = { .x = 0.0f, .y = 1.0f, .z = 0.0f, .w = 1.0f };

static Vector4 sChaseIntPos_9 = { .x = 0.0f, .y = 1.0f, .z = 0.0f, .w = 1.0f };

static signed char aaiVertexIndex[6][3] = { { 0, 2, 4 }, { 1, 0, 5 }, { 0, 1, 2 }, { 3, 1, 7 }, { 2, 3, 6 }, { 4, 6, 5 } };

/* Declared here, not at the top: moving it up changes the original code. */
#include "xgl_camera.h"
#include "main/xgl_camera.h"

static void xglCameraControlInit(StudioCamera *camera)
{
    camera->state = camera->active = 0;
}

/* Sets the default depth range, clip range, port effect, resolution and window of the camera screen from sRender. */
static void xglCameraScreenInit(StudioCamera *studio)
{
    XglCamera *camera = (XglCamera *)studio;
    XglCameraScreen *screen = &camera->screen;

    screen->depthMax = CAMERA_DEPTH_MAX;
    screen->depthMin = 0;
    QuadwordCopy(&screen->clipRange, &sClipRange_0);
    QuadwordCopy(&screen->portEffect, &sPortEffect_1);
    screen->resolution.x = sRender.width;
    screen->resolution.y = sRender.height;
    screen->resolution.z = sRender.width;
    screen->resolution.w = sRender.height;
    screen->window.x = 0.0f;
    screen->window.y = 0.0f;
    screen->window.z = sRender.width;
    screen->window.w = sRender.height;
}

/* Sets the travel state (focus, fov, rotation, position and chase point) to the default vectors. */
static void xglCameraTravelInit(StudioCamera *studio)
{
    XglCamera *camera = (XglCamera *)studio;
    CameraTravel *travel = &camera->travel;

    travel->focus = CAMERA_FOCUS_DEFAULT;
    travel->fov = CAMERA_FOV_DEFAULT;
    travel->fov_velocity = 0.0f;
    travel->scale = 0.0f;
    QuadwordCopy(&travel->rotation, &sShootAngleR_2);
    QuadwordCopy(&travel->angle_velocity, &sShootAngleV_3);
    QuadwordCopy(&travel->angle_delta, &sShootAngleA_4);
    QuadwordCopy(&travel->position, &sShootPlaceR_5);
    QuadwordCopy(&travel->place_velocity, &sShootPlaceV_6);
    QuadwordCopy(&travel->place_delta, &sShootPlaceA_7);
    travel->chase_yaw = travel->rotation.y;
    travel->chase_distance = travel->position.z;
    QuadwordCopy(&travel->chase_reference, &sChaseRefPos_8);
    QuadwordCopy(&travel->chase_position, &sChaseIntPos_9);
}

static void xglCameraTravelReset(CameraTravel *travel)
{
    travel->scale = 0.0f;
    QuadwordCopy(&travel->angle_delta, &sShootAngleA_10);
    QuadwordCopy(&travel->place_delta, &sShootPlaceA_11);
}

/* Steps focus with Circle and Square within its limits, and sets the zoom scale while R1 or R2 is held. */
static void xglCameraTravelFocus(CameraTravel *travel)
{
    if (PadData[1].pressed_buttons & PAD_CIRCLE) {
        if (travel->focus < CAMERA_FOCUS_MAX) {
            travel->focus += 1.0f;
        }
    }
    if (PadData[1].pressed_buttons & PAD_SQUARE) {
        if (travel->focus > CAMERA_FOCUS_MIN) {
            travel->focus -= 1.0f;
        }
    }
    if (PadData[1].held_buttons & PAD_R1) {
        travel->scale = CAMERA_FOV_STEP;
    }
    if (PadData[1].held_buttons & PAD_R2) {
        travel->scale = -CAMERA_FOV_STEP;
    }
}

/* Scales the angle and place deltas by the focus, and by the boost factor while Cross is held. */
static void xglCameraTravelScale(CameraTravel *travel)
{
    float angle_scale;
    Vector4 *angle = &travel->angle_delta;

    if (PadData[1].held_buttons & PAD_CROSS) {
        travel->scale *= CAMERA_BOOST_SCALE;
        xglVectorScaleXYZ(angle, angle, CAMERA_BOOST_SCALE);
        xglVectorScaleXYZ(&travel->place_delta, &travel->place_delta, CAMERA_BOOST_SCALE);
    }
    angle_scale = (travel->focus - CAMERA_FOCUS_DEFAULT) * 0.08f + 1.0f;
    travel->scale *= angle_scale;
    xglVectorScaleXYZ(angle, angle, angle_scale);
    xglVectorScaleXYZ(&travel->place_delta, &travel->place_delta,
                      (travel->focus - CAMERA_FOCUS_DEFAULT) * 0.24f + 1.0f);
}

/*
 * Free-camera travel for one frame: pad sticks and buttons set the angle and
 * place deltas, which are integrated into the rotation and position.
 */
static void xglCameraTravelManual(XglCamera *camera)
{
    CameraTravel *travel = &camera->travel;
    Vector4 axes[4];
    float damping;

    xglCameraTravelReset(travel);
    xglCameraTravelFocus(travel);
    if (PadData[1].analog_signed[1] < 0) {
        travel->angle_delta.x = (PadData[1].analog_signed[1] + PadData[1].analog_magnitude[1]) * -CAMERA_TURN_STEP_PITCH_YAW;
    }
    if (PadData[1].analog_signed[1] > 0) {
        travel->angle_delta.x = (PadData[1].analog_signed[1] - PadData[1].analog_magnitude[1]) * -CAMERA_TURN_STEP_PITCH_YAW;
    }
    if (PadData[1].analog_signed[0] < 0) {
        travel->angle_delta.y = (PadData[1].analog_signed[0] + PadData[1].analog_magnitude[0]) * -CAMERA_TURN_STEP_PITCH_YAW;
    }
    if (PadData[1].analog_signed[0] > 0) {
        travel->angle_delta.y = (PadData[1].analog_signed[0] - PadData[1].analog_magnitude[0]) * -CAMERA_TURN_STEP_PITCH_YAW;
    }
    if (PadData[1].held_buttons & PAD_LEFT) {
        travel->angle_delta.z = CAMERA_TURN_STEP_ROLL;
    }
    if (PadData[1].held_buttons & PAD_RIGHT) {
        travel->angle_delta.z = -CAMERA_TURN_STEP_ROLL;
    }
    if (PadData[1].analog_signed[2] < 0) {
        travel->place_delta.x = (PadData[1].analog_signed[2] + PadData[1].analog_magnitude[2]) * CAMERA_MOVE_STEP_STICK;
    }
    if (PadData[1].analog_signed[2] > 0) {
        travel->place_delta.x = (PadData[1].analog_signed[2] - PadData[1].analog_magnitude[2]) * CAMERA_MOVE_STEP_STICK;
    }
    if (PadData[1].held_buttons & PAD_L2) {
        travel->place_delta.y = -CAMERA_MOVE_STEP_HEIGHT;
    }
    if (PadData[1].held_buttons & PAD_L1) {
        travel->place_delta.y = CAMERA_MOVE_STEP_HEIGHT;
    }
    if (PadData[1].analog_signed[3] < 0) {
        travel->place_delta.z = (PadData[1].analog_signed[3] + PadData[1].analog_magnitude[3]) * CAMERA_MOVE_STEP_STICK;
    }
    if (PadData[1].analog_signed[3] > 0) {
        travel->place_delta.z = (PadData[1].analog_signed[3] - PadData[1].analog_magnitude[3]) * CAMERA_MOVE_STEP_STICK;
    }
    /* Damp the velocities, add the deltas, then integrate them into rotation and position. */
    damping = CAMERA_VELOCITY_DAMPING;
    xglCameraTravelScale(travel);
    travel->fov_velocity = travel->fov_velocity * damping + travel->scale;
    travel->fov += travel->fov_velocity;
    xglVectorScaleXYZ(&travel->angle_velocity, &travel->angle_velocity, damping);
    xglVectorScaleXYZ(&travel->place_velocity, &travel->place_velocity, damping);
    Vu0AddVectorXYZ(&travel->angle_velocity, &travel->angle_velocity, &travel->angle_delta);
    xglMatrixStackUnit();
    xglMatrixStackRotY(travel->rotation.y);
    xglMatrixStackSave((float (*)[4])axes);
    xglVectorScaleAddXYZ(&travel->place_velocity, &axes[0], &travel->place_velocity, travel->place_delta.x);
    travel->place_velocity.y += travel->place_delta.y;
    xglVectorScaleAddXYZ(&travel->place_velocity, &axes[2], &travel->place_velocity, travel->place_delta.z);
    Vu0AddVectorXYZ(&travel->rotation, &travel->rotation, &travel->angle_velocity);
    Vu0AddVectorXYZ(&travel->position, &travel->position, &travel->place_velocity);
}

/*
 * Chase-camera travel for one frame: the chase point follows chase_reference,
 * distance and height are clamped, and the rotation aims from the position at
 * the chase point.
 */
static void xglCameraTravelChase(XglCamera *camera)
{
    CameraTravel *travel = &camera->travel;
    Vector4 offset;
    Vector4 placement[4];
    float damping;
    float yaw_sine;

    xglCameraTravelReset(travel);
    xglCameraTravelFocus(travel);
    if (PadData[1].analog_signed[2] < 0) {
        travel->angle_delta.y = (PadData[1].analog_signed[2] + PadData[1].analog_magnitude[2]) * CAMERA_TURN_STEP_PITCH_YAW;
    }
    if (PadData[1].analog_signed[2] > 0) {
        travel->angle_delta.y = (PadData[1].analog_signed[2] - PadData[1].analog_magnitude[2]) * CAMERA_TURN_STEP_PITCH_YAW;
    }
    if (PadData[1].held_buttons & PAD_L2) {
        travel->place_delta.y = -CAMERA_MOVE_STEP_HEIGHT;
    }
    if (PadData[1].held_buttons & PAD_L1) {
        travel->place_delta.y = CAMERA_MOVE_STEP_HEIGHT;
    }
    if (PadData[1].analog_signed[3] < 0) {
        travel->place_delta.z = (PadData[1].analog_signed[3] + PadData[1].analog_magnitude[3]) * CAMERA_MOVE_STEP_STICK;
    }
    if (PadData[1].analog_signed[3] > 0) {
        travel->place_delta.z = (PadData[1].analog_signed[3] - PadData[1].analog_magnitude[3]) * CAMERA_MOVE_STEP_STICK;
    }
    damping = CAMERA_VELOCITY_DAMPING;
    xglCameraTravelScale(travel);
    travel->fov_velocity = travel->fov_velocity * damping + travel->scale;
    travel->fov += travel->fov_velocity;
    xglVectorScaleXYZ(&travel->angle_velocity, &travel->angle_velocity, damping);
    xglVectorScaleXYZ(&travel->place_velocity, &travel->place_velocity, damping);
    Vu0AddVectorXYZ(&travel->angle_velocity, &travel->angle_velocity, &travel->angle_delta);
    Vu0AddVectorXYZ(&travel->place_velocity, &travel->place_velocity, &travel->place_delta);
    travel->chase_distance += travel->place_velocity.z;
    travel->chase_yaw += travel->angle_velocity.y;
    if (travel->chase_distance < CAMERA_CHASE_DISTANCE_MIN) {
        travel->chase_distance = CAMERA_CHASE_DISTANCE_MIN;
    }
    if (travel->chase_distance > CAMERA_CHASE_DISTANCE_MAX) {
        travel->chase_distance = CAMERA_CHASE_DISTANCE_MAX;
    }
    /* Move the chase point toward the reference by CAMERA_CHASE_FOLLOW_RATE while it is within 1.0, otherwise snap to it. */
    Vu0SubVectorXYZ(&offset, &travel->chase_reference, &travel->chase_position);
    xglVectorLength(&offset.w, &offset);
    if (offset.w < 1.0f) {
        xglVectorScaleAddXYZ(&travel->chase_position, &offset, &travel->chase_position, CAMERA_CHASE_FOLLOW_RATE);
    } else {
        QuadwordCopy(&travel->chase_position, &travel->chase_reference);
    }
    offset.x = 0.0f;
    offset.y = 0.0f;
    offset.z = travel->chase_distance;
    offset.w = 1.0f;
    xglMatrixStackUnit();
    xglMatrixStackTrans(&travel->chase_position.x);
    xglMatrixStackRotY(travel->chase_yaw);
    xglMatrixStackTrans(&offset.x);
    xglMatrixStackSave((float (*)[4])placement);
    travel->position.y += travel->place_velocity.y;
    travel->position.x = placement[3].x;
    travel->position.z = placement[3].z;
    if (travel->position.y < CAMERA_CHASE_HEIGHT_MIN) {
        travel->position.y = CAMERA_CHASE_HEIGHT_MIN;
    }
    if (travel->position.y > CAMERA_CHASE_HEIGHT_MAX) {
        travel->position.y = CAMERA_CHASE_HEIGHT_MAX;
    }
    /* Derive yaw and pitch of the direction from position to the chase point. */
    Vu0SubVectorXYZ(&offset, &travel->chase_position, &travel->position);
    xglVectorNormal(&offset, &offset);
    offset.z = -offset.z;
    travel->rotation.y = atan2f(offset.x, offset.z);
    yaw_sine = xglSin(travel->rotation.y);
    travel->rotation.x = atan2f(offset.y, offset.x * yaw_sine + offset.z * xglCos(travel->rotation.y));
    travel->rotation.y = -travel->rotation.y;
}

/*
 * Runs the travel update for the camera's state: manual flight (state 2) or chase
 * (state 3). Holding Select suspends the update, and Select with Triangle resets the travel.
 */
int xglCameraTravelProc(StudioCamera *studio)
{
    XglCamera *camera = (XglCamera *)studio;

    switch (camera->state) {
    case 0:
    case 1:
    case 4:
    default:
        break;
    case CAMERA_STATE_TRAVEL_MANUAL:
        if (!(PadData[1].held_buttons & PAD_SELECT)) {
            xglCameraTravelManual(camera);
            break;
        }
        if (PadData[1].pressed_buttons & PAD_TRIANGLE) {
            xglCameraTravelInit(studio);
        }
        break;
    case CAMERA_STATE_TRAVEL_CHASE:
        if (PadData[1].held_buttons & PAD_SELECT) {
            if (PadData[1].pressed_buttons & PAD_TRIANGLE) {
                xglCameraTravelInit(studio);
            }
            break;
        }
        xglCameraTravelChase(camera);
        break;
    }
}

/* Computes the screen projection, screen offset and scale, and the clip range of the camera from its window and resolution. */
static void xglCameraViewScreen(StudioCamera *studio, const Vector4 *offset)
{
    XglCamera *camera = (XglCamera *)studio;
    XglCameraScreen *screen = &camera->screen;
    float depth[2];
    unsigned int near = screen->depthMin;
    unsigned int range;
    Vector4 scaled;
    float aspect;
    float magnify;
    float centerX;
    float centerY;

    centerX = (screen->window.z + screen->window.x) * 0.5f - screen->resolution.x * 0.5f;
    magnify = 54.0f / (float)(((int)(DispEnv.display >> 23) & 0xf) + 1);
    aspect = 12.272727f / magnify;
    centerY = (screen->window.w + screen->window.y) * 0.5f - screen->resolution.y * 0.5f;
    if ((int)near >= 0) {
        depth[0] = (int)near;
    } else {
        float half = (float)(int)((near & 1) | (near >> 1));
        depth[0] = half + half;
    }
    range = screen->depthMax - near;
    if ((int)range >= 0) {
        depth[1] = (int)range;
    } else {
        float half = (float)(int)((range & 1) | (range >> 1));
        depth[1] = half + half;
    }
    if (screen->resolution.z > screen->resolution.w) {
        xglVectorScale(&scaled, &screen->resolution,
                       (CAMERA_GS_COORD_SIZE - __builtin_fabsf(2.0f * centerX)) / screen->resolution.z);
    } else {
        xglVectorScale(&scaled, &screen->resolution,
                       (CAMERA_GS_COORD_SIZE - __builtin_fabsf(2.0f * centerY)) / screen->resolution.w);
    }
    screen->clipRange.z = screen->projection.x * (scaled.z / screen->resolution.z);
    screen->clipRange.w = screen->projection.y * (scaled.w / screen->resolution.w);
    QuadwordCopy(&screen->projection, &screen->resolution);
    screen->projection.x *= aspect;
    screen->screenOffset.x = centerX + CAMERA_GS_COORD_CENTER;
    screen->projection.z *= aspect;
    screen->screenOffset.y = centerY + CAMERA_GS_COORD_CENTER;
    screen->screenOffset.z = depth[0] + depth[1] * 0.5f;
    screen->screenOffset.w = (1.0f - screen->portEffect.z) * 255.0f;
    screen->screenScale.x = scaled.x * screen->portEffect.x * 0.5f;
    screen->screenScale.y = -(scaled.y * screen->portEffect.y * 0.5f);
    screen->screenScale.z = -(depth[1] * 0.5f);
    screen->screenScale.w = screen->portEffect.z * screen->portEffect.w * 255.0f;
}

/*
 * Computes the travel frustum from fov and the clip range, then the camera
 * transform and its inverse from the travel position and rotation, shifted by offset.
 */
static void xglCameraViewTravel(StudioCamera *studio, Vector4 *offset)
{
    XglCamera *camera = (XglCamera *)studio;
    Vector4 *frustum = &camera->travel.frustum[3];
    XglCameraScreen *screen = &camera->screen;
    CameraTravel *travel = &camera->travel;
    Vector4 extent;
    Vector4 ratio;
    Vector4 shift;
    float tangent;
    float nearClip;

    tangent = tanf(travel->fov * 0.5f);
    extent.y = extent.x = tangent;
    extent.w = extent.z = tangent * (screen->clipRange.w / screen->projection.y);
    ratio.y = ratio.x = screen->projection.x / screen->projection.y;
    nearClip = screen->clipRange.x;
    ratio.w = ratio.z = screen->clipRange.z / screen->clipRange.w;
    travel->frustum[3].x = nearClip;
    travel->frustum[3].w = travel->frustum[3].y = screen->clipRange.y;
    travel->frustum[3].z = nearClip;
    __asm__ __volatile__("lqc2 vf20, 0(%0)" : : "r"(frustum) : "memory");
    __asm__ __volatile__("lqc2 vf21, 0(%0)" : : "r"(&extent) : "memory");
    __asm__ __volatile__("vmul.xyzw vf20, vf20, vf21" : : : "memory");
    __asm__ __volatile__("sqc2 vf20, 0(%0)" : : "r"(frustum) : "memory");
    xglVectorScale(&camera->travel.frustum[2], frustum, -1.0f);
    __asm__ __volatile__("lqc2 vf20, 0(%1)\n\t"
                         "lqc2 vf21, 0(%2)\n\t"
                         "vmul.xyzw vf20, vf20, vf21\n\t"
                         "sqc2 vf20, 0(%0)"
                         : : "r"(&camera->travel.frustum[1]), "r"(frustum), "r"(&ratio) : "memory");
    xglVectorScale(&camera->travel.frustum[0], &camera->travel.frustum[1], -1.0f);
    xglMatrixStackUnit();
    Vu0SubVectorXYZ(&shift, &travel->position, offset);
    xglMatrixStackTrans(&shift.x);
    xglMatrixStackRotY(travel->rotation.y);
    xglMatrixStackRotX(travel->rotation.x);
    xglMatrixStackRotZ(travel->rotation.z);
    xglMatrixStackSave(camera->transform);
    xglMatrixStackInverse();
    xglMatrixStackSave(camera->inverseTransform);
}

/*
 * Builds the view volume: the frustum matrix, then the eight corners and six
 * planes of each of its two sets, and the packed plane set.
 */
static void xglCameraViewVolume(StudioCamera *studio, Vector4 *offset)
{
    XglCamera *camera = (XglCamera *)studio;
    XglCameraScreen *screen = &camera->screen;
    CameraVolume *volume;
    Vector4 corner;
    int i;
    int j;
    int k;
    int edge;
    float gx;
    float gy;
    float gz;
    float (*viewInverse)[4];
    float (*transform)[4];
    Vector4 *travelFrustum;

    i = 0;
    travelFrustum = camera->travel.frustum;
    viewInverse = camera->inverseTransform;
    transform = camera->transform;
    for (; i < 1; i++) {
        volume = &camera->volume[i];
        QuadwordCopyMatrix(volume->frustum, travelFrustum);
        xglMatrixStackUnit();
        xglMatrixStackFrustum(volume->frustum[0][2], volume->frustum[1][2],
                              volume->frustum[2][2], volume->frustum[3][2],
                              screen->clipRange.x, screen->clipRange.y);
        xglMatrixStackMul(viewInverse);
        xglMatrixStackSave(volume->viewMatrix);
        for (j = 0; j < 2; j++) {
            for (k = 0; k < 8; k++) {
                edge = (((k & 4) >> 2) | ((j & 1) << 1));
                gx = volume->frustum[k & 1][edge];
                gy = volume->frustum[((k & 2) >> 1) | 2][edge];
                gz = (&screen->clipRange.x)[edge & 1];
                corner.x = gx;
                corner.y = gy;
                corner.z = -gz;
                corner.w = 1.0f;
                xglVectorMulMat(&volume->vertex[j * 8 + k].vector, transform, &corner);
            }
            for (k = 0; k < 6; k++) {
                xglPlaneParameter(&(volume->plane + j * 6)[k].vector,
                                  &(volume->vertex + j * 8)[aaiVertexIndex[k][0]].vector,
                                  &(volume->vertex + j * 8)[aaiVertexIndex[k][1]].vector,
                                  &(volume->vertex + j * 8)[aaiVertexIndex[k][2]].vector);
            }
        }
        for (j = 0; j < 4; j++) {
            for (k = 0; k < 3; k++) {
                volume->planeSet[0][j][k] = volume->plane[k].element[j];
                volume->planeSet[1][j][k] = volume->plane[k + 3].element[j];
                volume->planeSet[2][j][k] = volume->plane[k + 6].element[j];
                volume->planeSet[3][j][k] = volume->plane[k + 9].element[j];
            }
        }
    }
}

void xglCameraInit(StudioCamera *camera)
{
    xglCameraControlInit(camera);
    xglCameraScreenInit(camera);
    xglCameraTravelInit(camera);
}

/* Updates the camera's screen, travel and view volume with no offset. */
void xglCameraMove(StudioCamera *camera)
{
    Vector4 offset;

    memset(&offset, 0, sizeof(offset));
    xglCameraViewScreen(camera, &offset);
    xglCameraViewTravel(camera, &offset);
    xglCameraViewVolume(camera, &offset);
}

/* Same as xglCameraMove, with the travel position shifted by offset. */
void xglCameraMoveOffset(StudioCamera *camera, Vector4 *offset)
{
    xglCameraViewScreen(camera, offset);
    xglCameraViewTravel(camera, offset);
    xglCameraViewVolume(camera, offset);
}

/*
 * Sets the screen window of camera to the rectangle (left, top)-(right, bottom).
 * Reversed edges are swapped and each edge is clamped to the render size.
 * Does nothing when camera is null.
 */
void xglCameraSetWindow(CameraWindowData *camera, int left, int top, int right, int bottom)
{
    int swapCoordinate;
    CameraScreen *screen;

    if (camera == 0) {
        return;
    }
    screen = &camera->screen;
    if (right < left) {
        swapCoordinate = right;
        right = left;
        left = swapCoordinate;
    }
    if (bottom < top) {
        swapCoordinate = bottom;
        bottom = top;
        top = swapCoordinate;
    }
    if (left < 0) left = 0;
    if (right < 0) right = 0;
    if (top < 0) top = 0;
    if (bottom < 0) bottom = 0;
    if (left >= sRender.width) left = sRender.width - 1;
    if (top >= sRender.height) top = sRender.height - 1;
    if (right >= sRender.width) right = sRender.width - 1;
    if (bottom >= sRender.height) bottom = sRender.height - 1;
    screen->window.x = (float)left;
    screen->window.y = (float)top;
    screen->window.z = (float)right;
    screen->window.w = (float)bottom;
}

void xglCameraClipRangeDefault(StudioCamera *camera)
{
    camera->nearClip = CAMERA_NEAR_CLIP_DEFAULT;
    camera->farClip = CAMERA_FAR_CLIP_DEFAULT;
}
