/*
 * OV12 original TU 62: 0x00a31970..0x00a32688 (14 functions)
 */
#include "common.h"
#include "shared.h"
#include "rg_colidata.h"

INCLUDE_ASM("asm/nonmatchings/ov12/rg_colidata", _InitTriColi);

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

INCLUDE_ASM("asm/nonmatchings/ov12/rg_colidata", _InitTriangles);

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
