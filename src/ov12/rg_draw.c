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
static void _InitRgDrawView(RgDrawView *pView, RgDrawStudio *pParentStudio);
static void _InitRgDrawStudio(RgDrawStudio *pStudio, int screenIndex,
                              RgFog *pFog);
static void _DestructDraw(RgDraw *pDraw);
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

static void _CopyFog(RgFog *pDst, RgFog *pSrc)
{
    pDst->dist[0] = pSrc->dist[0];
    pDst->dist[1] = pSrc->dist[1];
    pDst->dist[2] = pSrc->dist[2];
    pDst->dist[3] = pSrc->dist[3];
    pDst->color[0] = pSrc->color[0];
    pDst->color[1] = pSrc->color[1];
    pDst->color[2] = pSrc->color[2];
}

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

static void _InitRgDrawStudio(RgDrawStudio *pStudio, int screenIndex,
                              RgFog *pFog)
{
    if (pStudio == 0) {
        assert_prog(D_00A54B30, D_00A54AF0, 215);
    }
    pStudio->m_pFog = pFog;
    pStudio->m_ScreenIndex = screenIndex;
    if (screenIndex != -1) {
        pStudio->m_pView = _CreateRgDrawView(pStudio);
        return;
    }
    pStudio->m_pView = 0;
}

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

static void _DisposeRgDrawStudio(RgDrawStudio *pStudio)
{
    if (pStudio == 0) {
        assert_prog(D_00A54B30, D_00A54AF0, 247);
    }
    _DestructRgDrawStudio(pStudio);
    RgHeapFree(InstanceOfRgHeap(), pStudio, D_00A54AF0, 249);
}

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

static void _InitStudioList(RgDraw *pDraw)
{
    unsigned int slotIndex;

    if (pDraw == 0) {
        assert_prog(D_00A54B58, D_00A54AF0, 266);
    }
    slotIndex = 0;
    do {
        pDraw->m_pStudios[slotIndex] = 0;
        slotIndex += 1;
    } while (slotIndex < 2U);
    pDraw->m_ActiveStudioMask = 0;
}

static void _ClearStudioList(RgDraw *pDraw)
{
    RgDrawStudio **pSlot;
    RgDrawStudio *pStudio;
    unsigned int slotIndex;

    if (pDraw == 0) {
        assert_prog(D_00A54B58, D_00A54AF0, 276);
    }
    slotIndex = 0;
    pSlot = pDraw->m_pStudios;
    do {
        pStudio = *pSlot;
        slotIndex += 1;
        if (pStudio != 0) {
            _DisposeRgDrawStudio(pStudio);
            *pSlot = 0;
        }
        pSlot += 1;
    } while (slotIndex < 2U);
    pDraw->m_ActiveStudioMask = 0;
}

extern StudioCamera *xglStudioSelectGetActiveCamera(int studio_index);
extern void xglCameraSetWindow(StudioCamera *camera, int x0, int y0, int x1,
                               int y1);
extern void nmlModelUseSubWindow(int window_index, int mode);

/*
 * ov12:0x00a50080 is a fixed camera window rectangle (config/symbols/ov12.txt
 * s_aView_0, size 0x10), the constant _FullScreenStudio passes to
 * xglCameraSetWindow for the single full-screen studio.
 */
extern const RgRect s_aView_0;

static void _FullScreenStudio(RgDraw *pDraw)
{
    RgDrawStudio *pStudio;

    if (pDraw == 0) {
        assert_prog(D_00A54B58, D_00A54AF0, 291);
    }
    _ClearStudioList(pDraw);
    pDraw->m_FullScreenMode = 1;
    pDraw->m_FullScreenParam = 2;
    pStudio = _CreateRgDrawStudio(0, &pDraw->m_Fog);
    pDraw->m_pStudios[1] = 0;
    pDraw->m_ActiveStudioMask = 1;
    pDraw->m_pStudios[0] = pStudio;
    xglCameraSetWindow(xglStudioSelectGetActiveCamera(0), s_aView_0.x0,
                       s_aView_0.y0, s_aView_0.x1, s_aView_0.y1);
    nmlModelUseSubWindow(0, 2);
    nmlModelUseSubWindow(1, 0);
    nmlModelUseSubWindow(2, 0);
    nmlModelUseSubWindow(3, 0);
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_draw", _GetSplitRect);

static int _GetSplitRect(int splitMode, int screenIndex, RgRect *pRect);

static void _SetScreenSplitMode(RgDraw *pDraw, int splitMode)
{
    RgRect rect;
    RgRect *pScreenRect0;
    RgRect *pScreenRect1;

    if (pDraw == 0) {
        assert_prog(D_00A54B58, D_00A54AF0, 342);
    }
    if (pDraw->m_FullScreenMode == 0) {
        if (_GetSplitRect(splitMode, 0, &rect) != 0) {
            pScreenRect0 = &pDraw->m_pStudios[0]->m_pView->screenRect;
            __asm__ __volatile__("lqc2 vf31, 0(%0)" : : "r"(&rect) : "memory");
            __asm__ __volatile__("sqc2 vf31, 0(%0)" : : "r"(pScreenRect0) : "memory");
            xglCameraSetWindow(xglStudioSelectGetActiveCamera(0), rect.x0,
                               rect.y0, rect.x1, rect.y1);
        }
        if (_GetSplitRect(splitMode, 1, &rect) != 0) {
            pScreenRect1 = &pDraw->m_pStudios[1]->m_pView->screenRect;
            __asm__ __volatile__("lqc2 vf31, 0(%0)" : : "r"(&rect) : "memory");
            __asm__ __volatile__("sqc2 vf31, 0(%0)" : : "r"(pScreenRect1) : "memory");
            xglCameraSetWindow(xglStudioSelectGetActiveCamera(1), rect.x0,
                               rect.y0, rect.x1, rect.y1);
        }
    }
}

static void _DoubleScreenStudio(RgDraw *pDraw)
{
    RgFog *pFog;

    pFog = &pDraw->m_Fog;
    if (pDraw == 0) {
        assert_prog(D_00A54B58, D_00A54AF0, 375);
    }
    _ClearStudioList(pDraw);
    pDraw->m_FullScreenMode = 0;
    pDraw->m_FullScreenParam = 0;
    pDraw->m_pStudios[0] = _CreateRgDrawStudio(0, pFog);
    pDraw->m_pStudios[1] = _CreateRgDrawStudio(1, pFog);
    pDraw->m_ActiveStudioMask = 3;
    _SetScreenSplitMode(pDraw, 0);
    nmlModelUseSubWindow(0, 2);
    nmlModelUseSubWindow(1, 2);
    nmlModelUseSubWindow(2, 0);
    nmlModelUseSubWindow(3, 0);
}

static void _InitDraw(RgDraw *pDraw)
{
    RgFog *pFog;

    pFog = &pDraw->m_Fog;
    if (pDraw == 0) {
        assert_prog(D_00A54B58, D_00A54AF0, 403);
    }
    pDraw->m_FadeCounter = 0;
    pDraw->m_Enabled = 1;
    pDraw->m_FadeParam = 0;
    pDraw->m_RequestCount = 0;
    _DefaultFog(pFog);
    pDraw->m_pDefaultStudio = _CreateRgDrawStudio(-1, pFog);
    _InitStudioList(pDraw);
}

static void _DestructDraw(RgDraw *pDraw)
{
    if (pDraw == 0) {
        assert_prog(D_00A54B58, D_00A54AF0, 0x1A1);
    }
    _DisposeRgDrawStudio(pDraw->m_pDefaultStudio);
    _ClearStudioList(pDraw);
}

static void _WrapperDestruct(RgDraw *pDraw)
{
    _DestructDraw(pDraw);
    RgHeapFree(InstanceOfRgHeap(), pDraw, D_00A54AF0, 421);
}

void RgDrawCreateDrawStudioFullScreen(RgDraw *pDraw);

RgDraw *InstanceOfRgDraw(void)
{
    RgDraw *pDraw;

    pDraw = RgSingletonIDGet(9);
    if (pDraw == 0) {
        pDraw = RgHeapAlloc(InstanceOfRgHeap(), sizeof(RgDraw), D_00A54AF0, 431);
        if (pDraw == 0) {
            assert_prog(D_00A54B58, D_00A54AF0, 432);
        }
        _InitDraw(pDraw);
        RgSingletonIDEntry(9, (RgSimpleDB *)pDraw,
                           (void (*)(RgSimpleDB *))_WrapperDestruct);
        RgDrawCreateDrawStudioFullScreen(pDraw);
    }
    return pDraw;
}

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

/*
 * External file-backed witness of this allocation's own functions, not
 * candidate-emitted data (same window as the blocks above).
 *
 * ov12:0x00a54ad0 contains the error message "not exist xeno studio ID (%d)".
 */
extern const char D_00A54AD0[];

void RgDrawViewSetPosition(RgDrawView *pView, const Vector4 *pPosition)
{
    int screenIndex;
    int cameraIndex;
    StudioCamera *pCamera;

    screenIndex = pView->m_pParentStudio->m_ScreenIndex;
    pCamera = 0;
    if (screenIndex < 2) {
        if (screenIndex >= 0) {
            cameraIndex = 0;
            switch (screenIndex) {
            case 0:
                cameraIndex = 0;
                break;
            case 1:
                cameraIndex = 1;
                break;
            default:
                RgError(D_00A54AD0, D_00A54AF0, 69, screenIndex);
                break;
            }
            pCamera = xglStudioSelectGetActiveCamera(cameraIndex);
        }
    }
    if (pCamera != 0) {
        pCamera->position.x = pPosition->x;
        pCamera->position.y = pPosition->y;
        pCamera->position.z = pPosition->z;
    }
}

void RgDrawViewSetRotateX(RgDrawView *pView, float angle)
{
    int screenIndex;
    int cameraIndex;
    StudioCamera *pCamera;

    screenIndex = pView->m_pParentStudio->m_ScreenIndex;
    pCamera = 0;
    if (screenIndex < 2) {
        if (screenIndex >= 0) {
            cameraIndex = 0;
            switch (screenIndex) {
            case 0:
                cameraIndex = 0;
                break;
            case 1:
                cameraIndex = 1;
                break;
            default:
                RgError(D_00A54AD0, D_00A54AF0, 69, screenIndex);
                break;
            }
            pCamera = xglStudioSelectGetActiveCamera(cameraIndex);
        }
    }
    if (pCamera != 0) {
        pCamera->rotation.x = angle;
    }
}

void RgDrawViewSetRotateY(RgDrawView *pView, float angle)
{
    int screenIndex;
    int cameraIndex;
    StudioCamera *pCamera;

    screenIndex = pView->m_pParentStudio->m_ScreenIndex;
    pCamera = 0;
    if (screenIndex < 2) {
        if (screenIndex >= 0) {
            cameraIndex = 0;
            switch (screenIndex) {
            case 0:
                cameraIndex = 0;
                break;
            case 1:
                cameraIndex = 1;
                break;
            default:
                RgError(D_00A54AD0, D_00A54AF0, 69, screenIndex);
                break;
            }
            pCamera = xglStudioSelectGetActiveCamera(cameraIndex);
        }
    }
    if (pCamera != 0) {
        pCamera->rotation.y = angle;
    }
}

void RgDrawViewSetRotateZ(RgDrawView *pView, float angle)
{
    int screenIndex;
    int cameraIndex;
    StudioCamera *pCamera;

    screenIndex = pView->m_pParentStudio->m_ScreenIndex;
    pCamera = 0;
    if (screenIndex < 2) {
        if (screenIndex >= 0) {
            cameraIndex = 0;
            switch (screenIndex) {
            case 0:
                cameraIndex = 0;
                break;
            case 1:
                cameraIndex = 1;
                break;
            default:
                RgError(D_00A54AD0, D_00A54AF0, 69, screenIndex);
                break;
            }
            pCamera = xglStudioSelectGetActiveCamera(cameraIndex);
        }
    }
    if (pCamera != 0) {
        pCamera->rotation.z = angle;
    }
}

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

void _DrawReqTerminate(RgDraw *pDraw)
{
    RgDrawRequest *pRequest;
    unsigned int requestIndex;

    requestIndex = 0;
    if (pDraw->m_RequestCount != 0) {
        pRequest = pDraw->m_Requests;
        do {
            if (pRequest->clearFunc != 0) {
                pRequest->clearFunc(pRequest->pObject);
            }
            requestIndex += 1;
            pRequest += 1;
        } while (requestIndex < pDraw->m_RequestCount);
    }
    pDraw->m_RequestCount = 0;
}

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
