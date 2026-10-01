#ifndef INCLUDE_OV12_RG_ROBOT_H
#define INCLUDE_OV12_RG_ROBOT_H

#include "shared.h"

#include "ov12/rg_draw.h"

void RgRobotSetWeapon(RgStatus *pRobot, int eSide, int weaponID);

void RgRobotSetSpareWeapon(RgStatus *pRobot, int eSide, int weaponID);

RgGeomPoint *RgRobotGetGeom(RgStatus *pRobot);

#endif /* INCLUDE_OV12_RG_ROBOT_H */
