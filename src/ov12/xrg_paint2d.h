/*
 * TU-local declarations of ov12/tu086 (src/ov12/xrg_paint2d.c).
 *
 */

#ifndef SRC_OV12_XRG_PAINT2D_H
#define SRC_OV12_XRG_PAINT2D_H

#include "shared.h"

/*
 * The 3D line descriptor InitXrgPaint2DLine3D (0x00a4c1f8) builds and
 * XrgPaint2DDrawLine (0x00a4c240, outside this allocation) copies into the
 * paint's request ring for the geometry callback _Line3D (0x00a4b120):
 *
 *   mode   the same flags-word shape XrgPaint2DRect's mode has; InitXrgPaint2DLine3D
 *          zeroes it and _Line3D tests bit 0x1 of the copy (0x00a4b228).
 *   start, the line's two endpoints. InitXrgPaint2DLine3D clears both with
 *   end    XrgClearVector; the caller (e.g. RgShotEffectDisp, 0x00a184d8)
 *          fills them in afterwards, and XrgPaint2DDrawLine copies each as one
 *          quadword (lqc2/sqc2, 0x00a4c2c8/0x00a4c2d8).
 *   width  the line's thickness. XrgPaint2DDrawLine halves it (mul.s by 0.5,
 *          0x00a4c2ec) before storing it into the request, and _Line3D uses
 *          that half to offset each projected point's screen-space extent
 *          (0x00a4b190..0x00a4b1a8).
 *
 * The 12-byte gap after mode exists only to bring start to the 16-byte
 * alignment its quadword copy requires, the same reason XrgPaint2DRect below
 * has one.
 */
typedef struct XrgPaint2DLine3D {
    int mode;
    unsigned char quadword_alignment_gap[12];
    RgVector start;
    RgVector end;
    float width;
} XrgPaint2DLine3D;

/*
 * The rectangle request the caller fills in and hands to XrgPaint2DDrawRect,
 * which copies it into the paint object's request ring: `mode` reaches the
 * request at +0x08, the four words at +0x10..+0x1f reach it as one quadword
 * (lqc2/sqc2 at 0x00a4c0ec/0x00a4c0f0) and `scale`/`angle` reach +0x20/+0x24
 * with lwc1/swc1 (0x00a4c0f4..0x00a4c100).  Every field below is named from
 * what _Geom2D (0x00a4ad80), the geometry callback _InitReq installs for such
 * a request, does with the copy:
 *
 *   mode    the request's mode word.  _Geom2D loads it once (0x00a4adcc) and
 *           uses it as a bit set - bit 0x20 takes the size from the bound
 *           picture instead of width/height (0x00a4ade0), bit 0x40 applies
 *           `scale` (0x00a4ae38), bit 0x80 applies `angle` (0x00a4af24), bit
 *           0x100 a per-frame jitter (0x00a4afd8) - and the low four bits as
 *           two 2-bit anchor selectors, one per axis (0x00a4aeb0..0x00a4af0c:
 *           0 keeps the corner, 1 subtracts half the extent, 2 subtracts the
 *           whole extent).  An unknown selector reaches RgError with the
 *           game's own message "unknown XrgPaint2DGeom mode (%x)" (D_00A59070,
 *           0x00a4af00), which is where the word's name comes from.
 *   x, y    the screen position in whole pixels: _Geom2D loads them with lw,
 *           shifts left by 4 and converts with cvt.s.w (0x00a4ae60..0x00a4ae94),
 *           i.e. it turns integer pixels into the GS's 12.4 subpixel units,
 *           and the final vftoi0 (0x00a4b064) writes them back as integers.
 *           The three callers in rg_select_agws compute them with integer
 *           addiu (_DrawWepListDisp 0x00a39b20/0x00a39b24), never as floats.
 *   width,  the extent in whole pixels, converted the same way
 *   height  (0x00a4ae10..0x00a4ae34) and used as the second corner's offset.
 *   scale   a multiplier on width and height, mul.s at 0x00a4ae50/0x00a4ae54;
 *           InitXrgPaint2DRect seeds it with 1.0f.
 *   angle   the rotation about Z applied to the four corners,
 *           XrgRotMatrixZ(0x00a4af84) takes it in $f12.
 *
 * Nothing in the six images reads or writes +0x04..+0x0f: this TU's only
 * writers of a rectangle are InitXrgPaint2DRect and XrgPaint2DDrawXYWH
 * (0x00a4c1c0..0x00a4c1d0), the only reader is XrgPaint2DDrawRect, and the
 * three external callers all go through XrgPaint2DDrawXYWH.  The span is the
 * alignment the quadword payload forces after the mode word - XrgPaint2DDrawRect
 * reaches +0x10 with lqc2, which requires a 16-byte aligned address, and the
 * sibling descriptor InitXrgPaint2DLine3D builds (0x00a4c1f8) has the same
 * shape: mode at +0x00, untouched +0x04..+0x0f, and two XrgClearVector
 * vectors at +0x10 and +0x20.  It is declared as that alignment, not as a
 * field.  The type stops at +0x28; XrgPaint2DDrawXYWH's frame shows the
 * object is 0x30 bytes, and nothing evidences +0x28..+0x2f.
 */
typedef struct XrgPaint2DRect {
    int mode;
    unsigned char quadword_alignment_gap[12];
    int x;
    int y;
    int width;
    int height;
    float scale;
    float angle;
} XrgPaint2DRect;

typedef struct XrgPaint2D XrgPaint2D;
typedef struct XrgPaint2DDrawReq XrgPaint2DDrawReq;

/*
 * The draw-request object CreateXrgPaint2D_sub's paint keeps a pointer to at
 * +0xD014 (read by XrgPaint2DUseTexture/XrgPaint2DOffsetResult, outside this
 * allocation) and _InitReq (0x00a4a728, outside this allocation) fills in.
 * _DeriveInfoReq (0x00a4a788) copies two spans of it, both left exactly as
 * wide as the copy itself because nothing in this allocation's evidence
 * resolves their interior further:
 *
 *   offsetMode    a flags/selector word _CalcOffset (0x00a4aa58) reads as its
 *                 own mode argument: the low two bits pick one of four
 *                 anchor branches (a table lookup when clear, home-position
 *                 arithmetic otherwise) and bit 0x10 is the one
 *                 XrgPaint2DOffsetResult (outside this allocation) ORs in
 *                 when it stores an explicit override.  _InitReq zeroes it.
 *   offsetAnchor  the 16-byte block at +0x50 _CalcOffset reads as its own
 *                 anchor argument (address-only, never itself interpreted by
 *                 _DeriveInfoReq); _InitReq zeroes it as one quadword, which
 *                 is why it is four ints rather than an opaque byte span.
 *   offsetResult  the 16-byte block at +0x60 _CalcOffset reads as its own
 *                 current/result argument the same way; its second int
 *                 (+0x64) is the explicit value XrgPaint2DOffsetResult
 *                 stores there.  _InitReq zeroes it as one quadword.
 *
 * Everything before +0x40 (the geometry callback pointers _InitReq installs,
 * the mode/position/scale/angle XrgPaint2DDrawRect and XrgPaint2DDrawLine
 * copy in from the caller's XrgPaint2DRect, and the picture-size flags
 * _Geom2D reads) and the 12 bytes between offsetMode and offsetAnchor stay
 * unmodeled: this allocation only copies them, it never interprets them.
 * offsetAnchor's and offsetResult's own four ints are named only by position
 * (x, y, z, w): nothing in this allocation's evidence gives any of them an
 * individual role beyond offsetResult's second int.
 *
 * `quadword` is never read: it exists only so the type's own alignment (8,
 * from `long long`) matches the 8-byte loads/stores _DeriveInfoReq's whole-
 * union copy compiles to, the same width _CalcOffset reads this block with.
 * A plain four-int struct has 4-byte alignment, which cc1 cannot use for an
 * aligned load or store and reproduces with unaligned ldl/ldr instead.
 */
typedef union XrgPaint2DOffset {
    struct {
        int x;
        int y;
        int z;
        int w;
    } i;
    /*
     * XrgPaint2DOffset3DForce (0x00a4bc18) stores its float argument into
     * this union's first word (the same word `i.x` names) with a plain
     * float store (swc1), not the int store every other writer of this
     * union uses; `f` is the same four-float layout as `i` for that one
     * write.
     */
    struct {
        float x;
        float y;
        float z;
        float w;
    } f;
    long long quadword[2];
} XrgPaint2DOffset;

/*
 * XrgPaint2DUseTexture (0x00a4be70) fills in the rest of the request once a
 * texture is bound, still inside this allocation's own evidence:
 *
 *   draw    the callback the request's execution invokes once a texture is
 *           bound: XrgPaint2DUseTexture stores &_DrawWithTex (0x00a4b560,
 *           outside this allocation) here, in the same (paint, pStudio)
 *           calling shape _Paint2DFlush/_Paint2DClearReq already use for the
 *           adjacent RgDrawReq callbacks.
 *   texture the bound texture object; XrgPaint2DUseTexture stores its own
 *           argument here unchanged.
 *   tex0    the GS TEX0 register value XrgBxxPs2Tex0 computes for `texture`.
 *   uLow,   the GS TEXCLAMP bounds XrgPaint2DUseTexture derives from the
 *   vLow,   bound texture's own left/top/right/bottom (XrgPaint2DTexture,
 *   uHigh,  where XrgPaint2DUseTexture is defined), converted to 12.4
 *   vHigh   subpixel units and inset by half a texel (8 = 0.5 * 16) on each
 *           side so the clamp never samples past the bound sub-image.
 *   uvMode  a flags word the explicit XrgPaint2DSetUVOffset/-SetUVSize
 *           setters (0x00a4bf48/0x00a4bfc8) OR a bit into: bit 0x1 for an
 *           explicit uOffset/vOffset, bit 0x2 for an explicit uSize/vSize,
 *           the same "explicit override" idiom offsetMode's bit 0x10 above
 *           already uses.
 *   uOffset, the explicit UV origin XrgPaint2DSetUVOffset stores, already
 *   vOffset  converted to 12.4 subpixel units.
 *   uSize,   the explicit UV extent XrgPaint2DSetUVSize stores, the same
 *   vSize    way.
 */
typedef struct XrgPaint2DDrawReq {
    unsigned char unmodeled_00[4];
    void (*draw)(XrgPaint2D *paint, void *pStudio);
    unsigned char unmodeled_08[0x38];
    int offsetMode;
    unsigned char unmodeled_44[0xC];
    XrgPaint2DOffset offsetAnchor;
    XrgPaint2DOffset offsetResult;
    void *texture;
    unsigned char unmodeled_74[0xC];
    /*
     * XrgPaint2DColor (0x00a4bd38) copies its own 16-byte argument in here
     * as one COP2 quadword transfer (lqc2/sqc2); nothing in this allocation
     * reads it back, so no individual channel is named.
     */
    unsigned int color[4];
    /*
     * XrgPaint2DAlpha (0x00a4bd98) stores one of six fixed GS ALPHA_1-shaped
     * 64-bit blend-equation values here, selected by its own mode argument;
     * see that function for the values. Nothing in this allocation reads it
     * back.
     */
    long long alpha;
    long long tex0;
    int uLow;
    int vLow;
    int uHigh;
    int vHigh;
    int uvMode;
    unsigned char unmodeled_B4[0xC];
    int uOffset;
    int vOffset;
    int uSize;
    int vSize;
} XrgPaint2DDrawReq;

/*
 * The heap-allocated 2D paint renderer CreateXrgPaint2D_sub allocates
 * (XRG_PAINT2D_SIZE bytes, below) and DisposeXrgPaint2D_sub releases.  Only
 * the two fields this allocation's own setters touch are named:
 *
 *   prio    XrgPaint2DSetDrawPrio (0x00a4b9e8) stores its argument here
 *           unconditionally after asserting the object is non-nil.
 *   drawID  XrgPaint2DSetDrawID (0x00a4b998) stores its argument here the
 *           same way.
 *
 * The rest of the object (its draw-request ring at +0xD014, the debug
 * allocation-site name _InitPaint copies to +0xD018, and everything else
 * _Geom2D, _CalcOffset, _InitPaint, XrgPaint2DUseTexture and the sibling
 * XrgPaint2DOffset and XrgPaint2DSetUV setters read) is outside this
 * allocation's evidence and is not modeled here.
 */
#define XRG_PAINT2D_SIZE 0xD060

typedef struct XrgPaint2D {
    int prio;
    int drawID;
    /*
     * XrgPaint2DOffsetResult and XrgPaint2DUseTexture (0x00a4bca0/0x00a4be70)
     * are the first functions of this allocation to reach through the
     * pointer at +0xD014 the earlier comment names: both read it as the
     * draw-request object XrgPaint2DDrawReq (below) describes, so it is
     * named `request` here. The 0xD00C bytes before it stay unmodeled for
     * the same reason as above: nothing in this allocation reads or writes
     * them.
     */
    /*
     * _Paint2DClearReq (0x00a4c780) takes the address of the +0x10 word,
     * hands it to _InitReq (0x00a4a728, outside this allocation) and stores
     * that same address into `request` right after: this is the first
     * element of the embedded draw-request array `request` points into once
     * (re)initialized, so it is typed and named `req` here. Nothing in this
     * allocation resolves the rest of that array.
     */
    unsigned char unmodeled_08[0x10 - 8];
    XrgPaint2DDrawReq req;
    unsigned char unmodeled_e0[0xD010 - 0xE0];
    /*
     * _Paint2DClearReq always zeroes this word right before rewinding
     * `request` to `req`: the count of queued draw requests it also resets.
     */
    int reqCount;
    XrgPaint2DDrawReq *request;
} XrgPaint2D;

/*
 * The four corners _CalcRectangle (0x00a4a7b8) expands one XYWH rectangle
 * into, in the GS packet order its own caller _CalcUV (0x00a4a8a0) and
 * _Geom2D (0x00a4ad80) build on: top-left, top-right, bottom-left,
 * bottom-right.  `color` and `prio` are the same uniform values on all four
 * corners; `prio` is named for the draw-priority vocabulary this TU already
 * uses (XrgPaint2DSetDrawPrio).
 */
typedef struct XrgPaint2DVertex {
    int x;
    int y;
    int color;
    int prio;
} XrgPaint2DVertex;

/* The plain integer rectangle _CalcUV (0x00a4a8a0) builds and hands to
 * _CalcRectangle: nothing else in this allocation's evidence reads or
 * writes it, so no other field is claimed.
 */
typedef struct XrgIntRect {
    int x;
    int y;
    int width;
    int height;
} XrgIntRect;

extern void assert_prog(const char *expression, const char *source_file,
                        int line);

extern const char D_00A59098[];
extern const char D_00A59058[];

#endif /* SRC_OV12_XRG_PAINT2D_H */
