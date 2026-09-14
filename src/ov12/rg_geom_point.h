/*
 * TU-local declarations of ov12/tu052 (src/ov12/rg_geom_point.c).
 */

#ifndef SRC_OV12_RG_GEOM_POINT_H
#define SRC_OV12_RG_GEOM_POINT_H

#include "shared.h"

/*
 * The geometry helpers use four adjacent single-precision slots for a
 * position.  The historical public vector typedef is not established here.
 */
typedef struct RgPointVector {
    float x;
    float y;
    float z;
    float w;
} RgPointVector;

void __RgGeomPointGetPos(RgGeomPoint *point, RgPointVector *destination,
                         const char *source_file, int source_line);

void __RgGeomPointGetOldPos(RgGeomPoint *point, RgPointVector *destination,
                            const char *source_file, int source_line);

/* Linker witness for the original literal at ov12:0x00a550f0. */
extern const char rg_point_assert_expression[];

#endif /* SRC_OV12_RG_GEOM_POINT_H */
