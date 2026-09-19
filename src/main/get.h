/*
 * TU-local declarations of main/tu201 (src/main/get.c).
 */

#ifndef SRC_MAIN_GET_H
#define SRC_MAIN_GET_H

#include "shared.h"

typedef struct Vector3 {
    float x;
    float y;
    float z;
} Vector3;

typedef struct Point4 {
    float x;
    float y;
    float z;
    float w;
} Point4;

typedef struct {
    float x;
    float y;
    float z;
    float w;
} ParabolaVec;

void Get_Point_By_AngleLength(const Vector4 *source, Vector4 *destination,
                              float angle, float length);

extern float cosf(float angle);

float Get_Angle_Relative(const Point4 *first, const Point4 *second,
                         float reference);

/* MARK: negative-pi comparison bound (provisional name). */
extern volatile const float k_negative_pi;

/* Role name: full-circle addend applied when below negative pi. */
extern volatile const float kFullCircleAdd;

/* MARK: positive-pi comparison bound (provisional name). */
extern volatile const float k_positive_pi;

/* Role name: full-circle subtrahend applied when above positive pi.
   MARK: deliberately NON-volatile like the prior k_full_circle_again;
   making it volatile adds a reload (100B vs 96B). Qualifier asymmetry
   is load-bearing, not cosmetic. */
extern const float kFullCircleSub;

void BSpline_Add(short frame, short count, short period,
                 Vector4 *destination, const Vector3 *source, short offset);

void Get_One_Step(float length, const Point4 *source,
                  const Point4 *destination, Point4 *output);

extern float Get_Distance(const Point4 *first, const Point4 *second);

float Get_Distance3D(const Point4 *first, const Point4 *second);

extern float atan2f(float y, float x);

/* canon: config/header-canon.json chose src/math/main/review11-002d8058/Get_Angle.c over 1 other accepted spelling */
float Get_Angle(const Point4 *first, const Point4 *second);

float Get_Decimal_Surplus_for_Radius(float angle);

extern volatile const float radius_neg_pi;

extern const float radius_two_pi_a;

extern volatile const float radius_two_pi_b;

extern volatile const float radius_pi;

void Get_MiddlePoint_Parabora(const ParabolaVec *first, const ParabolaVec *second,
                              int current, int total, float height,
                              ParabolaVec *destination);

float Get_Cursol_by_Reduce_Speed_Angle_Loop(float current, float target,
                                            float speed);

extern volatile const float kAbsBoundPi;

/* MARK: |delta| bound (pi) */
extern volatile const float kNegPi;

/* MARK: negative-pi comparison bound */
extern volatile const float kTwoPiAddend;

/* Role name for TwoPiA: added to a
   below-pi delta, i.e. cursor = current + (delta + 2pi) / speed. */
extern volatile const float kTwoPiSubtrahend;

/* Role name for TwoPiB:
   delta is subtracted from it, i.e. cursor = current - (2pi - delta)/speed. */
extern volatile const float kNegPiLimit;

/* MARK: low wrap clamp (neg pi) */
extern volatile const float kTwoPiWrapLo;

/* Role name for TwoPiC: added to
   wrap the cursor up when it sits at/below the low clamp. */
extern volatile const float kPiLimit;

/* MARK: high wrap clamp (pos pi) */
extern volatile const float kTwoPiWrapHi;

extern void Get_MiddlePoint(const float *start, const float *following,
                            short point_index, short point_count,
                            float *result);

float Get_Multi_Max_Under(float value, float step, float maximum);

int Get_Rnd(int min, int max);

extern const float D_004D81C8;

int Check_InsideFan_Wooo(const Point4 *origin, const Point4 *target,
                         float facing, float radius, float fanWidthDegrees);

#endif /* SRC_MAIN_GET_H */
