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

INCLUDE_ASM("asm/main/nonmatchings/xgl_camera", xglCameraMove);

static void xglCameraViewScreen(StudioCamera *camera);
static void xglCameraViewTravel(StudioCamera *camera, Vector4 *offset);
static void xglCameraViewVolume(StudioCamera *camera, Vector4 *offset);

void xglCameraMoveOffset(StudioCamera *camera, Vector4 *offset)
{
    xglCameraViewScreen(camera);
    xglCameraViewTravel(camera, offset);
    xglCameraViewVolume(camera, offset);
}

INCLUDE_ASM("asm/main/nonmatchings/xgl_camera", xglCameraSetWindow);

extern const float D_004D88D0;
extern const float D_004D88D4;

void xglCameraClipRangeDefault(StudioCamera *camera)
{
    camera->nearClip = D_004D88D0;
    camera->farClip = D_004D88D4;
}
