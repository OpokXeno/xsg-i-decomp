#include "common.h"

#include "shared.h"

#include "main/init_uwamono_sys.h"

#include "main/xgl_2.h"

/* Query layout and floating return follow the original UnduCheck accesses. */
typedef struct SplineMotionParameters {
    unsigned char unmodeled_00[8];
    float step_length;
} SplineMotionParameters;

typedef struct SplineMotionActor {
    unsigned char unmodeled_00[0x80];
    unsigned char number;
    unsigned char unmodeled_81[0xc0 - 0x81];
    SplineMotionParameters parameters;
} SplineMotionActor;

typedef struct EnemySplineState {
    unsigned char unmodeled_00[0x40];
    short frame;
    unsigned char unmodeled_42[4];
    short segment_steps;
    unsigned char unmodeled_48[0x38b0 - 0x48];
} EnemySplineState;

extern EnemySplineState enepc[];

typedef struct Vector3 { float x, y, z; } Vector3;
typedef Vector4 Point4;
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
extern const float unduDistanceTolerance;
extern unsigned short *DataSpline;
extern float cosf(float angle);
extern float atan2f(float y, float x);
float Get_Distance(const Point4 *first, const Point4 *second);
float Get_Decimal_Surplus_for_Radius(float angle);

/* MARK: provisional extern block. The four words are _gp-relative .sdata
   witnesses at 0x004d81a8..0x004d81b4, not proven original declarations.
   Names below record roles observed in this function only. */

extern int CheckPointLine(const Vector4 *first, const Vector4 *second,
                          const Vector4 *point);

/* Check_Angle: defined below in this file, still assembler. */

int Check_Angle(float angle, float rangeStart, float rangeEnd);

extern const float getPi;

extern const float fanMinDistance;

extern const float fanHeightTolerance;

extern const float fanMaxHeightDelta;

int Check_Undu(const Point4 *start, const Point4 *end, int map_index,
                Point4 *destination, int query_mode, int excluded_units,
                int flags);

/* MARK: provisional extern block. All eight words are _gp-relative .sdata
   witnesses at 0x004d81cc..0x004d81e8. They are not proven original
   declarations; names below record roles observed in this function only. */

/* Role name for TwoPiD:
   subtracted to wrap the cursor down at/above the high clamp. */

#include "get.h"

extern const char moveSquarePlaceholderText[];
extern const char moveSquareForecastText[];

extern void xglFontDebugPrintf(int x, int y, const char *format, ...);

extern long EneTerritoryID[20];

extern int HitCheckMapUnitPosSize(const Vector4 *position, float radius);

int Check_Straight(const Point4 *start, const Point4 *end, short steps,
                   unsigned long *destination, float radius, int map_index,
                   int excluded_actors, int territory);

extern int Check_Actor(const Vector4 *position, short number);

int Check_CrossingOver(const Vector4 *first_start, const Vector4 *first_end,
                       const Vector4 *second_start, const Vector4 *second_end);

int Get_Attr(const Point4 *position, int mapIndex, int attrMask);

extern const float wallFullTurnRadians;

extern const float wallAngleStep;

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

int Check_MoveSquare(const Point4 *first, const Point4 *second,
                     Point4 *destination, int map_index, float radius)
{
    Point4 first_intersection;
    Point4 first_crossing;
    Point4 second_intersection;
    Point4 second_crossing;
    Point4 third_intersection;
    Point4 third_crossing;
    Point4 fourth_intersection;
    Point4 fourth_crossing;
    Point4 first_corner;
    Point4 second_corner;
    int debug_color[4];
    float first_x = first->x;
    float second_x = second->x;
    float second_y = second->y;
    float second_z = second->z;
    float first_z = first->z;
    int crossed = 0;

    debug_color[0] = 0;
    debug_color[1] = 0;
    debug_color[2] = 255;
    debug_color[3] = 128;
    first_corner.x = first_x;
    first_corner.y = second_y;
    first_corner.z = second_z;
    second_corner.x = second_x;
    second_corner.y = second_y;
    second_corner.z = first_z;
    {
        float adjusted_x = (first_x - second_x) < 0.0f
            ? first_x + -0.1f : first_x + 0.1f;
        float z_delta = first_z - second_z;
        first_corner.x = adjusted_x;
        {
            float corner_z = second_corner.z;
            float adjusted_z = corner_z + (z_delta < 0.0f ? -0.1f : 0.1f);
            second_corner.z = adjusted_z;
        }
    }
    Check_Undu(second, &first_corner, map_index,
               &first_intersection, 1, 0, 0);
    Check_Undu(second, &second_corner, map_index,
               &second_intersection, 1, 0, 0);
    CrossPointUwamono(second, &first_intersection, radius, &first_crossing);
    CrossPointUwamono(second, &second_intersection, radius, &second_crossing);
    first_crossing.w = 0.1f;
    second_crossing.w = 0.1f;
    first_intersection = first_crossing;
    second_intersection = second_crossing;

    first_corner.x = second->x;
    first_corner.y = first->y;
    first_corner.z = first->z;
    second_corner.x = first->x;
    second_corner.y = first->y;
    second_corner.z = second->z;
    first_corner.x += (second->x - first->x) < 0.0f
                                      ? -0.1f : 0.1f;
    second_corner.z += (second->z - first->z) < 0.0f
                                      ? -0.1f : 0.1f;
    Check_Undu(first, &first_corner, map_index,
               &third_intersection, 1, 0, 0);
    Check_Undu(first, &second_corner, map_index,
               &fourth_intersection, 1, 0, 0);
    CrossPointUwamono(first, &third_intersection, radius, &third_crossing);
    CrossPointUwamono(first, &fourth_intersection, radius, &fourth_crossing);
    third_crossing.w = 0.1f;
    fourth_crossing.w = 0.1f;
    third_intersection = third_crossing;
    fourth_intersection = fourth_crossing;

    xglFontDebugPrintf(8, 0x64, moveSquarePlaceholderText);
    if (Check_CrossingOver((const Vector4 *)second,
                           (const Vector4 *)&first_intersection,
                           (const Vector4 *)first,
                           (const Vector4 *)&fourth_intersection) == 1) {
        *destination = first_intersection;
        destination->z = second->z;
        crossed = 1;
        xglFontDebugPrintf(8, 0x64, moveSquareForecastText);
    }
    if (Check_CrossingOver((const Vector4 *)second,
                           (const Vector4 *)&second_intersection,
                           (const Vector4 *)first,
                           (const Vector4 *)&third_intersection) == 1) {
        *destination = second_intersection;
        destination->x = second->x;
        crossed = 1;
        xglFontDebugPrintf(8, 0x64, moveSquareForecastText);
    }
    return crossed;
}

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

void Check_Wall(float distance_scale, float wall_parameter,
                const float *position, signed char *blocked, int count_argument,
                int map_index, u8 actor_number, int territory)
{
    short count = (short)count_argument;
    short first = 0;
    short end = count;
    short index;
    Point4 sample;
    const float full_turn = 6.2831855f;
    float angle;

    if (count <= 0) {
        first = (int)xglSRand() % (count < 0 ? -(int)count : (int)count);
        end = (short)(first + 1);
        for (index = 0; index < (count < 0 ? -(int)count : (int)count); index++)
            blocked[index] = 0;
    }

    sample.w = 0.0f;
    index = first;
    if (index < end) {
        float direction_count = (float)count;
        do {
        float direction_index = (float)index;
        angle = direction_index * full_turn;
        angle /= direction_count;
        sample.x = position[0] + sinf(angle) * distance_scale;
        sample.y = position[1];
        sample.z = position[2] + cosf(angle) * distance_scale;
        blocked[index] = 0;
        if (!Get_HeightAttr(&sample, map_index, 0, &UnduTest)) {
            blocked[index] = 1;
        } else if (HitCheckMapUnitPosSize((const Vector4 *)&sample,
                                          wall_parameter) != -1) {
            blocked[index] = 1;
        } else if (territory != 0 &&
                   UnduTest.attribute != EneTerritoryID[territory]) {
            blocked[index] = 1;
        }
        index++;
        } while (index < end);
    }
}

void Check_Octpus(float length, float radius, const Point4 *origin,
                  Point4 *destination, int count_argument, int map_index,
                  int excluded_actors, int territory)
{
    short count = (short)count_argument;
    Point4 sample;
    Point4 checked[16];
    float distance[16];
    signed char used[16];
    short ranked[16];
    short iteration;

    iteration = 0;
    if (count > 0) {
        float full_turn = 6.2831855f;
        float direction_count = count;
        do {
        float angle = (float)iteration;
        angle *= full_turn;
        angle /= direction_count;
        sample.x = origin->x + sinf(angle) * length;
        sample.y = origin->y;
        sample.z = origin->z + cosf(angle) * length;
        sample.w = 0.0f;
        Check_Straight(origin, &sample, 16,
                       (unsigned long *)&checked[iteration], radius,
                       map_index, excluded_actors, (short)territory);
        distance[iteration] = Get_Distance(origin, &checked[iteration]);
        used[iteration] = 0;
        iteration++;
        } while (iteration < count);
    }
    for (iteration = 0; iteration < count; iteration++) {
        short direction;
        short best = -1;
        float farthest = -1.0f;
        for (direction = 0; direction < count; direction++) {
            if (farthest < distance[direction] && used[direction] == 0) {
                farthest = distance[direction];
                best = direction;
            }
        }
        ranked[iteration] = best;
        used[best] = 1;
    }
    for (iteration = 0; iteration < 4; iteration++) {
        short first = xglSRand() % count;
        short second = xglSRand() % count;
        short temporary = ranked[first];
        ranked[first] = ranked[second];
        ranked[second] = temporary;
    }
    *destination = checked[ranked[0]];
}

float Get_WallSpace(int unused, const signed char *wall_flags,
                    short count, float angle)
{
    int signed_count = count;
    float count_as_float = (float)signed_count;
    int half_count = count / 2;
    short checked = 0;
    short selected = 0;
    short start = (short)(angle / (wallFullTurnRadians / count_as_float)
                          + (float)half_count);

    if (half_count > 0) {
        short steps = half_count;
        int forward = start + steps;
        int backward = start + signed_count + steps;
        do {
            if (wall_flags[forward % signed_count] == 0) {
                selected = (short)(forward % signed_count);
                break;
            }
            forward++;
            if (wall_flags[backward % signed_count] == 0) {
                selected = (short)(backward % signed_count);
                break;
            }
            backward--;
            checked++;
        } while (checked < steps);
    }
    return (float)selected * (wallAngleStep / count_as_float);
}

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

int Check_Straight(const Point4 *start, const Point4 *end, short steps,
                   unsigned long *destination, float radius, int map_index,
                   int excluded_actors, int territory)
{
    Point4 current;
    Point4 previous;
    Point4 intersection;
    short territory_index = (short)territory;
    short step;
    __builtin_memcpy(&previous, start, sizeof(previous));

    if (Check_Undu(start, end, map_index, &intersection, 1, 0, 0x10) == 0) {
        __builtin_memcpy(destination, &intersection, sizeof(intersection));
        return 0;
    }
    step = 0;
    if (steps >= 0) {
        float step_count = (float)steps;
        do {
            current.x = start->x + (end->x - start->x) * (float)step / step_count;
            current.y = start->y + (end->y - start->y) * (float)step / step_count;
            current.z = start->z + (end->z - start->z) * (float)step / step_count;
            if ((territory_index != 0 &&
                 Get_Attr(&current, map_index, 0x10) !=
                    EneTerritoryID[territory_index]) ||
                HitCheckMapUnitPosSize((const Vector4 *)&current, radius) != -1 ||
                Check_Actor((const Vector4 *)&current, (short)excluded_actors) == 1) {
                __builtin_memcpy(destination, &previous, sizeof(previous));
                return (short)(step - 1);
            }
            __builtin_memcpy(&previous, &current, sizeof(current));
            step++;
        } while (step <= steps);
    }
    __builtin_memcpy(destination, end, sizeof(*end));
    return steps;
}

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
                || HitCheckMapUnitPos((const Vector4 *)&current) != -1) {
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

int Check_Angle(float angle, float rangeStart, float rangeEnd)
{
    rangeStart = Get_Decimal_Surplus_for_Radius(rangeStart);
    rangeEnd = Get_Decimal_Surplus_for_Radius(rangeEnd);
    angle = Get_Decimal_Surplus_for_Radius(angle);

    if (rangeStart <= rangeEnd) {
        if (rangeStart <= angle && angle <= rangeEnd) {
            return 1;
        }
        return 0;
    }
    if (rangeStart <= angle || angle <= rangeEnd) {
        return 1;
    }
    return 0;
}

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
    half_width = fan_width_degrees / 180.0f * getPi * 0.5f;
    if (Check_Angle(angle, facing - half_width, facing + half_width) == 0) {
        return 0;
    }
    if (map_index == -1) {
        return 1;
    }
    if (Check_Undu(origin, target, map_index, &intersection, 1, 0, 0x400) == 0) {
        return 0;
    }
    if (fanMinDistance <= Get_Distance(target, &intersection)) {
        return 0;
    }
    if (fanHeightTolerance <= __builtin_fabsf(target->y - intersection.y)) {
        return 0;
    }
    if (fanMaxHeightDelta < __builtin_fabsf(origin->y - target->y)) {
        return 0;
    }
    return CrossPointUwamono(origin, target, crossing_radius, &intersection) != 1;
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
            CrossPointUwamono(&sample, origin, 1.0f, &intersection);
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

float Get_HeightAttr(const Point4 *position, int mapIndex, int attrMask,
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
    return UnduCheck(position, 0, param);
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
    return Get_Distance3D(end, destination) <= unduDistanceTolerance;
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

void GetBSplineLoopDummy(short frame, short count, short period,
                         const Point4 *points, Point4 *destination,
                         const SplineMotionActor *actor)
{
    int actor_number = actor->number;
    int period_index = frame / period;
    unsigned short phase = frame % period;
    EnemySplineState *state = &enepc[actor_number];
    const SplineMotionParameters *parameters = &actor->parameters;
    int index = (unsigned short)(period_index % count);
    int next = index + 1;

    state->segment_steps = (int)(Get_Distance(&points[index], &points[next % count])
                                 / parameters->step_length);
    if (phase >= state->segment_steps - 1) {
        state->frame = state->frame - state->frame % 128 + 128;
    } else {
        state->frame++;
    }
    state->frame %= period * count;
    {
    short segment_steps = state->segment_steps;
    if (segment_steps == 0) {
        destination->x = points[index].x;
        destination->y = points[index].y;
        destination->z = points[index].z;
    } else {
        int following = next % count;
        float phase_value = phase;
        float step_count = segment_steps;
        {
            float coordinate = points[index].x;
            coordinate += (points[following].x - coordinate) * phase_value / step_count;
            destination->x = coordinate;
        }
        {
            float coordinate = points[index].y;
            coordinate += (points[following].y - coordinate) * phase_value / step_count;
            destination->y = coordinate;
            coordinate = points[index].z;
            coordinate += (points[following].z - coordinate) * phase_value / step_count;
            destination->z = coordinate;
        }
    }
    }
    destination->w = 1.0f;
}

const float wallFullTurnRadians = 6.2831854820251465f;

const float wallAngleStep = 6.2831854820251465f;

const float getPi = 3.1415927410125732f;

const float fanMinDistance = 0.0099999997764825821f;

const float fanHeightTolerance = 0.10000000149011612f;

const float fanMaxHeightDelta = 3.2000000476837158f;

const float unduDistanceTolerance = 0.05000000074505806f;

/* JP: ××
 * EN: XX */
const char moveSquarePlaceholderText[8] = "\241\337\241\337";

/* JP: 予想
 * EN: Forecast */
const char moveSquareForecastText[8] = "\315\275\301\333";

unsigned short * DataSpline = 0;
