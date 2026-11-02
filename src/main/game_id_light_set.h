#ifndef MAIN_GAME_ID_LIGHT_SET_H
#define MAIN_GAME_ID_LIGHT_SET_H

#include "shared.h"
#include "main/game_id_light_set.h"

typedef struct LayoutHeader LayoutHeader;

/* The actor fields GameIdLightSet touches. */
typedef struct IdLightActor {
    unsigned int flags;                    /* +0x00, bit 0x8000 = light changed */
    unsigned char unmodeled_04[0x10 - 0x04];
    Vector4 position;                      /* +0x10 */
    unsigned char unmodeled_20[0x92 - 0x20];
    unsigned char lightId;                 /* +0x92, 0 = studio light */
    unsigned char blendSteps;              /* +0x93, steps left to the target */
    unsigned char unmodeled_94[0x510 - 0x94];
    IdLightSet light;                      /* +0x510 */
} IdLightActor;

/* Terrain query block built in the scratchpad; only the fields set here. */
typedef struct IdLightQuery {
    int queryFlags;                        /* +0x00 */
    unsigned char unmodeled_04[0x08 - 0x04];
    short attrMask;                        /* +0x08 */
    unsigned char unmodeled_0a[0x18 - 0x0a];
    LayoutHeader *header;                          /* +0x18 */
    void *scratch;                         /* +0x1c */
    long long attribute;                   /* +0x20 */
} IdLightQuery;

void GameIdLightSet(IdLightActor *actor, IdLightQuery *query);

#endif
