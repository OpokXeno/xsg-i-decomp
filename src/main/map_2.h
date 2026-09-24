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

/*
 * TU-local view of one entry of the game-wide MapUnit[] array (0x300 bytes
 * apart; src/main/map_create_unit_peer.h, src/main/init_drill.c and
 * src/main/init_uwamono_sys.c keep their own views of the same record).
 * Only the fields MAP_updateUnit and MAP_drawUnit touch are named:
 *  - update is MAP_updateUnit's first per-frame callback, called with the
 *    unit itself as the sole argument when set; map_create_unit_peer.h
 *    records MAP_initUnitSequance storing MAP_updateUnitDefault here for
 *    ordinary units.
 *  - typeUpdate is MAP_updateUnit's second per-frame callback, reloaded and
 *    re-checked even when update is NULL; MAP_createUnit (this TU, still
 *    INCLUDE_ASM) stores unit6003_update here for the 0x6025 unit type, so
 *    this slot carries a unit type's own update logic.
 *  - serial is negative for an inactive/free slot (MAP_initUnit sets it to
 *    -1); MAP_updateUnit and MAP_drawUnit both skip an entry whose serial
 *    is negative.
 */
typedef struct MapUnitSlot {
    u32 flags; /* +0x00 */
    void (*update)(struct MapUnitSlot *unit);     /* +0x04 */
    unsigned char unmodeled_08[4];
    void (*typeUpdate)(struct MapUnitSlot *unit); /* +0x0C */
    unsigned char unmodeled_10[0x94];
    s16 serial; /* +0xA4 */
    unsigned char unmodeled_a6[0x25a];
} MapUnitSlot;

extern MapUnitSlot MapUnit[64];

void MAP_drawUnitAt(MapUnitSlot *unit);

#endif
