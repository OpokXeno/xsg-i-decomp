#ifndef INCLUDE_MAIN_SPLINE_H
#define INCLUDE_MAIN_SPLINE_H

#include "shared.h"

typedef struct Spline {
    void *class_pointer;
    s16 order;
    u16 weight_mode;
    s16 duration;
    u16 point_count;
    u32 component_count;
    float *control_points;
} Spline;

#endif /* INCLUDE_MAIN_SPLINE_H */
