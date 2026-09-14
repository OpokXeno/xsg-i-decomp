/*
 * TU-local declarations of main/tu258 (src/main/spl.c).
 */

#ifndef SRC_MAIN_SPL_H
#define SRC_MAIN_SPL_H

typedef struct SplinePoint {
    float weight;
    float x;
    float y;
    float z;
} SplinePoint;

extern void SPL_getValueXYZ(float *destination, void *spline, float frame);

#endif /* SRC_MAIN_SPL_H */
