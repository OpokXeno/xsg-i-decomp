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

/*
 * The failed-allocation assert reuses "pData != NIL" (D_00A551A8), not the
 * "pPoly != NIL" text the rest of this TU validates a geometry pointer with;
 * the object is still the same RgGeom every RgGeomPoly accessor here calls
 * geom.
 */
RgGeom *CreateRgGeomPoly(void)
{
    RgGeom *geom;

    geom = RgHeapAlloc(InstanceOfRgHeap(), sizeof(RgGeomPoly), D_00A551B8, 36);
    if (geom == 0) {
        assert_prog(D_00A551A8, D_00A551B8, 37);
    }
    _InitRgGeomPoly(geom);
    return geom;
}

void InitRgGeomPoly(RgGeom *geom)
{
    if (geom == 0) {
        assert_prog(D_00A551D0, D_00A551B8, 44);
    }
    _InitRgGeomPoly(geom);
}

void RgGeomPolySetColiData(RgGeom *geom, RgColiData *coliData)
{
    if (geom == 0) {
        assert_prog(D_00A551D0, D_00A551B8, 55);
    }
    ((RgGeomPoly *)geom)->coliData = coliData;
}

void RgGeomPolySetLocal(RgGeom *geom, const RgMatrix local)
{
    if (geom != 0) {
        RgGeomLocalMatrices *matrices = RgGeomLocalMatricesAt(geom);

        XrgCopyMatrix(matrices->local, local);
        XrgInvMatrix(matrices->inverse_local, local);
    }
}

RgColiData *RgGeomPolyGetData(const RgGeom *geom)
{
    if (geom == 0) {
        assert_prog(D_00A551D0, D_00A551B8, 77);
    }
    return ((const RgGeomPoly *)geom)->coliData;
}

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
