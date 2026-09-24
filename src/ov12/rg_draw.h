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
 *
 * This allocation now also claims _InitRgDrawStudio itself: it stores its
 * screenIndex parameter at offset 0x0 and its pFog parameter at offset 0x4,
 * in that order, so the two offsets above are named m_ScreenIndex and
 * m_pFog below rather than staying unmodeled. RgDrawViewSetPosition/
 * SetRotateX/SetRotateY/SetRotateZ (ov12:0x00a27ab0/0x00a27b50/0x00a27bd8/
 * 0x00a27c60, this allocation) read m_ScreenIndex back through a
 * RgDrawView's own m_pParentStudio pointer.
 */
struct RgDrawStudio {
    int m_ScreenIndex;
    RgFog *m_pFog;
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
 *
 * This allocation now also claims RgDrawViewSetPosition/SetRotateX/
 * SetRotateY/SetRotateZ (ov12:0x00a27ab0/0x00a27b50/0x00a27bd8/0x00a27c60):
 * each reads a RgDrawStudio pointer at offset 0x68
 * (`*(*(s32 **)(pView + 0x68))` reads that pointer, then its screenIndex
 * word at the RgDrawStudio's own offset 0x0) to find the screen the view
 * belongs to -- the role the comment above already names, and the same
 * concept _CreateRgDrawView's own "pParentStudio" parameter names. Offset
 * 0x68 is named m_pParentStudio below; offsets 0x0..0x67 and 0x6c..0x6f stay
 * explicit unmodeled spans.
 */
struct RgDrawView {
    unsigned char unmodeled_00[0x68];
    RgDrawStudio *m_pParentStudio;
    unsigned char unmodeled_6c[4];
    RgRect screenRect;
};

/*
 * One entry of RgDraw's request array at +0x10 (RgDrawReq, ov12:0x00a28028,
 * and _DrawMain, ov12:0x00a28320, both still assembly, outside this
 * allocation). Per src/ov12/xrg_dispmodel_impl.c's own citation of
 * RgDrawReq, one 20-byte entry stores the request's object, its draw
 * callback, its clear callback and RgDrawReq's last two int parameters
 * (prio, drawID), in that order. _DrawReqTerminate (this allocation) calls
 * only the clear callback at offset 0x8 with the object at offset 0x0 as
 * its single argument, so the other three words stay an explicit unmodeled
 * span here.
 */
typedef struct RgDrawRequest {
    void *pObject;
    unsigned char unmodeled_04[4];
    void (*clearFunc)(void *pObject);
    unsigned char unmodeled_0c[8];
} RgDrawRequest;

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
 *
 * This allocation now also claims _InitRgDrawStudio's sizing call site
 * InstanceOfRgDraw, _InitDraw, _ClearStudioList, _FullScreenStudio,
 * _DoubleScreenStudio, _SetScreenSplitMode and _DrawReqTerminate
 * (ov12:0x00a27640/0x00a272b0/0x00a27330/0x00a27588/0x00a27498/0x00a282b0),
 * which sizes this object at RgHeapAlloc(0xa60) and evidences most of the
 * previously unmodeled span, so it is broken out below:
 *   +0x000 m_Enabled: _InitDraw sets it to 1; _DrawMain (ov12:0x00a28320,
 *     outside this allocation) reads it at offset 0x0 as a master gate and
 *     returns immediately when it is 0.
 *   +0x010 m_Requests[128]: RgDraw's request array (RgDrawRequest above);
 *     128 entries of 0x14 bytes exactly fill the span up to m_RequestCount
 *     at +0xa10, both fixed by this object's own 0xa60-byte size.
 *   +0xa10 m_RequestCount: _InitDraw clears it and _DrawReqTerminate both
 *     reads it as the request array's length and clears it back to 0 once
 *     every entry's clear callback has run.
 *   +0xa3c m_pDefaultStudio: the screenIndex -1 studio _InitDraw creates
 *     over this same m_Fog block; _DrawMain's drawID -2 requests read it
 *     back (ov12:0x00a28438, outside this allocation). This replaces the
 *     unmodeled_0a3c[4] span named above.
 *   +0xa48 m_ActiveStudioMask: bit i is set when m_pStudios[i] is non-NIL
 *     (1 after _FullScreenStudio's single studio, 3 after
 *     _DoubleScreenStudio's two, matching the literal values each stores),
 *     read as a broadcast guard by _DrawMain (ov12:0x00a283a4, outside this
 *     allocation).
 *   +0xa4c m_FullScreenMode: 1 after _FullScreenStudio, 0 after
 *     _DoubleScreenStudio; _SetScreenSplitMode skips computing split
 *     rectangles for both studios while it is set.
 *   +0xa50 m_FullScreenParam, +0xa58 m_FadeParam: written alongside
 *     m_FullScreenMode/m_FadeCounter (2 and 0, 0 and 0) by this allocation's
 *     own functions, but nothing in this unit's disassembly reads either
 *     word, so no further role is claimed for them.
 *   +0xa54 m_FadeCounter: cleared by _InitDraw; the countdown RgDrawFadeIn
 *     (ov12:0x00a28510, outside this allocation) decrements by 20 per call,
 *     clearing m_Enabled once it drops below 50.
 */
struct RgDraw {
    int m_Enabled;
    unsigned char unmodeled_04[0xc];
    RgDrawRequest m_Requests[128];
    unsigned int m_RequestCount;
    unsigned char unmodeled_a14[0xc];
    RgFog m_Fog;
    RgDrawStudio *m_pDefaultStudio;
    RgDrawStudio *m_pStudios[2];
    unsigned int m_ActiveStudioMask;
    int m_FullScreenMode;
    int m_FullScreenParam;
    int m_FadeCounter;
    int m_FadeParam;
    unsigned char unmodeled_a5c[4];
};

#endif /* SRC_OV12_RG_DRAW_H */
