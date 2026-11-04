#ifndef INCLUDE_OV12_RG_ROBOT_DB_H
#define INCLUDE_OV12_RG_ROBOT_DB_H

#include "shared.h"
#include "ov12/rg_robot_spec.h"

struct RgReadText;

/* Functions and data used by other translation units. */
RgSimpleDB *InstanceOfRgRobotDB(void);
void RgRobotDBClear(RgSimpleDB *database);
void *RgRobotDBGet(RgSimpleDB *database, const char *name);
void *RgRobotDBGetByID(RgSimpleDB *database, int charID);
RgRobotSpec *RgRobotDBGetDefault(void);
void RgRobotDBRead(RgSimpleDB *database, struct RgReadText *pReader);
void RgRobotDBDump(RgSimpleDB *database);

#endif /* INCLUDE_OV12_RG_ROBOT_DB_H */
