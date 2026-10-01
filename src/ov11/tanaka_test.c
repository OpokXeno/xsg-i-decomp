/*
 * OV11 original TU 0: 0x00a00000..0x00a00100 (2 functions)
 */
#include "common.h"
#include "shared.h"
#include "main/xgl_studio.h"

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
    camera_to_initialize->position.y = 1.0f;
    camera_to_initialize->position.z = 8.0f;
    camera_to_initialize->position.w = 1.0f;
    camera_to_initialize->rotation.w = 1.0f;
    camera_to_initialize->rotation.x = 0.0f;
    camera_to_initialize->rotation.y = 0.0f;
    camera_to_initialize->rotation.z = 0.0f;
    camera_to_initialize->position.x = 0.0f;

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
extern void MiniG_Init(void);
extern int MiniG_Main(void);
extern const char D_00A0BB10[];
extern const char D_00A0BB20[];

void TanakaTest(void)
{
    xglRenderClearFrame();
    ClearEnv.color_a = 0x80;
    ClearEnv.color_g = 0;
    ClearEnv.color_b = 0;
    ClearEnv.color_r = 0;
    Init();
    printf(D_00A0BB10);
    MiniG_Init();
    while (MiniG_Main() == 0) {
        xglSleep();
    }
    xglSleep();
    xglSleep();
    printf(D_00A0BB20);
    xglRenderClearDepth();
}
