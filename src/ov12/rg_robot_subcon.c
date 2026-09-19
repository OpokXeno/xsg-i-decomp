/*
 * OV12 original TU 7: 0x00a0cbc8..0x00a0d178 (8 functions)
 */
#include "common.h"
#include "shared.h"

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot_subcon", RgRobSubAcceralate);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot_subcon", RgRobSubBreak);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot_subcon", RgRobSubTargetting);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot_subcon", RgRobSubHoming);

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
