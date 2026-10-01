#ifndef INCLUDE_OV12_XRG_RAND_INT_H
#define INCLUDE_OV12_XRG_RAND_INT_H

#include "shared.h"

extern void XrgCopyMatrix(RgMatrix destination, const RgMatrix source);

extern void XrgCopyVector(RgVector destination, RgVector source);

void XrgSetVectorXYZ(RgVector destination, float x, float y, float z);

void XrgClearVector(RgVector destination);

void XrgSubVector(RgVector destination, RgVector first, RgVector second);

void XrgCalcMatrixXtoZ(RgMatrix matrix, RgVector xAxis, RgVector zAxis);

#endif /* INCLUDE_OV12_XRG_RAND_INT_H */
