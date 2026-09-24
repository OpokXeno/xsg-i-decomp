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

static void _KillLoadIfMe(void *owner)
{
    if (s_uReqNum != 0) {
        if (s_pWhoAreYou == owner) {
            _KillLoad();
        }
    }
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_robot", _ReqLoad);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_robot", _IsEndOfLoad);

extern int xglCdReadFile(const char *name, void *buffer, int mode, int flags);
extern void *s_apBuf[4];
extern char s_aszData[4][64];

static void _JobLoad(void)
{
    if (s_uReqNum != 0 && s_uOkNum < s_uReqNum) {
        if (s_bLoading == 0 &&
            xglCdReadFile(s_aszData[s_uOkNum], s_apBuf[s_uOkNum], 1,
                          (int) _ReadCallback) >= 0) {
            s_bLoading = 1;
        }
        return;
    }
    s_pWhoAreYou = 0;
}

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

extern void XrgLinearIntpVector(RgVector destination, RgVector first,
                                 RgVector second, float weight);
/* The two positions _ActorPos interpolates between, weighted by
   RgSelectRobot::screenPos: index 1 is the weighted argument, index 0 the
   complement-weighted one. */
extern RgVector s_aPos_1[2];

static void _ActorPos(RgSelectRobot *pCont, RgVector position)
{
    XrgLinearIntpVector(position, s_aPos_1[1], s_aPos_1[0], pCont->screenPos);
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_robot", _ActorPosUpdate);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_robot", _ActivateAct);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_robot", _ReqAct);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_robot", _DestructAct);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_robot", _JobAct);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_robot", _DrawAct);

extern RgHeap *InstanceOfRgHeapData(void);
static void _InitAct(RgSelectRobot *pCont, void *heap, int regionBytes, int count);

void _InitSelRob(RgSelectRobot *pCont)
{
    void *heapBuffer;

    if (pCont == 0) {
        assert_prog(D_00A56EF8, D_00A56DA0, 720);
    }
    _InitLoad();
    heapBuffer = RgHeapAlloc(InstanceOfRgHeapData(), 0xC00000U, D_00A56DA0, 726);
    pCont->heapBuffer = heapBuffer;
    _InitAct(pCont, heapBuffer, 0xC0000, 1);
}

extern void ACT_init(void);
static void _DestructAct(RgSelectRobot *pCont);

void _DestructSelRob(RgSelectRobot *pCont)
{
    if (pCont == 0) {
        assert_prog(D_00A56EF8, D_00A56DA0, 734);
    }
    _KillLoad();
    _DestructAct(pCont);
    RgHeapFree(InstanceOfRgHeapData(), pCont->heapBuffer, D_00A56DA0, 737);
    ACT_init();
}

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

extern void InitXrgActorEssence(void *essence, int actorID);
static void _ReqAct(RgSelectRobot *pCont, void *essence, int *accessoryIDs,
                     int *extra);

void RgSelectRobotSet(RgSelectRobot *pCont, int *selection)
{
    /* Sized to match InitXrgActorEssence's record (xrg_actor.c,
       ov12/tu080); RgSelectRobotSet never reads or writes its fields
       directly. */
    unsigned char essence[0x88];

    if (pCont == 0) {
        assert_prog(D_00A56EF8, D_00A56DA0, 801);
    }
    InitXrgActorEssence(essence, selection[0]);
    _ReqAct(pCont, essence, selection + 1, selection + 4);
}

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
