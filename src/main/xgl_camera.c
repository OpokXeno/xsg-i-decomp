#include "common.h"
#include "shared.h"

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

static void xglCameraControlInit(StudioCamera *camera)
{
    camera->state = camera->active = 0;
}

static void xglCameraScreenInit(StudioCamera *studio)
{
    XglCamera *camera = (XglCamera *)studio;
    XglCameraScreen *screen = &camera->screen;

    screen->depthMax = 0xffffff;
    screen->depthMin = 0;
    __asm__ __volatile__("lq $2, 0(%1)\n\t"
                         "sq $2, 0(%0)"
                         : : "r"(&screen->clipRange), "r"(&sClipRange_0)
                         : "$2", "memory");
    __asm__ __volatile__("lq $2, 0(%1)\n\t"
                         "sq $2, 0(%0)"
                         : : "r"(&screen->portEffect), "r"(&sPortEffect_1)
                         : "$2", "memory");
    screen->resolution.x = sRender.width;
    screen->resolution.y = sRender.height;
    screen->resolution.z = sRender.width;
    screen->resolution.w = sRender.height;
    screen->window.x = 0.0f;
    screen->window.y = 0.0f;
    screen->window.z = sRender.width;
    screen->window.w = sRender.height;
}

static void xglCameraTravelInit(StudioCamera *studio)
{
    XglCamera *camera = (XglCamera *)studio;
    CameraTravel *travel = &camera->travel;

    travel->focus = 5.0f;
    travel->fov = 0.69813174f;
    travel->fov_velocity = 0.0f;
    travel->scale = 0.0f;
    __asm__ __volatile__("lq $2, 0(%1)\n\t"
                         "sq $2, 0(%0)"
                         : : "r"(&travel->rotation), "r"(&sShootAngleR_2)
                         : "$2", "memory");
    __asm__ __volatile__("lq $2, 0(%1)\n\t"
                         "sq $2, 0(%0)"
                         : : "r"(&travel->angle_velocity), "r"(&sShootAngleV_3)
                         : "$2", "memory");
    __asm__ __volatile__("lq $2, 0(%1)\n\t"
                         "sq $2, 0(%0)"
                         : : "r"(&travel->angle_delta), "r"(&sShootAngleA_4)
                         : "$2", "memory");
    __asm__ __volatile__("lq $2, 0(%1)\n\t"
                         "sq $2, 0(%0)"
                         : : "r"(&travel->position), "r"(&sShootPlaceR_5)
                         : "$2", "memory");
    __asm__ __volatile__("lq $2, 0(%1)\n\t"
                         "sq $2, 0(%0)"
                         : : "r"(&travel->place_velocity), "r"(&sShootPlaceV_6)
                         : "$2", "memory");
    __asm__ __volatile__("lq $2, 0(%1)\n\t"
                         "sq $2, 0(%0)"
                         : : "r"(&travel->place_delta), "r"(&sShootPlaceA_7)
                         : "$2", "memory");
    travel->chase_yaw = travel->rotation.y;
    travel->chase_distance = travel->position.z;
    __asm__ __volatile__("lq $2, 0(%1)\n\t"
                         "sq $2, 0(%0)"
                         : : "r"(&travel->chase_reference), "r"(&sChaseRefPos_8)
                         : "$2", "memory");
    __asm__ __volatile__("lq $2, 0(%1)\n\t"
                         "sq $2, 0(%0)"
                         : : "r"(&travel->chase_position), "r"(&sChaseIntPos_9)
                         : "$2", "memory");
}

static void xglCameraTravelReset(CameraTravel *travel)
{
    travel->scale = 0.0f;
    __asm__ __volatile__("lq $2, 0(%1)\nsq $2, 0(%0)"
                         : : "r"(&travel->angle_delta), "r"(&sShootAngleA_10)
                         : "$2", "memory");
    __asm__ __volatile__("lq $2, 0(%1)\nsq $2, 0(%0)"
                         : : "r"(&travel->place_delta), "r"(&sShootPlaceA_11)
                         : "$2", "memory");
}

static void xglCameraTravelFocus(CameraTravel *travel)
{
    if (PadData[1].pressed_buttons & 0x20) {
        if (travel->focus < 9.0f) {
            travel->focus += 1.0f;
        }
    }
    if (PadData[1].pressed_buttons & 0x80) {
        if (travel->focus > 1.0f) {
            travel->focus -= 1.0f;
        }
    }
    if (PadData[1].held_buttons & 0x8) {
        travel->scale = 0.0011170107f;
    }
    if (PadData[1].held_buttons & 0x2) {
        travel->scale = -0.0011170107f;
    }
}

static void xglCameraTravelScale(CameraTravel *travel)
{
    float angle_scale;
    Vector4 *angle = &travel->angle_delta;

    if (PadData[1].held_buttons & 0x40) {
        travel->scale *= 4.0f;
        xglVectorScaleXYZ(angle, angle, 4.0f);
        xglVectorScaleXYZ(&travel->place_delta, &travel->place_delta, 4.0f);
    }
    angle_scale = (travel->focus - 5.0f) * 0.08f + 1.0f;
    travel->scale *= angle_scale;
    xglVectorScaleXYZ(angle, angle, angle_scale);
    xglVectorScaleXYZ(&travel->place_delta, &travel->place_delta,
                      (travel->focus - 5.0f) * 0.24f + 1.0f);
}

static void xglCameraTravelManual(XglCamera *camera)
{
    CameraTravel *travel = &camera->travel;
    Vector4 axes[4];
    float damping;

    xglCameraTravelReset(travel);
    xglCameraTravelFocus(travel);
    if (PadData[1].analog_signed[1] < 0) {
        travel->angle_delta.x = (PadData[1].analog_signed[1] + PadData[1].analog_magnitude[1]) * -6.981317e-05f;
    }
    if (PadData[1].analog_signed[1] > 0) {
        travel->angle_delta.x = (PadData[1].analog_signed[1] - PadData[1].analog_magnitude[1]) * -6.981317e-05f;
    }
    if (PadData[1].analog_signed[0] < 0) {
        travel->angle_delta.y = (PadData[1].analog_signed[0] + PadData[1].analog_magnitude[0]) * -6.981317e-05f;
    }
    if (PadData[1].analog_signed[0] > 0) {
        travel->angle_delta.y = (PadData[1].analog_signed[0] - PadData[1].analog_magnitude[0]) * -6.981317e-05f;
    }
    if (PadData[1].held_buttons & 0x8000) {
        travel->angle_delta.z = 0.0022340214f;
    }
    if (PadData[1].held_buttons & 0x2000) {
        travel->angle_delta.z = -0.0022340214f;
    }
    if (PadData[1].analog_signed[2] < 0) {
        travel->place_delta.x = (PadData[1].analog_signed[2] + PadData[1].analog_magnitude[2]) * 0.0002f;
    }
    if (PadData[1].analog_signed[2] > 0) {
        travel->place_delta.x = (PadData[1].analog_signed[2] - PadData[1].analog_magnitude[2]) * 0.0002f;
    }
    if (PadData[1].held_buttons & 1) {
        travel->place_delta.y = -0.0064f;
    }
    if (PadData[1].held_buttons & 4) {
        travel->place_delta.y = 0.0064f;
    }
    if (PadData[1].analog_signed[3] < 0) {
        travel->place_delta.z = (PadData[1].analog_signed[3] + PadData[1].analog_magnitude[3]) * 0.0002f;
    }
    if (PadData[1].analog_signed[3] > 0) {
        travel->place_delta.z = (PadData[1].analog_signed[3] - PadData[1].analog_magnitude[3]) * 0.0002f;
    }
    damping = 0.8f;
    xglCameraTravelScale(travel);
    travel->fov_velocity = travel->fov_velocity * damping + travel->scale;
    travel->fov += travel->fov_velocity;
    xglVectorScaleXYZ(&travel->angle_velocity, &travel->angle_velocity, damping);
    xglVectorScaleXYZ(&travel->place_velocity, &travel->place_velocity, damping);
    __asm__ __volatile__("lqc2 vf3, 0(%1)\n\t"
                         "lqc2 vf2, 0(%0)\n\t"
                         "vadd.xyz vf2, vf2, vf3\n\t"
                         "sqc2 vf2, 0(%0)"
                         : : "r"(&travel->angle_velocity), "r"(&travel->angle_delta)
                         : "memory");
    xglMatrixStackUnit();
    xglMatrixStackRotY(travel->rotation.y);
    xglMatrixStackSave((float (*)[4])axes);
    xglVectorScaleAddXYZ(&travel->place_velocity, &axes[0], &travel->place_velocity, travel->place_delta.x);
    travel->place_velocity.y += travel->place_delta.y;
    xglVectorScaleAddXYZ(&travel->place_velocity, &axes[2], &travel->place_velocity, travel->place_delta.z);
    __asm__ __volatile__("lqc2 vf3, 0(%1)\n\t"
                         "lqc2 vf2, 0(%0)\n\t"
                         "vadd.xyz vf2, vf2, vf3\n\t"
                         "sqc2 vf2, 0(%0)"
                         : : "r"(&travel->rotation), "r"(&travel->angle_velocity) : "memory");
    __asm__ __volatile__("lqc2 vf3, 0(%1)\n\t"
                         "lqc2 vf2, 0(%0)\n\t"
                         "vadd.xyz vf2, vf2, vf3\n\t"
                         "sqc2 vf2, 0(%0)"
                         : : "r"(&travel->position), "r"(&travel->place_velocity) : "memory");
}

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
        travel->angle_delta.y = (PadData[1].analog_signed[2] + PadData[1].analog_magnitude[2]) * 6.981317e-05f;
    }
    if (PadData[1].analog_signed[2] > 0) {
        travel->angle_delta.y = (PadData[1].analog_signed[2] - PadData[1].analog_magnitude[2]) * 6.981317e-05f;
    }
    if (PadData[1].held_buttons & 1) {
        travel->place_delta.y = -0.0064f;
    }
    if (PadData[1].held_buttons & 4) {
        travel->place_delta.y = 0.0064f;
    }
    if (PadData[1].analog_signed[3] < 0) {
        travel->place_delta.z = (PadData[1].analog_signed[3] + PadData[1].analog_magnitude[3]) * 0.0002f;
    }
    if (PadData[1].analog_signed[3] > 0) {
        travel->place_delta.z = (PadData[1].analog_signed[3] - PadData[1].analog_magnitude[3]) * 0.0002f;
    }
    damping = 0.8f;
    xglCameraTravelScale(travel);
    travel->fov_velocity = travel->fov_velocity * damping + travel->scale;
    travel->fov += travel->fov_velocity;
    xglVectorScaleXYZ(&travel->angle_velocity, &travel->angle_velocity, damping);
    xglVectorScaleXYZ(&travel->place_velocity, &travel->place_velocity, damping);
    __asm__ __volatile__("lqc2 vf3, 0(%1)\n\t"
                         "lqc2 vf2, 0(%0)\n\t"
                         "vadd.xyz vf2, vf2, vf3\n\t"
                         "sqc2 vf2, 0(%0)"
                         : : "r"(&travel->angle_velocity), "r"(&travel->angle_delta) : "memory");
    __asm__ __volatile__("lqc2 vf3, 0(%1)\n\t"
                         "lqc2 vf2, 0(%0)\n\t"
                         "vadd.xyz vf2, vf2, vf3\n\t"
                         "sqc2 vf2, 0(%0)"
                         : : "r"(&travel->place_velocity), "r"(&travel->place_delta) : "memory");
    travel->chase_distance += travel->place_velocity.z;
    travel->chase_yaw += travel->angle_velocity.y;
    if (travel->chase_distance < 0.4f) {
        travel->chase_distance = 0.4f;
    }
    if (travel->chase_distance > 100.0f) {
        travel->chase_distance = 100.0f;
    }
    __asm__ __volatile__("lqc2 vf3, 0(%1)\n\t"
                         "lqc2 vf2, 0(%0)\n\t"
                         "vsub.xyz vf2, vf2, vf3\n\t"
                         "sqc2 vf2, 0(%2)"
                         : : "r"(&travel->chase_reference), "r"(&travel->chase_position), "r"(&offset)
                         : "memory");
    xglVectorLength(&offset.w, &offset);
    if (offset.w < 1.0f) {
        xglVectorScaleAddXYZ(&travel->chase_position, &offset, &travel->chase_position, 0.2f);
    } else {
        __asm__ __volatile__("lq $2, 0(%1)\n\t"
                             "sq $2, 0(%0)"
                             : : "r"(&travel->chase_position), "r"(&travel->chase_reference)
                             : "$2", "memory");
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
    if (travel->position.y < 0.4f) {
        travel->position.y = 0.4f;
    }
    if (travel->position.y > 100.0f) {
        travel->position.y = 100.0f;
    }
    __asm__ __volatile__("lqc2 vf3, 0(%1)\n\t"
                         "lqc2 vf2, 0(%0)\n\t"
                         "vsub.xyz vf2, vf2, vf3\n\t"
                         "sqc2 vf2, 0(%2)"
                         : : "r"(&travel->chase_position), "r"(&travel->position), "r"(&offset)
                         : "memory");
    xglVectorNormal(&offset, &offset);
    offset.z = -offset.z;
    travel->rotation.y = atan2f(offset.x, offset.z);
    yaw_sine = xglSin(travel->rotation.y);
    travel->rotation.x = atan2f(offset.y, offset.x * yaw_sine + offset.z * xglCos(travel->rotation.y));
    travel->rotation.y = -travel->rotation.y;
}

int xglCameraTravelProc(StudioCamera *studio)
{
    XglCamera *camera = (XglCamera *)studio;

    switch (camera->state) {
    case 0:
    case 1:
    case 4:
    default:
        break;
    case 2:
        if (!(PadData[1].held_buttons & 0x100)) {
            xglCameraTravelManual(camera);
            break;
        }
        if (PadData[1].pressed_buttons & 0x10) {
            xglCameraTravelInit(studio);
        }
        break;
    case 3:
        if (PadData[1].held_buttons & 0x100) {
            if (PadData[1].pressed_buttons & 0x10) {
                xglCameraTravelInit(studio);
            }
            break;
        }
        xglCameraTravelChase(camera);
        break;
    }
}

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
                       (4096.0f - __builtin_fabsf(2.0f * centerX)) / screen->resolution.z);
    } else {
        xglVectorScale(&scaled, &screen->resolution,
                       (4096.0f - __builtin_fabsf(2.0f * centerY)) / screen->resolution.w);
    }
    screen->clipRange.z = screen->projection.x * (scaled.z / screen->resolution.z);
    screen->clipRange.w = screen->projection.y * (scaled.w / screen->resolution.w);
    __asm__ __volatile__("lq $2, 0(%1)\n\t"
                         "sq $2, 0(%0)"
                         : : "r"(&screen->projection), "r"(&screen->resolution)
                         : "$2", "memory");
    screen->projection.x *= aspect;
    screen->screenOffset.x = centerX + 2048.0f;
    screen->projection.z *= aspect;
    screen->screenOffset.y = centerY + 2048.0f;
    screen->screenOffset.z = depth[0] + depth[1] * 0.5f;
    screen->screenOffset.w = (1.0f - screen->portEffect.z) * 255.0f;
    screen->screenScale.x = scaled.x * screen->portEffect.x * 0.5f;
    screen->screenScale.y = -(scaled.y * screen->portEffect.y * 0.5f);
    screen->screenScale.z = -(depth[1] * 0.5f);
    screen->screenScale.w = screen->portEffect.z * screen->portEffect.w * 255.0f;
}

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
    __asm__ __volatile__("lqc2 vf3, 0(%1)\n\t"
                         "lqc2 vf2, 0(%0)\n\t"
                         "vsub.xyz vf2, vf2, vf3\n\t"
                         "sqc2 vf2, 0(%2)"
                         : : "r"(&travel->position), "r"(offset), "r"(&shift) : "memory");
    xglMatrixStackTrans(&shift.x);
    xglMatrixStackRotY(travel->rotation.y);
    xglMatrixStackRotX(travel->rotation.x);
    xglMatrixStackRotZ(travel->rotation.z);
    xglMatrixStackSave(camera->transform);
    xglMatrixStackInverse();
    xglMatrixStackSave(camera->inverseTransform);
}

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
        __asm__ __volatile__("lq $2, 0(%1)\n\t"
                             "sq $2, 0(%0)\n\t"
                             "lq $2, 16(%1)\n\t"
                             "sq $2, 16(%0)\n\t"
                             "lq $2, 32(%1)\n\t"
                             "sq $2, 32(%0)\n\t"
                             "lq $2, 48(%1)\n\t"
                             "sq $2, 48(%0)"
                             : : "r"(volume->frustum), "r"(travelFrustum)
                             : "$2", "memory");
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

void xglCameraMove(StudioCamera *camera)
{
    Vector4 offset;

    memset(&offset, 0, sizeof(offset));
    xglCameraViewScreen(camera, &offset);
    xglCameraViewTravel(camera, &offset);
    xglCameraViewVolume(camera, &offset);
}

void xglCameraMoveOffset(StudioCamera *camera, Vector4 *offset)
{
    xglCameraViewScreen(camera, offset);
    xglCameraViewTravel(camera, offset);
    xglCameraViewVolume(camera, offset);
}

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
    camera->nearClip = 0.01f;
    camera->farClip = 99000.0f;
}
