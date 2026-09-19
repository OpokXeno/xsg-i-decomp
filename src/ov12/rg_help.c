/*
 * OV12 original TU 76: 0x00a420e0..0x00a434b8 (21 functions)
 */
#include "common.h"
#include "shared.h"
#include "rg_help.h"

extern void assert_prog(const char *expression, const char *source_file,
                        int line);
extern RgHeap *InstanceOfRgHeap(void);
extern void *RgHeapAlloc(void *heap, unsigned int size,
                         const char *source_file, int line);
extern void RgHeapFree(void *heap, void *pointer, const char *source_file,
                       int line);
static void _InitHelp(RgHelp *pHelp);
static void _DestructHelp(RgHelp *pHelp);

extern const char D_00A57AC8[]; /* "pHelp != NIL" */
extern const char D_00A57AD8[]; /* "../rg_help.euc.c" */

INCLUDE_ASM("asm/nonmatchings/ov12/rg_help", _set_global_blight);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_help", _InitHelp);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_help", _DestructHelp);

RgHelp *CreateRgHelp(void)
{
    RgHelp *pHelp;

    pHelp = RgHeapAlloc(InstanceOfRgHeap(), sizeof(RgHelp), D_00A57AD8, 172);
    _InitHelp(pHelp);
    return pHelp;
}

void DisposeRgHelp(RgHelp *pHelp)
{
    if (pHelp == 0) {
        assert_prog(D_00A57AC8, D_00A57AD8, 179);
    }
    _DestructHelp(pHelp);
    RgHeapFree(InstanceOfRgHeap(), pHelp, D_00A57AD8, 181);
}

int RgHelpIsEnd(RgHelp *pHelp)
{
    if (pHelp == 0) {
        assert_prog(D_00A57AC8, D_00A57AD8, 191);
    }
    return pHelp->ended;
}

static void _set_global_blight(float blight);
static void _control_mode_left(RgHelp *pHelp);
extern int XrgPadIsBatu(void);
extern int XrgPadIsStart(void);
extern void XrgPadSetID(int id);
extern void XrgSoundSystemCancel(void);

void RgHelpPassTime(RgHelp *pHelp, float deltaTime)
{
    unsigned int phase;

    if (pHelp == 0) {
        assert_prog(D_00A57AC8, D_00A57AD8, 203);
    }

    phase = pHelp->phase;
    pHelp->blinkTimer += deltaTime;

    switch (phase) {
    case 2:
        pHelp->ended = 1;
        return;

    case 0:
        _set_global_blight(1.0f);
        _control_mode_left(pHelp);
        XrgPadSetID(0);
        if (XrgPadIsBatu() || XrgPadIsStart()) {
            pHelp->phase = 1;
            pHelp->countdown = 0.5f;
            XrgSoundSystemCancel();
        }
        return;

    case 1:
        pHelp->countdown -= deltaTime;
        _set_global_blight(2.0f * pHelp->countdown);
        if (pHelp->countdown <= 0.0f) {
            pHelp->phase = 2;
        }
        break;
    }
}

static void _paint_mode_left(RgHelp *pHelp);
extern void XrgPaint2DFlush(void *paintContext);
static void _paint_bg_lower(RgHelp *pHelp);
static void _paint_bg_higher(RgHelp *pHelp);

void RgHelpDisp(RgHelp *pHelp)
{
    if (pHelp == 0) {
        assert_prog(D_00A57AC8, D_00A57AD8, 257);
    }

    if (pHelp->phase != 1) {
        if (pHelp->phase == 0) {
            _paint_bg_lower(pHelp);
            _paint_mode_left(pHelp);
            _paint_bg_higher(pHelp);
        }
    } else {
        _paint_bg_lower(pHelp);
        _paint_mode_left(pHelp);
        _paint_bg_higher(pHelp);
    }

    XrgPaint2DFlush(pHelp->paintContext);
}

static void _init_mode_left(RgHelp *pHelp)
{
    pHelp->phase = 0;
    pHelp->cursor = 0;
    pHelp->blinkTimer = 0.0f;
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_help", _control_mode_left);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_help", _color);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_help", _paint_00A428A8);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_help", _paint_b);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_help", _paint_lin_00A42A40);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_help", _paint_lin_uvwh);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_help", _paint_add_00A42B40);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_help", _paint_sub_00A42B68);

static void _paint(void *paintContext, RgBxxPic *pic, int blendMode, int x,
                   int y);

static void _paint_bg_lower(RgHelp *pHelp)
{
    void *paintContext;
    RgBxxPic *pic;

    paintContext = pHelp->paintContext;
    _paint(paintContext, pHelp->pics[60], 4, 0, 0x10);
    pic = pHelp->pics[61];
    _paint(paintContext, pic, 0, 0, 0x1D0 - pic->height);
}

static void _paint_lin(void *paintContext, RgBxxPic *pic, int x, int y);

static void _paint_bg_higher(RgHelp *pHelp)
{
    _paint_lin(pHelp->paintContext, pHelp->pics[46], 0, 0x168);
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_help", _paint_mode_left);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_help", _paint_mode_right);
