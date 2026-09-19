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

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_dispmodel_impl", _Disp_00A45120);

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

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_dispmodel_impl", _InitRgDispModelImpl);

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_dispmodel_impl", CreateXrgDispModelImpl);
