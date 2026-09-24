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
static void _CheckDatas(RgGaugeDisp *disp);
static void _InitDisp(RgGaugeDisp *disp);

static void _CheckDatas(RgGaugeDisp *disp)
{
    if (disp == 0)
        assert_prog(D_00A54640, D_00A54650, 45);
    disp->validState = 2;
    if (disp->barPic != 0 && disp->barUVWH[2] >= 0 && disp->barUVWH[3] >= 0) {
        if (disp->restPic != 0 && disp->restUVWH[2] >= 0 && disp->restUVWH[3] >= 0) {
            disp->validState = 0;
            return;
        }
        disp->validState = 1;
    }
}

static void _InitDisp(RgGaugeDisp *disp)
{
    if (disp == 0)
        assert_prog(D_00A54640, D_00A54650, 61);
    disp->currentValue = 100.0f;
    disp->validState = 2;
    disp->maxValue = 100.0f;
    disp->barPic = 0;
    disp->restPic = 0;
    XrgSetIVector(disp->barUVWH, 0, 0, 0, 0);
    XrgSetIVector(disp->restUVWH, 0, 0, 0, 0);
    disp->posX = 0;
    disp->alpha = 1;
    disp->order = 0;
    disp->posY = 0;
}

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

extern void XrgPaint2DUseTexture(XrgPaint2D *paint, int texture);
extern void XrgPaint2DSetUVOffset(XrgPaint2D *paint, int u, int v);
extern void XrgPaint2DSetUVSize(XrgPaint2D *paint, int width, int height);
extern void XrgPaint2DAlpha(XrgPaint2D *paint, int blend);
extern void XrgPaint2DDrawXYWH(XrgPaint2D *paint, int mode, int x, int y,
                               int width, int height);

static void _disp_bar(XrgPaint2D *paint, RgGaugeDisp *disp,
                      RgGaugeDispRect *rest)
{
    int barWidth;
    int y;
    int barHeight;
    int x;
    int filledWidth;
    int remainingWidth;
    int restX;

    XrgPaint2DUseTexture(paint, disp->barPic);
    XrgPaint2DSetUVOffset(paint, disp->barUVWH[0], disp->barUVWH[1]);
    XrgPaint2DSetUVSize(paint, disp->barUVWH[2], disp->barUVWH[3]);
    XrgPaint2DAlpha(paint, disp->alpha);
    barWidth = disp->barUVWH[2];
    y = disp->posY;
    barHeight = disp->barUVWH[3];
    x = disp->posX;
    filledWidth = (int) ((float) barWidth * disp->currentValue / disp->maxValue);
    if (disp->order != 0) {
        remainingWidth = barWidth - filledWidth;
        x += remainingWidth;
        rest->x = x;
    } else {
        restX = x + filledWidth;
        remainingWidth = barWidth - filledWidth;
        rest->x = restX;
    }
    XrgPaint2DDrawXYWH(paint, 0, x, y, filledWidth, barHeight);
    rest->y = y;
    rest->width = remainingWidth;
    rest->height = barHeight;
}

static void _disp_bar_rest(XrgPaint2D *paint, RgGaugeDisp *disp,
                           const RgGaugeDispRect *rest)
{
    XrgPaint2DUseTexture(paint, disp->restPic);
    XrgPaint2DSetUVOffset(paint, disp->restUVWH[0], disp->restUVWH[1]);
    XrgPaint2DSetUVSize(paint, disp->restUVWH[2], disp->restUVWH[3]);
    XrgPaint2DAlpha(paint, disp->alpha);
    XrgPaint2DDrawXYWH(paint, 0, rest->x, rest->y, rest->width, rest->height);
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_gauge_disp", RgGaugeDispDraw);
