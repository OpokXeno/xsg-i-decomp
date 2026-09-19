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

INCLUDE_ASM("asm/nonmatchings/ov12/rg_colidata", _DisposeColiData);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_colidata", _InitTriangles);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_colidata", _AddTriangle);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_colidata", CreateRgColiData);

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
