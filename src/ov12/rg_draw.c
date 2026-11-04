/*
 * OV12 original TU 45: 0x00a26c78..0x00a285e0 (45 functions)
 */
#include "common.h"
#include "shared.h"
#include "main/xgl_studio.h"
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

typedef struct RgLight RgLight;

extern RgLight *InstanceOfGlobalLight(void);
extern void xglStudioChange(int studio_index);
extern void XrgSleep(void);
extern StudioCamera *xglStudioSelectGetActiveCamera(int studioIndex);
static void _CopyFog(RgFog *pDst, RgFog *pSrc);
static void _SetGlobalLight(void);
static void _ClearStudioList(RgDraw *pDraw);
static void _FullScreenStudio(RgDraw *pDraw);
static void _DoubleScreenStudio(RgDraw *pDraw);
static void _DrawMain(RgDraw *pDraw);
static void _DrawReqTerminate(RgDraw *pDraw);

typedef unsigned int Quadword __attribute__((mode(TI)));

typedef union GifTag {
    struct {
        u64 lo;
        u64 hi;
    } part;
    Quadword quad;
} GifTag;

const GifTag D_00A54AC0 = {
    .part = {0x1000000000008000ULL, 0xE}
};

/* The assertion text, filename and error messages referenced by this TU. */
const char D_00A54AD0[32] = "not exist xeno studio ID (%d)";

const char D_00A54AF0[24] = "../rg_draw.euc.c";

const char D_00A54B08[16] = "pView != NIL";

const char D_00A54B18[24] = "pParentStudio != NIL";

const char D_00A54B30[16] = "pStudio != NIL";

const char D_00A54B40[24] = "pStudio->m_pView != NIL";

const char D_00A54B58[16] = "pDraw != NIL";

const char D_00A54B68[24] = "unknown studio id %d";

const char D_00A54B80[16] = "pFog != NIL";

const char D_00A54B90[32] = "global studio don't have view";

const char D_00A54BB0[24] = "unknown studio ID %d";

const char D_00A54BC8[32] = "pDraw->m_uReqNum < REQ_CAPA";

const char D_00A54BE8[40] = "pObject != NIL && pHandler != NIL";

const char D_00A54C50[24] = "unknown draw id %d";

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

static void _SetGlobalLight(void)
{
    RgLight *globalLight;
    StudioLight *studioLight;
    Vector4 *ambientColor;

    globalLight = InstanceOfGlobalLight();
    xglStudioChange(0);
    xglStudioGetLight(&studioLight);
    if (globalLight != 0) {
        if (studioLight != 0) {
            ambientColor = &studioLight->ambient_color;
            ambientColor->x = 1.0f;
            ambientColor->y = 1.0f;
            ambientColor->z = 1.0f;
            ambientColor->w = 1.0f;
        }
    }
}

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

extern void XrgUnitMatrix(RgMatrix destination);
extern void XrgUnitVector(RgVector vector);

static void _InitRgDrawView(RgDrawView *pView, RgDrawStudio *pParentStudio)
{
    if (pView == 0) {
        assert_prog(D_00A54B08, D_00A54AF0, 169);
    }
    if (pParentStudio == 0) {
        assert_prog(D_00A54B18, D_00A54AF0, 170);
    }
    XrgUnitMatrix(pView->viewMatrix);
    XrgUnitVector(pView->axisX);
    XrgUnitVector(pView->axisY);
    pView->projectionMode = 0;
    pView->clippingMode = 0;
    pView->m_pParentStudio = pParentStudio;
    pView->screenRect.x0 = 0;
    pView->screenRect.y0 = 0;
    pView->screenRect.x1 = 512;
    pView->screenRect.y1 = 448;
}

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
static RgRect s_aView_0 = {0, 0, 512, 448};

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

extern void XrgSetIVector(int destination[4], int x, int y, int z, int w);

static int _GetSplitRect(int splitMode, int screenIndex, RgRect *pRect)
{
    switch (splitMode) {
    case 0:
        if (screenIndex == 0) {
            XrgSetIVector(&pRect->x0, 0, 0, 256, 448);
        } else {
            XrgSetIVector(&pRect->x0, 256, 0, 512, 448);
        }
        break;
    case 1:
        if (screenIndex == 0) {
            XrgSetIVector(&pRect->x0, 0, 0, 512, 224);
        } else {
            XrgSetIVector(&pRect->x0, 0, 224, 512, 448);
        }
        break;
    default:
        return 0;
    }
    return 1;
}

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

void RgDrawViewInit(RgDrawView *pView)
{
    int screenIndex;
    int cameraIndex;
    StudioCamera *pCamera;

    if (pView == 0) {
        assert_prog(D_00A54B08, D_00A54AF0, 541);
    }
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
        pCamera->state = 4;
        pCamera->fov = 0.698132f;
        pCamera->position.vector.y = 0.0f;
        pCamera->position.vector.z = 0.0f;
        pCamera->rotation.vector.x = 0.0f;
        pCamera->rotation.vector.y = 0.0f;
        pCamera->rotation.vector.z = 0.0f;
        pCamera->position.vector.x = 0.0f;
    }
}

/*
 * External file-backed witness of this allocation's own functions, not
 * candidate-emitted data (same window as the blocks above).
 *
 * ov12:0x00a54ad0 contains the error message "not exist xeno studio ID (%d)".
 */
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
        pCamera->position.vector.x = pPosition->x;
        pCamera->position.vector.y = pPosition->y;
        pCamera->position.vector.z = pPosition->z;
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
        pCamera->rotation.vector.x = angle;
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
        pCamera->rotation.vector.y = angle;
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
        pCamera->rotation.vector.z = angle;
    }
}

extern void xglVectorMulMat(Vector4 *destination, Matrix4 matrix,
                            const Vector4 *vector);

int RgDrawViewIsPointInView(RgDrawView *pView, Vector4 *pPoint, float margin)
{
    int result;
    int screenIndex;
    int cameraIndex;
    float limit;
    StudioCamera *pCamera;
    Matrix4 *pClipMatrix0;
    Matrix4 *pClipMatrix1;
    Vector4 point;
    Vector4 clipped0;
    Vector4 clipped1;

    result = 0;
    if (pView == 0) {
        assert_prog(D_00A54B08, D_00A54AF0, 602);
    }
    limit = -margin;
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
        pClipMatrix0 = &pCamera->clipMatrices[0];
        pClipMatrix1 = &pCamera->clipMatrices[1];
        point.x = pPoint->x;
        point.y = pPoint->y;
        point.z = pPoint->z;
        point.w = 1.0f;
        xglVectorMulMat(&clipped0, *pClipMatrix0, &point);
        xglVectorMulMat(&clipped1, *pClipMatrix1, &point);
        if (clipped0.x < limit && clipped0.y < limit && clipped1.x < limit
            && clipped1.y < limit) {
            result = 1;
        }
    }
    return result;
}

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

RgDrawView *RgDrawStudioGetView(RgDrawStudio *pStudio)
{
    RgDrawView *pView;
    StudioCamera *camera;
    int screenIndex;
    int cameraIndex;

    if (pStudio == 0) {
        assert_prog(D_00A54B30, D_00A54AF0, 649);
    }
    screenIndex = pStudio->m_ScreenIndex;
    pView = _GetViewRgDrawStudio(pStudio);
    if (pView == 0) {
        assert_prog(D_00A54B08, D_00A54AF0, 653);
    }
    camera = 0;
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
            camera = xglStudioSelectGetActiveCamera(cameraIndex);
        }
    }
    if (camera != 0) {
        XrgCopyMatrix(pView->viewMatrix, camera->viewMatrix[0]);
        XrgCopyVector(pView->axisX, &camera->screenScale.x);
        XrgCopyVector(pView->axisY, &camera->screenOffset.x);
        switch (screenIndex) {
        case 0:
            pView->projectionMode = 5;
            pView->clippingMode = 6;
            break;
        case 1:
            pView->projectionMode = 7;
            pView->clippingMode = 8;
            break;
        case -1:
            RgError(D_00A54B90, D_00A54AF0, 669);
            break;
        default:
            RgError(D_00A54BB0, D_00A54AF0, 672, screenIndex);
        }
    }
    return pView;
}

void RgDrawReq(RgDraw *pDraw, void *pObject,
               void (*drawFunc)(void *pObject, RgDrawStudio *pStudio),
               void (*clearFunc)(void *pObject), int priority, int drawID)
{
    RgDrawRequest *request;

    if (pDraw == 0) {
        assert_prog(D_00A54B58, D_00A54AF0, 695);
    }
    if (pDraw->m_RequestCount >= 128) {
        assert_prog(D_00A54BC8, D_00A54AF0, 696);
    }
    if (pObject == 0 || drawFunc == 0) {
        assert_prog(D_00A54BE8, D_00A54AF0, 697);
    }
    request = &pDraw->m_Requests[pDraw->m_RequestCount++];
    request->pObject = pObject;
    request->drawFunc = drawFunc;
    request->clearFunc = clearFunc;
    request->priority = priority;
    request->drawID = drawID;
}

/* Four GIF tags, the 64-byte block _FadeShadow copies from ov12:0x00a54c10. */
/* Two 32-byte halves of the 64-byte vertex block. */
typedef struct GifTagBlock {
    GifTag v[4];
} GifTagBlock;

/* One GIF PACKED-mode RGBAQ quadword: four 32-bit channel words. */
typedef union RgDrawGifRgbaq {
    int channels[4];
    Quadword quad;
} RgDrawGifRgbaq;

/* The 0x20-byte VIF1 packet object sceVif1PkInit fills on the stack. */
typedef struct sceVif1Packet {
    unsigned char opaque[0x20];
} sceVif1Packet;

extern void sceVif1PkInit(XglPacket *packet, u32 buffer);
extern int sceVif1PkSize(XglPacket *packet);
extern void sceVif1PkAddGsAD(XglPacket *packet, int address, u64 data);
extern void sceVif1PkAddGsPacked(XglPacket *packet, Quadword data);
extern void nmlModelDirectSend(int mode, const void *data, int count);
extern const GifTagBlock D_00A54C10;

static void _FadeShadow(int alpha, int mode)
{
    GifTagBlock vertices = D_00A54C10;
    RgDrawGifRgbaq rgbaq;
    sceVif1Packet packet;
    XglPacket *pPacket;
    unsigned long long clampData;

    memset(&rgbaq, '\0', sizeof rgbaq);
    rgbaq.channels[3] = alpha;
    pPacket = (XglPacket *)&packet;
    sceVif1PkInit(pPacket, (u32)SCRATCHPAD_BASE);
    _openVifGifAD(pPacket);
    sceVif1PkAddGsAD(pPacket, 0x3f, 0x0ULL);
    sceVif1PkAddGsAD(pPacket, 0x47, 0x31001u);
    clampData = ((u64)0x3fcu << 32) | ((u64)0x3fu << 16) | 0xc000u;
    sceVif1PkAddGsAD(pPacket, 0x08, clampData);
    sceVif1PkAddGsAD(pPacket, 0x49, 0x0);
    sceVif1PkAddGsAD(pPacket, 0x42, 0x44);
    _closeVifGif(pPacket);
    _openVifGif(pPacket, 4u, 5u, 0x44441u);
    sceVif1PkAddGsPacked(pPacket, rgbaq.quad);
    sceVif1PkAddGsPacked(pPacket, vertices.v[0].quad);
    sceVif1PkAddGsPacked(pPacket, vertices.v[1].quad);
    sceVif1PkAddGsPacked(pPacket, vertices.v[2].quad);
    sceVif1PkAddGsPacked(pPacket, vertices.v[3].quad);
    _closeVifGif(pPacket);
    nmlModelDirectSend(mode, (const void *)SCRATCHPAD_BASE,
                       sceVif1PkSize(pPacket));
}

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

static void _DrawMain(RgDraw *pDraw)
{
    unsigned int priority;
    unsigned int requestIndex;
    RgDrawRequest *request;
    RgDrawStudio *studio;
    unsigned int activeStudioMask;

    if (pDraw == 0) {
        assert_prog(D_00A54B58, D_00A54AF0, 796);
    }
    if (pDraw->m_Enabled == 0) {
        return;
    }
    for (priority = 0; priority < 7; priority++) {
        for (requestIndex = 0; requestIndex < pDraw->m_RequestCount;
             requestIndex++) {
            request = &pDraw->m_Requests[requestIndex];
            if (request->priority != priority) {
                continue;
            }
            activeStudioMask = pDraw->m_ActiveStudioMask;
            if (request->drawFunc == 0) {
                continue;
            }
            switch (request->drawID) {
            case -2: {
                unsigned int screenIndex;
                for (screenIndex = 0; screenIndex < 2; screenIndex++) {
                    studio = pDraw->m_pStudios[screenIndex];
                    if (studio != 0) {
                        request->drawFunc(request->pObject, studio);
                    }
                }
                break;
            }
            case -1:
                if (activeStudioMask != 0) {
                    request->drawFunc(request->pObject,
                                      pDraw->m_pDefaultStudio);
                }
                break;
            case 0:
                if (pDraw->m_pStudios[0] != 0) {
                    request->drawFunc(request->pObject,
                                      pDraw->m_pStudios[0]);
                }
                break;
            case 1:
                if (pDraw->m_pStudios[1] != 0) {
                    request->drawFunc(request->pObject,
                                      pDraw->m_pStudios[1]);
                }
                break;
            default:
                RgError(D_00A54C50, D_00A54AF0, 776, request->drawID);
            }
        }
    }
}

void RgDrawJob(RgDraw *pDraw)
{
    if (pDraw == 0) {
        assert_prog(D_00A54B58, D_00A54AF0, 834);
    }
    _SetGlobalLight();
    _DrawMain(pDraw);
    _DrawReqTerminate(pDraw);
}

void RgDrawFadeIn(RgDraw *pDraw)
{
    int nextFadeCounter;

    if (pDraw == 0) {
        assert_prog(D_00A54B58, D_00A54AF0, 856);
    }
    if (pDraw->m_Enabled != 0) {
        while (pDraw->m_FadeCounter < 50) {
            nextFadeCounter = pDraw->m_FadeCounter - 20;
            pDraw->m_FadeCounter = nextFadeCounter;
            if (nextFadeCounter >= 0) {
                _DrawMain(pDraw);
            }
            XrgSleep();
        }
        pDraw->m_Enabled = 0;
    }
}

void RgDrawFadeOut(RgDraw *pDraw)
{
    if (pDraw == 0) {
        assert_prog(D_00A54B58, D_00A54AF0, 876);
    }
}
