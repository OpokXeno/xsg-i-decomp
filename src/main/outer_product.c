#include "common.h"
#include "shared.h"
#include "main/xgl_2.h"
#include "outer_product.h"

typedef union {
    Vector4 vector;
    u64 words[2];
} OuterProductVector;

void OuterProduct(const OuterProductVector *left, const OuterProductVector *right,
                  OuterProductVector *destination)
{
    OuterProductVector result;

    result.vector.x = left->vector.y * right->vector.z
                    - left->vector.z * right->vector.y;
    result.vector.y = left->vector.z * right->vector.x
                    - left->vector.x * right->vector.z;
    result.vector.z = left->vector.x * right->vector.y
                    - left->vector.y * right->vector.x;
    __builtin_memcpy(destination, &result, sizeof(result));
}

void CalcVerticalVector(const Vector4 *first, const Vector4 *second,
                        const Vector4 *third, Vector4 *vertical)
{
    Vector4 first_edge;
    Vector4 second_edge;
    Vector4 normal;

    __asm__ __volatile__(
        "lqc2 vf3, 0(%1)\n\t"
        "lqc2 vf2, 0(%2)\n\t"
        "vsub.xyz vf2, vf2, vf3\n\t"
        "sqc2 vf2, 0(%0)"
        :
        : "r"(&first_edge), "r"(first), "r"(second)
        : "memory");
    __asm__ __volatile__(
        "lqc2 vf3, 0(%1)\n\t"
        "lqc2 vf2, 0(%2)\n\t"
        "vsub.xyz vf2, vf2, vf3\n\t"
        "sqc2 vf2, 0(%0)"
        :
        : "r"(&second_edge), "r"(first), "r"(third)
        : "memory");
    xglVectorOuter(&normal, &first_edge, &second_edge);
    xglVectorOuter(vertical, &first_edge, &normal);
}

extern void CalcVerticalVector(const Vector4 *first, const Vector4 *second,
                               const Vector4 *third, Vector4 *vertical);
extern void xglVectorNormal(Vector4 *destination, const Vector4 *source);
extern void xglVectorInner(float *result, const Vector4 *left, const Vector4 *right);

float CalcLength(const Vector4 *first, const Vector4 *second, const Vector4 *third)
{
    Vector4 vertical;
    Vector4 delta;
    float length;

    CalcVerticalVector(first, second, third, &vertical);
    xglVectorNormal(&vertical, &vertical);
    __asm__ __volatile__(
        "lqc2 vf3, 0(%0)\n\t"
        "lqc2 vf2, 0(%1)\n\t"
        "vsub.xyz vf2, vf2, vf3\n\t"
        "sqc2 vf2, 0(%2)"
        :
        : "r"(third), "r"(first), "r"(&delta)
        : "memory");
    xglVectorInner(&length, &vertical, &delta);
    return length;
}

void CalcCrossPoint(const Vector4 *first, const Vector4 *second,
                    const Vector4 *third, const Vector4 *fourth,
                    Vector4 *cross_point)
{
    Vector4 normal;
    float intersection_values[2];

    intersection_values[1] = CalcLength(first, second, third);
    CalcVerticalVector(first, second, third, &normal);
    xglVectorNormal(&normal, &normal);
    __asm__ __volatile__(
        "lqc2 vf3, 0(%1)\n\t"
        "lqc2 vf2, 0(%2)\n\t"
        "vsub.xyz vf2, vf2, vf3\n\t"
        "sqc2 vf2, 0(%0)"
        :
        : "r"(cross_point), "r"(third), "r"(fourth)
        : "memory");
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

INCLUDE_ASM("asm/main/nonmatchings/outer_product", CheckCrossCircle);

float CheckDist3D(const Vector4 *first, const Vector4 *second)
{
    return __builtin_sqrtf((first->x - second->x) * (first->x - second->x)
                           + (first->y - second->y) * (first->y - second->y)
                           + (first->z - second->z) * (first->z - second->z));
}

float CheckDist2D(const Vector4 *first, const Vector4 *second)
{
    return __builtin_sqrtf((first->x - second->x) * (first->x - second->x)
                           + (first->z - second->z) * (first->z - second->z));
}
