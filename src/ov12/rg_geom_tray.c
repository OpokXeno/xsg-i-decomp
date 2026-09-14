/*
 * OV12 original TU 56: 0x00a2dd58..0x00a2e698 (8 functions)
 */
#include "common.h"
#include "shared.h"
#include "rg_geom_tray.h"

#define RgGeomLocalMatricesAt(geom) \
    ((RgGeomLocalMatrices *)((unsigned char *)(geom) + 0x20))
#define RgGeomLocalMatricesConstAt(geom) \
    ((const RgGeomLocalMatrices *)((const unsigned char *)(geom) + 0x20))

INCLUDE_ASM("asm/nonmatchings/ov12/rg_geom_tray", _InitRgGeomTray);

void InitRgGeomTray(RgGeom *geom)
{
    _InitRgGeomTray(geom);
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_geom_tray", CreateRgGeomTray);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_geom_tray", RgGeomTraySetSize);

/*
 * The tray stores its local and inverse-local four-by-four matrices at byte
 * offsets 0x20 and 0x60.  The preceding geometry header is intentionally not
 * modelled here: these two routines only address the two evidenced matrix
 * objects and pass them to the established matrix helpers.
 */

void RgGeomTraySetLocal(void *tray, Matrix4 source)
{
    XrgCopyMatrix((float *)((char *)tray + 0x20), (const float *)source);
    XrgInvMatrix((float *)((char *)tray + 0x60), (const float *)source);
}

void RgGeomTrayGetLocal(void *tray, Matrix4 destination)
{
    XrgCopyMatrix((float *)destination, (const float *)((char *)tray + 0x20));
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_geom_tray", _CalcIntersect);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_geom_tray", RgGeomTrayCheckBall);
