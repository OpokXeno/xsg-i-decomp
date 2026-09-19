/*
 * OV12 original TU 45: 0x00a26c78..0x00a285e0 (45 functions)
 */
#include "common.h"
#include "shared.h"
#include "rg_draw.h"

extern void assert_prog(const char *expression, const char *source_file,
                        int line);
extern RgHeap *InstanceOfRgHeap(void);
extern void *RgHeapAlloc(void *heap, unsigned int size,
                         const char *source_file, int line);
extern void RgHeapFree(void *heap, void *pointer, const char *source_file,
                       int line);
extern void sceVif1PkCloseGifTag(XglPacket *packet);
extern void sceVif1PkCloseDirectHLCode(XglPacket *packet);
extern void _InitRgDrawView(RgDrawView *pView, RgDrawStudio *pParentStudio);
extern void _InitRgDrawStudio(RgDrawStudio *pStudio, int screenIndex,
                              RgFog *pFog);
extern void _DestructDraw(RgDraw *pDraw);
extern void RgError(const char *message, const char *source_file, int line,
                    ...);
static void _CopyFog(RgFog *pDst, RgFog *pSrc);
static void _SetGlobalLight(void);
static void _ClearStudioList(RgDraw *pDraw);
static void _FullScreenStudio(RgDraw *pDraw);
static void _DoubleScreenStudio(RgDraw *pDraw);
static void _DrawMain(RgDraw *pDraw);
static void _DrawReqTerminate(RgDraw *pDraw);

/*
 * External file-backed witnesses, not candidate-emitted data: this window is
 * asm-owned scaffold data (splat names, no config/symbols/ov12.txt entry).
 *
 * ov12:0x00a54af0 contains the source filename "../rg_draw.euc.c".
 * ov12:0x00a54b08 contains the assertion expression "pView != NIL".
 * ov12:0x00a54b18 contains the assertion expression "pParentStudio != NIL".
 * ov12:0x00a54b30 contains the assertion expression "pStudio != NIL".
 * ov12:0x00a54b40 contains the assertion expression "pStudio->m_pView != NIL".
 */
extern const char D_00A54AF0[];
extern const char D_00A54B08[];
extern const char D_00A54B18[];
extern const char D_00A54B30[];
extern const char D_00A54B40[];

/*
 * External file-backed witnesses of this allocation's own functions, not
 * candidate-emitted data (same window as the block above).
 *
 * ov12:0x00a54b58 contains the assertion expression "pDraw != NIL".
 * ov12:0x00a54b68 contains the error message "unknown studio id %d".
 * ov12:0x00a54b80 contains the assertion expression "pFog != NIL".
 */
extern const char D_00A54B58[];
extern const char D_00A54B68[];
extern const char D_00A54B80[];

/*
 * GNU EE native TI storage/copy type (docs/native-ti.md), used only to pass
 * the 16-byte GIF tag _openVifGif and _openVifGifAD build to
 * sceVif1PkOpenGifTag in the single 128-bit register the original loads
 * with one `lq`; no wide arithmetic is done on it.
 */
typedef unsigned int Quadword __attribute__((mode(TI)));

/*
 * The 128-bit GIF tag _openVifGif and _openVifGifAD build for
 * sceVif1PkOpenGifTag: bit 15 is EOP, bit 46 is PRE, bits 47..57 are PRIM,
 * bits 60..63 are NREG, and the high 64 bits are the REGS descriptor. The
 * union lets the two halves be stored as ordinary 64-bit fields and the
 * whole 16 bytes be read back as the single register sceVif1PkOpenGifTag
 * takes.
 */
typedef union GifTag {
    struct {
        u64 lo;
        u64 hi;
    } part;
    Quadword quad;
} GifTag;

extern void sceVif1PkCnt(XglPacket *packet, int count);
extern void sceVif1PkAlign(XglPacket *packet, int align, int size);
extern void sceVif1PkOpenDirectCode(XglPacket *packet, int mode);
extern void sceVif1PkOpenDirectHLCode(XglPacket *packet, int mode);
extern void sceVif1PkOpenGifTag(XglPacket *packet, Quadword tag);

/*
 * ov12:0x00a54ac0 is a fixed GIF tag: EOP set, NREG 1, REGS 0xE (the A+D
 * address+data register code), the constant _openVifGifAD passes to
 * sceVif1PkOpenGifTag.
 */
extern const GifTag D_00A54AC0;

static void _openVifGif(XglPacket *packet, unsigned int prim,
                        unsigned int nreg, u64 regs)
{
    GifTag tag;

    tag.part.lo = (1u << 15) | (1ULL << 46) | ((u64)prim << 47) |
                  ((u64)nreg << 60);
    tag.part.hi = regs;
    sceVif1PkAlign(packet, 2, 3);
    sceVif1PkCnt(packet, 0);
    sceVif1PkOpenDirectHLCode(packet, 0);
    sceVif1PkOpenGifTag(packet, tag.quad);
}

static void _openVifGifAD(XglPacket *packet)
{
    GifTag tag;

    tag = D_00A54AC0;
    sceVif1PkCnt(packet, 0);
    sceVif1PkAlign(packet, 2, 3);
    sceVif1PkOpenDirectCode(packet, 0);
    sceVif1PkOpenGifTag(packet, tag.quad);
}

static void _closeVifGif(XglPacket *pPacket)
{
    sceVif1PkCloseGifTag(pPacket);
    sceVif1PkCloseDirectHLCode(pPacket);
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_draw", _SetGlobalLight);

static void _DefaultFog(RgFog *pFog)
{
    pFog->dist[0] = 50.0f;
    pFog->dist[1] = 250.0f;
    pFog->dist[2] = 0.0f;
    pFog->dist[3] = 1.0f;
    pFog->color[0] = 0.0f;
    pFog->color[1] = 0.0f;
    pFog->color[2] = 0.0f;
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_draw", _CopyFog);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_draw", _InitRgDrawView);

static void _DestructRgDrawView(RgDrawView *pView)
{
    if (pView == 0) {
        assert_prog(D_00A54B08, D_00A54AF0, 191);
    }
}

static RgDrawView *_CreateRgDrawView(RgDrawStudio *pParentStudio)
{
    RgDrawView *pView;

    if (pParentStudio == 0) {
        assert_prog(D_00A54B18, D_00A54AF0, 197);
    }
    pView = RgHeapAlloc(InstanceOfRgHeap(), 128, D_00A54AF0, 198);
    _InitRgDrawView(pView, pParentStudio);
    return pView;
}

static void _DisposeRgDrawView(RgDrawView *pView)
{
    if (pView == 0) {
        assert_prog(D_00A54B08, D_00A54AF0, 205);
    }
    _DestructRgDrawView(pView);
    RgHeapFree(InstanceOfRgHeap(), pView, D_00A54AF0, 207);
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_draw", _InitRgDrawStudio);

static void _DestructRgDrawStudio(RgDrawStudio *pStudio)
{
    if (pStudio == 0) {
        assert_prog(D_00A54B30, D_00A54AF0, 231);
    }
    if (pStudio->m_pView != 0) {
        _DisposeRgDrawView(pStudio->m_pView);
    }
}

static RgDrawStudio *_CreateRgDrawStudio(int screenIndex, RgFog *pFog)
{
    RgDrawStudio *pStudio;

    pStudio = RgHeapAlloc(InstanceOfRgHeap(), 12, D_00A54AF0, 240);
    _InitRgDrawStudio(pStudio, screenIndex, pFog);
    return pStudio;
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_draw", _DisposeRgDrawStudio);

static RgDrawView *_GetViewRgDrawStudio(RgDrawStudio *pStudio)
{
    if (pStudio == 0) {
        assert_prog(D_00A54B30, D_00A54AF0, 254);
    }
    if (pStudio->m_pView == 0) {
        assert_prog(D_00A54B40, D_00A54AF0, 255);
    }
    return pStudio->m_pView;
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_draw", _InitStudioList);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_draw", _ClearStudioList);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_draw", _FullScreenStudio);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_draw", _GetSplitRect);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_draw", _SetScreenSplitMode);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_draw", _DoubleScreenStudio);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_draw", _InitDraw);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_draw", _DestructDraw);

static void _WrapperDestruct(RgDraw *pDraw)
{
    _DestructDraw(pDraw);
    RgHeapFree(InstanceOfRgHeap(), pDraw, D_00A54AF0, 421);
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_draw", InstanceOfRgDraw);

RgDrawStudio *RgDrawGetStudio(RgDraw *pDraw, int screenIndex)
{
    if (pDraw == 0) {
        assert_prog(D_00A54B58, D_00A54AF0, 447);
    }
    switch (screenIndex) {
    case -1:
        return 0;
    case 0:
    case 1:
        return pDraw->m_pStudios[screenIndex];
    default:
        RgError(D_00A54B68, D_00A54AF0, 458, screenIndex);
        return 0;
    }
}

void RgDrawCreateDrawStudioFullScreen(RgDraw *pDraw)
{
    if (pDraw == 0) {
        assert_prog(D_00A54B58, D_00A54AF0, 471);
    }
    _ClearStudioList(pDraw);
    _FullScreenStudio(pDraw);
}

void RgDrawCreateDrawStudioDouble(RgDraw *pDraw)
{
    if (pDraw == 0) {
        assert_prog(D_00A54B58, D_00A54AF0, 480);
    }
    _ClearStudioList(pDraw);
    _DoubleScreenStudio(pDraw);
}

void RgDrawGetGlobalFog(RgDraw *pDraw, RgFog *pFog)
{
    if (pDraw == 0) {
        assert_prog(D_00A54B58, D_00A54AF0, 492);
    }
    if (pFog == 0) {
        assert_prog(D_00A54B80, D_00A54AF0, 493);
    }
    _CopyFog(pFog, &pDraw->m_Fog);
}

void RgDrawSetGlobalFog(RgDraw *pDraw, RgFog *pFog)
{
    if (pDraw == 0) {
        assert_prog(D_00A54B58, D_00A54AF0, 502);
    }
    if (pFog == 0) {
        assert_prog(D_00A54B80, D_00A54AF0, 503);
    }
    _CopyFog(&pDraw->m_Fog, pFog);
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_draw", RgDrawViewInit);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_draw", RgDrawViewSetPosition);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_draw", RgDrawViewSetRotateX);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_draw", RgDrawViewSetRotateY);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_draw", RgDrawViewSetRotateZ);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_draw", RgDrawViewIsPointInView);

void RgDrawViewGetScreenRect(RgDrawView *pView, RgRect *pRect)
{
    if (pView == 0) {
        assert_prog(D_00A54B08, D_00A54AF0, 632);
    }
    pRect->x0 = pView->screenRect.x0;
    pRect->y0 = pView->screenRect.y0;
    pRect->x1 = pView->screenRect.x1;
    pRect->y1 = pView->screenRect.y1;
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_draw", RgDrawStudioGetView);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_draw", RgDrawReq);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_draw", _FadeShadow);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_draw", _DrawReqTerminate);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_draw", _DrawMain);

void RgDrawJob(RgDraw *pDraw)
{
    if (pDraw == 0) {
        assert_prog(D_00A54B58, D_00A54AF0, 834);
    }
    _SetGlobalLight();
    _DrawMain(pDraw);
    _DrawReqTerminate(pDraw);
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_draw", RgDrawFadeIn);

void RgDrawFadeOut(RgDraw *pDraw)
{
    if (pDraw == 0) {
        assert_prog(D_00A54B58, D_00A54AF0, 876);
    }
}
