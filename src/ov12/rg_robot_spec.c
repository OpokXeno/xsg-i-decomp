/*
 * OV12 original TU 8: 0x00a0d178..0x00a0db58 (5 functions)
 */
#include "common.h"
#include "shared.h"
#include "rg_robot_spec.h"

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot_spec", InitRgRobotSpec);

static void _ResetAnaAutoHoming(void)
{
    s_nAnaAutoHomingID = -1;
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot_spec", _AnaAutoHoming);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot_spec", RgRobotSpecReadFromText);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot_spec", RgRobotSpecDump);
