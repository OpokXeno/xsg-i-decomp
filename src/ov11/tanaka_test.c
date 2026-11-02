/*
 * OV11 original TU 0: 0x00a00000..0x00a00100 (2 functions)
 */
#include "common.h"
#include "shared.h"
#include "main/xgl_studio.h"
#include "ov11/mini_g.h"

extern void xglStudioChange(int studio_index);
extern void xglStudioGetCamera(StudioCamera **camera_out, int camera_index);
extern void xglLightSetDefault(StudioLight *light);

static void Init(void)
{
    StudioCamera *camera;
    StudioCamera *camera_to_initialize;
    StudioLight *light;

    xglStudioChange(0);
    xglStudioGetCamera(&camera, 0);
    camera_to_initialize = camera;
    camera_to_initialize->position.vector.y = 1.0f;
    camera_to_initialize->position.vector.z = 8.0f;
    camera_to_initialize->position.vector.w = 1.0f;
    camera_to_initialize->rotation.vector.w = 1.0f;
    camera_to_initialize->rotation.vector.x = 0.0f;
    camera_to_initialize->rotation.vector.y = 0.0f;
    camera_to_initialize->rotation.vector.z = 0.0f;
    camera_to_initialize->position.vector.x = 0.0f;

    xglStudioGetLight(&light);
    xglLightSetDefault(light);
}

typedef struct TanakaClearEnv {
    u8 unmodeled_00[0x20];
    u32 color_r;
    u32 color_g;
    u32 color_b;
    u32 color_a;
    u8 unmodeled_30[0x30];
} TanakaClearEnv;

extern TanakaClearEnv ClearEnv;
extern void xglRenderClearFrame(void);
extern void xglRenderClearDepth(void);
extern int printf(const char *format, ...);
const char casinoEntryDebugText[] = "Casino Entry\n";
const char casinoExitDebugText[] = "Casino Exit\n";

void TanakaTest(void)
{
    xglRenderClearFrame();
    ClearEnv.color_a = 0x80;
    ClearEnv.color_g = 0;
    ClearEnv.color_b = 0;
    ClearEnv.color_r = 0;
    Init();
    printf(casinoEntryDebugText);
    MiniG_Init();
    while (MiniG_Main() == 0) {
        xglSleep();
    }
    xglSleep();
    xglSleep();
    printf(casinoExitDebugText);
    xglRenderClearDepth();
}
