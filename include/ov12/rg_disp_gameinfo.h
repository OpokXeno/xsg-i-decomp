#ifndef INCLUDE_OV12_RG_DISP_GAMEINFO_H
#define INCLUDE_OV12_RG_DISP_GAMEINFO_H

#include "shared.h"
#include "ov12/rg_bxx.h"
#include "ov12/xrg_paint2d.h"

/* The size RgHeapAlloc is called with (CreateRgDispGameInfo). */
#define RG_DISP_GAME_INFO_SIZE 0x70

/*
 * The lock-on style indicator drawn around the sub (target) robot for the
 * host robot. state runs the fade machine _DispInfo drives: it picks whether
 * the marker follows the target in 3D or sits at the 2D offset, and alpha is
 * the marker's current fade. swayPhase and pulsePhase are radians wrapped
 * into [-pi, pi]; swayPhase also rotates the marker when there is no
 * direction vector.
 */
typedef struct RgDispGameInfo {
    RgStatus *hostRobot;                   /* +0x00 */
    RgStatus *subRobot;                    /* +0x04 */
    XrgPaint2D *paint;                     /* +0x08 */
    RgBxx *dispTex;                        /* +0x0c: picture archive */
    float value1;                          /* +0x10: seeded to 1.0f, never read */
    unsigned char unmodeled_14[0x20 - 0x14];
    XrgPaint2DOffsetOverride offset;       /* +0x20..0x4f: filled by XrgPaint2DOffsetResult */
    float alpha;                           /* +0x50 */
    int state;                             /* +0x54 */
    int studio;                            /* +0x58: draw id of the paint */
    float swayPhase;                       /* +0x5c */
    float pulsePhase;                      /* +0x60 */
    unsigned char unmodeled_64[0x70 - 0x64];
} RgDispGameInfo;

/* Functions and data used by other translation units. */
RgDispGameInfo *CreateRgDispGameInfo(RgBxx *dispTex);
void DisposeRgDispGameInfo(RgDispGameInfo *pInfo);
void RgDispGameInfoSetHostRobot(RgDispGameInfo *pInfo, RgStatus *hostRobot);
void RgDispGameInfoSetSubRobot(RgDispGameInfo *pInfo, RgStatus *subRobot);
void RgDispGameInfoSetStudio(RgDispGameInfo *pInfo, int studio);
void RgDispGameInfoPassTime(RgDispGameInfo *pInfo, float deltaTime);
void RgDispGameInfoDisplay(RgDispGameInfo *pInfo);

#endif /* INCLUDE_OV12_RG_DISP_GAMEINFO_H */
