/*
 * TU-local declarations of ov12/tu009 (src/ov12/rg_robot_db.c).
 */

#ifndef SRC_OV12_RG_ROBOT_DB_H
#define SRC_OV12_RG_ROBOT_DB_H

#include "shared.h"

RgSimpleDB *InstanceOfRgRobotDB(void);

extern void RgRobotDBClear(RgSimpleDB *database);

#endif /* SRC_OV12_RG_ROBOT_DB_H */
