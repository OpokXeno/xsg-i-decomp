/* Partial layouts used by the Java spline entry points in main/tu234. */
#ifndef SRC_MAIN_SPLINE_H
#define SRC_MAIN_SPLINE_H

#include "shared.h"

typedef struct SplineClassEntry {
    u8 unmodeled_00[0x18];
    void *class_pointer;
} SplineClassEntry;

typedef struct Spline {
    void *class_pointer;
    s16 order;
    u16 weight_mode;
    s16 duration;
    u16 point_count;
    u32 component_count;
    float *control_points;
} Spline;

extern void *xmalloc(int size, int type);

extern SplineClassEntry *classJava_xeno_util_Spline;

#endif /* SRC_MAIN_SPLINE_H */
