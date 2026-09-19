/*
 * TU-local declarations of ov12/tu045 (src/ov12/rg_draw.c).
 */

#ifndef SRC_OV12_RG_DRAW_H
#define SRC_OV12_RG_DRAW_H

#include "shared.h"

typedef struct RgDraw RgDraw;
typedef struct RgDrawView RgDrawView;
typedef struct RgDrawStudio RgDrawStudio;

/*
 * Global fog settings, 28 bytes (_CopyFog, ov12:0x00a26e20, copies seven
 * float words). RgBgBuilderBuildFromText (ov12:0x00a33448) fills the first
 * four floats from the "fog" directive and the last three from the "fogcol"
 * directive, and _ActorDrawFunction (ov12:0x00a46148) passes the same split
 * to the model renderer: offsets 0x0..0xc as the four scalar arguments of
 * nmlModelSetFogDist and offsets 0x10..0x18 as the three-float color of
 * nmlModelSetFogCol. _DefaultFog resets the distances to (50, 250, 0, 1)
 * and the color to black. The assertion string "pFog != NIL"
 * (ov12:0x00a54b80) names the pointer role.
 */
typedef struct RgFog {
    float dist[4];
    float color[3];
} RgFog;

/*
 * The observed access view: _CreateRgDrawStudio (ov12:0x00a27108) allocates
 * exactly 12 bytes for the whole object. _InitRgDrawStudio
 * (ov12:0x00a27030, outside this allocation) stores its screenIndex
 * parameter at offset 0x0 and its pFog parameter at offset 0x4, then
 * creates the view only when screenIndex is not -1 (m_pView is NIL
 * otherwise). _FullScreenStudio and _DoubleScreenStudio
 * (ov12:0x00a27330/0x00a27588, both outside this allocation) pass the
 * parent RgDraw's own fog block at +0xa20 as pFog. This allocation's
 * _DestructRgDrawStudio and _GetViewRgDrawStudio (ov12:0x00a270b0,
 * 0x00a271e0) read only offset 0x8,
 * the RgDrawView pointer the assertion string "pStudio->m_pView != NIL"
 * (ov12:0x00a54b40) names m_pView. Offsets 0x0 and 0x4 stay an explicit
 * unmodeled span: they are real fields _InitRgDrawStudio owns, not ones
 * this allocation claims.
 */
struct RgDrawStudio {
    unsigned char unmodeled_00[8];
    RgDrawView *m_pView;
};

/*
 * The four screen-rectangle words RgDrawViewGetScreenRect (ov12:0x00a27e40)
 * copies out of a RgDrawView, in the order it copies them (offsets
 * 0x70..0x7c). The caller _Paint2DFlush (ov12:0x00a4c370, outside this
 * allocation) is the only reader; nothing in this allocation evidences
 * which corner or extent each word names, so the fields keep the neutral,
 * position-only names x0/y0/x1/y1.
 */
typedef struct RgRect {
    int x0;
    int y0;
    int x1;
    int y1;
} RgRect;

/*
 * RgDrawViewGetScreenRect (ov12:0x00a27e40) is the only accepted reader of a
 * RgDrawView, and it touches only the screen rectangle at offset 0x70. Every
 * other field the view carries (position, rotation, the studio it belongs
 * to) is set up by RgDrawViewInit and friends, all still outside this
 * allocation, so offsets 0x0..0x6f stay an explicit unmodeled span.
 */
struct RgDrawView {
    unsigned char unmodeled_00[0x70];
    RgRect screenRect;
};

/*
 * The observed access view of the singleton RgDraw object InstanceOfRgDraw
 * (ov12:0x00a27740, outside this allocation) returns. RgDrawGetGlobalFog and
 * RgDrawSetGlobalFog (ov12:0x00a27908/0x00a27978) pass this object's own
 * +0xa20 to _CopyFog as the RgFog side, matching the RgDrawStudio comment
 * above that names +0xa20 as "the parent RgDraw's own fog block". Nothing in
 * this allocation evidences offsets 0x0..0xa1f, which precede it. Between
 * the fog block and the studio array below, offsets 0xa3c..0xa3f stay an
 * explicit unmodeled span. RgDrawGetStudio (ov12:0x00a277e0) reads a two
 * entry array of RgDrawStudio pointers at +0xa40 for screenIndex 0 and 1
 * (screenIndex -1 returns NULL without touching memory, and any other value
 * reaches RgError); _ClearStudioList/_FullScreenStudio/_DoubleScreenStudio,
 * all still outside this allocation, are the array's likely writers.
 */
struct RgDraw {
    unsigned char unmodeled_00[0xa20];
    RgFog m_Fog;
    unsigned char unmodeled_0a3c[4];
    RgDrawStudio *m_pStudios[2];
};

#endif /* SRC_OV12_RG_DRAW_H */
