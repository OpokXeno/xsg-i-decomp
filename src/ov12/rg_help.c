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

/*
 * s_anMoveTbl_2 (ov12:0x00a50760, size 0x100) is a 16-entry table indexed by
 * the current cursor position; the member for whichever Xrg pad direction
 * the player just pressed gives the cursor value that press moves to.
 */
typedef struct RgHelpMoveEntry {
    int up;
    int down;
    int left;
    int right;
} RgHelpMoveEntry;

extern const RgHelpMoveEntry s_anMoveTbl_2[16];
extern int XrgPadIsUp(void);
extern int XrgPadIsDown(void);
extern int XrgPadIsLeft(void);
extern int XrgPadIsRight(void);
extern int XrgPadIsMaru(void);
extern void XrgSoundSystemCursor(void);

static void _control_mode_left(RgHelp *pHelp)
{
    int cursor;
    int next;

    next = -1;
    cursor = pHelp->cursor;
    XrgPadSetID(0);
    if (XrgPadIsUp()) {
        next = s_anMoveTbl_2[cursor].up;
    }
    if (XrgPadIsDown()) {
        next = s_anMoveTbl_2[cursor].down;
    }
    if (XrgPadIsLeft()) {
        next = s_anMoveTbl_2[cursor].left;
    }
    if (XrgPadIsRight() || XrgPadIsMaru()) {
        next = s_anMoveTbl_2[cursor].right;
    }
    if (next >= 0) {
        if ((unsigned int) next >= 0x10) {
            next = 0;
        }
        if (pHelp->cursor != next) {
            XrgSoundSystemCursor();
        }
        pHelp->cursor = next;
    }
    if (pHelp->blinkTimer > 2.0f) {
        pHelp->blinkTimer = 0.0f;
    }
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_help", _color);

/*
 * _paint_lin/_paint_add/_paint_sub each hand XrgPaint2DAlpha one of exactly
 * these three values: 0 pastes the source over the destination, 1 adds it,
 * 2 subtracts it.
 */
#define XRG_PAINT2D_BLEND_LINEAR 0
#define XRG_PAINT2D_BLEND_ADD 1
#define XRG_PAINT2D_BLEND_SUB 2

/*
 * XrgPaint2DDrawXYWH's mode word: bit 0x20 takes the drawn extent from the
 * bound picture instead of the width/height arguments (_paint).
 */
#define XRG_PAINT2D_MODE_USE_PIC_SIZE 0x20

extern void XrgPaint2DUseTexture(void *paintContext, RgBxxPic *pic);
extern void XrgPaint2DAlpha(void *paintContext, int blendMode);
extern void XrgPaint2DDrawXYWH(void *paintContext, int mode, int x, int y,
                               int width, int height);
extern void _color(void *paintContext, int color);

static void _paint(void *paintContext, RgBxxPic *pic, int blendMode, int x, int y)
{
    if (pic != 0) {
        XrgPaint2DUseTexture(paintContext, pic);
        XrgPaint2DAlpha(paintContext, blendMode);
        _color(paintContext, 0);
        XrgPaint2DDrawXYWH(paintContext, XRG_PAINT2D_MODE_USE_PIC_SIZE, x, y, 0, 0);
    }
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_help", _paint_b);

static void _paint_lin(void *paintContext, RgBxxPic *pic, int x, int y)
{
    _paint(paintContext, pic, XRG_PAINT2D_BLEND_LINEAR, x, y);
}

/*
 * The rectangle _paint_lin_uvwh's caller builds: u,v feed XrgPaint2DSetUVOffset
 * and, added to x,y, the screen position this call draws at; w,h feed both
 * XrgPaint2DSetUVSize and the draw call's own width/height.
 */
typedef struct RgHelpUvRect {
    int u;
    int v;
    int w;
    int h;
} RgHelpUvRect;

extern void XrgPaint2DSetUVOffset(void *paintContext, int u, int v);
extern void XrgPaint2DSetUVSize(void *paintContext, int width, int height);

static void _paint_lin_uvwh(void *paintContext, RgBxxPic *pic, int x, int y,
                            const RgHelpUvRect *rect)
{
    XrgPaint2DSetUVOffset(paintContext, rect->u, rect->v);
    XrgPaint2DSetUVSize(paintContext, rect->w, rect->h);
    if (pic != 0) {
        XrgPaint2DUseTexture(paintContext, pic);
        XrgPaint2DAlpha(paintContext, XRG_PAINT2D_BLEND_LINEAR);
        _color(paintContext, 0);
        XrgPaint2DDrawXYWH(paintContext, 0, x + rect->u, y + rect->v, rect->w,
                           rect->h);
    }
}

static void _paint_add(void *paintContext, RgBxxPic *pic, int x, int y)
{
    _paint(paintContext, pic, XRG_PAINT2D_BLEND_ADD, x, y);
}

static void _paint_sub(void *paintContext, RgBxxPic *pic, int x, int y)
{
    _paint(paintContext, pic, XRG_PAINT2D_BLEND_SUB, x, y);
}

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
