/*
 * OV12 original TU 43: 0x00a25d30..0x00a26a00 (12 functions)
 */
#include "common.h"
#include "rg_disp_life.h"

extern void assert_prog(const char *expression, const char *source_file,
                        int line);

extern void *RgHeapAlloc(RgHeap *heap, unsigned int size,
                         const char *source_file, int line);
extern void RgHeapFree(RgHeap *heap, void *ptr, const char *source_file,
                       int line);
extern RgHeap *InstanceOfRgHeap(void);

extern void _InitDisp(RgDispLife *pDisp, int dispTex, int timeFont);
extern void _DestructDisp(RgDispLife *pDisp);

/* ov12:0x00a54820 "../rg_disp_life.euc.c" (source filename, scaffold-owned
 * per config/tu-build.json data_ownership: this .rodata window is still
 * owner "asm"). */
extern const char D_00A54820[];
/* ov12:0x00a54838 "pDisp != NIL" */
extern const char D_00A54838[];

INCLUDE_ASM("asm/nonmatchings/ov12/rg_disp_life", _GetAgwsNameUVWH);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_disp_life", _DispAgwsName);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_disp_life", _InitDisp_00A25E58);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_disp_life", _DestructDisp_00A26190);

RgDispLife *CreateRgDispLife(int dispTex, int timeFont)
{
    RgDispLife *pDisp;

    pDisp = RgHeapAlloc(InstanceOfRgHeap(), 0x60, D_00A54820, 0xC7);
    _InitDisp(pDisp, dispTex, timeFont);
    return pDisp;
}

void DisposeRgDispLife(RgDispLife *pDisp)
{
    if (pDisp == 0) {
        assert_prog(D_00A54838, D_00A54820, 0xCF);
    }
    _DestructDisp(pDisp);
    RgHeapFree(InstanceOfRgHeap(), pDisp, D_00A54820, 0xD1);
}

void RgDispLifeSetRobot(RgDispLife *pDisp, RgStatus *pRobot1, RgStatus *pRobot2)
{
    if (pDisp == 0) {
        assert_prog(D_00A54838, D_00A54820, 0xDB);
    }
    pDisp->robot1P = pRobot1;
    pDisp->robot2P = pRobot2;
}

void RgDispLifeSetTimer(RgDispLife *pDisp, float timer)
{
    if (pDisp == 0) {
        assert_prog(D_00A54838, D_00A54820, 0xE5);
    }
    if (timer >= 0.0f) {
        pDisp->timer = timer;
    }
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_disp_life", RgDispLifeSetWin);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_disp_life", RgDispLifeSetVsMode);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_disp_life", RgDispLifePassTime);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_disp_life", RgDispLifeDisp);
