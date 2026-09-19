/*
 * TU-local declarations of ov12/tu044 (src/ov12/rg_disp.c).
 */

#ifndef SRC_OV12_RG_DISP_H
#define SRC_OV12_RG_DISP_H

#include "shared.h"

/*
 * The weapon handle owned by rg_weapon.c, opaque here.
 */
typedef struct RgWeapon RgWeapon;

/*
 * The per-weapon essence handle RgWeaponGetEss returns, owned by
 * rg_weapon.c and opaque there. RgDispWpnDat_CreateRestNumStr reads a
 * float at +0x14 to get the weapon's shot capacity; the span before it is
 * unattested here.
 */
typedef struct RgWeaponEssence {
    unsigned char unmodeled_00[0x14]; /* +0x00 */
    float capacity;                   /* +0x14 */
} RgWeaponEssence;

#endif /* SRC_OV12_RG_DISP_H */
