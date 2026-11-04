#ifndef INCLUDE_OV12_RG_DISP_LIFE_H
#define INCLUDE_OV12_RG_DISP_LIFE_H

#include "shared.h"
#include "ov12/rg_bxx.h"
#include "ov12/rg_font.h"
#include "ov12/rg_gauge.h"
#include "ov12/xrg_paint2d.h"

/*
 * The battle life/vs-mode display object CreateRgDispLife allocates
 * (sizeof(RgDispLife) == 0x60). Players are 1P (index 0) and 2P (index 1):
 * robots[] are the RgStatus handles RgDispLifeSetRobot stores (the same pair
 * _InitBattle passes to RgDispWpn2PSetRobot), gauge[] their life gauges and
 * winCount[] their win tallies.
 */
typedef struct RgDispLife {
    RgBxx *dispBxx;                  /* +0x00 */
    RgFont *timeFont;                /* +0x04 */
    RgBxxPic *primaryBoardPic;       /* +0x08: switched by RgDispLifeSetVsMode */
    RgBxxPic *secondaryBoardPic;     /* +0x0c */
    RgBxxPic *agwsNamePic;           /* +0x10 */
    RgStatus *robots[2];             /* +0x14 */
    RgGauge *gauge[2];               /* +0x1c */
    RgBxxPic *lifePic[8];            /* +0x24 */
    float timer;                     /* +0x44: stored only when not negative */
    XrgPaint2D *paint;               /* +0x48 */
    int vsMode;                      /* +0x4c */
    int winCount[2];                 /* +0x50: clamped to 0..2 by RgDispLifeSetWin */
    int winnerMask;                  /* +0x58: bit i set when player i won */
    float elapsedTime;               /* +0x5c: blink phase, wraps past 1.0 */
} RgDispLife;

/* Functions and data used by other translation units. */
RgDispLife *CreateRgDispLife(RgBxx *dispTex, RgFont *timeFont);
void DisposeRgDispLife(RgDispLife *pDisp);
void RgDispLifeSetRobot(RgDispLife *pDisp, RgStatus *pRobot1, RgStatus *pRobot2);
void RgDispLifeSetTimer(RgDispLife *pDisp, float timer);
void RgDispLifeSetWin(RgDispLife *pDisp, int win1P, int win2P, int winnerMask);
void RgDispLifeSetVsMode(RgDispLife *pDisp, int vsMode);
void RgDispLifePassTime(RgDispLife *pDisp, float deltaTime);
void RgDispLifeDisp(RgDispLife *pDisp);

#endif /* INCLUDE_OV12_RG_DISP_LIFE_H */
