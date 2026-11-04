/*
 * OV12 original TU 45: 0x00a26c78..0x00a285e0 (45 functions)
 */
#include "common.h"
#include "shared.h"
#include "main/xgl_studio.h"
#include "main/xgl_packet.h"
#include "ov12/rg_draw.h"
#include "ov12/rg_singleton_id.h"
#include "ov12/xrg_rand_int.h"
#include "ov12/assert_prog.h"
#include "ov12/rg_error.h"
#include "ov12/rg_heap.h"
#include "ov12/xrg_sysinit.h"
#include "main/xgl_2.h"
#include "main/nml_model_set.h"
#include "ov12/rg_light.h"

/* Requests are drawn in priority order 0..RG_DRAW_PRIORITY_COUNT - 1. */
#define RG_DRAW_PRIORITY_COUNT 7
/* RgDraw.m_FadeCounter: RgDrawFadeIn counts it down by this step to the limit. */
#define RG_DRAW_FADE_STEP 20
#define RG_DRAW_FADE_LIMIT 50

/* Screen index of the global studio, which has no screen (no view). */
#define RG_DRAW_STUDIO_GLOBAL -1
/* RgDraw.m_ActiveStudioMask: bit i is set when studio i exists. */
#define RG_DRAW_STUDIO_MASK_LEFT 1
#define RG_DRAW_STUDIO_MASK_BOTH 3

/* drawID of a request: which studio(s) its draw callback is called with. */
#define RG_DRAW_ID_ALL_STUDIOS -2
#define RG_DRAW_ID_GLOBAL -1

/* Screen split modes of _GetSplitRect: left/right or top/bottom halves. */
#define RG_DRAW_SPLIT_LEFT_RIGHT 0
#define RG_DRAW_SPLIT_TOP_BOTTOM 1

/* Sizes of the heap objects (RgDrawView, RgDrawStudio). */
#define RG_DRAW_VIEW_SIZE 128
#define RG_DRAW_STUDIO_SIZE 12

/* StudioCamera.state value the views reset their camera to, and its field of view. */
#define RG_DRAW_CAMERA_STATE 4
#define RG_DRAW_CAMERA_FOV 0.698132f

/* GS values _FadeShadow programs: depth test always passes, and the vertex packet. */
#define RG_DRAW_TEST_ZALWAYS 0x31001
#define RG_DRAW_REGS_COLOR_4_XYZ 0x44441

/* RgDrawView.projectionMode / clippingMode of the left and right screens. */
#define RG_DRAW_LEFT_PROJECTION_MODE 5
#define RG_DRAW_LEFT_CLIPPING_MODE 6
#define RG_DRAW_RIGHT_PROJECTION_MODE 7
#define RG_DRAW_RIGHT_CLIPPING_MODE 8

/*
 * The 0x20-byte VIF1 packet object sceVif1PkInit fills on the stack.
 * Partial view of XglPacket (measured: XglPacket is 0x28 bytes and the
 * frame of _FadeShadow needs 0x20).
 */
typedef struct XglPacketSdkView {
    unsigned char opaque[0x20];
} XglPacketSdkView;

typedef union GifTag {
    struct {
        u64 lo;
        u64 hi;
    } part;
    Quadword quad;
} GifTag;

/* The 64-byte block _FadeShadow copies from ov12:0x00a54c10: four GIF PACKED quadwords. */
typedef struct GifTagBlock {
    GifTag v[4];
} GifTagBlock;

/* One GIF PACKED-mode RGBAQ quadword: four 32-bit channel words. */
typedef union RgDrawGifRgbaq {
    int channels[4];
    Quadword quad;
} RgDrawGifRgbaq;

extern void xglCameraSetWindow(StudioCamera *camera, int x0, int y0, int x1,
                               int y1);
extern const GifTagBlock drawFadeShadowVertexBlock;

static void _InitRgDrawView(RgDrawView *pView, RgDrawStudio *pParentStudio);
static void _InitRgDrawStudio(RgDrawStudio *pStudio, int screenIndex,
                              RgFog *pFog);
static void _DestructDraw(RgDraw *pDraw);
static void _CopyFog(RgFog *pDst, RgFog *pSrc);
static void _SetGlobalLight(void);
static void _ClearStudioList(RgDraw *pDraw);
static void _FullScreenStudio(RgDraw *pDraw);
static void _DoubleScreenStudio(RgDraw *pDraw);
static int _GetSplitRect(int splitMode, int screenIndex, RgRect *pRect);
static void _DrawMain(RgDraw *pDraw);
static void _DrawReqTerminate(RgDraw *pDraw);
const GifTag drawDirectAdGifTag = {
    .part = {0x1000000000008000ULL, 0xE}
};

/* The assertion text, filename and error messages referenced by this TU. */
const char drawUnknownStudioIdError[32] = "not exist xeno studio ID (%d)";

const char drawSourcePath[24] = "../rg_draw.euc.c";

const char drawAssertViewNotNil[16] = "pView != NIL";

const char drawAssertParentStudioNotNil[24] = "pParentStudio != NIL";

const char drawAssertStudioNotNil[16] = "pStudio != NIL";

const char drawAssertStudioViewNotNil[24] = "pStudio->m_pView != NIL";

const char drawAssertDrawNotNil[16] = "pDraw != NIL";

const char drawUnknownStudioIdFormat[24] = "unknown studio id %d";

const char drawAssertFogNotNil[16] = "pFog != NIL";

const char drawGlobalStudioHasNoViewError[32] = "global studio don't have view";

const char drawUnknownStudioIdUpperFormat[24] = "unknown studio ID %d";

const char drawAssertRequestCountBelowCapacity[32] = "pDraw->m_uReqNum < REQ_CAPA";

const char drawAssertObjectAndHandlerNotNil[40] = "pObject != NIL && pHandler != NIL";

const char drawUnknownDrawIdFormat[24] = "unknown draw id %d";


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

    tag = drawDirectAdGifTag;
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

static void _InitRgDrawView(RgDrawView *pView, RgDrawStudio *pParentStudio)
{
    if (pView == 0) {
        assert_prog(drawAssertViewNotNil, drawSourcePath, 169);
    }
    if (pParentStudio == 0) {
        assert_prog(drawAssertParentStudioNotNil, drawSourcePath, 170);
    }
    XrgUnitMatrix(pView->viewMatrix);
    XrgUnitVector(pView->axisX);
    XrgUnitVector(pView->axisY);
    pView->projectionMode = 0;
    pView->clippingMode = 0;
    pView->m_pParentStudio = pParentStudio;
    pView->screenRect.x0 = 0;
    pView->screenRect.y0 = 0;
    pView->screenRect.x1 = RG_DRAW_SCREEN_WIDTH;
    pView->screenRect.y1 = RG_DRAW_SCREEN_HEIGHT;
}

static void _DestructRgDrawView(RgDrawView *pView)
{
    if (pView == 0) {
        assert_prog(drawAssertViewNotNil, drawSourcePath, 191);
    }
}

static RgDrawView *_CreateRgDrawView(RgDrawStudio *pParentStudio)
{
    RgDrawView *pView;

    if (pParentStudio == 0) {
        assert_prog(drawAssertParentStudioNotNil, drawSourcePath, 197);
    }
    pView = RgHeapAlloc(InstanceOfRgHeap(), RG_DRAW_VIEW_SIZE, drawSourcePath, 198);
    _InitRgDrawView(pView, pParentStudio);
    return pView;
}

static void _DisposeRgDrawView(RgDrawView *pView)
{
    if (pView == 0) {
        assert_prog(drawAssertViewNotNil, drawSourcePath, 205);
    }
    _DestructRgDrawView(pView);
    RgHeapFree(InstanceOfRgHeap(), pView, drawSourcePath, 207);
}

static void _InitRgDrawStudio(RgDrawStudio *pStudio, int screenIndex,
                              RgFog *pFog)
{
    if (pStudio == 0) {
        assert_prog(drawAssertStudioNotNil, drawSourcePath, 215);
    }
    pStudio->m_pFog = pFog;
    pStudio->m_ScreenIndex = screenIndex;
    if (screenIndex != RG_DRAW_STUDIO_GLOBAL) {
        pStudio->m_pView = _CreateRgDrawView(pStudio);
        return;
    }
    pStudio->m_pView = 0;
}

static void _DestructRgDrawStudio(RgDrawStudio *pStudio)
{
    if (pStudio == 0) {
        assert_prog(drawAssertStudioNotNil, drawSourcePath, 231);
    }
    if (pStudio->m_pView != 0) {
        _DisposeRgDrawView(pStudio->m_pView);
    }
}

static RgDrawStudio *_CreateRgDrawStudio(int screenIndex, RgFog *pFog)
{
    RgDrawStudio *pStudio;

    pStudio = RgHeapAlloc(InstanceOfRgHeap(), RG_DRAW_STUDIO_SIZE, drawSourcePath, 240);
    _InitRgDrawStudio(pStudio, screenIndex, pFog);
    return pStudio;
}

static void _DisposeRgDrawStudio(RgDrawStudio *pStudio)
{
    if (pStudio == 0) {
        assert_prog(drawAssertStudioNotNil, drawSourcePath, 247);
    }
    _DestructRgDrawStudio(pStudio);
    RgHeapFree(InstanceOfRgHeap(), pStudio, drawSourcePath, 249);
}

static RgDrawView *_GetViewRgDrawStudio(RgDrawStudio *pStudio)
{
    if (pStudio == 0) {
        assert_prog(drawAssertStudioNotNil, drawSourcePath, 254);
    }
    if (pStudio->m_pView == 0) {
        assert_prog(drawAssertStudioViewNotNil, drawSourcePath, 255);
    }
    return pStudio->m_pView;
}

static void _InitStudioList(RgDraw *pDraw)
{
    unsigned int slotIndex;

    if (pDraw == 0) {
        assert_prog(drawAssertDrawNotNil, drawSourcePath, 266);
    }
    slotIndex = 0;
    do {
        pDraw->m_pStudios[slotIndex] = 0;
        slotIndex += 1;
    } while (slotIndex < RG_DRAW_STUDIO_COUNT);
    pDraw->m_ActiveStudioMask = 0;
}

static void _ClearStudioList(RgDraw *pDraw)
{
    RgDrawStudio **pSlot;
    RgDrawStudio *pStudio;
    unsigned int slotIndex;

    if (pDraw == 0) {
        assert_prog(drawAssertDrawNotNil, drawSourcePath, 276);
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
    } while (slotIndex < RG_DRAW_STUDIO_COUNT);
    pDraw->m_ActiveStudioMask = 0;
}

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
        assert_prog(drawAssertDrawNotNil, drawSourcePath, 291);
    }
    _ClearStudioList(pDraw);
    pDraw->m_FullScreenMode = 1;
    pDraw->m_FullScreenParam = 2;
    pStudio = _CreateRgDrawStudio(0, &pDraw->m_Fog);
    pDraw->m_pStudios[1] = 0;
    pDraw->m_ActiveStudioMask = RG_DRAW_STUDIO_MASK_LEFT;
    pDraw->m_pStudios[0] = pStudio;
    xglCameraSetWindow(xglStudioSelectGetActiveCamera(0), s_aView_0.x0,
                       s_aView_0.y0, s_aView_0.x1, s_aView_0.y1);
    nmlModelUseSubWindow(0, 2);
    nmlModelUseSubWindow(1, 0);
    nmlModelUseSubWindow(2, 0);
    nmlModelUseSubWindow(3, 0);
}

static int _GetSplitRect(int splitMode, int screenIndex, RgRect *pRect)
{
    switch (splitMode) {
    case RG_DRAW_SPLIT_LEFT_RIGHT:
        if (screenIndex == 0) {
            XrgSetIVector(&pRect->x0, 0, 0, RG_DRAW_SCREEN_WIDTH / 2, RG_DRAW_SCREEN_HEIGHT);
        } else {
            XrgSetIVector(&pRect->x0, RG_DRAW_SCREEN_WIDTH / 2, 0, RG_DRAW_SCREEN_WIDTH, RG_DRAW_SCREEN_HEIGHT);
        }
        break;
    case RG_DRAW_SPLIT_TOP_BOTTOM:
        if (screenIndex == 0) {
            XrgSetIVector(&pRect->x0, 0, 0, RG_DRAW_SCREEN_WIDTH, RG_DRAW_SCREEN_HEIGHT / 2);
        } else {
            XrgSetIVector(&pRect->x0, 0, RG_DRAW_SCREEN_HEIGHT / 2, RG_DRAW_SCREEN_WIDTH, RG_DRAW_SCREEN_HEIGHT);
        }
        break;
    default:
        return 0;
    }
    return 1;
}

static void _SetScreenSplitMode(RgDraw *pDraw, int splitMode)
{
    RgRect rect;
    RgRect *pScreenRect0;
    RgRect *pScreenRect1;

    if (pDraw == 0) {
        assert_prog(drawAssertDrawNotNil, drawSourcePath, 342);
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
        assert_prog(drawAssertDrawNotNil, drawSourcePath, 375);
    }
    _ClearStudioList(pDraw);
    pDraw->m_FullScreenMode = 0;
    pDraw->m_FullScreenParam = 0;
    pDraw->m_pStudios[0] = _CreateRgDrawStudio(0, pFog);
    pDraw->m_pStudios[1] = _CreateRgDrawStudio(1, pFog);
    pDraw->m_ActiveStudioMask = RG_DRAW_STUDIO_MASK_BOTH;
    _SetScreenSplitMode(pDraw, RG_DRAW_SPLIT_LEFT_RIGHT);
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
        assert_prog(drawAssertDrawNotNil, drawSourcePath, 403);
    }
    pDraw->m_FadeCounter = 0;
    pDraw->m_Enabled = 1;
    pDraw->m_FadeParam = 0;
    pDraw->m_RequestCount = 0;
    _DefaultFog(pFog);
    pDraw->m_pDefaultStudio = _CreateRgDrawStudio(RG_DRAW_STUDIO_GLOBAL, pFog);
    _InitStudioList(pDraw);
}

static void _DestructDraw(RgDraw *pDraw)
{
    if (pDraw == 0) {
        assert_prog(drawAssertDrawNotNil, drawSourcePath, 0x1A1);
    }
    _DisposeRgDrawStudio(pDraw->m_pDefaultStudio);
    _ClearStudioList(pDraw);
}

static void _WrapperDestruct(void *pDraw)
{
    _DestructDraw(pDraw);
    RgHeapFree(InstanceOfRgHeap(), pDraw, drawSourcePath, 421);
}

RgDraw *InstanceOfRgDraw(void)
{
    RgDraw *pDraw;

    pDraw = RgSingletonIDGet(RG_SINGLETON_DRAW);
    if (pDraw == 0) {
        pDraw = RgHeapAlloc(InstanceOfRgHeap(), sizeof(RgDraw), drawSourcePath, 431);
        if (pDraw == 0) {
            assert_prog(drawAssertDrawNotNil, drawSourcePath, 432);
        }
        _InitDraw(pDraw);
        RgSingletonIDEntry(RG_SINGLETON_DRAW, pDraw, _WrapperDestruct);
        RgDrawCreateDrawStudioFullScreen(pDraw);
    }
    return pDraw;
}

RgDrawStudio *RgDrawGetStudio(RgDraw *pDraw, int screenIndex)
{
    if (pDraw == 0) {
        assert_prog(drawAssertDrawNotNil, drawSourcePath, 447);
    }
    switch (screenIndex) {
    case RG_DRAW_STUDIO_GLOBAL:
        return 0;
    case 0:
    case 1:
        return pDraw->m_pStudios[screenIndex];
    default:
        RgError(drawUnknownStudioIdFormat, drawSourcePath, 458, screenIndex);
        return 0;
    }
}

void RgDrawCreateDrawStudioFullScreen(RgDraw *pDraw)
{
    if (pDraw == 0) {
        assert_prog(drawAssertDrawNotNil, drawSourcePath, 471);
    }
    _ClearStudioList(pDraw);
    _FullScreenStudio(pDraw);
}

void RgDrawCreateDrawStudioDouble(RgDraw *pDraw)
{
    if (pDraw == 0) {
        assert_prog(drawAssertDrawNotNil, drawSourcePath, 480);
    }
    _ClearStudioList(pDraw);
    _DoubleScreenStudio(pDraw);
}

void RgDrawGetGlobalFog(RgDraw *pDraw, RgFog *pFog)
{
    if (pDraw == 0) {
        assert_prog(drawAssertDrawNotNil, drawSourcePath, 492);
    }
    if (pFog == 0) {
        assert_prog(drawAssertFogNotNil, drawSourcePath, 493);
    }
    _CopyFog(pFog, &pDraw->m_Fog);
}

void RgDrawSetGlobalFog(RgDraw *pDraw, RgFog *pFog)
{
    if (pDraw == 0) {
        assert_prog(drawAssertDrawNotNil, drawSourcePath, 502);
    }
    if (pFog == 0) {
        assert_prog(drawAssertFogNotNil, drawSourcePath, 503);
    }
    _CopyFog(&pDraw->m_Fog, pFog);
}

void RgDrawViewInit(RgDrawView *pView)
{
    int screenIndex;
    int cameraIndex;
    StudioCamera *pCamera;

    if (pView == 0) {
        assert_prog(drawAssertViewNotNil, drawSourcePath, 541);
    }
    screenIndex = pView->m_pParentStudio->m_ScreenIndex;
    pCamera = 0;
    if (screenIndex < RG_DRAW_STUDIO_COUNT) {
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
                RgError(drawUnknownStudioIdError, drawSourcePath, 69, screenIndex);
                break;
            }
            pCamera = xglStudioSelectGetActiveCamera(cameraIndex);
        }
    }
    if (pCamera != 0) {
        pCamera->state = RG_DRAW_CAMERA_STATE;
        pCamera->fov = RG_DRAW_CAMERA_FOV;
        pCamera->position.vector.y = 0.0f;
        pCamera->position.vector.z = 0.0f;
        pCamera->rotation.vector.x = 0.0f;
        pCamera->rotation.vector.y = 0.0f;
        pCamera->rotation.vector.z = 0.0f;
        pCamera->position.vector.x = 0.0f;
    }
}

void RgDrawViewSetPosition(RgDrawView *pView, const RgVector position)
{
    int screenIndex;
    int cameraIndex;
    StudioCamera *pCamera;

    screenIndex = pView->m_pParentStudio->m_ScreenIndex;
    pCamera = 0;
    if (screenIndex < RG_DRAW_STUDIO_COUNT) {
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
                RgError(drawUnknownStudioIdError, drawSourcePath, 69, screenIndex);
                break;
            }
            pCamera = xglStudioSelectGetActiveCamera(cameraIndex);
        }
    }
    if (pCamera != 0) {
        pCamera->position.vector.x = position[0];
        pCamera->position.vector.y = position[1];
        pCamera->position.vector.z = position[2];
    }
}

void RgDrawViewSetRotateX(RgDrawView *pView, float angle)
{
    int screenIndex;
    int cameraIndex;
    StudioCamera *pCamera;

    screenIndex = pView->m_pParentStudio->m_ScreenIndex;
    pCamera = 0;
    if (screenIndex < RG_DRAW_STUDIO_COUNT) {
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
                RgError(drawUnknownStudioIdError, drawSourcePath, 69, screenIndex);
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
    if (screenIndex < RG_DRAW_STUDIO_COUNT) {
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
                RgError(drawUnknownStudioIdError, drawSourcePath, 69, screenIndex);
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
    if (screenIndex < RG_DRAW_STUDIO_COUNT) {
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
                RgError(drawUnknownStudioIdError, drawSourcePath, 69, screenIndex);
                break;
            }
            pCamera = xglStudioSelectGetActiveCamera(cameraIndex);
        }
    }
    if (pCamera != 0) {
        pCamera->rotation.vector.z = angle;
    }
}


int RgDrawViewIsPointInView(RgDrawView *pView, const RgVector position, float margin)
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
        assert_prog(drawAssertViewNotNil, drawSourcePath, 602);
    }
    limit = -margin;
    screenIndex = pView->m_pParentStudio->m_ScreenIndex;
    pCamera = 0;
    if (screenIndex < RG_DRAW_STUDIO_COUNT) {
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
                RgError(drawUnknownStudioIdError, drawSourcePath, 69, screenIndex);
                break;
            }
            pCamera = xglStudioSelectGetActiveCamera(cameraIndex);
        }
    }
    if (pCamera != 0) {
        pClipMatrix0 = &pCamera->clipMatrices[0];
        pClipMatrix1 = &pCamera->clipMatrices[1];
        point.x = position[0];
        point.y = position[1];
        point.z = position[2];
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
        assert_prog(drawAssertViewNotNil, drawSourcePath, 632);
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
        assert_prog(drawAssertStudioNotNil, drawSourcePath, 649);
    }
    screenIndex = pStudio->m_ScreenIndex;
    pView = _GetViewRgDrawStudio(pStudio);
    if (pView == 0) {
        assert_prog(drawAssertViewNotNil, drawSourcePath, 653);
    }
    camera = 0;
    if (screenIndex < RG_DRAW_STUDIO_COUNT) {
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
                RgError(drawUnknownStudioIdError, drawSourcePath, 69, screenIndex);
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
            pView->projectionMode = RG_DRAW_LEFT_PROJECTION_MODE;
            pView->clippingMode = RG_DRAW_LEFT_CLIPPING_MODE;
            break;
        case 1:
            pView->projectionMode = RG_DRAW_RIGHT_PROJECTION_MODE;
            pView->clippingMode = RG_DRAW_RIGHT_CLIPPING_MODE;
            break;
        case RG_DRAW_STUDIO_GLOBAL:
            RgError(drawGlobalStudioHasNoViewError, drawSourcePath, 669);
            break;
        default:
            RgError(drawUnknownStudioIdUpperFormat, drawSourcePath, 672, screenIndex);
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
        assert_prog(drawAssertDrawNotNil, drawSourcePath, 695);
    }
    if (pDraw->m_RequestCount >= RG_DRAW_REQ_CAPA) {
        assert_prog(drawAssertRequestCountBelowCapacity, drawSourcePath, 696);
    }
    if (pObject == 0 || drawFunc == 0) {
        assert_prog(drawAssertObjectAndHandlerNotNil, drawSourcePath, 697);
    }
    request = &pDraw->m_Requests[pDraw->m_RequestCount++];
    request->pObject = pObject;
    request->drawFunc = drawFunc;
    request->clearFunc = clearFunc;
    request->priority = priority;
    request->drawID = drawID;
}

static void _FadeShadow(int alpha, int mode)
{
    GifTagBlock vertices = drawFadeShadowVertexBlock;
    RgDrawGifRgbaq rgbaq;
    XglPacketSdkView packet;
    XglPacket *pPacket;
    unsigned long long clampData;

    memset(&rgbaq, '\0', sizeof rgbaq);
    rgbaq.channels[3] = alpha;
    pPacket = (XglPacket *)&packet;
    sceVif1PkInit(pPacket, (u32)SCRATCHPAD_BASE);
    _openVifGifAD(pPacket);
    sceVif1PkAddGsAD(pPacket, SCE_GS_TEXFLUSH, 0x0ULL);
    sceVif1PkAddGsAD(pPacket, SCE_GS_TEST_1, RG_DRAW_TEST_ZALWAYS);
    clampData = ((u64)0x3fcu << 32) | ((u64)0x3fu << 16) | 0xc000u;
    sceVif1PkAddGsAD(pPacket, SCE_GS_CLAMP_1, clampData);
    sceVif1PkAddGsAD(pPacket, 0x49, 0x0);
    sceVif1PkAddGsAD(pPacket, SCE_GS_ALPHA_1, 0x44);
    _closeVifGif(pPacket);
    _openVifGif(pPacket, GS_PRIM_TRISTRIP, 5u, RG_DRAW_REGS_COLOR_4_XYZ);
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
        assert_prog(drawAssertDrawNotNil, drawSourcePath, 796);
    }
    if (pDraw->m_Enabled == 0) {
        return;
    }
    for (priority = 0; priority < RG_DRAW_PRIORITY_COUNT; priority++) {
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
            case RG_DRAW_ID_ALL_STUDIOS: {
                unsigned int screenIndex;
                for (screenIndex = 0; screenIndex < RG_DRAW_STUDIO_COUNT; screenIndex++) {
                    studio = pDraw->m_pStudios[screenIndex];
                    if (studio != 0) {
                        request->drawFunc(request->pObject, studio);
                    }
                }
                break;
            }
            case RG_DRAW_ID_GLOBAL:
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
                RgError(drawUnknownDrawIdFormat, drawSourcePath, 776, request->drawID);
            }
        }
    }
}

void RgDrawJob(RgDraw *pDraw)
{
    if (pDraw == 0) {
        assert_prog(drawAssertDrawNotNil, drawSourcePath, 834);
    }
    _SetGlobalLight();
    _DrawMain(pDraw);
    _DrawReqTerminate(pDraw);
}

void RgDrawFadeIn(RgDraw *pDraw)
{
    int nextFadeCounter;

    if (pDraw == 0) {
        assert_prog(drawAssertDrawNotNil, drawSourcePath, 856);
    }
    if (pDraw->m_Enabled != 0) {
        while (pDraw->m_FadeCounter < RG_DRAW_FADE_LIMIT) {
            nextFadeCounter = pDraw->m_FadeCounter - RG_DRAW_FADE_STEP;
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
        assert_prog(drawAssertDrawNotNil, drawSourcePath, 876);
    }
}



const GifTagBlock drawFadeShadowVertexBlock = {
    {
        { { 0x0000000000000000ULL, 0x0000007F00040000ULL } },
        { { 0x0000000000000FFFULL, 0x0000007F00040000ULL } },
        { { 0x00000FFF00000000ULL, 0x0000007F00040000ULL } },
        { { 0x00000FFF00000FFFULL, 0x0000007F00040000ULL } }
    }
};
