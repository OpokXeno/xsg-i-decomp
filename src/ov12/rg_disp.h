/*
 * TU-local declarations of ov12/tu044 (src/ov12/rg_disp.c).
 */

#ifndef SRC_OV12_RG_DISP_H
#define SRC_OV12_RG_DISP_H

#include "shared.h"

/*
 * RgWeaponEssence, the per-weapon essence RgWeaponGetEss returns, is
 * defined by the weapon database (ov12/tu027, rg_weapon_db.c).
 * RgDispWpnDat_CreateRestNumStr reads its capacity (+0x14).
 */
#include "ov12/rg_weapon_db.h"

/*
 * The weapon handle owned by rg_weapon.c, opaque here.
 */
typedef struct RgWeapon RgWeapon;

#endif /* SRC_OV12_RG_DISP_H */
