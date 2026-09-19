/*
 * TU-local declarations of ov12/tu015 (src/ov12/rg_player.c).
 */

#ifndef SRC_OV12_RG_PLAYER_H
#define SRC_OV12_RG_PLAYER_H

#include "shared.h"

typedef struct RgRobot RgRobot;
typedef struct RgRobotControl RgRobotControl;
typedef struct RgGeomGroup RgGeomGroup;
typedef struct RgCamera RgCamera;
typedef struct RgActor RgActor;
typedef struct RgDrawView RgDrawView;
typedef struct RgDrawStudio RgDrawStudio;

/* The player's set-up data: InitRgPlayerEssence initialises it and
 * _InitRgPlayer asserts "pDat != NIL" on it. */
typedef struct RgPlayerEssence RgPlayerEssence;

/*
 * No function of this TU reads or writes byte offset 0x14 (_InitRgPlayer
 * creates the geometry groups at 0x08..0x10 and 0x18..0x1C and skips it), so
 * it stays an unmodeled span rather than an invented field.
 */
typedef struct RgPlayer {
    RgRobot *robot;
    RgRobotControl *control;
    RgGeomGroup *bodyGeoms;
    RgGeomGroup *shotGeoms;
    RgGeomGroup *atkGeoms;
    unsigned char unmodeled_14[4];
    RgGeomGroup *eyeGeoms;
    RgGeomGroup *advGeoms;
    RgCamera *traceCamera;
} RgPlayer;

#endif /* SRC_OV12_RG_PLAYER_H */
