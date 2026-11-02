#ifndef INCLUDE_MAIN_INIT_DRILL_H
#define INCLUDE_MAIN_INIT_DRILL_H

#include "shared.h"

typedef struct DrillContainerModel {
    u8 hitMode;
    unsigned char unmodeled_01[3];
    int modelId;
    int itemBase;
    int unmodeled_0c;
} DrillContainerModel;

/*
 * container[]'s 16-byte size vector is copied whole as two ld/sd doublewords,
 * but the table itself is only 8-byte aligned in the original .data, so its
 * copy type is the 8-aligned DoublewordAlignedVector4 rather than AlignedVector4.
 */
typedef struct DrillContainerDef {
    u8 type;
    unsigned char unmodeled_01[0x0f];
    DoublewordAlignedVector4 size;
    DrillContainerModel model;
} DrillContainerDef;

extern DrillContainerDef container[31];
extern short Times;

/* Functions and data used by other translation units. */
extern short NowTester;

#endif /* INCLUDE_MAIN_INIT_DRILL_H */
