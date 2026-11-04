/*
 * OV12 original TU 57: 0x00a2e698..0x00a2f650 (12 functions)
 */
#include "common.h"
#include "shared.h"

/* CheckBallBoxCollision is an original function of this same TU and
 * XrgApplyVector and XrgSetVectorXYZ of a neighbouring xrg_* one; all three
 * are still assembly, so this TU declares them. */
static int CheckBallBoxCollision(RgVector contact, RgVector *corners[4], int flag);
static int _CheckPillarBall(const RgVector localPosition, RgVector localNormal, RgVector localContact, RgVector localCorrection, float halfWidth, float halfDepth, float radius);
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

static int CheckIntersect(const RgVector lineStart, const RgVector lineEnd,
                          const RgVector lower, const RgVector upper)
{
    float determinant;
    float lineDz;
    float cross;
    float endX;
    float upperZ;
    float lowerZ;
    float upperX;
    float lowerX;
    float endZ;
    float startZ;
    float originDx;
    float originDz;
    int reverse;
    int result;

    determinant = (upper[0] - lower[0]) * (lineEnd[2] - lineStart[2]);
    cross = (lineEnd[0] - lineStart[0]) * (upper[2] - lower[2]);
    if (determinant == cross) {
        return 0;
    }

    determinant -= cross;
    if (determinant < 0.0f) {
        reverse = 1;
        determinant = -determinant;
    } else {
        reverse = 0;
    }

    endX = lineEnd[0];
    upperZ = upper[2];
    lowerZ = lower[2];
    upperX = upper[0];
    lowerX = lower[0];
    endZ = lineEnd[2];
    startZ = lineStart[2];
    originDx = lower[0] - lineStart[0];
    originDz = lowerZ - startZ;
    lineDz = endZ - startZ;
    cross = originDz * (endX - lineStart[0]) - originDx * lineDz;
    if (reverse) {
        cross = -cross;
    }
    if (cross < 0.0f || determinant < cross) {
        return 0;
    }

    cross = originDz * (upperX - lowerX) - originDx * (upperZ - lowerZ);
    if (reverse) {
        cross = -cross;
    }
    result = 0;
    if (!(cross < 0.0f)) {
        result = 1;
        if (determinant < cross) {
            result = 0;
        }
    }
    return result;
}

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

/* Which side of the test box the segment start lies on, and whether the hit
 * is on a Z face. */
enum {
    RG_SIDE_Z_LOW = 1,
    RG_SIDE_Z_HIGH = 2,
    RG_SIDE_X_LOW = 4,
    RG_SIDE_X_HIGH = 8,
    RG_SIDE_HIT_Z = 16
};

/* The frame reserves 16 bytes after the two bounds corners that
 * CheckBallBoxCollision never accesses. */
typedef struct RgBoundsFrame {
    RgVector bounds[2];
    unsigned char unmodeled_20[16];
} RgBoundsFrame;

static int CheckBallBoxCollision(RgVector contact, RgVector *corners[4], int flag)
{
    RgBoundsFrame frame;
    float *cornerA = corners[0][0];
    float *cornerB = corners[1][0];
    float fraction;
    int side;
    int slope;
    int intersection;

    fraction = corners[2][0][3];
    if (cornerA[0] > cornerB[0]) {
        frame.bounds[1][0] = cornerA[0] + fraction;
        frame.bounds[0][0] = cornerB[0] - fraction;
    } else {
        frame.bounds[1][0] = cornerB[0] + fraction;
        frame.bounds[0][0] = cornerA[0] - fraction;
    }
    if (cornerA[2] > cornerB[2]) {
        frame.bounds[1][2] = cornerA[2] + fraction;
        frame.bounds[0][2] = cornerB[2] - fraction;
    } else {
        frame.bounds[1][2] = cornerB[2] + fraction;
        frame.bounds[0][2] = cornerA[2] - fraction;
    }

    if (CheckInBox(corners[2][0], frame.bounds[0], frame.bounds[1])) {
        if (contact) {
            __asm__ __volatile__(
                "lqc2 $vf31, 0(%1)\n\t"
                "sqc2 $vf31, 0(%0)\n\t"
                : : "r"(contact), "r"(corners[2][0]) : "memory");
        }
        return 1;
    }
    if (!CheckInBox(corners[3][0], frame.bounds[0], frame.bounds[1])
            && !CheckIntersect(corners[2][0], corners[3][0], frame.bounds[0], frame.bounds[1])) {
        swapf(&frame.bounds[0][0], &frame.bounds[1][0]);
        if (!CheckIntersect(corners[2][0], corners[3][0], frame.bounds[0], frame.bounds[1])) {
            return 0;
        }
        swapf(&frame.bounds[0][0], &frame.bounds[1][0]);
    }

    side = 0;
    if (corners[2][0][2] <= frame.bounds[0][2]) {
        side = RG_SIDE_Z_LOW;
    } else if (frame.bounds[1][2] <= corners[2][0][2]) {
        side = RG_SIDE_Z_HIGH;
    }
    if (corners[2][0][0] <= frame.bounds[0][0]) {
        side |= RG_SIDE_X_LOW;
    } else if (frame.bounds[1][0] <= corners[2][0][0]) {
        side |= RG_SIDE_X_HIGH;
    }

    switch (side) {
    case RG_SIDE_Z_LOW:
        intersection = GetIntersectionPointLineZ(&fraction, corners[2][0], corners[3][0], frame.bounds[0][2]);
        if (!intersection) {
            return intersection;
        }
        side |= RG_SIDE_HIT_Z;
        break;
    case RG_SIDE_Z_HIGH:
        intersection = GetIntersectionPointLineZ(&fraction, corners[2][0], corners[3][0], frame.bounds[1][2]);
        if (!intersection) {
            return intersection;
        }
        side |= RG_SIDE_HIT_Z;
        break;
    case RG_SIDE_X_LOW:
        if (!GetIntersectionPointLineX(&fraction, corners[2][0], corners[3][0], frame.bounds[0][0])) {
            return 0;
        }
        break;
    case RG_SIDE_X_HIGH:
        if (!GetIntersectionPointLineX(&fraction, corners[2][0], corners[3][0], frame.bounds[1][0])) {
            return 0;
        }
        break;
    case RG_SIDE_X_LOW | RG_SIDE_Z_LOW:
        slope = CheckSlope(corners[2][0], corners[3][0], corners[2][0], frame.bounds[0]);
        if (slope != -1) {
            if (slope != 1) {
                return 0;
            }
            intersection = GetIntersectionPointLineZ(&fraction, corners[2][0], corners[3][0], frame.bounds[0][2]);
            if (!intersection) {
                return intersection;
            }
            side |= RG_SIDE_HIT_Z;
        } else {
            if (!GetIntersectionPointLineX(&fraction, corners[2][0], corners[3][0], frame.bounds[0][0])) {
                return 0;
            }
        }
        break;
    case RG_SIDE_X_LOW | RG_SIDE_Z_HIGH:
        swapf(&frame.bounds[0][2], &frame.bounds[1][2]);
        slope = CheckSlope(corners[2][0], corners[3][0], corners[2][0], frame.bounds[1]);
        if (slope != -1) {
            if (slope != 1) {
                return 0;
            }
            if (!GetIntersectionPointLineX(&fraction, corners[2][0], corners[3][0], frame.bounds[0][0])) {
                return 0;
            }
        } else {
            intersection = GetIntersectionPointLineZ(&fraction, corners[2][0], corners[3][0], frame.bounds[0][2]);
            if (!intersection) {
                return intersection;
            }
            side |= RG_SIDE_HIT_Z;
        }
        swapf(&frame.bounds[0][2], &frame.bounds[1][2]);
        break;
    case RG_SIDE_X_HIGH | RG_SIDE_Z_LOW:
        swapf(&frame.bounds[0][0], &frame.bounds[1][0]);
        slope = CheckSlope(corners[2][0], corners[3][0], corners[2][0], frame.bounds[0]);
        if (slope != -1) {
            if (slope != 1) {
                return 0;
            }
            if (!GetIntersectionPointLineX(&fraction, corners[2][0], corners[3][0], frame.bounds[0][0])) {
                return 0;
            }
        } else {
            intersection = GetIntersectionPointLineZ(&fraction, corners[2][0], corners[3][0], frame.bounds[0][2]);
            if (!intersection) {
                return intersection;
            }
            side |= RG_SIDE_HIT_Z;
        }
        swapf(&frame.bounds[0][0], &frame.bounds[1][0]);
        break;
    case RG_SIDE_X_HIGH | RG_SIDE_Z_HIGH:
        slope = CheckSlope(corners[2][0], corners[3][0], corners[2][0], frame.bounds[1]);
        if (slope != -1) {
            if (slope != 1) {
                return 0;
            }
            intersection = GetIntersectionPointLineZ(&fraction, corners[2][0], corners[3][0], frame.bounds[1][2]);
            if (!intersection) {
                return intersection;
            }
            side |= RG_SIDE_HIT_Z;
        } else {
            if (!GetIntersectionPointLineX(&fraction, corners[2][0], corners[3][0], frame.bounds[1][0])) {
                return 0;
            }
        }
        break;
    default:
        return 0;
    }

    if (contact) {
        fraction -= 0.001f;
        switch (flag) {
        case 0:
        default:
            contact[0] = (corners[3][0][0] - corners[2][0][0]) * fraction + corners[2][0][0];
            contact[1] = (corners[3][0][1] - corners[2][0][1]) * fraction + corners[2][0][1];
            contact[2] = (corners[3][0][2] - corners[2][0][2]) * fraction + corners[2][0][2];
            break;
        case 1:
            if (side & RG_SIDE_HIT_Z) {
                contact[0] = corners[3][0][0];
                contact[2] = (corners[3][0][2] - corners[2][0][2]) * fraction + corners[2][0][2];
            } else {
                contact[0] = (corners[3][0][0] - corners[2][0][0]) * fraction + corners[2][0][0];
                contact[2] = corners[3][0][2];
            }
            contact[1] = (corners[3][0][1] - corners[2][0][1]) * fraction + corners[2][0][1];
            break;
        }
    }
    return 1;
}

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

extern float XrgNormalizeVector(RgVector destination, const RgVector source);

/* Pushes the local normal out of a box corner by the scaled corner offset. */
static inline void _AddCornerOffset(RgVector localNormal, RgVector cornerOffset)
{
    __asm__ __volatile__(
        "lqc2 $vf30, 0(%0)\n\t"
        "lqc2 $vf31, 0(%1)\n\t"
        "vadd.xyzw $vf30xyzw, $vf30xyzw, $vf31xyzw\n\t"
        "sqc2 $vf30, 0(%0)\n\t"
        : : "r"(localNormal), "r"(cornerOffset) : "memory");
}

/* Copies the corner probe point into the contact vector. */
static inline void _CopyContact(RgVector localContact, RgVector probe)
{
    __asm__ __volatile__(
        "lqc2 $vf31, 0(%1)\n\t"
        "sqc2 $vf31, 0(%0)\n\t"
        : : "r"(localContact), "r"(probe) : "memory");
}

static int _CheckPillarBall(const RgVector localPosition,
                            RgVector localNormal,
                            RgVector localContact,
                            RgVector localCorrection,
                            float halfWidth, float halfDepth, float radius)
{
    RgVector probe;
    RgVector cornerOffsets[4];
    float x;
    float z;
    float negHalfWidth;
    float negHalfDepth;
    float faceDistance;
    float cornerDistance;
    int cell;
    unsigned int sector;
    int hit = 0;

    __asm__ __volatile__(
        "lqc2 $vf31, 0(%1)\n\t"
        "sqc2 $vf31, 0(%0)\n\t"
        : : "r"(localNormal), "r"(localPosition) : "memory");
    __asm__ __volatile__(
        "lqc2 $vf31, 0(%1)\n\t"
        "sqc2 $vf31, 0(%0)\n\t"
        : : "r"(localContact), "r"(localPosition) : "memory");
    __asm__ __volatile__(
        "lqc2 $vf31, 0(%1)\n\t"
        "sqc2 $vf31, 0(%0)\n\t"
        : : "r"(probe), "r"(localPosition) : "memory");
    localCorrection[0] = localCorrection[1] = localCorrection[2] = localCorrection[3] = 0.0f;

    negHalfWidth = -halfWidth;
    x = localNormal[0];
    if (x < negHalfWidth) {
        cell = 0;
    } else if (halfWidth < x) {
        cell = 2;
    } else {
        cell = 1;
    }

    negHalfDepth = -halfDepth;
    z = localNormal[2];
    if (!(z < negHalfDepth)) {
        if (halfDepth < z) {
            cell += 6;
        } else {
            cell += 3;
        }
    }

    switch (cell) {
    case 1:
        faceDistance = negHalfDepth - z;
        hit = faceDistance < radius;
        if (hit) {
            float penetration = radius - faceDistance;
            localNormal[2] = z - penetration;
            localContact[2] = negHalfDepth;
            localCorrection[2] = -1.0f;
        }
        break;

    case 3:
        faceDistance = negHalfWidth - x;
        hit = faceDistance < radius;
        if (hit) {
            float penetration = radius - faceDistance;
            localNormal[0] = x - penetration;
            localContact[0] = negHalfWidth;
            localCorrection[0] = -1.0f;
        }
        break;

    case 5:
        faceDistance = x - halfWidth;
        hit = faceDistance < radius;
        if (hit) {
            float penetration = radius - faceDistance;
            localNormal[0] = x + penetration;
            localContact[0] = halfWidth;
            localCorrection[0] = 1.0f;
        }
        break;

    case 7:
        faceDistance = z - halfDepth;
        hit = faceDistance < radius;
        if (hit) {
            float penetration = radius - faceDistance;
            localNormal[2] = z + penetration;
            localContact[2] = halfDepth;
            localCorrection[2] = 1.0f;
        }
        break;

    case 0:
    {
        float *cornerOffset = cornerOffsets[0];

        probe[0] = negHalfWidth;
        probe[2] = negHalfDepth;
        __asm__ __volatile__(
            "lqc2 $vf30, 0(%1)\n\t"
            "lqc2 $vf31, 0(%2)\n\t"
            "vsub.xyzw $vf30xyzw, $vf30xyzw, $vf31xyzw\n\t"
            "sqc2 $vf30, 0(%0)\n\t"
            : : "r"(cornerOffset), "r"(localNormal), "r"(probe) : "memory");
        cornerOffsets[0][1] = 0.0f;
        cornerDistance = XrgNormalizeVector(localCorrection, cornerOffset);
        if (cornerDistance < radius) {
            float penetration = radius - cornerDistance;

            cornerOffsets[0][0] = localCorrection[0] * penetration;
            cornerOffsets[0][1] = localCorrection[1] * penetration;
            cornerOffsets[0][2] = localCorrection[2] * penetration;
            cornerOffsets[0][3] = localCorrection[3] * penetration;
            _AddCornerOffset(localNormal, cornerOffset);
            hit = 1;
        } else {
            hit = 0;
        }
        _CopyContact(localContact, probe);
        break;
    }

    case 2:
    {
        float *cornerOffset = cornerOffsets[1];

        probe[0] = halfWidth;
        probe[2] = negHalfDepth;
        __asm__ __volatile__(
            "lqc2 $vf30, 0(%1)\n\t"
            "lqc2 $vf31, 0(%2)\n\t"
            "vsub.xyzw $vf30xyzw, $vf30xyzw, $vf31xyzw\n\t"
            "sqc2 $vf30, 0(%0)\n\t"
            : : "r"(cornerOffset), "r"(localNormal), "r"(probe) : "memory");
        cornerOffsets[1][1] = 0.0f;
        cornerDistance = XrgNormalizeVector(localCorrection, cornerOffset);
        if (cornerDistance < radius) {
            float penetration = radius - cornerDistance;

            cornerOffsets[1][0] = localCorrection[0] * penetration;
            cornerOffsets[1][1] = localCorrection[1] * penetration;
            cornerOffsets[1][2] = localCorrection[2] * penetration;
            cornerOffsets[1][3] = localCorrection[3] * penetration;
            _AddCornerOffset(localNormal, cornerOffset);
            hit = 1;
        } else {
            hit = 0;
        }
        _CopyContact(localContact, probe);
        break;
    }

    case 6:
    {
        float *cornerOffset = cornerOffsets[2];

        probe[0] = negHalfWidth;
        probe[2] = halfDepth;
        __asm__ __volatile__(
            "lqc2 $vf30, 0(%1)\n\t"
            "lqc2 $vf31, 0(%2)\n\t"
            "vsub.xyzw $vf30xyzw, $vf30xyzw, $vf31xyzw\n\t"
            "sqc2 $vf30, 0(%0)\n\t"
            : : "r"(cornerOffset), "r"(localNormal), "r"(probe) : "memory");
        cornerOffsets[2][1] = 0.0f;
        cornerDistance = XrgNormalizeVector(localCorrection, cornerOffset);
        if (cornerDistance < radius) {
            float penetration = radius - cornerDistance;

            cornerOffsets[2][0] = localCorrection[0] * penetration;
            cornerOffsets[2][1] = localCorrection[1] * penetration;
            cornerOffsets[2][2] = localCorrection[2] * penetration;
            cornerOffsets[2][3] = localCorrection[3] * penetration;
            _AddCornerOffset(localNormal, cornerOffset);
            hit = 1;
        } else {
            hit = 0;
        }
        _CopyContact(localContact, probe);
        break;
    }

    case 8:
    {
        float *cornerOffset = cornerOffsets[3];

        probe[0] = halfWidth;
        probe[2] = halfDepth;
        __asm__ __volatile__(
            "lqc2 $vf30, 0(%1)\n\t"
            "lqc2 $vf31, 0(%2)\n\t"
            "vsub.xyzw $vf30xyzw, $vf30xyzw, $vf31xyzw\n\t"
            "sqc2 $vf30, 0(%0)\n\t"
            : : "r"(cornerOffset), "r"(localNormal), "r"(probe) : "memory");
        cornerOffsets[3][1] = 0.0f;
        cornerDistance = XrgNormalizeVector(localCorrection, cornerOffset);
        if (cornerDistance < radius) {
            float penetration = radius - cornerDistance;

            cornerOffsets[3][0] = localCorrection[0] * penetration;
            cornerOffsets[3][1] = localCorrection[1] * penetration;
            cornerOffsets[3][2] = localCorrection[2] * penetration;
            cornerOffsets[3][3] = localCorrection[3] * penetration;
            _AddCornerOffset(localNormal, cornerOffset);
            hit = 1;
        } else {
            hit = 0;
        }
        _CopyContact(localContact, probe);
        break;
    }

    case 4:
        sector = (z <= -x) ? 0 : 1;
        if (!(z <= x)) {
            sector += 2;
        }
        hit = 1;
        switch (sector) {
        case 0:
            localNormal[2] = negHalfDepth - radius;
            localContact[2] = negHalfDepth;
            localCorrection[2] = -1.0f;
            break;
        case 1:
            localNormal[0] = halfWidth + radius;
            localContact[0] = halfWidth;
            localCorrection[0] = 1.0f;
            break;
        case 2:
            localNormal[0] = negHalfWidth - radius;
            localContact[0] = negHalfWidth;
            localCorrection[0] = -1.0f;
            break;
        case 3:
            localNormal[2] = halfDepth + radius;
            localContact[2] = halfDepth;
            localCorrection[2] = 1.0f;
            break;
        }
        break;
    }

    return hit;
}
