#ifndef INCLUDE_MAIN_OUTER_PRODUCT_H
#define INCLUDE_MAIN_OUTER_PRODUCT_H

#include "shared.h"

int CheckPointLine(const Vector4 *first, const Vector4 *second,
                   const Vector4 *point);

/* Functions and data used by other translation units. */
float CheckDist3D(const Vector4 *first, const Vector4 *second);
float CheckDist2D(const Vector4 *first, const Vector4 *second);

#endif /* INCLUDE_MAIN_OUTER_PRODUCT_H */
