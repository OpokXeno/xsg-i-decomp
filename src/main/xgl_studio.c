#include "common.h"
#include "main/xgl_thread.h"
#include "shared.h"
#include "main/xgl_studio.h"
#include "xgl_studio.h"
#include "main/Undulate.h"
#include "main/xgl_camera.h"

/*
 * Each studio holds a light and eight cameras.  In a StudioCamera (include/shared.h)
 * +0 is the active flag and +4 the state; xglStudioMainCameraInit sets both to 1.
 * src/main/game.c also reads the state field.
 */
typedef struct Studio {
    StudioLight light;
    StudioCamera cameras[8];
} Studio;

extern void xglStudioGetCamera(StudioCamera **camera_out, int camera_index);
extern StudioLight *xglStudioGetLight2(void);
extern StudioCamera *xglStudioGetActiveCamera(void);
extern void xglCameraTravelProc(StudioCamera *camera);
extern void xglLightInit(StudioLight *light);
static Studio asStudioSource[4];

static Studio *pCurrentStudio;

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

/* Returns the first active camera of the current studio, or 0 when none is active. */
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

/* Returns the first active camera of studio `studio_index`, or 0 when none is active. */
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

/* Moves the active camera of each of the four studios. */
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

/* Activates camera 0 of every studio and selects studio 0. */
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

/* Selects studio 0 and returns its camera 0. */
StudioCamera *xglStudioMainCameraMove(void)
{
    xglStudioChange(0);
    return xglStudioGetCamera2(0);
}

/* Initializes the light and all cameras of every studio, then selects studio 0. */
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

/* Runs the travel and move step for every active camera of every studio, then selects studio 0. */
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

/* Task entry: initializes the studios, then moves the cameras each frame unless StudioEntrySkip is set. */
void xglStudioEntry(void *argument)
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
