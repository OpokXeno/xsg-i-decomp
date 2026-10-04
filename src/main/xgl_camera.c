#include "common.h"
#include "shared.h"

static void xglCameraControlInit(StudioCamera *camera)
{
    camera->state = camera->active = 0;
}

INCLUDE_ASM("asm/main/nonmatchings/xgl_camera", xglCameraScreenInit);

INCLUDE_ASM("asm/main/nonmatchings/xgl_camera", xglCameraTravelInit);

INCLUDE_ASM("asm/main/nonmatchings/xgl_camera", xglCameraTravelReset);

INCLUDE_ASM("asm/main/nonmatchings/xgl_camera", xglCameraTravelFocus);

INCLUDE_ASM("asm/main/nonmatchings/xgl_camera", xglCameraTravelScale);

INCLUDE_ASM("asm/main/nonmatchings/xgl_camera", xglCameraTravelManual);

INCLUDE_ASM("asm/main/nonmatchings/xgl_camera", xglCameraTravelChase);

INCLUDE_ASM("asm/main/nonmatchings/xgl_camera", xglCameraTravelProc);

INCLUDE_ASM("asm/main/nonmatchings/xgl_camera", xglCameraViewScreen);

INCLUDE_ASM("asm/main/nonmatchings/xgl_camera", xglCameraViewTravel);

INCLUDE_ASM("asm/main/nonmatchings/xgl_camera", xglCameraViewVolume);

static void xglCameraScreenInit(StudioCamera *camera);
static void xglCameraTravelInit(StudioCamera *camera);

void xglCameraInit(StudioCamera *camera)
{
    xglCameraControlInit(camera);
    xglCameraScreenInit(camera);
    xglCameraTravelInit(camera);
}

/* Move passes an unused second register argument; the callee reads only a0. */
static void xglCameraViewScreen();
static void xglCameraViewTravel(StudioCamera *camera, Vector4 *offset);
static void xglCameraViewVolume(StudioCamera *camera, Vector4 *offset);

void xglCameraMove(StudioCamera *camera)
{
    Vector4 offset;

    memset(&offset, 0, sizeof(offset));
    xglCameraViewScreen(camera, &offset);
    xglCameraViewTravel(camera, &offset);
    xglCameraViewVolume(camera, &offset);
}

static void xglCameraViewScreen(StudioCamera *camera);
static void xglCameraViewTravel(StudioCamera *camera, Vector4 *offset);
static void xglCameraViewVolume(StudioCamera *camera, Vector4 *offset);

void xglCameraMoveOffset(StudioCamera *camera, Vector4 *offset)
{
    xglCameraViewScreen(camera);
    xglCameraViewTravel(camera, offset);
    xglCameraViewVolume(camera, offset);
}

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
