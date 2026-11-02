/*
 * TU-local declarations of main/tu269 (src/main/map_2.c).
 */

#ifndef SRC_MAIN_MAP_2_H
#define SRC_MAIN_MAP_2_H

#include "shared.h"
#include "main/map_2.h"
#include "main/Undulate.h"

/*
 * MAP_getHeight's in/out argument: x/z are the query point on the map
 * grid, y is the ground height MAP_getHeight writes back when UnduGet
 * finds one.
 */
typedef struct MapPosition {
    float x;
    float y;
    float z;
} MapPosition;


/* MAP_drawUnitAt reads the matrix array at +0x824 in a parent record. */
struct MapUnitParent {
    unsigned char unmodeled_00[0x824];
    Matrix4 *matrices;
};

/* The two words consumed from the map resource returned by RES_loadFile. */
typedef struct MapUnitResource {
    void *model;
    const char *status;
} MapUnitResource;

extern MapUnitRecord MapUnit[64];

void MAP_getHeight(MapPosition *position);
#endif
