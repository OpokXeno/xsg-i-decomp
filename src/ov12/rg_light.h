/*
 * TU-local declarations of ov12/tu046 (src/ov12/rg_light.c).
 */

#ifndef SRC_OV12_RG_LIGHT_H
#define SRC_OV12_RG_LIGHT_H

#include "shared.h"

/*
 * The 0x70-byte light object InitRgLight fills and InstanceOfGlobalLight
 * allocates (RgBgBuilder's light member, src/ov12/rg_bg_builder.h, is one
 * instance of this same object). InitRgLight writes ambient_color, the
 * first two direction/color pairs and clears the third pair of each array.
 */
typedef struct RgLight {
    RgVector ambient_color;
    RgVector direction[3];
    RgVector color[3];
} RgLight;

void InitRgLight(RgLight *light);
RgLight *InstanceOfGlobalLight(void);

#endif /* SRC_OV12_RG_LIGHT_H */
