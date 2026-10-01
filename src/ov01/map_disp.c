/*
 * OV01 original TU 10: 0x00a2c3a0..0x00a2c5a8 (9 functions)
 */
#include "common.h"
#include "map_disp.h"

void mapInit(void) {
    mapDispWork[1] = 0;
    mapMul.components[0] = 1.0f;
    mapDispWork[0] = 0;
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
        mapDispWork[mapId] = 0;
    }
}

void mapDispOff(int mapId) {
    if (mapId < 2) {
        mapDispWork[mapId] = 1;
    }
}

void mapDispOnAll(void)
{
    mapDispWork[1] = 0;
    mapDispWork[0] = 0;
}

void mapDispOffAll(void)
{
    mapDispWork[1] = 1;
    mapDispWork[0] = 1;
}

void mapDisp(int mapNo)
{
    MapDispData *mapData;
    NmlModel *model;
    float matrix[4][4];
    int i;

    if (mapDispWork[0] == 1 && mapDispWork[1] == 1) {
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
        if (mapDispWork[i] == 1) {
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
