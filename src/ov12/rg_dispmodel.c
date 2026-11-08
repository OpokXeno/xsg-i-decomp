/*
 * OV12 original TU 78: 0x00a44f08..0x00a45120 (10 functions)
 */
#include "common.h"
#include "shared.h"
#include "ov12/rg_dispmodel.h"
#include "ov12/assert_prog.h"
#include "ov12/rg_heap.h"
#include "ov12/xrg_rand_int.h"


const char dispModelAssertNotNil[] = "pDispModel != NIL";
const char dispModelSourcePath[] = "../rg_dispmodel.euc.c";

/* Sets the default methods, mode, transparency and blight, and resets the local matrix to identity. */
void InitRgDispModel(RgDispModel *pDispModel)
{
    if (pDispModel == 0) {
        assert_prog(dispModelAssertNotNil, dispModelSourcePath, 18);
    }

    pDispModel->dispMethod = 0;
    pDispModel->destructMethod = 0;
    pDispModel->mode = 0;
    pDispModel->transparent = 1.0f;
    pDispModel->blight = 1.0f;
    XrgUnitMatrix(pDispModel->local);
}

/* Runs the destruct method if one is set, then frees the model from the RG heap. */
void DisposeRgDispModel(RgDispModel *pDispModel)
{
    if (pDispModel == 0) {
        assert_prog(dispModelAssertNotNil, dispModelSourcePath, 30);
    }

    if (pDispModel->destructMethod != 0) {
        pDispModel->destructMethod(pDispModel);
    }
    RgHeapFree(InstanceOfRgHeap(), pDispModel, dispModelSourcePath, 33);
}

/* Calls the display method unless the model has the hidden mode bit set. */
void RgDispModelDisplay(RgDispModel *pDispModel)
{
    if (pDispModel == 0) {
        assert_prog(dispModelAssertNotNil, dispModelSourcePath, 41);
    }

    if (pDispModel->dispMethod != 0 && !(pDispModel->mode & RG_DISP_MODEL_MODE_HIDDEN)) {
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

void RgDispModelGetLocal(RgDispModel *pDispModel, RgMatrix destination)
{
    if (pDispModel != 0)
        XrgCopyMatrix(destination, pDispModel->local);
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
