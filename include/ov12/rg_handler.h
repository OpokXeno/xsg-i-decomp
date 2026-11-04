#ifndef INCLUDE_OV12_RG_HANDLER_H
#define INCLUDE_OV12_RG_HANDLER_H

#include "shared.h"

/*
 * The collision record the geometry-group collision job hands to every
 * handler: the hit direction at +0x00 (RgRobotHitByBody's direction), the
 * contact direction at +0x10 and the struck object's geometry at +0x20.
 */
typedef struct RgBgCollision {
    RgVector hitDirection; /* +0x00 */
    RgVector direction;    /* +0x10 */
    RgGeom *geom;          /* +0x20 */
} RgBgCollision;

/* Functions and data used by other translation units. */
void RgHandlerRobotVsRobot(RgGeom *robotGeom, RgBgCollision *collision);
void RgHandlerShotVsRobot(RgGeom *shotGeom, RgBgCollision *collision);
void RgHandlerWeaponVsRobot(RgGeom *weaponGeom, RgBgCollision *collision);
void RgHandlerShotVsBG(RgGeom *shotGeom, RgBgCollision *collision);
void RgHandlerWeaponVsBG(RgGeom *weaponGeom, RgBgCollision *collision);
void RgHandlerCameraVsBG(RgGeom *cameraGeom, RgBgCollision *collision);
void RgHandlerRobotVsBG(RgGeom *robotGeom, RgBgCollision *collision);
void RgHandlerRobotAdvVsBG(RgGeom *robotGeom, RgBgCollision *collision);

#endif /* INCLUDE_OV12_RG_HANDLER_H */
