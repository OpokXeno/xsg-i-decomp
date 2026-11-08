/*
 * OV12 original TU 40: 0x00a23e48..0x00a245d8 (16 functions)
 */
#include "common.h"
#include "shared.h"
#include "ov12/rg_gauge_disp.h"
#include "ov12/xrg_paint2d.h"
#include "ov12/assert_prog.h"
#include "ov12/rg_heap.h"
#include "ov12/xrg_rand_int.h"

/* RgGaugeDisp.validState: what _CheckDatas found drawable. */
#define RG_GAUGE_DISP_DRAW_BAR_AND_REST 0
#define RG_GAUGE_DISP_DRAW_BAR_ONLY 1
#define RG_GAUGE_DISP_DRAW_NOTHING 2

#define RG_GAUGE_DISP_FULL_VALUE 100.0f

/*
 * The rectangle _disp_bar computes for the drawn bar's remainder and hands
 * to _disp_bar_rest.
 */
typedef struct RgGaugeDispRect {
    int x;
    int y;
    int width;
    int height;
} RgGaugeDispRect;

static void _CheckDatas(RgGaugeDisp *disp);
static void _InitDisp(RgGaugeDisp *disp);

const char gaugeDispAssertDispNotNil[] = "pDisp != NIL";
const char gaugeDispSourcePath[] = "../rg_gauge_disp.euc.c";
const char gaugeDispAssertPaintNotNil[] = "pPaint != NIL";

/* Sets validState from the bar and rest textures and their UV sizes. */
static void _CheckDatas(RgGaugeDisp *disp)
{
    if (disp == 0)
        assert_prog(gaugeDispAssertDispNotNil, gaugeDispSourcePath, 45);
    disp->validState = RG_GAUGE_DISP_DRAW_NOTHING;
    if (disp->barPic != 0 && disp->barUVWH[2] >= 0 && disp->barUVWH[3] >= 0) {
        if (disp->restPic != 0 && disp->restUVWH[2] >= 0 && disp->restUVWH[3] >= 0) {
            disp->validState = RG_GAUGE_DISP_DRAW_BAR_AND_REST;
            return;
        }
        disp->validState = RG_GAUGE_DISP_DRAW_BAR_ONLY;
    }
}

/* Resets a gauge to its default values: full value, no textures, origin. */
static void _InitDisp(RgGaugeDisp *disp)
{
    if (disp == 0)
        assert_prog(gaugeDispAssertDispNotNil, gaugeDispSourcePath, 61);
    disp->currentValue = RG_GAUGE_DISP_FULL_VALUE;
    disp->validState = RG_GAUGE_DISP_DRAW_NOTHING;
    disp->maxValue = RG_GAUGE_DISP_FULL_VALUE;
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
        assert_prog(gaugeDispAssertDispNotNil, gaugeDispSourcePath, 88);
}

RgGaugeDisp *CreateRgGaugeDisp(void)
{
    RgGaugeDisp *disp;

    disp = RgHeapAlloc(InstanceOfRgHeap(), sizeof(RgGaugeDisp), gaugeDispSourcePath,
                       95);
    _InitDisp(disp);
    return disp;
}

void DisposeRgGaugeDisp(RgGaugeDisp *disp)
{
    if (disp == 0)
        assert_prog(gaugeDispAssertDispNotNil, gaugeDispSourcePath, 103);
    _DestructDisp(disp);
    RgHeapFree(InstanceOfRgHeap(), disp, gaugeDispSourcePath, 105);
}

int RgGaugeDispIsDrawable(RgGaugeDisp *disp)
{
    if (disp == 0)
        assert_prog(gaugeDispAssertDispNotNil, gaugeDispSourcePath, 115);
    return disp->validState < RG_GAUGE_DISP_DRAW_NOTHING;
}

void RgGaugeDispSetValue(RgGaugeDisp *disp, float currentValue,
                         float maxValue)
{
    if (disp == 0)
        assert_prog(gaugeDispAssertDispNotNil, gaugeDispSourcePath, 131);
    disp->currentValue = currentValue;
    disp->maxValue = maxValue;
    _CheckDatas(disp);
}

void RgGaugeDispSetTex(RgGaugeDisp *disp, RgBxxPic *barPic, RgBxxPic *restPic)
{
    if (disp == 0)
        assert_prog(gaugeDispAssertDispNotNil, gaugeDispSourcePath, 144);
    disp->barPic = barPic;
    disp->restPic = restPic;
    _CheckDatas(disp);
}

void RgGaugeDispSetBarUVWH(RgGaugeDisp *disp, int texU, int texV, int texW,
                           int texH)
{
    if (disp == 0)
        assert_prog(gaugeDispAssertDispNotNil, gaugeDispSourcePath, 157);
    XrgSetIVector(disp->barUVWH, texU, texV, texW, texH);
    _CheckDatas(disp);
}

void RgGaugeDispSetRestUVWH(RgGaugeDisp *disp, int texU, int texV, int texW,
                            int texH)
{
    if (disp == 0)
        assert_prog(gaugeDispAssertDispNotNil, gaugeDispSourcePath, 169);
    XrgSetIVector(disp->restUVWH, texU, texV, texW, texH);
    _CheckDatas(disp);
}

void RgGaugeDispSetPos(RgGaugeDisp *disp, int x, int y)
{
    if (disp == 0)
        assert_prog(gaugeDispAssertDispNotNil, gaugeDispSourcePath, 180);
    disp->posX = x;
    disp->posY = y;
    _CheckDatas(disp);
}

void RgGaugeDispSetOrder(RgGaugeDisp *disp, int order)
{
    if (disp == 0)
        assert_prog(gaugeDispAssertDispNotNil, gaugeDispSourcePath, 193);
    disp->order = order;
    _CheckDatas(disp);
}

void RgGaugeDispSetAlpha(RgGaugeDisp *disp, int alpha)
{
    if (disp == 0)
        assert_prog(gaugeDispAssertDispNotNil, gaugeDispSourcePath, 204);
    disp->alpha = alpha;
    _CheckDatas(disp);
}

/*
 * Draws the filled part of the bar and stores the unfilled strip in rest.
 * With order set, the filled part is drawn from the right end.
 */
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

/* Draws the unfilled strip rest with the rest texture. */
static void _disp_bar_rest(XrgPaint2D *paint, RgGaugeDisp *disp,
                           const RgGaugeDispRect *rest)
{
    XrgPaint2DUseTexture(paint, disp->restPic);
    XrgPaint2DSetUVOffset(paint, disp->restUVWH[0], disp->restUVWH[1]);
    XrgPaint2DSetUVSize(paint, disp->restUVWH[2], disp->restUVWH[3]);
    XrgPaint2DAlpha(paint, disp->alpha);
    XrgPaint2DDrawXYWH(paint, 0, rest->x, rest->y, rest->width, rest->height);
}

/* Draws the bar, plus the rest strip when a rest texture is set. */
void RgGaugeDispDraw(RgGaugeDisp *disp, XrgPaint2D *paint)
{
    RgGaugeDispRect rest;

    if (paint == 0)
        assert_prog(gaugeDispAssertPaintNotNil, gaugeDispSourcePath, 268);
    if (disp == 0)
        assert_prog(gaugeDispAssertDispNotNil, gaugeDispSourcePath, 269);
    if (disp->validState != RG_GAUGE_DISP_DRAW_BAR_ONLY) {
        if (disp->validState != RG_GAUGE_DISP_DRAW_BAR_AND_REST)
            return;
        _disp_bar(paint, disp, &rest);
        _disp_bar_rest(paint, disp, &rest);
    } else {
        _disp_bar(paint, disp, &rest);
    }
}
