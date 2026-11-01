#ifndef INCLUDE_MAIN_INIT_DRILL_H
#define INCLUDE_MAIN_INIT_DRILL_H

#include "shared.h"

typedef union DrillVector {
    Vector4 vector;
    u64 words[2];
} DrillVector;

typedef struct DrillContainerModel {
    u8 hitMode;
    unsigned char unmodeled_01[3];
    int modelId;
    int itemBase;
    int unmodeled_0c;
} DrillContainerModel;

typedef struct DrillContainerDef {
    u8 type;
    unsigned char unmodeled_01[0x0f];
    DrillVector size;
    DrillContainerModel model;
} DrillContainerDef;

extern DrillContainerDef container[31];
extern short Times;

#endif /* INCLUDE_MAIN_INIT_DRILL_H */
