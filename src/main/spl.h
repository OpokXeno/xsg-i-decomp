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

/* The first twenty bytes of a camera spline, before its coefficient tables. */
typedef struct SplineState {
    unsigned char unmodeled_00[4];
    unsigned short weight_mode;
    unsigned short first_key;
    unsigned short last_key;
    unsigned short sample_count;
    unsigned int component_count;
    const float *samples;
} SplineState;

static void setWeightLen(SplinePoint *points, int count);
static void setWeightLen2(SplinePoint *points, int count);
static void setWeightTime(SplinePoint *points, int count, float duration);
static void setWeightIndex(SplinePoint *points, int count);
static void SPL_cardinalInit(SplineState *spline);

extern void SPL_getValueXYZ(float *destination, void *spline, float frame);

#endif /* SRC_MAIN_SPL_H */
