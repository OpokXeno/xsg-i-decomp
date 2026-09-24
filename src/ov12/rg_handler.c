/*
 * OV12 original TU 48: 0x00a2ab70..0x00a2af20 (8 functions)
 */
#include "common.h"
#include "rg_handler.h"

INCLUDE_ASM("asm/nonmatchings/ov12/rg_handler", RgHandlerRobotVsRobot);

extern void *RgGeomGetParent(RgGeom *pGeom);
extern void RgShotHitRobot(void *shot, int robot, RgVector position);
extern void RgWeaponHitRobot(void *weapon, int robotId, int damage);
extern void RgShotHitBg(void *shot, int bgObject, RgVector position);
extern void RgWeaponHitBG(void *weapon, int bgObject, int damage);
extern void RgBgObjSetHide(void *pObj, float duration);

/*
 * Each of RgShotHitRobot/RgWeaponHitRobot/RgShotHitBg/RgWeaponHitBG forwards
 * this handler's own collision record unchanged as its trailing
 * position/damage argument (ov12:0x00a2acd8..0x00a2adf7): none of them
 * dereference it along this call path, so it is passed opaquely, cast to
 * whatever scalar or vector shape that parameter declares.
 */
void RgHandlerShotVsRobot(RgGeom *shotGeom, RgBgCollision *collision)
{
    void *shot;

    shot = RgGeomGetParent(shotGeom);
    RgShotHitRobot(shot, (int) RgGeomGetParent(collision->geom), (float *) collision);
}

void RgHandlerWeaponVsRobot(RgGeom *weaponGeom, RgBgCollision *collision)
{
    void *weapon;

    weapon = RgGeomGetParent(weaponGeom);
    RgWeaponHitRobot(weapon, (int) RgGeomGetParent(collision->geom), (int) collision);
}

void RgHandlerShotVsBG(RgGeom *shotGeom, RgBgCollision *collision)
{
    void *shot;

    shot = RgGeomGetParent(shotGeom);
    RgShotHitBg(shot, (int) RgGeomGetParent(collision->geom), (float *) collision);
}

void RgHandlerWeaponVsBG(RgGeom *weaponGeom, RgBgCollision *collision)
{
    void *weapon;

    weapon = RgGeomGetParent(weaponGeom);
    RgWeaponHitBG(weapon, (int) RgGeomGetParent(collision->geom), (int) collision);
}

void RgHandlerCameraVsBG(RgGeom *cameraGeom, RgBgCollision *collision)
{
    void *bgObject;

    bgObject = RgGeomGetParent(collision->geom);
    if (bgObject != 0) {
        RgBgObjSetHide(bgObject, 0.5f);
    }
}

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

extern void RgRobotNearBG(void *pRobot, RgVector direction);

void RgHandlerRobotAdvVsBG(RgGeom *robotGeom, RgBgCollision *collision)
{
    RgRobotNearBG(RgGeomGetParent(robotGeom), collision->direction);
}
