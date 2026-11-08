#include "common.h"
#include "shared.h"
#include "main/xgl_2.h"
#include "outer_product.h"
#include "main/outer_product.h"
#include "vu0_macro.h"

extern void CalcVerticalVector(const Vector4 *first, const Vector4 *second,
                               const Vector4 *third, Vector4 *vertical);

/* Stores the cross product of `left` and `right` (x, y, z) in `destination`. */
void OuterProduct(const Vector4 *left, const Vector4 *right,
                  AlignedVector4 *destination)
{
    AlignedVector4 result;

    result.x = left->y * right->z - left->z * right->y;
    result.y = left->z * right->x - left->x * right->z;
    result.z = left->x * right->y - left->y * right->x;
    *destination = result;
}

/*
 * Stores in `vertical` the cross product of the first edge (first to second) with the
 * plane normal of the three points.  It lies in the plane and is perpendicular to that edge.
 */
void CalcVerticalVector(const Vector4 *first, const Vector4 *second,
                        const Vector4 *third, Vector4 *vertical)
{
    Vector4 first_edge;
    Vector4 second_edge;
    Vector4 normal;

    Vu0SubVectorXYZ(&first_edge, second, first);
    Vu0SubVectorXYZ(&second_edge, third, first);
    xglVectorOuter(&normal, &first_edge, &second_edge);
    xglVectorOuter(vertical, &first_edge, &normal);
}

/* Returns the length of (first - third) projected onto the unit vertical vector of the three points. */
float CalcLength(const Vector4 *first, const Vector4 *second, const Vector4 *third)
{
    Vector4 vertical;
    Vector4 delta;
    float length;

    CalcVerticalVector(first, second, third, &vertical);
    xglVectorNormal(&vertical, &vertical);
    Vu0SubVectorXYZ(&delta, first, third);
    xglVectorInner(&length, &vertical, &delta);
    return length;
}

/*
 * Stores in `cross_point` where the line through `third` and `fourth` meets the
 * plane of the first three points.
 */
void CalcCrossPoint(const Vector4 *first, const Vector4 *second,
                    const Vector4 *third, const Vector4 *fourth,
                    Vector4 *cross_point)
{
    Vector4 normal;
    float intersection_values[2];

    intersection_values[1] = CalcLength(first, second, third);
    CalcVerticalVector(first, second, third, &normal);
    xglVectorNormal(&normal, &normal);
    Vu0SubVectorXYZ(cross_point, fourth, third);
    xglVectorInner(&intersection_values[0], cross_point, &normal);
    intersection_values[0] = intersection_values[1] / intersection_values[0];
    xglVectorLength(&intersection_values[1], cross_point);
    xglVectorNormal(cross_point, cross_point);
    cross_point->x *= intersection_values[0] * intersection_values[1];
    cross_point->y *= intersection_values[0] * intersection_values[1];
    cross_point->z *= intersection_values[0] * intersection_values[1];
    __asm__ __volatile__(
        "lqc2 vf20, 0(%0)\n\t"
        "lqc2 vf21, 0(%1)\n\t"
        "vadd.xyzw vf20, vf20, vf21\n\t"
        "sqc2 vf20, 0(%0)"
        :
        : "r"(cross_point), "r"(third)
        : "memory");
}

/*
 * Classifies `point` against the line from `first` to `second` in the XZ plane:
 * 1 or 2 for the two sides, 0 when it is on the line.
 */
int CheckPointLine(const Vector4 *first, const Vector4 *second,
                   const Vector4 *point)
{
    Vector4 first_delta;
    Vector4 second_delta;
    float cross;
    float first_x = first->x;
    int result = 1;

    first_delta.x = second->x - first_x;
    second_delta.x = point->x - first_x;
    first_delta.z = second->z - first->z;
    second_delta.z = point->z - first->z;
    cross = first_delta.x * second_delta.z
          - first_delta.z * second_delta.x;

    if (!(0.0f < cross)) {
        if (cross < 0.0f) {
            result = 2;
        } else {
            result = 0;
        }
    }
    return result;
}

/* Returns 1 when the two segments cross in the XZ plane, otherwise 0. */
int CheckCrossLine(const Vector4 *first_start, const Vector4 *first_end,
                   const Vector4 *second_start, const Vector4 *second_end)
{
    int first_start_side = CheckPointLine(first_start, first_end, second_start);
    int first_end_side = CheckPointLine(first_start, first_end, second_end);

    if (first_start_side != first_end_side) {
        int second_start_side = CheckPointLine(second_start, second_end, first_start);
        int second_end_side = CheckPointLine(second_start, second_end, first_end);

        if (second_start_side != second_end_side) {
            return 1;
        }
    }
    return 0;
}

/* Returns 1 when the segment from start to end passes within `radius` of `center` in the XZ plane, otherwise 0. */
int CheckCrossCircle(const Vector4 *start, const Vector4 *end,
                     const Vector4 *center, float radius)
{
    Vector4 middle;
    Vector4 direction;
    float projection_x;
    float half_length;
    float along;
    float x;
    float z;
    int result;

    direction.x = end->x - start->x;
    direction.y = 0.0f;
    direction.z = end->z - start->z;
    direction.w = 1.0f;
    half_length = __builtin_sqrtf(direction.x * direction.x
                                  + direction.z * direction.z) * 0.5f;
    xglVectorNormal(&direction, &direction);
    half_length += radius;
    result = 0;
    middle.x = start->x + (end->x - start->x) * 0.5f;
    middle.z = start->z + (end->z - start->z) * 0.5f;
    x = center->x - middle.x;
    z = center->z - middle.z;
    along = x * direction.x + z * direction.z;
    if (!(half_length < __builtin_fabsf(along))) {
        projection_x = along * direction.x;
        x = center->x - (projection_x + middle.x);
        z = center->z - (along * direction.z + middle.z);
        result = x * x + z * z < radius * radius;
    }
    return result;
}

/* Returns the distance between two points in 3D. */
float CheckDist3D(const Vector4 *first, const Vector4 *second)
{
    return __builtin_sqrtf((first->x - second->x) * (first->x - second->x)
                           + (first->y - second->y) * (first->y - second->y)
                           + (first->z - second->z) * (first->z - second->z));
}

/* Returns the distance between two points in the XZ plane. */
float CheckDist2D(const Vector4 *first, const Vector4 *second)
{
    return __builtin_sqrtf((first->x - second->x) * (first->x - second->x)
                           + (first->z - second->z) * (first->z - second->z));
}
