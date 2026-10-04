/*
 * OV12 original TU 57: 0x00a2e698..0x00a2f650 (12 functions)
 */
#include "common.h"
#include "shared.h"

/* CheckBallBoxCollision is an original function of this same TU and
 * XrgApplyVector and XrgSetVectorXYZ of a neighbouring xrg_* one; all three
 * are still assembly, so this TU declares them. */
static int CheckBallBoxCollision(RgVector contact, RgVector *corners[4], int flag);
static int _CheckPillarBall(const RgVector localPosition,
                            RgVector localNormal,
                            RgVector localContact,
                            RgVector localCorrection,
                            float halfWidth, float halfDepth, float radius);
extern void XrgApplyVector(RgVector destination, const RgMatrix matrix, const RgVector source);
extern void XrgSetVectorXYZ(RgVector destination, float x, float y, float z);

/* InitRgGeomTray is an original function of a neighbouring tray TU
 * (rg_geom_tray.c, ov12/tu056), already recovered as C; CreateRgGeomTray in
 * that same TU is still assembly.  This TU declares both, as it does for the
 * other still-assembly siblings above. */
extern void InitRgGeomTray(RgGeom *geom);
extern RgGeom *CreateRgGeomTray(void);

/* RgGeomSetType is an original function of the base geometry TU
 * (rg_geom.c, ov12/tu051), already recovered as C. */
extern void RgGeomSetType(RgGeom *pGeom, int type);

extern void assert_prog(const char *expression, const char *source_file,
                        int line);

/* Assertion and source strings referenced by InitRgGeomPillar. */
const char D_00A55260[16] = "pPillar != NIL";
const char D_00A55270[32] = "../rg_geom_pillar.euc.c";

/* The type tag InitRgGeomPillar and CreateRgGeomPillar pass to
 * RgGeomSetType; no other OV12 translation unit names this value yet. */
#define RG_GEOM_TYPE_PILLAR 5

/* The four scratch vectors are defined as function-scope statics below. */

/* The members this TU evidences.  RgGeom's two matrix members are the ones
 * rg_geom_tray.c and rg_geom_poly.c already document at +0x20 and +0x60; the
 * pillar's half-extents along local X and Z sit just before them.  The bytes
 * this allocation has no evidence about are named for their offset and stay
 * unread. */
struct RgGeom {
    unsigned char unknown_0x00[0x18];
    float halfWidth;
    float halfDepth;
    RgMatrix local;
    RgMatrix inverseLocal;
};

struct RgGeomPoint {
    unsigned char unmodeled_00[0x20];
    RgVector position;
    RgVector oldPosition;
    unsigned char unmodeled_40[0x30];
    float radius;
};

/* Both diagonal corners of the test box are given the same fixed local Y;
 * only their X and Z carry the pillar's own half-extents. */
#define RG_GEOM_PILLAR_CORNER_Y 3.0f

/*
 * Initialises the pillar: asserts the geometry handle is non-null, then
 * reuses the tray's own initialiser and marks the type as a pillar.
 */
void InitRgGeomPillar(RgGeom *pPillar)
{
    if (pPillar == 0) {
        assert_prog(D_00A55260, D_00A55270, 0x15);
    }
    InitRgGeomTray(pPillar);
    RgGeomSetType(pPillar, RG_GEOM_TYPE_PILLAR);
}

/* Allocates and initialises a tray, then re-tags it as a pillar. */
RgGeom *CreateRgGeomPillar(void)
{
    RgGeom *pillar;

    pillar = CreateRgGeomTray();
    RgGeomSetType(pillar, RG_GEOM_TYPE_PILLAR);
    return pillar;
}

/*
 * Tests the point's path this frame -- the segment from its old position to
 * its current one, the pair __RgGeomPointGetOldPos / __RgGeomPointGetPos
 * reads -- against the pillar's local box, and on a hit transforms the
 * contact point from the pillar's local space back to world space.
 *
 * The path is brought into the pillar's local space with the inverse-local
 * matrix, tested against a box whose two diagonal corners carry the pillar's
 * own half-extents in local X and Z, and the hit point is brought back out
 * with the local matrix.  The two path endpoints and the box's two corners
 * are the function's own four scratch vectors.
 *
 */
int RgGeomPillarCheckPoint(RgGeom *pillar, RgGeomPoint *point, RgVector contact)
{
    static RgVector aFrom;
    static RgVector aTo;
    static RgVector aLeftC;
    static RgVector aRightC;
    RgVector *corners[4];
    int hit;

    __asm__ __volatile__(
        "lqc2 $vf31, 0(%1)\n\t"
        "sqc2 $vf31, 0(%0)\n\t"
        : : "r"(aFrom), "r"(point->oldPosition) : "memory");
    __asm__ __volatile__(
        "lqc2 $vf31, 0(%1)\n\t"
        "sqc2 $vf31, 0(%0)\n\t"
        : : "r"(aTo), "r"(point->position) : "memory");

    {
        const float *inverseLocal = pillar->inverseLocal;

        aTo[3] = 1.0f;
        aFrom[3] = 1.0f;
        XrgApplyVector(aFrom, inverseLocal, aFrom);
        XrgApplyVector(aTo, inverseLocal, aTo);
    }
    aFrom[3] = point->radius;
    aTo[3] = point->radius;

    XrgSetVectorXYZ(aLeftC, -pillar->halfWidth,
                    RG_GEOM_PILLAR_CORNER_Y, -pillar->halfDepth);
    XrgSetVectorXYZ(aRightC, pillar->halfWidth,
                    RG_GEOM_PILLAR_CORNER_Y, pillar->halfDepth);

    corners[0] = &aLeftC;
    corners[1] = &aRightC;
    corners[2] = &aFrom;
    corners[3] = &aTo;
    hit = CheckBallBoxCollision(contact, corners, 0);

    if (hit != 0) {
        const float *local = pillar->local;

        contact[3] = 1.0f;
        __asm__ __volatile__(
            "lqc2 $vf2, 0(%0)\n\t"
            "lqc2 $vf3, 0(%1)\n\t"
            "lqc2 $vf4, 16(%1)\n\t"
            "lqc2 $vf5, 32(%1)\n\t"
            "lqc2 $vf6, 48(%1)\n\t"
            "vmulax.xyzw ACCxyzw, vf3xyzw, vf2x\n\t"
            "vmadday.xyzw ACCxyzw, vf4xyzw, vf2y\n\t"
            "vmaddaz.xyzw ACCxyzw, vf5xyzw, vf2z\n\t"
            "vmaddw.xyzw vf2xyzw, vf6xyzw, vf2w\n\t"
            "sqc2 $vf2, 0(%0)\n\t"
            : : "r"(contact), "r"(local) : "memory");
    }
    return hit;
}

/* Swap the single-precision values addressed by the two local-helper pointers. */
static void swapf(float *left, float *right)
{
    float value = *left;

    *left = *right;
    *right = value;
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_geom_pillar", CheckIntersect_00A2E8A8);

static int GetIntersectionPointLineX(float *intersection, const float *line_start,
                                     const float *line_end, float x)
{
    if (line_start[0] == line_end[0]) {
        *intersection = 0;
        return 0;
    }

    *intersection = (x - line_start[0]) / (line_end[0] - line_start[0]);
    return 1;
}

static int GetIntersectionPointLineZ(float *intersection, const RgVector lineStart,
                                     const RgVector lineEnd, float z)
{
    if (lineStart[2] == lineEnd[2]) {
        *intersection = 0.0f;
        return 0;
    }

    *intersection = (z - lineStart[2]) / (lineEnd[2] - lineStart[2]);
    return 1;
}

static int CheckSlope(const RgVector p0, const RgVector p1, const RgVector p2,
                      const RgVector p3)
{
    float cross = (p1[0] - p0[0]) * (p3[2] - p2[2])
                - (p1[2] - p0[2]) * (p3[0] - p2[0]);

    if (cross == 0.0f) {
        return 0;
    }
    if (cross > 0.0f) {
        return 1;
    }
    return -1;
}

static int CheckInBox(const RgVector point, const RgVector lower,
                      const RgVector upper)
{
    if (point[0] < upper[0] && lower[0] < point[0]
            && point[2] < upper[2] && lower[2] < point[2]) {
        return 1;
    }
    return 0;
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_geom_pillar", CheckBallBoxCollision_00A2EB18);

int RgGeomPillarCheckBall(RgGeom *pillar, RgGeomPoint *point,
                          RgVector worldResults[3])
{
    RgVector localPosition;
    RgVector localNormal;
    RgVector localContact;
    RgVector localCorrection;
    int hit;

    __asm__ __volatile__(
        "lqc2 $vf2, 0(%2)\n\t"
        "lqc2 $vf3, 0(%1)\n\t"
        "lqc2 $vf4, 16(%1)\n\t"
        "lqc2 $vf5, 32(%1)\n\t"
        "lqc2 $vf6, 48(%1)\n\t"
        "vmulax.xyzw ACCxyzw, vf3xyzw, vf2x\n\t"
        "vmadday.xyzw ACCxyzw, vf4xyzw, vf2y\n\t"
        "vmaddaz.xyzw ACCxyzw, vf5xyzw, vf2z\n\t"
        "vmaddw.xyzw vf2xyzw, vf6xyzw, vf2w\n\t"
        "sqc2 $vf2, 0(%0)\n\t"
        : : "r"(localPosition), "r"(pillar->inverseLocal),
            "r"(point->position) : "memory");

    hit = _CheckPillarBall(localPosition, localNormal, localContact,
                           localCorrection, pillar->halfWidth,
                           pillar->halfDepth, point->radius);
    if (hit == 0) {
        return hit;
    }

    __asm__ __volatile__(
        "lqc2 $vf2, 0(%1)\n\t"
        "lqc2 $vf3, 0(%2)\n\t"
        "lqc2 $vf4, 16(%2)\n\t"
        "lqc2 $vf5, 32(%2)\n\t"
        "lqc2 $vf6, 48(%2)\n\t"
        "vmulax.xyzw ACCxyzw, vf3xyzw, vf2x\n\t"
        "vmadday.xyzw ACCxyzw, vf4xyzw, vf2y\n\t"
        "vmaddaz.xyzw ACCxyzw, vf5xyzw, vf2z\n\t"
        "vmaddw.xyzw vf2xyzw, vf6xyzw, vf2w\n\t"
        "sqc2 $vf2, 0(%0)\n\t"
        : : "r"(worldResults[0]), "r"(localContact),
            "r"(pillar->local) : "memory");
    __asm__ __volatile__(
        "lqc2 $vf2, 0(%1)\n\t"
        "lqc2 $vf3, 0(%2)\n\t"
        "lqc2 $vf4, 16(%2)\n\t"
        "lqc2 $vf5, 32(%2)\n\t"
        "lqc2 $vf6, 48(%2)\n\t"
        "vmulax.xyzw ACCxyzw, vf3xyzw, vf2x\n\t"
        "vmadday.xyzw ACCxyzw, vf4xyzw, vf2y\n\t"
        "vmaddaz.xyzw ACCxyzw, vf5xyzw, vf2z\n\t"
        "vmaddw.xyzw vf2xyzw, vf6xyzw, vf2w\n\t"
        "sqc2 $vf2, 0(%0)\n\t"
        : : "r"(worldResults[1]), "r"(localNormal),
            "r"(pillar->local) : "memory");
    localCorrection[3] = 0.0f;
    __asm__ __volatile__(
        "lqc2 $vf2, 0(%1)\n\t"
        "lqc2 $vf3, 0(%2)\n\t"
        "lqc2 $vf4, 16(%2)\n\t"
        "lqc2 $vf5, 32(%2)\n\t"
        "lqc2 $vf6, 48(%2)\n\t"
        "vmulax.xyzw ACCxyzw, vf3xyzw, vf2x\n\t"
        "vmadday.xyzw ACCxyzw, vf4xyzw, vf2y\n\t"
        "vmaddaz.xyzw ACCxyzw, vf5xyzw, vf2z\n\t"
        "vmaddw.xyzw vf2xyzw, vf6xyzw, vf2w\n\t"
        "sqc2 $vf2, 0(%0)\n\t"
        : : "r"(worldResults[2]), "r"(localCorrection),
            "r"(pillar->local) : "memory");
    return hit;
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_geom_pillar", _CheckPillarBall);
