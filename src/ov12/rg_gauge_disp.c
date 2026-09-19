/*
 * OV12 original TU 40: 0x00a23e48..0x00a245d8 (16 functions)
 */
#include "common.h"
#include "shared.h"
#include "rg_gauge_disp.h"

extern void assert_prog(const char *expression, const char *source_file,
                        int line);
extern void *RgHeapAlloc(void *heap, unsigned int size,
                         const char *source_file, int line);
extern void RgHeapFree(void *heap, void *ptr, const char *source_file,
                       int line);
extern RgHeap *InstanceOfRgHeap(void);
extern void XrgSetIVector(int *destination, int texU, int texV, int texW,
                          int texH);
void _CheckDatas(RgGaugeDisp *disp);
void _InitDisp(RgGaugeDisp *disp);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_gauge_disp", _CheckDatas);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_gauge_disp", _InitDisp_00A23EE0);

static void _DestructDisp(RgGaugeDisp *disp)
{
    if (disp == 0)
        assert_prog(D_00A54640, D_00A54650, 88);
}

RgGaugeDisp *CreateRgGaugeDisp(void)
{
    RgGaugeDisp *disp;

    disp = RgHeapAlloc(InstanceOfRgHeap(), sizeof(RgGaugeDisp), D_00A54650,
                       95);
    _InitDisp(disp);
    return disp;
}

void DisposeRgGaugeDisp(RgGaugeDisp *disp)
{
    if (disp == 0)
        assert_prog(D_00A54640, D_00A54650, 103);
    _DestructDisp(disp);
    RgHeapFree(InstanceOfRgHeap(), disp, D_00A54650, 105);
}

int RgGaugeDispIsDrawable(RgGaugeDisp *disp)
{
    if (disp == 0)
        assert_prog(D_00A54640, D_00A54650, 115);
    return disp->validState < 2;
}

void RgGaugeDispSetValue(RgGaugeDisp *disp, float currentValue,
                         float maxValue)
{
    if (disp == 0)
        assert_prog(D_00A54640, D_00A54650, 131);
    disp->currentValue = currentValue;
    disp->maxValue = maxValue;
    _CheckDatas(disp);
}

void RgGaugeDispSetTex(RgGaugeDisp *disp, int barPic, int restPic)
{
    if (disp == 0)
        assert_prog(D_00A54640, D_00A54650, 144);
    disp->barPic = barPic;
    disp->restPic = restPic;
    _CheckDatas(disp);
}

void RgGaugeDispSetBarUVWH(RgGaugeDisp *disp, int texU, int texV, int texW,
                           int texH)
{
    if (disp == 0)
        assert_prog(D_00A54640, D_00A54650, 157);
    XrgSetIVector(disp->barUVWH, texU, texV, texW, texH);
    _CheckDatas(disp);
}

void RgGaugeDispSetRestUVWH(RgGaugeDisp *disp, int texU, int texV, int texW,
                            int texH)
{
    if (disp == 0)
        assert_prog(D_00A54640, D_00A54650, 169);
    XrgSetIVector(disp->restUVWH, texU, texV, texW, texH);
    _CheckDatas(disp);
}

void RgGaugeDispSetPos(RgGaugeDisp *disp, int x, int y)
{
    if (disp == 0)
        assert_prog(D_00A54640, D_00A54650, 180);
    disp->posX = x;
    disp->posY = y;
    _CheckDatas(disp);
}

void RgGaugeDispSetOrder(RgGaugeDisp *disp, int order)
{
    if (disp == 0)
        assert_prog(D_00A54640, D_00A54650, 193);
    disp->order = order;
    _CheckDatas(disp);
}

void RgGaugeDispSetAlpha(RgGaugeDisp *disp, int alpha)
{
    if (disp == 0)
        assert_prog(D_00A54640, D_00A54650, 204);
    disp->alpha = alpha;
    _CheckDatas(disp);
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_gauge_disp", _disp_bar);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_gauge_disp", _disp_bar_rest);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_gauge_disp", RgGaugeDispDraw);
