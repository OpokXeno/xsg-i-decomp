/*
 * OV12 original TU 52: 0x00a2c288..0x00a2ca28 (18 functions)
 */
#include "common.h"
#include "shared.h"
#include "ov12/assert_prog.h"
#include "ov12/rg_geom.h"
#include "ov12/rg_geom_point.h"
#include "ov12/rg_heap.h"
#include "ov12/xrg_rand_int.h"


/*
 * Scaffold-owned .rodata, kept under splat names:
 * ov12:0x00a55100 "../rg_geom_point.euc.c", 0x00a55118
 * "fWeight > RG_FCONST(0.0)", 0x00a55138 "pGeom != NIL".
 */
const char rg_point_assert_expression[16] = "pPoint != NIL";
const char geomPointSourcePath[24] = "../rg_geom_point.euc.c";
const char geomPointAssertWeightPositive[32] = "fWeight > RG_FCONST(0.0)";
const char geomPointAssertGeomNotNil[16] = "pGeom != NIL";

float RgGetGeomGravity(void)
{
    return RG_GEOM_GRAVITY;
}

void InitRgGeomPoint(RgGeomPoint *point, float weight)
{
    if (point == 0) {
        assert_prog(rg_point_assert_expression, geomPointSourcePath, 32);
    }
    if (!(weight > 0.0f)) {
        assert_prog(geomPointAssertWeightPositive, geomPointSourcePath, 33);
    }
    RgGeomInit(&point->geom);
    RgGeomSetType(&point->geom, RG_GEOM_TYPE_POINT);
    RgGeomSetPassTimeMeshod(&point->geom,
                            (RgGeomPassTimeMethod)RgGeomPointPassTime);
    XrgClearVector(point->position);
    XrgClearVector(point->velocity);
    XrgClearVector(point->force);
    XrgClearVector(point->oldPosition);
    point->moveResist = 0.0f;
    point->maxXYSpd = RG_GEOM_NO_LIMIT;
}

RgGeomPoint *CreateRgGeomPoint(float weight)
{
    RgGeomPoint *point;

    point = RgHeapAlloc(InstanceOfRgHeap(), sizeof(RgGeomPoint), geomPointSourcePath,
                        50);
    if (point == 0) {
        assert_prog(rg_point_assert_expression, geomPointSourcePath, 51);
    }
    InitRgGeomPoint(point, weight);
    return point;
}

void RgGeomPointSetPos(RgGeomPoint *point, RgVector position)
{
    if (point == 0) {
        assert_prog(rg_point_assert_expression, geomPointSourcePath, 64);
    }
    XrgCopyVector(point->position, position);
    XrgCopyVector(point->oldPosition, position);
}

void RgGeomPointMovePos(RgGeomPoint *point, RgVector position)
{
    if (point == 0) {
        assert_prog(rg_point_assert_expression, geomPointSourcePath, 71);
    }
    XrgCopyVector(point->position, position);
}

void RgGeomPointResetPos(RgGeomPoint *point)
{
    if (point == 0) {
        assert_prog(rg_point_assert_expression, geomPointSourcePath, 77);
    }
    XrgCopyVector(point->oldPosition, point->position);
}

void RgGeomPointSetVel(RgGeomPoint *point, RgVector velocity)
{
    if (point == 0) {
        assert_prog(rg_point_assert_expression, geomPointSourcePath, 84);
    }
    XrgCopyVector(point->velocity, velocity);
}

void RgGeomPointAddForce(RgGeomPoint *point, RgVector force)
{
    if (point == 0) {
        assert_prog(rg_point_assert_expression, geomPointSourcePath, 92);
    }
    XrgAddVectorXYZ(point->force, point->force, force);
}

void RgGeomPointSetWeight(RgGeomPoint *point, float weight)
{
    if (point == 0) {
        assert_prog(rg_point_assert_expression, geomPointSourcePath, 99);
    }
    if (!(weight > 0.0f)) {
        assert_prog(geomPointAssertWeightPositive, geomPointSourcePath, 100);
    }
    point->weight = weight;
}

void RgGeomPointSetMoveResist(RgGeomPoint *point, float moveResist)
{
    if (point == 0) {
        assert_prog(rg_point_assert_expression, geomPointSourcePath, 107);
    }
    point->moveResist = moveResist;
}

/*
 * The pointer-validation assert reuses the "pGeom != NIL" text
 * (ov12:0x00a55138), not "pPoint != NIL"; the parameter stays an
 * RgGeomPoint, the same object every other setter here validates.
 */
void RgGeomPointSetMaxXYSpd(RgGeomPoint *point, float maxXYSpd)
{
    if (point == 0) {
        assert_prog(geomPointAssertGeomNotNil, geomPointSourcePath, 114);
    }
    if (maxXYSpd <= 0.0f) {
        maxXYSpd = 1.0f;
    }
    point->maxXYSpd = maxXYSpd;
}

void __RgGeomPointGetPos(RgGeomPoint *point, RgVector destination,
                         const char *source_file, int source_line)
{
    if (point == 0) {
        assert_prog(rg_point_assert_expression, source_file, source_line);
    }
    XrgCopyVector(destination, point->position);
}

void __RgGeomPointGetOldPos(RgGeomPoint *point, RgVector destination,
                            const char *source_file, int source_line)
{
    if (point == 0) {
        assert_prog(rg_point_assert_expression, source_file, source_line);
    }
    XrgCopyVector(destination, point->oldPosition);
}

void RgGeomPointGetVel(RgGeomPoint *point, RgVector velocity)
{
    if (point == 0) {
        assert_prog(rg_point_assert_expression, geomPointSourcePath, 139);
    }
    XrgCopyVector(velocity, point->velocity);
}

float RgGeomPointGetSpeed(RgGeomPoint *point)
{
    if (point == 0) {
        assert_prog(rg_point_assert_expression, geomPointSourcePath, 145);
    }
    return XrgLengthVector(point->velocity);
}

float RgGeomPointGetWeight(RgGeomPoint *point)
{
    if (point == 0) {
        assert_prog(rg_point_assert_expression, geomPointSourcePath, 152);
    }
    return point->weight;
}

float RgGeomPointGetMoveResist(RgGeomPoint *point)
{
    if (point == 0) {
        assert_prog(rg_point_assert_expression, geomPointSourcePath, 159);
    }
    return point->moveResist;
}

void RgGeomPointPassTime(RgGeomPoint *pPoint, float deltaTime)
{
    RgVector delta;

    if (pPoint == 0) {
        assert_prog(rg_point_assert_expression, geomPointSourcePath, 172);
    }

    __asm__ __volatile__(
        "lqc2 $vf31, 0(%1)\n\t"
        "sqc2 $vf31, 0(%0)\n\t"
        : : "r"(pPoint->oldPosition), "r"(pPoint->position) : "memory");

    XrgScaleVector(delta, pPoint->force, deltaTime / pPoint->weight);
    XrgAddVectorXYZ(pPoint->velocity, pPoint->velocity, delta);

    if (pPoint->velocity[0] * pPoint->velocity[0] +
        pPoint->velocity[2] * pPoint->velocity[2] >
        pPoint->maxXYSpd * pPoint->maxXYSpd) {
        RgVector horizontal;

        __asm__ __volatile__(
            "lqc2 $vf31, 0(%1)\n\t"
            "sqc2 $vf31, 0(%0)\n\t"
            : : "r"(horizontal), "r"(pPoint->velocity) : "memory");
        horizontal[1] = 0.0f;
        XrgNormalizeVector(horizontal, horizontal);
        horizontal[0] *= pPoint->maxXYSpd;
        horizontal[1] *= pPoint->maxXYSpd;
        horizontal[2] *= pPoint->maxXYSpd;
        horizontal[3] *= pPoint->maxXYSpd;
        pPoint->velocity[0] = horizontal[0];
        pPoint->velocity[2] = horizontal[2];
    }

    if (pPoint->geom.status & RG_GEOM_STATUS_GRAVITY) {
        pPoint->velocity[1] -= RG_GEOM_GRAVITY * deltaTime;
    }

    XrgScaleVector(delta, pPoint->velocity, deltaTime);
    XrgAddVectorXYZ(pPoint->position, pPoint->position, delta);

    if (RgGeomGetStatus(&pPoint->geom) & RG_GEOM_STATUS_MOVE_RESIST) {
        float damping = pPoint->moveResist * deltaTime;

        pPoint->velocity[0] -= pPoint->velocity[0] * damping;
        pPoint->velocity[2] -= pPoint->velocity[2] * damping;
    }

    XrgClearVector(pPoint->force);
}
