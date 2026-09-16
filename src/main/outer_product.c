#include "common.h"
#include "shared.h"

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

INCLUDE_ASM("asm/main/nonmatchings/outer_product", CalcVerticalVector);

INCLUDE_ASM("asm/main/nonmatchings/outer_product", CalcLength);

INCLUDE_ASM("asm/main/nonmatchings/outer_product", CalcCrossPoint);

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

INCLUDE_ASM("asm/main/nonmatchings/outer_product", CheckDist2D);
