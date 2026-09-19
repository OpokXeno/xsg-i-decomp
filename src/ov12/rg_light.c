/*
 * OV12 original TU 46: 0x00a285e0..0x00a28858 (4 functions)
 */
#include "common.h"
#include "shared.h"

INCLUDE_ASM("asm/nonmatchings/ov12/rg_light", InitRgLight);

extern RgHeap *InstanceOfRgHeap(void);
extern void RgHeapFree(RgHeap *heap, void *ptr, const char *source_file,
                       int line);
extern const char D_00A54C78[];

static void _DisposeGlobalLight(void *light)
{
    RgHeapFree(InstanceOfRgHeap(), light, D_00A54C78, 35);
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_light", InstanceOfGlobalLight);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_light", RgLightCopy);
