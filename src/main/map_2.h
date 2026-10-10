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
/* MAP_initUnit, MAP_createUnit, and MAP_drawUnitAt also establish the
 * transforms, parent matrix, model, and rendering fields at their evidenced
 * offsets in this same record. */
typedef struct MapUnitSlot {
    u32 flags; /* +0x00 */
    void (*update)(struct MapUnitSlot *unit);     /* +0x04 */
    void (*draw)(struct MapUnitSlot *unit);       /* +0x08 */
    void (*typeUpdate)(struct MapUnitSlot *unit); /* +0x0C */
    float position[4]; /* +0x10 */
    float rotation[4]; /* +0x20 */
    float scale[4];    /* +0x30 */
    unsigned char unmodeled_40[0x50];
    Vector4 globalPosition; /* +0x90 */
    unsigned char slot; /* +0xA0 */
    unsigned char unmodeled_a1;
    signed char actionSub; /* +0xA2 */
    unsigned char unmodeled_a3;
    s16 serial; /* +0xA4 */
    s16 cleared_a6; /* +0xA6 */
    unsigned char unmodeled_a8[0x28];
    void *sequence; /* +0xD0 */
    void *resource_model; /* +0xD4 */
    int cleared_d8[2];
    union {
        struct {
            void *model; /* +0xE0 */
            int resource_status; /* +0xE4 */
        };
        unsigned int modelResourceWords[2]; /* MAP_initUnit clears both words in one loop */
    };
    int effectCf[3]; /* +0xE8 */
    unsigned char unmodeled_f4[4];
    int parentMatrixIndex; /* +0xF8 */
    struct MapUnitParent *parent; /* +0xFC */
    unsigned char unmodeled_100[0x120];
    float modelPosition[4]; /* +0x220 */
    unsigned char unmodeled_230[4];
    int texMapMode; /* +0x234 */
    int texMapSize; /* +0x238 */
    float texMapWeight; /* +0x23C */
    void *model_state; /* +0x240, MDL_create's in-place state begins here */
    unsigned char unmodeled_244[0x8C];
    unsigned short shadowParts[8]; /* +0x2D0 */
    int shadowPartCount; /* +0x2E0 */
    int renderLevel; /* +0x2E4 */
    int filter; /* +0x2E8 */
    int sortOffset; /* +0x2EC */
    float transparency; /* +0x2F0 */
    float filterStart; /* +0x2F4 */
    float filterEnd; /* +0x2F8 */
    float reflTransparency; /* +0x2FC */
} MapUnitSlot;

/* MAP_drawUnitAt reads the matrix array at +0x824 in a parent record. */
struct MapUnitParent {
    unsigned char unmodeled_00[0x824];
    Matrix4 *matrices;
};

/* The two words consumed from the map resource returned by RES_loadFile. */
typedef struct MapUnitResource {
    void *model;
    int status;
} MapUnitResource;

extern MapUnitSlot MapUnit[64];

void MAP_drawUnitAt(MapUnitSlot *unit);

#endif
