/*
 * OV01 original TU 10: 0x00a2c3a0..0x00a2c5a8 (9 functions)
 */
#include "common.h"
#include "map_disp.h"

void mapInit(void) {
    mapDispWork[0] = 0;
    mapMul.components[0] = 1.0f;
    mapDispWork[1] = 0;
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

INCLUDE_ASM("asm/nonmatchings/ov01/map_disp", mapDispOn);

INCLUDE_ASM("asm/nonmatchings/ov01/map_disp", mapDispOff);

INCLUDE_ASM("asm/nonmatchings/ov01/map_disp", mapDispOnAll);

INCLUDE_ASM("asm/nonmatchings/ov01/map_disp", mapDispOffAll);

INCLUDE_ASM("asm/nonmatchings/ov01/map_disp", mapDisp);

INCLUDE_ASM("asm/nonmatchings/ov01/map_disp", mapDispTest);
