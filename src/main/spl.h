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

enum {
    SPL_LINEAR_TERM,
    SPL_QUADRATIC_TERM,
    SPL_CUBIC_TERM
};

/* The first twenty bytes of a camera spline, before its coefficient tables. */
typedef struct SplineState {
    unsigned char unmodeled_00[4];
    unsigned short weight_mode;
    unsigned short first_key;
    unsigned short last_key;
    unsigned short sample_count;
    unsigned int component_count;
    const float *samples;
    float coefficients[3][96];
} SplineState;

/* Five 128-byte float tables at the start of EE scratchpad memory. */
struct SplineWorkspace {
    float interval[32];
    float slope[32];
    float diagonal[32];
    float right_hand_side[32];
    float second_derivative[32];
};

extern float minCardinalInterval;

static void setWeightLen(SplinePoint *points, int count);
static void setWeightLen2(SplinePoint *points, int count);
static void setWeightTime(SplinePoint *points, int count, float duration);
static void setWeightIndex(SplinePoint *points, int count);
static void SPL_cardinalInit(SplineState *spline);
extern void SPL_getValueXYZ(float *destination, void *spline, float frame);

#endif /* SRC_MAIN_SPL_H */
