/*
 * OV12 original TU 86: 0x00a4a620..0x00a4c840 (42 functions)
 */
#include "common.h"
#include "shared.h"
#include "xrg_paint2d.h"
#include "ov12/rg_draw.h"
#include "ov12/rg_singleton_id.h"
#include "ov12/xrg_rand_int.h"

/*
 * _openVifGif and _openVifGifAD build the 128-bit VIF1 GIF tag
 * sceVif1PkOpenGifTag takes in the single register the original loads with
 * one `lq`; no wide arithmetic is done on it: bit 15 is EOP, bit 46 is PRE,
 * bits 47..57 are PRIM, bits 60..63 are NREG, and the high 64 bits are the
 * REGS descriptor. The union lets the two halves be stored as ordinary
 * 64-bit fields and the whole 16 bytes be read back as the single register
 * sceVif1PkOpenGifTag takes.
 */
typedef unsigned int Quadword __attribute__((mode(TI)));

typedef union XrgPaint2DGifTag {
    struct {
        u64 lo;
        u64 hi;
    } part;
    Quadword quad;
} XrgPaint2DGifTag;

extern void sceVif1PkCnt(XglPacket *packet, int count);
extern void sceVif1PkAlign(XglPacket *packet, int align, int size);
extern void sceVif1PkOpenDirectCode(XglPacket *packet, int mode);
extern void sceVif1PkOpenDirectHLCode(XglPacket *packet, int mode);
extern void sceVif1PkOpenGifTag(XglPacket *packet, Quadword tag);

static void _openVifGif(XglPacket *packet, unsigned int prim,
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
extern const XrgPaint2DGifTag D_00A59030;

static void _openVifGifAD(XglPacket *packet)
{
    XrgPaint2DGifTag tag;

    tag = D_00A59030;
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
typedef struct sceVif1Packet sceVif1Packet;
extern void sceVif1PkCloseGifTag(sceVif1Packet *packet);
extern void sceVif1PkCloseDirectHLCode(sceVif1Packet *packet);

static void _closeVifGif(sceVif1Packet *packet) {
    sceVif1PkCloseGifTag(packet);
    sceVif1PkCloseDirectHLCode(packet);
}

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_paint2d", _InitReq);

static void _DeriveInfoReq(XrgPaint2DDrawReq *dst, XrgPaint2DDrawReq *src)
{
    dst->offsetMode = src->offsetMode;
    dst->offsetAnchor = src->offsetAnchor;
    dst->offsetResult = src->offsetResult;
}

#define XRG_PAINT2D_COORD_MAX 0x3FFF

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

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_paint2d", _CalcPosOnScreen);

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_paint2d", _CalcPosOnScreenFloat);

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_paint2d", _CalcOffset);

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_paint2d", _Geom2D);

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_paint2d", _Line3D);

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_paint2d", _DrawNoTex);

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_paint2d", _DrawWithTex);

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_paint2d", _InitPaint);

static void _DestructPaint(XrgPaint2D *paint) {
    if (paint == 0) {
        assert_prog(D_00A59098, D_00A59058, 713);
    }
}

extern RgHeap *InstanceOfRgHeap(void);
extern void *RgHeapAlloc(RgHeap *heap, unsigned int size, const char *source_file,
                        int line);
extern void RgHeapFree(RgHeap *heap, void *ptr, const char *source_file,
                       int line);
static void _InitPaint(XrgPaint2D *paint, const char *sourceFile, int line);

XrgPaint2D *CreateXrgPaint2D_sub(const char *sourceFile, int line) {
    XrgPaint2D *paint;

    paint = RgHeapAlloc(InstanceOfRgHeap(), XRG_PAINT2D_SIZE, D_00A59058, 720);
    _InitPaint(paint, sourceFile, line);
    return paint;
}

void DisposeXrgPaint2D_sub(XrgPaint2D *paint, const char *sourceFile, int line) {
    if (paint == 0) {
        assert_prog(D_00A59098, D_00A59058, 727);
    }
    _DestructPaint(paint);
    RgHeapFree(InstanceOfRgHeap(), paint, sourceFile, line);
}

static void _WrapperDestruct(RgSimpleDB *database) {
    DisposeXrgPaint2D_sub((XrgPaint2D *) database, D_00A59058, 0x2DC);
}

void XrgPaint2DSetDrawPrio(XrgPaint2D *paint, int prio);

XrgPaint2D *InstanceOfXrgPaint2D(void) {
    XrgPaint2D *paint;

    paint = RgSingletonIDGet(10);
    if (paint == 0) {
        paint = CreateXrgPaint2D_sub(D_00A59058, 742);
        XrgPaint2DSetDrawPrio(paint, 3);
        RgSingletonIDEntry(10, (RgSimpleDB *) paint, _WrapperDestruct);
    }
    return paint;
}

void XrgPaint2DSetDrawID(XrgPaint2D *paint, int drawID) {
    if (paint == 0) {
        assert_prog(D_00A59098, D_00A59058, 757);
    }
    paint->drawID = drawID;
}

void XrgPaint2DSetDrawPrio(XrgPaint2D *paint, int prio) {
    if (paint == 0) {
        assert_prog(D_00A59098, D_00A59058, 765);
    }
    paint->prio = prio;
}

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_paint2d", XrgPaint2DOffset2D);

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_paint2d", XrgPaint2DOffset2DDot);

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_paint2d", XrgPaint2DOffsetCenter);

void XrgPaint2DOffset3D(XrgPaint2D *paint, const XrgPaint2DOffset *anchor)
{
    XrgPaint2DDrawReq *req;

    if (paint == 0) {
        assert_prog(D_00A59098, D_00A59058, 804);
    }
    req = paint->request;
    req->offsetMode = 2;
    __asm__ __volatile__("lqc2 vf31, 0(%0)\n\tsqc2 vf31, 0(%1)"
                         : : "r"(anchor), "r"(&req->offsetAnchor) : "memory");
}

/*
 * XrgPaint2DOffset3DForce (0x00a4bc18) re-reads paint->request for the final
 * store instead of reusing `req`: the original reloads it into a second
 * register there rather than keeping the first one live across the COP2
 * transfer.
 */
void XrgPaint2DOffset3DForce(XrgPaint2D *paint, const XrgPaint2DOffset *anchor,
                             float force)
{
    XrgPaint2DDrawReq *req;

    if (paint == 0) {
        assert_prog(D_00A59098, D_00A59058, 812);
    }
    req = paint->request;
    req->offsetMode = 3;
    __asm__ __volatile__("lqc2 vf31, 0(%0)\n\tsqc2 vf31, 0(%1)"
                         : : "r"(anchor), "r"(&req->offsetAnchor) : "memory");
    paint->request->offsetResult.f.x = force;
}

/*
 * XrgPaint2DOffsetResult stores an explicit offset-result override into the
 * bound paint's draw request and flags it as such (offsetMode bit 0x10,
 * XrgPaint2DDrawReq above).
 */
void XrgPaint2DOffsetResult(XrgPaint2D *paint, int value)
{
    XrgPaint2DDrawReq *req;

    if (paint == 0) {
        assert_prog(D_00A59098, D_00A59058, 821);
    }
    req = paint->request;
    req->offsetResult.i.y = value;
    req->offsetMode |= 0x10;
}

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_paint2d", XrgPaint2DOffsetLinear);

void XrgPaint2DColor(XrgPaint2D *paint, const unsigned int *color)
{
    unsigned int *dst;

    if (paint == 0) {
        assert_prog(D_00A59098, D_00A59058, 843);
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
        assert_prog(D_00A59098, D_00A59058, 852);
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

extern void RgBxxGetHeader(int header);
extern long long XrgBxxPs2Tex0(void *texture);
extern const char D_00A590C8[];
static void _DrawWithTex(XrgPaint2D *paint, void *pStudio);

/*
 * The texture object XrgPaint2DUseTexture binds to a paint request: only the
 * four members this function itself reads are named, all of them consumed
 * as the source rectangle for the GS TEXCLAMP bounds it derives (see
 * XrgPaint2DDrawReq's uLow/vLow/uHigh/vHigh). `header` is the value the
 * function forwards to RgBxxGetHeader without itself reading the result.
 * Nothing else about this foreign object is evidenced here.
 */
typedef struct XrgPaint2DTexture {
    unsigned char unmodeled_00[0x2C];
    int left;
    int top;
    int right;
    int bottom;
    unsigned char unmodeled_3C[0x1C];
    int header;
} XrgPaint2DTexture;

void XrgPaint2DUseTexture(XrgPaint2D *paint, XrgPaint2DTexture *texture)
{
    XrgPaint2DDrawReq *req;
    long long tex0;

    if (paint == 0) {
        assert_prog(D_00A59098, D_00A59058, 887);
    }
    if (texture == 0) {
        assert_prog(D_00A590C8, D_00A59058, 888);
    }
    req = paint->request;
    RgBxxGetHeader(texture->header);
    tex0 = XrgBxxPs2Tex0(texture);
    req->texture = texture;
    req->tex0 = tex0;
    req->draw = _DrawWithTex;
    req->uLow = texture->left * 0x10 + 8;
    req->vLow = texture->top * 0x10 + 8;
    req->uHigh = texture->right * 0x10 - 8;
    req->vHigh = texture->bottom * 0x10 - 8;
}

void XrgPaint2DSetUVOffset(XrgPaint2D *paint, int uOffset, int vOffset)
{
    int scaledUOffset;
    XrgPaint2DDrawReq *req;

    if (paint == 0) {
        assert_prog(D_00A59098, D_00A59058, 913);
    }
    scaledUOffset = uOffset * 0x10;
    req = paint->request;
    req->vOffset = vOffset * 0x10;
    req->uOffset = scaledUOffset;
    req->uvMode |= 1;
}

void XrgPaint2DSetUVSize(XrgPaint2D *paint, int width, int height)
{
    int uSize;
    XrgPaint2DDrawReq *req;

    if (paint == 0) {
        assert_prog(D_00A59098, D_00A59058, 927);
    }
    uSize = width * 0x10;
    req = paint->request;
    req->vSize = height * 0x10;
    req->uSize = uSize;
    req->uvMode |= 2;
}

/*
 * InitXrgPaint2DRect clears the rectangle request consumed by the adjacent
 * XrgPaint2DDrawRect/XrgPaint2DDrawXYWH routines: an all-zero rectangle at the
 * screen origin, unrotated, at its natural size.  The layout the members come
 * from is recovered in src/ov12/xrg_paint2d.h.
 *
 * The store order (mode, scale, height, width, y, x, angle) is the one the
 * accepted form was matched with and is unchanged here, so this form isolates
 * the layout recovery from the schedule.
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

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_paint2d", XrgPaint2DDrawRect);

extern void XrgPaint2DDrawRect(XrgPaint2D *paint, XrgPaint2DRect *rect);

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

void InitXrgPaint2DLine3D(XrgPaint2DLine3D *line, float width)
{
    line->mode = 0;
    XrgClearVector(line->start);
    XrgClearVector(line->end);
    line->width = width;
}

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_paint2d", XrgPaint2DDrawLine);

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_paint2d", _Paint2DFlush);

static void _InitReq(XrgPaint2DDrawReq *req);

static void _Paint2DClearReq(XrgPaint2D *paint, void *pStudio) {
    if (paint == 0) {
        assert_prog(D_00A59098, D_00A59058, 0x484);
    }
    paint->reqCount = 0;
    paint->request = &paint->req;
    _InitReq(&paint->req);
}

extern RgDraw *InstanceOfRgDraw(void);
/*
 * RgDraw's _DrawMain (ov12:0x00a28320) calls both callbacks with the paint
 * and the studio being drawn; _Paint2DFlush passes that studio to
 * RgDrawStudioGetView.
 */
extern void RgDrawReq(RgDraw *draw, XrgPaint2D *paint,
                      void (*flush)(XrgPaint2D *paint, void *pStudio),
                      void (*clear)(XrgPaint2D *paint, void *pStudio),
                      int prio, int drawID);

static void _Paint2DFlush(XrgPaint2D *paint, void *pStudio);
static void _Paint2DClearReq(XrgPaint2D *paint, void *pStudio);

void XrgPaint2DFlush(XrgPaint2D *paint)
{
    if (paint == 0) {
        assert_prog(D_00A59098, D_00A59058, 1168);
    }
    RgDrawReq(InstanceOfRgDraw(), paint, _Paint2DFlush, _Paint2DClearReq,
             paint->prio, paint->drawID);
}
