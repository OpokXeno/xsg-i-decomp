/*
 * OV12 original TU 52: 0x00a2c288..0x00a2ca28 (18 functions)
 */
#include "common.h"
#include "shared.h"
#include "rg_geom_point.h"

extern void assert_prog(const char *expression, const char *source_file,
                        int line);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_geom_point", RgGetGeomGravity);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_geom_point", InitRgGeomPoint);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_geom_point", CreateRgGeomPoint);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_geom_point", RgGeomPointSetPos);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_geom_point", RgGeomPointMovePos);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_geom_point", RgGeomPointResetPos);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_geom_point", RgGeomPointSetVel);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_geom_point", RgGeomPointAddForce);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_geom_point", RgGeomPointSetWeight);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_geom_point", RgGeomPointSetMoveResist);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_geom_point", RgGeomPointSetMaxXYSpd);

void __RgGeomPointGetPos(RgGeomPoint *point, RgPointVector *destination,
                         const char *source_file, int source_line)
{
    if (point == 0) {
        assert_prog(rg_point_assert_expression, source_file, source_line);
    }
    XrgCopyVector((float *)destination,
                  (float *)((const char *)point + 0x20));
}

void __RgGeomPointGetOldPos(RgGeomPoint *point, RgPointVector *destination,
                            const char *source_file, int source_line)
{
    if (point == 0) {
        assert_prog(rg_point_assert_expression, source_file, source_line);
    }
    XrgCopyVector((float *)destination,
                  (float *)((const char *)point + 0x30));
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_geom_point", RgGeomPointGetVel);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_geom_point", RgGeomPointGetSpeed);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_geom_point", RgGeomPointGetWeight);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_geom_point", RgGeomPointGetMoveResist);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_geom_point", RgGeomPointPassTime);
