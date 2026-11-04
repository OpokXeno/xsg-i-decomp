#ifndef INCLUDE_OV12_RG_BG_BUILDER_H
#define INCLUDE_OV12_RG_BG_BUILDER_H

#include "shared.h"
#include "ov12/rg_geom_group.h"

typedef struct RgReadText RgReadText;

/*
 * A robot-game background builder: it reads a background description
 * (RgBgBuilderBuildFromText), registers the collision boxes of the stage's
 * data file by name and creates the background objects in its geometry
 * group. The layout is private to src/ov12/rg_bg_builder.c.
 */
typedef struct RgBgBuilder RgBgBuilder;

struct RgLight;

/* Functions and data used by other translation units. */
RgBgBuilder *CreateRgBgBuilder(RgGeomGroup *pGroup);
void DisposeRgBgBuilder(RgBgBuilder *pBuilder);
void RgBgBuilderBuildFromText(RgBgBuilder *pBuilder, RgReadText *pReader,
                              const char *pszStageName);
RgFileSysData *RgBgBuilderGetLoadData(RgBgBuilder *pBuilder);
void RgBgBuilderGetLight(RgBgBuilder *pBuilder, struct RgLight *pGetBuf);

#endif /* INCLUDE_OV12_RG_BG_BUILDER_H */
