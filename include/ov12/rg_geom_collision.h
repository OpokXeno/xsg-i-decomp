#ifndef INCLUDE_OV12_RG_GEOM_COLLISION_H
#define INCLUDE_OV12_RG_GEOM_COLLISION_H

#include "shared.h"
#include "ov12/rg_geom.h"

/*
 * A collision-response callback: called with the object it belongs to and the
 * contact point of the hit.
 */
typedef void (*RgGeomColiCallback)(void *object, RgVector contact);

/*
 * A pair of geometries to test, each with the callback to run when the other
 * side hits it (InitRgGeomColiArg clears all four words).
 */
typedef struct RgGeomCollisionPair {
    RgGeom *pGeom1;
    RgGeomColiCallback callback1;
    RgGeom *pGeom2;
    RgGeomColiCallback callback2;
} RgGeomCollisionPair;

/* Functions and data used by other translation units. */
void InitRgGeomColiArg(RgGeomCollisionPair *pArg);
int RgGeomCollisionJob(RgGeomCollisionPair *pArg);

#endif /* INCLUDE_OV12_RG_GEOM_COLLISION_H */
