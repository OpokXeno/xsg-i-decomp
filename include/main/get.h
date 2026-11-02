#ifndef INCLUDE_MAIN_GET_H
#define INCLUDE_MAIN_GET_H

#include "shared.h"

typedef struct Vector3 { float x, y, z; } Vector3;

int Get_Rnd(int min, int max);
int Check_Angle(float angle, float rangeStart, float rangeEnd);

float Get_Angle_Relative(const Vector4 *first, const Vector4 *second,
                         float reference);

/* Functions and data used by other translation units. */
extern unsigned short *DataSpline;
void Get_Point_By_AngleLength(const Vector4 *source, Vector4 *destination,
                              float angle, float length);
float Get_Distance(const Vector4 *first, const Vector4 *second);
int Check_CrossingOver(const Vector4 *first_start, const Vector4 *first_end,
                       const Vector4 *second_start, const Vector4 *second_end);
/* canon: config/header-canon.json chose src/math/main/review11-002d8058/Get_Angle.c over 1 other accepted spelling */
float Get_Angle(const Vector4 *first, const Vector4 *second);
int Get_Attr(const Vector4 *position, int mapIndex, int attrMask);
int Check_Undu(const Vector4 *start, const Vector4 *end, int map_index,
                Vector4 *destination, int query_mode, int excluded_units,
                int flags);
void Get_Height(const Vector4 *position, int mapIndex, int attrMask);
void Get_EnemyIDPos(const Vector4 *origin, Vector4 *destination);
int Check_InsideFan_Wooo(const Vector4 *origin, const Vector4 *target,
                         float facing, float radius, float fanWidthDegrees);
float Get_Cursol_by_Reduce_Speed_Angle_Loop(float current, float target,
                                            float speed);
void Check_Wall(float distance_scale, float wall_parameter, const float *position, signed char *blocked, int count_argument, int map_index, u8 actor_number, int territory);
int Check_Straight(const Vector4 *start, const Vector4 *end, short steps,
                   unsigned long *destination, float radius, int map_index,
                   int excluded_actors, int territory);
void BSpline_Add(short frame, short count, short period,
                 Vector4 *destination, const Vector3 *source, short offset);
void Get_One_Step(float length, const Vector4 *source,
                  const Vector4 *destination, Vector4 *output);
float Get_Decimal_Surplus_for_Radius(float angle);
float Get_Distance3D(const Vector4 *first, const Vector4 *second);
void Get_MiddlePoint(const float *start, const float *following,
                     short point_index, short point_count, float *result);
float Get_Multi_Max_Under(float value, float step, float maximum);
int Get_Attr_NU(const Vector4 *position, int mapIndex, int attrMask);

#endif /* INCLUDE_MAIN_GET_H */
