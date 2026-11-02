#ifndef INCLUDE_OV01_M_EF_H
#define INCLUDE_OV01_M_EF_H

#include "shared.h"

#include "main/act_1.h"

/* Actor origin, raised origin, or an accessory coordinate selector. */
typedef enum MEfActorPoint {
    MEF_POINT_ACTOR = -1,
    MEF_POINT_ACTOR_RAISED = -2,
    MEF_POINT_ACCESSORY_COUNT = 0x28
} MEfActorPoint;

extern Matrix4 *MEfGetActorMatrix(Matrix4 *destination, Actor *actor, u32 coordinate);

#endif /* INCLUDE_OV01_M_EF_H */
