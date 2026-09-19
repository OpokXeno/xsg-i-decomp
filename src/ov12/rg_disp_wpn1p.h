/*
 * TU-local declarations of ov12/tu041 (src/ov12/rg_disp_wpn1p.c).
 */

#ifndef SRC_OV12_RG_DISP_WPN1P_H
#define SRC_OV12_RG_DISP_WPN1P_H

#include "shared.h"

/*
 * Handles owned by other TUs, opaque here: the picture archive
 * (rg_bxx, RgBxxGetPic), the weapon (rg_weapon.c) and the gauge (rg_gauge.c).
 */
typedef struct RgBxx RgBxx;
typedef struct RgWeapon RgWeapon;
typedef struct RgGauge RgGauge;

/*
 * One weapon slot's gauge state. _InitWepDisp sets +0x00 from its pBxx
 * argument (asserted non-NULL as "pBxx != NIL") and creates the gauge at
 * +0x0c; _DestructWepDisp disposes that gauge, and _PassTimeWepDisp reads
 * the weapon pointer at +0x04 to feed the gauge from RgWeaponGetShotNum.
 * +0x08 is never read or written by any of those three functions (it is
 * the weapon slot index _SetWepDisp/_UpdateWepDisp use, both still
 * INCLUDE_ASM in this TU) and stays unmodeled here.
 */
typedef struct WepDisp {
    RgBxx *bxx;                    /* +0x00 */
    RgWeapon *weapon;              /* +0x04 */
    unsigned char unmodeled_08[4]; /* +0x08 */
    RgGauge *gauge;                /* +0x0c */
} WepDisp;

/*
 * The single-player weapon/status HUD. CreateRgDispWpn1P/DisposeRgDispWpn1P
 * only allocate/free it and forward the pointer to this TU's _InitDisp/
 * _DestructDisp (both still INCLUDE_ASM), never dereferencing a field
 * themselves, so it stays opaque here; RgHeapAlloc is called with the
 * original's own literal size (0x58) rather than a sizeof this allocation
 * cannot take.
 */
typedef struct RgDispWpn1P RgDispWpn1P;

#endif /* SRC_OV12_RG_DISP_WPN1P_H */
