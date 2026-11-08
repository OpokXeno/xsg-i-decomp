/*
 * OV12 original TU 7: 0x00a0cbc8..0x00a0d178 (8 functions)
 */
#include "common.h"
#include "shared.h"
#include "ov12/assert_prog.h"
#include "ov12/rg_geom_point.h"
#include "ov12/rg_actor_motid.h"
#include "ov12/rg_geom_robot.h"
#include "ov12/rg_robot_subcon.h"
#include "ov12/xrg_rand_int.h"
#include "ov12/rg_error.h"

/* Directions a velocity is quantized to (eight around the robot). */
#define RG_ROBOT_DIR8_NUM 8


/* Text: assertion expression "pGeom != NIL". */
static const char robotSubconAssertGeomNotNil[16] = "pGeom != NIL";
/* Text: source file name "../rg_robot_subcon.euc.c". */
static const char robotSubconSourcePath[32] = "../rg_robot_subcon.euc.c";

/* Text: assertion expression "0 <= nDir8 && nDir8 < 8". */
static const char robotSubconAssertDir8InRange[24] = "0 <= nDir8 && nDir8 < 8";
/* Text: format string "unknown damage dir %d". */
static const char robotSubconUnknownDamageDirFormat[24] = "unknown damage dir %d";

/* Adds a force along the normalized direction, scaled by scale, to a geometry point. */
void RgRobSubAcceralate(RgGeomPoint *geometry, RgVector direction, float scale)
{
    RgVector force;

    if (geometry == 0) {
        assert_prog(robotSubconAssertGeomNotNil, robotSubconSourcePath, 21);
    }
    XrgNormalizeVector(force, direction);
    XrgScaleVectorXYZ(force, force, scale);
    RgGeomPointAddForce(geometry, force);
}

/* Scales the linear velocity and the rotational velocity of a geometry point. */
void RgRobSubBreak(RgGeomPoint *geometry, float linear_scale,
                   float rotational_scale)
{
    RgVector velocity;

    if (geometry == 0) {
        assert_prog(robotSubconAssertGeomNotNil, robotSubconSourcePath, 33);
    }
    RgGeomPointGetVel(geometry, velocity);
    XrgScaleVector(velocity, velocity, linear_scale);
    RgGeomPointSetVel(geometry, velocity);
    RgGeomRobotSetRotVel(&geometry->geom,
                         RgGeomRobotGetRotVel(&geometry->geom) * rotational_scale);
}

/*
 * Sets the rotational velocity toward a target position, clamped to speed.
 * Returns the signed angle to the target, wrapped to [-pi, pi].
 */
float RgRobSubTargetting(RgGeomPoint *geometry,
                         RgVector targetPosition, float speed,
                         float turnRate)
{
    RgVector position;
    RgVector direction;
    float angle;

    __RgGeomPointGetPos(geometry, position, robotSubconSourcePath, 51);
    XrgSubVector(direction, targetPosition, position);
    angle = atan2f(direction[0], direction[2]);
    angle -= RgGeomRobotGetRotate(&geometry->geom);

    if (angle > 3.1415927f) {
        angle -= 6.2831855f;
        while (angle > 3.1415927f) {
            angle -= 6.2831855f;
        }
    }
    if (angle < -3.1415927f) {
        do {
            angle += 6.2831855f;
        } while (angle < -3.1415927f);
    }

    RgGeomRobotSetRotVel(&geometry->geom, 0.0f);
    if (angle < 0.0f) {
        RgGeomRobotSetRotVel(&geometry->geom,
            (-speed < angle / turnRate)
                ? angle / turnRate : -speed);
    } else if (angle > 0.0f) {
        RgGeomRobotSetRotVel(&geometry->geom,
            speed < angle / turnRate
                ? speed : angle / turnRate);
    }
    return angle;
}

/*
 * Steers toward targetGeometry (see RgRobSubTargetting), passing deltaTime on as
 * the turn rate. Returns 0 when targetGeometry is null.
 */
float RgRobSubHoming(RgGeomPoint *geometry, RgGeomPoint *targetGeometry,
                     float speed, float deltaTime)
{
    RgVector targetPosition;

    if (targetGeometry != 0) {
        __RgGeomPointGetPos(targetGeometry, targetPosition,
                            robotSubconSourcePath, 70);
        return RgRobSubTargetting(geometry, targetPosition,
                                  speed, deltaTime);
    }
    return 0.0f;
}

/* Faces the geometry point toward direction on the XZ plane; a zero vector leaves it unchanged. */
void RgRobSubDirTo(RgGeomPoint *geometry, RgVector direction)
{
    if (geometry == 0) {
        assert_prog(robotSubconAssertGeomNotNil, robotSubconSourcePath, 81);
    }
    if (direction[0] == 0.0f && direction[2] == 0.0f) {
        return;
    }
    RgGeomRobotSetRotate(&geometry->geom, atan2f(direction[0], direction[2]));
}

/* Returns the advance or dash motion ID for the velocity direction, quantized to eight directions. */
int RgRobSubGetAdvanceMot(RgGeomPoint *geometry, int dash)
{
    static int s_aeAdvMotID[RG_ROBOT_DIR8_NUM] = { RG_MOT_ADV_F, RG_MOT_ADV_FL, RG_MOT_ADV_L, RG_MOT_ADV_BL, RG_MOT_ADV_B,
                                    RG_MOT_ADV_BR, RG_MOT_ADV_R, RG_MOT_ADV_FR };
    static int s_aeDashMotID[RG_ROBOT_DIR8_NUM] = { RG_MOT_DASH_F, RG_MOT_DASH_FL, RG_MOT_DASH_L, RG_MOT_DASH_BL, RG_MOT_DASH_B,
                                     RG_MOT_DASH_BR, RG_MOT_DASH_R, RG_MOT_DASH_FR };
    RgVector velocity;
    float rotate;
    int direction;

    rotate = RgGeomRobotGetRotate(&geometry->geom);
    RgGeomPointGetVel(geometry, velocity);
    direction = XrgQuantAngle(atan2f(velocity[0], velocity[2]) - rotate);
    if ((unsigned int)direction >= RG_ROBOT_DIR8_NUM) {
        assert_prog(robotSubconAssertDir8InRange, robotSubconSourcePath, 122);
    }
    if (dash != 0) {
        return s_aeDashMotID[direction];
    }
    return s_aeAdvMotID[direction];
}

/* Returns the roll motion for the rotational force: right when negative, left when positive, none at zero. */
int RgRobSubGetRollMotion(RgGeomPoint *geometry)
{
    float rotForce;

    rotForce = RgGeomRobotGetRotForce(&geometry->geom);
    if (rotForce < 0.0f) {
        return RG_MOT_ROLL_R;
    }
    if (rotForce > 0.0f) {
        return RG_MOT_ROLL_L;
    }
    return RG_MOT_NONE;
}

/* Returns the damage motion for a hit direction, quantized to four directions. */
int RgRobSubGetDamageMotion(RgGeomPoint *geometry, RgVector direction)
{
    float angle;
    int damageDirection;

    angle = atan2f(direction[0], direction[2]);
    angle -= RgGeomRobotGetRotate(&geometry->geom);
    angle -= 0.7853982f;
    damageDirection = XrgQuantAngle4(angle);
    switch (damageDirection) {
    case 0:
        return RG_MOT_DAMAGE_FL;
    case 1:
        return RG_MOT_DAMAGE_FR;
    case 2:
        return RG_MOT_DAMAGE_BR;
    case 3:
        return RG_MOT_DAMAGE_BL;
    default:
        RgError(robotSubconUnknownDamageDirFormat, robotSubconSourcePath, 155, XrgQuantAngle4(angle));
        return RG_MOT_NONE;
    }
}
