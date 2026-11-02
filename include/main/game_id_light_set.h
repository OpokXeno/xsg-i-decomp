#ifndef INCLUDE_MAIN_GAME_ID_LIGHT_SET_H
#define INCLUDE_MAIN_GAME_ID_LIGHT_SET_H

#include "shared.h"

typedef unsigned int IdLightQuadword __attribute__((mode(TI)));

typedef struct IdLightPair {
    IdLightQuadword color;
    IdLightQuadword direction;
} IdLightPair;

/* A studio light block (0xF0 bytes): ambient, then colour/direction pairs. */
typedef struct IdLightSet {
    IdLightQuadword ambientColor;
    IdLightPair lights[3];
    IdLightQuadword unmodeled_70[8];
} IdLightSet;

#endif /* INCLUDE_MAIN_GAME_ID_LIGHT_SET_H */
