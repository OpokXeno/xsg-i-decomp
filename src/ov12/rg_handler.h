/*
 * TU-local declarations of ov12/tu048 (src/ov12/rg_handler.c).
 */

#ifndef SRC_OV12_RG_HANDLER_H
#define SRC_OV12_RG_HANDLER_H

#include "shared.h"

/*
 * RgHandlerRobotVsBG's collision record: a direction vector at +0x10,
 * passed by address to RgRobotHitBG, and the struck background object's
 * geometry pointer at +0x20, passed to RgGeomGetParent. The leading span
 * is untouched by this function.
 */
typedef struct RgBgCollision {
    RgVector hitDirection;            /* +0x00: RgRobotHitByBody direction */
    RgVector direction;               /* +0x10 */
    RgGeom *geom;                     /* +0x20 */
} RgBgCollision;

/* At +0x00, RgHandlerRobotVsRobot passes this vector as the first RgVector
 * argument to RgRobotHitByBody; its +0x10 vector is the second argument. */

/* RgRobotGetSpec returns this record; body-hit power is loaded at +0x54. */
typedef struct RgRobotSpecPower {
    unsigned char unmodeled_00[0x54];
    float bodyPower;
} RgRobotSpecPower;

#endif /* SRC_OV12_RG_HANDLER_H */
