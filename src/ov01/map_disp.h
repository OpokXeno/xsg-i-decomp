/*
 * TU-local declarations of ov01/tu010 (src/ov01/map_disp.c).
 */

#ifndef SRC_OV01_MAP_DISP_H
#define SRC_OV01_MAP_DISP_H

#include "shared.h"

typedef union MapMultiplier {
    float components[4];
    long long raw[2];
} MapMultiplier;

void mapInit(void);

void mapMulSet(const MapMultiplier *value);

extern unsigned int mapDispWork[];

MapMultiplier *mapMulGet(void);

extern MapMultiplier mapMul;

void mapDispTest(void);

/* dataMapLoadAdrGet is defined in src/ov01/data_unit_org_get.c; that TU's
 * own header does not declare it, so this is this file's own reading aid. */
extern void *dataMapLoadAdrGet(int mapNo);

/*
 * Partial view of the map data blob dataMapLoadAdrGet returns. mapDisp reads
 * two byte offsets from the blob's own start: entryOffset (+0x08) locates
 * the model record mapDisp hands to nmlModelSetNameVisible/nmlModelSetMapEntry/
 * nmlModelEntry, textureOffset (+0x0c) locates the texture data it hands to
 * nmlModelSetTexture.
 */
typedef struct MapDispData {
    unsigned char unmodeled_00[8];
    int entryOffset;   /* +0x08 */
    int textureOffset; /* +0x0c */
} MapDispData;

/* Opaque model record nmlModelSetNameVisible/nmlModelSetMapEntry/nmlModelEntry
 * (src/main/nml_model_set.c) read; that TU's own header does not declare it. */
typedef struct NmlModel NmlModel;

extern void nmlModelSetTexture(const char *texture);
extern void nmlModelSetPlace(float matrix[4][4]);
extern void nmlModelSetMulColor(const float color[4]);
extern void nmlModelSetMapEntry(void);
extern void nmlModelSetNameVisible(NmlModel *model, const char *name, int visible, int match);
extern void nmlModelEntry(NmlModel *model);

/* "ten" (ceiling) and "yuka" (floor), the two named map parts mapDisp shows
 * or hides through nmlModelSetNameVisible. */

void mapDisp(int mapNo);

#endif /* SRC_OV01_MAP_DISP_H */
