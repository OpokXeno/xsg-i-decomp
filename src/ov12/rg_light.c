/*
 * OV12 original TU 46: 0x00a285e0..0x00a28858 (4 functions)
 */
#include "common.h"
#include "shared.h"
#include "ov12/rg_singleton_id.h"
#include "rg_light.h"

extern void assert_prog(const char *expression, const char *source_file,
                        int line);
extern void XrgSetVectorXYZ(RgVector destination, float x, float y, float z);
extern void XrgClearVector(RgVector destination);
extern const char D_00A54C68[]; /* "pLight != NIL" */
extern const char D_00A54C78[];

void InitRgLight(RgLight *light)
{
    if (light == 0) {
        assert_prog(D_00A54C68, D_00A54C78, 18);
    }
    XrgSetVectorXYZ(light->ambient_color, 0.08f, 0.08f, 0.11f);
    XrgSetVectorXYZ(light->direction[0], -0.5f, 1.0f, 0.5f);
    XrgSetVectorXYZ(light->direction[1], 0.5f, -1.0f, -0.5f);
    XrgClearVector(light->direction[2]);
    XrgSetVectorXYZ(light->color[0], 0.7f, 0.7f, 0.7f);
    XrgSetVectorXYZ(light->color[1], 0.15f, 0.15f, 0.2f);
    XrgClearVector(light->color[2]);
}

extern RgHeap *InstanceOfRgHeap(void);
extern void RgHeapFree(RgHeap *heap, void *ptr, const char *source_file,
                       int line);
extern const char D_00A54C78[];

static void _DisposeGlobalLight(void *light)
{
    RgHeapFree(InstanceOfRgHeap(), light, D_00A54C78, 35);
}

extern void *RgHeapAlloc(RgHeap *heap, unsigned int size,
                         const char *source_file, int line);

RgLight *InstanceOfGlobalLight(void)
{
    RgLight *light;

    light = RgSingletonIDGet(6U);
    if (light == 0) {
        light = RgHeapAlloc(InstanceOfRgHeap(), sizeof(RgLight), D_00A54C78,
                             42);
        InitRgLight(light);
        RgSingletonIDEntry(6, (RgSimpleDB *) light,
                           (void (*)(RgSimpleDB *)) _DisposeGlobalLight);
    }
    return light;
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_light", RgLightCopy);
