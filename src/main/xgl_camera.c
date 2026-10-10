#include "common.h"

#include "shared.h"

static void xglCameraScreenInit(StudioCamera *camera);

static void xglCameraTravelInit(StudioCamera *camera);

/* Move passes an unused second register argument; the callee reads only a0. */

static void xglCameraViewScreen(StudioCamera *camera, const Vector4 *offset);

static void xglCameraViewTravel(StudioCamera *camera, Vector4 *offset);

static void xglCameraViewVolume(StudioCamera *camera, Vector4 *offset);

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
    short width;
    short height;
    u8 unmodeled_08[0x54];
} RenderWindowLimits;

extern RenderWindowLimits sRender;

#include "xgl_camera.h"

static Vector4 sShootAngleA_10 = {0.0f, 0.0f, 0.0f, 1.0f};

static Vector4 sShootPlaceA_11 = {0.0f, 0.0f, 0.0f, 1.0f};

/* Two 0x68-byte pad records; TravelScale reads port 1's held mask. */
struct CameraPadRecord {
    u8 unmodeled_00[0x28];
    u16 held_buttons;
    u16 pressed_buttons;
    u8 unmodeled_2c[0x3c];
};

extern struct CameraPadRecord PadData[2];

extern void xglVectorScaleXYZ(Vector4 *destination, const Vector4 *source,
                              float scale);

static void xglCameraControlInit(StudioCamera *camera)
{
    camera->state = camera->active = 0;
}

INCLUDE_ASM("asm/main/nonmatchings/xgl_camera", xglCameraScreenInit);

INCLUDE_ASM("asm/main/nonmatchings/xgl_camera", xglCameraTravelInit);

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

INCLUDE_ASM("asm/main/nonmatchings/xgl_camera", xglCameraTravelFocus);

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

INCLUDE_ASM("asm/main/nonmatchings/xgl_camera", xglCameraTravelManual);

INCLUDE_ASM("asm/main/nonmatchings/xgl_camera", xglCameraTravelChase);

INCLUDE_ASM("asm/main/nonmatchings/xgl_camera", xglCameraTravelProc);

INCLUDE_ASM("asm/main/nonmatchings/xgl_camera", xglCameraViewScreen);

INCLUDE_ASM("asm/main/nonmatchings/xgl_camera", xglCameraViewTravel);

INCLUDE_ASM("asm/main/nonmatchings/xgl_camera", xglCameraViewVolume);

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


