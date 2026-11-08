/*
 * OV12 original TU 55: 0x00a2d900..0x00a2dd58 (10 functions)
 */
#include "common.h"
#include "shared.h"
#include "ov12/assert_prog.h"
#include "ov12/rg_colidata.h"
#include "ov12/rg_geom.h"
#include "ov12/rg_geom_poly.h"
#include "ov12/rg_heap.h"
#include "ov12/xrg_rand_int.h"

const char geomPolyAssertDataNotNil[16] = "pData != NIL";

const char geomPolySourcePath[24] = "../rg_geom_poly.euc.c";

const char geomPolyAssertPolyNotNil[16] = "pPoly != NIL";

const char geomPolyAssertCollisionArgumentsNotNil[56] = "pPoly != NIL && pOrgArg != NIL && pResult != NIL";

/* Sets up a poly geometry with no collision data and identity local matrices. */
static void _InitRgGeomPoly(RgGeom *geom)
{
    RgGeomPoly *poly = (RgGeomPoly *)geom;

    if (geom == 0) {
        assert_prog(geomPolyAssertDataNotNil, geomPolySourcePath, 21);
    }
    RgGeomInit(geom);
    RgGeomSetType(geom, RG_GEOM_TYPE_POLY);
    poly->coliData = 0;
    XrgUnitMatrix(poly->local);
    XrgUnitMatrix(poly->inverseLocal);
}

/*
 * Allocates and initializes a poly geometry. The failed-allocation assert uses the
 * "pData != NIL" text, not the "pPoly != NIL" text the other checks here use.
 */
RgGeom *CreateRgGeomPoly(void)
{
    RgGeom *geom;

    geom = RgHeapAlloc(InstanceOfRgHeap(), sizeof(RgGeomPoly), geomPolySourcePath, 36);
    if (geom == 0) {
        assert_prog(geomPolyAssertDataNotNil, geomPolySourcePath, 37);
    }
    _InitRgGeomPoly(geom);
    return geom;
}

/* Initializes an existing poly geometry. */
void InitRgGeomPoly(RgGeom *geom)
{
    if (geom == 0) {
        assert_prog(geomPolyAssertPolyNotNil, geomPolySourcePath, 44);
    }
    _InitRgGeomPoly(geom);
}

void RgGeomPolySetColiData(RgGeom *geom, RgColiData *coliData)
{
    if (geom == 0) {
        assert_prog(geomPolyAssertPolyNotNil, geomPolySourcePath, 55);
    }
    ((RgGeomPoly *)geom)->coliData = coliData;
}

/* Sets the local matrix and its inverse. A null geometry is ignored. */
void RgGeomPolySetLocal(RgGeom *geom, const RgMatrix local)
{
    RgGeomPoly *poly = (RgGeomPoly *)geom;

    if (geom != 0) {
        XrgCopyMatrix(poly->local, local);
        XrgInvMatrix(poly->inverseLocal, local);
    }
}

RgColiData *RgGeomPolyGetData(const RgGeom *geom)
{
    if (geom == 0) {
        assert_prog(geomPolyAssertPolyNotNil, geomPolySourcePath, 77);
    }
    return ((const RgGeomPoly *)geom)->coliData;
}

/* Copies the local matrix to destination, or the identity when geom is null. */
void RgGeomPolyGetLocal(const RgGeom *geom, RgMatrix destination)
{
    const RgGeomPoly *poly = (const RgGeomPoly *)geom;

    if (geom != 0) {
        XrgCopyMatrix(destination, poly->local);
    } else {
        XrgUnitMatrix(destination);
    }
}

/* Copies the inverse local matrix to destination, or the identity when geom is null. */
void RgGeomPolyGetInvLocal(const RgGeom *geom, RgMatrix destination)
{
    const RgGeomPoly *poly = (const RgGeomPoly *)geom;

    if (geom != 0) {
        XrgCopyMatrix(destination, poly->inverseLocal);
    } else {
        XrgUnitMatrix(destination);
    }
}

/*
 * Tests a ball against the poly. The ball centre is moved into local space with
 * inverseLocal as a point, and on a hit the correction vector is moved back out
 * with the local matrix as a direction.
 */
int RgGeomPolyCheckBall(RgGeom *pPoly, RgColiBall *pOrgArg,
                        RgColiBallResult *pResult)
{
    RgGeomPoly *poly = (RgGeomPoly *)pPoly;
    RgColiBall ball;

    if (pPoly == 0 || pOrgArg == 0 || pResult == 0) {
        assert_prog(geomPolyAssertCollisionArgumentsNotNil, geomPolySourcePath, 108);
    }

    if (poly->coliData == 0) {
        return 0;
    }

    pOrgArg->m_center[3] = 1.0f;
    XrgApplyVector(ball.m_center, poly->inverseLocal, pOrgArg->m_center);
    ball.m_radius = pOrgArg->m_radius;

    if (RgColiDataVsBall(poly->coliData, &ball, pResult) == 0) {
        return 0;
    }

    pResult->m_correction[3] = 0.0f;
    XrgApplyVector(pResult->m_correction, poly->local, pResult->m_correction);
    return 1;
}

/*
 * Tests a segment against the poly. The start and vector are moved into local
 * space with inverseLocal, and on a hit the hit plane is moved back out with the
 * local matrix.
 */
int RgGeomPolyCheckRay(RgGeom *pPoly, RgColiSegment *pOrgArg,
                       RgColiSegmentHit *pResult)
{
    RgGeomPoly *poly = (RgGeomPoly *)pPoly;
    RgColiSegment ray;

    if (pPoly == 0 || pOrgArg == 0 || pResult == 0) {
        assert_prog(geomPolyAssertCollisionArgumentsNotNil, geomPolySourcePath, 134);
    }

    if (poly->coliData == 0) {
        return 0;
    }

    pOrgArg->m_start[3] = 1.0f;
    pOrgArg->m_vector[3] = 0.0f;
    /* Transforms the segment start and then its vector by inverseLocal on VU0. */
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
        : : "r"(ray.m_start), "r"(pOrgArg->m_start), "r"(poly->inverseLocal)
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
        : : "r"(ray.m_vector), "r"(pOrgArg->m_vector), "r"(poly->inverseLocal)
        : "memory");

    /* Tests the local segment against the collision data. */
    if (RgColiDataVsRay(poly->coliData, &ray, pResult) == 0) {
        return 0;
    }

    pResult->m_plane[3] = 0.0f;
    /* Moves the hit plane back out with the local matrix. */
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
        : : "r"(pResult->m_plane), "r"(pResult->m_plane), "r"(poly->local)
        : "memory");
    return 1;
}
