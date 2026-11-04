#ifndef INCLUDE_OV12_RG_DISP_WPN1P_H
#define INCLUDE_OV12_RG_DISP_WPN1P_H

#include "shared.h"
#include "ov12/rg_bxx.h"
#include "ov12/rg_equip_type.h"
#include "ov12/rg_font.h"
#include "ov12/rg_gauge.h"
#include "ov12/rg_weapon.h"
#include "ov12/xrg_paint2d.h"

/* One weapon slot's HUD entry: the gauge that shows its remaining rounds. */
typedef struct WepDisp {
    RgBxx *bxx;                    /* +0x00: the display texture archive */
    RgWeapon *weapon;              /* +0x04 */
    unsigned int index;            /* +0x08: the slot, an RgEquipType */
    RgGauge *gauge;                /* +0x0c */
} WepDisp;

/* The single-player weapon and status HUD (0x58 bytes). */
typedef struct RgDispWpn1P {
    RgStatus *robot;                 /* +0x00 */
    WepDisp wep[RG_EQUIP_TYPE_NUM];  /* +0x04 */
    RgBxxPic *boardSaPic;            /* +0x34: "board_sa.bmp" */
    RgBxxPic *boardSbPic;            /* +0x38: "board_sb.bmp" */
    RgBxxPic *boostPic;              /* +0x3c: "boost.bmp" */
    RgBxxPic *mphPic;                /* +0x40: "mph1.bmp" */
    RgFont *bulletFont;              /* +0x44 */
    RgFont *weaponFont;              /* +0x48 */
    RgFont *timeFont;                /* +0x4c */
    RgGauge *gauge;                  /* +0x50: the dash gauge */
    XrgPaint2D *paint;               /* +0x54 */
} RgDispWpn1P;

/* Functions and data used by other translation units. */
RgDispWpn1P *CreateRgDispWpn1P(RgBxx *dispTex);
void DisposeRgDispWpn1P(RgDispWpn1P *pDisp);
void RgDispWpn1PSetRobot(RgDispWpn1P *pDisp, RgStatus *pRobot);
void RgDispWpn1PPassTime(RgDispWpn1P *pDisp, float deltaTime);
void RgDispWpn1PDisp(RgDispWpn1P *pDisp);

#endif /* INCLUDE_OV12_RG_DISP_WPN1P_H */
