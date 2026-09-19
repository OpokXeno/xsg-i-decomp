/*
 * OV12 original TU 71: 0x00a3cd10..0x00a3e518 (26 functions)
 */
#include "common.h"
#include "shared.h"
#include "rg_select_robot.h"

extern void assert_prog(const char *expression, const char *source_file,
                        int line);
extern RgHeap *InstanceOfRgHeap(void);
extern void *RgHeapAlloc(void *heap, unsigned int size,
                         const char *source_file, int line);
extern void RgHeapFree(void *heap, void *ptr, const char *source_file,
                       int line);

static void _InitSelRob(RgSelectRobot *pCont);
static void _DestructSelRob(RgSelectRobot *pCont);
static void _JobAct(RgSelectRobot *pCont, float deltaTime);

/* Linker witness: this TU's own source-file name, used by every assert here. */
extern const char D_00A56DA0[];
/* Linker witness for the original literal at ov12:0x00a56ef8 ("pCont != NIL"). */
extern const char D_00A56EF8[];
/* Linker witness for the original literal at ov12:0x00a56d90 ("who are you ?"). */
extern const char D_00A56D90[];

/* jal RgWarn(format, file, line, ...), the XrgLogSys-shaped debug warning
   (also declared this way by src/ov12/rg_main.c, its accepted caller). */
extern void RgWarn(const char *format, const char *file, int line, ...);

/*
 * xglCdReadFile's completion callback: only completion code 4 does
 * anything, and only when a requester (s_pWhoAreYou) is on file to receive
 * the count; any other requester-less code-4 completion is logged instead
 * of counted.  Codes below -2 and codes 1..3 are guarded out and ignored.
 */
static void _ReadCallback(int completionCode)
{
    if (completionCode < -2) {
        return;
    }
    if (completionCode < 4) {
        return;
    }
    if (completionCode != 4) {
        return;
    }
    if (s_pWhoAreYou != 0) {
        s_uOkNum++;
    } else {
        RgWarn(D_00A56D90, D_00A56DA0, 55);
    }
    s_bLoading = 0;
}

static void _KillLoad(void)
{
    if (s_bLoading != 0) {
        xglCdReadCancel();
        s_bLoading = 0;
    }
    s_uReqNum = (s_uOkNum = 0);
    s_pWhoAreYou = 0;
}

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

RgSelectRobot *CreateRgSelectRobot(void)
{
    RgSelectRobot *pCont;

    pCont = RgHeapAlloc(InstanceOfRgHeap(), RG_SELECT_ROBOT_SIZE, D_00A56DA0,
                        745);
    _InitSelRob(pCont);
    return pCont;
}

void DisposeRgSelectRobot(RgSelectRobot *pCont)
{
    if (pCont == 0) {
        assert_prog(D_00A56EF8, D_00A56DA0, 753);
    }
    _DestructSelRob(pCont);
    RgHeapFree(InstanceOfRgHeap(), pCont, D_00A56DA0, 755);
    xglCdReadCancel();
}

void RgSelectRobotScreenPos(RgSelectRobot *pCont, float screenPos)
{
    if (pCont == 0) {
        assert_prog(D_00A56EF8, D_00A56DA0, 766);
    }
    pCont->screenPos = screenPos;
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_robot", RgSelectRobotSetMode);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_robot", RgSelectRobotSet);

void RgSelectRobotPassTime(RgSelectRobot *pCont, float deltaTime)
{
    if (pCont == 0) {
        assert_prog(D_00A56EF8, D_00A56DA0, 820);
    }
    _JobAct(pCont, deltaTime);
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_robot", RgSelectRobotDisp);

static void _DumpHead(void)
{
}
