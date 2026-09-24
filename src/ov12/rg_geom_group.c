/*
 * OV12 original TU 59: 0x00a303a8..0x00a31150 (28 functions)
 */
#include "common.h"
#include "shared.h"
#include "ov12/rg_game_collision.h"
#include "rg_geom_group.h"

static void _InitGroup(RgGeomGroup *pGroup)
{
    if (pGroup == 0) {
        assert_prog(D_00A55338, D_00A55348, 36);
    }
    pGroup->m_pList = CreateRgVector(128, D_00A55348, 37);
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_geom_group", _DisposeGroup);

RgGeomGroup *CreateRgGeomGroup(void)
{
    RgGeomGroup *pGroup;

    pGroup = RgHeapAlloc(InstanceOfRgHeap(), sizeof(RgGeomGroup), D_00A55348, 62);
    if (pGroup == 0) {
        assert_prog(D_00A55338, D_00A55348, 63);
    }
    _InitGroup(pGroup);
    return pGroup;
}

void DisposeRgGeomGroup(RgGeomGroup *pGroup)
{
    if (pGroup == 0) {
        assert_prog(D_00A55338, D_00A55348, 70);
    }
    _DisposeGroup(pGroup);
    RgHeapFree(InstanceOfRgHeap(), pGroup, D_00A55348, 72);
}

static void RgGeomGroupAddElm(RgGeomGroup *pGroup, void *pElm)
{
    if ((pGroup == 0) || (pElm == 0)) {
        assert_prog(D_00A55360, D_00A55348, 82);
    }
    if (RgVectorFind_sub(pGroup->m_pList, pElm, D_00A55348, 83) >= 0) {
        assert_prog(D_00A55380, D_00A55348, 83);
    }
    RgVectorPush(pGroup->m_pList, pElm);
}

static void RgGeomGroupRemoveElm(RgGeomGroup *group, void *element)
{
    if ((group == 0) || (element == 0)) {
        assert_prog(D_00A55360, D_00A55348, 91);
    }
    RgVectorRemove(group->m_pList, element, D_00A55348, 93);
}

/*
 * Every geometry kind a group creates stores the RgGeomGroup that owns it
 * inside its own RgGeom object, but at a different fixed offset per kind
 * (evidenced by each destructor below, which reads exactly one field at one
 * offset and passes it straight to RgGeomGroupRemoveElm). RgGeom's full
 * layout is not established in this TU -- src/ov12/rg_geom_point.c and
 * rg_geom_pillar.c already document other fields of a plain point object
 * (position, oldPosition, radius) inside the same 0x70..0x80 span the point
 * and ball kinds use here for their owner pointer, and the two pieces of
 * evidence are not reconciled -- so each owner is reached with one named,
 * documented offset access instead of a struct (docs/naming.md, "a narrowly
 * evidenced scalar access outside a partial type can be reviewed with an
 * explicit limitation").
 */
#define RG_GEOM_POINT_GROUP_OFFSET  0x70
#define RG_GEOM_BALL_GROUP_OFFSET   0x80
#define RG_GEOM_ROBOT_GROUP_OFFSET  0xA0
#define RG_GEOM_POLY_GROUP_OFFSET   0xA0
#define RG_GEOM_TRAY_GROUP_OFFSET   0xA0
#define RG_GEOM_PILLAR_GROUP_OFFSET 0xA0

static void _DestructPointMadeByGroup(RgGeom *pGeom)
{
    if (pGeom == 0) {
        assert_prog(D_00A553A8, D_00A55348, 138);
    }
    RgGeomGroupRemoveElm(
        *(RgGeomGroup **)((unsigned char *)pGeom + RG_GEOM_POINT_GROUP_OFFSET),
        pGeom);
}

static void _DestructBallMadeByGroup(RgGeom *pGeom)
{
    if (pGeom == 0) {
        assert_prog(D_00A553A8, D_00A55348, 143);
    }
    RgGeomGroupRemoveElm(
        *(RgGeomGroup **)((unsigned char *)pGeom + RG_GEOM_BALL_GROUP_OFFSET),
        pGeom);
}

static void _DestructRobotMadeByGroup(RgGeom *pGeom)
{
    if (pGeom == 0) {
        assert_prog(D_00A553A8, D_00A55348, 148);
    }
    RgGeomGroupRemoveElm(
        *(RgGeomGroup **)((unsigned char *)pGeom + RG_GEOM_ROBOT_GROUP_OFFSET),
        pGeom);
}

static void _DestructPolyMadeByGroup(RgGeom *pGeom)
{
    if (pGeom == 0) {
        assert_prog(D_00A553A8, D_00A55348, 153);
    }
    RgGeomGroupRemoveElm(
        *(RgGeomGroup **)((unsigned char *)pGeom + RG_GEOM_POLY_GROUP_OFFSET),
        pGeom);
}

static void _DestructTrayMadeByGroup(RgGeom *pGeom)
{
    if (pGeom == 0) {
        assert_prog(D_00A553A8, D_00A55348, 158);
    }
    RgGeomGroupRemoveElm(
        *(RgGeomGroup **)((unsigned char *)pGeom + RG_GEOM_TRAY_GROUP_OFFSET),
        pGeom);
}

static void _DestructPillarMadeByGroup(RgGeom *pGeom)
{
    if (pGeom == 0) {
        assert_prog(D_00A553A8, D_00A55348, 163);
    }
    RgGeomGroupRemoveElm(
        *(RgGeomGroup **)((unsigned char *)pGeom + RG_GEOM_PILLAR_GROUP_OFFSET),
        pGeom);
}

static RgGeom *_CreatePointWithGroup(RgGeomGroup *group)
{
    RgGeom *geom;

    geom = RgHeapAlloc(InstanceOfRgHeap(), 0x80, D_00A55348, 170);
    if (group == 0) {
        assert_prog(D_00A55338, D_00A55348, 171);
    }
    InitRgGeomPoint(geom, 1.0f);
    RgGeomSetDestructMethod(geom, _DestructPointMadeByGroup);
    *(RgGeomGroup **)((unsigned char *)geom + RG_GEOM_POINT_GROUP_OFFSET) = group;
    RgGeomGroupAddElm(group, geom);
    return geom;
}

static RgGeom *_CreateBallWithGroup(RgGeomGroup *group)
{
    RgGeom *geom;

    geom = RgHeapAlloc(InstanceOfRgHeap(), 0x90, D_00A55348, 184);
    if (group == 0) {
        assert_prog(D_00A55338, D_00A55348, 185);
    }
    InitRgGeomBall(geom, 1.0f, 1.0f);
    RgGeomSetDestructMethod(geom, _DestructBallMadeByGroup);
    *(RgGeomGroup **)((unsigned char *)geom + RG_GEOM_BALL_GROUP_OFFSET) = group;
    RgGeomGroupAddElm(group, geom);
    return geom;
}

static RgGeom *_CreateRobotWithGroup(RgGeomGroup *group)
{
    RgGeom *geom;

    geom = RgHeapAlloc(InstanceOfRgHeap(), 0xB0, D_00A55348, 199);
    if (group == 0) {
        assert_prog(D_00A55338, D_00A55348, 200);
    }
    InitRgGeomRobot(geom);
    RgGeomSetDestructMethod(geom, _DestructRobotMadeByGroup);
    *(RgGeomGroup **)((unsigned char *)geom + RG_GEOM_ROBOT_GROUP_OFFSET) = group;
    RgGeomGroupAddElm(group, geom);
    return geom;
}

static RgGeom *_CreatePolyWithGroup(RgGeomGroup *group)
{
    RgGeom *geom;

    geom = RgHeapAlloc(InstanceOfRgHeap(), 0xB0, D_00A55348, 214);
    if (group == 0) {
        assert_prog(D_00A55338, D_00A55348, 215);
    }
    InitRgGeomPoly(geom);
    RgGeomSetDestructMethod(geom, _DestructPolyMadeByGroup);
    *(RgGeomGroup **)((unsigned char *)geom + RG_GEOM_POLY_GROUP_OFFSET) = group;
    RgGeomGroupAddElm(group, geom);
    return geom;
}

static RgGeom *_CreateTrayWithGroup(RgGeomGroup *group)
{
    RgGeom *geom;

    geom = RgHeapAlloc(InstanceOfRgHeap(), 0xB0, D_00A55348, 229);
    if (group == 0) {
        assert_prog(D_00A55338, D_00A55348, 230);
    }
    InitRgGeomTray(geom);
    RgGeomSetDestructMethod(geom, _DestructTrayMadeByGroup);
    *(RgGeomGroup **)((unsigned char *)geom + RG_GEOM_TRAY_GROUP_OFFSET) = group;
    RgGeomGroupAddElm(group, geom);
    return geom;
}

static RgGeom *_CreatePillarWithGroup(RgGeomGroup *group)
{
    RgGeom *geom;

    geom = RgHeapAlloc(InstanceOfRgHeap(), 0xB0, D_00A55348, 244);
    if (group == 0) {
        assert_prog(D_00A55338, D_00A55348, 245);
    }
    InitRgGeomPillar(geom);
    RgGeomSetDestructMethod(geom, _DestructPillarMadeByGroup);
    *(RgGeomGroup **)((unsigned char *)geom + RG_GEOM_PILLAR_GROUP_OFFSET) = group;
    RgGeomGroupAddElm(group, geom);
    return geom;
}

RgGeom *RgGeomGroupCreatePoint(RgGeomGroup *pGroup, RgGeom *parent)
{
    RgGeom *geom;

    if (pGroup == 0) {
        assert_prog(D_00A55338, D_00A55348, 261);
    }
    geom = _CreatePointWithGroup(pGroup);
    RgGeomSetParent(geom, parent);
    return geom;
}

RgGeom *RgGeomGroupCreateBall(RgGeomGroup *pGroup, RgGeom *parent)
{
    RgGeom *geom;

    if (pGroup == 0) {
        assert_prog(D_00A55338, D_00A55348, 271);
    }
    geom = _CreateBallWithGroup(pGroup);
    RgGeomSetParent(geom, parent);
    return geom;
}

/*
 * Unlike its siblings this creator has no return statement and ends in a tail
 * jump to RgGeomSetParent. Its caller _InitRgPlayer still passes the result
 * register to RgRobotSetGeom: RgGeomSetParent leaves that register untouched
 * unless its assert fires, so the caller receives the geometry
 * _CreateRobotWithGroup returned. The value is used, but the function never
 * returns it explicitly.
 */
void RgGeomGroupCreateRobot(RgGeomGroup *pGroup, RgGeom *parent)
{
    if (pGroup == 0) {
        assert_prog(D_00A55338, D_00A55348, 281);
    }
    RgGeomSetParent(_CreateRobotWithGroup(pGroup), parent);
}

RgGeom *RgGeomGroupCreatePoly(RgGeomGroup *pGroup, RgGeom *parent,
                              RgColiData *coliData)
{
    RgGeom *geom;

    if (pGroup == 0) {
        assert_prog(D_00A55338, D_00A55348, 290);
    }
    geom = _CreatePolyWithGroup(pGroup);
    RgGeomSetParent(geom, parent);
    RgGeomPolySetColiData(geom, coliData);
    return geom;
}

RgGeom *RgGeomGroupCreateTray(RgGeomGroup *pGroup, RgGeom *parent)
{
    RgGeom *geom;

    if (pGroup == 0) {
        assert_prog(D_00A55338, D_00A55348, 300);
    }
    geom = _CreateTrayWithGroup(pGroup);
    RgGeomSetParent(geom, parent);
    return geom;
}

RgGeom *RgGeomGroupCreatePiller(RgGeomGroup *pGroup, RgGeom *parent)
{
    RgGeom *geom;

    if (pGroup == 0) {
        assert_prog(D_00A55338, D_00A55348, 309);
    }
    geom = _CreatePillarWithGroup(pGroup);
    RgGeomSetParent(geom, parent);
    return geom;
}

void RgGeomGroupDisposeElm(RgGeomGroup *pGroup, RgGeom *pElm)
{
    if (pGroup == 0) {
        assert_prog(D_00A55338, D_00A55348, 319);
    }
    RgGeomGroupRemoveElm(pGroup, pElm);
    RgGeomFree(pElm);
}

void InitRgGeomGroupColiArg(RgGameColiArg *pArg)
{
    pArg->unmodeled_14 = 0;
    /*
     * gcc 2.96 -O2 reorders an unbroken run of independent stores by moving
     * the last one to the front (unmodeled_14 above); grouping this middle
     * run in its own scope keeps it from being merged with the trailing
     * unmodeled_10 store and reordered again. A plain sequential rewrite of
     * this whole function (no scope) fails the whole-file compare (6 diff
     * ranges), confirming the scope is compiler-forced, not incidental.
     */
    do {
        pArg->m_pGroup1 = 0;
        pArg->unmodeled_04 = 0;
        pArg->unmodeled_08 = 0;
        pArg->m_pGroup2 = 0;
    } while (0);
    pArg->unmodeled_10 = 0;
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_geom_group", RgGeomGroupCollision);

void RgGeomGroupDebugDisp(RgGeomGroup *pGroup)
{
    if (pGroup == 0) {
        assert_prog(D_00A55338, D_00A55348, 396);
    }
}
