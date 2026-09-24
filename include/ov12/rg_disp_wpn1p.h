#ifndef INCLUDE_OV12_RG_DISP_WPN1P_H
#define INCLUDE_OV12_RG_DISP_WPN1P_H

#include "shared.h"

/*
 * RgBxx is defined by ov12/tu073 (src/ov12/rg_bxx.c); this allocation only
 * stores and forwards the pointer LoadRgBxx_sub returns and DisposeRgBxx_sub
 * takes, so an incomplete type is enough here.
 */
typedef struct RgBxx RgBxx;

/*
 * The weapon handle owned by rg_weapon.c, opaque here.
 */
typedef struct RgWeapon RgWeapon;

/*
 * The gauge handle owned by ov12/rg_gauge.c; this allocation only disposes
 * it (still created by _InitDisp, still INCLUDE_ASM), so an incomplete type
 * is enough here.
 */
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
 * _DestructDisp, RgDispWpn1PSetRobot and RgDispWpn1PDisp evidence the 0x58
 * bytes of this TU's own allocation (CreateRgDispWpn1P's literal size):
 *   +0x00 robot      RgDispWpn1PSetRobot's argument; RgDispWpn1PDisp's null
 *                     check and its RgDispWpnDat_CreateSpeedStr/
 *                     _UpdateWepDisp argument
 *   +0x04 wep[3]      RgDispWpn1PSetRobot/RgDispWpn1PDisp/_DestructDisp,
 *                     0x10-byte WepDisp stride (_SetWepDisp/_UpdateWepDisp/
 *                     _DispWepDisp, still INCLUDE_ASM, are this array's own
 *                     accessors)
 *   +0x34 boardSaPic  RgBxxGetPic "board_sa.bmp" (set by _InitDisp, still
 *                     INCLUDE_ASM); RgDispWpn1PDisp draws it at (0, 0x160)
 *   +0x38 boardSbPic  RgBxxGetPic "board_sb.bmp"; RgDispWpn1PDisp draws it
 *                     at the same (0, 0x160)
 *   +0x3c boostPic    RgBxxGetPic "boost.bmp"; RgDispWpn1PDisp draws it at
 *                     (0xA0, 0x190)
 *   +0x44 bulletFont  set by _InitDisp; RgDispWpn1PDisp passes it to
 *                     _DispWepDisp
 *   +0x48 weaponFont  set by _InitDisp; RgDispWpn1PDisp passes it to
 *                     _DispWepDisp
 *   +0x4c timeFont    set by _InitDisp; RgDispWpn1PDisp passes it to
 *                     RgFontStr for the speed string
 *   +0x50 gauge       CreateRgGauge (set by _InitDisp); RgGaugeDraw/
 *                     DisposeRgGauge
 *   +0x54 paint       CreateXrgPaint2D_sub (set by _InitDisp);
 *                     DisposeXrgPaint2D_sub and every XrgPaint2D* call
 * +0x40 (RgBxxGetPic "mph1.bmp", set by _InitDisp) is never read by any of
 * these three functions and stays unmodeled.
 */
struct RgDispWpn1P {
    RgStatus *robot;                 /* +0x00 */
    WepDisp wep[3];                  /* +0x04 */
    int boardSaPic;                  /* +0x34 */
    int boardSbPic;                  /* +0x38 */
    int boostPic;                    /* +0x3c */
    unsigned char unmodeled_40[4];   /* +0x40 */
    int bulletFont;                  /* +0x44 */
    int weaponFont;                  /* +0x48 */
    int timeFont;                    /* +0x4c */
    RgGauge *gauge;                  /* +0x50 */
    void *paint;                     /* +0x54 */
};

#endif /* INCLUDE_OV12_RG_DISP_WPN1P_H */
