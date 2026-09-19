/*
 * TU-local declarations of ov12/tu064 (src/ov12/rg_bg_builder.c).
 */

#ifndef SRC_OV12_RG_BG_BUILDER_H
#define SRC_OV12_RG_BG_BUILDER_H

#include "shared.h"

/*
 * RgBgObj is defined by src/ov12/rg_bgobj.c (ov12/tu017); only the pointer
 * identity _SetBgObjAttr forwards to RgBgObjSetHardness is claimed here.
 */
typedef struct RgBgObj RgBgObj;

/*
 * A robot-game background builder. CreateRgBgBuilder allocates
 * sizeof(RgBgBuilder) (0x90) bytes for one and DisposeRgBgBuilder frees it,
 * so the struct ends at +0x90 with nothing past it claimed here.
 *
 * group (+0x00) is the CreateRgBgBuilder argument, asserted non-NULL as
 * "pGroup"; its own layout is outside this allocation's evidence.
 * loadData (+0x04) is cleared by CreateRgBgBuilder and returned by
 * RgBgBuilderGetLoadData ("the background builder's loaded file-data
 * pointer").
 * light (+0x10) is the RgLight object InitRgLight initializes and
 * RgLightCopy copies out of (RgBgBuilderGetLight); it is owned by
 * ov12/tu046 (src/ov12/rg_light.c), not yet recovered, so only its start
 * and its extent up to the next evidenced member (database at +0x80) are
 * attested here.
 * database (+0x80) is the RgSimpleDB CreateRgBgBuilder creates with
 * CreateRgSimpleDB(0x40, 0x40) and DisposeRgBgBuilder tears down with
 * DisposeRgSimpleDB.
 */
typedef struct RgBgBuilder {
    void *group;                     /* +0x00 */
    void *loadData;                  /* +0x04 */
    unsigned char unmodeled_08[8];   /* +0x08 */
    unsigned char light[0x70];       /* +0x10 */
    RgSimpleDB *database;            /* +0x80 */
    unsigned char unmodeled_84[0xc]; /* +0x84 */
} RgBgBuilder;

RgBgBuilder *CreateRgBgBuilder(void *pGroup);
void DisposeRgBgBuilder(RgBgBuilder *pBuilder);
void *RgBgBuilderGetLoadData(RgBgBuilder *pBuilder);
void RgBgBuilderGetLight(RgBgBuilder *pBuilder, void *pGetBuf);

#endif /* SRC_OV12_RG_BG_BUILDER_H */
