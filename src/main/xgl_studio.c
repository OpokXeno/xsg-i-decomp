#include "common.h"
#include "main/xgl_thread.h"
#include "shared.h"
#include "xgl_studio.h"

/*
 * StudioCamera (include/shared.h; config/header-canon.json,
 * decisions.StudioCamera) is the active-flag/state/camera-slot layout this
 * TU evidences. xglStudioSelectGetActiveCamera (main:0x0022c618,
 * `lw $3,0($4)` feeding the `bne $3,$0` "camera in use" test) reads +0 as a
 * boolean active flag, and xglStudioMainCameraInit (main:0x0022c6f0/
 * 0x0022c6f8, `sw $17,4($2)` / `sw $17,0($2)` with $17=1) stores the same
 * word 1 to both +4 and +0 on init. The same function's
 * `addiu $4,$4,1520` stride (main:0x0022c620) sizes one camera slot at
 * 0x5F0 bytes, not just the 8-byte leading pair. +4 state keeps its name,
 * type and offset, so src/main/game.c's `camera->state == 4` reads the
 * same byte either way.
 */

typedef struct Studio {
    StudioLight light;
    StudioCamera cameras[8];
} Studio;

extern Studio asStudioSource[4];
extern Studio *pCurrentStudio;

extern void xglStudioChange(int studio_index);
extern void xglStudioGetCamera(StudioCamera **camera_out, int camera_index);
extern StudioLight *xglStudioGetLight2(void);
extern StudioCamera *xglStudioGetCamera2(int camera_index);
extern StudioCamera *xglStudioGetActiveCamera(void);
extern StudioCamera *xglStudioSelectGetActiveCamera(int studio_index);
extern void xglStudioFlushActiveCamera(void);
extern void xglStudioMainCameraInit(void);
extern StudioCamera *xglStudioMainCameraMove(void);
extern void xglStudioInit(void);
extern void xglStudioMove(void);
extern void xglStudioEntry(void);
extern void xglCameraMove(StudioCamera *camera);
extern void xglCameraTravelProc(StudioCamera *camera);
extern void xglLightInit(StudioLight *light);
extern void xglCameraInit(StudioCamera *camera);
extern int StudioEntrySkip;

void xglStudioChange(int studio_index)
{
    pCurrentStudio = &asStudioSource[studio_index];
}

void xglStudioGetLight(StudioLight **light_out)
{
    *light_out = &pCurrentStudio->light;
}

void xglStudioGetCamera(StudioCamera **camera_out, int camera_index)
{
    *camera_out = &pCurrentStudio->cameras[camera_index];
}

StudioCamera *xglStudioGetActiveCamera(void)
{
    int camera_index = 0;
    StudioCamera *camera;

    for (;;) {
        camera = xglStudioGetCamera2(camera_index);
        camera_index++;
        if (camera->active != 0)
            return camera;
        if (camera_index >= 8)
            return 0;
    }
}

StudioCamera *xglStudioSelectGetActiveCamera(int studio_index)
{
    int camera_index;
    StudioCamera *camera;

    for (camera_index = 0; camera_index < 8; camera_index++) {
        camera = &asStudioSource[studio_index].cameras[camera_index];
        if (camera->active != 0)
            return camera;
    }
    return 0;
}

void xglStudioFlushActiveCamera(void)
{
    int studio_index;
    StudioCamera *camera;

    for (studio_index = 0; studio_index < 4; studio_index++) {
        camera = xglStudioSelectGetActiveCamera(studio_index);
        if (camera != 0)
            xglCameraMove(camera);
    }
}

StudioLight *xglStudioGetLight2(void)
{
    return &pCurrentStudio->light;
}

StudioCamera *xglStudioGetCamera2(int camera_index)
{
    return &pCurrentStudio->cameras[camera_index];
}

void xglStudioMainCameraInit(void)
{
    int studio_index;
    StudioCamera *camera;

    for (studio_index = 0; studio_index < 4; studio_index++) {
        xglStudioChange(studio_index);
        camera = xglStudioGetCamera2(0);
        camera->state = camera->active = 1;
    }
    xglStudioChange(0);
}

StudioCamera *xglStudioMainCameraMove(void)
{
    xglStudioChange(0);
    return xglStudioGetCamera2(0);
}

void xglStudioInit(void)
{
    int studio_index;
    int camera_index;
    StudioCamera *camera;

    StudioEntrySkip = 0;
    for (studio_index = 0; studio_index < 4; studio_index++) {
        xglStudioChange(studio_index);
        xglLightInit(xglStudioGetLight2());
        for (camera_index = 0; camera_index < 8; camera_index++) {
            camera = xglStudioGetCamera2(camera_index);
            xglCameraInit(camera);
        }
    }
    xglStudioChange(0);
}

void xglStudioMove(void)
{
    int studio_index;
    int camera_index;
    StudioCamera *camera;

    for (studio_index = 0; studio_index < 4; studio_index++) {
        xglStudioChange(studio_index);
        for (camera_index = 0; camera_index < 8; camera_index++) {
            camera = xglStudioGetCamera2(camera_index);
            if (camera->active == 1) {
                xglCameraTravelProc(camera);
                xglCameraMove(camera);
            }
        }
    }
    xglStudioChange(0);
}

void xglStudioEntry(void)
{
    xglSleep();
    xglStudioInit();
    xglStudioMainCameraInit();
    for (;;) {
        if (StudioEntrySkip == 0) {
            xglStudioMainCameraMove();
            xglStudioMove();
        }
        xglSleep();
    }
}
