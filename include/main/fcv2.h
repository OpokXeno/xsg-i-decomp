#ifndef INCLUDE_MAIN_FCV2_H
#define INCLUDE_MAIN_FCV2_H

#include "shared.h"

/*
 * FCV2Value is the curve FCV2_getValue samples: a cursor into its key table,
 * the same two-bit type code FCV_getFKeyType returns (1 = constant sample,
 * 3 = sampled spline), and either the constant itself or the base of its key
 * table, aliased in the same word (FCV2_getValue, VA 0x0030d740).
 * FCV2_getVal (VA 0x0030d428, still ASM) compares and subtracts a float
 * argument in $f12; FCV2_getValue never writes $f12 before its tail call, so
 * `frame` is its own parameter, forwarded unchanged.
 */
typedef struct FCV2Value {
    u16 cursor;
    u16 type;
    f32 value;
} FCV2Value;

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

/* FCVPack is defined in src/main/fcv2.c; callers only pass it by pointer. */
typedef struct FCVPack FCVPack;

/* Functions and data used by other translation units. */
void FCV2_setStep(f32 step);
float FCV_getPackValue(FCVPack *pack, float frame);
float FCV2_getPackValue(FCV2Pack *pack, float frame);

#endif /* INCLUDE_MAIN_FCV2_H */
