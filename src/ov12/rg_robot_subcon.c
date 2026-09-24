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

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot_subcon", RgRobSubTargetting);

/* Opaque here: rg_geom_point.c (ov12/tu052) owns the RgPointVector definition. */
typedef struct RgPointVector RgPointVector;
extern float RgRobSubTargetting(RgGeomPoint *geometry,
                                RgPointVector *targetPosition, float speed,
                                float turnRate);
extern void __RgGeomPointGetPos(RgGeomPoint *point,
                                RgPointVector *destination,
                                const char *source_file, int source_line);

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

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot_subcon", RgRobSubDirTo);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot_subcon", RgRobSubGetAdvanceMot);

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

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot_subcon", RgRobSubGetDamageMotion);
