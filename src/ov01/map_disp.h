/*
 * TU-local declarations of ov01/tu010 (src/ov01/map_disp.c).
 */

#ifndef SRC_OV01_MAP_DISP_H
#define SRC_OV01_MAP_DISP_H

typedef union MapMultiplier {
    float components[4];
    long long raw[2];
} MapMultiplier;

void mapInit(void);

void mapMulSet(const MapMultiplier *value);

extern volatile unsigned int mapDispWork[];

MapMultiplier *mapMulGet(void);

extern MapMultiplier mapMul;

#endif /* SRC_OV01_MAP_DISP_H */
