#include "common.h"
#include "map_2.h"

INCLUDE_ASM("asm/main/nonmatchings/map_2", MAP_loadUnitResource);

INCLUDE_ASM("asm/main/nonmatchings/map_2", MAP_initUnit);

INCLUDE_ASM("asm/main/nonmatchings/map_2", MAP_createUnit);

#define NULL ((void *)0)

/*
 * Bit 0x10 of a unit's flags, tested for every MapUnit[] entry, skips both
 * per-frame callbacks below when set; the bit's own meaning is not
 * otherwise evidenced in this TU.
 */
void MAP_updateUnit(void)
{
    MapUnitSlot *unit = MapUnit;
    int i;

    for (i = 0; i < 64; i++) {
        if (unit->serial >= 0 && !(unit->flags & 0x10)) {
            if (unit->update != NULL) {
                unit->update(unit);
            }
            if (unit->typeUpdate != NULL) {
                unit->typeUpdate(unit);
            }
        }
        unit++;
    }
}

INCLUDE_ASM("asm/main/nonmatchings/map_2", MAP_drawUnitAt);

void MAP_drawUnit(void)
{
    MapUnitSlot *unit = MapUnit;
    int i;

    for (i = 0; i < 64; i++) {
        if (unit->serial >= 0) {
            MAP_drawUnitAt(unit);
        }
        unit++;
    }
}

void MAP_getHeight(MapPosition *position)
{
    float height = UnduGet(position->x, position->z);
    int found = 1;

    if (height == -1000.0f) {
        found = 0;
    }
    if (found) {
        position->y = height;
    }
}
