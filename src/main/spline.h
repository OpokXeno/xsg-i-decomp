/* Partial layouts used by the Java spline entry points in main/tu234. */
#ifndef SRC_MAIN_SPLINE_H
#define SRC_MAIN_SPLINE_H

#include "shared.h"
#include "main/jni.h"

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

#endif /* SRC_MAIN_SPLINE_H */
