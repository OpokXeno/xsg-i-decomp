#ifndef INCLUDE_OV12_XRG_RAND_INT_H
#define INCLUDE_OV12_XRG_RAND_INT_H

#include "shared.h"

/* Angles are radians; the literals keep the original float spelling. */
#define XRG_PI 3.1415927f
#define XRG_TWO_PI 6.2831855f
#define XRG_HALF_PI 1.5707964f
#define XRG_QUARTER_PI 0.7853982f
#define XRG_THREE_QUARTER_PI 2.3561945f
#define XRG_EIGHTH_PI 0.3926991f
#define XRG_THREE_EIGHTH_PI 1.1780972f
#define XRG_FIVE_EIGHTH_PI 1.9634955f
#define XRG_SEVEN_EIGHTH_PI 2.7488937f

/* XrgRand scales rand() by 1 / 2^31 into [0, 1). */
#define XRG_RAND_UNIT 4.656613e-10f

/* Floats in one RgMatrix, and in one RgVector row of it. */
#define XRG_MATRIX_FLOATS 16
#define XRG_VECTOR_FLOATS 4

/* Functions and data used by other translation units. */
int XrgRandInt(void);
float XrgRand(float lower, float upper);
int XrgRandIntRange(int lower, int upper);

float *XrgVectorZero(void);
float *XrgVectorX(void);
float *XrgVectorY(void);
float *XrgVectorZ(void);

void XrgUnitVector(RgVector vector);
void XrgSetVector(RgVector destination, float x, float y, float z, float w);
void XrgSetIVector(int destination[4], int x, int y, int z, int w);
void XrgSetIVectorXYZ(int destination[4], int x, int y, int z);
void XrgSetVectorXYZ(RgVector destination, float x, float y, float z);
void XrgClearVector(RgVector destination);
void XrgAddVector(RgVector destination, RgVector first, RgVector second);
void XrgAddVectorXYZ(RgVector destination, RgVector first, RgVector second);
void XrgSubVector(RgVector destination, RgVector first, RgVector second);
void XrgSubVectorXYZ(RgVector destination, RgVector first, RgVector second);
void XrgScaleVector(RgVector destination, RgVector source, float scale);
void XrgScaleVectorXYZ(RgVector destination, RgVector source, float scale);
void XrgCopyVectorXYZ(RgVector destination, RgVector source);
void XrgCopyVector(RgVector destination, RgVector source);
void XrgLinearIntpVector(RgVector destination, RgVector first,
                         RgVector second, float weight);
void XrgNegateVector(RgVector destination, RgVector source);
float XrgLengthVector(RgVector source);
float XrgNormalizeVector(RgVector destination, RgVector source);
void XrgOuterVector(RgVector destination, RgVector first, RgVector second);
float XrgInnerVector(RgVector first, RgVector second);
float XrgInnerVectorXYZW(RgVector first, RgVector second);
void XrgRandVector(RgVector destination);
void XrgRandVectorRange(RgVector destination, float pitchRange, float yawRange);
void XrgApplyVector(RgVector destination, RgMatrix matrix, RgVector vector);
void XrgRotVector(RgVector destination, RgMatrix matrix, RgVector vector);

void clearRgMatrix(RgMatrix matrix);
void XrgCopyMatrix(RgMatrix destination, const RgMatrix source);
void XrgUnitMatrix(RgMatrix destination);
void XrgMulMatrix(RgMatrix destination, RgMatrix left, RgMatrix right);
void XrgTransMatrix(RgMatrix destination, RgMatrix source,
                    RgVector translation);
void XrgRotMatrixX(RgMatrix destination, RgMatrix source, float angle);
void XrgRotMatrixY(RgMatrix destination, RgMatrix source, float angle);
void XrgRotMatrixZ(RgMatrix destination, RgMatrix source, float angle);
void XrgInvMatrix(RgMatrix destination, const RgMatrix source);
void XrgCalcMatrixYtoZ(RgMatrix matrix, RgVector zAxis, RgVector yAxis);
void XrgCalcMatrixZtoY(RgMatrix matrix, RgVector zAxis, RgVector yAxis);
void XrgCalcMatrixXtoY(RgMatrix matrix, RgVector xAxis, RgVector yAxis);
void XrgCalcMatrixXtoZ(RgMatrix matrix, RgVector xAxis, RgVector zAxis);
float XrgCalcRotY(RgVector vector);

void XrgClearMatStack(void);
void XrgPushMatStack(void);
void XrgSetTopMatStack(const RgMatrix source);
void XrgPopMatStack(void);
void XrgMulMatStack(RgMatrix matrix);
void XrgTransMatStack(float x, float y, float z);
void XrgRotMatStack(float angle_x, float angle_y, float angle_z);
float *XrgTopMatStack(void);
void XrgCopyMatStack(RgMatrix destination);

void XrgCalcPlaneNorm(RgVector normal, RgVector p0, RgVector p1, RgVector p2);
void XrgCalcPlane(RgVector plane, RgVector p0, RgVector p1, RgVector p2);
int XrgIsTriIntersectedByRay(RgVector origin, RgVector direction,
                             RgVector triangle[3], float *distance);
int XrgQuantAngle(float angle);
int XrgQuantAngle4(float angle);

#endif /* INCLUDE_OV12_XRG_RAND_INT_H */
