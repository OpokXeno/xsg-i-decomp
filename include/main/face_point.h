#ifndef INCLUDE_MAIN_FACE_POINT_H
#define INCLUDE_MAIN_FACE_POINT_H

#include "shared.h"

typedef int s32;

s32 nmlModelCalcClip(const Vector4 *model);

/* Functions and data used by other translation units. */
void xglCullingIgnore(void);

#endif /* INCLUDE_MAIN_FACE_POINT_H */
