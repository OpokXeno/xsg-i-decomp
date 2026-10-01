#include "common.h"
#include "map_2.h"

extern MapUnitResource *RES_loadFile(int command, int callback, int resource_id, int flags);
extern void LOG(const char *format, ...);
extern int MDL_create(void *model_instance, void *resource_model);
extern const char D_004D2130[];

int MAP_loadUnitResource(MapUnitSlot *unit, int resource_id)
{
    MapUnitResource *resource = RES_loadFile(-1, 4, resource_id, 0);
    void *model;
    int status;

    if (resource == 0) {
        resource = RES_loadFile(-1, 4, resource_id & 0x7F00, 0);
    }

    if (resource != 0) {
        model = resource->model;
        status = resource->status;
    } else {
        model = 0;
        status = 0;
    }

    if (model == 0) {
        LOG((const char *)D_004D2130);
    }

    unit->resource_status = status;
    unit->resource_model = model;
    unit->model = model;
    return MDL_create(&unit->model_state, model);
}

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
