/*
 * TU-local declarations of main/tu269 (src/main/map_2.c).
 */

#ifndef SRC_MAIN_MAP_2_H
#define SRC_MAIN_MAP_2_H

#include "shared.h"

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

extern float UnduGet(float x, float z);

void MAP_getHeight(MapPosition *position);

#endif
