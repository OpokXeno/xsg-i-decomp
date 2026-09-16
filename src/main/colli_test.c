#include "common.h"

typedef struct CollisionPoint {
    float x;
    float y;
    float z;
    float w;
} CollisionPoint;

static void swapf(float *left, float *right)
{
    float value = *left;

    *left = *right;
    *right = value;
}

INCLUDE_ASM("asm/main/nonmatchings/colli_test", CheckIntersect);

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

static int GetIntersectionPointLineZ(float *intersection, const float *line_start,
                                     const float *line_end, float z)
{
    if (line_start[2] == line_end[2]) {
        *intersection = 0;
        return 0;
    }

    *intersection = (z - line_start[2]) / (line_end[2] - line_start[2]);
    return 1;
}

static int CheckSlope(const CollisionPoint *p0, const CollisionPoint *p1,
                      const CollisionPoint *p2, const CollisionPoint *p3)
{
    float cross = (p1->x - p0->x) * (p3->z - p2->z)
                - (p1->z - p0->z) * (p3->x - p2->x);

    if (cross == 0.0f)
        return 0;
    if (0.0f < cross)
        return 1;
    return -1;
}

static int CheckInBox(const CollisionPoint *point, const CollisionPoint *lower,
                      const CollisionPoint *upper)
{
    if (point->x < upper->x && lower->x < point->x
            && point->z < upper->z && lower->z < point->z)
        return 1;
    return 0;
}

INCLUDE_ASM("asm/main/nonmatchings/colli_test", CheckBallBoxCollision);

INCLUDE_ASM("asm/main/nonmatchings/colli_test", InitTest_002DBE88);

INCLUDE_ASM("asm/main/nonmatchings/colli_test", ColliTest);
