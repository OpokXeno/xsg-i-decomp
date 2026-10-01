/*
 * OV12 original TU 7: 0x00a0cbc8..0x00a0d178 (8 functions)
 */
#include "common.h"
#include "shared.h"

/* ov12:0x00a51e08 contains the assertion expression "pGeom != NIL". */
extern const char D_00A51E08[];
/* ov12:0x00a51e18 contains the source filename "../rg_robot_subcon.euc.c". */
extern const char D_00A51E18[];

extern void assert_prog(const char *expression, const char *source_file,
                        int line);
extern void RgGeomPointAddForce(RgGeomPoint *point, RgVector force);
extern void XrgNormalizeVector(RgVector destination, RgVector source);
extern void XrgScaleVectorXYZ(RgVector destination, RgVector source,
                              float scale);
extern float atan2f(float y, float x);
extern void RgGeomRobotSetRotate(RgGeom *geom, float rotate);
extern float RgGeomRobotGetRotate(const RgGeom *geom);
extern void RgGeomPointGetVel(RgGeomPoint *point, RgVector velocity);
/* The owning geometry-point TU defines this type; this TU only passes it. */
typedef struct RgPointVector RgPointVector;
extern void __RgGeomPointGetPos(RgGeomPoint *point,
                                RgPointVector *destination,
                                const char *source_file, int source_line);
extern int XrgQuantAngle(float angle);
extern const int s_aeAdvMotID_0[8];
extern const int s_aeDashMotID_1[8];
extern int XrgQuantAngle4(float angle);
extern void XrgSubVector(RgVector destination, RgVector first,
                         RgVector second);
extern void RgGeomRobotSetRotVel(RgGeom *geom, float rotVel);
extern void RgError(const char *message, const char *source_file, int line,
                    ...);
/* ov12:0x00a51e38 contains "0 <= nDir8 && nDir8 < 8". */
extern const char D_00A51E38[];
/* ov12:0x00a51e50 contains "unknown damage dir %d". */
extern const char D_00A51E50[];

void RgRobSubAcceralate(RgGeomPoint *geometry, RgVector direction, float scale)
{
    RgVector force;

    if (geometry == 0) {
        assert_prog(D_00A51E08, D_00A51E18, 21);
    }
    XrgNormalizeVector(force, direction);
    XrgScaleVectorXYZ(force, force, scale);
    RgGeomPointAddForce(geometry, force);
}

extern void RgGeomPointGetVel(RgGeomPoint *point, RgVector velocity);
extern void RgGeomPointSetVel(RgGeomPoint *point, RgVector velocity);
extern float RgGeomRobotGetRotVel(const RgGeom *geom);
extern void RgGeomRobotSetRotVel(RgGeom *geom, float rotVel);
extern void XrgScaleVector(RgVector destination, RgVector source,
                           float scale);

void RgRobSubBreak(RgGeomPoint *geometry, float linear_scale,
                   float rotational_scale)
{
    RgVector velocity;

    if (geometry == 0) {
        assert_prog(D_00A51E08, D_00A51E18, 33);
    }
    RgGeomPointGetVel(geometry, velocity);
    XrgScaleVector(velocity, velocity, linear_scale);
    RgGeomPointSetVel(geometry, velocity);
    RgGeomRobotSetRotVel((RgGeom *)geometry,
                         RgGeomRobotGetRotVel((RgGeom *)geometry) * rotational_scale);
}

float RgRobSubTargetting(RgGeomPoint *geometry,
                         RgPointVector *targetPosition, float speed,
                         float turnRate)
{
    RgVector position;
    RgVector direction;
    float angle;

    __RgGeomPointGetPos(geometry, (RgPointVector *)position, D_00A51E18, 51);
    XrgSubVector(direction, (float *)targetPosition, position);
    angle = atan2f(direction[0], direction[2]);
    angle -= RgGeomRobotGetRotate((RgGeom *)geometry);

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

    RgGeomRobotSetRotVel((RgGeom *)geometry, 0.0f);
    if (angle < 0.0f) {
        RgGeomRobotSetRotVel((RgGeom *)geometry,
            (-speed < angle / turnRate)
                ? angle / turnRate : -speed);
    } else if (angle > 0.0f) {
        RgGeomRobotSetRotVel((RgGeom *)geometry,
            speed < angle / turnRate
                ? speed : angle / turnRate);
    }
    return angle;
}

float RgRobSubHoming(RgGeomPoint *geometry, RgGeomPoint *targetGeometry,
                     float speed, float turnRate)
{
    RgVector targetPosition;

    if (targetGeometry != 0) {
        __RgGeomPointGetPos(targetGeometry, (RgPointVector *)targetPosition,
                            D_00A51E18, 70);
        return RgRobSubTargetting(geometry, (RgPointVector *)targetPosition,
                                  speed, turnRate);
    }
    return 0.0f;
}

void RgRobSubDirTo(RgGeomPoint *geometry, RgVector direction)
{
    if (geometry == 0) {
        assert_prog(D_00A51E08, D_00A51E18, 81);
    }
    if (direction[0] == 0.0f && direction[2] == 0.0f) {
        return;
    }
    RgGeomRobotSetRotate((RgGeom *)geometry, atan2f(direction[0], direction[2]));
}

int RgRobSubGetAdvanceMot(RgGeomPoint *geometry, int dash)
{
    RgVector velocity;
    float rotate;
    int direction;

    rotate = RgGeomRobotGetRotate((RgGeom *)geometry);
    RgGeomPointGetVel(geometry, velocity);
    direction = XrgQuantAngle(atan2f(velocity[0], velocity[2]) - rotate);
    if ((unsigned int)direction >= 8) {
        assert_prog(D_00A51E38, D_00A51E18, 122);
    }
    if (dash != 0) {
        return s_aeDashMotID_1[direction];
    }
    return s_aeAdvMotID_0[direction];
}

extern float RgGeomRobotGetRotForce(const RgGeom *geom);

/*
 * Motion id 9 for a negative rotational force, 10 for a positive one, -1
 * (no roll) when the force is exactly zero.
 */
int RgRobSubGetRollMotion(const RgGeom *geom)
{
    float rotForce;

    rotForce = RgGeomRobotGetRotForce(geom);
    if (rotForce < 0.0f) {
        return 9;
    }
    if (rotForce > 0.0f) {
        return 10;
    }
    return -1;
}

int RgRobSubGetDamageMotion(RgGeomPoint *geometry, RgVector direction)
{
    float angle;
    int damageDirection;

    angle = atan2f(direction[0], direction[2]);
    angle -= RgGeomRobotGetRotate((RgGeom *)geometry);
    angle -= 0.7853982f;
    damageDirection = XrgQuantAngle4(angle);
    switch (damageDirection) {
    case 0:
        return 23;
    case 1:
        return 24;
    case 2:
        return 25;
    case 3:
        return 26;
    default:
        RgError(D_00A51E50, D_00A51E18, 155, XrgQuantAngle4(angle));
        return -1;
    }
}
