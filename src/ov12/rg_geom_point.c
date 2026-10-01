/*
 * OV12 original TU 52: 0x00a2c288..0x00a2ca28 (18 functions)
 */
#include "common.h"
#include "shared.h"
#include "rg_geom_point.h"

extern void assert_prog(const char *expression, const char *source_file,
                        int line);
extern RgHeap *InstanceOfRgHeap(void);
extern void *RgHeapAlloc(void *heap, unsigned int size,
                         const char *source_file, int line);
extern void InitRgGeomPoint(RgGeomPoint *point, float weight);
extern void XrgAddVectorXYZ(RgVector destination, RgVector source1,
                            RgVector source2);
extern float XrgLengthVector(RgVector vector);
extern void XrgScaleVector(RgVector destination, RgVector source, float scale);
extern float XrgNormalizeVector(RgVector destination, RgVector source);
extern void XrgClearVector(RgVector destination);
extern unsigned int RgGeomGetStatus(RgGeom *pGeom);

/*
 * External file-backed witnesses, not candidate-emitted data: this window is
 * asm-owned scaffold data (splat names, no config/symbols/ov12.txt entry).
 *
 * ov12:0x00a55100 contains the source filename "../rg_geom_point.euc.c".
 * ov12:0x00a55118 contains the assertion expression
 * "fWeight > RG_FCONST(0.0)".
 * ov12:0x00a55138 contains the assertion expression "pGeom != NIL".
 */
extern const char D_00A55100[];
extern const char D_00A55118[];
extern const char D_00A55138[];

float RgGetGeomGravity(void)
{
    return 60.0f;
}

/*
 * RgGeomInit, RgGeomSetType and RgGeomSetPassTimeMeshod are original
 * functions of the base geometry TU (rg_geom.c, ov12/tu051), already
 * recovered as C.
 */
extern void RgGeomInit(RgGeom *pGeom);
extern void RgGeomSetType(RgGeom *pGeom, int type);
extern void RgGeomSetPassTimeMeshod(RgGeom *pGeom,
                                     void (*method)(RgGeom *, float));
extern void RgGeomPointPassTime(RgGeomPoint *pPoint, float deltaTime);

/* The type tag InitRgGeomPoint passes to RgGeomSetType; no other OV12
   translation unit names this value yet. */
#define RG_GEOM_TYPE_POINT 0

void InitRgGeomPoint(RgGeomPoint *point, float weight)
{
    if (point == 0) {
        assert_prog(rg_point_assert_expression, D_00A55100, 32);
    }
    if (!(weight > 0.0f)) {
        assert_prog(D_00A55118, D_00A55100, 33);
    }
    RgGeomInit((RgGeom *)point);
    RgGeomSetType((RgGeom *)point, RG_GEOM_TYPE_POINT);
    RgGeomSetPassTimeMeshod((RgGeom *)point,
                            (void (*)(RgGeom *, float))RgGeomPointPassTime);
    XrgClearVector(point->position);
    XrgClearVector(point->velocity);
    XrgClearVector(point->force);
    XrgClearVector(point->oldPosition);
    point->moveResist = 0.0f;
    point->maxXYSpd = 1e8f;
}

RgGeomPoint *CreateRgGeomPoint(float weight)
{
    RgGeomPoint *point;

    point = RgHeapAlloc(InstanceOfRgHeap(), sizeof(RgGeomPoint), D_00A55100,
                        50);
    if (point == 0) {
        assert_prog(rg_point_assert_expression, D_00A55100, 51);
    }
    InitRgGeomPoint(point, weight);
    return point;
}

void RgGeomPointSetPos(RgGeomPoint *point, RgVector position)
{
    if (point == 0) {
        assert_prog(rg_point_assert_expression, D_00A55100, 64);
    }
    XrgCopyVector(point->position, position);
    XrgCopyVector(point->oldPosition, position);
}

void RgGeomPointMovePos(RgGeomPoint *point, RgVector position)
{
    if (point == 0) {
        assert_prog(rg_point_assert_expression, D_00A55100, 71);
    }
    XrgCopyVector(point->position, position);
}

void RgGeomPointResetPos(RgGeomPoint *point)
{
    if (point == 0) {
        assert_prog(rg_point_assert_expression, D_00A55100, 77);
    }
    XrgCopyVector(point->oldPosition, point->position);
}

void RgGeomPointSetVel(RgGeomPoint *point, RgVector velocity)
{
    if (point == 0) {
        assert_prog(rg_point_assert_expression, D_00A55100, 84);
    }
    XrgCopyVector(point->velocity, velocity);
}

void RgGeomPointAddForce(RgGeomPoint *point, RgVector force)
{
    if (point == 0) {
        assert_prog(rg_point_assert_expression, D_00A55100, 92);
    }
    XrgAddVectorXYZ(point->force, point->force, force);
}

void RgGeomPointSetWeight(RgGeomPoint *point, float weight)
{
    if (point == 0) {
        assert_prog(rg_point_assert_expression, D_00A55100, 99);
    }
    if (!(weight > 0.0f)) {
        assert_prog(D_00A55118, D_00A55100, 100);
    }
    point->weight = weight;
}

void RgGeomPointSetMoveResist(RgGeomPoint *point, float moveResist)
{
    if (point == 0) {
        assert_prog(rg_point_assert_expression, D_00A55100, 107);
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
        assert_prog(D_00A55138, D_00A55100, 114);
    }
    if (maxXYSpd <= 0.0f) {
        maxXYSpd = 1.0f;
    }
    point->maxXYSpd = maxXYSpd;
}

void __RgGeomPointGetPos(RgGeomPoint *point, RgPointVector *destination,
                         const char *source_file, int source_line)
{
    if (point == 0) {
        assert_prog(rg_point_assert_expression, source_file, source_line);
    }
    XrgCopyVector((float *)destination,
                  (float *)((const char *)point + 0x20));
}

void __RgGeomPointGetOldPos(RgGeomPoint *point, RgPointVector *destination,
                            const char *source_file, int source_line)
{
    if (point == 0) {
        assert_prog(rg_point_assert_expression, source_file, source_line);
    }
    XrgCopyVector((float *)destination,
                  (float *)((const char *)point + 0x30));
}

void RgGeomPointGetVel(RgGeomPoint *point, RgVector velocity)
{
    if (point == 0) {
        assert_prog(rg_point_assert_expression, D_00A55100, 139);
    }
    XrgCopyVector(velocity, point->velocity);
}

float RgGeomPointGetSpeed(RgGeomPoint *point)
{
    if (point == 0) {
        assert_prog(rg_point_assert_expression, D_00A55100, 145);
    }
    return XrgLengthVector(point->velocity);
}

float RgGeomPointGetWeight(RgGeomPoint *point)
{
    if (point == 0) {
        assert_prog(rg_point_assert_expression, D_00A55100, 152);
    }
    return point->weight;
}

float RgGeomPointGetMoveResist(RgGeomPoint *point)
{
    if (point == 0) {
        assert_prog(rg_point_assert_expression, D_00A55100, 159);
    }
    return point->moveResist;
}

/*
 * This function reads RgGeom's own status word (rg_geom.c's third member,
 * "status") directly through the same (RgGeom *) cast RgGeomInit and
 * RgGeomSetType already use on this same pointer above, alongside a second
 * read through RgGeomGetStatus further down; each TU that looks inside
 * RgGeom keeps its own minimal view of the bytes it touches, the way
 * rg_geom_pillar.c already does for the matrices past this word.
 */
struct RgGeom {
    unsigned char unmodeled_00[8];
    unsigned int status;
};

/*
 * The two RgGeom status bits this function tests: set, gravity is
 * integrated into the vertical velocity below, or the horizontal velocity
 * is damped by moveResist. Bit 1, RG_GEOM_STATUS_ACTIVE, is rg_geom.c's own
 * name for this same status word's low bit.
 */
#define RG_GEOM_STATUS_GRAVITY 2
#define RG_GEOM_STATUS_MOVE_RESIST 4

void RgGeomPointPassTime(RgGeomPoint *pPoint, float deltaTime)
{
    RgVector delta;

    if (pPoint == 0) {
        assert_prog(rg_point_assert_expression, D_00A55100, 172);
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

    if (((RgGeom *)pPoint)->status & RG_GEOM_STATUS_GRAVITY) {
        pPoint->velocity[1] -= 60.0f * deltaTime;
    }

    XrgScaleVector(delta, pPoint->velocity, deltaTime);
    XrgAddVectorXYZ(pPoint->position, pPoint->position, delta);

    if (RgGeomGetStatus((RgGeom *)pPoint) & RG_GEOM_STATUS_MOVE_RESIST) {
        float damping = pPoint->moveResist * deltaTime;

        pPoint->velocity[0] -= pPoint->velocity[0] * damping;
        pPoint->velocity[2] -= pPoint->velocity[2] * damping;
    }

    XrgClearVector(pPoint->force);
}
