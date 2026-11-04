#ifndef INCLUDE_OV12_RG_DISP_H
#define INCLUDE_OV12_RG_DISP_H

#include "shared.h"
#include "ov12/rg_weapon_db.h"

/* Functions and data used by other translation units. */
void RgDispWpnDat_CreateRestNumStr(RgWeapon *weapon, char *buffer);
void RgDispWpnDat_CreateSpeedStr(RgStatus *robot, char *buffer);

#endif /* INCLUDE_OV12_RG_DISP_H */
