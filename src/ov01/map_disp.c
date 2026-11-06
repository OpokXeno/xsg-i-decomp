/*
 * OV01 original TU 10: 0x00a2c3a0..0x00a2c5a8 (9 functions)
 */
#include "common.h"
#include "map_disp.h"

static const char *mapPartsName[2] = {"ten", "yuka"};
static unsigned int mapDispWork[4];
static MapMultiplier mapMul;

/* mapDispWork per map part: shown, or hidden (its name visibility reads 0 while hidden). */
#define MAP_DISP_SHOWN 0
#define MAP_DISP_HIDDEN 1

void mapInit(void) {
    mapDispWork[1] = MAP_DISP_SHOWN;
    mapMul.components[0] = 1.0f;
    mapDispWork[0] = MAP_DISP_SHOWN;
    mapMul.components[1] = 1.0f;
    mapMul.components[2] = 1.0f;
    mapMul.components[3] = 1.0f;
}

void mapMulSet(const MapMultiplier *value) {
    mapMul = *value;
}

MapMultiplier *mapMulGet(void) {
    return &mapMul;
}

void mapDispOn(int mapId) {
    if (mapId < 2) {
        mapDispWork[mapId] = MAP_DISP_SHOWN;
    }
}

void mapDispOff(int mapId) {
    if (mapId < 2) {
        mapDispWork[mapId] = MAP_DISP_HIDDEN;
    }
}

void mapDispOnAll(void)
{
    mapDispWork[1] = MAP_DISP_SHOWN;
    mapDispWork[0] = MAP_DISP_SHOWN;
}

void mapDispOffAll(void)
{
    mapDispWork[1] = MAP_DISP_HIDDEN;
    mapDispWork[0] = MAP_DISP_HIDDEN;
}

void mapDisp(int mapNo)
{
    MapDispData *mapData;
    NmlModel *model;
    float matrix[4][4];
    int i;

    if (mapDispWork[0] == MAP_DISP_HIDDEN && mapDispWork[1] == MAP_DISP_HIDDEN) {
        return;
    }

    mapData = dataMapLoadAdrGet(mapNo);
    nmlModelSetTexture((char *)mapData + mapData->textureOffset);
    xglMatrixStackUnit();
    xglMatrixStackSave(matrix);
    nmlModelSetPlace(matrix);
    nmlModelSetMulColor(mapMul.components);
    model = (NmlModel *)((char *)mapData + mapData->entryOffset);

    for (i = 0; i < 2; i++) {
        if (mapDispWork[i] == MAP_DISP_HIDDEN) {
            nmlModelSetNameVisible(model, mapPartsName[i], 0, 3);
        } else {
            nmlModelSetNameVisible(model, mapPartsName[i], 1, 3);
        }
    }

    nmlModelSetMapEntry();
    nmlModelEntry(model);
}

void mapDispTest(void) {
}
