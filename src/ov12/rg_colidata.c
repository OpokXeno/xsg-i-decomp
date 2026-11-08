/*
 * OV12 original TU 62: 0x00a31970..0x00a32688 (14 functions)
 */
#include "common.h"
#include "shared.h"
#include "ov12/assert_prog.h"
#include "ov12/rg_colidata.h"
#include "ov12/rg_heap.h"
#include "ov12/xrg_rand_int.h"

struct RgColiTriangle {
    RgVector m_vertices[3];
    RgVector m_plane;
    RgMatrix m_planeMatrix;
};


/* Per-triangle result of the ball and segment tests: a hit, or the reason
 * the triangle was missed (any negative value). */
enum {
    RG_COLI_HIT = 1,
    RG_COLI_MISS_OUT_OF_RANGE = -1,
    RG_COLI_MISS_OUTSIDE_TRIANGLE = -2,
    RG_COLI_MISS_BEYOND_SEGMENT = -3,
    RG_COLI_MISS_NOT_FACING = -4
};

/* Tolerance of the segment test's side and facing comparisons. */
#define RG_COLI_EPSILON 1e-4f

/* The quad prism template: 8 vertices and 6 quads, 2 triangles each. */
#define RG_COLI_PRISM_VERTICES 8
#define RG_COLI_PRISM_QUADS 6
#define RG_COLI_PRISM_TRIANGLES (RG_COLI_PRISM_QUADS * 2)

/*
 * Quad prism template: 0x00a55590 holds eight vertices, and 0x00a55610 holds
 * six quads of four vertex indices each.
 */
typedef struct RgColiQuadVertices {
    DoublewordAlignedVector4 m_vertices[RG_COLI_PRISM_VERTICES];
} RgColiQuadVertices;

typedef union RgColiQuadIndices {
    DoublewordAlignedVector4 m_vectors[RG_COLI_PRISM_QUADS];
    int m_quads[RG_COLI_PRISM_QUADS][4];
} RgColiQuadIndices;

extern const RgColiQuadVertices coliDataQuadPrismVertexTable;
extern const RgColiQuadIndices coliDataQuadPrismIndexTable;

const char coliDataAssertDataNotNil[] = "pData != NIL";

const char coliDataSourcePath[] = "../rg_colidata.euc.c";

const char coliDataAssertCapacityPositive[] = "nCapa > 0";

const char coliDataAssertTrianglesNotNil[] = "pData->m_pTriangles != NIL";

const char coliDataAssertTriangleCountPositive[] = "nNumOfTriangles > 0";

const char coliDataAssertTriangleCapacityLeftOver[] = "pData->m_nCapaOfTri > pData->m_nNumOfTri";

const char coliDataAssertBallArgumentsNotNil[48] = "pPoly != NIL && pArg != NIL && pResult != NIL";

/*
 * Stores the three vertices of a triangle and its face plane, and sets up the
 * three edge side planes and the face plane as the columns of m_planeMatrix.
 */
static void _InitTriColi(RgColiTriangle *triangle, void *pV0, void *pV1,
                         void *pV2)
{
    RgVector edge;
    RgVector cross;
    RgVector facePlane;
    RgVector sidePlanes[3];
    int component;
    int plane;

    XrgUnitMatrix(triangle->m_planeMatrix);
    XrgClearVector(triangle->m_vertices[0]);
    XrgClearVector(triangle->m_vertices[1]);
    XrgClearVector(triangle->m_vertices[2]);
    XrgCopyVectorXYZ(triangle->m_vertices[0], pV0);
    XrgCopyVectorXYZ(triangle->m_vertices[1], pV1);
    XrgCopyVectorXYZ(triangle->m_vertices[2], pV2);
    XrgCalcPlane(facePlane, pV0, pV1, pV2);
    XrgCopyVector(triangle->m_plane, facePlane);

    XrgSubVector(edge, pV1, pV0);
    XrgOuterVector(cross, facePlane, edge);
    XrgNormalizeVector(sidePlanes[0], cross);
    sidePlanes[0][3] = -XrgInnerVector(sidePlanes[0], pV0);

    XrgSubVector(edge, pV2, pV1);
    XrgOuterVector(cross, facePlane, edge);
    XrgNormalizeVector(sidePlanes[1], cross);
    sidePlanes[1][3] = -XrgInnerVector(sidePlanes[1], pV1);

    XrgSubVector(edge, pV0, pV2);
    XrgOuterVector(cross, facePlane, edge);
    XrgNormalizeVector(sidePlanes[2], cross);
    sidePlanes[2][3] = -XrgInnerVector(sidePlanes[2], pV2);

    for (plane = 0; plane < 3; plane++) {
        for (component = 0; component < 4; component++) {
            triangle->m_planeMatrix[component * 4 + plane] =
                sidePlanes[plane][component];
        }
    }
    for (plane = 0; plane < 4; plane++) {
        triangle->m_planeMatrix[plane * 4 + 3] = facePlane[plane];
    }
}

static void _AllocDataMemory(RgColiData *pData, int nCapa)
{
    if (pData == 0) {
        assert_prog(coliDataAssertDataNotNil, coliDataSourcePath, 0xAC);
    }
    if (nCapa <= 0) {
        assert_prog(coliDataAssertCapacityPositive, coliDataSourcePath, 0xAD);
    }
    pData->m_pTriangles = RgHeapAlloc(InstanceOfRgHeap(), nCapa * sizeof(RgColiTriangle),
                                      coliDataSourcePath, 0xAE);
    if (pData->m_pTriangles == 0) {
        assert_prog(coliDataAssertTrianglesNotNil, coliDataSourcePath, 0xAF);
    }
    pData->m_nCapacity = nCapa;
    pData->m_nTriangles = 0;
}

/*
 * Adds up the push-out correction of every triangle the ball overlaps into
 * pHit. Returns 1 when at least one triangle was hit, 0 otherwise.
 */
static int _CheckIntersectBall(RgColiData *pPoly, RgColiBall *pArg, RgColiBallResult *pHit)
{
    int i;

    if (pPoly->m_pTriangles == 0 || pPoly->m_nTriangles <= 0) {
        return 0;
    }

    XrgClearVector(pHit->m_correction);
    pHit->m_nHits = 0;
    for (i = 0; i < pPoly->m_nTriangles; i++) {
        RgVector delta;
        AlignedVector4 planeDistance;
        AlignedVector4 ball;
        RgColiTriangle *pTriangle;
        int status;
        float *pMatrix;
        float radius;

        pTriangle = &pPoly->m_pTriangles[i];
        pMatrix = pTriangle->m_planeMatrix;
        radius = pArg->m_radius;
        __asm__ __volatile__(
            "lqc2 $vf31, 0(%1)\n\t"
            "sqc2 $vf31, 0(%0)\n\t"
            : : "r"(&ball), "r"(pArg) : "memory");
        ball.w = 1.0f;
        /* Signed distances from the ball centre to the triangle's four planes. */
        __asm__ __volatile__(
            "lqc2 $vf2, 0(%2)\n\t"
            "lqc2 $vf3, 0(%1)\n\t"
            "lqc2 $vf4, 16(%1)\n\t"
            "lqc2 $vf5, 32(%1)\n\t"
            "lqc2 $vf6, 48(%1)\n\t"
            "vmulax.xyzw ACCxyzw, vf3xyzw, vf2x\n\t"
            "vmadday.xyzw ACCxyzw, vf4xyzw, vf2y\n\t"
            "vmaddaz.xyzw ACCxyzw, vf5xyzw, vf2z\n\t"
            "vmaddw.xyzw vf2xyzw, vf6xyzw, vf2w\n\t"
            "sqc2 $vf2, 0(%0)\n\t"
            : : "r"(&planeDistance), "r"(pTriangle->m_planeMatrix), "r"(&ball)
            : "memory");
        if (!(planeDistance.w < -radius || radius < planeDistance.w)) {
            status = RG_COLI_MISS_OUTSIDE_TRIANGLE;
            if (planeDistance.x >= 0.0f && planeDistance.y >= 0.0f && planeDistance.z >= 0.0f) {
                delta[0] = pMatrix[3] * (radius - planeDistance.w);
                delta[1] = pMatrix[7] * (radius - planeDistance.w);
                delta[2] = pMatrix[11] * (radius - planeDistance.w);
                delta[3] = 0.0f;
                status = RG_COLI_HIT;
            }
        } else {
            status = RG_COLI_MISS_OUT_OF_RANGE;
        }
        if (status >= 0) {
            XrgAddVector(pHit->m_correction, pHit->m_correction, delta);
            pHit->m_nHits++;
        }
    }
    if (pHit->m_nHits != 0) {
        return 1;
    }
    return 0;
}

static void _InitColiData(RgColiData *pData)
{
    if (pData == 0) {
        assert_prog(coliDataAssertDataNotNil, coliDataSourcePath, 0xEB);
    }
    pData->m_pTriangles = 0;
    pData->m_nTriangles = 0;
    pData->m_nCapacity = 0;
}

static void _DisposeColiData(RgColiData *pData)
{
    if (pData == 0) {
        assert_prog(coliDataAssertDataNotNil, coliDataSourcePath, 243);
    }
    if (pData->m_pTriangles != 0) {
        RgHeapFree(InstanceOfRgHeap(), pData->m_pTriangles, coliDataSourcePath, 245);
    }
    _InitColiData(pData);
}

/* Replaces the triangle array with an empty array of nCapa entries. */
static void _InitTriangles(RgColiData *pData, int nCapa)
{
    if (pData == 0) {
        assert_prog(coliDataAssertDataNotNil, coliDataSourcePath, 252);
    }
    if (nCapa <= 0) {
        assert_prog(coliDataAssertTriangleCountPositive, coliDataSourcePath, 253);
    }

    _DisposeColiData(pData);
    _AllocDataMemory(pData, nCapa);
}

/* Appends a triangle built from three vertices. The array must have room left. */
static void _AddTriangle(RgColiData *pData, void *pV0, void *pV1, void *pV2)
{
    /* separate load keeps the new triangle's index in its own register */
    int nIndex;
    int nTriangles;

    nIndex = pData->m_nTriangles;
    nTriangles = nIndex;

    if (pData->m_pTriangles == 0) {
        assert_prog(coliDataAssertTrianglesNotNil, coliDataSourcePath, 264);
        nTriangles = pData->m_nTriangles;
    }

    if (nTriangles >= pData->m_nCapacity) {
        assert_prog(coliDataAssertTriangleCapacityLeftOver, coliDataSourcePath, 265);
    }

    _InitTriColi(&pData->m_pTriangles[nIndex], pV0, pV1, pV2);
    pData->m_nTriangles++;
}

RgColiData *CreateRgColiData(void)
{
    RgColiData *pData;

    pData = RgHeapAlloc(InstanceOfRgHeap(), sizeof(RgColiData), coliDataSourcePath, 275);
    _InitColiData(pData);
    return pData;
}

/*
 * Creates a collision set of the quad prism template, scaled to width in x
 * and z and to height in y.
 */
RgColiData *CreateRgColiDataQuadPrism(float width, float height)
{
    RgColiData *pData;
    RgColiQuadVertices vertices;
    RgColiQuadIndices indices;
    int *pIndex;
    float *pCoord;
    int i;

    pData = CreateRgColiData();
    vertices = coliDataQuadPrismVertexTable;
    pIndex = indices.m_quads[0];
    indices = coliDataQuadPrismIndexTable;
    RgColiDataInitTriangles(pData, RG_COLI_PRISM_TRIANGLES);
    pCoord = &vertices.m_vertices[0].z;
    for (i = RG_COLI_PRISM_VERTICES - 1; i >= 0; i--, pCoord += 4) {
        pCoord[-2] *= width;
        pCoord[-1] *= height;
        pCoord[0] *= width;
    }
    pIndex += 1;
    for (i = RG_COLI_PRISM_QUADS - 1; i >= 0; i--, pIndex += 4) {
        RgColiDataAddTriangle(pData, &vertices.m_vertices[pIndex[-1]],
                              &vertices.m_vertices[pIndex[1]],
                              &vertices.m_vertices[pIndex[0]]);
        RgColiDataAddTriangle(pData, &vertices.m_vertices[pIndex[0]],
                              &vertices.m_vertices[pIndex[1]],
                              &vertices.m_vertices[pIndex[2]]);
    }
    return pData;
}

void DisposeRgColiData(RgColiData *pData)
{
    if (pData == 0) {
        assert_prog(coliDataAssertDataNotNil, coliDataSourcePath, 0x145);
    }
    _DisposeColiData(pData);
    RgHeapFree(InstanceOfRgHeap(), pData, coliDataSourcePath, 0x147);
}

void RgColiDataInitTriangles(RgColiData *pData, int nCapa)
{
    if (pData == 0) {
        assert_prog(coliDataAssertDataNotNil, coliDataSourcePath, 0x150);
    }
    _InitTriangles(pData, nCapa);
}

void RgColiDataAddTriangle(RgColiData *pData, void *pV0, void *pV1, void *pV2)
{
    if (pData == 0) {
        assert_prog(coliDataAssertDataNotNil, coliDataSourcePath, 0x156);
    }
    _AddTriangle(pData, pV0, pV1, pV2);
}

int RgColiDataVsBall(RgColiData *pPoly, RgColiBall *pBall,
                     RgColiBallResult *pResult)
{
    if (pPoly == 0 || pBall == 0 || pResult == 0) {
        assert_prog(coliDataAssertBallArgumentsNotNil, coliDataSourcePath, 0x18D);
    }
    return _CheckIntersectBall(pPoly, pBall, pResult);
}

extern const char coliDataAssertSegmentNotNil[];
extern const char coliDataAssertRayResultNotNil[];

/*
 * Finds the first triangle, in array order, that the segment enters through its
 * front face. On a hit, stores the triangle's plane and the hit fraction in
 * pResult and returns 1; returns 0 when no triangle is hit.
 */
int RgColiDataVsRay(RgColiData *pData, RgColiSegment *pSegment,
                    RgColiSegmentHit *pResult)
{
    int i;
    int status;
    float fraction;
    AlignedVector4 edges[3];
    AlignedVector4 cross;
    AlignedVector4 offset;

    if (pData == 0) {
        assert_prog(coliDataAssertDataNotNil, coliDataSourcePath, 411);
    }
    if (pSegment == 0) {
        assert_prog(coliDataAssertSegmentNotNil, coliDataSourcePath, 412);
    }
    if (pResult == 0) {
        assert_prog(coliDataAssertRayResultNotNil, coliDataSourcePath, 413);
    }
    if (pData->m_pTriangles == 0 || pData->m_nTriangles <= 0) {
        return 0;
    }

    for (i = 0; i < pData->m_nTriangles; i++) {
        RgColiTriangle *pTriangle = &pData->m_pTriangles[i];
        float *pPlane = pTriangle->m_plane;
        float *pVector = pSegment->m_vector;
        float dot;
        float denom;
        float t;
        float *pVertex1 = pTriangle->m_vertices[1];

        /* Triangle edge vectors; the side tests below use them. */
        __asm__ __volatile__(
            "lqc2 $vf30, 0(%1)\n\t"
            "lqc2 $vf31, 0(%2)\n\t"
            "vsub.xyzw $vf30, $vf30, $vf31\n\t"
            "sqc2 $vf30, 0(%0)\n\t"
            : : "r"(&edges[0]), "r"(pVertex1),
                "r"(pTriangle->m_vertices[0]) : "memory", "$2");
        __asm__ __volatile__(
            "lqc2 $vf30, 0(%1)\n\t"
            "lqc2 $vf31, 0(%2)\n\t"
            "vsub.xyzw $vf30, $vf30, $vf31\n\t"
            "sqc2 $vf30, 0(%0)\n\t"
            : : "r"(&edges[1]), "r"(pTriangle->m_vertices[2]),
                "r"(pVertex1) : "memory", "$2");
        __asm__ __volatile__(
            "lqc2 $vf30, 0(%1)\n\t"
            "lqc2 $vf31, 0(%2)\n\t"
            "vsub.xyzw $vf30, $vf30, $vf31\n\t"
            "sqc2 $vf30, 0(%0)\n\t"
            : : "r"(&edges[2]), "r"(pTriangle->m_vertices[0]),
                "r"(pTriangle->m_vertices[2]) : "memory", "$2");

        __asm__ __volatile__(
            "lqc2 $vf30, 0(%1)\n\t"
            "lqc2 $vf31, 0(%2)\n\t"
            "vopmula.xyz ACCxyz, vf30xyz, vf31xyz\n\t"
            "vopmsub.xyz vf30xyz, vf31xyz, vf30xyz\n\t"
            "sqc2 $vf30, 0(%0)\n\t"
            : : "r"(&cross), "r"(&edges[0]), "r"(pVector)
            : "memory", "$2");
        __asm__ __volatile__(
            "lqc2 $vf30, 0(%1)\n\t"
            "lqc2 $vf31, 0(%2)\n\t"
            "vsub.xyzw $vf30, $vf30, $vf31\n\t"
            "sqc2 $vf30, 0(%0)\n\t"
            : : "r"(&offset), "r"(pSegment->m_start),
                "r"(pTriangle->m_vertices[0]) : "memory", "$2");
        if (offset.x * cross.x + offset.y * cross.y + offset.z * cross.z <= -RG_COLI_EPSILON) {
            status = RG_COLI_MISS_OUTSIDE_TRIANGLE;
        } else {
            __asm__ __volatile__(
                "lqc2 $vf30, 0(%1)\n\t"
                "lqc2 $vf31, 0(%2)\n\t"
                "vopmula.xyz ACCxyz, vf30xyz, vf31xyz\n\t"
                "vopmsub.xyz vf30xyz, vf31xyz, vf30xyz\n\t"
                "sqc2 $vf30, 0(%0)\n\t"
                : : "r"(&cross), "r"(&edges[1]), "r"(pVector)
                : "memory", "$2");
            __asm__ __volatile__(
                "lqc2 $vf30, 0(%1)\n\t"
                "lqc2 $vf31, 0(%2)\n\t"
                "vsub.xyzw $vf30, $vf30, $vf31\n\t"
                "sqc2 $vf30, 0(%0)\n\t"
                : : "r"(&offset), "r"(pSegment->m_start),
                    "r"(pVertex1) : "memory", "$2");
            if (offset.x * cross.x + offset.y * cross.y + offset.z * cross.z <= -RG_COLI_EPSILON) {
                status = RG_COLI_MISS_OUTSIDE_TRIANGLE;
            } else {
                __asm__ __volatile__(
                    "lqc2 $vf30, 0(%1)\n\t"
                    "lqc2 $vf31, 0(%2)\n\t"
                    "vopmula.xyz ACCxyz, vf30xyz, vf31xyz\n\t"
                    "vopmsub.xyz vf30xyz, vf31xyz, vf30xyz\n\t"
                    "sqc2 $vf30, 0(%0)\n\t"
                    : : "r"(&cross), "r"(&edges[2]), "r"(pVector)
                    : "memory", "$2");
                __asm__ __volatile__(
                    "lqc2 $vf30, 0(%1)\n\t"
                    "lqc2 $vf31, 0(%2)\n\t"
                    "vsub.xyzw $vf30, $vf30, $vf31\n\t"
                    "sqc2 $vf30, 0(%0)\n\t"
                    : : "r"(&offset), "r"(pSegment->m_start),
                        "r"(pTriangle->m_vertices[2]) : "memory", "$2");
                if (offset.x * cross.x + offset.y * cross.y + offset.z * cross.z <= -RG_COLI_EPSILON) {
                    status = RG_COLI_MISS_OUTSIDE_TRIANGLE;
                } else {
                    /* Facing test, then the parameter t where the segment meets the plane. */
                    denom = pPlane[0] * pVector[0]
                          + pPlane[1] * pVector[1]
                          + pPlane[2] * pVector[2];
                    if (denom >= -RG_COLI_EPSILON) {
                        status = RG_COLI_MISS_NOT_FACING;
                    } else {
                        t = -(pPlane[0] * pSegment->m_start[0]
                              + pPlane[1] * pSegment->m_start[1]
                              + pPlane[2] * pSegment->m_start[2]
                              + pPlane[3]) / denom;
                        if (0.0f <= t && t <= 1.0f) {
                            fraction = t;
                            status = RG_COLI_HIT;
                        } else {
                            status = RG_COLI_MISS_BEYOND_SEGMENT;
                        }
                    }
                }
            }
        }
        if (status >= 0) {
            __asm__ __volatile__(
                "lqc2 $vf31, 0(%1)\n\t"
                "sqc2 $vf31, 0(%0)\n\t"
                : : "r"(pResult), "r"(pPlane) : "memory", "$2");
            pResult->m_fraction = fraction;
            return 1;
        }
    }
    return 0;
}



const RgColiQuadVertices coliDataQuadPrismVertexTable = {
    {
        { -1.0f, 0.0f, -1.0f, 1.0f },
        { 1.0f, 0.0f, -1.0f, 1.0f },
        { -1.0f, 0.0f, 1.0f, 1.0f },
        { 1.0f, 0.0f, 1.0f, 1.0f },
        { -1.0f, 1.0f, -1.0f, 1.0f },
        { 1.0f, 1.0f, -1.0f, 1.0f },
        { -1.0f, 1.0f, 1.0f, 1.0f },
        { 1.0f, 1.0f, 1.0f, 1.0f }
    }
};

const RgColiQuadIndices coliDataQuadPrismIndexTable = {
    .m_quads = {
        { 1, 0, 3, 2 },
        { 4, 5, 6, 7 },
        { 5, 4, 1, 0 },
        { 7, 5, 3, 1 },
        { 6, 7, 2, 3 },
        { 4, 6, 0, 2 }
    }
};

const char coliDataAssertSegmentNotNil[16] = "pArg != NIL";

const char coliDataAssertRayResultNotNil[16] = "pResult != NIL";
