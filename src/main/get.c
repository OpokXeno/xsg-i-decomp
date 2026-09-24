#include "common.h"
#include "shared.h"
#include "main/xgl_2.h"
#include "get.h"

float Get_Decimal_Surplus_for_Radius(float angle)
{
    if (angle <= 0.0f) {
        float negative_pi = radius_neg_pi;
        if (angle < negative_pi) {
            do {
                angle += radius_two_pi_a;
            } while (angle < negative_pi);
        }
    } else {
        while (radius_pi <= angle) {
            angle -= radius_two_pi_b;
        }
    }
    return angle;
}

INCLUDE_ASM("asm/main/nonmatchings/get", Check_MoveSquare);

void Get_Point_By_AngleLength(const Vector4 *source, Vector4 *destination,
                              float angle, float length)
{
    destination->x = source->x + sinf(angle) * length;
    destination->y = source->y;
    destination->z = source->z + cosf(angle) * length;
    destination->w = source->w;
}

INCLUDE_ASM("asm/main/nonmatchings/get", Get_MiddlePoint);

void Get_MiddlePoint_Parabora(const ParabolaVec *first, const ParabolaVec *second,
                              int current, int total, float height,
                              ParabolaVec *destination)
{
    int current_value = (short)current;
    int total_value = (short)total;
    int midpoint = (int)((short)total +
                         ((unsigned int)(total << 16) >> 31)) >> 1;
    int delta = midpoint - current_value;
    float adjusted_height = height * (float)(delta * delta)
                           / (float)(midpoint * midpoint);
    height -= adjusted_height;

    if (current_value == total_value) {
        destination->x = second->x;
        destination->y = second->y;
        destination->z = second->z;
        destination->w = second->w;
    } else {
        destination->x = first->x + (second->x - first->x)
                       * (float)current_value / (float)total_value;
        destination->y = first->y + (second->y - first->y)
                       * (float)current_value / (float)total_value + height;
        destination->z = first->z + (second->z - first->z)
                       * (float)current_value / (float)total_value;
        destination->w = first->w + (second->w - first->w)
                       * (float)current_value / (float)total_value;
    }
}

INCLUDE_ASM("asm/main/nonmatchings/get", Check_Wall);

INCLUDE_ASM("asm/main/nonmatchings/get", Check_Octpus);

INCLUDE_ASM("asm/main/nonmatchings/get", Get_WallSpace);

void Get_One_Step(float length, const Point4 *source,
                  const Point4 *destination, Point4 *output)
{
    float x_step = destination->x - source->x;
    float z_step = destination->z - source->z;
    float inverse_distance = 1.0f / Get_Distance(source, destination);

    output->x = x_step * inverse_distance * length;
    output->y = 0.0f;
    output->z = z_step * inverse_distance * length;
    output->w = 1.0f;
}

INCLUDE_ASM("asm/main/nonmatchings/get", Check_Straight);

INCLUDE_ASM("asm/main/nonmatchings/get", Check_Straight_ID);

float Get_Distance(const Point4 *first, const Point4 *second)
{
    float first_x = first->x;
    float first_z = first->z;

    return __builtin_sqrtf((second->x - first_x) * (second->x - first_x)
                           + (second->z - first_z) * (second->z - first_z));
}

float Get_Distance3D(const Point4 *first, const Point4 *second)
{
    float first_x = first->x;
    float first_y = first->y;
    float first_z = first->z;

    return __builtin_sqrtf((second->x - first_x) * (second->x - first_x)
                           + (second->y - first_y) * (second->y - first_y)
                           + (second->z - first_z) * (second->z - first_z));
}

float Get_Angle(const Point4 *first, const Point4 *second)
{
    return atan2f(second->x - first->x, second->z - first->z);
}

INCLUDE_ASM("asm/main/nonmatchings/get", Check_Angle);

/* MARK: provisional extern block. The four words are _gp-relative .sdata
   witnesses at 0x004d81a8..0x004d81b4, not proven original declarations.
   Names below record roles observed in this function only. */
float Get_Angle_Relative(const Point4 *first, const Point4 *second,
                         float reference)
{
    float relative = Get_Angle(first, second) - reference;

    if (relative < k_negative_pi)
        relative += kFullCircleAdd;
    if (k_positive_pi < relative)
        relative -= kFullCircleSub;
    return relative;
}

INCLUDE_ASM("asm/main/nonmatchings/get", Check_CrossingOver);

INCLUDE_ASM("asm/main/nonmatchings/get", Check_InsideFan);

/* Check_Angle: defined below in this file, still assembler. */
extern int Check_Angle(float angle, float rangeStart, float rangeEnd);

int Check_InsideFan_Wooo(const Point4 *origin, const Point4 *target,
                         float facing, float radius, float fanWidthDegrees)
{
    int inside = 0;

    if (!(radius < Get_Distance3D(origin, target))) {
        float angle = Get_Angle(origin, target);
        float halfWidth = (fanWidthDegrees / 180.0f) * D_004D81C8 * 0.5f;

        inside = Check_Angle(angle, facing - halfWidth, facing + halfWidth) != 0;
    }
    return inside;
}

/* MARK: provisional extern block. All eight words are _gp-relative .sdata
   witnesses at 0x004d81cc..0x004d81e8. They are not proven original
   declarations; names below record roles observed in this function only. */
        /* Role name for TwoPiD:
   subtracted to wrap the cursor down at/above the high clamp. */
float Get_Cursol_by_Reduce_Speed_Angle_Loop(float current, float target,
                                            float speed)
{
    float cursor;

    if (current == target)
        return current;
    /* Difference first, then the same variable carries the adjusted
       cursor position for the remainder of the loop. */
    cursor = target - current;
    /* MARK: builtin form emits abs.s under -fno-builtin; a plain fabsf
       call would tail-call instead and break the match. */
    if (__builtin_fabsf(cursor) < kAbsBoundPi)
        cursor = current + cursor / speed;
    else if (cursor < kNegPi)
        cursor = current + (cursor + kTwoPiAddend) / speed;
    else
        cursor = current - (kTwoPiSubtrahend - cursor) / speed;
    if (cursor <= kNegPiLimit)
        cursor += kTwoPiWrapLo;
    if (kPiLimit <= cursor)
        cursor -= kTwoPiWrapHi;
    return cursor;
}

INCLUDE_ASM("asm/main/nonmatchings/get", Get_EnemyIDPos);

float Get_Multi_Max_Under(float value, float step, float maximum)
{
    if (value < maximum) {
        maximum -= step;
        while (value <= maximum)
            value += step;
    } else {
        maximum -= step;
        while (!(value <= maximum))
            value -= step;
    }
    return value;
}

void Get_HeightAttr(const Point4 *position, int mapIndex, int attrMask,
                    UnduParam *param)
{
    UnduParamInit(param);
    param->attrMask = (short)(attrMask | 0x800);
    param->queryFlags = 0;
    if (mapIndex == 0) {
        param->header = ((PlayerActorHeaderView *)GameLoopState[1])->data_header;
    } else {
        param->header = UnduDataGetHeader(mapIndex, 0x8000);
    }
    UnduCheck(position, 0, param);
}

void Get_Height(const Point4 *position, int mapIndex, int attrMask)
{
    UnduParamInit(&UnduTest);
    UnduTest.queryFlags = 0;
    UnduTest.attrMask = (short)(attrMask | 0x800);
    if (mapIndex == 0) {
        UnduTest.header = ((PlayerActorHeaderView *)GameLoopState[1])->data_header;
    } else {
        UnduTest.header = UnduDataGetHeader(mapIndex, 0x8000);
    }
    UnduCheck(position, 0, &UnduTest);
}

INCLUDE_ASM("asm/main/nonmatchings/get", Get_Attr);

INCLUDE_ASM("asm/main/nonmatchings/get", Get_Attr_NU);

INCLUDE_ASM("asm/main/nonmatchings/get", Check_Undu);

int Get_Rnd(int min, int max)
{
    return min + xglSRand() % (max - min + 1);
}

void BSpline_Init(Vector4 *destination, const Vector3 *source)
{
    short i;

    for (i = 0; i < 4; i++) {
        destination[i].x = source->x;
        destination[i].y = source->y;
        destination[i].z = source->z;
        destination[i].w = 1.0f;
    }
}

void BSpline_Add(short frame, short count, short period,
                 Vector4 *destination, const Vector3 *source, short offset)
{
    unsigned short index;

    index = (frame / period + offset) % count;
    destination[index].x = source->x;
    destination[index].y = source->y;
    destination[index].z = source->z;
    destination[index].w = 1.0f;
}

INCLUDE_ASM("asm/main/nonmatchings/get", GetBSplineLoop);

INCLUDE_ASM("asm/main/nonmatchings/get", GetBSplineLoopDummy);
