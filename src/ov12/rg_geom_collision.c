/*
 * OV12 original TU 58: 0x00a2f650..0x00a303a8 (15 functions)
 */
#include "common.h"

INCLUDE_ASM("asm/nonmatchings/ov12/rg_geom_collision", _BallVsBall);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_geom_collision", _BallVsPoly);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_geom_collision", _PolyVsBall);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_geom_collision", _RobotVsPoly);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_geom_collision", _PolyVsRobot);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_geom_collision", _BallVsTray);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_geom_collision", _TrayVsBall);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_geom_collision", _BallVsPillar);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_geom_collision", _PillarVsBall);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_geom_collision", _RobVsTray);

/* Still INCLUDE_ASM in this TU. */
static void _TrayVsBall(void);

/* Reuses tray-versus-ball collision handling for a robot collider (elf_names
 * annotation); a plain tail call to _TrayVsBall. */
static void _TrayVsRob(void)
{
    _TrayVsBall();
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_geom_collision", _RobVsPillar);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_geom_collision", _PillarVsRob);

extern void assert_prog(const char *expression, const char *source_file, int line);

extern const char D_00A55328[]; /* "pArg != NIL" */
extern const char D_00A552F8[]; /* "../rg_geom_collision.euc.c" */

/*
 * The pairwise collision-check argument this TU's _XxxVsYyy functions use
 * (still INCLUDE_ASM in this allocation, so only InitRgGeomColiArg's own
 * writes are evidenced here: it clears exactly these four words to a common
 * role, so they are modelled as one array rather than four distinct fields).
 */
typedef struct RgGeomColiArg {
    unsigned int data[4];
} RgGeomColiArg;

void InitRgGeomColiArg(RgGeomColiArg *pArg)
{
    if (pArg == 0) {
        assert_prog(D_00A55328, D_00A552F8, 465);
    }
    pArg->data[0] = 0;
    pArg->data[1] = 0;
    pArg->data[2] = 0;
    pArg->data[3] = 0;
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_geom_collision", RgGeomCollisionJob);
