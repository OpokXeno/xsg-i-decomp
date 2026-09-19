/*
 * OV12 original TU 48: 0x00a2ab70..0x00a2af20 (8 functions)
 */
#include "common.h"
#include "rg_handler.h"

INCLUDE_ASM("asm/nonmatchings/ov12/rg_handler", RgHandlerRobotVsRobot);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_handler", RgHandlerShotVsRobot);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_handler", RgHandlerWeaponVsRobot);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_handler", RgHandlerShotVsBG);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_handler", RgHandlerWeaponVsBG);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_handler", RgHandlerCameraVsBG);

extern void *RgGeomGetParent(RgGeom *pGeom);
extern float RgGeomPointGetSpeed(RgGeomPoint *point);
extern float RgGeomPointGetWeight(RgGeomPoint *point);
extern void RgRobotHitBG(void *robot, RgVector direction);
extern void RgBgObjTryToBodyAttack(void *bgObject, float energy);

/*
 * point's leading 0x20 bytes are unmodeled in rg_geom_point.h, wide enough
 * to hold the RgGeom every geometry object leads with (rg_geom.c), so
 * passing it to RgGeomGetParent through that common head is exact here.
 */
void RgHandlerRobotVsBG(RgGeomPoint *point, RgBgCollision *collision)
{
    void *robot;
    void *bgObject;
    float weight;
    float speed;

    robot = RgGeomGetParent((RgGeom *) point);
    bgObject = RgGeomGetParent(collision->geom);
    RgRobotHitBG(robot, collision->direction);
    if (bgObject != 0) {
        weight = RgGeomPointGetWeight(point);
        speed = RgGeomPointGetSpeed(point);
        RgBgObjTryToBodyAttack(bgObject, weight * 0.5f * speed * speed);
    }
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_handler", RgHandlerRobotAdvVsBG);
