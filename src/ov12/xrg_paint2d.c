/*
 * OV12 original TU 86: 0x00a4a620..0x00a4c840 (42 functions)
 */
#include "common.h"
#include "shared.h"
#include "ov12/xrg_paint2d.h"
#include "ov12/rg_draw.h"
#include "ov12/rg_singleton_id.h"
#include "ov12/xrg_rand_int.h"
#include "main/xgl_packet.h"
#include "vu0_macro.h"
#include "main/nml_model_set.h"
#include "ov12/xrg_bxx_ps2.h"
#include "ov12/rg_heap.h"

/* Largest vertex coordinate _CalcRectangle lets the far corners reach. */
#define XRG_PAINT2D_COORD_MAX 0x3FFF

/*
 * XrgPaint2DRect.mode low four bits: one 2-bit anchor selector per axis
 * (_Geom2D): keep the corner, subtract half the extent, subtract the extent.
 */
#define XRG_PAINT2D_ANCHOR_CORNER 0
#define XRG_PAINT2D_ANCHOR_HALF 1
#define XRG_PAINT2D_ANCHOR_FULL 2

/*
 * XrgPaint2DDrawReq.offsetMode: the low two bits pick how _CalcOffset derives
 * the screen position; the other bits are flags.
 */
#define XRG_PAINT2D_OFFSET_HOME 0   /* the screen home position */
#define XRG_PAINT2D_OFFSET_2D 1     /* home plus a 2D offset */
#define XRG_PAINT2D_OFFSET_3D 2     /* project a 3D point onto the screen */
#define XRG_PAINT2D_OFFSET_3D_FORCE 3 /* 3D point clamped to a radius */
#define XRG_PAINT2D_OFFSET_KIND_MASK 3
#define XRG_PAINT2D_OFFSET_LINEAR 8   /* average with the explicit linear pair */
#define XRG_PAINT2D_OFFSET_OVERRIDE 0x10 /* offsetResult.i.y is an override block */

/* XrgPaint2DDrawReq.uvMode bits set by the explicit UV setters. */
#define XRG_PAINT2D_UV_OFFSET 1
#define XRG_PAINT2D_UV_SIZE 2

/*
 * What a geometry callback returns and the draw callback receives (flags & 3
 * as the primitive): 0 nothing to draw, 1 a two-corner sprite, 2 a
 * four-corner strip, 6 (strip plus the depth-test bit) a projected line.
 */
#define XRG_PAINT2D_GEOM_SPRITE 1
#define XRG_PAINT2D_GEOM_QUAD 2
#define XRG_PAINT2D_GEOM_PRIMITIVE_MASK 3
#define XRG_PAINT2D_GEOM_DEPTH_TEST 4
#define XRG_PAINT2D_GEOM_LINE (XRG_PAINT2D_GEOM_QUAD | XRG_PAINT2D_GEOM_DEPTH_TEST)

/* The request ring holds at most this many requests. */
#define XRG_PAINT2D_MAX_REQUESTS 255

/* Packed GIF register descriptors of the vertex packets. */
#define XRG_PAINT2D_REGS_COLOR_4_XYZ 0x44441
#define XRG_PAINT2D_REGS_COLOR_2_UV_XYZ 0x43431
#define XRG_PAINT2D_REGS_COLOR_4_UV_XYZ 0x434343431ULL

/* GS TEST_1 values: depth test always passes, or only greater passes. */
#define XRG_PAINT2D_TEST_ZALWAYS 0x31001
#define XRG_PAINT2D_TEST_ZGREATER 0x71001

#define XRG_PAINT2D_VIF_FLUSHA 0x13000000
#define XRG_PAINT2D_COLOR_HALF 0x7F

/*
 * The 128-bit VIF1 GIF tag sceVif1PkOpenGifTag takes in the single register
 * the original loads with one `lq`; no wide arithmetic is done on it: bit 15
 * is EOP, bit 46 is PRE, bits 47..57 are PRIM, bits 60..63 are NREG, and the
 * high 64 bits are the REGS descriptor. The union lets the two halves be
 * stored as ordinary 64-bit fields and the whole 16 bytes be read back as the
 * single register sceVif1PkOpenGifTag takes.
 */

typedef union XrgPaint2DGifTag {
    struct {
        u64 lo;
        u64 hi;
    } part;
    Quadword quad;
} XrgPaint2DGifTag;

/*
 * Partial view of main's DispEnv: the display bitfields at +0x18 (magh only
 * is read here).
 */
extern struct XrgPaint2DDispEnvView {
    u64 flags;
    unsigned char unmodeled_08[0x10];
    struct {
        unsigned int dx : 12;
        unsigned int dy : 11;
        unsigned int magh : 4;
        unsigned int magv : 2;
        unsigned int unmodeled_29 : 3;
        unsigned int dw : 12;
        unsigned int dh : 11;
        unsigned int unmodeled_55 : 9;
    } display;
} DispEnv;

/* The current view and its screen metrics, set by _Paint2DFlush. */
static RgDrawView *s_pCurView_00A51220;
static int s_anHomePos[2];
static float s_aSizeRatio[2];
static RgVector s_aCurCenterPos;

/* Scaffold-owned strings. */
extern const char paint2dUnknownOffsetModeFormat[];
extern const char paint2dUnknownGeomModeFormat[];
extern const char paint2dAssertRequestCountBelowMax[];
extern const char paint2dAssertViewNotNil[];
/* Defined here: paint2dAssertPicNotNil ("pPic != NIL"), further down. */
extern const char paint2dAssertPicNotNil[];

static int _Geom2D(XrgPaint2DDrawReq *req, XrgPaint2DVertex *corners);
static int _Line3D(XrgPaint2DDrawReq *req, XrgPaint2DVertex *corners);
static void _DrawNoTex(XrgPaint2DDrawReq *req, XglPacket *packet,
                       XrgPaint2DVertex *corners, int primitive);
static void _DrawWithTex(XrgPaint2DDrawReq *req, XglPacket *packet,
                         XrgPaint2DVertex *corners, int primitive);
static void _InitReq(XrgPaint2DDrawReq *req);
static void _Paint2DFlush(void *pObject, RgDrawStudio *pStudio);
static void _Paint2DClearReq(void *pObject);

static void _openVifGif(XglPacket *packet, u64 prim,
                        unsigned int nreg, u64 regs)
{
    XrgPaint2DGifTag tag;

    tag.part.lo = (1u << 15) | (1ULL << 46) | ((u64)prim << 47) |
                  ((u64)nreg << 60);
    tag.part.hi = regs;
    sceVif1PkAlign(packet, 2, 3);
    sceVif1PkCnt(packet, 0);
    sceVif1PkOpenDirectHLCode(packet, 0);
    sceVif1PkOpenGifTag(packet, tag.quad);
}

/*
 * ov12:0x00a59030 is a fixed GIF tag: EOP set, NREG 1, REGS 0xE (the A+D
 * address+data register code), the constant _openVifGifAD passes to
 * sceVif1PkOpenGifTag.
 */
const XrgPaint2DGifTag paint2dGifTag = {
    .part = {0x1000000000008000ULL, 0xE}
};

const char paint2dSourcePath[24] = "../xrg_paint2d.euc.c";

const char paint2dAssertPaintNotNil[24] = "pPaint != NIL";

static void _openVifGifAD(XglPacket *packet)
{
    XrgPaint2DGifTag tag;

    tag = paint2dGifTag;
    sceVif1PkCnt(packet, 0);
    sceVif1PkAlign(packet, 2, 3);
    sceVif1PkOpenDirectCode(packet, 0);
    sceVif1PkOpenGifTag(packet, tag.quad);
}

/*
 * The VIF1 direct/HL packet the paint renderer's draw calls build (_DrawNoTex,
 * _DrawWithTex, _Paint2DFlush, all outside this allocation) is closed through
 * this pair, both of which take the packet itself: the caller keeps its
 * argument live across both calls (a0 into s0) rather than discarding it
 * after the first.
 */
static void _closeVifGif(XglPacket *packet) {
    sceVif1PkCloseGifTag(packet);
    sceVif1PkCloseDirectHLCode(packet);
}

static void _InitReq(XrgPaint2DDrawReq *req)
{
    req->geom = _Geom2D;
    req->draw = _DrawNoTex;
    req->rectQuad = 0;
    req->rectScaleAngle.whole = 0;
    req->lineWidth.whole = 0;
    req->offsetAnchor.whole = 0;
    req->offsetResult.whole = 0;
    req->mode = 0;
    req->color[3] = XRG_PAINT2D_COLOR_HALF;
    req->color[2] = XRG_PAINT2D_COLOR_HALF;
    req->color[1] = XRG_PAINT2D_COLOR_HALF;
    req->color[0] = XRG_PAINT2D_COLOR_HALF;
    req->alpha = 0;
    req->tex0 = 0;
    req->texture = 0;
    req->offsetMode = 0;
    req->uvMode = 0;
}

static void _DeriveInfoReq(XrgPaint2DDrawReq *dst, XrgPaint2DDrawReq *src)
{
    dst->offsetMode = src->offsetMode;
    dst->offsetAnchor = src->offsetAnchor;
    dst->offsetResult = src->offsetResult;
}

static void _CalcRectangle(XrgPaint2DVertex corners[4], const XrgIntRect *rect,
                           int color, int prio)
{
    corners[0].x = rect->x;
    corners[0].y = rect->y;
    corners[1].x = rect->x + rect->width;
    corners[1].y = rect->y;
    corners[2].x = rect->x;
    corners[2].y = rect->y + rect->height;
    corners[3].x = rect->x + rect->width;
    corners[3].y = rect->y + rect->height;

    if (corners[2].x >= XRG_PAINT2D_COORD_MAX)
        corners[2].x = XRG_PAINT2D_COORD_MAX;
    if (corners[2].y >= XRG_PAINT2D_COORD_MAX)
        corners[2].y = XRG_PAINT2D_COORD_MAX;
    if (corners[3].x >= XRG_PAINT2D_COORD_MAX)
        corners[3].x = XRG_PAINT2D_COORD_MAX;
    if (corners[3].y >= XRG_PAINT2D_COORD_MAX)
        corners[3].y = XRG_PAINT2D_COORD_MAX;

    corners[3].color = color;
    corners[2].color = color;
    corners[1].color = color;
    corners[0].color = color;
    corners[3].prio = prio;
    corners[2].prio = prio;
    corners[1].prio = prio;
    corners[0].prio = prio;
}

/*
 * _CalcUV expands one rectangle into the four vertices _CalcRectangle
 * builds, after applying up to two independent overrides from a second
 * rectangle-shaped argument: bit 0 of `mode` adds its x/y to the base
 * rectangle's x/y, bit 1 replaces the base's width/height with its own.
 * The base rectangle is brought in as one 16-byte COP2 quadword transfer
 * (lqc2/sqc2), the same admitted idiom XrgPaint2DOffset3D and
 * XrgPaint2DColor use elsewhere in this allocation.
 */
static void _CalcUV(XrgPaint2DVertex corners[4], const XrgIntRect *base,
                    int mode, const XrgIntRect *override)
{
    XrgIntRect rect;

    __asm__ __volatile__("lqc2 vf31, 0(%0)\n\tsqc2 vf31, 0(%1)"
                         : : "r"(base), "r"(&rect) : "memory");
    if (mode & 1) {
        rect.x += override->x;
        rect.y += override->y;
    }
    if (mode & 2) {
        rect.width = override->width;
        rect.height = override->height;
    }
    _CalcRectangle(corners, &rect, 0, 0);
}

/*
 * Both projections are whole VU0 sequences of the original: load the current
 * view (matrix, then screen scale and translation), then transform a point.
 */
static int _CalcPosOnScreen(XrgPaint2DOffset *screen, const XrgPaint2DOffset *point,
                            XrgPaint2DOffset *scaledAxisX)
{
    int clip;

    Vu0SetCurrentMatrix(s_pCurView_00A51220->viewMatrix);
    Vu0SetViewScaleTranslation(s_pCurView_00A51220->axisX, s_pCurView_00A51220->axisY);
    __asm__ __volatile__("lqc2 vf31, 0(%0)\n\tsqc2 vf31, 0(%1)"
                         : : "r"(s_pCurView_00A51220->axisX), "r"(scaledAxisX) : "memory");
    scaledAxisX->f.x *= 0.375f;
    scaledAxisX->f.y *= 0.375f;
    __asm__ __volatile__(
        "ctc2 $0, $vi18\n"
        "lqc2 vf31, 0(%2)\n"
        "vmulax.xyzw ACC, vf27, vf31x\n"
        "vmadday.xyzw ACC, vf28, vf31y\n"
        "vmaddaz.xyzw ACC, vf29, vf31z\n"
        "vmaddw.xyzw vf31, vf30, vf0w\n"
        "nop\n"
        "nop\n"
        "nop\n"
        "vclipw.xyz vf31, vf31w\n"
        "vdiv Q, vf0w, vf31w\n"
        "vwaitq\n"
        "vmulq.xyzw vf31, vf31, Q\n"
        "vmulaw.xyzw ACC, vf26, vf0w\n"
        "vmadd.xyzw vf31, vf31, vf25\n"
        "vftoi4.xyzw vf20, vf31\n"
        "vsub.w vf20, vf20, vf20\n"
        "sqc2 vf20, 0(%1)\n"
        "cfc2 $2, $vi18\n"
        "nop\n"
        "andi $2, $2, 0x3f\n"
        "move %0, $2\n"
        : "=r"(clip)
        : "r"(screen), "r"(point)
        : "$2", "memory");
    return clip == 0;
}

static void _CalcPosOnScreenFloat(XrgPaint2DOffset *screen, const XrgPaint2DOffset *point)
{
    Vu0SetCurrentMatrix(s_pCurView_00A51220->viewMatrix);
    Vu0SetViewScaleTranslation(s_pCurView_00A51220->axisX, s_pCurView_00A51220->axisY);
    __asm__ __volatile__(
        "lqc2 vf31, 0(%1)\n"
        "vmulax.xyzw ACC, vf27, vf31x\n"
        "vmadday.xyzw ACC, vf28, vf31y\n"
        "vmaddaz.xyzw ACC, vf29, vf31z\n"
        "vmaddw.xyzw vf31, vf30, vf0w\n"
        "nop\n"
        "nop\n"
        "nop\n"
        "vclipw.xyz vf31, vf31w\n"
        "vdiv Q, vf0w, vf31w\n"
        "vwaitq\n"
        "vmulq.xyzw vf31, vf31, Q\n"
        "vmulaw.xyzw ACC, vf26, vf0w\n"
        "vmadd.xyzw vf31, vf31, vf25\n"
        "vsub.w vf31, vf31, vf31\n"
        "sqc2 vf31, 0(%0)\n"
        : : "r"(screen), "r"(point) : "$2", "memory");
}


static int _CalcOffset(XrgPaint2DOffset *out, int mode,
                       const XrgPaint2DOffset *anchor,
                       XrgPaint2DOffset *result)
{
    XrgPaint2DOffset anchorCopy;
    XrgPaint2DOffset resultCopy;
    XrgPaint2DOffsetOverride *override;
    XrgPaint2DOffset screen;
    XrgPaint2DOffset dir;
    XrgPaint2DOffset scaledAxisX;
    int flag;
    float length;
    float scale;


    flag = 1;
    override = 0;
    anchorCopy = *anchor;
    resultCopy = *result;
    result = &resultCopy;
    override = (mode & XRG_PAINT2D_OFFSET_OVERRIDE) ? (XrgPaint2DOffsetOverride *) resultCopy.i.y : override;
    switch ((unsigned int) mode & XRG_PAINT2D_OFFSET_KIND_MASK) {
    case XRG_PAINT2D_OFFSET_HOME:
        out->i.x = s_anHomePos[0];
        out->i.y = s_anHomePos[1];
        break;
    case XRG_PAINT2D_OFFSET_2D:
        out->i.x = s_anHomePos[0] + anchorCopy.i.x;
        out->i.y = s_anHomePos[1] + anchorCopy.i.y;
        break;
    case XRG_PAINT2D_OFFSET_3D:
        flag = _CalcPosOnScreen(&screen, &anchorCopy, &scaledAxisX);
        out->i.x = screen.i.x;
        out->i.y = screen.i.y;
        break;
    case XRG_PAINT2D_OFFSET_3D_FORCE:
        _CalcPosOnScreenFloat(&screen, &anchorCopy);
        if (0.0f > screen.f.z) {
            screen.f.x = (s_aCurCenterPos[0] + s_aCurCenterPos[0]) - screen.f.x;
            screen.f.y = (s_aCurCenterPos[1] + s_aCurCenterPos[1]) - screen.f.y;
        }
        XrgSubVectorXYZ((float *) &dir, (float *) &screen, s_aCurCenterPos);
        dir.i.z = 0;
        dir.i.w = 0;
        length = XrgNormalizeVector((float *) &dir, (float *) &dir);
        result->f.x = scale = result->f.x * s_aSizeRatio[0];
        if (scale < length) {
            dir.f.x = dir.f.x * scale;
            dir.f.y = dir.f.y * scale;
            dir.f.z = dir.f.z * scale;
            dir.f.w = dir.f.w * scale;
            screen.f.x = s_aCurCenterPos[0] + dir.f.x;
            screen.f.y = s_aCurCenterPos[1] + dir.f.y;
            if (override) {
                override->flag = flag;
            }
        } else {
            if (override) {
                override->flag = 0;
            }
        }
        __asm__ __volatile__(
            "lqc2 vf11, 0(%0)\n\t"
            "vftoi4.xyzw vf11, vf11\n\t"
            "sqc2 vf11, 0(%0)"
            : : "r"(&screen) : "memory");
        out->i.x = screen.i.x;
        out->i.y = screen.i.y;
        if (override) {
            __asm__ __volatile__(
                "lqc2 vf31, 0(%0)\n\t"
                "sqc2 vf31, 0(%1)"
                : : "r"(&dir), "r"(&override->direction) : "memory");
        }
        break;
    default:
        RgError(paint2dUnknownOffsetModeFormat, paint2dSourcePath, 385, mode);
    }
    if (override) {
        override->dx = out->i.x - s_anHomePos[0];
        override->dy = out->i.y - s_anHomePos[1];
        if (mode & XRG_PAINT2D_OFFSET_LINEAR) {
            int nx;

            nx = (result->i.z + override->dx) / 2;
            override->cx = nx;
            override->cy = (result->i.w + override->dy) / 2;

            out->i.x = nx + s_anHomePos[0];
            out->i.y = override->cy + s_anHomePos[1];
        } else {
            override->cx = out->i.x - s_anHomePos[0];
            override->cy = out->i.y - s_anHomePos[1];
        }
    }
    return flag;
}

static int _Geom2D(XrgPaint2DDrawReq *req, XrgPaint2DVertex *corners)
{
    struct {
        float box[4];
        XrgPaint2DOffset offset;
        XrgPaint2DOffset quad[4];
        RgMatrix matrix;
    } w;
    int status;
    int mode;
    unsigned int bits;
    int i;
    float x1;
    float y1;
    XrgPaint2DVertex *corner;
    int color = 0x40000;
    int prio = 0x7F;

    mode = req->mode;
    status = XRG_PAINT2D_GEOM_SPRITE;
    if (!_CalcOffset(&w.offset, req->offsetMode, &req->offsetAnchor, &req->offsetResult)) {
        return 0;
    }
    if ((mode & XRG_PAINT2D_MODE_PIC_SIZE) && req->tex0 != 0) {
        w.box[2] = (float) req->uHigh;
        w.box[3] = (float) req->vHigh;
    } else {
        w.box[2] = (float) (((XrgIntRect *) &req->rectQuad)->width << 4);
        w.box[3] = (float) (((XrgIntRect *) &req->rectQuad)->height << 4);
    }
    if (mode & XRG_PAINT2D_MODE_SCALE) {
        w.box[2] = w.box[2] * req->rectScaleAngle.scaleAngle.scale;
        w.box[3] = w.box[3] * req->rectScaleAngle.scaleAngle.scale;
    }
    w.box[0] = (float) (((XrgIntRect *) &req->rectQuad)->x << 4);
    w.box[1] = (float) (((XrgIntRect *) &req->rectQuad)->y << 4);
    bits = mode;
    for (i = 0; i < 2; i++) {
        unsigned int action = bits & 3;
        float edge = w.box[i];

        bits = bits >> 2;
        switch (action) {
        case XRG_PAINT2D_ANCHOR_CORNER:
            w.box[i] = edge;
            break;
        case XRG_PAINT2D_ANCHOR_HALF:
            w.box[i] = edge - w.box[i + 2] * 0.5f;
            break;
        case XRG_PAINT2D_ANCHOR_FULL:
            w.box[i] = edge - w.box[i + 2];
            break;
        default:
            RgError(paint2dUnknownGeomModeFormat, paint2dSourcePath, 454, mode);
            break;
        }
    }
    *(Quadword *) &w.quad[3] = 0;
    *(Quadword *) &w.quad[2] = 0;
    *(Quadword *) &w.quad[1] = 0;
    *(Quadword *) &w.quad[0] = 0;
    w.quad[2].f.x = w.box[0];
    w.quad[1].f.y = w.box[1];
    w.quad[0].f.x = w.box[0];
    w.quad[0].f.y = w.box[1];
    x1 = w.box[0] + w.box[2];
    w.quad[1].f.x = x1;
    w.quad[3].f.x = x1;
    y1 = w.box[1] + w.box[3];
    w.quad[2].f.y = y1;
    w.quad[3].f.y = y1;
    if (mode & XRG_PAINT2D_MODE_ANGLE) {
        XrgUnitMatrix(w.matrix);
        XrgRotMatrixZ(w.matrix, w.matrix, req->rectScaleAngle.scaleAngle.angle);
        for (i = 0; i < 4; i++) {
            __asm__ __volatile__(
                "lqc2 vf2, 0(%0)\n\t"
                "lqc2 vf3, 0(%1)\n\t"
                "lqc2 vf4, 16(%1)\n\t"
                "lqc2 vf5, 32(%1)\n\t"
                "lqc2 vf6, 48(%1)\n\t"
                "vmulax.xyzw ACC, vf3, vf2x\n\t"
                "vmadday.xyzw ACC, vf4, vf2y\n\t"
                "vmaddaz.xyzw ACC, vf5, vf2z\n\t"
                "vmaddw.xyzw vf2, vf6, vf2w\n\t"
                "sqc2 vf2, 0(%0)"
                : : "r"(&w.quad[i].f.x), "r"(w.matrix) : "memory");
        }
        status = XRG_PAINT2D_GEOM_QUAD;
    }
    if (mode & XRG_PAINT2D_MODE_JITTER) {
        float scale = 12.272727f / (54.0f / (float) (DispEnv.display.magh + 1));

        float *py = &w.quad[0].f.y;

        for (i = 0; i < 4; i++, py += 4) {
            *py = *py * scale;
        }
    }
    for (i = 0; i < 4; i++) {
        __asm__ __volatile__(
            "lqc2 vf11, 0(%0)\n\t"
            "vftoi0.xyzw vf11, vf11\n\t"
            "sqc2 vf11, 0(%1)"
            : : "r"(&w.quad[i].f.x), "r"(&corners[i]) : "memory");
    }
    for (i = 0; i < 4; i++) {
        corners[i].x += w.offset.i.x;
        corners[i].y += w.offset.i.y;
    }
    corners[3].color = color;
    corners[2].color = color;
    corners[1].color = color;
    corners[0].color = color;
    corners[3].prio = prio;
    corners[2].prio = prio;
    corners[1].prio = prio;
    corners[0].prio = prio;
    return status;
}

static int _Line3D(XrgPaint2DDrawReq *req, XrgPaint2DVertex *corners)
{
    XrgPaint2DOffset screen[2];
    int flags[2];
    unsigned int i;
    XrgPaint2DOffset work[4];
    XrgPaint2DOffset normal;
    XrgPaint2DOffset delta;
    XrgPaint2DOffset offset[2];
    int idx[4];
    XrgPaint2DOffset *point;
    XrgPaint2DOffset *scr = screen;
    XrgPaint2DOffset *dst;
    XrgPaint2DOffset *quad;
    XrgPaint2DOffset *out;
    int dy0;
    int dx0;
    float halfWidth;
    int *flag;
    int off;

    point = (XrgPaint2DOffset *) &req->rectQuad;
    dst = quad = work + 2;
    i = 0;
    flag = flags;
    off = 0;
    for (i = 0; i < 2; i++) {
        int visible;
        visible = _CalcPosOnScreen(scr, point, (XrgPaint2DOffset *) ((char *) work + off));

        do {
            dst->f.z = 0;
        } while (0);
        halfWidth = req->lineWidth.halfWidth;
        *flag = visible;
        dst->f.y = halfWidth;
        dst->f.x = halfWidth;
        __asm__ __volatile__(
            "lqc2 vf11, 0(%0)\n\t"
            "vmulq.xyzw vf11, vf11, Q\n\t"
            "sqc2 vf11, 0(%0)"
            : : "r"(quad) : "memory");
        quad->f.x = quad->f.x * quad[-2].f.x;
        quad->f.y = quad->f.y * quad[-2].f.y;
        quad->f.z = quad->f.z * quad[-2].f.z;
        quad->f.w = quad->f.w * quad[-2].f.w;
        scr++;
        dst++;
        quad++;
        flag++;
        point++;
        off += 16;
    }
    if (flags[0] && flags[1]) {
        if (req->mode & 1) {
            for (i = 0; i < 4; i++) {
                idx[i] = (i + 1) & 3;
            }
        } else {
            for (i = 0; i < 4; i++) {
                idx[i] = i;
            }
        }
        ((Quadword *) &delta)[0] = 0;
        delta.i.x = screen[0].i.x - screen[1].i.x;
        delta.i.y = screen[0].i.y - screen[1].i.y;
        __asm__ __volatile__(
            "lqc2 vf11, 0(%0)\n\t"
            "vitof4.xyzw vf11, vf11\n\t"
            "sqc2 vf11, 0(%1)"
            : : "r"(&delta), "r"(&delta) : "memory");
        normal.f.x = -delta.f.y;
        normal.f.y = delta.f.x;
        normal.i.z = 0;
        XrgNormalizeVector((float *) &normal, (float *) &normal);
        for (i = 0, dst = work + 2, out = offset; i < 2; i++) {
            float len;

            len = XrgLengthVector((float *) dst);
            out->f.x = normal.f.x * len;
            out->f.y = normal.f.y * len;
            out->f.z = normal.f.z * len;
            out->f.w = normal.f.w * len;
            __asm__ __volatile__(
                "lqc2 vf11, 0(%0)\n\t"
                "vftoi4.xyzw vf11, vf11\n\t"
                "sqc2 vf11, 0(%0)"
                : : "r"(out) : "memory");
            out++;
            dst++;
        }
        __asm__ __volatile__(
            "lqc2 vf31, 0(%0)\n\t"
            "sqc2 vf31, 0(%1)"
            : : "r"(&screen[0]), "r"((idx[0] * 16 + (int) corners)) : "memory");
        __asm__ __volatile__(
            "lqc2 vf31, 0(%0)\n\t"
            "sqc2 vf31, 0(%1)"
            : : "r"(&screen[0]), "r"((idx[1] * 16 + (int) corners)) : "memory");
        __asm__ __volatile__(
            "lqc2 vf31, 0(%0)\n\t"
            "sqc2 vf31, 0(%1)"
            : : "r"(&screen[1]), "r"((idx[2] * 16 + (int) corners)) : "memory");
        __asm__ __volatile__(
            "lqc2 vf31, 0(%0)\n\t"
            "sqc2 vf31, 0(%1)"
            : : "r"(&screen[1]), "r"((idx[3] * 16 + (int) corners)) : "memory");
        dx0 = offset[0].i.x;
        dy0 = offset[0].i.y;
        {
            int *x = (int *) (idx[0] * 16 + (int) corners);
            int *y = x + 1;
            x[0] += dx0;
            y[0] += dy0;
        }
        {
            int *x = (int *) (idx[1] * 16 + (int) corners);
            int *y = x + 1;
            x[0] -= dx0;
            y[0] -= dy0;
        }
        {
            int *x = (int *) (idx[2] * 16 + (int) corners);
            int *y = x + 1;
            x[0] += offset[1].i.x;
            y[0] += offset[1].i.y;
        }
        {
            int *x = (int *) (idx[3] * 16 + (int) corners);
            int *y = x + 1;
            x[0] -= offset[1].i.x;
            y[0] -= offset[1].i.y;
        }
        return XRG_PAINT2D_GEOM_LINE;
    }
    return 0;
}

static void _DrawNoTex(XrgPaint2DDrawReq *req, XglPacket *packet,
                       XrgPaint2DVertex *corners, int primitive)
{
    Quadword color;
    Quadword *vertices;
    unsigned int prim;

    vertices = (Quadword *)corners;
    prim = GS_PRIM_TRISTRIP;
    __asm__ __volatile__(
        "lqc2 vf31, 0(%0)\n\t"
        "sqc2 vf31, 0(%1)"
        : : "r"(req->color), "r"(&color) : "memory");
    if (req->alpha != 0) {
        _openVifGifAD(packet);
        sceVif1PkAddGsAD(packet, SCE_GS_ALPHA_1, req->alpha);
        _closeVifGif(packet);
        prim = GS_PRIM_TRISTRIP | GS_PRIM_ABE;
    }
    _openVifGif(packet, prim, 5, XRG_PAINT2D_REGS_COLOR_4_XYZ);
    sceVif1PkAddGsPacked(packet, color);
    sceVif1PkAddGsPacked(packet, vertices[0]);
    sceVif1PkAddGsPacked(packet, vertices[1]);
    sceVif1PkAddGsPacked(packet, vertices[2]);
    sceVif1PkAddGsPacked(packet, vertices[3]);
    _closeVifGif(packet);
}

static void _DrawWithTex(XrgPaint2DDrawReq *req, XglPacket *packet,
                         XrgPaint2DVertex *corners, int primitive)
{
    struct {
        Quadword color;
        XrgPaint2DVertex uvs[4];
    } frame;
    Quadword *xyz;
    int alphaFlag;
    unsigned long long prim;

    prim = GS_PRIM_TRISTRIP;
    alphaFlag = 0;
    _CalcUV(frame.uvs, (const XrgIntRect *) &req->uLow, req->uvMode,
            (const XrgIntRect *) &req->uOffset);
    __asm__ __volatile__(
        "lqc2 vf31, 0(%0)\n\t"
        "sqc2 vf31, 0(%1)"
        : : "r"(req->color), "r"(&frame.color) : "memory");
    if (primitive == XRG_PAINT2D_GEOM_SPRITE) {
        prim = GS_PRIM_SPRITE;
    }
    if (req->alpha != 0) {
        _openVifGifAD(packet);
        sceVif1PkAddGsAD(packet, SCE_GS_ALPHA_1, req->alpha);
        _closeVifGif(packet);
        alphaFlag = 1;
    }
    xyz = (Quadword *) corners;
    prim = prim | (alphaFlag << 6) | GS_PRIM_TME | GS_PRIM_FST;
    _openVifGifAD(packet);
    sceVif1PkAddGsAD(packet, SCE_GS_TEX0_1, req->tex0);
    _closeVifGif(packet);
    if (primitive == XRG_PAINT2D_GEOM_SPRITE) {
        _openVifGif(packet, prim, 5, XRG_PAINT2D_REGS_COLOR_2_UV_XYZ);
        sceVif1PkAddGsPacked(packet, frame.color);
        sceVif1PkAddGsPacked(packet, ((Quadword *) frame.uvs)[0]);
        sceVif1PkAddGsPacked(packet, xyz[0]);
        sceVif1PkAddGsPacked(packet, ((Quadword *) frame.uvs)[3]);
        sceVif1PkAddGsPacked(packet, xyz[3]);
        _closeVifGif(packet);
        return;
    }
    _openVifGif(packet, prim, 9, XRG_PAINT2D_REGS_COLOR_4_UV_XYZ);
    sceVif1PkAddGsPacked(packet, frame.color);
    sceVif1PkAddGsPacked(packet, ((Quadword *) frame.uvs)[0]);
    sceVif1PkAddGsPacked(packet, xyz[0]);
    sceVif1PkAddGsPacked(packet, ((Quadword *) frame.uvs)[1]);
    sceVif1PkAddGsPacked(packet, xyz[1]);
    sceVif1PkAddGsPacked(packet, ((Quadword *) frame.uvs)[2]);
    sceVif1PkAddGsPacked(packet, xyz[2]);
    sceVif1PkAddGsPacked(packet, ((Quadword *) frame.uvs)[3]);
    sceVif1PkAddGsPacked(packet, xyz[3]);
    _closeVifGif(packet);
}

static void _InitPaint(XrgPaint2D *paint, const char *sourceFile, int line)
{
    if (paint == 0) {
        assert_prog(paint2dAssertPaintNotNil, paint2dSourcePath, 701);
    }
    strcpy(paint->sourceFile, sourceFile);
    paint->line = line;
    paint->drawID = -2;
    paint->reqCount = 0;
    paint->request = &paint->req;
    paint->prio = 3;
    _InitReq(&paint->req);
}

static void _DestructPaint(XrgPaint2D *paint) {
    if (paint == 0) {
        assert_prog(paint2dAssertPaintNotNil, paint2dSourcePath, 713);
    }
}

XrgPaint2D *CreateXrgPaint2D_sub(const char *sourceFile, int line) {
    XrgPaint2D *paint;

    paint = RgHeapAlloc(InstanceOfRgHeap(), XRG_PAINT2D_SIZE, paint2dSourcePath, 720);
    _InitPaint(paint, sourceFile, line);
    return paint;
}

void DisposeXrgPaint2D_sub(XrgPaint2D *paint, const char *sourceFile, int line) {
    if (paint == 0) {
        assert_prog(paint2dAssertPaintNotNil, paint2dSourcePath, 727);
    }
    _DestructPaint(paint);
    RgHeapFree(InstanceOfRgHeap(), paint, sourceFile, line);
}

static void _WrapperDestruct(void *database) {
    DisposeXrgPaint2D_sub(database, paint2dSourcePath, 0x2DC);
}

XrgPaint2D *InstanceOfXrgPaint2D(void) {
    XrgPaint2D *paint;

    paint = RgSingletonIDGet(RG_SINGLETON_PAINT2D);
    if (paint == 0) {
        paint = CreateXrgPaint2D_sub(paint2dSourcePath, 742);
        XrgPaint2DSetDrawPrio(paint, 3);
        RgSingletonIDEntry(RG_SINGLETON_PAINT2D, paint, _WrapperDestruct);
    }
    return paint;
}

void XrgPaint2DSetDrawID(XrgPaint2D *paint, int drawID) {
    if (paint == 0) {
        assert_prog(paint2dAssertPaintNotNil, paint2dSourcePath, 757);
    }
    paint->drawID = drawID;
}

void XrgPaint2DSetDrawPrio(XrgPaint2D *paint, int prio) {
    if (paint == 0) {
        assert_prog(paint2dAssertPaintNotNil, paint2dSourcePath, 765);
    }
    paint->prio = prio;
}

/*
 * The three 2D anchor setters below store x through `req` but y through a
 * fresh paint->request load: the original reloads the request pointer after
 * the offsetMode store (lw -0x2FEC again before the +0x54 store), the same
 * reload XrgPaint2DOffset3DForce documents.
 *
 * XRG_PAINT2D_SUBPIXEL_SHIFT converts whole pixels into the GS's 12.4
 * subpixel units (x << 4); XRG_PAINT2D_SCREEN_CENTER_X/Y are the centre of
 * the 512x448 screen (RgDrawView's own default screenRect) in pixels.
 */
#define XRG_PAINT2D_SUBPIXEL_SHIFT 4
#define XRG_PAINT2D_SCREEN_CENTER_X (512 / 2)
#define XRG_PAINT2D_SCREEN_CENTER_Y (448 / 2)

void XrgPaint2DOffset2D(XrgPaint2D *paint, int x, int y)
{
    XrgPaint2DDrawReq *req;

    if (paint == 0) {
        assert_prog(paint2dAssertPaintNotNil, paint2dSourcePath, 777);
    }
    req = paint->request;
    req->offsetAnchor.i.x = x;
    req->offsetMode = XRG_PAINT2D_OFFSET_2D;
    paint->request->offsetAnchor.i.y = y;
}

void XrgPaint2DOffset2DDot(XrgPaint2D *paint, int x, int y)
{
    XrgPaint2DDrawReq *req;

    if (paint == 0) {
        assert_prog(paint2dAssertPaintNotNil, paint2dSourcePath, 786);
    }
    req = paint->request;
    req->offsetAnchor.i.x = x << XRG_PAINT2D_SUBPIXEL_SHIFT;
    req->offsetMode = XRG_PAINT2D_OFFSET_2D;
    paint->request->offsetAnchor.i.y = y << XRG_PAINT2D_SUBPIXEL_SHIFT;
}

void XrgPaint2DOffsetCenter(XrgPaint2D *paint)
{
    XrgPaint2DDrawReq *req;

    if (paint == 0) {
        assert_prog(paint2dAssertPaintNotNil, paint2dSourcePath, 795);
    }
    req = paint->request;
    req->offsetAnchor.i.x = XRG_PAINT2D_SCREEN_CENTER_X << XRG_PAINT2D_SUBPIXEL_SHIFT;
    req->offsetMode = XRG_PAINT2D_OFFSET_2D;
    paint->request->offsetAnchor.i.y = XRG_PAINT2D_SCREEN_CENTER_Y
                                       << XRG_PAINT2D_SUBPIXEL_SHIFT;
}

void XrgPaint2DOffset3D(XrgPaint2D *paint, const RgVector anchor)
{
    XrgPaint2DDrawReq *req;

    if (paint == 0) {
        assert_prog(paint2dAssertPaintNotNil, paint2dSourcePath, 804);
    }
    req = paint->request;
    req->offsetMode = XRG_PAINT2D_OFFSET_3D;
    __asm__ __volatile__("lqc2 vf31, 0(%0)\n\tsqc2 vf31, 0(%1)"
                         : : "r"(anchor), "r"(&req->offsetAnchor) : "memory");
}

/*
 * XrgPaint2DOffset3DForce (0x00a4bc18) re-reads paint->request for the final
 * store instead of reusing `req`: the original reloads it into a second
 * register there rather than keeping the first one live across the COP2
 * transfer.
 */
void XrgPaint2DOffset3DForce(XrgPaint2D *paint, const RgVector anchor,
                             float force)
{
    XrgPaint2DDrawReq *req;

    if (paint == 0) {
        assert_prog(paint2dAssertPaintNotNil, paint2dSourcePath, 812);
    }
    req = paint->request;
    req->offsetMode = XRG_PAINT2D_OFFSET_3D_FORCE;
    __asm__ __volatile__("lqc2 vf31, 0(%0)\n\tsqc2 vf31, 0(%1)"
                         : : "r"(anchor), "r"(&req->offsetAnchor) : "memory");
    paint->request->offsetResult.f.x = force;
}

/*
 * XrgPaint2DOffsetResult stores an explicit offset-result override into the
 * bound paint's draw request and flags it as such (offsetMode bit 0x10,
 * XrgPaint2DDrawReq above).
 */
void XrgPaint2DOffsetResult(XrgPaint2D *paint, XrgPaint2DOffsetOverride *value)
{
    XrgPaint2DDrawReq *req;

    if (paint == 0) {
        assert_prog(paint2dAssertPaintNotNil, paint2dSourcePath, 821);
    }
    req = paint->request;
    req->offsetResult.i.y = (int) value;
    req->offsetMode |= XRG_PAINT2D_OFFSET_OVERRIDE;
}

void XrgPaint2DOffsetLinear(XrgPaint2D *paint, int x, int y)
{
    XrgPaint2DDrawReq *request;

    request = paint->request;
    request->offsetResult.i.z = x;
    request->offsetMode |= XRG_PAINT2D_OFFSET_LINEAR;
    paint->request->offsetResult.i.w = y;
}

void XrgPaint2DColor(XrgPaint2D *paint, const XrgPaint2DRGBA *color)
{
    unsigned int *dst;

    if (paint == 0) {
        assert_prog(paint2dAssertPaintNotNil, paint2dSourcePath, 843);
    }
    dst = paint->request->color;
    __asm__ __volatile__("lqc2 vf31, 0(%0)" : : "r"(color) : "memory");
    __asm__ __volatile__("sqc2 vf31, 0(%0)" : : "r"(dst) : "memory");
}

/*
 * XrgPaint2DAlpha (0x00a4bd98) selects one of six fixed GS ALPHA_1-shaped
 * 64-bit blend-equation values by its own mode argument; an out-of-range
 * mode leaves the request's alpha at the zero this function seeds before
 * the switch (the same value case 4 also selects).
 */
void XrgPaint2DAlpha(XrgPaint2D *paint, unsigned int mode)
{
    long long alpha;

    alpha = 0;
    if (paint == 0) {
        assert_prog(paint2dAssertPaintNotNil, paint2dSourcePath, 852);
    }
    switch (mode) {
    case 0:
        alpha = 0x44;
        break;
    case 1:
        alpha = ((long long)0x80 << 32) | 0x68;
        break;
    case 3:
        alpha = 0x48;
        break;
    case 2:
        alpha = ((long long)0x60 << 32) | 0x62;
        break;
    case 4:
        alpha = 0;
        break;
    case 5:
        alpha = 0x42;
        break;
    }
    paint->request->alpha = alpha;
}

const char paint2dAssertPicNotNil[16] = "pPic != NIL";

void XrgPaint2DUseTexture(XrgPaint2D *paint, RgBxxPic *texture)
{
    XrgPaint2DDrawReq *req;
    long long tex0;

    if (paint == 0) {
        assert_prog(paint2dAssertPaintNotNil, paint2dSourcePath, 887);
    }
    if (texture == 0) {
        assert_prog(paint2dAssertPicNotNil, paint2dSourcePath, 888);
    }
    req = paint->request;
    RgBxxGetHeader(texture->pOwner);
    tex0 = XrgBxxPs2Tex0(texture);
    req->texture = texture;
    req->tex0 = tex0;
    req->draw = _DrawWithTex;
    req->uLow = texture->x * 0x10 + 8;
    req->vLow = texture->y * 0x10 + 8;
    req->uHigh = texture->width * 0x10 - 8;
    req->vHigh = texture->height * 0x10 - 8;
}

void XrgPaint2DSetUVOffset(XrgPaint2D *paint, int uOffset, int vOffset)
{
    int scaledUOffset;
    XrgPaint2DDrawReq *req;

    if (paint == 0) {
        assert_prog(paint2dAssertPaintNotNil, paint2dSourcePath, 913);
    }
    scaledUOffset = uOffset * 0x10;
    req = paint->request;
    req->vOffset = vOffset * 0x10;
    req->uOffset = scaledUOffset;
    req->uvMode |= XRG_PAINT2D_UV_OFFSET;
}

void XrgPaint2DSetUVSize(XrgPaint2D *paint, int width, int height)
{
    int uSize;
    XrgPaint2DDrawReq *req;

    if (paint == 0) {
        assert_prog(paint2dAssertPaintNotNil, paint2dSourcePath, 927);
    }
    uSize = width * 0x10;
    req = paint->request;
    req->vSize = height * 0x10;
    req->uSize = uSize;
    req->uvMode |= XRG_PAINT2D_UV_SIZE;
}

/*
 * InitXrgPaint2DRect clears the rectangle request consumed by the adjacent
 * XrgPaint2DDrawRect/XrgPaint2DDrawXYWH routines: an all-zero rectangle at the
 * screen origin, unrotated, at its natural size.
 */
void InitXrgPaint2DRect(XrgPaint2DRect *rectangle, int mode)
{
    rectangle->mode = mode;
    rectangle->scale = 1.0f;
    rectangle->height = 0;
    rectangle->width = 0;
    rectangle->y = 0;
    rectangle->x = 0;
    rectangle->angle = 0.0f;
}

void XrgPaint2DDrawRect(XrgPaint2D *paint, XrgPaint2DRect *rect)
{
    XrgPaint2DDrawReq *current;
    XrgPaint2DDrawReq *old;
    unsigned int count;

    if (paint == 0) {
        assert_prog(paint2dAssertPaintNotNil, paint2dSourcePath, 954);
    }
    if (paint->reqCount >= XRG_PAINT2D_MAX_REQUESTS) {
        assert_prog(paint2dAssertRequestCountBelowMax, paint2dSourcePath, 955);
    }
    current = paint->request;
    current->mode = rect->mode;
    __asm__ __volatile__(
        "lqc2 vf31, 0(%0)\n\t"
        "sqc2 vf31, 0(%1)"
        : : "r"(&rect->x), "r"(&current->rectQuad) : "memory");
    current->rectScaleAngle.scaleAngle.angle = rect->angle;
    current->rectScaleAngle.scaleAngle.scale = rect->scale;
    count = paint->reqCount + 1;
    old = paint->request;
    paint->reqCount = count;
    paint->request = &paint->req + count;
    _InitReq(paint->request);
    _DeriveInfoReq(paint->request, old);
}

void XrgPaint2DDrawXYWH(XrgPaint2D *paint, int mode, int x, int y,
                        int width, int height)
{
    XrgPaint2DRect rect;

    InitXrgPaint2DRect(&rect, mode);
    rect.x = x;
    rect.y = y;
    rect.width = width;
    rect.height = height;
    XrgPaint2DDrawRect(paint, &rect);
}

/* flags is not read: mode is always cleared (RgShotEffectDisp passes 1). */
void InitXrgPaint2DLine3D(XrgPaint2DLine3D *line, int flags, float width)
{
    line->mode = 0;
    XrgClearVector(line->start);
    XrgClearVector(line->end);
    line->width = width;
}

void XrgPaint2DDrawLine(XrgPaint2D *paint, XrgPaint2DLine3D *line)
{
    XrgPaint2DDrawReq *current;
    XrgPaint2DDrawReq *old;
    unsigned int count;

    if (paint == 0) {
        assert_prog(paint2dAssertPaintNotNil, paint2dSourcePath, 1004);
    }
    if (paint->reqCount >= XRG_PAINT2D_MAX_REQUESTS) {
        assert_prog(paint2dAssertRequestCountBelowMax, paint2dSourcePath, 1005);
    }
    current = paint->request;
    current->geom = _Line3D;
    current->mode = line->mode;
    __asm__ __volatile__(
        "lqc2 vf31, 0(%0)\n\t"
        "sqc2 vf31, 0(%1)"
        : : "r"(&line->start), "r"(&current->rectQuad) : "memory");
    __asm__ __volatile__(
        "lqc2 vf31, 0(%0)\n\t"
        "sqc2 vf31, 0(%1)"
        : : "r"(&line->end), "r"(&current->rectScaleAngle) : "memory");
    current->lineWidth.halfWidth = line->width * 0.5f;
    count = paint->reqCount + 1;
    old = paint->request;
    paint->reqCount = count;
    paint->request = &paint->req + count;
    _InitReq(paint->request);
    _DeriveInfoReq(paint->request, old);
}

static void _Paint2DFlush(void *pObject, RgDrawStudio *pStudio)
{
    XrgPaint2D *paint = pObject;
    unsigned char packetBuffer[0x20];
    XglPacket *packet;
    RgRect rect;
    XrgPaint2DVertex corners[4];
    int flags;
    int prevFlags;
    int prim;
    RgDrawView *view;
    XrgPaint2DDrawReq *req;
    void *xtx;
    void *prevXtx;
    unsigned int size;
    unsigned int i;
    int rectX0;
    int rectY1;
    int rectX1;

    prevFlags = 0;
    packet = (XglPacket *) packetBuffer;
    if (paint == 0) {
        assert_prog(paint2dAssertPaintNotNil, paint2dSourcePath, 1042);
    }
    if (paint->reqCount == 0) {
        return;
    }
    if (pStudio == 0) {
        return;
    }
    view = RgDrawStudioGetView(pStudio);
    if (view == 0) {
        assert_prog(paint2dAssertViewNotNil, paint2dSourcePath, 1052);
    }
    s_pCurView_00A51220 = view;
    RgDrawViewGetScreenRect(view, &rect);
    rectX0 = rect.x0;
    rectX1 = rect.x1;
    rectY1 = rect.y1;
    s_aSizeRatio[0] = (float) (rectX1 - rectX0) * 0.001953125f;
    s_aCurCenterPos[0] = 2048.0f + (float) (((rectX0 + rectX1) >> 1) - 256);
    s_aCurCenterPos[1] = 2048.0f + (float) (((rect.y0 + rectY1) >> 1) - 224);
    s_aSizeRatio[1] = (float) (rectY1 - rect.y0) / 448.0f;

    sceVif1PkInit(packet, SCRATCHPAD_BASE);
    sceVif1PkCnt(packet, 0);
    sceVif1PkAddCode(packet, XRG_PAINT2D_VIF_FLUSHA);
    sceVif1PkAlign(packet, 2, 3);
    _openVifGifAD(packet);
    sceVif1PkAddGsAD(packet, SCE_GS_TEXFLUSH, 0);
    sceVif1PkAddGsAD(packet, SCE_GS_TEST_1, XRG_PAINT2D_TEST_ZALWAYS);
    sceVif1PkAddGsAD(packet, SCE_GS_CLAMP_1, (0x3FCULL << 32) | (0x3FULL << 16) | 0xC000);
    sceVif1PkAddGsAD(packet, 0x49, 0);
    sceVif1PkAddGsAD(packet, SCE_GS_ALPHA_1, 0x44);
    _closeVifGif(packet);
    size = sceVif1PkSize(packet);
    if (size != 0) {
        nmlModelDirectSend(view->projectionMode, (void *) SCRATCHPAD_BASE, size);
    }
    sceVif1PkInit(packet, SCRATCHPAD_BASE);

    prevXtx = 0;
    i = 0;
    if (paint->reqCount != 0) {
        req = (XrgPaint2DDrawReq *) &paint->req;
        do {
            xtx = 0;
            if (req->texture != 0) {
                xtx = RgBxxGetXtx(req->texture->pOwner);
            }
            if (xtx != prevXtx) {
                size = sceVif1PkSize(packet);
                if (size != 0) {
                    nmlModelDirectSend(view->projectionMode, (void *) SCRATCHPAD_BASE, size);
                }
                sceVif1PkInit(packet, SCRATCHPAD_BASE);
                prevXtx = xtx;
                if (xtx != 0) {
                    nmlModelDirectSendXtx(view->projectionMode, xtx);
                    _openVifGifAD(packet);
                    sceVif1PkAddGsAD(packet, SCE_GS_TEXFLUSH, 0);
                    _closeVifGif(packet);
                }
            }
            if (req->geom != 0) {
                flags = ((int (*)(XrgPaint2DDrawReq *, XrgPaint2DVertex *)) req->geom)(req, corners);
                if ((flags & XRG_PAINT2D_GEOM_DEPTH_TEST) != (prevFlags & XRG_PAINT2D_GEOM_DEPTH_TEST)) {
                    _openVifGifAD(packet);
                    if ((flags & XRG_PAINT2D_GEOM_DEPTH_TEST) != 0) {
                        sceVif1PkAddGsAD(packet, SCE_GS_TEST_1, XRG_PAINT2D_TEST_ZGREATER);
                    } else {
                        sceVif1PkAddGsAD(packet, SCE_GS_TEST_1, XRG_PAINT2D_TEST_ZALWAYS);
                    }
                    _closeVifGif(packet);
                }
                prim = flags & XRG_PAINT2D_GEOM_PRIMITIVE_MASK;
                if (prim != 0 && req->draw != 0) {
                    ((void (*)(XrgPaint2DDrawReq *, XglPacket *, XrgPaint2DVertex *, int)) req->draw)(req, packet, corners, prim);
                    prevFlags = flags;
                }
            }
            size = sceVif1PkSize(packet);
            if (size >= 0x381) {
                nmlModelDirectSend(view->projectionMode, (void *) SCRATCHPAD_BASE, size);
                sceVif1PkInit(packet, SCRATCHPAD_BASE);
            }
            i++;
            req++;
        } while (i < paint->reqCount);
    }
    size = sceVif1PkSize(packet);
    if (size != 0) {
        nmlModelDirectSend(view->projectionMode, (void *) SCRATCHPAD_BASE, size);
    }
}

static void _Paint2DClearReq(void *pObject) {
    XrgPaint2D *paint = pObject;

    if (paint == 0) {
        assert_prog(paint2dAssertPaintNotNil, paint2dSourcePath, 0x484);
    }
    paint->reqCount = 0;
    paint->request = &paint->req;
    _InitReq(&paint->req);
}

void XrgPaint2DFlush(XrgPaint2D *paint)
{
    if (paint == 0) {
        assert_prog(paint2dAssertPaintNotNil, paint2dSourcePath, 1168);
    }
    RgDrawReq(InstanceOfRgDraw(), paint, _Paint2DFlush, _Paint2DClearReq,
             paint->prio, paint->drawID);
}



static int s_anHomePos[2] = { 28672, 29184 };

static float s_aSizeRatio[2] = { 1.0f, 1.0f };

static RgVector s_aCurCenterPos = { 2048.0f, 2048.0f, 0.0f, 0.0f };

static RgDrawView * s_pCurView_00A51220 = 0;

const char paint2dUnknownOffsetModeFormat[24] = "unknown offset mode %x";

const char paint2dUnknownGeomModeFormat[40] = "unknown XrgPaint2DGeom mode (%x)";

const char paint2dAssertRequestCountBelowMax[32] = "pPaint->m_uReqNum < REQ_MAX - 1";

const char paint2dAssertViewNotNil[16] = "pView != NIL";
