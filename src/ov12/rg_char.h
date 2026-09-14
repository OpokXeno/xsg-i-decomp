/*
 * TU-local declarations of ov12/tu002 (src/ov12/rg_char.c).
 */

#ifndef SRC_OV12_RG_CHAR_H
#define SRC_OV12_RG_CHAR_H

#include "shared.h"

void RgCalcLocalForChar(RgMatrix dest_matrix, RgVector position,
                        RgVector facing, RgVector up_ref);

extern void XrgNegateVector(RgVector destination, RgVector source);

extern void XrgCalcMatrixYtoZ(RgMatrix destination, RgVector first,
                              RgVector second);

extern void XrgCopyVectorXYZ(RgVector destination, RgVector source);

#endif /* SRC_OV12_RG_CHAR_H */
