/*
 * OV12 original TU 54: 0x00a2d290..0x00a2d900 (14 functions)
 */
#include "common.h"
#include "shared.h"
#include "rg_geom_robot.h"

INCLUDE_ASM("asm/nonmatchings/ov12/rg_geom_robot", _InitRgGeomRobot);

RgGeom *CreateRgGeomRobot(void)
{
    RgGeom *geom;

    geom = RgHeapAlloc(InstanceOfRgHeap(), 0xA0, D_00A55180, 51);
    if (geom == 0) {
        assert_prog(D_00A55170, D_00A55180, 52);
    }
    _InitRgGeomRobot(geom);
    return geom;
}

void InitRgGeomRobot(RgGeom *geom)
{
    if (geom == 0) {
        assert_prog(D_00A55198, D_00A55180, 59);
    }
    _InitRgGeomRobot(geom);
}

void RgGeomRobotSetRotate(RgGeom *geom, float rotate)
{
    if (geom == 0) {
        assert_prog(D_00A55170, D_00A55180, 68);
    }
    RgGeomRobotStateAt(geom)->rotate = rotate;
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_geom_robot", RgGeomRobotSetDir);

void RgGeomRobotSetRotVel(RgGeom *geom, float rotVel)
{
    if (geom == 0) {
        assert_prog(D_00A55170, D_00A55180, 80);
    }
    RgGeomRobotStateAt(geom)->rotVel = rotVel;
}

void RgGeomRobotAddRotForce(RgGeom *geom, float force)
{
    if (geom == 0) {
        assert_prog(D_00A55170, D_00A55180, 86);
    }
    RgGeomRobotStateAt(geom)->rotForce += force;
}

void RgGeomRobotSetRotResist(RgGeom *geom, float resist)
{
    if (geom == 0) {
        assert_prog(D_00A55170, D_00A55180, 94);
    }
    RgGeomRobotStateAt(geom)->rotResist = resist;
}

void RgGeomRobotSetMaxRotVel(RgGeom *geom, float maxRotVel)
{
    if (geom == 0) {
        assert_prog(D_00A55170, D_00A55180, 101);
    }
    RgGeomRobotStateAt(geom)->maxRotVel = maxRotVel;
}

float RgGeomRobotGetRotate(const RgGeom *geom)
{
    if (geom == 0) {
        assert_prog(D_00A55170, D_00A55180, 111);
    }
    return RgGeomRobotStateConstAt(geom)->rotate;
}

float RgGeomRobotGetRotVel(const RgGeom *geom)
{
    if (geom == 0) {
        assert_prog(D_00A55170, D_00A55180, 117);
    }
    return RgGeomRobotStateConstAt(geom)->rotVel;
}

float RgGeomRobotGetRotForce(const RgGeom *geom)
{
    if (geom == 0) {
        assert_prog(D_00A55170, D_00A55180, 123);
    }
    return RgGeomRobotStateConstAt(geom)->rotForce;
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_geom_robot", RgGeomRobotCalcLocal);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_geom_robot", RgGeomRobotPassTime);
