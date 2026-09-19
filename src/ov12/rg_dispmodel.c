/*
 * OV12 original TU 78: 0x00a44f08..0x00a45120 (10 functions)
 */
#include "common.h"
#include "shared.h"
#include "rg_dispmodel.h"

extern void assert_prog(const char *expression, const char *source_file,
                        int line);
extern RgHeap *InstanceOfRgHeap(void);
extern void RgHeapFree(void *heap, void *pointer, const char *source_file,
                       int line);
extern void XrgUnitMatrix(RgMatrix destination);

/*
 * External file-backed witnesses, not candidate-emitted data: this window is
 * asm-owned scaffold data (splat names, no config/symbols/ov12.txt entry).
 *
 * ov12:0x00a58720 contains the assertion expression "pDispModel != NIL".
 * ov12:0x00a58738 contains the source filename "../rg_dispmodel.euc.c".
 */
extern const char D_00A58720[];
extern const char D_00A58738[];

void InitRgDispModel(RgDispModel *pDispModel)
{
    if (pDispModel == 0) {
        assert_prog(D_00A58720, D_00A58738, 18);
    }

    pDispModel->dispMethod = 0;
    pDispModel->destructMethod = 0;
    pDispModel->mode = 0;
    pDispModel->transparent = 1.0f;
    pDispModel->blight = 1.0f;
    XrgUnitMatrix(pDispModel->local);
}

void DisposeRgDispModel(RgDispModel *pDispModel)
{
    if (pDispModel == 0) {
        assert_prog(D_00A58720, D_00A58738, 30);
    }

    if (pDispModel->destructMethod != 0) {
        pDispModel->destructMethod(pDispModel);
    }
    RgHeapFree(InstanceOfRgHeap(), pDispModel, D_00A58738, 33);
}

void RgDispModelDisplay(RgDispModel *pDispModel)
{
    if (pDispModel == 0) {
        assert_prog(D_00A58720, D_00A58738, 41);
    }

    if (pDispModel->dispMethod != 0 && !(pDispModel->mode & 2)) {
        pDispModel->dispMethod(pDispModel);
    }
}

void RgDispModelSetMode(RgDispModel *pDispModel, int mode)
{
    if (pDispModel != 0) {
        pDispModel->mode |= mode;
    }
}

void RgDispModelResetMode(RgDispModel *pDispModel, int mode)
{
    if (pDispModel != 0) {
        pDispModel->mode &= ~mode;
    }
}

void RgDispModelSetLocal(RgDispModel *pDispModel, const RgMatrix source)
{
    if (pDispModel != 0) {
        XrgCopyMatrix(pDispModel->local, source);
    }
}

/*
 * RgDispModel keeps its local four-by-four matrix at byte offset zero.  The
 * neighboring setter passes the model pointer directly to XrgCopyMatrix,
 * while the getter reverses the argument order for the destination copy.
 */

void RgDispModelGetLocal(void *model, RgMatrix destination)
{
    if (model != 0)
        XrgCopyMatrix(destination, (float *)model);
}

void RgDispModelSetTransparent(RgDispModel *pDispModel, float transparent)
{
    if (pDispModel != 0) {
        pDispModel->transparent = transparent;
    }
}

void RgDispModelSetBlight(RgDispModel *pDispModel, float blight)
{
    if (pDispModel != 0) {
        pDispModel->blight = blight;
    }
}

float RgDispModelGetTransparent(RgDispModel *pDispModel)
{
    float transparent;

    transparent = 0.0f;
    if (pDispModel != 0) {
        transparent = pDispModel->transparent;
    }
    return transparent;
}
