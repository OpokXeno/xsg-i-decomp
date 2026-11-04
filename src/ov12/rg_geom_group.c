/*
 * OV12 original TU 59: 0x00a303a8..0x00a31150 (28 functions)
 */
#include "common.h"
#include "shared.h"
#include "ov12/assert_prog.h"
#include "ov12/rg_game_collision.h"
#include "ov12/rg_geom.h"
#include "ov12/rg_geom_ball.h"
#include "ov12/rg_geom_collision.h"
#include "ov12/rg_geom_group.h"
#include "ov12/rg_geom_point.h"
#include "ov12/rg_geom_poly.h"
#include "ov12/rg_geom_pillar.h"
#include "ov12/rg_geom_robot.h"
#include "ov12/rg_geom_tray.h"
#include "ov12/rg_heap.h"
#include "ov12/rg_vector.h"

/*
 * The geometries a group creates are allocated larger than their plain kind:
 * the RgGeomGroup that owns the object is appended, so the destructor can
 * remove it from its group's list.
 */
typedef struct RgGeomGroupPoint {
    RgGeomPoint point;
    RgGeomGroup *group;
    unsigned char unmodeled_74[0x0c];
} RgGeomGroupPoint;

typedef struct RgGeomGroupBall {
    RgGeomBall ball;
    RgGeomGroup *group;
    unsigned char unmodeled_84[0x0c];
} RgGeomGroupBall;

typedef struct RgGeomGroupRobot {
    RgGeomRobot robot;
    RgGeomGroup *group;
    unsigned char unmodeled_a4[0x0c];
} RgGeomGroupRobot;

typedef struct RgGeomGroupPoly {
    RgGeomPoly poly;
    RgGeomGroup *group;
    unsigned char unmodeled_a4[0x0c];
} RgGeomGroupPoly;

/* A pillar is a tray tagged as a pillar. */
typedef struct RgGeomGroupTray {
    RgGeomTray tray;
    RgGeomGroup *group;
    unsigned char unmodeled_a4[0x0c];
} RgGeomGroupTray;

/* Most geometries one group can hold. */
#define RG_GEOM_GROUP_CAPACITY 128

static void RgGeomGroupRemoveElm(RgGeomGroup *group, void *element);

const char geomGroupAssertGroupNotNil[] = "pGroup != NIL";

const char geomGroupSourcePath[] = "../rg_geom_group.euc.c";

const char geomGroupAssertGroupAndElmNotNil[] = "pGroup != NIL && pElm != NIL";

const char geomGroupAssertElmNotYetInList[] = "RgVectorFind(pGroup->m_pList,pElm) < 0";

const char geomGroupAssertGeomNotNil[] = "pGeom != NIL";

static void _InitGroup(RgGeomGroup *pGroup)
{
    if (pGroup == 0) {
        assert_prog(geomGroupAssertGroupNotNil, geomGroupSourcePath, 36);
    }
    pGroup->m_pList = CreateRgVector(RG_GEOM_GROUP_CAPACITY, geomGroupSourcePath, 37);
}

static void _DisposeGroup(RgGeomGroup *group)
{
    unsigned int index;
    unsigned int size;
    RgGeom *geom;

    if (group == 0) {
        assert_prog(geomGroupAssertGroupNotNil, geomGroupSourcePath, 43);
    }
    index = 0;
    size = RgVectorSize(group->m_pList);
    if (size != 0) {
        do {
            geom = RgVectorIndex(group->m_pList, index, geomGroupSourcePath, 50);
            index += 1;
            RgGeomSetDestructMethod(geom, 0);
        } while (index < size);
    }
    DisposeRgVector(group->m_pList, geomGroupSourcePath, 52);
    group->m_pList = 0;
}

RgGeomGroup *CreateRgGeomGroup(void)
{
    RgGeomGroup *pGroup;

    pGroup = RgHeapAlloc(InstanceOfRgHeap(), sizeof(RgGeomGroup), geomGroupSourcePath, 62);
    if (pGroup == 0) {
        assert_prog(geomGroupAssertGroupNotNil, geomGroupSourcePath, 63);
    }
    _InitGroup(pGroup);
    return pGroup;
}

void DisposeRgGeomGroup(RgGeomGroup *pGroup)
{
    if (pGroup == 0) {
        assert_prog(geomGroupAssertGroupNotNil, geomGroupSourcePath, 70);
    }
    _DisposeGroup(pGroup);
    RgHeapFree(InstanceOfRgHeap(), pGroup, geomGroupSourcePath, 72);
}

static void RgGeomGroupAddElm(RgGeomGroup *pGroup, void *pElm)
{
    if ((pGroup == 0) || (pElm == 0)) {
        assert_prog(geomGroupAssertGroupAndElmNotNil, geomGroupSourcePath, 82);
    }
    if (RgVectorFind_sub(pGroup->m_pList, pElm, geomGroupSourcePath, 83) >= 0) {
        assert_prog(geomGroupAssertElmNotYetInList, geomGroupSourcePath, 83);
    }
    RgVectorPush(pGroup->m_pList, pElm);
}

static void RgGeomGroupRemoveElm(RgGeomGroup *group, void *element)
{
    if ((group == 0) || (element == 0)) {
        assert_prog(geomGroupAssertGroupAndElmNotNil, geomGroupSourcePath, 91);
    }
    RgVectorRemove(group->m_pList, element, geomGroupSourcePath, 93);
}

static void _DestructPointMadeByGroup(void *pGeom)
{
    RgGeomGroupPoint *geom = pGeom;

    if (pGeom == 0) {
        assert_prog(geomGroupAssertGeomNotNil, geomGroupSourcePath, 138);
    }
    RgGeomGroupRemoveElm(geom->group, pGeom);
}

static void _DestructBallMadeByGroup(void *pGeom)
{
    RgGeomGroupBall *geom = pGeom;

    if (pGeom == 0) {
        assert_prog(geomGroupAssertGeomNotNil, geomGroupSourcePath, 143);
    }
    RgGeomGroupRemoveElm(geom->group, pGeom);
}

static void _DestructRobotMadeByGroup(void *pGeom)
{
    RgGeomGroupRobot *geom = pGeom;

    if (pGeom == 0) {
        assert_prog(geomGroupAssertGeomNotNil, geomGroupSourcePath, 148);
    }
    RgGeomGroupRemoveElm(geom->group, pGeom);
}

static void _DestructPolyMadeByGroup(void *pGeom)
{
    RgGeomGroupPoly *geom = pGeom;

    if (pGeom == 0) {
        assert_prog(geomGroupAssertGeomNotNil, geomGroupSourcePath, 153);
    }
    RgGeomGroupRemoveElm(geom->group, pGeom);
}

static void _DestructTrayMadeByGroup(void *pGeom)
{
    RgGeomGroupTray *geom = pGeom;

    if (pGeom == 0) {
        assert_prog(geomGroupAssertGeomNotNil, geomGroupSourcePath, 158);
    }
    RgGeomGroupRemoveElm(geom->group, pGeom);
}

static void _DestructPillarMadeByGroup(void *pGeom)
{
    RgGeomGroupTray *geom = pGeom;

    if (pGeom == 0) {
        assert_prog(geomGroupAssertGeomNotNil, geomGroupSourcePath, 163);
    }
    RgGeomGroupRemoveElm(geom->group, pGeom);
}

static RgGeom *_CreatePointWithGroup(RgGeomGroup *group)
{
    RgGeomGroupPoint *geom;

    geom = RgHeapAlloc(InstanceOfRgHeap(), sizeof(RgGeomGroupPoint), geomGroupSourcePath, 170);
    if (group == 0) {
        assert_prog(geomGroupAssertGroupNotNil, geomGroupSourcePath, 171);
    }
    InitRgGeomPoint(&geom->point, 1.0f);
    RgGeomSetDestructMethod(&geom->point.geom, _DestructPointMadeByGroup);
    geom->group = group;
    RgGeomGroupAddElm(group, geom);
    return &geom->point.geom;
}

static RgGeomBall *_CreateBallWithGroup(RgGeomGroup *group)
{
    RgGeomGroupBall *geom;

    geom = RgHeapAlloc(InstanceOfRgHeap(), sizeof(RgGeomGroupBall), geomGroupSourcePath, 184);
    if (group == 0) {
        assert_prog(geomGroupAssertGroupNotNil, geomGroupSourcePath, 185);
    }
    InitRgGeomBall(&geom->ball, 1.0f, 1.0f);
    RgGeomSetDestructMethod(&geom->ball.point.geom, _DestructBallMadeByGroup);
    geom->group = group;
    RgGeomGroupAddElm(group, geom);
    return &geom->ball;
}

static RgGeom *_CreateRobotWithGroup(RgGeomGroup *group)
{
    RgGeomGroupRobot *geom;

    geom = RgHeapAlloc(InstanceOfRgHeap(), sizeof(RgGeomGroupRobot), geomGroupSourcePath, 199);
    if (group == 0) {
        assert_prog(geomGroupAssertGroupNotNil, geomGroupSourcePath, 200);
    }
    InitRgGeomRobot(&geom->robot.ball.point.geom);
    RgGeomSetDestructMethod(&geom->robot.ball.point.geom, _DestructRobotMadeByGroup);
    geom->group = group;
    RgGeomGroupAddElm(group, geom);
    return &geom->robot.ball.point.geom;
}

static RgGeom *_CreatePolyWithGroup(RgGeomGroup *group)
{
    RgGeomGroupPoly *geom;

    geom = RgHeapAlloc(InstanceOfRgHeap(), sizeof(RgGeomGroupPoly), geomGroupSourcePath, 214);
    if (group == 0) {
        assert_prog(geomGroupAssertGroupNotNil, geomGroupSourcePath, 215);
    }
    InitRgGeomPoly(&geom->poly.geom);
    RgGeomSetDestructMethod(&geom->poly.geom, _DestructPolyMadeByGroup);
    geom->group = group;
    RgGeomGroupAddElm(group, geom);
    return &geom->poly.geom;
}

static RgGeom *_CreateTrayWithGroup(RgGeomGroup *group)
{
    RgGeomGroupTray *geom;

    geom = RgHeapAlloc(InstanceOfRgHeap(), sizeof(RgGeomGroupTray), geomGroupSourcePath, 229);
    if (group == 0) {
        assert_prog(geomGroupAssertGroupNotNil, geomGroupSourcePath, 230);
    }
    InitRgGeomTray(&geom->tray.geom);
    RgGeomSetDestructMethod(&geom->tray.geom, _DestructTrayMadeByGroup);
    geom->group = group;
    RgGeomGroupAddElm(group, geom);
    return &geom->tray.geom;
}

static RgGeom *_CreatePillarWithGroup(RgGeomGroup *group)
{
    RgGeomGroupTray *geom;

    geom = RgHeapAlloc(InstanceOfRgHeap(), sizeof(RgGeomGroupTray), geomGroupSourcePath, 244);
    if (group == 0) {
        assert_prog(geomGroupAssertGroupNotNil, geomGroupSourcePath, 245);
    }
    InitRgGeomPillar(&geom->tray.geom);
    RgGeomSetDestructMethod(&geom->tray.geom, _DestructPillarMadeByGroup);
    geom->group = group;
    RgGeomGroupAddElm(group, geom);
    return &geom->tray.geom;
}

RgGeom *RgGeomGroupCreatePoint(RgGeomGroup *pGroup, void *parent)
{
    RgGeom *geom;

    if (pGroup == 0) {
        assert_prog(geomGroupAssertGroupNotNil, geomGroupSourcePath, 261);
    }
    geom = _CreatePointWithGroup(pGroup);
    RgGeomSetParent(geom, parent);
    return geom;
}

RgGeomBall *RgGeomGroupCreateBall(RgGeomGroup *pGroup, void *parent)
{
    RgGeomBall *ball;

    if (pGroup == 0) {
        assert_prog(geomGroupAssertGroupNotNil, geomGroupSourcePath, 271);
    }
    ball = _CreateBallWithGroup(pGroup);
    RgGeomSetParent(&ball->point.geom, parent);
    return ball;
}

/*
 * Unlike its siblings this creator has no return statement and ends in a tail
 * jump to RgGeomSetParent. Its caller _InitRgPlayer still passes the result
 * register to RgRobotSetGeom: RgGeomSetParent leaves that register untouched
 * unless its assert fires, so the caller receives the geometry
 * _CreateRobotWithGroup returned. The value is used, but the function never
 * returns it explicitly.
 */
void RgGeomGroupCreateRobot(RgGeomGroup *pGroup, void *parent)
{
    if (pGroup == 0) {
        assert_prog(geomGroupAssertGroupNotNil, geomGroupSourcePath, 281);
    }
    RgGeomSetParent(_CreateRobotWithGroup(pGroup), parent);
}

RgGeom *RgGeomGroupCreatePoly(RgGeomGroup *pGroup, void *parent,
                              RgColiData *coliData)
{
    RgGeom *geom;

    if (pGroup == 0) {
        assert_prog(geomGroupAssertGroupNotNil, geomGroupSourcePath, 290);
    }
    geom = _CreatePolyWithGroup(pGroup);
    RgGeomSetParent(geom, parent);
    RgGeomPolySetColiData(geom, coliData);
    return geom;
}

RgGeom *RgGeomGroupCreateTray(RgGeomGroup *pGroup, void *parent)
{
    RgGeom *geom;

    if (pGroup == 0) {
        assert_prog(geomGroupAssertGroupNotNil, geomGroupSourcePath, 300);
    }
    geom = _CreateTrayWithGroup(pGroup);
    RgGeomSetParent(geom, parent);
    return geom;
}

RgGeom *RgGeomGroupCreatePiller(RgGeomGroup *pGroup, void *parent)
{
    RgGeom *geom;

    if (pGroup == 0) {
        assert_prog(geomGroupAssertGroupNotNil, geomGroupSourcePath, 309);
    }
    geom = _CreatePillarWithGroup(pGroup);
    RgGeomSetParent(geom, parent);
    return geom;
}

void RgGeomGroupDisposeElm(RgGeomGroup *pGroup, RgGeom *pElm)
{
    if (pGroup == 0) {
        assert_prog(geomGroupAssertGroupNotNil, geomGroupSourcePath, 319);
    }
    RgGeomGroupRemoveElm(pGroup, pElm);
    RgGeomFree(pElm);
}

void InitRgGeomGroupColiArg(RgGameColiArg *pArg)
{
    pArg->missCallback2 = 0;
    /*
     * gcc 2.96 -O2 reorders an unbroken run of independent stores by moving
     * the last one to the front (missCallback2 above); grouping this middle
     * run in its own scope keeps it from being merged with the trailing
     * hitCallback2 store and reordered again. A plain sequential rewrite of
     * this whole function (no scope) fails the whole-file compare (6 diff
     * ranges), confirming the scope is compiler-forced, not incidental.
     */
    do {
        pArg->m_pGroup1 = 0;
        pArg->hitCallback1 = 0;
        pArg->missCallback1 = 0;
        pArg->m_pGroup2 = 0;
    } while (0);
    pArg->hitCallback2 = 0;
}

const char geomGroupAssertArgNotNil[] = "pArg != NIL";

void RgGeomGroupCollision(RgGameColiArg *pArg)
{
    RgGeomCollisionPair pair;
    RgPtrVector *list1;
    RgPtrVector *list2;
    unsigned int size1;
    unsigned int size2;
    unsigned int i;
    unsigned int j;
    int hits;

    hits = 0;
    if (pArg == 0) {
        assert_prog(geomGroupAssertArgNotNil, geomGroupSourcePath, 348);
    }
    if (pArg->m_pGroup1 != 0 && pArg->m_pGroup2 != 0) {
        list1 = pArg->m_pGroup1->m_pList;
        list2 = pArg->m_pGroup2->m_pList;
        size1 = RgVectorSize(list1);
        size2 = RgVectorSize(list2);
        InitRgGeomColiArg(&pair);
        pair.callback1 = (RgGeomColiCallback)pArg->hitCallback1;
        pair.callback2 = (RgGeomColiCallback)pArg->hitCallback2;
        for (i = 0; i < size2; i++) {
            for (j = 0; j < size1; j++) {
                pair.pGeom1 = RgVectorIndex(list1, j, geomGroupSourcePath, 367);
                pair.pGeom2 = RgVectorIndex(list2, i, geomGroupSourcePath, 368);
                if (RgGeomCollisionJob(&pair) != 0) {
                    hits++;
                }
            }
        }
        if (hits <= 0) {
            if (pArg->missCallback1 != 0) {
                for (i = 0; i < size1; i++) {
                    pArg->missCallback1(
                        RgVectorIndex(list1, i, geomGroupSourcePath, 378), 0);
                }
            }
            if (pArg->missCallback2 != 0) {
                for (i = 0; i < size2; i++) {
                    pArg->missCallback2(
                        RgVectorIndex(list2, i, geomGroupSourcePath, 382), 0);
                }
            }
        }
    }
}

void RgGeomGroupDebugDisp(RgGeomGroup *pGroup)
{
    if (pGroup == 0) {
        assert_prog(geomGroupAssertGroupNotNil, geomGroupSourcePath, 396);
    }
}
