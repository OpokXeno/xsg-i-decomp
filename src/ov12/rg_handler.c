/*
 * OV12 original TU 48: 0x00a2ab70..0x00a2af20 (8 functions)
 */
#include "common.h"
#include "shared.h"
#include "ov12/rg_bgobj.h"
#include "ov12/rg_geom.h"
#include "ov12/rg_geom_point.h"
#include "ov12/rg_handler.h"
#include "ov12/rg_robot.h"
#include "ov12/rg_robot_spec.h"
#include "ov12/rg_shot.h"
#include "ov12/rg_weapon.h"
#include "ov12/xrg_rand_int.h"

/* Force scale of a body hit (damaging) and of a plain push (no damage) between robots. */
#define RG_HANDLER_BODY_HIT_FORCE 600000.0f

#define RG_HANDLER_PUSH_FORCE 3000000.0f

/* Seconds a background object stays hidden after the camera passes through it. */
#define RG_HANDLER_CAMERA_HIDE_TIME 0.5f


void RgHandlerRobotVsRobot(RgGeom *robotGeom, RgBgCollision *collision)
{
    RgStatus *robotAStatus;
    RgStatus *robotBStatus;
    unsigned int flagsA;
    unsigned int flagsB;
    RgRobotSpec *specA;
    RgRobotSpec *specB;
    float robotAPower;
    float robotBPower;
    RgVector negatedDirection;

    robotAStatus = RgGeomGetParent(robotGeom);
    robotBStatus = RgGeomGetParent(collision->geom);
    flagsA = RgRobotGetStatusFlags(robotAStatus);
    flagsB = RgRobotGetStatusFlags(robotBStatus);
    specA = RgRobotGetSpec(robotAStatus);
    specB = RgRobotGetSpec(robotBStatus);
    robotAPower = specA->bodyAttackDamage;
    robotBPower = specB->bodyAttackDamage;
    if (flagsB & 0x400) {
        RgRobotHitByBody(robotAStatus, collision->hitDirection, collision->direction,
                         robotBPower, RG_HANDLER_BODY_HIT_FORCE);
    } else if (flagsB & 0x100) {
        RgRobotHitByBody(robotAStatus, collision->hitDirection, collision->direction,
                         0.0f, RG_HANDLER_PUSH_FORCE);
    }
    XrgNegateVector(negatedDirection, collision->direction);
    if (flagsA & 0x400) {
        RgRobotHitByBody(robotBStatus, collision->hitDirection, negatedDirection,
                         robotAPower, RG_HANDLER_BODY_HIT_FORCE);
    }
    if (flagsA & 0x100) {
        RgRobotHitByBody(robotBStatus, collision->hitDirection, negatedDirection,
                         0.0f, RG_HANDLER_PUSH_FORCE);
    }
}

/*
 * The shot handlers pass the collision's hit direction (its first member, so
 * the record's own address) as the position; the weapon handlers pass that
 * same address where the callee declares an int damage.
 */
void RgHandlerShotVsRobot(RgGeom *shotGeom, RgBgCollision *collision)
{
    void *shot;

    shot = RgGeomGetParent(shotGeom);
    RgShotHitRobot(shot, RgGeomGetParent(collision->geom), collision->hitDirection);
}

void RgHandlerWeaponVsRobot(RgGeom *weaponGeom, RgBgCollision *collision)
{
    void *weapon;

    weapon = RgGeomGetParent(weaponGeom);
    RgWeaponHitRobot(weapon, RgGeomGetParent(collision->geom), collision->hitDirection);
}

void RgHandlerShotVsBG(RgGeom *shotGeom, RgBgCollision *collision)
{
    void *shot;

    shot = RgGeomGetParent(shotGeom);
    RgShotHitBg(shot, RgGeomGetParent(collision->geom), collision->hitDirection);
}

void RgHandlerWeaponVsBG(RgGeom *weaponGeom, RgBgCollision *collision)
{
    void *weapon;

    weapon = RgGeomGetParent(weaponGeom);
    RgWeaponHitBG(weapon, RgGeomGetParent(collision->geom), collision->hitDirection);
}

void RgHandlerCameraVsBG(RgGeom *cameraGeom, RgBgCollision *collision)
{
    void *bgObject;

    bgObject = RgGeomGetParent(collision->geom);
    if (bgObject != 0) {
        RgBgObjSetHide(bgObject, RG_HANDLER_CAMERA_HIDE_TIME);
    }
}

/* The robot's geometry is a point: its RgGeom head leads the RgGeomPoint. */
void RgHandlerRobotVsBG(RgGeom *robotGeom, RgBgCollision *collision)
{
    RgGeomPoint *point = (RgGeomPoint *) robotGeom;
    void *robot;
    void *bgObject;
    float weight;
    float speed;

    robot = RgGeomGetParent(robotGeom);
    bgObject = RgGeomGetParent(collision->geom);
    RgRobotHitBG(robot, collision->direction);
    if (bgObject != 0) {
        weight = RgGeomPointGetWeight(point);
        speed = RgGeomPointGetSpeed(point);
        RgBgObjTryToBodyAttack(bgObject, weight * 0.5f * speed * speed);
    }
}

void RgHandlerRobotAdvVsBG(RgGeom *robotGeom, RgBgCollision *collision)
{
    RgRobotNearBG(RgGeomGetParent(robotGeom), collision->direction);
}
