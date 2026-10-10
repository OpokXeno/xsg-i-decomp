#ifndef INCLUDE_MAIN_UNIT_H
#define INCLUDE_MAIN_UNIT_H

#include "shared.h"

/* The original global actor table contains 64 records of 0xa70 bytes
 * (symbol-table size 0x29c00). These are this TU's storage view: the known
 * member extents come from the independent actor call sites documented in
 * src/main/act_2.h; unnamed spans remain byte storage. */
typedef struct UnitActorAnimSlot {
    unsigned char unmodeled_00[0x14];
    unsigned short currentDataId;
} UnitActorAnimSlot;

typedef struct UnitActorStorage {
    u32 flags;
    void (*update)(struct UnitActorStorage *actor);
    void (*draw)(struct UnitActorStorage *actor);
    u32 quadword_alignment_gap;
    Vector4 position;
    Vector4 previous_position;
    Vector4 velocity;
    Vector4 acceleration;
    Vector4 rotation;
    Vector4 scale;
    unsigned char unmodeled_70[0x6f0 - 0x70];
    UnitActorAnimSlot animSlot;
    unsigned char unmodeled_706[0x71c - 0x706];
    void *animData;
    void *animUserData;
    unsigned char unmodeled_724[0x7fc - 0x724];
    int moveElementId;
    unsigned char unmodeled_800[0x840 - 0x800];
    unsigned char model[0x58];
    unsigned char unmodeled_898[0x8d8 - 0x898];
    void *move;
    void *animPackTables[8];
    unsigned char unmodeled_8fc[0xa70 - 0x8fc];
} UnitActorStorage;

#endif /* INCLUDE_MAIN_UNIT_H */
