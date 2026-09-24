/*
 * OV12 original TU 79: 0x00a45120..0x00a454f8 (5 functions)
 */
#include "common.h"
#include "shared.h"
#include "ov12/rg_dispmodel.h"
#include "ov12/rg_draw.h"

extern void assert_prog(const char *expression, const char *source_file,
                        int line);
extern RgDraw *InstanceOfRgDraw(void);

/*
 * RgDrawReq (ov12:0x00a28028, still assembly) queues one draw request: it
 * stores the object, the draw callback, the clear callback and the last two
 * words into one 20-byte entry of the RgDraw request list at +0xa10.
 * XrgPaint2DFlush passes a real clear callback and the paint's prio and
 * drawID in the same argument slots; this caller has no clear callback and
 * requests prio 0 with drawID -1.
 */
extern void RgDrawReq(RgDraw *pDraw, RgDispModel *pDisp,
                      void (*drawFunc)(RgDispModel *pDisp, void *pStudio),
                      void (*clearFunc)(RgDispModel *pDisp, void *pStudio),
                      int prio, int drawID);

/*
 * The draw callback's second argument is the studio being drawn:
 * _DrawMain (ov12:0x00a28320) calls each request's callback with the
 * object and an entry of RgDraw's m_pStudios, and _Disp reads the RgFog
 * pointer at the studio's +0x4 (the pFog word _InitRgDrawStudio stores) to
 * set the model fog distance and color.
 */
static void _Disp(RgDispModel *pDisp, void *pStudio);

/*
 * External file-backed witnesses, not candidate-emitted data: this window is
 * asm-owned scaffold data (splat names, no config/symbols/ov12.txt entry).
 *
 * ov12:0x00a58750 contains the assertion expression "pDisp != NIL".
 * ov12:0x00a58760 contains the source filename "../xrg_dispmodel_impl.euc.c".
 * ov12:0x00a58780 contains the assertion expression "pXtxData != NIL".
 * ov12:0x00a58790 contains the assertion expression "pLexData != NIL".
 */
extern const char D_00A58750[];
extern const char D_00A58760[];
extern const char D_00A58780[];
extern const char D_00A58790[];

/*
 * The object CreateXrgDispModelImpl (ov12:0x00a453cc) allocates: 112 bytes
 * from the RgHeap, initialised as a RgDispModel by _InitRgDispModelImpl and
 * then handed to RgDispModelDisplay through the base object's dispMethod.
 * Past the base object it keeps the two link-data blocks
 * _InitRgDispModelImpl receives and asserts on, named by the assertion
 * expressions "pXtxData != NIL" and "pLexData != NIL": the texture block
 * _Disp passes to nmlModelSetTexture, which accepts it only when its first
 * three bytes spell "XTX" (main:0x0022fe60), and the model block _Disp
 * passes to nmlModelEntry. Offsets 0x54..0x5f stay an explicit unmodeled
 * span; no claimed function reads or writes the eight bytes past pLexData,
 * so the type ends with the two link-data pointers.
 */
typedef struct XrgDispModelImpl {
    RgDispModel model;
    unsigned char unmodeled_54[0xc];
    const char *pXtxData;
    const char *pLexData;
} XrgDispModelImpl;

extern void nmlModelEntry(const char *pLexData);
extern void nmlModelSetAlpha(int alpha);
extern void nmlModelSetFogCol(float *);
extern void nmlModelSetFogDist(float, float, float, float);
extern void nmlModelSetMapClip(int enabled);
extern void nmlModelSetPlace(RgMatrix matrix);
extern void nmlModelSetRenderLevel(int);
extern void nmlModelSetTexture(const char *pXtxData);
extern void nmlModelSetToumei(int enabled);
extern void nmlModelSetTransparency(float transparency);
extern void nmlModelSetZwrite(int enabled);

static void _Disp(RgDispModel *pDisp, void *pStudio)
{
    XrgDispModelImpl *pImpl;
    RgDrawStudio *pDrawStudio;
    RgFog *pFog;
    float transparent;
    float color[4];

    if (pDisp == 0) {
        assert_prog(D_00A58750, D_00A58760, 47);
    }

    pImpl = (XrgDispModelImpl *)pDisp;
    if (pImpl->pXtxData == 0 || pImpl->pLexData == 0) {
        return;
    }

    transparent = pDisp->transparent;
    if (transparent <= 0.0f) {
        return;
    }

    if (transparent >= 0.0f && transparent < 0.95f) {
        /*
         * color is opaque white with the model's own transparency as its
         * fourth component here; the only call that reads the array back is
         * nmlModelSetFogCol below, after the fog pass has replaced the first
         * three components with the studio's fog color.
         */
        color[3] = transparent;
        color[0] = color[1] = color[2] = 1.0f;
        nmlModelSetTransparency(transparent);
        nmlModelSetToumei(1);
        nmlModelSetZwrite(1);
        if (pDisp->mode & 4) {
            nmlModelSetAlpha(0x48);
        }
    }

    if (pStudio != 0 && (pDisp->mode & 1)) {
        pDrawStudio = pStudio;
        pFog = pDrawStudio->m_pFog;
        color[0] = pFog->color[0];
        color[1] = pFog->color[1];
        color[2] = pFog->color[2];
        nmlModelSetFogCol(color);
        nmlModelSetFogDist(pFog->dist[0], pFog->dist[1], pFog->dist[2],
                           pFog->dist[3]);
    }

    if (pDisp->mode & 8) {
        nmlModelSetMapClip(1);
    }
    nmlModelSetPlace(pDisp->local);
    nmlModelSetTexture(pImpl->pXtxData);
    nmlModelSetRenderLevel(2);
    nmlModelEntry(pImpl->pLexData);
}

static void _DispRgDispModelImpl(RgDispModel *pDisp)
{
    RgDrawReq(InstanceOfRgDraw(), pDisp, &_Disp, 0, 0, -1);
}

/*
 * External file-backed witnesses, not candidate-emitted data: this window is
 * asm-owned scaffold data (splat names, no config/symbols/ov12.txt entry).
 *
 * ov12:0x00a58750 contains the assertion expression "pDisp != NIL".
 * ov12:0x00a58760 contains the source filename "../xrg_dispmodel_impl.euc.c".
 */
extern const char D_00A58750[];
extern const char D_00A58760[];

static void _DestructRgDispModelImpl(RgDispModel *pDisp)
{
    if (pDisp == 0) {
        assert_prog(D_00A58750, D_00A58760, 119);
    }
}

extern void InitRgDispModel(RgDispModel *pDispModel);

static void _InitRgDispModelImpl(XrgDispModelImpl *pDisp, const char *pXtxData,
                                 const char *pLexData)
{
    if (pDisp == 0) {
        assert_prog(D_00A58750, D_00A58760, 126);
    }
    if (pXtxData == 0) {
        assert_prog(D_00A58780, D_00A58760, 127);
    }
    if (pLexData == 0) {
        assert_prog(D_00A58790, D_00A58760, 128);
    }

    InitRgDispModel(&pDisp->model);
    pDisp->model.dispMethod = _DispRgDispModelImpl;
    pDisp->model.destructMethod = _DestructRgDispModelImpl;
    pDisp->pXtxData = pXtxData;
    pDisp->pLexData = pLexData;
}

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_dispmodel_impl", CreateXrgDispModelImpl);
