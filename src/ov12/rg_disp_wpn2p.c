/*
 * OV12 original TU 42: 0x00a251a8..0x00a25d30 (17 functions)
 */
#include "common.h"
#include "rg_disp_wpn2p.h"

extern void assert_prog(const char *expression, const char *source_file,
                        int line);

extern void *RgHeapAlloc(RgHeap *heap, unsigned int size,
                         const char *source_file, int line);
extern void RgHeapFree(RgHeap *heap, void *ptr, const char *source_file,
                       int line);
extern RgHeap *InstanceOfRgHeap(void);

extern float RgRobotGetDashTime(RgStatus *pRobot);
extern void RgGaugeSetValue(void *gauge, float value);
extern void RgGaugePassTime(void *gauge, float deltaTime);

extern void _InitDisp(RgDispWpn2P *pDisp);
extern void _DestructRobInfo(RobInfo *pInfo);
extern void _SetRobInfo(RobInfo *pInfo, RgStatus *pRobot, int side);

/* ov12:0x00a54780 "../rg_disp_wpn2p.euc.c" (source filename, scaffold-owned
 * per config/tu-build.json data_ownership: this .rodata window is still
 * owner "asm"). */
extern const char D_00A54780[];
/* ov12:0x00a54770 "pWep != NIL" */
extern const char D_00A54770[];
/* ov12:0x00a547a0 "pInfo != NIL" */
extern const char D_00A547A0[];
/* ov12:0x00a547f0 "pDisp != NIL" */
extern const char D_00A547F0[];

static void _InitWep(WepInfo *pWep)
{
    if (pWep == 0) {
        assert_prog(D_00A54770, D_00A54780, 0x28);
    }
    pWep->weapon = 0;
    pWep->index = -1;
}

static void _DestructWep(WepInfo *pWep)
{
    if (pWep == 0) {
        assert_prog(D_00A54770, D_00A54780, 0x31);
    }
}

static void _SetWep(WepInfo *pWep, void *weapon, int index)
{
    if (pWep == 0) {
        assert_prog(D_00A54770, D_00A54780, 0x37);
    }
    pWep->weapon = weapon;
    pWep->index = index;
    if (weapon == 0 || index == -1) {
        pWep->index = index;
        pWep->weapon = 0;
    }
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_disp_wpn2p", _UpdateWep);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_disp_wpn2p", _DispWep);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_disp_wpn2p", _InitRobInfo);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_disp_wpn2p", _DestructRobInfo);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_disp_wpn2p", _SetRobInfo);

static void _PassTimeRobInfo(RobInfo *pInfo, float deltaTime)
{
    RgStatus *robot;

    if (pInfo == 0) {
        assert_prog(D_00A547A0, D_00A54780, 0xFB);
    }
    robot = pInfo->robot;
    if (robot == 0) {
        return;
    }
    RgGaugeSetValue(pInfo->gauge, RgRobotGetDashTime(robot));
    RgGaugePassTime(pInfo->gauge, deltaTime);
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_disp_wpn2p", _DispRobInfo);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_disp_wpn2p", _InitDisp_00A25A30);

static void _DestructDisp(RgDispWpn2P *pDisp)
{
    if (pDisp == 0) {
        assert_prog(D_00A547F0, D_00A54780, 0x158);
    }
    _DestructRobInfo(&pDisp->rob[0]);
    _DestructRobInfo(&pDisp->rob[1]);
}

RgDispWpn2P *CreateRgDispWpn2P(void)
{
    RgDispWpn2P *pDisp;

    pDisp = RgHeapAlloc(InstanceOfRgHeap(), sizeof(RgDispWpn2P), D_00A54780,
                        0x161);
    _InitDisp(pDisp);
    return pDisp;
}

void DisposeRgDispWpn2P(RgDispWpn2P *pDisp)
{
    if (pDisp == 0) {
        assert_prog(D_00A547F0, D_00A54780, 0x169);
    }
    _DestructDisp(pDisp);
    RgHeapFree(InstanceOfRgHeap(), pDisp, D_00A54780, 0x16B);
}

void RgDispWpn2PSetRobot(RgDispWpn2P *pDisp, RgStatus *pRobot1,
                         RgStatus *pRobot2)
{
    if (pDisp == 0) {
        assert_prog(D_00A547F0, D_00A54780, 0x175);
    }
    _SetRobInfo(&pDisp->rob[0], pRobot1, 0);
    _SetRobInfo(&pDisp->rob[1], pRobot2, 1);
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_disp_wpn2p", RgDispWpn2PPassTime);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_disp_wpn2p", RgDispWpn2PDisp);
