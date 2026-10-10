/*
 * TU-local declarations of main/tu259 (src/main/fcv2.c).
 */

#ifndef SRC_MAIN_FCV2_H
#define SRC_MAIN_FCV2_H

typedef signed short s16;
typedef int s32;
typedef float f32;

/* FCV2 pack state consumed by the packed-curve readers and initializer. */
typedef struct FCV2Pack {
    void *cursor;
    float frame_scale;
    float frame_offset;
    unsigned int attribute_hash;
    unsigned short attribute_id;
    void *curve_attribute;
    void *attribute_flagged;
    void *attribute_unflagged;
    float step;
} FCV2Pack;

float FCV2_getPackValue(FCV2Pack *pack, float frame);

#endif /* SRC_MAIN_FCV2_H */
