#include "common.h"
#include "map_2.h"

INCLUDE_ASM("asm/main/nonmatchings/map_2", MAP_loadUnitResource);

INCLUDE_ASM("asm/main/nonmatchings/map_2", MAP_initUnit);

INCLUDE_ASM("asm/main/nonmatchings/map_2", MAP_createUnit);

INCLUDE_ASM("asm/main/nonmatchings/map_2", MAP_updateUnit);

INCLUDE_ASM("asm/main/nonmatchings/map_2", MAP_drawUnitAt);

INCLUDE_ASM("asm/main/nonmatchings/map_2", MAP_drawUnit);

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
