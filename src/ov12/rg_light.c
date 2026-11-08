/*
 * OV12 original TU 46: 0x00a285e0..0x00a28858 (4 functions)
 */
#include "common.h"
#include "shared.h"
#include "ov12/rg_singleton_id.h"
#include "ov12/rg_light.h"
#include "ov12/assert_prog.h"
#include "ov12/xrg_rand_int.h"
#include "ov12/rg_heap.h"


const char lightAssertLightNotNil[] = "pLight != NIL";
const char lightSourcePath[] = "../rg_light.euc.c";
const char lightAssertSourceNotNil[] = "pSrc != NIL";

/* Sets the default ambient color, directions and colors of a light. */
void InitRgLight(RgLight *light)
{
    if (light == 0) {
        assert_prog(lightAssertLightNotNil, lightSourcePath, 18);
    }
    XrgSetVectorXYZ(light->ambient_color, 0.08f, 0.08f, 0.11f);
    XrgSetVectorXYZ(light->direction[0], -0.5f, 1.0f, 0.5f);
    XrgSetVectorXYZ(light->direction[1], 0.5f, -1.0f, -0.5f);
    XrgClearVector(light->direction[2]);
    XrgSetVectorXYZ(light->color[0], 0.7f, 0.7f, 0.7f);
    XrgSetVectorXYZ(light->color[1], 0.15f, 0.15f, 0.2f);
    XrgClearVector(light->color[2]);
}

static void _DisposeGlobalLight(void *light)
{
    RgHeapFree(InstanceOfRgHeap(), light, lightSourcePath, 35);
}

/* Returns the global light singleton, creating it on first use. */
RgLight *InstanceOfGlobalLight(void)
{
    RgLight *light;

    light = RgSingletonIDGet(RG_SINGLETON_GLOBAL_LIGHT);
    if (light == 0) {
        light = RgHeapAlloc(InstanceOfRgHeap(), sizeof(RgLight), lightSourcePath,
                             42);
        InitRgLight(light);
        RgSingletonIDEntry(RG_SINGLETON_GLOBAL_LIGHT, light, _DisposeGlobalLight);
    }
    return light;
}

/* Copies the ambient color and the colors and directions of pSrc into pLight. */
void RgLightCopy(RgLight *pLight, RgLight *pSrc)
{
    int i;

    if (pLight == 0) {
        assert_prog(lightAssertLightNotNil, lightSourcePath, 55);
    }
    if (pSrc == 0) {
        assert_prog(lightAssertSourceNotNil, lightSourcePath, 56);
    }

    XrgCopyVector(pLight->ambient_color, pSrc->ambient_color);
    for (i = 0; i < 3; i++) {
        XrgCopyVector(pLight->color[i], pSrc->color[i]);
        XrgCopyVector(pLight->direction[i], pSrc->direction[i]);
    }
}
