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
    unsigned char unmodeled_00[0x10]; /* +0x00 */
    RgVector direction;               /* +0x10 */
    RgGeom *geom;                     /* +0x20 */
} RgBgCollision;

#endif /* SRC_OV12_RG_HANDLER_H */
