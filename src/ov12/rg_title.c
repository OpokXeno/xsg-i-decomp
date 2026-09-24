/*
 * OV12 original TU 74: 0x00a3f230..0x00a40008 (12 functions)
 */
#include "common.h"
#include "shared.h"

extern void assert_prog(const char *expression, const char *source_file,
                        int line);
extern RgHeap *InstanceOfRgHeap(void);
extern void *RgHeapAlloc(void *heap, unsigned int size,
                         const char *source_file, int line);
extern void RgHeapFree(void *heap, void *pointer, const char *source_file,
                       int line);
typedef struct XrgPaint2D XrgPaint2D;

extern void XrgPaint2DUseTexture(XrgPaint2D *paint, void *pic);
extern void XrgPaint2DAlpha(XrgPaint2D *paint, int blendMode);
extern void XrgPaint2DDrawXYWH(XrgPaint2D *paint, int mode, int x, int y,
                               int width, int height);

#define XRG_PAINT2D_MODE_USE_PIC_SIZE 0x20
#define XRG_PAINT2D_BLEND_LINEAR 0
#define XRG_PAINT2D_BLEND_ADD 1

extern const char D_00A57118[]; /* "pPaint != NIL" */
extern const char D_00A57128[]; /* "../rg_title.euc.c" */
extern const char D_00A57140[]; /* "pPic != NIL" */
extern const char D_00A57150[]; /* "pTitle != NIL" */

/*
 * The blend color XrgPaint2DColor copies into a paint object with one
 * lqc2/sqc2 quadword (ov12:0x00a4bd7c/0x00a4bd80).  s_aCol's own bytes are
 * the words 0x80, 0x80, 0x80, 0x7F: full-range red/green/blue in the GS's
 * 0-0x80 fixed scale with alpha one unit short of it, the same per-component
 * integer layout RgPicColor documents for RgPicSetColor
 * (src/ov12/rg_piclist.h).
 */
typedef struct XrgColor {
    int r;
    int g;
    int b;
    int a;
} XrgColor;

extern void XrgPaint2DColor(XrgPaint2D *paint, const XrgColor *color);
extern XrgColor s_aCol;

#include "rg_title.h"

typedef struct RgTitle RgTitle;

static void _InitTitle(RgTitle *pTitle, int count);
static void _DestructTitle(RgTitle *pTitle);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_title", _disp_blight);

static void _disp_normal(XrgPaint2D *pPaint, void *pPic, int x, int y)
{
    if (pPaint == 0) {
        assert_prog(D_00A57118, D_00A57128, 101);
    }
    if (pPic == 0) {
        assert_prog(D_00A57140, D_00A57128, 102);
    }
    XrgPaint2DUseTexture(pPaint, pPic);
    XrgPaint2DColor(pPaint, &s_aCol);
    XrgPaint2DDrawXYWH(pPaint, XRG_PAINT2D_MODE_USE_PIC_SIZE, x, y, 0, 0);
}

static void _disp_linear(XrgPaint2D *pPaint, void *pPic, int x, int y)
{
    if (pPaint == 0) {
        assert_prog(D_00A57118, D_00A57128, 110);
    }
    if (pPic == 0) {
        assert_prog(D_00A57140, D_00A57128, 111);
    }
    XrgPaint2DUseTexture(pPaint, pPic);
    XrgPaint2DColor(pPaint, &s_aCol);
    XrgPaint2DAlpha(pPaint, XRG_PAINT2D_BLEND_LINEAR);
    XrgPaint2DDrawXYWH(pPaint, XRG_PAINT2D_MODE_USE_PIC_SIZE, x, y, 0, 0);
}

static void _disp_add(XrgPaint2D *pPaint, void *pPic, int x, int y)
{
    if (pPaint == 0) {
        assert_prog(D_00A57118, D_00A57128, 119);
    }
    if (pPic == 0) {
        assert_prog(D_00A57140, D_00A57128, 120);
    }
    XrgPaint2DUseTexture(pPaint, pPic);
    XrgPaint2DColor(pPaint, &s_aCol);
    XrgPaint2DAlpha(pPaint, XRG_PAINT2D_BLEND_ADD);
    XrgPaint2DDrawXYWH(pPaint, XRG_PAINT2D_MODE_USE_PIC_SIZE, x, y, 0, 0);
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_title", _disp_add_blight);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_title", _InitTitle);

extern void DisposeXrgPaint2D_sub(XrgPaint2D *paint, const char *source_file,
                                  int line);
extern void DisposeRgBxx_sub(RgBxx *pBxx, const char *pszFile, int nLine);

static void _DestructTitle(RgTitle *pTitle)
{
    if (pTitle == 0) {
        assert_prog(D_00A57150, D_00A57128, 218);
    }
    DisposeXrgPaint2D_sub(pTitle->paint, D_00A57128, 219);
    DisposeRgBxx_sub(pTitle->bxx[0], D_00A57128, 221);
    DisposeRgBxx_sub(pTitle->bxx[1], D_00A57128, 222);
    DisposeRgBxx_sub(pTitle->bxx[2], D_00A57128, 223);
    DisposeRgBxx_sub(pTitle->bxx[3], D_00A57128, 224);
    DisposeRgBxx_sub(pTitle->bxx[4], D_00A57128, 225);
    DisposeRgBxx_sub(pTitle->bxx[5], D_00A57128, 226);
    DisposeRgBxx_sub(pTitle->bxx[6], D_00A57128, 227);
    DisposeRgBxx_sub(pTitle->bxx[7], D_00A57128, 228);
    DisposeRgBxx_sub(pTitle->bxx[8], D_00A57128, 229);
}

RgTitle *CreateRgTitle(int count)
{
    RgTitle *pTitle;

    pTitle = RgHeapAlloc(InstanceOfRgHeap(), 0x70, D_00A57128, 236);
    _InitTitle(pTitle, count);
    return pTitle;
}

void DisposeRgTitle(RgTitle *pTitle)
{
    if (pTitle == 0) {
        assert_prog(D_00A57150, D_00A57128, 244);
    }
    _DestructTitle(pTitle);
    RgHeapFree(InstanceOfRgHeap(), pTitle, D_00A57128, 246);
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_title", RgTitleGetResult);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_title", RgTitlePassTime);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_title", RgTitleDisp);
