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


const char rg_point_assert_expression[16] = "pPoint != NIL";
const char geomPointSourcePath[24] = "../rg_geom_point.euc.c";
const char geomPointAssertWeightPositive[32] = "fWeight > RG_FCONST(0.0)";
const char geomPointAssertGeomNotNil[16] = "pGeom != NIL";

float RgGetGeomGravity(void)
{
    return RG_GEOM_GRAVITY;
}

/* Initializes a point geometry with the given weight, cleared vectors and no horizontal speed limit. */
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

/* Allocates and initializes a point geometry with the given weight. */
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

/* Sets the position and the old position to the same value. */
void RgGeomPointSetPos(RgGeomPoint *point, RgVector position)
{
    if (point == 0) {
        assert_prog(rg_point_assert_expression, geomPointSourcePath, 64);
    }
    XrgCopyVector(point->position, position);
    XrgCopyVector(point->oldPosition, position);
}

/* Moves the position and leaves the old position unchanged. */
void RgGeomPointMovePos(RgGeomPoint *point, RgVector position)
{
    if (point == 0) {
        assert_prog(rg_point_assert_expression, geomPointSourcePath, 71);
    }
    XrgCopyVector(point->position, position);
}

/* Sets the old position to the current position. */
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

/* Adds force to the force accumulated for the next time step. */
void RgGeomPointAddForce(RgGeomPoint *point, RgVector force)
{
    if (point == 0) {
        assert_prog(rg_point_assert_expression, geomPointSourcePath, 92);
    }
    XrgAddVectorXYZ(point->force, point->force, force);
}

/* Sets the weight, which must be positive. */
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
 * The pointer-validation assert uses the "pGeom != NIL" text, not
 * "pPoint != NIL"; the parameter stays an RgGeomPoint, the object every
 * other setter here validates.
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

/* Copies the position into destination. source_file and source_line give the caller's location for the assert. */
void __RgGeomPointGetPos(RgGeomPoint *point, RgVector destination,
                         const char *source_file, int source_line)
{
    if (point == 0) {
        assert_prog(rg_point_assert_expression, source_file, source_line);
    }
    XrgCopyVector(destination, point->position);
}

/* Copies the old position into destination. source_file and source_line give the caller's location for the assert. */
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

/*
 * Advances the point by deltaTime: integrates force into velocity, limits
 * horizontal speed, applies gravity, moves the position and applies move
 * resistance.
 */
void RgGeomPointPassTime(RgGeomPoint *pPoint, float deltaTime)
{
    RgVector delta;

    if (pPoint == 0) {
        assert_prog(rg_point_assert_expression, geomPointSourcePath, 172);
    }

    /* Copies position to oldPosition through vf31 before the position is integrated. */
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

        /* Copies the velocity to horizontal through vf31 so its horizontal part can be clamped. */
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
