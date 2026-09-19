/*
 * OV12 original TU 60: 0x00a31150..0x00a31768 (12 functions)
 */
#include "common.h"
#include "shared.h"
#include "rg_game_collision.h"

static RgGameColiArg *_CreateColiArg(RgGameColiArg *pSrc)
{
    RgGameColiArg *pArg;

    pArg = RgHeapAlloc(InstanceOfRgHeap(), sizeof(RgGameColiArg), D_00A553C8, 31);
    if (pSrc == 0) {
        assert_prog(D_00A553E8, D_00A553C8, 33);
    }
    if (pSrc->m_pGroup1 == pSrc->m_pGroup2) {
        assert_prog(D_00A553F8, D_00A553C8, 34);
    }
    InitRgGeomGroupColiArg(pArg);
    pArg->m_pGroup1 = pSrc->m_pGroup1;
    pArg->unmodeled_04 = pSrc->unmodeled_04;
    pArg->m_pGroup2 = pSrc->m_pGroup2;
    pArg->unmodeled_10 = pSrc->unmodeled_10;
    pArg->unmodeled_08 = pSrc->unmodeled_08;
    pArg->unmodeled_14 = pSrc->unmodeled_14;
    return pArg;
}

static void _DisposeColiArg(RgGameColiArg *pArg)
{
    if (pArg == 0) {
        assert_prog(D_00A55420, D_00A553C8, 48);
    }
    RgHeapFree(InstanceOfRgHeap(), pArg, D_00A553C8, 49);
}

static void _InitGameCollision(RgGameCollision *pGameColi)
{
    if (pGameColi == 0) {
        assert_prog(D_00A55430, D_00A553C8, 56);
    }
    pGameColi->m_pCheckList = CreateRgVector(16, D_00A553C8, 57);
}

static void _DisposeGameCollision(RgGameCollision *pGameColi)
{
    unsigned int i;
    unsigned int size;
    RgGameColiArg *pArg;

    if (pGameColi == 0) {
        assert_prog(D_00A55430, D_00A553C8, 64);
    }
    size = RgVectorSize(pGameColi->m_pCheckList);
    for (i = 0; i < size; i++) {
        pArg = RgVectorIndex(pGameColi->m_pCheckList, i, D_00A553C8, 69);
        _DisposeColiArg(pArg);
    }
    DisposeRgVector(pGameColi->m_pCheckList, D_00A553C8, 72);
    pGameColi->m_pCheckList = 0;
}

static void _AddCheck(RgGameCollision *pGameColi, RgGameColiArg *pArg)
{
    RgGameColiArg *pNewArg;

    if (pGameColi == 0) {
        assert_prog(D_00A55430, D_00A553C8, 83);
    }
    if (pArg == 0) {
        assert_prog(D_00A55420, D_00A553C8, 84);
    }
    pNewArg = _CreateColiArg(pArg);
    RgVectorPush(pGameColi->m_pCheckList, pNewArg);
}

static void _ClearCheck(RgGameCollision *pGameColi)
{
    void *pCheckList;
    unsigned int i;
    unsigned int size;
    RgGameColiArg *pArg;

    if (pGameColi == 0) {
        assert_prog(D_00A55430, D_00A553C8, 96);
    }
    pCheckList = pGameColi->m_pCheckList;
    size = RgVectorSize(pCheckList);
    for (i = 0; i < size; i++) {
        pArg = RgVectorIndex(pCheckList, i, D_00A553C8, 100);
        _DisposeColiArg(pArg);
    }
    RgVectorClear(pCheckList);
}

static void _JobCheck(RgGameCollision *pGameColi)
{
    unsigned int i;
    unsigned int size;
    RgGeomGroup *pGroup;

    if (pGameColi == 0) {
        assert_prog(D_00A55430, D_00A553C8, 110);
    }
    size = RgVectorSize(pGameColi->m_pCheckList);
    for (i = 0; i < size; i++) {
        pGroup = RgVectorIndex(pGameColi->m_pCheckList, i, D_00A553C8, 113);
        RgGeomGroupCollision(pGroup);
    }
}

RgGameCollision *CreateRgGameCollision(void)
{
    RgGameCollision *pGameColi;

    pGameColi = RgHeapAlloc(InstanceOfRgHeap(), sizeof(RgGameCollision), D_00A553C8, 123);
    if (pGameColi == 0) {
        assert_prog(D_00A55430, D_00A553C8, 124);
    }
    _InitGameCollision(pGameColi);
    return pGameColi;
}

void DisposeRgGameCollision(RgGameCollision *pGameColi)
{
    if (pGameColi == 0) {
        assert_prog(D_00A55430, D_00A553C8, 131);
    }
    _DisposeGameCollision(pGameColi);
    RgHeapFree(InstanceOfRgHeap(), pGameColi, D_00A553C8, 133);
}

void RgGameCollisionAdd(RgGameCollision *pGameColi, RgGameColiArg *pArg)
{
    if (pGameColi == 0) {
        assert_prog(D_00A55430, D_00A553C8, 140);
    }
    if (pArg == 0) {
        assert_prog(D_00A55420, D_00A553C8, 141);
    }
    if ((pArg->m_pGroup1 != 0) && (pArg->m_pGroup2 != 0)) {
        _AddCheck(pGameColi, pArg);
    }
}

void RgGameCollisionClear(RgGameCollision *pGameColi)
{
    if (pGameColi == 0) {
        assert_prog(D_00A55430, D_00A553C8, 150);
    }
    _ClearCheck(pGameColi);
}

void RgGameCollisionJob(RgGameCollision *pGameColi)
{
    if (pGameColi == 0) {
        assert_prog(D_00A55430, D_00A553C8, 158);
    }
    _JobCheck(pGameColi);
}
