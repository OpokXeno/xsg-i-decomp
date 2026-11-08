/*
 * OV12 original TU 60: 0x00a31150..0x00a31768 (12 functions)
 */
#include "common.h"
#include "shared.h"
#include "ov12/assert_prog.h"
#include "ov12/rg_game_collision.h"
#include "ov12/rg_geom_group.h"
#include "ov12/rg_heap.h"
#include "ov12/rg_vector.h"

/* Most group pairs queued for checking at once. */
#define RG_GAME_COLLISION_MAX_CHECKS 16

struct RgGameCollision {
    RgPtrVector *m_pCheckList;
};

const char gameCollisionSourcePath[32] = "../rg_game_collision.euc.c";
const char gameCollisionAssertSrcNotNil[16] = "pSrc != NIL";
const char gameCollisionAssertGroupsDiffer[40] = "pSrc->m_pGroup1 != pSrc->m_pGroup2";
const char gameCollisionAssertArgNotNil[16] = "pArg != NIL";
const char gameCollisionAssertGameColiNotNil[24] = "pGameColi != NIL";

/* Copies the group pair and callbacks of pSrc into a new heap-allocated check argument. */
static RgGameColiArg *_CreateColiArg(RgGameColiArg *pSrc)
{
    RgGameColiArg *pArg;

    pArg = RgHeapAlloc(InstanceOfRgHeap(), sizeof(RgGameColiArg), gameCollisionSourcePath, 31);
    if (pSrc == 0) {
        assert_prog(gameCollisionAssertSrcNotNil, gameCollisionSourcePath, 33);
    }
    if (pSrc->m_pGroup1 == pSrc->m_pGroup2) {
        assert_prog(gameCollisionAssertGroupsDiffer, gameCollisionSourcePath, 34);
    }
    InitRgGeomGroupColiArg(pArg);
    pArg->m_pGroup1 = pSrc->m_pGroup1;
    pArg->hitCallback1 = pSrc->hitCallback1;
    pArg->m_pGroup2 = pSrc->m_pGroup2;
    pArg->hitCallback2 = pSrc->hitCallback2;
    pArg->missCallback1 = pSrc->missCallback1;
    pArg->missCallback2 = pSrc->missCallback2;
    return pArg;
}

static void _DisposeColiArg(RgGameColiArg *pArg)
{
    if (pArg == 0) {
        assert_prog(gameCollisionAssertArgNotNil, gameCollisionSourcePath, 48);
    }
    RgHeapFree(InstanceOfRgHeap(), pArg, gameCollisionSourcePath, 49);
}

static void _InitGameCollision(RgGameCollision *pGameColi)
{
    if (pGameColi == 0) {
        assert_prog(gameCollisionAssertGameColiNotNil, gameCollisionSourcePath, 56);
    }
    pGameColi->m_pCheckList = CreateRgVector(RG_GAME_COLLISION_MAX_CHECKS, gameCollisionSourcePath, 57);
}

/* Disposes every queued check argument, then the check list itself. */
static void _DisposeGameCollision(RgGameCollision *pGameColi)
{
    unsigned int i;
    unsigned int size;
    RgGameColiArg *pArg;

    if (pGameColi == 0) {
        assert_prog(gameCollisionAssertGameColiNotNil, gameCollisionSourcePath, 64);
    }
    size = RgVectorSize(pGameColi->m_pCheckList);
    for (i = 0; i < size; i++) {
        pArg = RgVectorIndex(pGameColi->m_pCheckList, i, gameCollisionSourcePath, 69);
        _DisposeColiArg(pArg);
    }
    DisposeRgVector(pGameColi->m_pCheckList, gameCollisionSourcePath, 72);
    pGameColi->m_pCheckList = 0;
}

/* Queues a copy of pArg in the check list. */
static void _AddCheck(RgGameCollision *pGameColi, RgGameColiArg *pArg)
{
    RgGameColiArg *pNewArg;

    if (pGameColi == 0) {
        assert_prog(gameCollisionAssertGameColiNotNil, gameCollisionSourcePath, 83);
    }
    if (pArg == 0) {
        assert_prog(gameCollisionAssertArgNotNil, gameCollisionSourcePath, 84);
    }
    pNewArg = _CreateColiArg(pArg);
    RgVectorPush(pGameColi->m_pCheckList, pNewArg);
}

/* Disposes the queued check arguments and empties the check list. */
static void _ClearCheck(RgGameCollision *pGameColi)
{
    RgPtrVector *pCheckList;
    unsigned int i;
    unsigned int size;
    RgGameColiArg *pArg;

    if (pGameColi == 0) {
        assert_prog(gameCollisionAssertGameColiNotNil, gameCollisionSourcePath, 96);
    }
    pCheckList = pGameColi->m_pCheckList;
    size = RgVectorSize(pCheckList);
    for (i = 0; i < size; i++) {
        pArg = RgVectorIndex(pCheckList, i, gameCollisionSourcePath, 100);
        _DisposeColiArg(pArg);
    }
    RgVectorClear(pCheckList);
}

/* Runs the group collision check for every queued argument. */
static void _JobCheck(RgGameCollision *pGameColi)
{
    unsigned int i;
    unsigned int size;
    RgGameColiArg *pArg;

    if (pGameColi == 0) {
        assert_prog(gameCollisionAssertGameColiNotNil, gameCollisionSourcePath, 110);
    }
    size = RgVectorSize(pGameColi->m_pCheckList);
    for (i = 0; i < size; i++) {
        pArg = RgVectorIndex(pGameColi->m_pCheckList, i, gameCollisionSourcePath, 113);
        RgGeomGroupCollision(pArg);
    }
}

/* Allocates a collision checker with an empty check list. */
RgGameCollision *CreateRgGameCollision(void)
{
    RgGameCollision *pGameColi;

    pGameColi = RgHeapAlloc(InstanceOfRgHeap(), sizeof(RgGameCollision), gameCollisionSourcePath, 123);
    if (pGameColi == 0) {
        assert_prog(gameCollisionAssertGameColiNotNil, gameCollisionSourcePath, 124);
    }
    _InitGameCollision(pGameColi);
    return pGameColi;
}

void DisposeRgGameCollision(RgGameCollision *pGameColi)
{
    if (pGameColi == 0) {
        assert_prog(gameCollisionAssertGameColiNotNil, gameCollisionSourcePath, 131);
    }
    _DisposeGameCollision(pGameColi);
    RgHeapFree(InstanceOfRgHeap(), pGameColi, gameCollisionSourcePath, 133);
}

/* Queues a check for pArg when both of its groups are set. */
void RgGameCollisionAdd(RgGameCollision *pGameColi, RgGameColiArg *pArg)
{
    if (pGameColi == 0) {
        assert_prog(gameCollisionAssertGameColiNotNil, gameCollisionSourcePath, 140);
    }
    if (pArg == 0) {
        assert_prog(gameCollisionAssertArgNotNil, gameCollisionSourcePath, 141);
    }
    if ((pArg->m_pGroup1 != 0) && (pArg->m_pGroup2 != 0)) {
        _AddCheck(pGameColi, pArg);
    }
}

void RgGameCollisionClear(RgGameCollision *pGameColi)
{
    if (pGameColi == 0) {
        assert_prog(gameCollisionAssertGameColiNotNil, gameCollisionSourcePath, 150);
    }
    _ClearCheck(pGameColi);
}

void RgGameCollisionJob(RgGameCollision *pGameColi)
{
    if (pGameColi == 0) {
        assert_prog(gameCollisionAssertGameColiNotNil, gameCollisionSourcePath, 158);
    }
    _JobCheck(pGameColi);
}
