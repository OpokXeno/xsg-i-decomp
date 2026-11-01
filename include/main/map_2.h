#ifndef INCLUDE_MAIN_MAP_2_H
#define INCLUDE_MAIN_MAP_2_H

#include "shared.h"

/* Accurate defining-owner proposals; normal owner transport is pending.
 * MapUnitSlot fields/extent: src/main/map_2.h. ActorUndulation and
 * LookAtPlayerActor spelling: src/main/look_at_player.c21..75. */
typedef struct MapUnitSlot {
    u32 flags;
    void (*update)(struct MapUnitSlot *unit);
    u8 unmodeled_08[8];
    /* Vector consumer at CheckGameSymbol and position producers agree on
     * four floats at+0x10; canonical map_2.h uses float[4], so this nominal
     * proposal requires defining-owner transport. Extent remains0x300. */
    Vector4 position;
    float rotation[4];
    float scale[4];
    u8 unmodeled_40[0xa4 - 0x40];
    s16 serial;
    u8 unmodeled_a6[0x1a0 - 0xa6];
    s16 symbol_kind;
    s16 shop_id;
    u8 unmodeled_1a4[0x300 - 0x1a4];
} MapUnitSlot;

MapUnitSlot *MAP_createUnit(int slot, int serial);
void MAP_drawUnit(void);

#endif /* INCLUDE_MAIN_MAP_2_H */
