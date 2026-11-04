#ifndef INCLUDE_OV12_RG_GEOM_ROBOT_H
#define INCLUDE_OV12_RG_GEOM_ROBOT_H

#include "shared.h"
#include "ov12/rg_geom.h"

/* Functions and data used by other translation units. */
RgGeom *CreateRgGeomRobot(void);
void InitRgGeomRobot(RgGeom *geom);
void RgGeomRobotSetRotate(RgGeom *geom, float rotate);
void RgGeomRobotSetDir(RgGeom *geom, RgVector direction);
void RgGeomRobotSetRotVel(RgGeom *geom, float rotVel);
void RgGeomRobotAddRotForce(RgGeom *geom, float force);
void RgGeomRobotSetRotResist(RgGeom *geom, float resist);
void RgGeomRobotSetMaxRotVel(RgGeom *geom, float maxRotVel);
float RgGeomRobotGetRotate(const RgGeom *geom);
float RgGeomRobotGetRotVel(const RgGeom *geom);
float RgGeomRobotGetRotForce(const RgGeom *geom);
void RgGeomRobotCalcLocal(RgGeom *geom, RgMatrix matrix);
void RgGeomRobotPassTime(RgGeom *geom, float deltaTime);

#endif /* INCLUDE_OV12_RG_GEOM_ROBOT_H */
