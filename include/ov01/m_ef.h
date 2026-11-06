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

/* The actor argument is the Actor record itself (its rotation, position and joint matrices are read). */
extern Matrix4 *MEfGetActorMatrix(Matrix4 *destination, Actor *actor, u32 coordinate);
extern Vector4 *MEfGetActorCoord(Vector4 *destination, Actor *actor, u32 coordinate);

/* Functions and data used by other translation units. */
extern Vector4 *MEfCalcAngle(Vector4 *destination, const Vector4 *from, const Vector4 *to);
extern Vector4 *MEfCalcAngleMatrix(Vector4 *out, Matrix4 *matrix);
extern void MEfDrawModel(const Vector4 *place, int entry, const char *texture);
extern int MEfMake4Vertex2(Vector4 *dst, const Vector4 *source, float size, float angleBase);

#endif /* INCLUDE_OV01_M_EF_H */
