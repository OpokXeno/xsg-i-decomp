/*
 * TU-local declarations of ov12/tu042 (src/ov12/rg_disp_wpn2p.c).
 */

#ifndef SRC_OV12_RG_DISP_WPN2P_H
#define SRC_OV12_RG_DISP_WPN2P_H

#include "shared.h"

/*
 * One weapon slot's display state. _InitWep sets both fields to their
 * "unset" values (weapon 0, index -1); _SetRobInfo fills each slot from
 * RgRobotGetWeapon(robot, slot) and the slot number (0..2), and _DispWep
 * passes `weapon` to RgWeaponGetEss. The weapon's own type is local to
 * ov12/rg_weapon, so the handle stays void * here, as RgRobotGetWeapon
 * returns it.
 */
typedef struct WepInfo {
    void *weapon; /* +0x00 */
    int index;    /* +0x04 */
} WepInfo;

/*
 * One player's robot display, 0x50 bytes (the stride of RgDispWpn2P's
 * rob[] array in RgDispWpn2PPassTime/RgDispWpn2PDisp). Each field is
 * evidenced by this TU's _InitRobInfo/_SetRobInfo/_DispRobInfo:
 *   +0x00 robot     RgRobotGetWeapon/RgRobotGetDashTime argument
 *   +0x04 paint     CreateXrgPaint2D_sub handle used for every draw
 *   +0x10 wep[3]    _InitWep/_SetWep/_UpdateWep/_DispWep, 8-byte stride
 *   +0x30 posX      8 for side 0, 0x108 for side 1; the x origin of the
 *                   boards and the dash gauge
 *   +0x34 gauge     CreateRgGauge handle (RgGaugeGetDisp/RgGaugeDraw)
 *   +0x38 boardSaPic RgBxxGetPic "board_sa.bmp"
 *   +0x3c boardSbPic RgBxxGetPic "board_sb.bmp"
 *   +0x40 boostPic  RgBxxGetPic "boost.bmp"
 * The gaps are never touched by any function of this TU.
 */
typedef struct RobInfo {
    RgStatus *robot;                 /* +0x00 */
    void *paint;                     /* +0x04 */
    unsigned char unmodeled_08[8];   /* +0x08 */
    WepInfo wep[3];                  /* +0x10 */
    unsigned char unmodeled_28[8];   /* +0x28 */
    int posX;                        /* +0x30 */
    void *gauge;                     /* +0x34 */
    int boardSaPic;                  /* +0x38 */
    int boardSbPic;                  /* +0x3c */
    int boostPic;                    /* +0x40 */
    unsigned char unmodeled_44[0xc]; /* +0x44 */
} RobInfo;

/*
 * The two-player weapon display, one RobInfo per side. _InitDisp stores
 * the "wpn_all.bmp" picture at +0xa0 and the bullet font at +0xa4, and
 * RgDispWpn2PDisp passes both to _DispRobInfo. CreateRgDispWpn2P allocates
 * 0xb0 bytes; the last 8 are never touched by this TU.
 */
typedef struct RgDispWpn2P {
    RobInfo rob[2];                /* +0x00, +0x50 */
    int weaponPic;                 /* +0xa0 */
    int bulletFont;                /* +0xa4 */
    unsigned char unmodeled_a8[8]; /* +0xa8 */
} RgDispWpn2P;

#endif /* SRC_OV12_RG_DISP_WPN2P_H */
