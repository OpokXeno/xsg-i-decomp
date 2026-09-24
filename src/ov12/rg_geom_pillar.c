/*
 * OV12 original TU 57: 0x00a2e698..0x00a2f650 (12 functions)
 */
#include "common.h"
#include "shared.h"

/* CheckBallBoxCollision is an original function of this same TU and
 * XrgApplyVector and XrgSetVectorXYZ of a neighbouring xrg_* one; all three
 * are still assembly, so this TU declares them. */
static int CheckBallBoxCollision(RgVector contact, RgVector *corners[4], int flag);
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

/* Referenced by InitRgGeomPillar: the assertion text "pPillar != NIL" and
 * this TU's own original file name "../rg_geom_pillar.euc.c", both
 * scaffold-owned .rodata (config/tu-build.json, data_ownership.rodata). */
extern const char D_00A55260[];
extern const char D_00A55270[];

/* The type tag InitRgGeomPillar and CreateRgGeomPillar pass to
 * RgGeomSetType; no other OV12 translation unit names this value yet. */
#define RG_GEOM_TYPE_PILLAR 5

/* Scaffold-owned (.bss still owner: asm, config/tu-build.json): the function's
 * own four file-local scratch vectors, kept under their splat names
 * (docs/naming.md, "Scaffold-owned data keeps its splat name").  The original
 * object names them aFrom.0, aTo.1, aLeftC.2 and aRightC.3, the GCC spelling
 * of four function-scope statics declared in that order. */
extern RgVector D_00A5AB80;
extern RgVector D_00A5AB90;
extern RgVector D_00A5ABA0;
extern RgVector D_00A5ABB0;

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
    unsigned char unknown_0x00[0x20];
    RgVector position;
    RgVector oldPosition;
    unsigned char unknown_0x40[0x30];
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
 * are the function's own scratch vectors, aFrom, aTo, aLeftC and aRightC in
 * the original object, still named for their addresses here because this TU's
 * .bss is scaffold-owned.
 *
 */
int RgGeomPillarCheckPoint(RgGeom *pillar, RgGeomPoint *point, RgVector contact)
{
    RgVector *corners[4];
    int hit;

    __asm__ __volatile__(
        "lqc2 $vf31, 0(%1)\n\t"
        "sqc2 $vf31, 0(%0)\n\t"
        : : "r"(D_00A5AB80), "r"(point->oldPosition) : "memory");
    __asm__ __volatile__(
        "lqc2 $vf31, 0(%1)\n\t"
        "sqc2 $vf31, 0(%0)\n\t"
        : : "r"(D_00A5AB90), "r"(point->position) : "memory");

    {
        const float *inverseLocal = pillar->inverseLocal;

        D_00A5AB90[3] = 1.0f;
        D_00A5AB80[3] = 1.0f;
        XrgApplyVector(D_00A5AB80, inverseLocal, D_00A5AB80);
        XrgApplyVector(D_00A5AB90, inverseLocal, D_00A5AB90);
    }
    D_00A5AB80[3] = point->radius;
    D_00A5AB90[3] = point->radius;

    XrgSetVectorXYZ(D_00A5ABA0, -pillar->halfWidth,
                    RG_GEOM_PILLAR_CORNER_Y, -pillar->halfDepth);
    XrgSetVectorXYZ(D_00A5ABB0, pillar->halfWidth,
                    RG_GEOM_PILLAR_CORNER_Y, pillar->halfDepth);

    corners[0] = &D_00A5ABA0;
    corners[1] = &D_00A5ABB0;
    corners[2] = &D_00A5AB80;
    corners[3] = &D_00A5AB90;
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

INCLUDE_ASM("asm/nonmatchings/ov12/rg_geom_pillar", GetIntersectionPointLineX_00A2E9B0);

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

INCLUDE_ASM("asm/nonmatchings/ov12/rg_geom_pillar", RgGeomPillarCheckBall);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_geom_pillar", _CheckPillarBall);
