#include "common.h"

#include "shared.h"

#include "main/xgl_2.h"

/* Query layout and floating return follow the original UnduCheck accesses. */
typedef struct Vector3 { float x, y, z; } Vector3;
typedef struct Point4 { float x, y, z, w; } Point4;
typedef struct ParabolaVec { float x, y, z, w; } ParabolaVec;
typedef struct LayoutHeader LayoutHeader;
typedef unsigned int GameLoopStateWords[];
extern GameLoopStateWords GameLoopState;
typedef struct PlayerActorHeaderView {
    unsigned char unmodeled_000[0x4e0];
    LayoutHeader *data_header;
} PlayerActorHeaderView;
typedef struct UnduParam {
    int queryFlags;
    unsigned char unmodeled_04[4];
    short attrMask;
    unsigned char unmodeled_0a[0x0e];
    LayoutHeader *header;
    unsigned char unmodeled_1c[4];
    long long attribute;
    unsigned char unmodeled_28[8];
    long long excluded_units; /* Check_Undu stores the excluded mask at +0x30. */
    long long query_mode;     /* Check_Undu stores the included mask at +0x38. */
} UnduParam;
extern void UnduParamInit(UnduParam *param);
extern float UnduCheck(const Point4 *position, void *exclude, UnduParam *param);
extern LayoutHeader *UnduDataGetHeader(int map_index, int unit_index);
extern UnduParam UnduTest;
extern UnduParam UnduTemp;
extern const float D_004D81F0;
extern unsigned short *DataSpline;
extern float cosf(float angle);
extern float atan2f(float y, float x);
extern float Get_Distance(const Point4 *first, const Point4 *second);


/* MARK: provisional extern block. The four words are _gp-relative .sdata
   witnesses at 0x004d81a8..0x004d81b4, not proven original declarations.
   Names below record roles observed in this function only. */

extern int CheckPointLine(const Vector4 *first, const Vector4 *second,
                          const Vector4 *point);

/* Check_Angle: defined below in this file, still assembler. */

extern int Check_Angle(float angle, float rangeStart, float rangeEnd);

extern const float D_004D81B8;

extern const float D_004D81BC;

extern const float D_004D81C0;

extern const float D_004D81C4;

extern int Check_Undu(const Point4 *start, const Point4 *end, int map_index,
                      Point4 *destination, int query_mode,
                      int excluded_units, int flags);

extern int CrossPointUwamono(const Point4 *start, const Point4 *end,
                             Point4 *intersection, float radius);

/* MARK: provisional extern block. All eight words are _gp-relative .sdata
   witnesses at 0x004d81cc..0x004d81e8. They are not proven original
   declarations; names below record roles observed in this function only. */

/* Role name for TwoPiD:
   subtracted to wrap the cursor down at/above the high clamp. */

float Get_Decimal_Surplus_for_Radius(float angle)
{
    if (angle <= 0.0f) {
        float negative_pi = -3.1415927f;
        if (angle < negative_pi) {
            do {
                angle += 6.2831855f;
            } while (angle < negative_pi);
        }
    } else {
        while (3.1415927f <= angle) {
            angle -= 6.2831855f;
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

void Get_MiddlePoint(const float *start, const float *following,
                     short point_index, short point_count, float *result)
{
    if (point_index == point_count) {
        result[0] = following[0];
        result[1] = following[1];
        result[2] = following[2];
        result[3] = 1.0f;
    } else {
        result[0] = start[0] + (following[0] - start[0]) * (float)point_index
                    / (float)point_count;
        result[1] = start[1] + (following[1] - start[1]) * (float)point_index
                    / (float)point_count;
        result[2] = start[2] + (following[2] - start[2]) * (float)point_index
                    / (float)point_count;
        result[3] = 1.0f;
    }
}

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

int Check_Straight_ID(const Point4 *start, const Point4 *end,
                      short count, Point4 *destination, int map_index)
{
    Point4 current;
    Point4 previous;
    short checked = 0;
    int signed_count = count;

    UnduTemp.header = UnduDataGetHeader(map_index, 0x8000);
    previous.x = start->x;
    previous.y = start->y;
    previous.z = start->z;
    if (signed_count >= 0) {
        float count_as_float = (float)signed_count;
        do {
            float step = (float)checked;
            current.x = start->x + (end->x - start->x) * step / count_as_float;
            current.y = start->y + (end->y - start->y) * step / count_as_float;
            current.z = start->z + (end->z - start->z) * step / count_as_float;
            if (UnduCheck(&current, 0, &UnduTemp) == -1000.0f
                || HitCheckMapUnitPos(&current) != -1) {
                destination->x = previous.x;
                destination->y = previous.y;
                destination->z = previous.z;
                return (short)(checked - 1);
            }
            checked++;
            previous.x = current.x;
            previous.y = current.y;
            previous.z = current.z;
        } while (checked <= signed_count);
    }
    destination->x = end->x;
    destination->y = end->y;
    destination->z = end->z;
    return signed_count;
}

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

float Get_Angle_Relative(const Point4 *first, const Point4 *second,
                         float reference)
{
    float relative = Get_Angle(first, second) - reference;

    if (relative < -3.1415927f)
        relative += 6.2831855f;
    if (3.1415927f < relative)
        relative -= 6.2831855f;
    return relative;
}

int Check_CrossingOver(const Vector4 *first_start, const Vector4 *first_end,
                       const Vector4 *second_start, const Vector4 *second_end)
{
    int first_start_side = CheckPointLine(first_start, first_end, second_start);
    int first_end_side = CheckPointLine(first_start, first_end, second_end);

    if (first_start_side != first_end_side) {
        int second_start_side = CheckPointLine(second_start, second_end, first_start);
        int second_end_side = CheckPointLine(second_start, second_end, first_end);

        if (second_start_side != second_end_side)
            return 1;
    }
    return 0;
}

int Check_InsideFan(float facing, float radius, float fan_width_degrees,
                    float crossing_radius, const Point4 *origin,
                    const Point4 *target, int map_index)
{
    Point4 intersection;
    float angle;
    float half_width;

    if (radius < Get_Distance3D(origin, target)) {
        return 0;
    }
    angle = Get_Angle(origin, target);
    half_width = fan_width_degrees / 180.0f * D_004D81B8 * 0.5f;
    if (Check_Angle(angle, facing - half_width, facing + half_width) == 0) {
        return 0;
    }
    if (map_index == -1) {
        return 1;
    }
    if (Check_Undu(origin, target, map_index, &intersection, 1, 0, 0x400) == 0) {
        return 0;
    }
    if (D_004D81BC <= Get_Distance(target, &intersection)) {
        return 0;
    }
    if (D_004D81C0 <= __builtin_fabsf(target->y - intersection.y)) {
        return 0;
    }
    if (D_004D81C4 < __builtin_fabsf(origin->y - target->y)) {
        return 0;
    }
    return CrossPointUwamono(origin, target, &intersection, crossing_radius) != 1;
}

int Check_InsideFan_Wooo(const Point4 *origin, const Point4 *target,
                         float facing, float radius, float fanWidthDegrees)
{
    int inside = 0;

    if (!(radius < Get_Distance3D(origin, target))) {
        float angle = Get_Angle(origin, target);
        float halfWidth = (fanWidthDegrees / 180.0f) * 3.1415927f * 0.5f;

        inside = Check_Angle(angle, facing - halfWidth, facing + halfWidth) != 0;
    }
    return inside;
}

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
    if (__builtin_fabsf(cursor) < 3.1415927f)
        cursor = current + cursor / speed;
    else if (cursor < -3.1415927f)
        cursor = current + (cursor + 6.2831855f) / speed;
    else
        cursor = current - (6.2831855f - cursor) / speed;
    if (cursor <= -3.1415927f)
        cursor += 6.2831855f;
    if (3.1415927f <= cursor)
        cursor -= 6.2831855f;
    return cursor;
}

void Get_EnemyIDPos(const Point4 *origin, Point4 *destination)
{
    Point4 sample;
    Point4 checked;
    Point4 intersection;
    short index = 0;
    float nearest = 1000.0f;
    int debugColor[4]; /* RGBA; written but never read in the release build */

    *destination = *origin;
    UnduParamInit(&UnduTemp);
    UnduTemp.queryFlags = 0;
    UnduTemp.attrMask = 3;
    debugColor[0] = 255;
    debugColor[1] = 127;
    debugColor[2] = 0;
    debugColor[3] = 128;
    UnduTemp.header = UnduDataGetHeader(772, 32768);
    if (UnduCheck(origin, 0, &UnduTemp) != -1000.0f) {
        destination->x = origin->x;
        destination->y = origin->y;
        destination->z = origin->z;
    } else {
        for (; index < 8; index++) {
            float angle = (float)index * 6.2831855f * 0.125f;
            float distance;
            sample.x = origin->x + 2.0f * sinf(angle);
            sample.y = origin->y;
            sample.z = origin->z + 2.0f * cosf(angle);
            sample.w = 1.0f;
            Check_Undu(&sample, origin, 772, &checked, 1, 0, 0);
            CrossPointUwamono(&sample, origin, &intersection, 1.0f);
            distance = Get_Distance(origin, &checked);
            if (distance != 0.0f && distance < nearest) {
                nearest = distance;
                destination->x = checked.x;
                destination->y = checked.y;
                destination->z = checked.z;
            }
        }
    }
    destination->w = 1.0f;
}

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

int Get_Attr(const Point4 *position, int mapIndex, int attrMask)
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
    return (int)UnduTest.attribute;
}

int Get_Attr_NU(const Point4 *position, int mapIndex, int attrMask)
{
    UnduParamInit(&UnduTest);
    UnduTest.queryFlags = 0;
    UnduTest.attrMask = (short)attrMask;
    if (mapIndex == 0) {
        UnduTest.header = ((PlayerActorHeaderView *)GameLoopState[1])->data_header;
    } else {
        UnduTest.header = UnduDataGetHeader(mapIndex, 0x8000);
    }
    UnduCheck(position, 0, &UnduTest);
    return (int)UnduTest.attribute;
}

int Check_Undu(const Point4 *start, const Point4 *end, int map_index,
                Point4 *destination, int query_mode, int excluded_units,
                int flags)
{
    Point4 delta;

    UnduParamInit(&UnduTest);
    UnduTest.queryFlags = 0;
    UnduTest.attrMask = (short)(flags | 0x813);
    if (map_index == 0) {
        UnduTest.header = ((PlayerActorHeaderView *)GameLoopState[1])->data_header;
    } else {
        UnduTest.header = UnduDataGetHeader(map_index, 0x8000);
    }
    delta.x = end->x - start->x;
    delta.y = end->y - start->y;
    delta.z = end->z - start->z;
    delta.w = 1.0f;
    *destination = *start;
    UnduTest.query_mode = query_mode;
    UnduTest.excluded_units = excluded_units;
    UnduCheck(destination, &delta, &UnduTest);
    return Get_Distance3D(end, destination) <= D_004D81F0;
}

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

void GetBSplineLoop(short frame, short count, int period_value,
                    Vector4 *points, Vector4 *destination)
{
    short period = period_value;
    unsigned short index = (frame / period) % count;
    unsigned short phase = (frame % period) * (2048 / period);
    float first_weight = DataSpline[(phase + 4304) / 2];
    float second_weight = DataSpline[(phase + 4306) / 2];
    float third_weight = DataSpline[(phase + 4308) / 2];
    float fourth_weight = DataSpline[(phase + 4310) / 2];
    unsigned short second_index = (index + 1) % count;
    unsigned short third_index = (index + 2) % count;
    unsigned short fourth_index = (index + 3) % count;

    destination->x = (first_weight * points[index].x + second_weight * points[second_index].x
                      + third_weight * points[third_index].x + fourth_weight * points[fourth_index].x)
                     / 24576.0f;
    destination->y = (first_weight * points[index].y + second_weight * points[second_index].y
                      + third_weight * points[third_index].y + fourth_weight * points[fourth_index].y)
                     / 24576.0f;
    destination->z = (first_weight * points[index].z + second_weight * points[second_index].z
                      + third_weight * points[third_index].z + fourth_weight * points[fourth_index].z)
                     / 24576.0f;
}

INCLUDE_ASM("asm/main/nonmatchings/get", GetBSplineLoopDummy);
