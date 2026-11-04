#ifndef INCLUDE_OV12_RG_SHOT_DB_H
#define INCLUDE_OV12_RG_SHOT_DB_H

#include "shared.h"
#include "ov12/rg_shot.h"

/* Functions and data used by other translation units. */
RgSimpleDB *InstanceOfRgShotDB(void);
RgShotEssence *RgShotDBGetEssence(RgSimpleDB *database, const char *name);
void RgShotDBRead(RgSimpleDB *pDB, const char *pszDataBase);
void RgShotDBClear(RgSimpleDB *database);

#endif /* INCLUDE_OV12_RG_SHOT_DB_H */
