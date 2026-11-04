#ifndef INCLUDE_OV12_RG_ROBOT_SUBCON_H
#define INCLUDE_OV12_RG_ROBOT_SUBCON_H

#include "shared.h"

/* Functions and data used by other translation units. */
void RgRobSubAcceralate(RgGeomPoint *geometry, RgVector direction,
                        float scale);
void RgRobSubBreak(RgGeomPoint *geometry, float linear_scale,
                   float rotational_scale);
float RgRobSubTargetting(RgGeomPoint *geometry,
                         RgVector targetPosition, float speed,
                         float turnRate);
float RgRobSubHoming(RgGeomPoint *geometry, RgGeomPoint *targetGeometry,
                     float speed, float deltaTime);
void RgRobSubDirTo(RgGeomPoint *geometry, RgVector direction);
int RgRobSubGetAdvanceMot(RgGeomPoint *geometry, int dash);
int RgRobSubGetRollMotion(RgGeomPoint *geometry);
int RgRobSubGetDamageMotion(RgGeomPoint *geometry, RgVector direction);

#endif /* INCLUDE_OV12_RG_ROBOT_SUBCON_H */
