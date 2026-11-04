#ifndef INCLUDE_OV12_RG_GEOM_POINT_H
#define INCLUDE_OV12_RG_GEOM_POINT_H

#include "shared.h"
#include "ov12/rg_geom.h"

/* Functions and data used by other translation units. */
float RgGetGeomGravity(void);
void InitRgGeomPoint(RgGeomPoint *point, float weight);
RgGeomPoint *CreateRgGeomPoint(float weight);
void RgGeomPointSetPos(RgGeomPoint *point, RgVector position);
void RgGeomPointMovePos(RgGeomPoint *point, RgVector position);
void RgGeomPointResetPos(RgGeomPoint *point);
void RgGeomPointSetVel(RgGeomPoint *point, RgVector velocity);
void RgGeomPointAddForce(RgGeomPoint *point, RgVector force);
void RgGeomPointSetWeight(RgGeomPoint *point, float weight);
void RgGeomPointSetMoveResist(RgGeomPoint *point, float moveResist);
void RgGeomPointSetMaxXYSpd(RgGeomPoint *point, float maxXYSpd);
void __RgGeomPointGetPos(RgGeomPoint *point, RgVector destination,
                         const char *source_file, int source_line);
void __RgGeomPointGetOldPos(RgGeomPoint *point, RgVector destination,
                            const char *source_file, int source_line);
void RgGeomPointGetVel(RgGeomPoint *point, RgVector velocity);
float RgGeomPointGetSpeed(RgGeomPoint *point);
float RgGeomPointGetWeight(RgGeomPoint *point);
float RgGeomPointGetMoveResist(RgGeomPoint *point);
void RgGeomPointPassTime(RgGeomPoint *pPoint, float deltaTime);

#endif /* INCLUDE_OV12_RG_GEOM_POINT_H */
