/*
 * OV12 original TU 71: 0x00a3cd10..0x00a3e518 (26 functions)
 */
#include "common.h"
#include "rg_select_robot.h"

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_robot", _ReadCallback);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_robot", _KillLoad);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_robot", _KillLoadIfMe);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_robot", _ReqLoad);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_robot", _IsEndOfLoad);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_robot", _JobLoad);

/*
 * Reset the paired request-completion counters through the helper interface
 * used by the load-state owner.  Its parameter order is completion then
 * request, while the reset order is request then completion.
 */
static __inline__ void ResetLoadCounts(unsigned int *completed_count,
                                      unsigned int *request_count)
{
    *request_count = 0;
    *completed_count = 0;
}

/* Original OV12 local function at 0x00a3d008. */
static void _InitLoad(void)
{
    s_bLoading = 0;
    ResetLoadCounts(&s_uOkNum, &s_uReqNum);
    s_pWhoAreYou = 0;
    xglCdReadCancel();
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_robot", _InitAct);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_robot", _select_light);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_robot", _ActorPos);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_robot", _ActorPosUpdate);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_robot", _ActivateAct);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_robot", _ReqAct);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_robot", _DestructAct);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_robot", _JobAct);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_robot", _DrawAct);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_robot", _InitSelRob);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_robot", _DestructSelRob);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_robot", CreateRgSelectRobot);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_robot", DisposeRgSelectRobot);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_robot", RgSelectRobotScreenPos);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_robot", RgSelectRobotSetMode);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_robot", RgSelectRobotSet);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_robot", RgSelectRobotPassTime);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_robot", RgSelectRobotDisp);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_robot", _DumpHead_00A3E510);
