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

INCLUDE_ASM("asm/nonmatchings/ov12/rg_colidata", _CheckIntersectBall);

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

INCLUDE_ASM("asm/nonmatchings/ov12/rg_colidata", CreateRgColiDataQuadPrism);

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

INCLUDE_ASM("asm/nonmatchings/ov12/rg_colidata", RgColiDataVsRay);
