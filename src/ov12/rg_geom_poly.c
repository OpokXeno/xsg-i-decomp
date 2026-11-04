/*
 * OV12 original TU 55: 0x00a2d900..0x00a2dd58 (10 functions)
 */
#include "common.h"
#include "shared.h"
#include "rg_geom_poly.h"

const char D_00A551A8[16] = "pData != NIL";

const char D_00A551B8[24] = "../rg_geom_poly.euc.c";

const char D_00A551D0[16] = "pPoly != NIL";

const char D_00A551E0[56] = "pPoly != NIL && pOrgArg != NIL && pResult != NIL";

#define RgGeomLocalMatricesAt(geom) \
    ((RgGeomLocalMatrices *)((unsigned char *)(geom) + 0x20))
#define RgGeomLocalMatricesConstAt(geom) \
    ((const RgGeomLocalMatrices *)((const unsigned char *)(geom) + 0x20))

/* RgGeomInit and RgGeomSetType are original functions of the base geometry
 * TU (rg_geom.c, ov12/tu051), already recovered as C. */
extern void RgGeomInit(RgGeom *pGeom);
extern void RgGeomSetType(RgGeom *pGeom, int type);

/* The type tag _InitRgGeomPoly passes to RgGeomSetType; no other OV12
 * translation unit names this value yet. */
#define RG_GEOM_TYPE_POLY 3

static void _InitRgGeomPoly(RgGeom *geom)
{
    RgGeomPoly *poly = (RgGeomPoly *)geom;

    if (geom == 0) {
        assert_prog(D_00A551A8, D_00A551B8, 21);
    }
    RgGeomInit(geom);
    RgGeomSetType(geom, RG_GEOM_TYPE_POLY);
    poly->coliData = 0;
    XrgUnitMatrix(poly->local);
    XrgUnitMatrix(poly->inverseLocal);
}

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

/*
 * pOrgArg is transformed into the poly's local space with inverseLocal, its
 * homogeneous component forced to 1.0 (a point, not a direction), then
 * bundled with the ball's own radius (the float just past pOrgArg's four
 * components) into a local Ball-shaped buffer RgColiDataVsBall reads through
 * its own void *pArg. On a hit, pResult's hit-point vector (its homogeneous
 * component forced to 0.0, a direction rather than a point) is transformed
 * back out of local space with the poly's local matrix.
 */
int RgGeomPolyCheckBall(RgGeom *pPoly, float *pOrgArg, float *pResult)
{
    RgGeomPoly *poly = (RgGeomPoly *)pPoly;
    struct {
        RgVector center;
        float radius;
    } ball;

    if (pPoly == 0 || pOrgArg == 0 || pResult == 0) {
        assert_prog(D_00A551E0, D_00A551B8, 108);
    }

    if (poly->coliData == 0) {
        return 0;
    }

    pOrgArg[3] = 1.0f;
    XrgApplyVector(ball.center, poly->inverseLocal, pOrgArg);
    ball.radius = pOrgArg[4];

    if (RgColiDataVsBall(poly->coliData, &ball, pResult) == 0) {
        return 0;
    }

    pResult[7] = 0.0f;
    XrgApplyVector(pResult + 4, poly->local, pResult + 4);
    return 1;
}

/*
 * pOrgArg holds the ray origin (pOrgArg[0..3], homogeneous component forced
 * to 1.0) and direction (pOrgArg[4..7], component forced to 0.0). Both are
 * transformed into poly space with inverseLocal into a local ray buffer that
 * RgColiDataVsRay reads. On a hit, the hit-point vector pResult (component
 * forced to 0.0) is transformed back out with the poly's local matrix.
 */
int RgGeomPolyCheckRay(RgGeom *pPoly, float *pOrgArg, float *pResult)
{
    RgGeomPoly *poly = (RgGeomPoly *)pPoly;
    struct {
        RgVector origin;
        RgVector direction;
    } ray;

    if (pPoly == 0 || pOrgArg == 0 || pResult == 0) {
        assert_prog(D_00A551E0, D_00A551B8, 134);
    }

    if (poly->coliData == 0) {
        return 0;
    }

    pOrgArg[3] = 1.0f;
    pOrgArg[7] = 0.0f;
    __asm__ __volatile__(
        "lqc2 $vf2, 0(%1)\n\t"
        "lqc2 $vf3, 0(%2)\n\t"
        "lqc2 $vf4, 16(%2)\n\t"
        "lqc2 $vf5, 32(%2)\n\t"
        "lqc2 $vf6, 48(%2)\n\t"
        "vmulax.xyzw ACCxyzw, vf3xyzw, vf2x\n\t"
        "vmadday.xyzw ACCxyzw, vf4xyzw, vf2y\n\t"
        "vmaddaz.xyzw ACCxyzw, vf5xyzw, vf2z\n\t"
        "vmaddw.xyzw vf2xyzw, vf6xyzw, vf2w\n\t"
        "sqc2 $vf2, 0(%0)\n\t"
        : : "r"(ray.origin), "r"(pOrgArg), "r"(poly->inverseLocal)
        : "memory");
    __asm__ __volatile__(
        "lqc2 $vf2, 0(%1)\n\t"
        "lqc2 $vf3, 0(%2)\n\t"
        "lqc2 $vf4, 16(%2)\n\t"
        "lqc2 $vf5, 32(%2)\n\t"
        "lqc2 $vf6, 48(%2)\n\t"
        "vmulax.xyzw ACCxyzw, vf3xyzw, vf2x\n\t"
        "vmadday.xyzw ACCxyzw, vf4xyzw, vf2y\n\t"
        "vmaddaz.xyzw ACCxyzw, vf5xyzw, vf2z\n\t"
        "vmaddw.xyzw vf2xyzw, vf6xyzw, vf2w\n\t"
        "sqc2 $vf2, 0(%0)\n\t"
        : : "r"(ray.direction), "r"(pOrgArg + 4), "r"(poly->inverseLocal)
        : "memory");

    if (RgColiDataVsRay(poly->coliData, &ray, pResult) == 0) {
        return 0;
    }

    pResult[3] = 0.0f;
    __asm__ __volatile__(
        "lqc2 $vf2, 0(%1)\n\t"
        "lqc2 $vf3, 0(%2)\n\t"
        "lqc2 $vf4, 16(%2)\n\t"
        "lqc2 $vf5, 32(%2)\n\t"
        "lqc2 $vf6, 48(%2)\n\t"
        "vmulax.xyzw ACCxyzw, vf3xyzw, vf2x\n\t"
        "vmadday.xyzw ACCxyzw, vf4xyzw, vf2y\n\t"
        "vmaddaz.xyzw ACCxyzw, vf5xyzw, vf2z\n\t"
        "vmaddw.xyzw vf2xyzw, vf6xyzw, vf2w\n\t"
        "sqc2 $vf2, 0(%0)\n\t"
        : : "r"(pResult), "r"(pResult), "r"(poly->local)
        : "memory");
    return 1;
}
