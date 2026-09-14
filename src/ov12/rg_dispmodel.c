/*
 * OV12 original TU 78: 0x00a44f08..0x00a45120 (10 functions)
 */
#include "common.h"
#include "shared.h"
#include "rg_dispmodel.h"

INCLUDE_ASM("asm/nonmatchings/ov12/rg_dispmodel", InitRgDispModel);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_dispmodel", DisposeRgDispModel);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_dispmodel", RgDispModelDisplay);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_dispmodel", RgDispModelSetMode);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_dispmodel", RgDispModelResetMode);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_dispmodel", RgDispModelSetLocal);

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

INCLUDE_ASM("asm/nonmatchings/ov12/rg_dispmodel", RgDispModelSetTransparent);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_dispmodel", RgDispModelSetBlight);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_dispmodel", RgDispModelGetTransparent);
