/*
 * OV12 original TU 62: 0x00a31970..0x00a32688 (14 functions)
 */
#include "common.h"
#include "shared.h"
#include "rg_colidata.h"

struct RgColiTriangle {
    RgVector m_vertices[3];
    RgVector m_plane;
    RgMatrix m_planeMatrix;
};

extern void XrgUnitMatrix(RgMatrix destination);
extern void XrgCopyVectorXYZ(RgVector destination, RgVector source);
extern void XrgCalcPlane(RgVector plane, RgVector p0, RgVector p1,
                         RgVector p2);
extern void XrgSubVector(RgVector destination, RgVector first,
                         RgVector second);
extern void XrgOuterVector(RgVector destination, RgVector first,
                           RgVector second);
extern float XrgNormalizeVector(RgVector destination, RgVector source);
extern float XrgInnerVector(RgVector first, RgVector second);

const char D_00A554F0[] = "pData != NIL";

const char D_00A55500[] = "../rg_colidata.euc.c";

const char D_00A55518[] = "nCapa > 0";

const char D_00A55528[] = "pData->m_pTriangles != NIL";

const char D_00A55548[] = "nNumOfTriangles > 0";

const char D_00A55560[] = "pData->m_nCapaOfTri > pData->m_nNumOfTri";

const char D_00A55670[] = "pPoly != NIL && pArg != NIL && pResult != NIL";

static void _InitTriColi(void *pTri, void *pV0, void *pV1, void *pV2)
{
    RgVector edge;
    RgVector cross;
    RgVector facePlane;
    RgVector sidePlanes[3];
    int component;
    int plane;
    RgColiTriangle *triangle;

    triangle = pTri;
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
        assert_prog(D_00A554F0, D_00A55500, 0xAC);
    }
    if (nCapa <= 0) {
        assert_prog(D_00A55518, D_00A55500, 0xAD);
    }
    pData->m_pTriangles = RgHeapAlloc(InstanceOfRgHeap(), nCapa << 7,
                                      D_00A55500, 0xAE);
    if (pData->m_pTriangles == 0) {
        assert_prog(D_00A55528, D_00A55500, 0xAF);
    }
    pData->m_nCapacity = nCapa;
    pData->m_nTriangles = 0;
}

extern void XrgAddVector(RgVector destination, RgVector first,
                         RgVector second);

static int _CheckIntersectBall(RgColiData *pPoly, void *pArg, RgColiBallResult *pHit)
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
        radius = ((RgColiBall *)pArg)->m_radius;
        __asm__ __volatile__(
            "lqc2 $vf31, 0(%1)\n\t"
            "sqc2 $vf31, 0(%0)\n\t"
            : : "r"(&ball), "r"(pArg) : "memory");
        ball.w = 1.0f;
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
            status = -2;
            if (planeDistance.x >= 0.0f && planeDistance.y >= 0.0f && planeDistance.z >= 0.0f) {
                delta[0] = pMatrix[3] * (radius - planeDistance.w);
                delta[1] = pMatrix[7] * (radius - planeDistance.w);
                delta[2] = pMatrix[11] * (radius - planeDistance.w);
                delta[3] = 0.0f;
                status = 1;
            }
        } else {
            status = -1;
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
        assert_prog(D_00A554F0, D_00A55500, 0xEB);
    }
    pData->m_pTriangles = 0;
    pData->m_nTriangles = 0;
    pData->m_nCapacity = 0;
}

static void _DisposeColiData(RgColiData *pData)
{
    if (pData == 0) {
        assert_prog(D_00A554F0, D_00A55500, 243);
    }
    if (pData->m_pTriangles != 0) {
        RgHeapFree(InstanceOfRgHeap(), pData->m_pTriangles, D_00A55500, 245);
    }
    _InitColiData(pData);
}

static void _InitTriangles(RgColiData *pData, int nCapa)
{
    if (pData == 0) {
        assert_prog(D_00A554F0, D_00A55500, 252);
    }
    if (nCapa <= 0) {
        assert_prog(D_00A55548, D_00A55500, 253);
    }

    _DisposeColiData(pData);
    _AllocDataMemory(pData, nCapa);
}

static void _InitTriColi(void *pTri, void *pV0, void *pV1, void *pV2);

static void _AddTriangle(RgColiData *pData, void *pV0, void *pV1, void *pV2)
{
    /* separate load keeps the new triangle's index in its own register */
    int nIndex;
    int nTriangles;

    nIndex = pData->m_nTriangles;
    nTriangles = nIndex;

    if (pData->m_pTriangles == 0) {
        assert_prog(D_00A55528, D_00A55500, 264);
        nTriangles = pData->m_nTriangles;
    }

    if (nTriangles >= pData->m_nCapacity) {
        assert_prog(D_00A55560, D_00A55500, 265);
    }

    _InitTriColi((char *) pData->m_pTriangles + (nIndex << 7), pV0, pV1, pV2);
    pData->m_nTriangles++;
}

RgColiData *CreateRgColiData(void)
{
    RgColiData *pData;

    pData = RgHeapAlloc(InstanceOfRgHeap(), sizeof(RgColiData), D_00A55500, 275);
    _InitColiData(pData);
    return pData;
}

RgColiData *CreateRgColiDataQuadPrism(float width, float height)
{
    RgColiData *pData;
    RgColiQuadVertices vertices;
    RgColiQuadIndices indices;
    int *pIndex;
    float *pCoord;
    int i;

    pData = CreateRgColiData();
    vertices = D_00A55590;
    pIndex = indices.m_quads[0];
    indices = D_00A55610;
    RgColiDataInitTriangles(pData, 12);
    pCoord = &vertices.m_vertices[0].z;
    for (i = 7; i >= 0; i--, pCoord += 4) {
        pCoord[-2] *= width;
        pCoord[-1] *= height;
        pCoord[0] *= width;
    }
    pIndex += 1;
    for (i = 5; i >= 0; i--, pIndex += 4) {
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
        assert_prog(D_00A554F0, D_00A55500, 0x145);
    }
    _DisposeColiData(pData);
    RgHeapFree(InstanceOfRgHeap(), pData, D_00A55500, 0x147);
}

void RgColiDataInitTriangles(RgColiData *pData, int nCapa)
{
    if (pData == 0) {
        assert_prog(D_00A554F0, D_00A55500, 0x150);
    }
    _InitTriangles(pData, nCapa);
}

void RgColiDataAddTriangle(RgColiData *pData, void *pV0, void *pV1, void *pV2)
{
    if (pData == 0) {
        assert_prog(D_00A554F0, D_00A55500, 0x156);
    }
    _AddTriangle(pData, pV0, pV1, pV2);
}

void RgColiDataVsBall(RgColiData *pPoly, void *pArg, void *pResult)
{
    if (pPoly == 0 || pArg == 0 || pResult == 0) {
        assert_prog(D_00A55670, D_00A55500, 0x18D);
    }
    _CheckIntersectBall(pPoly, pArg, pResult);
}

/* The segment RgColiDataVsRay tests: its start point, then the vector to its
 * end point (the hit fraction runs over 0..1 along this vector). */
typedef struct RgColiSegment {
    RgVector m_start;
    RgVector m_vector;
} RgColiSegment;

/* The hit record RgColiDataVsRay writes: the plane of the first triangle the
 * segment crosses, then the fraction along the segment where it crosses. */
typedef struct RgColiSegmentHit {
    RgVector m_plane;
    float m_fraction;
} RgColiSegmentHit;

/* 0x00a556a0 "pArg != NIL", 0x00a556b0 "pResult != NIL" */
extern const char D_00A556A0[];
extern const char D_00A556B0[];


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
        assert_prog(D_00A554F0, D_00A55500, 411);
    }
    if (pSegment == 0) {
        assert_prog(D_00A556A0, D_00A55500, 412);
    }
    if (pResult == 0) {
        assert_prog(D_00A556B0, D_00A55500, 413);
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
        if (offset.x * cross.x + offset.y * cross.y + offset.z * cross.z <= -1e-4f) {
            status = -2;
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
            if (offset.x * cross.x + offset.y * cross.y + offset.z * cross.z <= -1e-4f) {
                status = -2;
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
                if (offset.x * cross.x + offset.y * cross.y + offset.z * cross.z <= -1e-4f) {
                    status = -2;
                } else {
                    denom = pPlane[0] * pVector[0]
                          + pPlane[1] * pVector[1]
                          + pPlane[2] * pVector[2];
                    if (denom >= -1e-4f) {
                        status = -4;
                    } else {
                        t = -(pPlane[0] * pSegment->m_start[0]
                              + pPlane[1] * pSegment->m_start[1]
                              + pPlane[2] * pSegment->m_start[2]
                              + pPlane[3]) / denom;
                        if (0.0f <= t && t <= 1.0f) {
                            fraction = t;
                            status = 1;
                        } else {
                            status = -3;
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
