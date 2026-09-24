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

static void _InitDisp(RgDispLife *pDisp, int dispTex, int timeFont);
static void _DestructDisp(RgDispLife *pDisp);

/* ov12:0x00a54820 "../rg_disp_life.euc.c" (source filename, scaffold-owned
 * per config/tu-build.json data_ownership: this .rodata window is still
 * owner "asm"). */
extern const char D_00A54820[];
/* ov12:0x00a54838 "pDisp != NIL" */
extern const char D_00A54838[];

INCLUDE_ASM("asm/nonmatchings/ov12/rg_disp_life", _GetAgwsNameUVWH);

/*
 * The AGWS name texture's UV rectangle _GetAgwsNameUVWH (still INCLUDE_ASM)
 * fills: u, v feed XrgPaint2DSetUVOffset and w, h feed both
 * XrgPaint2DSetUVSize and the draw call's own width/height.
 */
typedef struct AgwsNameUvwh {
    int u;
    int v;
    int w;
    int h;
} AgwsNameUvwh;

static int _GetAgwsNameUVWH(int nameIndex, AgwsNameUvwh *uvwh);

extern void XrgPaint2DUseTexture(void *paint, void *pic);
extern void XrgPaint2DSetUVOffset(void *paint, int u, int v);
extern void XrgPaint2DSetUVSize(void *paint, int width, int height);
extern void XrgPaint2DAlpha(void *paint, int blendMode);
extern void XrgPaint2DDrawXYWH(void *paint, int mode, int x, int y,
                               int width, int height);

/* ov12:0x00a54810 "pPic != NIL" */
extern const char D_00A54810[];

static void _DispAgwsName(void *paint, void *pPic, int nameIndex, int x,
                          int y)
{
    AgwsNameUvwh uvwh;

    if (pPic == 0) {
        assert_prog(D_00A54810, D_00A54820, 100);
    }
    if (_GetAgwsNameUVWH(nameIndex, &uvwh) && (pPic != 0)) {
        XrgPaint2DUseTexture(paint, pPic);
        XrgPaint2DSetUVOffset(paint, uvwh.u, uvwh.v);
        XrgPaint2DSetUVSize(paint, uvwh.w, uvwh.h);
        XrgPaint2DAlpha(paint, 0);
        XrgPaint2DDrawXYWH(paint, 0, x, y, uvwh.w, uvwh.h);
    }
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_disp_life", _InitDisp_00A25E58);

extern void DisposeRgGauge(RgGauge *gauge);
extern void DisposeXrgPaint2D_sub(void *paint, const char *source_file,
                                  int line);

static void _DestructDisp(RgDispLife *pDisp)
{
    unsigned int i;

    if (pDisp == 0) {
        assert_prog(D_00A54838, D_00A54820, 188);
    }
    for (i = 0; i < 2; i++) {
        DisposeRgGauge(pDisp->gauge[i]);
    }
    DisposeXrgPaint2D_sub(pDisp->paint, D_00A54820, 192);
}

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
