#ifndef INCLUDE_OV12_RG_DISP_WPN2P_H
#define INCLUDE_OV12_RG_DISP_WPN2P_H

#include "shared.h"
#include "ov12/rg_bxx.h"
#include "ov12/rg_equip_type.h"
#include "ov12/rg_font.h"
#include "ov12/rg_gauge.h"
#include "ov12/rg_weapon.h"
#include "ov12/xrg_paint2d.h"

/* Players in a two-player battle, one RobInfo each. */
#define RG_DISP_WPN2P_SIDE_NUM 2

/* One weapon slot's display state: the weapon and its slot (-1 when unset). */
typedef struct WepInfo {
    RgWeapon *weapon; /* +0x00 */
    int index;        /* +0x04: the slot, an RgEquipType */
} WepInfo;

/* One player's robot display (0x50 bytes). */
typedef struct RobInfo {
    RgStatus *robot;                 /* +0x00 */
    XrgPaint2D *paint;               /* +0x04 */
    unsigned char unmodeled_08[8];   /* +0x08 */
    WepInfo wep[RG_EQUIP_TYPE_NUM];  /* +0x10 */
    unsigned char unmodeled_28[8];   /* +0x28 */
    int posX;                        /* +0x30: x origin of the boards and the dash gauge, per side */
    RgGauge *gauge;                  /* +0x34: the dash gauge */
    RgBxxPic *boardSaPic;            /* +0x38: "board_sa.bmp" */
    RgBxxPic *boardSbPic;            /* +0x3c: "board_sb.bmp" */
    RgBxxPic *boostPic;              /* +0x40: "boost.bmp" */
    unsigned char unmodeled_44[0xc]; /* +0x44 */
} RobInfo;

/* The two-player weapon display (0xb0 bytes). */
typedef struct RgDispWpn2P {
    RobInfo rob[RG_DISP_WPN2P_SIDE_NUM]; /* +0x00, +0x50 */
    RgBxxPic *weaponPic;                 /* +0xa0: "wpn_all.bmp" */
    RgFont *bulletFont;                  /* +0xa4 */
    unsigned char unmodeled_a8[8];       /* +0xa8 */
} RgDispWpn2P;

/* Functions and data used by other translation units. */
RgDispWpn2P *CreateRgDispWpn2P(void);
void DisposeRgDispWpn2P(RgDispWpn2P *pDisp);
void RgDispWpn2PSetRobot(RgDispWpn2P *pDisp, RgStatus *pRobot1, RgStatus *pRobot2);
void RgDispWpn2PPassTime(RgDispWpn2P *pDisp, float deltaTime);
void RgDispWpn2PDisp(RgDispWpn2P *pDisp);

#endif /* INCLUDE_OV12_RG_DISP_WPN2P_H */
