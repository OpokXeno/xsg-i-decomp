/*
 * OV12 original TU 5: 0x00a09480..0x00a09ba0 (9 functions)
 */
#include "common.h"
#include "rg_robot_control.h"

extern void assert_prog(const char *expression, const char *source_file,
                        int line);
extern RgHeap *InstanceOfRgHeap(void);
extern void *RgHeapAlloc(RgHeap *heap, unsigned int size,
                         const char *source_file, int line);
extern void RgHeapFree(RgHeap *heap, void *pointer, const char *source_file,
                       int line);

/*
 * External file-backed witnesses, not candidate-emitted data: this window is
 * asm-owned scaffold data (splat names, no config/symbols/ov12.txt entry).
 *
 * ov12:0x00a51c20 contains the assertion expression "pControl != NIL".
 * ov12:0x00a51c30 contains the source filename "../rg_robot_control.euc.c".
 */
extern const char D_00A51C20[];
extern const char D_00A51C30[];

/*
 * ov12:0x00a51c50 contains the assertion expression "pCam != NIL", the same
 * asm-owned scaffold data window as above; _InitControlInput reuses it as a
 * generic non-null check on its pEssence argument.
 */
extern const char D_00A51C50[];

extern void InitXrgInput(void *pBuffer, int padId);

static void _InitControlInput(RgControlInput *pInput, RgRobot *pRobot,
                              void *pEssence, int padId);
static void _jobControlInput(RgRobotControl *pControl);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot_control", _jobControlInput);

void RgRobotControlSetRobot(RgRobotControl *pControl, RgRobot *pRobot)
{
    if (pControl == 0) {
        assert_prog(D_00A51C20, D_00A51C30, 175);
    }
    pControl->robot = pRobot;
}

static void _DestructControlInput(RgRobotControl *pControl)
{
    RgControlInput *pInput;

    if (pControl == 0) {
        assert_prog(D_00A51C20, D_00A51C30, 185);
    }
    pInput = (RgControlInput *)pControl;
    RgHeapFree(InstanceOfRgHeap(), pInput->buffer, D_00A51C30, 186);
}

void InitRgRobotControlCommon(RgRobotControl *pControl, RgRobot *pRobot)
{
    if (pControl == 0) {
        assert_prog(D_00A51C20, D_00A51C30, 195);
    }
    pControl->robot = pRobot;
    pControl->jobMethod = 0;
    pControl->destructMethod = 0;
}

static void _InitControlInput(RgControlInput *pInput, RgRobot *pRobot,
                              void *pEssence, int padId)
{
    void *pBuffer;

    if (pInput == 0) {
        assert_prog(D_00A51C20, D_00A51C30, 204);
    }
    if (pEssence == 0) {
        assert_prog(D_00A51C50, D_00A51C30, 205);
    }
    InitRgRobotControlCommon(&pInput->control, pRobot);
    pInput->control.destructMethod = _DestructControlInput;
    pInput->control.jobMethod = _jobControlInput;
    pBuffer = RgHeapAlloc(InstanceOfRgHeap(), 48, D_00A51C30, 213);
    pInput->buffer = pBuffer;
    InitXrgInput(pBuffer, padId);
    pInput->essence = pEssence;
}

RgRobotControl *CreateRgRobotControlNul(RgRobot *pRobot)
{
    RgRobotControl *pControl;

    pControl = RgHeapAlloc(InstanceOfRgHeap(), sizeof(RgRobotControl), D_00A51C30, 224);
    if (pControl == 0) {
        assert_prog(D_00A51C20, D_00A51C30, 225);
    }
    InitRgRobotControlCommon(pControl, pRobot);
    return pControl;
}

RgRobotControl *CreateRgRobotControlInput(RgRobot *pRobot, void *pEssence,
                                          int padId)
{
    RgControlInput *pInput;

    pInput = RgHeapAlloc(InstanceOfRgHeap(), sizeof(RgControlInput), D_00A51C30,
                         233);
    if (pInput == 0) {
        assert_prog(D_00A51C20, D_00A51C30, 234);
    }
    _InitControlInput(pInput, pRobot, pEssence, padId);
    return &pInput->control;
}

void DisposeRgRobotControl(RgRobotControl *pControl)
{
    if (pControl == 0) {
        assert_prog(D_00A51C20, D_00A51C30, 242);
    }
    if (pControl->destructMethod != 0) {
        pControl->destructMethod(pControl);
    }
    RgHeapFree(InstanceOfRgHeap(), pControl, D_00A51C30, 245);
}

void RgRobotControlJob(RgRobotControl *pControl)
{
    if (pControl == 0) {
        assert_prog(D_00A51C20, D_00A51C30, 253);
    }
    if (pControl->jobMethod != 0) {
        pControl->jobMethod(pControl);
    }
}
