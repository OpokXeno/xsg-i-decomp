/*
 * TU-local declarations of ov12/tu088 (src/ov12/xrg_rand_int.c).
 */

#ifndef SRC_OV12_XRG_RAND_INT_H
#define SRC_OV12_XRG_RAND_INT_H

#include "shared.h"

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

void XrgScaleVector(RgVector destination, RgVector source, float scale);
void XrgScaleVectorXYZ(RgVector destination, RgVector source, float scale);

void XrgCopyVectorXYZ(RgVector destination, RgVector source);
void XrgLinearIntpVector(RgVector destination, RgVector first,
                         RgVector second, float weight);
void XrgCopyVector(RgVector destination, RgVector source);

void XrgPushMatStack(void);
void XrgSetTopMatStack(const RgMatrix source);
void XrgPopMatStack(void);
void XrgMulMatStack(RgMatrix matrix);
void XrgRotMatStack(float angle_x, float angle_y, float angle_z);
float *XrgTopMatStack(void);
void XrgCopyMatStack(RgMatrix destination);
void XrgCalcPlane(RgVector plane, RgVector p0, RgVector p1, RgVector p2);

#endif /* SRC_OV12_XRG_RAND_INT_H */
