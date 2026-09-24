#ifndef INCLUDE_OV12_RG_DRAW_H
#define INCLUDE_OV12_RG_DRAW_H

typedef struct RgDrawStudio RgDrawStudio;

typedef struct RgDraw RgDraw;

typedef struct RgDrawView RgDrawView;

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

#endif /* INCLUDE_OV12_RG_DRAW_H */
