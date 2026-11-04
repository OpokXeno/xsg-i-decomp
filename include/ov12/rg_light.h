#ifndef INCLUDE_OV12_RG_LIGHT_H
#define INCLUDE_OV12_RG_LIGHT_H

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

/* Functions and data used by other translation units. */
void InitRgLight(RgLight *light);
void RgLightCopy(RgLight *pLight, RgLight *pSrc);
RgLight *InstanceOfGlobalLight(void);

#endif /* INCLUDE_OV12_RG_LIGHT_H */
