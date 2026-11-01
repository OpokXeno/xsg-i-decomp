#include "common.h"
#include "shared.h"
#include "fcv2.h"

#define FCV2_CURSOR_COUNT_SCALE (1.5259021893143654e-05f)
#define FCV2_PACK_OFFSET_FRAME_TIME (0.033333335071802139f)
#define FCV2_RANGE_COUNT_SCALE (1.5259021893143654e-05f)
#define FCV2_PACK_SCALE_FRAME_TIME (0.033333335071802139f)
#define FCV2_KEY_FRAME_SCALE (0.033333335071802139f)
#define FCV2_KEY_FRAME_TIME (0.033333335071802139f)
#define FCV2_SEGMENT_FRAME_TIME (0.033333335071802139f)

/*
 * FCVKeyData is the per-key-set source record FCV_resetPack reads: a packed
 * 2-bit key-type table (4 keys per byte, FCV_getFKeyType) at +0x10, 16 key
 * frames at +0x14 and the key-value stream from +0x34. FCV_getPackValue
 * (still ASM in this TU) indexes the type table by the pack's key_index and
 * walks the frame and value streams through the pack's advancing pointers.
 * key_values is declared with one element only to name where the value
 * stream starts; its length is not evidenced here.
 */
typedef struct FCVKeyData {
    unsigned char unmodeled_00[0x10];
    u8 key_types[4];
    u16 key_frames[16];
    f32 key_values[1];
} FCVKeyData;

/*
 * FCVPack is the destination FCV_resetPack initializes and the state
 * FCV_getPackValue/FCV_getPackVal2f/FCV_getPackVal2s/FCV_skipPack (still ASM
 * in this TU) consume: pointers into a FCVKeyData's three streams, plus a
 * key cursor. FCV_getPackValue passes +4 to FCV_getFKeyType as the packed
 * key-type table, reads a halfword through +8 advancing it by 2, reads a
 * float through +0xC advancing it by 4, and reads/writes +0x10 as the key
 * index; the leading word it never touches stays unmodeled.
 */
typedef struct FCVPack {
    unsigned char unmodeled_00[4];
    u8 *key_types;
    u16 *key_frames;
    f32 *key_values;
    s16 key_index;
} FCVPack;

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

typedef struct FCV2DataHeader {
    u32 signature;
} FCV2DataHeader;

typedef struct FCVKeyFrame {
    f32 frame;
    f32 in_tangent;
    f32 out_tangent;
    f32 value;
} FCVKeyFrame;

typedef struct FCVCurve {
    u32 interpolation_modes;
    FCVKeyFrame *keys;
    u16 key_count;
    u16 unmodeled_0A;
} FCVCurve;

typedef struct FCV2SampleValues {
    f32 value;
    f32 in_tangent;
    f32 out_tangent;
} FCV2SampleValues;

/* Compact spline keys extend their type/frame prefix with value and tangents. */
typedef struct FCV2Sample {
    u16 type;
    u16 frame;
    FCV2SampleValues values;
} FCV2Sample;

/* Float-frame keys use the same variable-length value/tangent payload. */
typedef struct FCVFloatSample {
    u32 type;
    f32 frame;
    FCV2SampleValues values;
} FCVFloatSample;

typedef struct FCV2AttributeRecord {
    u16 size;
    u16 type;
    u32 payload_words[2];
} FCV2AttributeRecord;

typedef struct FCV2AttributeStream {
    u32 byte_size;
    FCV2AttributeRecord records[1];
} FCV2AttributeStream;

typedef struct FCV2CurveData {
    FCV2DataHeader common;
    u16 format;
    u16 frame_count;
    u16 first_frame;
    u16 last_frame;
    s16 frame_offset;
    FCV2AttributeStream attributes;
} FCV2CurveData;

typedef struct FCV2Attribute {
    u16 size;
    u16 type;
    u8 payload[12];
} FCV2Attribute;

typedef struct FCV2KeyHeader {
    u16 key_count;
    u16 type_flags;
    f32 value_or_spline_data;
} FCV2KeyHeader;

static f32 FCV_getPackVal2f(FCVPack *pack, s32 keyCount, f32 frame);

static f32 FCV2_getVal(void *output, void *data, s32 key, f32 frame);

static f32 FCV_getPackVal2s(FCVPack *pack, u32 keyCount, f32 frame);
/*
 * types is a packed array of 2-bit interpolation-type codes, four keys per
 * byte, stored in reverse bit order within each byte (FCV_getFKeyType,
 * VA 0x0030d060).
 */
static s32 FCV_getFKeyType(u32 keyIndex, u8 *types);
static f32 FCV2_getPackVal(FCV2Pack *pack, void *key_data, s32 key_count, f32 frame);

static f32 fcv2Step = 0.033333335f;

static FCV2Attribute defaultAttr;

static FCV2Attribute defaultAttr = { .size = 0, .type = 0, .payload = { 255, 255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } };

f32 FCV_getValue(FCVCurve *curve, f32 frame)
{
    FCVKeyFrame *key;
    f32 first_frame = curve->keys[0].frame;
    f32 last_frame;
    f32 period;
    f32 previous_frame;
    f32 previous_value;
    f32 previous_out_tangent;
    f32 next_frame;
    f32 next_value;
    f32 next_in_tangent;

    if (frame < first_frame) {
        switch ((curve->interpolation_modes >> 8) & 0xF) {
        case 0:
            return curve->keys[0].value;
        case 1: {
            FCVKeyFrame *last_key =
                &curve->keys[curve->key_count];
            period = last_key[-1].frame - first_frame;
            while (frame < first_frame) {
                frame += period;
            }
            break;
        }
        }
    }

    key = &curve->keys[curve->key_count - 1];
    last_frame = key->frame;
    if (last_frame <= frame) {
        switch ((curve->interpolation_modes >> 4) & 0xF) {
        case 0:
            return key->value;
        case 1:
            period = last_frame - curve->keys[0].frame;
            while (last_frame < frame) {
                frame -= period;
            }
            key = &curve->keys[1];
            break;
        default:
            return 0.0f;
        }
    } else {
        key = &curve->keys[1];
    }

    while (key->frame <= frame) {
        if (frame == key->frame) {
            return key->value;
        }
        key++;
    }

    next_frame = key->frame;
    next_value = key->value;
    next_in_tangent = key->in_tangent;
    key--;
    previous_frame = key->frame;
    previous_value = key->value;
    previous_out_tangent = key->out_tangent;

    switch (curve->interpolation_modes & 0xF) {
    case 1: {
        f32 value_difference = next_value - previous_value;
        f32 frame_offset = frame - previous_frame;
        f32 frame_span = next_frame - previous_frame;

        return previous_value + value_difference * frame_offset / frame_span;
    }
    case 0:
        return previous_value;
    case 2: {
        f32 frame_offset = frame - previous_frame;
        f32 inverse_span = 1.0f / (next_frame - previous_frame);
        f32 offset_squared = frame_offset * frame_offset;
        f32 offset_cubed = offset_squared * frame_offset;
        f32 three_times_offset_squared = offset_squared * 3.0f;
        f32 inverse_span_squared = inverse_span * inverse_span;
        f32 offset_squared_over_span = offset_squared * inverse_span;
        f32 offset_cubed_over_span_squared = offset_cubed * inverse_span_squared;
        f32 three_offset_squared_over_span_squared =
            three_times_offset_squared * inverse_span_squared;
        f32 twice_offset_cubed_over_span_cubed =
            (offset_cubed_over_span_squared + offset_cubed_over_span_squared) *
            inverse_span;
        f32 next_tangent_weight =
            offset_cubed_over_span_squared - offset_squared_over_span;
        f32 previous_tangent_weight =
            (next_tangent_weight - offset_squared_over_span) + frame_offset;
        f32 previous_value_weight =
            (twice_offset_cubed_over_span_cubed -
             three_offset_squared_over_span_squared) + 1.0f;
        f32 next_value_weight =
            three_offset_squared_over_span_squared -
            twice_offset_cubed_over_span_cubed;

        return previous_value * previous_value_weight +
               next_value * next_value_weight +
               previous_out_tangent * previous_tangent_weight +
               next_in_tangent * next_tangent_weight;
    }
    default:
        return 0.0f;
    }
}

void FCV_resetPack(FCVPack *pack, FCVKeyData *source)
{
    pack->key_types = source->key_types;
    pack->key_frames = source->key_frames;
    pack->key_values = source->key_values;
    pack->key_index = 0;
}

void FCV_skipPack(FCVPack *pack, s32 keyCount)
{
    FCV_getPackVal2f(pack, keyCount, 0.0f);
}

f32 FCV_getPackValue(FCVPack *pack, f32 frame)
{
    u32 keyType = (u32)FCV_getFKeyType((u32)pack->key_index, pack->key_types);
    f32 value;

    switch (keyType) {
    case 1:
        value = *pack->key_values++;
        break;
    case 0:
        value = 0.0f;
        break;
    case 3: {
        u16 keyCount = *pack->key_frames++;

        value = FCV_getPackVal2s(pack, keyCount, frame);
        break;
    }
    default:
        value = 0.0f;
        break;
    }

    pack->key_index = (s16)((u16)pack->key_index + 1);
    return value;
}

static f32 FCV_getPackVal2f(FCVPack *pack, s32 key_count, f32 frame)
{
    FCVFloatSample *previous = (FCVFloatSample *)pack->key_values;
    FCVFloatSample *next;
    FCVFloatSample *last;
    FCVFloatSample *end;
    FCV2SampleValues *values;
    u32 previous_type = previous->type;
    u32 index;
    f32 boundary_frame;
    f32 previous_frame;
    f32 next_frame;
    f32 previous_value;
    f32 next_value;
    f32 previous_out_tangent;
    f32 next_in_tangent;

    switch (previous_type) {
    case 0:
    case 2:
    case 3:
        next = (FCVFloatSample *)((f32 *)previous + 3);
        break;
    case 4:
        next = (FCVFloatSample *)((f32 *)previous + 4);
        break;
    case 1:
        next = previous;
        break;
    default:
        next = previous;
        break;
    }

    last = previous;
    end = next;
    for (index = 1; index < key_count; index++) {
        last = end;
        switch (last->type) {
        case 0:
        case 2:
        case 3:
            end = (FCVFloatSample *)((f32 *)end + 3);
            break;
        case 4:
            end = (FCVFloatSample *)((f32 *)end + 4);
            break;
        case 1:
            end = (FCVFloatSample *)((f32 *)last + 5);
            break;
        default:
            break;
        }
    }

    boundary_frame = previous->frame;
    pack->key_values = (f32 *)end;
    if (frame < boundary_frame) {
        return previous->values.value;
    }
    boundary_frame = last->frame;
    if (boundary_frame <= frame) {
        return last->values.value;
    }

    next_frame = next->frame;
    for (index = 1; index < key_count; index++) {
        if (frame <= next_frame) {
            if (frame == next_frame) {
                return next->values.value;
            }
            break;
        }

        previous = next;
        previous_type = previous->type;
        switch (previous_type) {
        case 0:
        case 2:
            next = (FCVFloatSample *)((f32 *)next + 3);
            next_frame = next->frame;
            break;
        case 1:
            next = (FCVFloatSample *)((f32 *)previous + 5);
            next_frame = next->frame;
            break;
        case 4:
            next = (FCVFloatSample *)((f32 *)previous + 4);
            next_frame = next->frame;
            break;
        default:
            break;
        }
    }

    previous_frame = previous->frame;
    values = &next->values;
    switch (next->type) {
    case 0:
    case 2:
        next_in_tangent = 0.0f;
        next_value = values->value;
        break;
    case 1:
    case 4:
        next_value = values->value;
        next_in_tangent = values->in_tangent;
        break;
    default:
        next_in_tangent = 0.0f;
        next_value = 0.0f;
        break;
    }

    values = &previous->values;
    switch (previous_type) {
    case 0:
    case 2:
        previous_out_tangent = 0.0f;
        previous_value = values->value;
        break;
    case 4:
        previous_value = values->value;
        previous_out_tangent = values->in_tangent;
        break;
    case 1:
        previous_value = values->value;
        previous_out_tangent = values->out_tangent;
        break;
    default:
        previous_out_tangent = 0.0f;
        previous_value = 0.0f;
        break;
    }

    switch (previous_type) {
    case 0:
        return previous_value +
            ((next_value - previous_value) * (frame - previous_frame)) /
            (next_frame - previous_frame);
    case 1:
    case 2:
    case 4: {
        f32 inverse_span = 1.0f / (next_frame - previous_frame);
        f32 frame_offset = frame - previous_frame;
        f32 offset_squared = frame_offset * frame_offset;
        f32 offset_cubed = offset_squared * frame_offset;
        f32 three_times_offset_squared = offset_squared * 3.0f;
        f32 inverse_span_squared = inverse_span * inverse_span;
        f32 offset_squared_over_span = offset_squared * inverse_span;
        f32 offset_cubed_over_span_squared = offset_cubed * inverse_span_squared;
        f32 three_offset_squared_over_span_squared =
            three_times_offset_squared * inverse_span_squared;
        f32 twice_offset_cubed_over_span_cubed =
            (offset_cubed_over_span_squared + offset_cubed_over_span_squared) *
            inverse_span;
        f32 next_tangent_weight =
            offset_cubed_over_span_squared - offset_squared_over_span;
        f32 previous_tangent_weight =
            (next_tangent_weight - offset_squared_over_span) + frame_offset;
        f32 previous_value_weight =
            (twice_offset_cubed_over_span_cubed -
             three_offset_squared_over_span_squared) + 1.0f;
        f32 next_value_weight =
            three_offset_squared_over_span_squared -
            twice_offset_cubed_over_span_cubed;

        return previous_value * previous_value_weight +
            next_value * next_value_weight +
            previous_out_tangent * previous_tangent_weight +
            next_in_tangent * next_tangent_weight;
    }
    default:
        return 0.0f;
    }
}

static f32 FCV_getPackVal2s(FCVPack *pack, u32 key_count, f32 frame)
{
    FCV2Sample *previous = (FCV2Sample *)pack->key_values;
    FCV2Sample *next = (FCV2Sample *)((u16 *)previous + 8);
    FCV2Sample *last;
    FCV2Sample *end;
    FCV2SampleValues *values;
    u16 previous_type = previous->type;
    s32 next_frame_index;
    u32 index;
    f32 previous_frame;
    f32 next_frame;
    f32 previous_value;
    f32 next_value;
    f32 previous_out_tangent;
    f32 next_in_tangent;
    f32 frame_scale;
    f32 boundary_frame;

    switch (previous_type) {
    case 0:
    case 2:
        next = (FCV2Sample *)((u16 *)previous + 4);
        break;
    case 1:
        break;
    default:
        next = previous;
        break;
    }

    last = previous;
    end = next;
    for (index = 1; index < key_count; index++) {
        last = end;
        switch (last->type) {
        case 0:
        case 2:
            end = (FCV2Sample *)((u16 *)end + 4);
            break;
        case 1:
            end = (FCV2Sample *)((u16 *)last + 8);
            break;
        default:
            break;
        }
    }

    frame_scale = 0.033333335f;
    boundary_frame = (f32)previous->frame * frame_scale;
    pack->key_values = (f32 *)end;
    if (frame < boundary_frame) {
        return previous->values.value;
    }
    boundary_frame = (f32)last->frame * frame_scale;
    if (boundary_frame <= frame) {
        return last->values.value;
    }

    for (index = 1;
         (next_frame_index = next->frame, index < key_count);
         index++) {
        boundary_frame = (f32)next_frame_index * 0.033333335f;

        if (frame <= boundary_frame) {
            if (frame == boundary_frame) {
                return next->values.value;
            }
            break;
        }

        previous = next;
        previous_type = previous->type;
        switch (previous_type) {
        case 0:
        case 2:
            next = (FCV2Sample *)((u16 *)next + 4);
            break;
        case 1:
            next = (FCV2Sample *)((u16 *)previous + 8);
            break;
        default:
            break;
        }
    }

    next_frame = (f32)next->frame * 0.033333335f;
    previous_frame = (f32)previous->frame * 0.033333335f;
    values = &next->values;
    switch (next->type) {
    case 0:
    case 2:
        next_in_tangent = 0.0f;
        next_value = values->value;
        break;
    case 1:
        next_value = values->value;
        next_in_tangent = values->in_tangent;
        break;
    default:
        next_value = 0.0f;
        next_in_tangent = 0.0f;
        break;
    }

    values = &previous->values;
    switch (previous_type) {
    case 0:
    case 2:
        previous_out_tangent = 0.0f;
        previous_value = values->value;
        break;
    case 1:
        previous_value = values->value;
        previous_out_tangent = values->out_tangent;
        break;
    default:
        previous_value = 0.0f;
        previous_out_tangent = 0.0f;
        break;
    }

    switch (previous_type) {
    case 0:
        return previous_value +
            ((next_value - previous_value) * (frame - previous_frame)) /
            (next_frame - previous_frame);
    case 1:
    case 2: {
        f32 inverse_span = 1.0f / (next_frame - previous_frame);
        f32 frame_offset = frame - previous_frame;
        f32 offset_squared = frame_offset * frame_offset;
        f32 offset_cubed = offset_squared * frame_offset;
        f32 three_times_offset_squared = offset_squared * 3.0f;
        f32 inverse_span_squared = inverse_span * inverse_span;
        f32 offset_squared_over_span = offset_squared * inverse_span;
        f32 offset_cubed_over_span_squared = offset_cubed * inverse_span_squared;
        f32 three_offset_squared_over_span_squared =
            three_times_offset_squared * inverse_span_squared;
        f32 twice_offset_cubed_over_span_cubed =
            (offset_cubed_over_span_squared + offset_cubed_over_span_squared) *
            inverse_span;
        f32 next_tangent_weight =
            offset_cubed_over_span_squared - offset_squared_over_span;
        f32 previous_tangent_weight =
            (next_tangent_weight - offset_squared_over_span) + frame_offset;
        f32 previous_value_weight =
            (twice_offset_cubed_over_span_cubed -
             three_offset_squared_over_span_squared) + 1.0f;
        f32 next_value_weight =
            three_offset_squared_over_span_squared -
            twice_offset_cubed_over_span_cubed;

        return previous_value * previous_value_weight +
            next_value * next_value_weight +
            previous_out_tangent * previous_tangent_weight +
            next_in_tangent * next_tangent_weight;
    }
    default:
        return 0.0f;
    }
}

static s32 FCV_getFKeyType(u32 keyIndex, u8 *types)
{
    unsigned char packed = types[keyIndex >> 2];
    return (packed >> ((3 - (keyIndex & 3)) * 2)) & 3;
}

static f32 FCV2_getPackVal(FCV2Pack *pack, void *key_data, s32 key_count, f32 frame)
{
    FCV2Sample *keys = key_data;
    FCV2Sample *previous = keys;
    FCV2Sample *next = previous;
    FCV2Sample *near_previous;
    FCV2Sample *near_next;
    FCV2SampleValues *values;
    u16 previous_type;
    s32 index;
    f32 frame_scale;
    f32 first_frame;
    f32 near_distance;
    f32 previous_frame;
    f32 next_frame;
    f32 previous_value;
    f32 next_value;
    f32 previous_out_tangent;
    f32 next_in_tangent;

    switch (previous->type) {
    case 0:
    case 2:
    case 3:
        next = (FCV2Sample *)((u16 *)keys + 4);
        break;
    case 4:
        next = (FCV2Sample *)((u16 *)keys + 6);
        break;
    case 1:
        next = (FCV2Sample *)((u16 *)keys + 8);
        break;
    default:
        break;
    }

    frame_scale = pack->frame_scale;
    frame += pack->frame_offset;
    first_frame = (f32)keys->frame * frame_scale;
    /* A selected bracket replaces this initial endpoint measure. */
    near_distance = first_frame;
    if (frame <= first_frame) {
        values = &keys->values;
        for (index = 1; index < key_count; index++) {
            switch (next->type) {
            case 0:
            case 2:
            case 3:
                next = (FCV2Sample *)((u16 *)next + 4);
                break;
            case 4:
                next = (FCV2Sample *)((u16 *)next + 6);
                break;
            case 1:
                next = (FCV2Sample *)((u16 *)next + 8);
                break;
            default:
                break;
            }
        }
        pack->cursor = next;
        return values->value;
    }

    near_previous = 0;
    near_next = 0;
    for (index = 1; index < key_count; index++) {
        f32 key_frame = (f32)next->frame * frame_scale;
        f32 distance = frame - key_frame;

        if (near_previous == 0 && distance < pack->step) {
            near_distance = __builtin_fabsf(distance);
            near_previous = previous;
            near_next = next;
        }
        previous = next;
        switch (previous->type) {
        case 0:
        case 2:
        case 3:
            next = (FCV2Sample *)((u16 *)next + 4);
            break;
        case 4:
            next = (FCV2Sample *)((u16 *)next + 6);
            break;
        case 1:
            next = (FCV2Sample *)((u16 *)next + 8);
            break;
        default:
            break;
        }
    }
    pack->cursor = next;
    if (near_previous == 0) {
        return previous->values.value;
    }
    if (near_distance < pack->step * 0.5f) {
        return near_next->values.value;
    }

    previous_frame = frame_scale * (f32)near_previous->frame;
    next_frame = frame_scale * (f32)near_next->frame;
    values = &near_next->values;
    switch (near_next->type) {
    case 2:
    case 0:
        next_in_tangent = 0.0f;
        next_value = values->value;
        break;
    case 1:
    case 4:
        next_value = values->value;
        next_in_tangent = values->in_tangent;
        break;
    default:
        next_in_tangent = 0.0f;
        next_value = 0.0f;
        break;
    }

    previous_type = near_previous->type;
    values = &near_previous->values;
    switch (previous_type) {
    case 0:
    case 2:
    case 3:
        previous_out_tangent = 0.0f;
        previous_value = values->value;
        break;
    case 4:
        previous_value = values->value;
        previous_out_tangent = values->in_tangent;
        break;
    case 1:
        previous_value = values->value;
        previous_out_tangent = values->out_tangent;
        break;
    default:
        previous_out_tangent = 0.0f;
        previous_value = 0.0f;
        break;
    }

    switch (near_previous->type) {
    case 3:
        return previous_value;
    case 0:
        return previous_value +
            ((next_value - previous_value) * (frame - previous_frame)) /
            (next_frame - previous_frame);
    case 1:
    case 2:
    case 4: {
        f32 inverse_span = 1.0f / (next_frame - previous_frame);
        f32 frame_offset = frame - previous_frame;
        f32 offset_squared = frame_offset * frame_offset;
        f32 offset_cubed = offset_squared * frame_offset;
        f32 three_times_offset_squared = offset_squared * 3.0f;
        f32 inverse_span_squared = inverse_span * inverse_span;
        f32 offset_squared_over_span = offset_squared * inverse_span;
        f32 offset_cubed_over_span_squared = offset_cubed * inverse_span_squared;
        f32 three_offset_squared_over_span_squared =
            three_times_offset_squared * inverse_span_squared;
        f32 twice_offset_cubed_over_span_cubed =
            (offset_cubed_over_span_squared + offset_cubed_over_span_squared) *
            inverse_span;
        f32 next_tangent_weight =
            offset_cubed_over_span_squared - offset_squared_over_span;
        f32 previous_tangent_weight =
            (next_tangent_weight - offset_squared_over_span) + frame_offset;
        f32 previous_value_weight =
            (twice_offset_cubed_over_span_cubed -
             three_offset_squared_over_span_squared) + 1.0f;
        f32 next_value_weight =
            three_offset_squared_over_span_squared -
            twice_offset_cubed_over_span_cubed;

        return previous_value * previous_value_weight +
            next_value * next_value_weight +
            previous_out_tangent * previous_tangent_weight +
            next_in_tangent * next_tangent_weight;
    }
    default:
        return 0.0f;
    }
}

static f32 FCV2_getVal(void *output, void *data, s32 key_count, f32 frame)
{
    FCV2Sample **key_output = output;
    FCV2Sample *previous = data;
    FCV2Sample *next = (FCV2Sample *)((u16 *)previous + 8);
    FCV2SampleValues *values;
    u16 previous_type = previous->type;
    s32 index;
    f32 key_frame;
    f32 previous_frame;
    f32 next_frame;
    f32 previous_value;
    f32 next_value;
    f32 previous_out_tangent;
    f32 next_in_tangent;

    switch (previous_type) {
    case 0:
    case 2:
    case 3:
        next = (FCV2Sample *)((u16 *)previous + 4);
        break;
    case 1:
        break;
    default:
        next = previous;
        break;
    }

    if (output != 0) {
        *key_output = 0;
    }
    key_frame = (f32)previous->frame * FCV2_KEY_FRAME_TIME;
    if (frame <= key_frame) {
        if (__builtin_fabsf(frame - key_frame) < FCV2_KEY_FRAME_TIME && output != 0) {
            *key_output = previous;
        }
        return previous->values.value;
    }

    for (index = 1; index < key_count; index++) {
        f32 next_key_frame = (f32)next->frame * 0.033333335f;

        if (frame <= next_key_frame) {
            if (__builtin_fabsf(frame - next_key_frame) < 0.033333335f) {
                if (output != 0) {
                    *key_output = next;
                }
                return next->values.value;
            }
            break;
        }

        previous = next;
        previous_type = previous->type;
        switch (previous_type) {
        case 0:
        case 2:
        case 3:
            next = (FCV2Sample *)((u16 *)next + 4);
            break;
        case 1:
            next = (FCV2Sample *)((u16 *)previous + 8);
            break;
        default:
            break;
        }
    }

    if (index == key_count) {
        return previous->values.value;
    }

    previous_frame = (f32)previous->frame * FCV2_SEGMENT_FRAME_TIME;
    next_frame = (f32)next->frame * FCV2_SEGMENT_FRAME_TIME;
    values = &next->values;
    switch (next->type) {
    case 0:
    case 2:
        next_in_tangent = 0.0f;
        next_value = values->value;
        break;
    case 1:
        next_value = values->value;
        next_in_tangent = values->in_tangent;
        break;
    default:
        next_in_tangent = 0.0f;
        next_value = 0.0f;
        break;
    }

    values = &previous->values;
    switch (previous_type) {
    case 0:
    case 2:
    case 3:
        previous_out_tangent = 0.0f;
        previous_value = values->value;
        break;
    case 1:
        previous_value = values->value;
        previous_out_tangent = values->out_tangent;
        break;
    default:
        previous_out_tangent = 0.0f;
        previous_value = 0.0f;
        break;
    }

    switch (previous_type) {
    case 3:
        return previous_value;
    case 0:
        return previous_value +
            ((next_value - previous_value) * (frame - previous_frame)) /
            (next_frame - previous_frame);
    case 1:
    case 2: {
        f32 inverse_span = 1.0f / (next_frame - previous_frame);
        f32 frame_offset = frame - previous_frame;
        f32 offset_squared = frame_offset * frame_offset;
        f32 offset_cubed = offset_squared * frame_offset;
        f32 three_times_offset_squared = offset_squared * 3.0f;
        f32 inverse_span_squared = inverse_span * inverse_span;
        f32 offset_squared_over_span = offset_squared * inverse_span;
        f32 offset_cubed_over_span_squared = offset_cubed * inverse_span_squared;
        f32 three_offset_squared_over_span_squared =
            three_times_offset_squared * inverse_span_squared;
        f32 twice_offset_cubed_over_span_cubed =
            (offset_cubed_over_span_squared + offset_cubed_over_span_squared) *
            inverse_span;
        f32 next_tangent_weight =
            offset_cubed_over_span_squared - offset_squared_over_span;
        f32 previous_tangent_weight =
            (next_tangent_weight - offset_squared_over_span) + frame_offset;
        f32 previous_value_weight =
            (twice_offset_cubed_over_span_cubed -
             three_offset_squared_over_span_squared) + 1.0f;
        f32 next_value_weight =
            three_offset_squared_over_span_squared -
            twice_offset_cubed_over_span_cubed;

        return previous_value * previous_value_weight +
            next_value * next_value_weight +
            previous_out_tangent * previous_tangent_weight +
            next_in_tangent * next_tangent_weight;
    }
    default:
        return 0.0f;
    }
}

f32 FCV2_getValue(FCV2Value *curve, f32 frame)
{
    if (curve->type == 3) {
        return FCV2_getVal(0, &curve->value, curve->cursor, frame);
    }
    if (curve->type == 1) {
        return curve->value;
    }
    return 0.0f;
}

/*
 * key is zeroed unconditionally, then forwarded to FCV2_getVal as its
 * output parameter only while walking a spline (curve->type == 3);
 * FCV2_getValue's equivalent call passes a null output there instead.
 */
f32 FCV2_getValueAndKey(s32 *key, FCV2Value *curve, f32 frame)
{
    *key = 0;
    if (curve->type == 3) {
        return FCV2_getVal(key, &curve->value, curve->cursor, frame);
    }
    if (curve->type == 1) {
        return curve->value;
    }
    return 0.0f;
}

void FCV2_readAttribute(FCV2Pack *pack, FCV2AttributeStream *attributes)
{
    FCV2AttributeStream *stream = attributes;
    FCV2AttributeRecord *attribute;
    u32 *stream_size = &stream->byte_size;
    u32 bytes_used;

    pack->curve_attribute = 0;
    bytes_used = 4;
    pack->attribute_flagged = 0;
    attribute = stream->records;
    pack->attribute_unflagged = 0;

    for (;;) {
        s32 type = attribute->type;

        switch (type) {
        case 1: {
            u32 *payload = attribute->payload_words;

            pack->attribute_hash = payload[0] * 0x1f + payload[1];
            break;
        }
        case 3:
            pack->curve_attribute = &attribute->payload_words[0];
            break;
        case 2: {
            u32 *payload = attribute->payload_words;
            u8 flags = (u8)payload[0];

            if ((flags & 0x7f) == pack->attribute_id) {
                if ((flags & 0x80) != 0) {
                    pack->attribute_flagged = payload;
                } else {
                    pack->attribute_unflagged = payload;
                }
            }
            break;
        }
        }

        if (attribute->type == 0) {
            return;
        }

        bytes_used += attribute->size;
        attribute = (FCV2AttributeRecord *)((u8 *)attribute + attribute->size);
        if (*stream_size <= bytes_used) {
            return;
        }
    }
}

int FCV2_checkData(void *data)
{
    FCV2DataHeader *header;

    if (data == 0) {
        return 1;
    }

    header = data;
    if (header->signature != 0x00564346) {
        return 2;
    }
    return 0;
}

void FCV2_setStep(f32 step)
{
    fcv2Step = __builtin_fabsf(step) * 0.5f;
}

void FCV2_resetPack(FCV2Pack *pack, FCV2CurveData *curve)
{
    f32 frame_scale;
    f32 frame_offset;
    u16 *frame_cursor;

    if (curve == 0) {
        return;
    }

    if (curve->format >= 2) {
        f32 normalized_count;
        frame_cursor = &curve->frame_count;
        normalized_count = (f32)*frame_cursor * FCV2_CURSOR_COUNT_SCALE;
        frame_offset = (f32)curve->frame_offset * FCV2_PACK_OFFSET_FRAME_TIME;
        frame_scale = normalized_count * FCV2_PACK_OFFSET_FRAME_TIME;
    } else {
        f32 normalized_count = (f32)((s32)curve->last_frame - curve->first_frame) * FCV2_RANGE_COUNT_SCALE;
        frame_scale = normalized_count * FCV2_PACK_SCALE_FRAME_TIME;
        frame_offset = 0.0f;
    }

    pack->attribute_hash = 0;
    if (curve->format >= 2) {
        FCV2AttributeStream *attributes = &curve->attributes;
        u32 stream_size;

        FCV2_readAttribute(pack, attributes);
        stream_size = attributes->byte_size;
        frame_cursor = (u16 *)((u8 *)attributes + stream_size);
    } else {
        frame_cursor = (u16 *)&curve->attributes;
    }
    pack->cursor = (u8 *)frame_cursor;
    pack->frame_scale = frame_scale;
    pack->frame_offset = frame_offset;
    pack->step = fcv2Step;
}

FCV2Attribute *FCV2_getPackAttribute(void **cursor)
{
    FCV2Attribute *attribute = *cursor;
    FCV2Attribute *result;

    if (attribute->type == 4) {
        result = attribute;
        *cursor = &attribute->payload[attribute->size];
    } else {
        result = &defaultAttr;
    }
    return result;
}

f32 FCV2_getPackValue(FCV2Pack *pack, f32 frame)
{
    FCV2KeyHeader *key = pack->cursor;
    s32 type = key->type_flags & 0xf;
    f32 value = 0.0f;

    switch (type) {
    case 1:
        value = key->value_or_spline_data;
        pack->cursor = key + 1;
        break;
    case 0:
        pack->cursor = &key->value_or_spline_data;
        break;
    case 3:
        value = FCV2_getPackVal(pack, &key->value_or_spline_data, key->key_count, frame);
        break;
    default:
        value = 0.0f;
        pack->cursor = &key->value_or_spline_data;
        break;
    }

    return value;
}

u16 *FCV2_getKey(u16 *cursor, f32 frame)
{
    s32 last_key;
    s32 index;
    u16 type;

    if ((cursor[1] & 0xf) == 3) {
        last_key = (s32)cursor[0] - 1;
        cursor += 2;
        index = 0;
        if (last_key > 0) {
            f32 frame_scale = FCV2_KEY_FRAME_SCALE;

            do {
                if (frame <= (f32)cursor[1] * frame_scale) {
                    return cursor;
                }

                type = cursor[0];
                switch (type) {
                case 0:
                case 2:
                case 3:
                    cursor += 4;
                    break;
                case 1:
                    cursor += 8;
                    break;
                default:
                    break;
                }

                index++;
            } while (index < last_key);

        }

        return cursor;
    }

    return 0;
}

void FCV2_dump(u16 *data)
{
    u16 key_count;
    u16 *cursor;
    s32 index;

    if (data[1] == 3) {
        key_count = data[0];
        cursor = data + 2;
        index = 0;
        if (key_count != 0) {
            do {
                switch (cursor[0]) {
                case 0:
                case 2:
                case 3:
                    cursor += 4;
                    break;
                case 1:
                    cursor += 8;
                    break;
                default:
                    break;
                }
                index++;
            } while (index < key_count);
        }
    }
}
