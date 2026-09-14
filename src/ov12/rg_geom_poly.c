/*
 * OV12 original TU 55: 0x00a2d900..0x00a2dd58 (10 functions)
 */
#include "common.h"
#include "shared.h"
#include "rg_geom_poly.h"

#define RgGeomLocalMatricesAt(geom) \
    ((RgGeomLocalMatrices *)((unsigned char *)(geom) + 0x20))
#define RgGeomLocalMatricesConstAt(geom) \
    ((const RgGeomLocalMatrices *)((const unsigned char *)(geom) + 0x20))

INCLUDE_ASM("asm/nonmatchings/ov12/rg_geom_poly", _InitRgGeomPoly);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_geom_poly", CreateRgGeomPoly);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_geom_poly", InitRgGeomPoly);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_geom_poly", RgGeomPolySetColiData);

void RgGeomPolySetLocal(RgGeom *geom, const RgMatrix local)
{
    if (geom != 0) {
        RgGeomLocalMatrices *matrices = RgGeomLocalMatricesAt(geom);

        XrgCopyMatrix(matrices->local, local);
        XrgInvMatrix(matrices->inverse_local, local);
    }
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_geom_poly", RgGeomPolyGetData);

void RgGeomPolyGetLocal(const RgGeom *geom, RgMatrix destination)
{
    if (geom != 0) {
        XrgCopyMatrix(destination, RgGeomLocalMatricesConstAt(geom)->local);
    } else {
        XrgUnitMatrix(destination);
    }
}

void RgGeomPolyGetInvLocal(const RgGeom *geom, RgMatrix destination)
{
    if (geom != 0) {
        XrgCopyMatrix(destination, RgGeomLocalMatricesConstAt(geom)->inverse_local);
    } else {
        XrgUnitMatrix(destination);
    }
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_geom_poly", RgGeomPolyCheckBall);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_geom_poly", RgGeomPolyCheckRay);
