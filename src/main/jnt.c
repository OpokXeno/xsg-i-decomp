#include "common.h"
#include "main/xgl_2.h"
#include "shared.h"
#include "main/jnt.h"
#include "jnt.h"
#include "main/align.h"
#include "main/matrix_print.h"
#include "quadword.h"
#include "vu0_macro.h"

/*
 * The JNT producer's work area is in the EE scratchpad (SPR, 0x70000000);
 * functions reach it through JNT_SCRATCH_BASE.
 *
 * The base has to stay a value the function holds: folding it into each
 * store's address instead (`*(T *)0x700007a0`) makes 2.96 emit separate
 * li/sw pairs in place of one shared `lui $2,0x7000` and displaced stores.
 */
#define JNT_SCRATCH_BASE ((void *)SCRATCHPAD_BASE)

/* Channel flags word of a packed FCV attribute (FcvPackAttribute.channel_flags). */
#define JNT_CHANNEL_FLAGS_END 0xFFFF     /* end-of-pack marker */
#define JNT_CHANNEL_FLAG_INHERIT 0x8000  /* carries its own translation/rotation/scale packs */

/* Joint element key (the channel flags kept in JntInterruptElement.key). */
#define JNT_KEY_LINK_PIVOT 2        /* pivot follows attribute->pivot_index */
#define JNT_KEY_LINK_TRANSLATION 4  /* translation follows attribute->translation_index */
#define JNT_KEY_LINK_ROTATION 8     /* rotation follows attribute->rotation_index */
#define JNT_KEY_LINK_MASK 0xe

/* JntWork.flags */
#define JNT_WORK_PACK_ENDED 0x04U /* the FCV pack's end marker was read */
#define JNT_WORK_NO_PACK 0x20     /* JNT_setFCurve got no pack: consumers do nothing */

/* JntProducer.consumer_ids: bit marking a filter rather than an interrupt channel. */
#define JNT_CONSUMER_ID_FILTER 0x8000

/* A producer consumer is called only when its address is word-aligned and in 0x1F0000..0x1FFFFFF. */
#define JNT_CONSUMER_CODE_BASE 0x001F0000U
#define JNT_CONSUMER_CODE_SPAN 0x01E0FFFFU

/* Hair id tag (JntChannelTag.tag): high half selects the kind, low half is the offset. */
#define JNT_TAG_KIND_MASK 0xFFFF0000
#define JNT_TAG_KIND_HAIR_OFFSET 0x008A0000
#define JNT_TAG_VALUE_MASK 0xFFFF

#define jntChainAt(work, index) (&(work)->chain_slots[(index)])

struct JntGameLoopState {
    unsigned char unmodeled_00[0x0c];
    unsigned short mode;
};

typedef struct JntChannelTag {
    int word_count; /* 0x00: the record's length in words, never 0 on the disc */
    int tag;        /* 0x04 */
} JntChannelTag;

typedef struct JntChannelWork {
    unsigned char unmodeled_000[0x28];
    struct JntHairIdSource *hair_id_source; /* 0x28 */
    unsigned char unmodeled_02c[0x7dc];
    struct JntChannelTag *tag;              /* 0x808 */
} JntChannelWork;

typedef struct JntAccessoryRecord {
    unsigned short size;
    unsigned short type;
} JntAccessoryRecord;

typedef void *(*JntProductionConsumer)(JntWork *work,
                                      struct JntStaticChannel *channels,
                                      int argument);

/*
 * Three more scratchpad slots share a fixed layout with the pair above, so
 * they are named with a partial struct instead of one offset macro per
 * field: JNT_setInterpMatrix stores its matrix-buffer argument at 0x7A4 and
 * its float argument at 0x800 (interp_matrix / interpolation in JntWork above).
 *
 * JNT_setCurve stores its two pointer arguments at 0x7A8 and 0x7AC. Neither is
 * an end pointer: JNT_getStaticVal reads the 0x7A8 table as
 * static_value_records and JNT_getVal reads the 0x7AC table as value_records.
 * Both are independent 32-byte record tables indexed by the current element
 * index, and each function uses its table only when both slots are non-null.
 */
/*
 * A joint element is a fixed-size record: JNT_getElement indexes an array of
 * them and JNT_nextElement advances by exactly one. The layout beyond that
 * 0x40-byte stride is not modeled, so it stays a single unmodeled span
 * (docs/naming.md) rather than invented members.
 */
typedef struct JntElement {
    unsigned char unmodeled_00[0x40];
} JntElement;

/*
 * The joint resource header JNT_getRootElement reads: 0x10 bytes not
 * modeled here, then a self-relative offset field whose own address plus
 * its value is the joint's root element.
 */
typedef struct JntElementHeader {
    unsigned char unmodeled_00[0x10];
    int root_element_offset;
} JntElementHeader;

typedef unsigned int JntRootStorage __attribute__((mode(TI)));

typedef union JntRootSlot {
    JntRootStorage storage;
    struct {
        float x;
        float y;
        float z;
        unsigned int opaque_w;
    } components;
} JntRootSlot;

typedef struct JntRootScratch {
    unsigned char unmodeled_00[0x40];
    JntRootSlot translation;
    JntRootSlot rotation;
    JntRootSlot scale;
} JntRootScratch;

typedef struct JntMoveElement {
    unsigned short type;
    unsigned char unmodeled_02[0x3E];
} JntMoveElement;

typedef struct JntMoveElementHeader {
    unsigned char unmodeled_00[0x08];
    unsigned short element_count;
    unsigned char unmodeled_0A[0x06];
    int element_offset;
} JntMoveElementHeader;

struct FcvPackAttribute {
    unsigned char unmodeled_00[4];
    unsigned short channel_flags;
    unsigned short value_mask; /* 0x06 */
    unsigned short translation_index; /* 0x08 */
    unsigned short pivot_index;       /* 0x0A */
    unsigned short rotation_index;    /* 0x0C */
};

struct JntProductionOwner {
    unsigned char unmodeled_000[0x10];
    Vector4 position; /* 0x10 */
    unsigned char unmodeled_020[0x50 - 0x20];
    Vector4 rotation; /* 0x50 */
    unsigned char unmodeled_060[0xA40 - 0x60];
    unsigned char movement_flags; /* 0xA40 */
    unsigned char unmodeled_A41[3];
    float movement_delay; /* 0xA44 */
    unsigned char unmodeled_A48[8];
    Vector4 movement; /* 0xA50 */
    JntMatrix *matrix_buffer; /* 0xA60 */
};

typedef struct JntRotationLimitHeader {
    unsigned short values_offset;
    unsigned short deferred_offset;
} JntRotationLimitHeader;

typedef struct JntRotationLimits {
    float minimum[3];
    unsigned short flags;
    unsigned char unmodeled_0e[2];
    float maximum[3];
} JntRotationLimits;

typedef struct JntPreviousTransform {
    Vector4 translation;
    Vector4 rotation;
} JntPreviousTransform;

typedef struct JntModelMatrixResource {
    unsigned char unmodeled_00[0x50];
    int matrices_offset;          /* 0x50 */
    unsigned char unmodeled_54[4];
    int combined_matrices_offset; /* 0x58 */
    int model_matrices_offset;    /* 0x5C */
} JntModelMatrixResource;

/* File scope: as a late block-scope extern in JNT_computeMatrix, the
   contract cc1 reads uninitialized memory for it and compiles that
   function nondeterministically. */
extern struct JntGameLoopState GameLoopState;

char *strncpy(char *destination, const char *source, unsigned int count);
float FCV2_getPackValue(unsigned short **cursor, float frame);
struct FcvPackAttribute *FCV2_getPackAttribute(unsigned short **cursor);

JntStaticChannel *JNT_setMDLMatrix(JntWork *work,
                                 JntStaticChannel *channel);

void FCV2_resetPack(void *pack_state, void *pack);
void xglMatrix2Quaternion(float quaternion[4], const void *matrix);
void xglQuaternion2Matrix(Matrix4 destination, const Vector4 *quaternion);
void xglQuaternionInterpolateLinear(Vector4 *destination, const Vector4 *left, const Vector4 *right, float left_weight, float right_weight);
JntStaticChannel *JNT_getVal2(JntWork *work, JntStaticChannel *channel, float frame);
void CUR_MATRIX_4s3(Matrix4 matrix);
void CUR_MATRIX_GetT(Vector4 *translation);
void CUR_MATRIX_SetR(Matrix4 matrix);
JntStaticChannel *JNT_getVal_HOGE2(JntWork *work, JntStaticChannel *channel, float frame);
JntStaticChannel *EXM_CalcExMotion(JntWork *work, JntStaticChannel *channel);
void EXM_PresetHair(JntWork *work);
void xglMatrixMul(Matrix4 destination, Matrix4 left, Matrix4 right);
void xglMatrixUnit4s(Matrix4 destination);
JntStaticChannel *JNT_interpolate(JntWork *work, JntStaticChannel *channel);
JntStaticChannel *JNT_constructMatrix(JntWork *work, JntStaticChannel *channel);

void get_fcvpack(float *destination, const float *fallback_values,
                 JntWork *pack, unsigned int pack_mask, float frame);

void get_fcvpack_fix(float *destination, float default_value,
                    JntWork *pack, unsigned int pack_mask, float frame);

void get_fcvpack_flag(JntWork *pack, int channel_flags, float frame);

JntStaticChannel *JNT_getVal(JntWork *work, JntStaticChannel *channel,
                             float frame);

JntStaticChannel *JNT_getVal_staticChain(JntWork *work,
                                         JntStaticChannel *channel,
                                         float frame);

/*
 * Hair-smoothing toggle, set by JNT_onSmoothHair and JNT_offSmoothHair.
 * JNT_computeMatrix reads it to decide whether to apply the interpolated hair matrix.
 */
static signed char flagSmoothHair = 0;

static unsigned char dummyAttr[0x10] = {0};

JntChannelTag dummyCntFree[2] = {0};

char dummyName[0x10] = {0};

float jointMovementFollowRate = 0.80000001192092896f;

static inline void jntCopyMatrix(JntMatrix *destination, const JntMatrix *source)
{
    QuadwordCopyMatrix(destination, source);
}

static inline void jntCopyMatrixRow(float destination[4], const float source[4])
{
    QuadwordCopy(destination, source);
}

static inline JntMatrix *jntMatrixAt(void *base, int index)
{
    return (JntMatrix *)((index << 6) + (unsigned int)base);
}

/* Fills three values: each from the pack when its mask bit is set, otherwise from fallback_values. */
void get_fcvpack(float *destination, const float *fallback_values,
                 JntWork *pack, unsigned int pack_mask,
                 float frame)
{
    int index;

    for (index = 0; index < 3; index++) {
        if (pack_mask & (1 << index)) {
            destination[index] = FCV2_getPackValue(&pack->cursor, frame);
        } else {
            const float *fallback_sample = fallback_values;

            fallback_sample += index;
            destination[index] = *fallback_sample;
        }
    }
}

/* Like get_fcvpack, but components whose mask bit is clear take default_value. */
void get_fcvpack_fix(float *destination, float default_value,
                    JntWork *pack, unsigned int pack_mask, float frame)
{
    int index = 0;

    do {
        if (pack_mask & (1 << index)) {
            *destination = FCV2_getPackValue(&pack->cursor, frame);
        } else {
            *destination = default_value;
        }
        destination++;
        index++;
    } while (index < 3);
}

/* When channel_flags bit 0 is set, sets or clears pack->flags bit 0 from the next pack value. */
void get_fcvpack_flag(JntWork *pack, int channel_flags,
                      float frame)
{
    if (channel_flags & 1) {
        if (FCV2_getPackValue(&pack->cursor, frame) != 0.0f) {
            pack->flags |= 1;
        } else {
            pack->flags &= ~1;
        }
    }
}

/*
 * Copies the static channel's translation and rotation into the current
 * element and its static record, then advances to the next channel.
 */
JntStaticChannel *JNT_getStaticVal(JntWork *work, JntStaticChannel *channel)
{
    int index;
    int type;
    JntInterruptElement *element;
    JntStaticChannel *next_channel;

    index = work->current_index;
    next_channel = channel + 1;
    type = channel->type;
    element = &work->elements[index];

    /* Types 1-4 also reset the element scale to 1; type 5 copies only translation and rotation. */
    switch (type) {
    case 5:
        {
            float *translation = &element->translation.x;
            float *rotation = element->rotation;
            const float *translate = &channel->translation.x;
            const float *rotate = channel->rotate.values;

            translation[0] = translate[0];
            translation[1] = translate[1];
            translation[2] = translate[2];
            rotation[0] = rotate[0];
            rotation[1] = rotate[1];
            rotation[2] = rotate[2];
        }
        break;
    case 1:
    case 2:
    case 3:
    case 4:
        {
        float *translation = &element->translation.x;
        float *rotation = element->rotation;
        const float *translate = &channel->translation.x;
        const float *rotate = channel->rotate.values;

        translation[0] = translate[0];
        translation[1] = translate[1];
        translation[2] = translate[2];
        rotation[0] = rotate[0];
        rotation[1] = rotate[1];
        rotation[2] = rotate[2];
        element->scale.z = 1.0f;
        element->scale.y = 1.0f;
        element->scale.x = 1.0f;
        }
        break;
    case 0:
    default:
        break;
    }

    if (work->value_records != 0 && work->static_value_records != 0) {
        /* The original adds the byte offset to the integer table address. */
        JntTransformRecord *record = (JntTransformRecord *)(index * sizeof(JntTransformRecord)
                                                            + (unsigned int)work->static_value_records);
        Vector4 *rotation = &record->rotation;
        const float *translate = &channel->translation.x;
        const float *rotate = channel->rotate.values;

        record->translation.x = translate[0];
        record->translation.y = translate[1];
        record->translation.z = translate[2];
        rotation->x = rotate[0];
        rotation->y = rotate[1];
        rotation->z = rotate[2];
        record->translation.w = 1.0f;
        rotation->w = 1.0f;
    }

    work->current_index = index + 1;
    element->key = 0;
    element->attribute_data = dummyAttr;
    element->interrupt_channel = 0;
    return next_channel;
}

/*
 * Reads a pack attribute, then the values its value_mask selects into values,
 * with a stride of four floats. Unselected entries are set to zero.
 */
struct FcvPackAttribute *FCV_getAttrAndValue(float *values,
                                             unsigned short **cursor,
                                             float frame)
{
    struct FcvPackAttribute *attribute;
    unsigned int value_mask;
    unsigned int third_value;
    int remaining = 6;

    attribute = FCV2_getPackAttribute(cursor);
    value_mask = attribute->value_mask;
    do {
        if (value_mask & 1) {
            values[0] = FCV2_getPackValue(cursor, frame);
        } else {
            values[0] = 0.0f;
        }
        if (value_mask & 2) {
            values[1] = FCV2_getPackValue(cursor, frame);
        } else {
            values[1] = 0.0f;
        }
        third_value = value_mask & 4;
        value_mask >>= 3;
        if (third_value) {
            values[2] = FCV2_getPackValue(cursor, frame);
        } else {
            values[2] = 0.0f;
        }
        remaining -= 3;
        values += 4;
    } while (remaining >= 0);
    if (value_mask & 1) {
        values[0] = FCV2_getPackValue(cursor, frame);
    } else {
        values[0] = 0.0f;
    }
    return attribute;
}

/*
 * Samples the current channel from the joint pack into the current element,
 * applying inheritance and the type's rotation limits, then advances the
 * element index. Returns the next channel.
 */
JntStaticChannel *JNT_getVal(JntWork *work, JntStaticChannel *channel,
                            float frame)
{
    JntInterruptElement *element = &work->elements[work->current_index];
    const float *translation = &channel->translation.x;
    const float *rotation = channel->rotate.values;
    void *attribute_data;
    unsigned int channel_flags;
    unsigned int value_mask;
    int inherit_channels;
    JntStaticChannel *next_channel;
    const float unit_scale = 1.0f;
    unsigned int scale_mask;

    element->interrupt_channel = 0;
    if (work->flags & JNT_WORK_PACK_ENDED) {
        value_mask = 0;
        attribute_data = dummyAttr;
        channel_flags = 0;
    } else {
        struct FcvPackAttribute *attribute = FCV2_getPackAttribute(&work->cursor);

        attribute_data = attribute;
        if (attribute->channel_flags == JNT_CHANNEL_FLAGS_END) {
            element->key = 0;
            element->attribute_data = 0;
            work->flags |= JNT_WORK_PACK_ENDED;
            return channel;
        }
        channel_flags = attribute->channel_flags;
        value_mask = attribute->value_mask;
    }
    inherit_channels = channel_flags & JNT_CHANNEL_FLAG_INHERIT;
    element->attribute_data = attribute_data;
    element->key = channel_flags;
    next_channel = channel + 1;
    if (inherit_channels) {
        work->channel_start_index = work->current_index;
        get_fcvpack(&element->translation.x,translation, work, value_mask, frame);
        value_mask >>= 3;
        get_fcvpack(element->rotation,rotation, work, value_mask, frame);
        value_mask >>= 3;
        get_fcvpack_fix(&element->scale.x,unit_scale, work, value_mask, frame);
        get_fcvpack_flag(work, value_mask >> 1, frame);
        value_mask = 0;
    }
    switch (channel->type) {
    case 0:
    case 1:
        if (!inherit_channels) {
            get_fcvpack(&element->translation.x,translation, work, value_mask, frame);
            value_mask >>= 3;
            get_fcvpack(element->rotation,rotation, work, value_mask, frame);
            get_fcvpack_fix(&element->scale.x,1.0f, work, value_mask >> 3, frame);
        }
        break;
    case 3:
        get_fcvpack(&element->translation.x,translation, work, value_mask, frame);
        get_fcvpack(element->rotation,rotation, work, value_mask >> 3, frame);
        break;
    case 2:
        {
            float (*angles)[3] = &element->rotation;
            float *sampled_translation;
            int limits_offset;

            value_mask >>= 3;
            get_fcvpack(*angles, rotation, work, value_mask, frame);
            get_fcvpack_fix(&element->scale.x,unit_scale, work, value_mask >> 3, frame);
            sampled_translation = &element->translation.x;
            limits_offset = channel->rotation_limits_offset;
            sampled_translation[0] = translation[0];
            sampled_translation[1] = translation[1];
            sampled_translation[2] = translation[2];
            if (limits_offset != 0) {
                JntRotationLimitHeader *header = (JntRotationLimitHeader *)
                    ((unsigned char *)channel + limits_offset);
                int index = 0;

                if (header->values_offset != 0) {
                    JntRotationLimits *limits = (JntRotationLimits *)
                        ((unsigned char *)header + header->values_offset);
                    unsigned short flags = limits->flags;
                    const float *minimum = limits->minimum;

                    do {
                        if ((flags >> index) & 1) {
                            float angle = (*angles)[(unsigned int)index];
                            float limit = minimum[index];

                            if (angle < limit) {
                                (*angles)[(unsigned int)index] = limit;
                            }
                        }
                        index++;
                    } while (index < 3);
                    {
                        const float *maximum = limits->maximum;

                        flags >>= 3;
                        index = 0;
                        do {
                            if ((flags >> index) & 1) {
                                float angle = (*angles)[(unsigned int)index];
                                float limit = maximum[index];

                                if (angle > limit) {
                                    (*angles)[(unsigned int)index] = limit;
                                }
                            }
                            index++;
                        } while (index < 3);
                    }
                }
            }
        }
        break;
    case 4:
        {
            float *sampled_translation = &element->translation.x;

            sampled_translation[0] = translation[0];
            sampled_translation[1] = translation[1];
            sampled_translation[2] = translation[2];
        }
        break;
    case 5:
        {
            get_fcvpack(&element->translation.x,translation, work, value_mask, frame);
            value_mask >>= 3;
            get_fcvpack(element->rotation,rotation, work, value_mask, frame);
            scale_mask = value_mask >> 3;
            get_fcvpack_fix(&element->scale.x,unit_scale, work, scale_mask, frame);
        }
        break;
    case 6:
        break;
    }
    /* With both record tables set, blend with the previous frame's record (flag 0x80), then store the sample. */
    {
        const JntPreviousTransform *previous = work->value_records;

    if (previous != 0 && work->static_value_records != 0) {
        JntTransformRecord *records = *work->static_value_records;

        if (work->flags & 0x80) {
            const Vector4 *previous_translation;
            const Vector4 *previous_rotation;

            previous_translation = (const Vector4 *)((work->current_index << 5) +
                (unsigned int)previous);

            if (previous_translation->w != 0.0f) {
                previous_rotation = previous_translation + 1;
                element->translation.x = previous_translation->x +
                    (element->translation.x - previous_translation->x) * work->interpolation;
                element->translation.y = previous_translation->y +
                    (element->translation.y - previous_translation->y) * work->interpolation;
                element->translation.z = previous_translation->z +
                    (element->translation.z - previous_translation->z) * work->interpolation;
                element->rotation[0] = previous_rotation->x +
                    (element->rotation[0] - previous_rotation->x) * work->interpolation;
                element->rotation[1] = previous_rotation->y +
                    (element->rotation[1] - previous_rotation->y) * work->interpolation;
                element->rotation[2] = previous_rotation->z +
                    (element->rotation[2] - previous_rotation->z) * work->interpolation;
            }
        }
        {
            float *sampled_translation = &element->translation.x;
            float *sampled_rotation = element->rotation;
            JntTransformRecord *record = (JntTransformRecord *)
                ((work->current_index << 5) + (unsigned int)records);
            Vector4 *stored_rotation = &record->rotation;

            record->translation.x = sampled_translation[0];
            record->translation.y = sampled_translation[1];
            record->translation.z = sampled_translation[2];
            stored_rotation->x = sampled_rotation[0];
            stored_rotation->y = sampled_rotation[1];
            stored_rotation->z = sampled_rotation[2];
            record->translation.w = 1.0f;
            stored_rotation->w = 1.0f;
        }
    }
    }
    work->current_index++;
    return next_channel;
}

/*
 * Static-chain variant of JNT_getVal: samples the channel into the current
 * element without the previous-frame blend or the record-table store, then
 * advances the element index.
 */
JntStaticChannel *JNT_getVal_staticChain(JntWork *work,
                                        JntStaticChannel *channel, float frame)
{
    int index;
    JntInterruptElement *element = &work->elements[work->current_index];
    const float *translation = &channel->translation.x;
    const float *rotation = channel->rotate.values;
    void *attribute_data;
    unsigned int channel_flags;
    unsigned int value_mask;
    int inherit_channels;
    JntStaticChannel *next_channel;

    element->interrupt_channel = 0;
    if (work->flags & JNT_WORK_PACK_ENDED) {
        value_mask = 0;
        attribute_data = dummyAttr;
        channel_flags = 0;
    } else {
        struct FcvPackAttribute *attribute = FCV2_getPackAttribute(&work->cursor);

        attribute_data = attribute;
        if (attribute->channel_flags == JNT_CHANNEL_FLAGS_END) {
            element->key = 0;
            element->attribute_data = 0;
            work->flags |= JNT_WORK_PACK_ENDED;
            return channel;
        }
        channel_flags = attribute->channel_flags;
        value_mask = attribute->value_mask;
    }
    element->key = channel_flags;
    inherit_channels = channel_flags & JNT_CHANNEL_FLAG_INHERIT;
    next_channel = channel + 1;
    element->attribute_data = attribute_data;
    if (inherit_channels) {
        work->channel_start_index = work->current_index;
        get_fcvpack(&element->translation.x, translation, work, value_mask, frame);
        value_mask >>= 3;
        get_fcvpack(element->rotation, rotation, work, value_mask, frame);
        value_mask >>= 3;
        get_fcvpack_fix(&element->scale.x, 1.0f, work, value_mask, frame);
        get_fcvpack_flag(work, value_mask >> 1, frame);
        value_mask = 0;
    }
    switch (channel->type) {
    case 0:
    case 1:
        if (!inherit_channels) {
            unsigned int scale_mask;

            get_fcvpack(&element->translation.x, translation, work, value_mask, frame);
            value_mask >>= 3;
            get_fcvpack(element->rotation, rotation, work, value_mask, frame);
            scale_mask = value_mask >> 3;
            get_fcvpack_fix(&element->scale.x, 1.0f, work, scale_mask, frame);
        }
        break;
    case 3:
        {
            for (index = 0; index < 3; index++) {
                int channel_bit = 1 << index;

                if (value_mask & channel_bit) {
                    (&element->translation.x)[index] = FCV2_getPackValue(&work->cursor, frame);
                }
                {
                    const float *static_sample = translation;

                    static_sample += index;
                    (&element->translation.x)[index] = *static_sample;
                }
            }
        }
        {
            unsigned int rotation_mask = value_mask >> 3;

            get_fcvpack(element->rotation, rotation, work, rotation_mask, frame);
        }
        break;
    case 2:
        {
            float (*angles)[3];
            float *sampled_translation;
            int limits_offset;

            value_mask >>= 3;
            angles = &element->rotation;
            get_fcvpack(*angles, rotation, work, value_mask, frame);
            get_fcvpack_fix(&element->scale.x, 1.0f, work, value_mask >> 3, frame);
            sampled_translation = &element->translation.x;
            limits_offset = channel->rotation_limits_offset;
            sampled_translation[0] = translation[0];
            sampled_translation[1] = translation[1];
            sampled_translation[2] = translation[2];
            if (limits_offset != 0) {
                JntRotationLimitHeader *header = (JntRotationLimitHeader *)
                    ((unsigned char *)channel + limits_offset);

                index = 0;
                if (header->values_offset != 0) {
                    JntRotationLimits *limits = (JntRotationLimits *)
                        ((unsigned char *)header + header->values_offset);
                    unsigned short flags = limits->flags;
                    const float *axis_limits = limits->minimum;

                    for (index = 0; index < 3; index++) {
                        if ((flags >> index) & 1) {
                            float angle = (*angles)[(unsigned int)index];
                            float limit = axis_limits[index];

                            if (angle < limit) {
                                (*angles)[(unsigned int)index] = limit;
                            }
                        }
                    }
                    {
                        axis_limits = limits->maximum;

                        flags >>= 3;
                        index = 0;
                        do {
                            if ((flags >> index) & 1) {
                                float angle = (*angles)[(unsigned int)index];
                                float limit = axis_limits[index];

                                if (angle > limit) {
                                    (*angles)[(unsigned int)index] = limit;
                                }
                            }
                            index++;
                        } while (index < 3);
                    }
                }
            }
        }
        break;
    case 4:
        {
            float *sampled_translation = &element->translation.x;

            sampled_translation[0] = translation[0];
            sampled_translation[1] = translation[1];
            sampled_translation[2] = translation[2];
        }
        break;
    case 6:
        work->current_index++;
        return next_channel;
    case 5:
        get_fcvpack(&element->translation.x, translation, work, value_mask, frame);
        value_mask >>= 3;
        get_fcvpack(element->rotation, rotation, work, value_mask, frame);
        get_fcvpack_fix(&element->scale.x, 1.0f, work, value_mask >> 3, frame);
        break;
    }
    work->current_index++;
    return next_channel;
}

static inline void JntCopyVector(Vector4 *destination, const Vector4 *source)
{
    *destination = *source;
}

/*
 * Builds the current channel's matrix into matrix_buffer. Direct channels
 * (types 0, 1, 5) use their own transform, chain-root and chain-link channels
 * (types 3 and 2) add slots to the chain, and the chain end (type 4) solves it.
 * An interrupt consumer, when set, runs first.
 */
JntStaticChannel *JNT_constructMatrix(JntWork *work, JntStaticChannel *channel)
{
    Vector4 target;
    Matrix4 alignment;
    Vector4 two_link_axis;
    Vector4 endpoint;
    Vector4 chain_axis;
    Vector4 current_translation;
    float pivot_angle;
    float joint_angle;
    float chain_angle;
    JntStaticChannel *next_channel = channel + 1;
    JntInterruptElement *element = &work->elements[work->current_index];
    struct FcvPackAttribute *attribute = element->attribute_data;
    unsigned short flags = element->key;
    JntChainSlot *chain_slot;

    if (attribute != 0) {
        short interrupt_channel = element->interrupt_channel;
        if (interrupt_channel != 0) {
            JntProducer *producer = work->producer;
            JntProductionConsumer consumer = producer->consumers[interrupt_channel];
            int argument = producer->consumer_arguments[interrupt_channel];
            unsigned int address = (unsigned int)consumer;
            if (address - JNT_CONSUMER_CODE_BASE <= JNT_CONSUMER_CODE_SPAN) {
                if ((address & 3U) == 0) {
                    consumer(work, channel, argument);
                }
            }
        }
        /* A link flag takes translation or rotation from an earlier joint's matrix when that joint is already built. */
        switch (channel->type) {
        case 0:
        case 1: {
            unsigned short link;
            CUR_MATRIX_Set(work->matrix_buffer[channel->matrix_index].elements);
            if ((flags & JNT_KEY_LINK_MASK) != 0) {
                if ((flags & JNT_KEY_LINK_TRANSLATION) != 0) {
                    link = attribute->translation_index;
                    if (link < work->current_index) {
                        CUR_MATRIX_SetT((const Vector4 *)work->matrix_buffer[link].elements[3]);
                    }
                } else {
                    CUR_MATRIX_Txyz4s(&element->translation);
                }
                if ((flags & JNT_KEY_LINK_ROTATION) != 0) {
                    link = attribute->rotation_index;
                    if (link < work->current_index) {
                        CUR_MATRIX_SetR(work->matrix_buffer[link].elements);
                    }
                } else {
                    CUR_MATRIX_Rzyx4s((const int *)element->rotation);
                    CUR_MATRIX_Sxyz4s(&element->scale);
                }
            } else {
                CUR_MATRIX_Txyz4s(&element->translation);
                CUR_MATRIX_Rzyx4s((const int *)element->rotation);
                CUR_MATRIX_Sxyz4s(&element->scale);
            }
            if (channel->rotation_limits_offset != 0) {
                JntRotationLimitHeader *limit = (JntRotationLimitHeader *)
                    ((unsigned char *)channel + channel->rotation_limits_offset);
                if (limit->deferred_offset != 0) {
                    JntProducer *producer = work->producer;
                    int slot = producer->deferred_count++;
                    producer->deferred_limits[slot] = (struct ActorShapeBinding *)
                        ((unsigned char *)limit + limit->deferred_offset);
                    if ((flags & JNT_KEY_LINK_TRANSLATION) != 0) {
                        if (attribute->translation_index < work->current_index) {
                            element = &work->elements[attribute->translation_index];
                            CUR_MATRIX_SetT(&element->translation);
                        }
                    } else {
                        CUR_MATRIX_SetT(&element->translation);
                    }
                }
            }
            CUR_MATRIX_Get(work->matrix_buffer[work->current_index].elements);
            break;
        }
        case 3: {
            Vector4 *root_translation;
            CUR_MATRIX_Set(work->matrix_buffer[channel->matrix_index].elements);
            work->chain_base_index = work->current_index;
            CUR_MATRIX_Txyz4s(&element->translation);
            CUR_MATRIX_Rzyx4s((const int *)element->rotation);
            CUR_MATRIX_Get(work->transform_basis);
            CUR_MATRIX_Get(work->matrix_buffer[work->current_index].elements);
            work->chain_length = 0.0f;
            root_translation = &work->chain_slots[0].translation;
            {
                const Vector4 *basis_translation = (const Vector4 *)work->transform_basis[3];
                *root_translation = *basis_translation;
            }
            root_translation->w = 1.0f;
            work->chain_slots[0].attribute = attribute;
            work->matrix_index = 1;
            break;
        }
        case 2: {
            float initial_length = element->translation.x;
            float *source_rotation = element->rotation;
            Vector4 *translation;
            Vector4 *scale;
            Vector4 *rotation;
            chain_slot = &work->chain_slots[work->matrix_index];
            translation = &chain_slot->translation;
            scale = &chain_slot->scale;
            rotation = &chain_slot->rotation;
            work->chain_length += initial_length;
            {
                const Vector4 *translation_vector = &element->translation;
                *translation = *translation_vector;
            }
            translation->w = 1.0f;
            chain_slot->attribute = attribute;
            rotation->x = source_rotation[0];
            rotation->y = source_rotation[1];
            rotation->z = source_rotation[2];
            {
                const Vector4 *source_scale = &element->scale;
                *scale = *source_scale;
            }
            scale->w = 1.0f;
            if (0.0f <= channel->translation_direction) {
                chain_slot->translation.x = -element->translation.x;
            }
            work->matrix_index++;
            break;
        }
        /* Chain end: solves the collected chain toward its target, or places it without one. */
        case 4: {
            float distance;
            float z_component;
            int index;
            int have_target = 0;
            if ((flags & JNT_KEY_LINK_MASK) != 0 && (flags & JNT_KEY_LINK_TRANSLATION) != 0 &&
                attribute->translation_index < work->current_index) {
                float *translation = work->matrix_buffer[attribute->translation_index].elements[3];
                float x;
                float y;
                float z;
                have_target = 1;
                target.x = translation[0];
                target.y = translation[1];
                target.z = translation[2];
                x = target.x;
                y = target.y;
                z = target.z;
                target.x = x * work->root_correction[0][0] +
                           y * work->root_correction[1][0] +
                           z * work->root_correction[2][0] +
                           work->root_correction[3][0];
                target.y = x * work->root_correction[0][1] +
                           y * work->root_correction[1][1] +
                           z * work->root_correction[2][1] +
                           work->root_correction[3][1];
                target.z = x * work->root_correction[0][2] +
                           y * work->root_correction[1][2] +
                           z * work->root_correction[2][2] +
                           work->root_correction[3][2];
            }
            if (!have_target) {
                CUR_MATRIX_Set(work->transform_basis);
                for (index = 1; index <= work->matrix_index - 1; index++) {
                    JntChainSlot *slot = &work->chain_slots[index];
                    slot->translation.x = __builtin_fabsf(slot->translation.x);
                    slot->translation.w = 1.0f;
                    if ((slot->attribute->channel_flags & JNT_KEY_LINK_ROTATION) != 0) {
                        unsigned short rotation_index = slot->attribute->rotation_index;
                        if (rotation_index < work->current_index) {
                            JntMatrix *rotation_matrix = &work->matrix_buffer[rotation_index];
                            CUR_MATRIX_Txyz4s(&slot->translation);
                            CUR_MATRIX_SetR(rotation_matrix->elements);
                            CUR_MATRIX_Get(work->matrix_buffer[work->chain_base_index + index].elements);
                        }
                    } else {
                        slot->translation.x = __builtin_fabsf(slot->translation.x);
                        CUR_MATRIX_Txyz4s(&slot->translation);
                        CUR_MATRIX_Rzyx4s((const int *)&slot->rotation);
                        CUR_MATRIX_Sxyz4s(&slot->scale);
                        CUR_MATRIX_Get(work->matrix_buffer[work->chain_base_index + index].elements);
                    }
                }
                CUR_MATRIX_Txyz4s(&element->translation);
                CUR_MATRIX_Get(work->matrix_buffer[work->current_index].elements);
            } else {
                Vector4 *translation = &element->translation;
                struct FcvPackAttribute *slot_attribute = element->attribute_data;
                JntChainSlot *slot = &work->chain_slots[work->matrix_index];
                Vector4 *slot_translation = &slot->translation;
                *slot_translation = *translation;
                slot->attribute = slot_attribute;
                slot_translation->w = 1.0f;
                work->matrix_index++;
                if (work->matrix_index == 4) {
                    float (*basis)[4];
                    float cosine;
                    float homogeneous;
                    float segment_length;
                    float squared_difference;
                    float sine;
                    float sine_magnitude;
                    Vector4 *upper_translation;
                    Vector4 *lower_translation;
                    Vu0SubVector(&two_link_axis, &target, &work->chain_slots[0].translation);
                    {
                        float x = two_link_axis.x;
                        float y = two_link_axis.y;
                        float z = two_link_axis.z;
                        two_link_axis.x = work->transform_basis[0][0] * x +
                                          work->transform_basis[0][1] * y +
                                          work->transform_basis[0][2] * z;
                        two_link_axis.y = work->transform_basis[1][0] * x +
                                          work->transform_basis[1][1] * y +
                                          work->transform_basis[1][2] * z;
                        two_link_axis.z = work->transform_basis[2][0] * x +
                                          work->transform_basis[2][1] * y +
                                          work->transform_basis[2][2] * z;
                    }
                    distance = two_link_axis.x * two_link_axis.x +
                               two_link_axis.y * two_link_axis.y +
                               two_link_axis.z * two_link_axis.z;
                    basis = work->transform_basis;
                    alignXAxis(&two_link_axis, alignment);
                    MATRIX_transpose3(alignment);
                    CUR_MATRIX_Set(basis);
                    CUR_MATRIX_4s3(alignment);
                    CUR_MATRIX_Get(basis);
                    if ((work->chain_slots[1].attribute->channel_flags & JNT_KEY_LINK_PIVOT) != 0) {
                        Vector4 *pivot = (Vector4 *)work->matrix_buffer[work->chain_slots[1].attribute->pivot_index].elements[3];
                        float radius;
                        float x;
                        float y;
                        float z;
                        float norm_squared;
                        pivot->w = 1.0f;
                        two_link_axis.x = pivot->x - work->transform_basis[3][0];
                        two_link_axis.y = pivot->y - work->transform_basis[3][1];
                        two_link_axis.z = pivot->z - work->transform_basis[3][2];
                        x = two_link_axis.x;
                        y = two_link_axis.y;
                        z = two_link_axis.z;
                        two_link_axis.x = work->transform_basis[0][0] * x +
                                          work->transform_basis[0][1] * y +
                                          work->transform_basis[0][2] * z;
                        two_link_axis.y = work->transform_basis[1][0] * x +
                                          work->transform_basis[1][1] * y +
                                          work->transform_basis[1][2] * z;
                        two_link_axis.z = work->transform_basis[2][0] * x +
                                          work->transform_basis[2][1] * y +
                                          work->transform_basis[2][2] * z;
                        radius = __builtin_sqrtf(two_link_axis.y * two_link_axis.y +
                                                two_link_axis.z * two_link_axis.z);
                        z_component = two_link_axis.z;
                        norm_squared = two_link_axis.x * two_link_axis.x +
                                       two_link_axis.y * two_link_axis.y +
                                       z_component * z_component;
                        __builtin_sqrtf(norm_squared);
                        z_component = two_link_axis.z;
                        pivot_angle = -xglAtan2(-z_component / radius, two_link_axis.y / radius);
                        CUR_MATRIX_Set(basis);
                        CUR_MATRIX_Rx4s((const int *)&pivot_angle);
                        CUR_MATRIX_Sxyz4s(&work->chain_slots[1].scale);
                        CUR_MATRIX_Get(basis);
                    } else {
                        CUR_MATRIX_Rx4s((const int *)&work->chain_slots[1].rotation.z);
                        CUR_MATRIX_Sxyz4s(&work->chain_slots[1].scale);
                        CUR_MATRIX_Get(basis);
                    }
                    upper_translation = &work->chain_slots[2].translation;
                    lower_translation = &work->chain_slots[3].translation;
                    distance = __builtin_sqrtf(distance);
                    segment_length = upper_translation->x;
                    if (0.0f <= segment_length) {
                        float final_length = lower_translation->x;
                        squared_difference = segment_length * segment_length - final_length * final_length;
                        cosine = (distance * distance + squared_difference) /
                                 ((2.0f * segment_length) * distance);
                        sine_magnitude = __builtin_sqrtf(1.0f - cosine * cosine);
                        sine = sine_magnitude;
                    } else {
                        float final_length;
                        float magnitude = __builtin_fabsf(segment_length);
                        upper_translation->x = magnitude;
                        final_length = lower_translation->x;
                        squared_difference = magnitude * magnitude - final_length * final_length;
                        cosine = (distance * distance + squared_difference) /
                                 ((2.0f * magnitude) * distance);
                        sine_magnitude = __builtin_sqrtf(1.0f - cosine * cosine);
                        sine = -sine_magnitude;
                    }
                    if (1.0f < cosine) {
                        cosine = 1.0f;
                        sine = 0.0f;
                    } else if (cosine < -1.0f) {
                        cosine = -1.0f;
                        sine = 0.0f;
                    }
                    joint_angle = xglAtan2(sine, cosine);
                    CUR_MATRIX_Set(basis);
                    CUR_MATRIX_Rz4s((const int *)&joint_angle);
                    CUR_MATRIX_Get(work->matrix_buffer[work->chain_base_index + 1].elements);
                    CUR_MATRIX_Txyz4s(upper_translation);
                    CUR_MATRIX_Get(basis);
                    CUR_MATRIX_Get(work->interpolation_root.elements);
                    homogeneous = 1.0f;
                    JntCopyVector(&endpoint, (const Vector4 *)work->interpolation_root.elements[3]);
                    endpoint.w = homogeneous;
                    Vu0SubVector(&two_link_axis, &target, &endpoint);
                    {
                        float *output_y = &two_link_axis.y;
                        float *output_z = &two_link_axis.z;
                        float x = two_link_axis.x;
                        float y = two_link_axis.y;
                        float z = two_link_axis.z;
                        two_link_axis.w = homogeneous;
                        two_link_axis.x = basis[0][0] * x +
                                          work->transform_basis[0][1] * y +
                                          work->transform_basis[0][2] * z;
                        *output_y = work->transform_basis[1][0] * x +
                                          work->transform_basis[1][1] * y +
                                          work->transform_basis[1][2] * z;
                        *output_z = work->transform_basis[2][0] * x +
                                          work->transform_basis[2][1] * y +
                                          work->transform_basis[2][2] * z;
                    }
                    alignXAxis(&two_link_axis, alignment);
                    MATRIX_transpose3(alignment);
                    CUR_MATRIX_Set(basis);
                    CUR_MATRIX_4s3(alignment);
                    CUR_MATRIX_Get(work->matrix_buffer[work->chain_base_index + 2].elements);
                    CUR_MATRIX_Txyz4s(translation);
                    CUR_MATRIX_Get(work->matrix_buffer[work->current_index].elements);
                } else if (work->matrix_index == 3) {
                    float x = target.x - work->chain_slots[0].translation.x;
                    float y = target.y - work->chain_slots[0].translation.y;
                    float z = target.z - work->chain_slots[0].translation.z;
                    endpoint.x = work->transform_basis[0][0] * x +
                                 work->transform_basis[0][1] * y +
                                 work->transform_basis[0][2] * z;
                    endpoint.y = work->transform_basis[1][0] * x +
                                 work->transform_basis[1][1] * y +
                                 work->transform_basis[1][2] * z;
                    endpoint.z = work->transform_basis[2][0] * x +
                                 work->transform_basis[2][1] * y +
                                 work->transform_basis[2][2] * z;
                    alignXAxis(&endpoint, alignment);
                    MATRIX_transpose3(alignment);
                    CUR_MATRIX_Set(work->transform_basis);
                    CUR_MATRIX_4s3(alignment);
                    CUR_MATRIX_Rx4s((const int *)&work->chain_slots[1].rotation.z);
                    CUR_MATRIX_Sxyz4s(&work->chain_slots[1].scale);
                    CUR_MATRIX_Get(work->matrix_buffer[work->chain_base_index + 1].elements);
                    CUR_MATRIX_Txyz4s(translation);
                    CUR_MATRIX_Get(work->matrix_buffer[work->chain_base_index + 2].elements);
                } else {
                    Vu0SubVector(&chain_axis, &target, &work->chain_slots[0].translation);
                    {
                        float x = chain_axis.x;
                        float y = chain_axis.y;
                        float z = chain_axis.z;
                        chain_axis.x = work->transform_basis[0][0] * x +
                                       work->transform_basis[0][1] * y +
                                       work->transform_basis[0][2] * z;
                        chain_axis.y = work->transform_basis[1][0] * x +
                                       work->transform_basis[1][1] * y +
                                       work->transform_basis[1][2] * z;
                        chain_axis.z = work->transform_basis[2][0] * x +
                                       work->transform_basis[2][1] * y +
                                       work->transform_basis[2][2] * z;
                    }
                    distance = chain_axis.x * chain_axis.x +
                               chain_axis.y * chain_axis.y +
                               chain_axis.z * chain_axis.z;
                    chain_slot = &work->chain_slots[1];
                    alignXAxis(&chain_axis, alignment);
                    MATRIX_transpose3(alignment);
                    CUR_MATRIX_Set(work->transform_basis);
                    CUR_MATRIX_4s3(alignment);
                    CUR_MATRIX_Get(work->transform_basis);
                    if ((chain_slot->attribute->channel_flags & JNT_KEY_LINK_PIVOT) != 0) {
                        Vector4 *pivot = (Vector4 *)work->matrix_buffer[chain_slot->attribute->pivot_index].elements[3];
                        float radius;
                        float x;
                        float y;
                        float z;
                        float norm_squared;
                        pivot->w = 1.0f;
                        chain_axis.x = pivot->x - work->transform_basis[3][0];
                        chain_axis.y = pivot->y - work->transform_basis[3][1];
                        chain_axis.z = pivot->z - work->transform_basis[3][2];
                        x = chain_axis.x;
                        y = chain_axis.y;
                        z = chain_axis.z;
                        chain_axis.x = work->transform_basis[0][0] * x +
                                       work->transform_basis[0][1] * y +
                                       work->transform_basis[0][2] * z;
                        chain_axis.y = work->transform_basis[1][0] * x +
                                       work->transform_basis[1][1] * y +
                                       work->transform_basis[1][2] * z;
                        chain_axis.z = work->transform_basis[2][0] * x +
                                       work->transform_basis[2][1] * y +
                                       work->transform_basis[2][2] * z;
                        radius = __builtin_sqrtf(chain_axis.y * chain_axis.y + chain_axis.z * chain_axis.z);
                        z_component = chain_axis.z;
                        norm_squared = chain_axis.x * chain_axis.x +
                                       chain_axis.y * chain_axis.y + z_component * z_component;
                        __builtin_sqrtf(norm_squared);
                        z_component = chain_axis.z;
                        chain_slot->rotation.z = -xglAtan2(-z_component / radius, chain_axis.y / radius);
                    }
                    for (index = 2; index < work->matrix_index - 1; index++) {
                        float *joint_length = &work->chain_slots[index + 1].translation.x;
                        float remaining_length = work->chain_length - *joint_length;
                        float cosine;
                        float sine;
                        float sine_magnitude;
                        float segment_length;
                        float squared_difference;
                        distance = __builtin_sqrtf(distance);
                        segment_length = *joint_length;
                        if (0.0f <= segment_length) {
                            float segment_squared;
                            float denominator;
                            segment_squared = segment_length * segment_length;
                            squared_difference = segment_squared - remaining_length * remaining_length;
                            denominator = (2.0f * segment_length) * distance;
                            cosine = (distance * distance + squared_difference) / denominator;
                            sine_magnitude = __builtin_sqrtf(1.0f - cosine * cosine);
                            sine = sine_magnitude;
                        } else {
                            float magnitude;
                            float denominator;
                            magnitude = __builtin_fabsf(segment_length);
                            *joint_length = magnitude;
                            squared_difference = magnitude * magnitude - remaining_length * remaining_length;
                            denominator = (2.0f * magnitude) * distance;
                            cosine = (distance * distance + squared_difference) / denominator;
                            sine_magnitude = __builtin_sqrtf(1.0f - cosine * cosine);
                            sine = -sine_magnitude;
                        }
                        if (1.0f < cosine) {
                            cosine = 1.0f;
                            sine = 0.0f;
                        } else if (cosine < -1.0f) {
                            cosine = -1.0f;
                            sine = 0.0f;
                        }
                        chain_angle = xglAtan2(sine, cosine);
                        CUR_MATRIX_Set(work->transform_basis);
                        CUR_MATRIX_Rz4s((const int *)&chain_angle);
                        CUR_MATRIX_Get(work->matrix_buffer[work->chain_base_index + index].elements);
                        CUR_MATRIX_Txyz4s(&work->elements[index].translation);
                        CUR_MATRIX_Get(work->transform_basis);
                    }
                    CUR_MATRIX_GetT(&current_translation);
                    {
                        float *output_y = &chain_axis.y;
                        float *output_z = &chain_axis.z;
                        float x = target.x - current_translation.x;
                        float y = target.y - current_translation.y;
                        z_component = target.z - current_translation.z;
                        chain_axis.x = work->transform_basis[0][0] * x +
                                        work->transform_basis[0][1] * y +
                                        work->transform_basis[0][2] * z_component;
                        chain_axis.w = 1.0f;
                        *output_y = work->transform_basis[1][0] * x +
                                      work->transform_basis[1][1] * y +
                                      work->transform_basis[1][2] * z_component;
                        *output_z = work->transform_basis[2][0] * x +
                                      work->transform_basis[2][1] * y +
                                      work->transform_basis[2][2] * z_component;
                    }
                    distance = chain_axis.x * chain_axis.x +
                               chain_axis.y * chain_axis.y +
                               chain_axis.z * chain_axis.z;
                    alignXAxis(&chain_axis, alignment);
                    MATRIX_transpose3(alignment);
                    CUR_MATRIX_Set(work->transform_basis);
                    CUR_MATRIX_4s3(alignment);
                    CUR_MATRIX_Get(work->matrix_buffer[work->chain_base_index + index].elements);
                    CUR_MATRIX_Txyz4s(&work->chain_slots[index + 1].translation);
                    CUR_MATRIX_Get(work->matrix_buffer[work->current_index].elements);
                }
            }
            break;
        }
        case 5:
            CUR_MATRIX_Set(work->matrix_buffer[channel->matrix_index].elements);
            CUR_MATRIX_Txyz4s(&element->translation);
            CUR_MATRIX_Rzyx4s((const int *)element->rotation);
            CUR_MATRIX_Sxyz4s(&element->scale);
            CUR_MATRIX_Get(work->matrix_buffer[work->current_index].elements);
            break;
        case 6:
            break;
        }
        if ((flags & JNT_CHANNEL_FLAG_INHERIT) != 0) {
            unsigned short channel_index;
            xglMatrixUnit(work->root_correction);
            channel_index = work->channel_index;
            if (work->current_index >= 4) {
                float (*current)[4] = work->matrix_buffer[work->current_index].elements;
                JntStaticChannel *channels = work->channels;
                current[3][1] *= (channels[work->current_index - 1].translation.y +
                                  work->matrix_buffer[0].elements[3][1]) *
                                 (1.0f / work->matrix_buffer[work->current_index - 1].elements[3][1]);
            }
            if ((channel_index & 7) == 0) {
                unsigned short parent_index = channel->matrix_index;
                float (*current)[4] =
                    jntMatrixAt(work->matrix_buffer, work->current_index)->elements;
                float (*parent)[4] =
                    jntMatrixAt(work->matrix_buffer, parent_index)->elements;
                work->root_correction[3][0] = -(current[3][0] - parent[3][0]);
                work->root_correction[3][1] = -(current[3][1] - parent[3][1]);
                work->root_correction[3][2] = -(current[3][2] - parent[3][2]);
                current[3][0] = parent[3][0];
                current[3][1] = parent[3][1];
                current[3][2] = parent[3][2];
            }
        }
        work->current_index++;
        return next_channel;
    }
    work->current_index++;
    return next_channel;
}

/* Builds the channel's static matrix into matrix_buffer and advances the element index. */
void *JNT_getStaticVal2(JntWork *work, JntStaticChannel *channel)
{
    int type;
    const Vector4 *translate;
    const int *rotate;
    void *next;
    void *matrix;

    type = channel->type;
    translate = &channel->translation;
    rotate = channel->rotate.bits;
    next = channel + 1;

    switch (type) {
    case 5:
        matrix = &work->matrix_buffer[channel->matrix_index];
        CUR_MATRIX_Set(matrix);
        CUR_MATRIX_Txyz4s(translate);
        CUR_MATRIX_Rzyx4s(rotate);
        matrix = &work->matrix_buffer[work->current_index];
        CUR_MATRIX_Get(matrix);
        break;
    case 1:
    case 2:
    case 3:
    case 4:
        matrix = &work->matrix_buffer[channel->matrix_index];
        CUR_MATRIX_Set(matrix);
        CUR_MATRIX_Txyz4s(translate);
        CUR_MATRIX_Rzyx4s(rotate);
        matrix = &work->matrix_buffer[work->current_index];
        CUR_MATRIX_Get(matrix);
        break;
    case 0:
    default:
        break;
    }

    work->current_index++;
    return next;
}

/*
 * Samples the current channel from the joint pack for the matrix path and
 * stores its matrix in matrix_buffer. Channels with the inherit flag also
 * update the root translation, rotation and scale.
 */
JntStaticChannel *JNT_getVal2(JntWork *work, JntStaticChannel *channel, float frame) {
    Vector4 *translation = &channel->translation;
    JntChainSlot *slot;
    float *rotate = channel->rotate.values;
    float root_len;
    float last_len;
    struct FcvPackAttribute *attribute;
    JntStaticChannel *next_channel;
    unsigned int value_mask;
    unsigned int channel_flags;
    int has_fcv;
    int index;
    Vector4 sums_pair[2];
    Matrix4 basis_frame;
    Vector4 axis_a;
    Vector4 mid_b;
    Vector4 axis_c;
    Vector4 tip_c;
    float *ap = &axis_a.y;
    float *terminal_axis_tail = &axis_c.y;

    sums_pair[1].w = 1.0f;
    sums_pair[0].w = 1.0f;

    if (work->flags & JNT_WORK_PACK_ENDED) {
        attribute = (struct FcvPackAttribute *)dummyAttr;
        value_mask = attribute->value_mask;
        channel_flags = attribute->channel_flags;
    } else {
        attribute = FCV2_getPackAttribute(&work->cursor);
        if (attribute->channel_flags == JNT_CHANNEL_FLAGS_END) {
            work->flags |= JNT_WORK_PACK_ENDED;
            return channel;
        }
        value_mask = attribute->value_mask;
        channel_flags = attribute->channel_flags;
    }
    has_fcv = channel_flags & JNT_CHANNEL_FLAG_INHERIT;
    next_channel = channel + 1;
    if (has_fcv != 0) {
        get_fcvpack(&work->channel_translation.x, &translation->x, work, value_mask, frame);
        value_mask >>= 3;
        get_fcvpack(&work->channel_rotation.x, rotate, work, value_mask, frame);
        value_mask >>= 3;
        get_fcvpack_fix(&work->channel_scale.x, 1.0f, work, value_mask, frame);
        get_fcvpack_flag(work, value_mask >> 1, frame);
        value_mask = 0;
        work->root_translation.x = work->channel_translation.x;
        work->root_translation.y = work->channel_translation.y;
        work->root_translation.z = work->channel_translation.z;
        work->root_rotation.x = work->channel_rotation.z;
        work->root_rotation.y = work->channel_rotation.y;
        work->root_rotation.z = work->channel_rotation.x;
        work->root_scale.x = work->channel_scale.x;
        work->root_scale.y = work->channel_scale.y;
        work->root_scale.z = work->channel_scale.z;
    }
    /* Each case builds this channel's part of the chain by type: 1 direct, 3 chain root, 2 chain link, 4 chain end, 5 direct with pack values. */
    switch (channel->type) {
    case 0:
        break;
    case 1: {
        JntRotationLimitHeader *limit;
        int limit_offset;
        int *slot_count;
        int slot;
        int link;

        CUR_MATRIX_Set((work->matrix_buffer + channel->matrix_index)->elements);
        work->channel_translation.w = 1.0f;
        work->channel_scale.w = 1.0f;
        if (has_fcv == 0) {
            get_fcvpack(&work->channel_translation.x, &translation->x, work, value_mask, frame);
            value_mask >>= 3;
            get_fcvpack(&work->channel_rotation.x, rotate, work, value_mask, frame);
            value_mask >>= 3;
            for (index = 0; index < 3; index++) {
                if ((value_mask & (1 << index)) != 0) {
                    ((float *)&work->channel_scale)[index] = FCV2_getPackValue(&work->cursor, frame);
                }
            }
        }
        if ((channel_flags & JNT_KEY_LINK_MASK) != 0) {
            if (channel_flags & JNT_KEY_LINK_TRANSLATION) {
                link = attribute->translation_index;
                if (link < work->current_index) {
                    CUR_MATRIX_SetT((const Vector4 *)(work->matrix_buffer + link)->elements[3]);
                }
            } else {
                CUR_MATRIX_Txyz4s(&work->channel_translation);
            }
            if (channel_flags & JNT_KEY_LINK_ROTATION) {
                link = attribute->rotation_index;
                if (link < work->current_index) {
                    CUR_MATRIX_SetR((work->matrix_buffer + link)->elements);
                }
            } else {
                CUR_MATRIX_Rzyx4s((const int *)&work->channel_rotation);
                CUR_MATRIX_Sxyz4s(&work->channel_scale);
            }
        } else {
            CUR_MATRIX_Txyz4s(&work->channel_translation);
            CUR_MATRIX_Rzyx4s((const int *)&work->channel_rotation);
        }
        limit_offset = channel->rotation_limits_offset;
        if (limit_offset != 0) {
            limit = (JntRotationLimitHeader *)((unsigned char *)channel + limit_offset);
            if (limit->deferred_offset != 0) {
                slot_count = &work->producer->deferred_count;
                slot = (*slot_count)++;
                work->producer->deferred_limits[slot] = (struct ActorShapeBinding *)
                    ((unsigned char *)limit + limit->deferred_offset);
                if (channel_flags & JNT_KEY_LINK_TRANSLATION) {
                    if (attribute->translation_index < work->current_index) {
                        CUR_MATRIX_SetT(&work->channel_translation);
                    }
                } else {
                    CUR_MATRIX_SetT(&work->channel_translation);
                }
            }
        }
        CUR_MATRIX_Get((work->matrix_buffer + work->current_index)->elements);
        break;
    }
    case 3: {
        CUR_MATRIX_Set((work->matrix_buffer + channel->matrix_index)->elements);
        work->chain_base_index = (short)work->current_index;
        get_fcvpack(&work->channel_translation.x, &translation->x, work, value_mask, frame);
        get_fcvpack(&work->channel_rotation.x, rotate, work, value_mask >> 3, frame);
        CUR_MATRIX_Txyz4s(&work->channel_translation);
        CUR_MATRIX_Rzyx4s((const int *)&work->channel_rotation);
        CUR_MATRIX_Get((float (*)[4])work->transform_basis);
        CUR_MATRIX_Get((work->matrix_buffer + work->current_index)->elements);
        work->chain_length = 0.0f;
        {
            Vector4 *root_translation = &jntChainAt(work, 0)->translation;

            *root_translation = *(Vector4 *)work->transform_basis[3];
            root_translation->w = 1.0f;
        }
        work->matrix_index = 1;
        break;
    }
    case 2: {
        float *rotation;
        JntRotationLimitHeader *limit;
        JntRotationLimits *bounds;
        float *upper;
        unsigned short limit_count;
        unsigned short mask;
        int limit_offset;

        slot = jntChainAt(work, work->matrix_index);
        rotation = &slot->rotation.x;
        value_mask >>= 3;
        get_fcvpack(rotation, rotate, work, value_mask, frame);
        get_fcvpack_fix(&slot->scale.x, 1.0f, work, value_mask >> 3, frame);
        limit_offset = channel->rotation_limits_offset;
        if (limit_offset != 0) {
            limit = (JntRotationLimitHeader *)((unsigned char *)channel + limit_offset);
            limit_count = limit->values_offset;
            if (limit_count != 0) {
                bounds = (JntRotationLimits *)((unsigned char *)limit + limit_count);
                upper = bounds->minimum;
                mask = bounds->flags;
                for (index = 0; index < 3; index++) {
                    if (((mask >> index) & 1) && rotation[index] < upper[index]) {
                        rotation[index] = upper[index];
                    }
                }
                upper = bounds->maximum;
                mask >>= 3;
                for (index = 0; index < 3; index++) {
                    if (((mask >> index) & 1) && rotation[index] > upper[index]) {
                        rotation[index] = upper[index];
                    }
                }
            }
        }
        work->chain_length += translation->x;
        slot->attribute = attribute;
        {
            Vector4 *slot_translation = &slot->translation;

            *slot_translation = *translation;
            slot_translation->w = 1.0f;
        }
        slot->scale.w = 1.0f;
        if (0.0f <= channel->translation_direction) {
            slot->translation.x = -translation->x;
        }
        work->matrix_index++;
        break;
    }
    /* Chain end: when the target joint is already built, solves the chain toward it. */
    case 4: {
        float *source;
        int computed = 0;
        float dz;
        unsigned short cf = attribute->channel_flags;
        if ((cf & 0xe) != 0 && (cf & 4) != 0 &&
            attribute->translation_index < work->current_index) {
            float p0;
            float p1;
            float p2;

            computed = 1;
            source = (work->matrix_buffer + attribute->translation_index)->elements[3];
            p0 = source[0];
            sums_pair[1].x = p0;
            p1 = source[1];
            sums_pair[1].y = p1;
            p2 = source[2];
            sums_pair[1].z = p2;
            sums_pair[1].x = ((p0 * work->root_correction[0][0] +
                        p1 * work->root_correction[1][0]) +
                       p2 * work->root_correction[2][0]) +
                      work->root_correction[3][0];
            sums_pair[1].y = ((p0 * work->root_correction[0][1] +
                        p1 * work->root_correction[1][1]) +
                       p2 * work->root_correction[2][1]) +
                      work->root_correction[3][1];
            sums_pair[1].z = ((p0 * work->root_correction[0][2] +
                        p1 * work->root_correction[1][2]) +
                       p2 * work->root_correction[2][2]) +
                      work->root_correction[3][2];
        }
        if (!computed) {
            Vector4 *slot_translation;

            CUR_MATRIX_Set(work->transform_basis);
            for (index = 1; index <= work->matrix_index - 1; index++) {
                struct FcvPackAttribute *slot_attribute;

                slot = jntChainAt(work, index);
                slot->translation.x = __builtin_fabsf(slot->translation.x);
                slot->translation.w = 1.0f;
                slot_attribute = slot->attribute;
                if ((slot_attribute->channel_flags & JNT_KEY_LINK_ROTATION) != 0) {
                    int rotation_index = slot_attribute->rotation_index;

                    if (rotation_index < work->current_index) {
                        JntMatrix *rotation_matrix = work->matrix_buffer + rotation_index;

                        CUR_MATRIX_Txyz4s(&slot->translation);
                        CUR_MATRIX_SetR(rotation_matrix->elements);
                        CUR_MATRIX_Get((work->matrix_buffer + (work->chain_base_index + index))->elements);
                    }
                } else {
                    slot->translation.x = __builtin_fabsf(slot->translation.x);
                    CUR_MATRIX_Txyz4s(&slot->translation);
                    CUR_MATRIX_Rzyx4s((const int *)&slot->rotation);
                    CUR_MATRIX_Sxyz4s(&slot->scale);
                    CUR_MATRIX_Get((work->matrix_buffer + (work->chain_base_index + index))->elements);
                }
            }
            slot_translation = &jntChainAt(work, index)->translation;
            *slot_translation = *translation;
            slot_translation->w = 1.0f;
            CUR_MATRIX_Txyz4s(slot_translation);
            CUR_MATRIX_Get((work->matrix_buffer + work->current_index)->elements);
        } else {
            Vector4 *slot_translation;

            slot = jntChainAt(work, work->matrix_index);
            slot_translation = &slot->translation;
            work->matrix_index++;
            *slot_translation = *translation;
            slot_translation->w = 1.0f;
            if (work->matrix_index == 4) {
                float lane_x;
                float lane_y;
                float lane_z;

                /* sums_pair[1] minus slot 0's translation, as one VU0 vector subtract. */
                Vu0SubVector(&axis_a, &sums_pair[1], &jntChainAt(work, 0)->translation);
                lane_x = axis_a.x;
                lane_y = axis_a.y;
                lane_z = axis_a.z;
                axis_a.x = work->transform_basis[0][0] * lane_x +
                           work->transform_basis[0][1] * lane_y +
                           work->transform_basis[0][2] * lane_z;
                axis_a.y = work->transform_basis[1][0] * lane_x +
                           work->transform_basis[1][1] * lane_y +
                           work->transform_basis[1][2] * lane_z;
                axis_a.z = work->transform_basis[2][0] * lane_x +
                           work->transform_basis[2][1] * lane_y +
                           work->transform_basis[2][2] * lane_z;
                root_len = (axis_a.x * axis_a.x + axis_a.y * axis_a.y) + axis_a.z * axis_a.z;
                alignXAxis(&axis_a, basis_frame);
                MATRIX_transpose3(basis_frame);
                CUR_MATRIX_Set(work->transform_basis);
                CUR_MATRIX_4s3(basis_frame);
                CUR_MATRIX_Get(work->transform_basis);
                if ((jntChainAt(work, 1)->attribute->channel_flags & JNT_KEY_LINK_PIVOT) != 0) {
                    float *pivot;
                    float len;
                    float angle;

                    pivot = (work->matrix_buffer +
                             jntChainAt(work, 1)->attribute->pivot_index)->elements[3];
                    pivot[3] = 1.0f;
                    axis_a.x = pivot[0] - work->transform_basis[3][0];
                    axis_a.y = pivot[1] - work->transform_basis[3][1];
                    axis_a.z = pivot[2] - work->transform_basis[3][2];
                    lane_x = axis_a.x;
                    lane_y = axis_a.y;
                    lane_z = axis_a.z;
                    axis_a.x = (work->transform_basis[0][0] * lane_x + work->transform_basis[0][1] * lane_y) +
                               work->transform_basis[0][2] * lane_z;
                    axis_a.y = (work->transform_basis[1][0] * lane_x + work->transform_basis[1][1] * lane_y) +
                               work->transform_basis[1][2] * lane_z;
                    axis_a.z = (work->transform_basis[2][0] * lane_x + work->transform_basis[2][1] * lane_y) +
                               work->transform_basis[2][2] * lane_z;
                    len = __builtin_sqrtf(axis_a.y * axis_a.y + axis_a.z * axis_a.z);
                    __builtin_sqrtf(axis_a.x * axis_a.x + axis_a.y * axis_a.y + axis_a.z * axis_a.z);
                    angle = -xglAtan2(-axis_a.z / len, axis_a.y / len);
                    CUR_MATRIX_Set(work->transform_basis);
                    CUR_MATRIX_Rx4s((const int *)&angle);
                    CUR_MATRIX_Sxyz4s(&jntChainAt(work, 1)->scale);
                    CUR_MATRIX_Get(work->transform_basis);
                } else {
                    CUR_MATRIX_Rx4s((const int *)&jntChainAt(work, 1)->rotation.z);
                    CUR_MATRIX_Sxyz4s(&jntChainAt(work, 1)->scale);
                    CUR_MATRIX_Set(work->transform_basis);
                }
                {
                    float lane_x;
                    float lane_y;
                    float lane_z;
                    float a_ref;
                    float a_abs;
                    float cosine;
                    float diff;
                    float sine;
                    float yv;
                    float ang;
                    Vector4 *upper_joint = &jntChainAt(work, 2)->translation;
                    Vector4 *lower_joint = &jntChainAt(work, 3)->translation;

                    root_len = __builtin_sqrtf(root_len);
                    a_ref = upper_joint->x;
                    if (0.0f <= a_ref) {
                        diff = a_ref * a_ref - lower_joint->x * lower_joint->x;
                        cosine = (root_len * root_len + diff) / ((a_ref + a_ref) * root_len);
                        sine = __builtin_sqrtf(1.0f - cosine * cosine);
                        yv = sine;
                    } else {
                        a_abs = __builtin_fabsf(a_ref);
                        upper_joint->x = a_abs;
                        diff = a_abs * a_abs - lower_joint->x * lower_joint->x;
                        cosine = (root_len * root_len + diff) / ((a_abs + a_abs) * root_len);
                        sine = __builtin_sqrtf(1.0f - cosine * cosine);
                        yv = -sine;
                    }
                    if (1.0f < cosine) {
                        cosine = 1.0f;
                        yv = 0.0f;
                    } else if (cosine < -1.0f) {
                        cosine = -1.0f;
                        yv = 0.0f;
                    }
                    ang = xglAtan2(yv, cosine);
                    CUR_MATRIX_Set(work->transform_basis);
                    CUR_MATRIX_Rz4s((const int *)&ang);
                    CUR_MATRIX_Get((work->matrix_buffer + work->chain_base_index + 1)->elements);
                    CUR_MATRIX_Txyz4s(&jntChainAt(work, 2)->translation);
                    CUR_MATRIX_Get(work->transform_basis);
                    CUR_MATRIX_GetT(&mid_b);
                    /* sums_pair[1] minus mid_b (the translation just read back), as one VU0 vector subtract. */
                    Vu0SubVector(&axis_a, &sums_pair[1], &mid_b);
                    lane_x = axis_a.x;
                    lane_y = axis_a.y;
                    lane_z = axis_a.z;
                    axis_a.w = 1.0f;
                    axis_a.x = (work->transform_basis[0][0] * lane_x + work->transform_basis[0][1] * lane_y) +
                               work->transform_basis[0][2] * lane_z;
                    ap[0] = (work->transform_basis[1][0] * lane_x + work->transform_basis[1][1] * lane_y) +
                               work->transform_basis[1][2] * lane_z;
                    ap[1] = (work->transform_basis[2][0] * lane_x + work->transform_basis[2][1] * lane_y) +
                               work->transform_basis[2][2] * lane_z;
                    alignXAxis(&axis_a, basis_frame);
                    MATRIX_transpose3(basis_frame);
                    CUR_MATRIX_Set(work->transform_basis);
                    CUR_MATRIX_4s3(basis_frame);
                    CUR_MATRIX_Get((work->matrix_buffer + work->chain_base_index + 2)->elements);
                    CUR_MATRIX_Txyz4s(&jntChainAt(work, 3)->translation);
                    CUR_MATRIX_Get((work->matrix_buffer + work->current_index)->elements);
                }
            } else if (work->matrix_index == 3) {
                float dx = sums_pair[1].x - jntChainAt(work, 0)->translation.x;
                float dy = sums_pair[1].y - jntChainAt(work, 0)->translation.y;
                dz = sums_pair[1].z - jntChainAt(work, 0)->translation.z;

                mid_b.x = work->transform_basis[0][0] * dx +
                          work->transform_basis[0][1] * dy +
                          work->transform_basis[0][2] * dz;
                mid_b.y = work->transform_basis[1][0] * dx +
                          work->transform_basis[1][1] * dy +
                          work->transform_basis[1][2] * dz;
                mid_b.z = work->transform_basis[2][0] * dx +
                          work->transform_basis[2][1] * dy +
                          work->transform_basis[2][2] * dz;
                alignXAxis(&mid_b, basis_frame);
                MATRIX_transpose3(basis_frame);
                CUR_MATRIX_Set(work->transform_basis);
                CUR_MATRIX_4s3(basis_frame);
                CUR_MATRIX_Rx4s((const int *)&jntChainAt(work, 1)->rotation.z);
                CUR_MATRIX_Sxyz4s(&jntChainAt(work, 1)->scale);
                CUR_MATRIX_Get((work->matrix_buffer + work->chain_base_index + 1)->elements);
                CUR_MATRIX_Txyz4s(&jntChainAt(work, 2)->translation);
                CUR_MATRIX_Get((work->matrix_buffer + work->chain_base_index + 2)->elements);
            } else {
                Vector4 *cp = &axis_c;
                float lane_x;
                float lane_y;
                float lane_z;
                float direction_z;

                /* sums_pair[1] minus slot 0's translation, as one VU0 vector subtract. */
                Vu0SubVector(cp, &sums_pair[1], &jntChainAt(work, 0)->translation);
                lane_x = axis_c.x;
                lane_y = axis_c.y;
                lane_z = axis_c.z;
                axis_c.x = work->transform_basis[0][0] * lane_x +
                           work->transform_basis[0][1] * lane_y +
                           work->transform_basis[0][2] * lane_z;
                axis_c.y = work->transform_basis[1][0] * lane_x +
                           work->transform_basis[1][1] * lane_y +
                           work->transform_basis[1][2] * lane_z;
                direction_z = work->transform_basis[2][0] * lane_x +
                            work->transform_basis[2][1] * lane_y +
                            work->transform_basis[2][2] * lane_z;
                axis_c.z = direction_z;
                root_len = (axis_c.x * axis_c.x + axis_c.y * axis_c.y) + axis_c.z * axis_c.z;
                alignXAxis(cp, basis_frame);
                MATRIX_transpose3(basis_frame);
                CUR_MATRIX_Set(work->transform_basis);
                CUR_MATRIX_4s3(basis_frame);
                CUR_MATRIX_Get(work->transform_basis);
                slot = jntChainAt(work, 1);
                if ((slot->attribute->channel_flags & JNT_KEY_LINK_PIVOT) != 0) {
                    float *pivot;
                    float len;

                    pivot = (work->matrix_buffer +
                             slot->attribute->pivot_index)->elements[3];
                    pivot[3] = 1.0f;
                    axis_c.x = pivot[0] - work->transform_basis[3][0];
                    axis_c.y = pivot[1] - work->transform_basis[3][1];
                    axis_c.z = pivot[2] - work->transform_basis[3][2];
                    lane_x = axis_c.x;
                    lane_y = axis_c.y;
                    lane_z = axis_c.z;
                    axis_c.x = (work->transform_basis[0][0] * lane_x + work->transform_basis[0][1] * lane_y) +
                               work->transform_basis[0][2] * lane_z;
                    axis_c.y = (work->transform_basis[1][0] * lane_x + work->transform_basis[1][1] * lane_y) +
                               work->transform_basis[1][2] * lane_z;
                    axis_c.z = (work->transform_basis[2][0] * lane_x + work->transform_basis[2][1] * lane_y) +
                               work->transform_basis[2][2] * lane_z;
                    direction_z = axis_c.y * axis_c.y + axis_c.z * axis_c.z;
                    len = __builtin_sqrtf(direction_z);
                    __builtin_sqrtf(axis_c.x * axis_c.x + axis_c.y * axis_c.y + axis_c.z * axis_c.z);
                    slot->rotation.z =
                        -xglAtan2(-axis_c.z / len, axis_c.y / len);
                }
                for (index = 2; index < work->matrix_index - 1; index++) {
                    float a_val;
                    float a_abs;
                    float b_val;
                    float diff;
                    float cosine;
                    float sine;
                    float yv;
                    float ang;
                    Vector4 *joint_translation = &jntChainAt(work, index + 1)->translation;

                    b_val = work->chain_length - joint_translation->x;
                    root_len = __builtin_sqrtf(root_len);
                    a_val = joint_translation->x;
                    if (0.0f <= a_val) {
                        diff = a_val * a_val - b_val * b_val;
                        cosine = (root_len * root_len + diff) / ((a_val + a_val) * root_len);
                        sine = __builtin_sqrtf(1.0f - cosine * cosine);
                        yv = sine;
                    } else {
                        a_abs = __builtin_fabsf(a_val);
                        joint_translation->x = a_abs;
                        diff = a_abs * a_abs - b_val * b_val;
                        cosine = (root_len * root_len + diff) / ((a_abs + a_abs) * root_len);
                        sine = __builtin_sqrtf(1.0f - cosine * cosine);
                        yv = -sine;
                    }
                    if (1.0f < cosine) {
                        cosine = 1.0f;
                        yv = 0.0f;
                    } else if (cosine < -1.0f) {
                        cosine = -1.0f;
                        yv = 0.0f;
                    }
                    ang = xglAtan2(yv, cosine);
                    CUR_MATRIX_Set(work->transform_basis);
                    CUR_MATRIX_Rz4s((const int *)&ang);
                    CUR_MATRIX_Get((work->matrix_buffer + (work->chain_base_index + index))->elements);
                    CUR_MATRIX_Txyz4s(&jntChainAt(work, index)->translation);
                    CUR_MATRIX_Get(work->transform_basis);
                }
                last_len = root_len;
                CUR_MATRIX_GetT(&tip_c);
                {
                    float tip_dx = sums_pair[1].x - tip_c.x;
                    float tip_dy = sums_pair[1].y - tip_c.y;
                    direction_z = sums_pair[1].z - tip_c.z;

                    axis_c.x = (work->transform_basis[0][0] * tip_dx + work->transform_basis[0][1] * tip_dy) +
                               work->transform_basis[0][2] * direction_z;
                    last_len = 1.0f;
                    axis_c.w = last_len;
                    terminal_axis_tail[0] = (work->transform_basis[1][0] * tip_dx + work->transform_basis[1][1] * tip_dy) +
                               work->transform_basis[1][2] * direction_z;
                    terminal_axis_tail[1] = (work->transform_basis[2][0] * tip_dx + work->transform_basis[2][1] * tip_dy) +
                               work->transform_basis[2][2] * direction_z;
                }
                alignXAxis(cp, basis_frame);
                MATRIX_transpose3(basis_frame);
                CUR_MATRIX_Set(work->transform_basis);
                CUR_MATRIX_4s3(basis_frame);
                CUR_MATRIX_Get((work->matrix_buffer + (work->chain_base_index + index))->elements);
                CUR_MATRIX_Txyz4s(&jntChainAt(work, index + 1)->translation);
                CUR_MATRIX_Get((work->matrix_buffer + work->current_index)->elements);
            }
        }
        break;
    }
    case 5:
        CUR_MATRIX_Set((work->matrix_buffer + channel->matrix_index)->elements);
        get_fcvpack(&work->channel_translation.x, &translation->x, work, value_mask, frame);
        value_mask >>= 3;
        get_fcvpack(&work->channel_rotation.x, rotate, work, value_mask, frame);
        get_fcvpack_fix(&work->channel_scale.x, 1.0f, work, value_mask >> 3, frame);
        CUR_MATRIX_Txyz4s(&work->channel_translation);
        CUR_MATRIX_Rzyx4s((const int *)&work->channel_rotation);
        CUR_MATRIX_Sxyz4s(&work->channel_scale);
        CUR_MATRIX_Get((work->matrix_buffer + work->current_index)->elements);
        break;
    case 6:
        break;
    }
    /* Inherited channels snap this matrix's translation onto its parent and keep the offset in root_correction. */
    if (has_fcv != 0) {
        xglMatrixUnit(work->root_correction);
        value_mask = work->channel_index;
        if ((value_mask & 7) == 0) {
            float *dest = (float *)(work->current_index * sizeof(JntMatrix) + (int)work->matrix_buffer);
            float *source = (float *)(channel->matrix_index * sizeof(JntMatrix) + (int)work->matrix_buffer);

            work->root_correction[3][0] = -(dest[12] - source[12]);
            work->root_correction[3][1] = -(dest[13] - source[13]);
            work->root_correction[3][2] = -(dest[14] - source[14]);
            dest[12] = source[12];
            dest[13] = source[13];
            dest[14] = source[14];
        }
    }
    work->current_index++;
    return next_channel;
}

/*
 * Matrix-path sampler, a variant of JNT_getVal2 that falls back to the static
 * channel's translation when a pack value is unset.
 */
JntStaticChannel *JNT_getVal_HOGE2(JntWork *work, JntStaticChannel *channel, float frame) {
    Vector4 displacement;
    Vector4 sums;
    float *translate = &channel->translation.x;
    float *rotate = channel->rotate.values;
    struct FcvPackAttribute *attribute;
    Vector4 *decoded_translation;
    JntStaticChannel *next_channel;
    unsigned int value_mask;
    int index;
    sums.w = 1.0f;
    displacement.w = 1.0f;
    next_channel = channel + 1;
    attribute = FCV2_getPackAttribute(&work->cursor);
    value_mask = attribute->value_mask;
    if ((attribute->channel_flags & JNT_CHANNEL_FLAG_INHERIT) != 0) {
        decoded_translation = &work->channel_translation;
        get_fcvpack(&decoded_translation->x, translate, work, value_mask, frame);
        value_mask >>= 3;
        get_fcvpack(&work->channel_rotation.x, rotate, work, value_mask, frame);
        value_mask >>= 3;
        get_fcvpack_fix(&work->channel_scale.x, 1.0f, work, value_mask, frame);
        get_fcvpack_flag(work, value_mask >> 1, frame);
        value_mask = 0;
        work->root_translation.x = work->channel_translation.x;
        work->root_translation.y = work->channel_translation.y;
        work->root_translation.z = work->channel_translation.z;
        work->root_rotation.x = work->channel_rotation.z;
        work->root_rotation.y = work->channel_rotation.y;
        work->root_rotation.z = work->channel_rotation.x;
        work->root_scale.x = work->channel_scale.x;
        work->root_scale.y = work->channel_scale.y;
        work->root_scale.z = work->channel_scale.z;
    }
    switch (channel->type) {
    case 0:
    case 6:
        break;
    case 1: {
        JntRotationLimitHeader *limit;
        int limit_offset;
        int *slot_count;
        int slot;
        int link;

        CUR_MATRIX_Set((work->matrix_buffer + channel->matrix_index)->elements);
        work->channel_translation.w = 1.0f;
        work->channel_scale.w = 1.0f;
        for (index = 0; index < 3; index++) {
            if ((value_mask & (1 << index)) != 0) {
                (&work->channel_translation.x)[index] = FCV2_getPackValue(&work->cursor, frame);
            }
        }
        value_mask >>= 3;
        for (index = 0; index < 3; index++) {
            if ((value_mask & (1 << index)) != 0) {
                (&work->channel_rotation.x)[index] = FCV2_getPackValue(&work->cursor, frame);
            }
        }
        value_mask >>= 3;
        for (index = 0; index < 3; index++) {
            if ((value_mask & (1 << index)) != 0) {
                (&work->channel_scale.x)[index] = FCV2_getPackValue(&work->cursor, frame);
            }
        }
        if ((attribute->channel_flags & JNT_KEY_LINK_MASK) != 0) {
            if (attribute->channel_flags & JNT_KEY_LINK_TRANSLATION) {
                link = attribute->translation_index;
                if (link < work->current_index) {
                    CUR_MATRIX_SetT((const Vector4 *)(work->matrix_buffer + link)->elements[3]);
                }
            } else {
                CUR_MATRIX_Txyz4s(&work->channel_translation);
            }
            if (attribute->channel_flags & JNT_KEY_LINK_ROTATION) {
                link = attribute->rotation_index;
                if (link < work->current_index) {
                    CUR_MATRIX_SetR((work->matrix_buffer + link)->elements);
                }
            } else {
                CUR_MATRIX_Rzyx4s((const int *)&work->channel_rotation);
                CUR_MATRIX_Sxyz4s(&work->channel_scale);
            }
        } else {
            CUR_MATRIX_Txyz4s(&work->channel_translation);
            CUR_MATRIX_Rzyx4s((const int *)&work->channel_rotation);
            CUR_MATRIX_Sxyz4s(&work->channel_scale);
        }
        limit_offset = channel->rotation_limits_offset;
        if (limit_offset != 0) {
            limit = (JntRotationLimitHeader *)((unsigned char *)channel + limit_offset);
            if (limit->deferred_offset != 0) {
                slot_count = &work->producer->deferred_count;
                slot = (*slot_count)++;
                work->producer->deferred_limits[slot] = (struct ActorShapeBinding *)
                    ((unsigned char *)limit + limit->deferred_offset);
                if (attribute->channel_flags & JNT_KEY_LINK_TRANSLATION) {
                    if (attribute->translation_index < work->current_index) {
                        CUR_MATRIX_SetT(&work->channel_translation);
                    }
                } else {
                    CUR_MATRIX_SetT(&work->channel_translation);
                }
            }
        }
        CUR_MATRIX_Get((work->matrix_buffer + work->current_index)->elements);
        break;
    }
    case 3: {
        decoded_translation = &work->channel_translation;
        CUR_MATRIX_Set((work->matrix_buffer + channel->matrix_index)->elements);
        work->chain_base_index = (short)work->current_index;
        for (index = 0; index < 3; index++) {
            if ((value_mask & (1 << index)) != 0) {
                (&decoded_translation->x)[index] = FCV2_getPackValue(&work->cursor, frame);
                {
                    const float *fallback_sample = translate;

                    fallback_sample += index;
                    (&decoded_translation->x)[index] = *fallback_sample;
                }
            } else {
                (&decoded_translation->x)[index] = 0.0f;
            }
        }
        get_fcvpack_fix(&work->channel_rotation.x, 0.0f, work, value_mask >> 3, frame);
        CUR_MATRIX_Txyz4s(decoded_translation);
        CUR_MATRIX_Rzyx4s((const int *)&work->channel_rotation);
        CUR_MATRIX_Get((float (*)[4])work->transform_basis);
        CUR_MATRIX_Get((work->matrix_buffer + work->current_index)->elements);
        {
            Vector4 *root_translation = &jntChainAt(work, 0)->translation;

            *root_translation = *(Vector4 *)work->transform_basis[3];
            work->matrix_index = 1;
            root_translation->w = 1.0f;
        }
        break;
    }
    case 2: {
        JntChainSlot *slot;
        float *rotation;
        JntRotationLimitHeader *limit;
        JntRotationLimits *bounds;
        float *upper;
        unsigned short limit_count;
        unsigned short mask;
        int limit_offset;

        slot = jntChainAt(work, work->matrix_index);
        rotation = &slot->rotation.x;
        value_mask >>= 3;
        get_fcvpack_fix(rotation, 0.0f, work, value_mask, frame);
        get_fcvpack_fix(&slot->scale.x, 0.0f, work, value_mask >> 3, frame);
        limit_offset = channel->rotation_limits_offset;
        if (limit_offset != 0) {
            limit = (JntRotationLimitHeader *)((unsigned char *)channel + limit_offset);
            limit_count = limit->values_offset;
            if (limit_count != 0) {
                bounds = (JntRotationLimits *)((unsigned char *)limit + limit_count);
                upper = bounds->minimum;
                mask = bounds->flags;
                for (index = 0; index < 3; index++) {
                    if (((mask >> index) & 1) && rotation[index] < upper[index]) {
                        rotation[index] = upper[index];
                    }
                }
                upper = bounds->maximum;
                mask >>= 3;
                for (index = 0; index < 3; index++) {
                    if (((mask >> index) & 1) && rotation[index] > upper[index]) {
                        rotation[index] = upper[index];
                    }
                }
            }
        }
        {
            Vector4 *slot_translation = &slot->translation;

            __builtin_memcpy(slot_translation, translate, sizeof(*slot_translation));
            slot_translation->w = 1.0f;
        }
        slot->attribute = attribute;
        slot->scale.w = 1.0f;
        if (channel->translation_direction >= 0.0f) {
            slot->translation.x = -translate[0];
        }
        work->matrix_index++;
        break;
    }
    case 4: {
        float root_len;
        Matrix4 basis_frame;
        Vector4 axis_a;
        Vector4 mid_b;
        float *source;
        int computed = 0;
        float dz;
        float a_abs;
        if ((attribute->channel_flags & JNT_KEY_LINK_MASK) != 0 && (attribute->channel_flags & JNT_KEY_LINK_TRANSLATION) != 0 &&
            attribute->translation_index < work->current_index) {
            float p0;
            float p1;
            float p2;

            computed = 1;
            source = (work->matrix_buffer + attribute->translation_index)->elements[3];
            p0 = source[0];
            sums.x = p0;
            p1 = source[1];
            sums.y = p1;
            p2 = source[2];
            sums.z = p2;
            sums.x = ((p0 * work->root_correction[0][0] +
                        p1 * work->root_correction[1][0]) +
                       p2 * work->root_correction[2][0]) +
                      work->root_correction[3][0];
            sums.y = ((p0 * work->root_correction[0][1] +
                        p1 * work->root_correction[1][1]) +
                       p2 * work->root_correction[2][1]) +
                      work->root_correction[3][1];
            sums.z = ((p0 * work->root_correction[0][2] +
                        p1 * work->root_correction[1][2]) +
                       p2 * work->root_correction[2][2]) +
                      work->root_correction[3][2];
        }
        if (!computed) {
            JntChainSlot *slot;
            Vector4 *slot_translation;

            CUR_MATRIX_Set(work->transform_basis);
            for (index = 1; index <= work->matrix_index - 1; index++) {
                struct FcvPackAttribute *slot_attribute;

                slot = jntChainAt(work, index);
                slot->translation.x = __builtin_fabsf(slot->translation.x);
                slot->translation.w = 1.0f;
                slot_attribute = slot->attribute;
                if ((slot_attribute->channel_flags & JNT_KEY_LINK_ROTATION) != 0) {
                    int rotation_index = slot_attribute->rotation_index;

                    if (rotation_index < work->current_index) {
                        JntMatrix *rotation_matrix = work->matrix_buffer + rotation_index;

                        CUR_MATRIX_Txyz4s(&slot->translation);
                        CUR_MATRIX_SetR(rotation_matrix->elements);
                        CUR_MATRIX_Get((work->matrix_buffer + (work->chain_base_index + index))->elements);
                    }
                } else {
                    slot->translation.x = __builtin_fabsf(slot->translation.x);
                    CUR_MATRIX_Txyz4s(&slot->translation);
                    CUR_MATRIX_Rzyx4s((const int *)&slot->rotation);
                    CUR_MATRIX_Sxyz4s(&slot->scale);
                    CUR_MATRIX_Get((work->matrix_buffer + (work->chain_base_index + index))->elements);
                }
            }
            slot_translation = &jntChainAt(work, index)->translation;
            slot_translation->x = translate[0];
            slot_translation->y = translate[1];
            slot_translation->z = translate[2];
            CUR_MATRIX_Txyz4s(slot_translation);
            CUR_MATRIX_Get((work->matrix_buffer + work->current_index)->elements);
        } else {
            JntChainSlot *slot = jntChainAt(work, work->matrix_index);

            Vector4 *slot_translation = &slot->translation;
            unsigned short chain_count = ++work->matrix_index;

            *slot_translation = *(Vector4 *)translate;
            slot_translation->w = 1.0f;
            if (chain_count == 4) {
                float lane_x;
                float lane_y;
                float lane_z;

                axis_a.x = sums.x - jntChainAt(work, 0)->translation.x;
                axis_a.y = sums.y - jntChainAt(work, 0)->translation.y;
                axis_a.z = sums.z - jntChainAt(work, 0)->translation.z;
                lane_x = axis_a.x;
                lane_y = axis_a.y;
                lane_z = axis_a.z;
                axis_a.x = (work->transform_basis[0][0] * lane_x + work->transform_basis[0][1] * lane_y) + work->transform_basis[0][2] * lane_z;
                axis_a.y = (work->transform_basis[1][0] * lane_x + work->transform_basis[1][1] * lane_y) + work->transform_basis[1][2] * lane_z;
                axis_a.z = (work->transform_basis[2][0] * lane_x + work->transform_basis[2][1] * lane_y) + work->transform_basis[2][2] * lane_z;
                root_len = (axis_a.x * axis_a.x + axis_a.y * axis_a.y) + axis_a.z * axis_a.z;
                alignXAxis(&axis_a, basis_frame);
                MATRIX_transpose3(basis_frame);
                CUR_MATRIX_Set(work->transform_basis);
                CUR_MATRIX_4s3(basis_frame);
                CUR_MATRIX_Get(work->transform_basis);
                if ((jntChainAt(work, 1)->attribute->channel_flags & JNT_KEY_LINK_PIVOT) != 0) {
                    float *pivot;
                    float len;
                    float angle;
                    float pivot_x;
                    float pivot_y;
                    float pivot_z;

                    pivot = (work->matrix_buffer +
                             jntChainAt(work, 1)->attribute->pivot_index)->elements[3];
                    pivot[3] = 1.0f;
                    axis_a.x = pivot[0] - work->transform_basis[3][0];
                    axis_a.y = pivot[1] - work->transform_basis[3][1];
                    axis_a.z = pivot[2] - work->transform_basis[3][2];
                    pivot_x = axis_a.x;
                    pivot_y = axis_a.y;
                    pivot_z = axis_a.z;
                    axis_a.x = (work->transform_basis[0][0] * pivot_x + work->transform_basis[0][1] * pivot_y) + work->transform_basis[0][2] * pivot_z;
                    axis_a.y = (work->transform_basis[1][0] * pivot_x + work->transform_basis[1][1] * pivot_y) + work->transform_basis[1][2] * pivot_z;
                    axis_a.z = (work->transform_basis[2][0] * pivot_x + work->transform_basis[2][1] * pivot_y) + work->transform_basis[2][2] * pivot_z;
                    lane_z = axis_a.y * axis_a.y + axis_a.z * axis_a.z;
                    len = __builtin_sqrtf(lane_z);
                    __builtin_sqrtf(axis_a.x * axis_a.x + axis_a.y * axis_a.z + axis_a.z * axis_a.z);
                    angle = -xglAtan2(-axis_a.z / len, axis_a.y / len);
                    CUR_MATRIX_Set(work->transform_basis);
                    CUR_MATRIX_Rx4s((const int *)&angle);
                    CUR_MATRIX_Sxyz4s(&jntChainAt(work, 1)->scale);
                    CUR_MATRIX_Get(work->transform_basis);
                } else {
                    CUR_MATRIX_Rx4s((const int *)&jntChainAt(work, 1)->rotation.z);
                    CUR_MATRIX_Sxyz4s(&jntChainAt(work, 1)->scale);
                    CUR_MATRIX_Get(work->transform_basis);
                }
                {
                    float lane_x;
                    float lane_y;
                    float a_ref;
                    float cosine;
                    float diff;
                    float sine;
                    float yv;
                    float ang;
                    Vector4 *upper_joint = &jntChainAt(work, 2)->translation;
                    Vector4 *lower_joint = &jntChainAt(work, 3)->translation;

                    root_len = __builtin_sqrtf(root_len);
                    a_ref = upper_joint->x;
                    if (a_ref >= 0.0f) {
                        diff = a_ref * a_ref - lower_joint->x * lower_joint->x;
                        cosine = (root_len * root_len + diff) / (2.0f * a_ref * root_len);
                        sine = __builtin_sqrtf(1.0f - cosine * cosine);
                        yv = sine;
                    } else {
                        a_abs = __builtin_fabsf(a_ref);
                        upper_joint->x = a_abs;
                        diff = a_abs * a_abs - lower_joint->x * lower_joint->x;
                        cosine = (root_len * root_len + diff) / ((a_abs + a_abs) * root_len);
                        sine = __builtin_sqrtf(1.0f - cosine * cosine);
                        yv = -sine;
                    }
                    if (1.0f < cosine) {
                        cosine = 1.0f;
                        yv = 0.0f;
                    } else if (cosine < -1.0f) {
                        cosine = -1.0f;
                        yv = 0.0f;
                    }
                    ang = xglAtan2(yv, cosine);
                    CUR_MATRIX_Set(work->transform_basis);
                    CUR_MATRIX_Rz4s((const int *)&ang);
                    CUR_MATRIX_Get((work->matrix_buffer + work->chain_base_index + 1)->elements);
                    CUR_MATRIX_Txyz4s(&jntChainAt(work, 2)->translation);
                    CUR_MATRIX_Get(work->transform_basis);
                    CUR_MATRIX_Get(work->interpolation_root.elements);
                    __builtin_memcpy(&mid_b, work->interpolation_root.elements[3], sizeof(mid_b));
                    mid_b.w = 1.0f;
                    lane_x = sums.x - mid_b.x;
                    lane_y = sums.y - mid_b.y;
                    lane_z = sums.z - mid_b.z;
                    axis_a.w = 1.0f;
                    axis_a.x = (work->transform_basis[0][0] * lane_x + work->transform_basis[0][1] * lane_y) +
                               work->transform_basis[0][2] * lane_z;
                    axis_a.y = (work->transform_basis[1][0] * lane_x + work->transform_basis[1][1] * lane_y) +
                               work->transform_basis[1][2] * lane_z;
                    axis_a.z = (work->transform_basis[2][0] * lane_x + work->transform_basis[2][1] * lane_y) +
                               work->transform_basis[2][2] * lane_z;
                    root_len = (axis_a.x * axis_a.x + axis_a.y * axis_a.y) + axis_a.z * axis_a.z;
                    alignXAxis(&axis_a, basis_frame);
                    MATRIX_transpose3(basis_frame);
                    CUR_MATRIX_Set(work->transform_basis);
                    CUR_MATRIX_4s3(basis_frame);
                    CUR_MATRIX_Get((work->matrix_buffer + work->chain_base_index + 2)->elements);
                    CUR_MATRIX_Txyz4s(&jntChainAt(work, 3)->translation);
                    CUR_MATRIX_Get((work->matrix_buffer + work->current_index)->elements);
                }
            } else if (chain_count == 3) {
                float dx;
                float dy;

                dx = sums.x - jntChainAt(work, 0)->translation.x;
                dy = sums.y - jntChainAt(work, 0)->translation.y;
                dz = sums.z - jntChainAt(work, 0)->translation.z;

                mid_b.x = work->transform_basis[0][0] * dx +
                          work->transform_basis[0][1] * dy +
                          work->transform_basis[0][2] * dz;
                mid_b.y = work->transform_basis[1][0] * dx +
                          work->transform_basis[1][1] * dy +
                          work->transform_basis[1][2] * dz;
                mid_b.z = work->transform_basis[2][0] * dx +
                          work->transform_basis[2][1] * dy +
                          work->transform_basis[2][2] * dz;
                root_len = (mid_b.x * mid_b.x + mid_b.y * mid_b.y) + mid_b.z * mid_b.z;
                alignXAxis(&mid_b, basis_frame);
                MATRIX_transpose3(basis_frame);
                CUR_MATRIX_Set(work->transform_basis);
                CUR_MATRIX_4s3(basis_frame);
                CUR_MATRIX_Rx4s((const int *)&jntChainAt(work, 1)->rotation.z);
                CUR_MATRIX_Sxyz4s(&jntChainAt(work, 1)->scale);
                CUR_MATRIX_Get((work->matrix_buffer + work->chain_base_index + 1)->elements);
                CUR_MATRIX_Txyz4s(&jntChainAt(work, 2)->translation);
                CUR_MATRIX_Get((work->matrix_buffer + work->chain_base_index + 2)->elements);

            }
        }
        break;
    }
    case 5:
        CUR_MATRIX_Set((work->matrix_buffer + channel->matrix_index)->elements);
        get_fcvpack(&work->channel_translation.x, translate, work, value_mask, frame);
        value_mask >>= 3;
        get_fcvpack(&work->channel_rotation.x, rotate, work, value_mask, frame);
        get_fcvpack_fix(&work->channel_scale.x, 1.0f, work, value_mask >> 3, frame);
        CUR_MATRIX_Txyz4s(&work->channel_translation);
        CUR_MATRIX_Rzyx4s((const int *)&work->channel_rotation);
        CUR_MATRIX_Sxyz4s(&work->channel_scale);
        CUR_MATRIX_Get((work->matrix_buffer + work->current_index)->elements);
        break;
    }
    /* Inherited channels rescale this matrix's y translation from the previous channel, then take the parent's translation. */
    if ((attribute->channel_flags & JNT_CHANNEL_FLAG_INHERIT) != 0) {
        JntMatrix *matrices;
        float (*dest)[4];
        float (*previous)[4];
        JntStaticChannel *previous_channel;

        xglMatrixUnit(work->root_correction);
        value_mask = work->channel_index;
        matrices = work->matrix_buffer;
        dest = (matrices + work->current_index)->elements;
        previous = (matrices + work->current_index - 1)->elements;
        previous_channel = work->channels + work->current_index - 1;
        dest[3][1] = dest[3][1] *
            ((previous_channel->translation.y + matrices[0].elements[3][1]) *
             (1.0f / previous[3][1]));
        if ((value_mask & 7) == 0) {
            float (*source)[4] = ((JntMatrix *)(channel->matrix_index * sizeof(JntMatrix) + (int)matrices))->elements;
            float previous_channel_y;
            float previous_matrix_y;

            work->root_correction[3][0] = -(dest[3][0] - source[3][0]);
            work->root_correction[3][1] = -(dest[3][1] - source[3][1]);
            work->root_correction[3][2] = -(dest[3][2] - source[3][2]);
            previous_channel_y = previous_channel->translation.y;
            previous_matrix_y = previous[3][1];
            dest[3][0] = source[3][0];
            dest[3][1] = source[3][1] *
                (previous_matrix_y * (1.0f / previous_channel_y));
            dest[3][2] = source[3][2];
        }
    }
    work->current_index++;
    return next_channel;
}

/*
 * Blends the translation row of matrix toward other_matrix by blend. The
 * basis rows are kept from matrix, with their w lanes set to 1.
 */
static void liner_interpolate(JntMatrix *matrix, const JntMatrix *other_matrix, float blend)
{
    /* Move the scalar blend from COP1 through $3 into VF31, whose x lane is used as the weight. */
    __asm__ __volatile__(
        "mfc1 $3, %0\n"
        "qmtc2.ni $3, $vf31"
        :
        : "f"(blend)
        : "$3", "memory");

    /* Preserve basis xyz and set their w lanes to VF0.w (1.0). */
    __asm__ __volatile__(
        "lqc2 $vf22, 0x30(%0)\n"
        "lqc2 $vf23, 0x30(%1)\n"
        "lqc2 $vf19, 0x00(%0)\n"
        "lqc2 $vf20, 0x10(%0)\n"
        "lqc2 $vf21, 0x20(%0)\n"
        "vsub.xyz $vf24, $vf22, $vf23\n"
        "vmove.w $vf19, $vf0\n"
        "vmove.w $vf20, $vf0\n"
        "vmove.w $vf21, $vf0"
        :
        : "r"(matrix), "r"(other_matrix)
        : "memory");

    /* Blend only the translation vector's xyz lanes. */
    __asm__ __volatile__(
        "vmulx.xyz $vf24, $vf24, $vf31x\n"
        "vadd.xyz $vf24, $vf24, $vf23"
        :
        :
        : "memory");

    /* Store the blended basis and translation rows. */
    __asm__ __volatile__(
        "sqc2 $vf19, 0x00(%0)\n"
        "sqc2 $vf20, 0x10(%0)\n"
        "sqc2 $vf21, 0x20(%0)\n"
        "sqc2 $vf24, 0x30(%0)"
        :
        : "r"(matrix)
        : "memory");
}

static inline void jntCopyTranslation(float destination[3],
                                      const float translation[3])
{
    destination[0] = translation[0];
    destination[1] = translation[1];
    destination[2] = translation[2];
}

/* Blends the current matrix with the interpolation matrix for the channel type, then advances the element index. */
JntStaticChannel *JNT_interpolate(JntWork *work, JntStaticChannel *channel)
{
    JntMatrix result_matrix;
    Vector4 left_quaternion;
    Vector4 right_quaternion;
    Vector4 result_quaternion;
    JntStaticChannel *next_channel = channel + 1;
    float left_weight = 1.0f - work->interpolation;
    float right_weight = work->interpolation;

    switch (channel->type) {
    case 0:
    case 1:
        liner_interpolate(&work->matrix_buffer[work->current_index],
                          &work->interp_matrix[work->current_index],
                          work->interpolation);
        break;
    case 3:
        {
        jntCopyMatrix(&work->interpolation_root, &work->matrix_buffer[channel->matrix_index]);
        MATRIX_translate4s(work->interpolation_root.elements, channel->translation.x,
                           channel->translation.y, channel->translation.z);
        {
            jntCopyMatrix(&work->matrix_buffer[work->current_index], &work->interpolation_root);
            jntCopyTranslation(work->matrix_buffer[work->current_index + 1].elements[3],
                               work->matrix_buffer[work->current_index].elements[3]);
        }
        break;
        }
    case 4:
        {
        if (channel->interpolation_flags != 0) {
            xglMatrix2Quaternion((float *)&left_quaternion,
                                 &work->interp_matrix[work->current_index]);
            xglMatrix2Quaternion((float *)&right_quaternion,
                                 &work->matrix_buffer[work->current_index]);
            xglQuaternionInterpolateLinear(&result_quaternion,
                                           &left_quaternion,
                                           &right_quaternion,
                                           left_weight, right_weight);
            xglQuaternion2Matrix(result_matrix.elements, &result_quaternion);
            result_matrix.elements[0][3] = result_matrix.elements[1][3] =
                result_matrix.elements[2][3] = result_matrix.elements[3][3] = 1.0f;
            jntCopyTranslation(result_matrix.elements[3],
                               work->matrix_buffer[work->current_index].elements[3]);
            jntCopyMatrix(&work->matrix_buffer[work->current_index], &result_matrix);
        }
        break;
        }
    case 2:
        {
        /* Rotation is interpolated as a quaternion; scale comes from the interpolation basis lengths when flag 0x20000 is set. */
        float scale;
        if (work->flags & 0x40000) {
            if ((work->current_index == 0x30) ||
                (work->current_index == 0x24)) {
                left_weight = 0.0f;
                right_weight = 1.0f;
            }
        }
        xglMatrix2Quaternion((float *)&left_quaternion,
                             &work->interp_matrix[work->current_index]);
        xglMatrix2Quaternion((float *)&right_quaternion,
                             &work->matrix_buffer[work->current_index]);
        xglQuaternionInterpolateLinear(&result_quaternion, &left_quaternion,
                                       &right_quaternion, left_weight,
                                       right_weight);
        xglQuaternion2Matrix(result_matrix.elements, &result_quaternion);
        if (work->flags & 0x20000) {
            scale = 0.0f;
            xglVectorLength(&scale, (const Vector4 *)&work->interp_matrix[work->current_index].elements[0][0]);
            result_matrix.elements[0][3] = scale;
            xglVectorLength(&scale, (const Vector4 *)&work->interp_matrix[work->current_index].elements[1][0]);
            result_matrix.elements[1][3] = scale;
            xglVectorLength(&scale, (const Vector4 *)&work->interp_matrix[work->current_index].elements[2][0]);
            result_matrix.elements[2][3] = scale;
            result_matrix.elements[3][3] = 1.0f;
        } else {
            result_matrix.elements[0][3] = result_matrix.elements[1][3] =
                result_matrix.elements[2][3] = result_matrix.elements[3][3] = 1.0f;
        }
        jntCopyMatrixRow(result_matrix.elements[3],
                         work->matrix_buffer[work->current_index].elements[3]);
        jntCopyMatrix(&work->matrix_buffer[work->current_index], &result_matrix);
        MATRIX_translate4s(result_matrix.elements,
                           next_channel->translation.x, 0.0f, 0.0f);
        jntCopyTranslation(work->matrix_buffer[work->current_index + 1].elements[3],
                           result_matrix.elements[3]);
        break;
        }
    case 5:
    case 6:
        break;
    }
    work->current_index++;
    return next_channel;
}

void JNT_setFlags(int flags)
{
    JntWork *work = JNT_SCRATCH_BASE;
    work->flags = flags;
}

void JNT_setClipR(float clip_radius)
{
    JntWork *work = JNT_SCRATCH_BASE;
    work->clip_radius = clip_radius;
}

void JNT_animSetFlags(int anim_flags)
{
    JntWork *work = JNT_SCRATCH_BASE;
    work->animation_flags = anim_flags;
}

int JNT_getFlags(void)
{
    JntWork *work = JNT_SCRATCH_BASE;
    return work->flags;
}

/* Returns the data after the list's type-4 accessory record, or 0 when there is none. */
void *JNT_getAccessories(JntMoveResource *move)
{
    unsigned int accumulated = 4;
    JntAccessoryRecord *record;
    int *chunk_size;

    if (move == 0) {
        return 0;
    }
    chunk_size = &move->accessory_size;
    record = (JntAccessoryRecord *)(move + 1);
    for (;;) {
        if (record->type == 4) {
            return record + 1;
        }
        if (record->type == 0) {
            break;
        }
        {
        unsigned int size = record->size;
        accumulated += size;
        record = (JntAccessoryRecord *)((unsigned char *)record + size);
        if (accumulated >= (unsigned int)*chunk_size) {
            break;
        }
        }
    }
    return 0;
}

/*
 * Walks an attribute buffer: a type-5 record sets the current value tag, and
 * a type-1 record sets the attribute key and its 8-character name.
 */
void JNT_readAttribute(void *scratch_address, void *attribute_buffer)
{
    typedef struct JntAttributeRecord {
        unsigned short size;
        unsigned short type;
        union {
            char text[8];
            unsigned char bytes[8];
            unsigned int words[2];
            JntChannelTag tag;
        } value;
    } JntAttributeRecord;
    typedef struct JntAttributeHeader {
        unsigned int byte_size;
    } JntAttributeHeader;
    JntWork *workspace = scratch_address;
    JntAttributeHeader *attributes = attribute_buffer;
    JntAttributeRecord *record = (JntAttributeRecord *)(attributes + 1);
    unsigned int offset = sizeof(*attributes);

    workspace->current_value = dummyCntFree;
    workspace->attribute_name = dummyName;
    dummyName[0] = 0;

    for (;;) {
        switch (record->type) {
        case 5:
            workspace->current_value = &record->value.tag;
            break;
        case 1:
            {
                const unsigned int *value_words = record->value.words;

                workspace->attribute_key =
                    value_words[0] * 31 + value_words[1];
                strncpy(workspace->attribute_name, record->value.text, 8);
                workspace->attribute_name[8] = 0;
            }
            break;
        }
        if (record->type == 0) {
            return;
        }
        {
            unsigned short record_size = record->size;

            offset += record_size;
            record = (JntAttributeRecord *)((unsigned char *)record + record_size);
        }
        if (offset >= attributes->byte_size) {
            return;
        }
    }
}

void JNT_setMatrix(JntMatrixBuffer matrix_buffer)
{
    JntWork *work = JNT_SCRATCH_BASE;
    work->matrix_buffer = matrix_buffer;
    work->matrix_select = 0;
}

void JNT_setMatrix2(JntMatrixBuffer matrix_buffer, JntMatrixSelect matrix_select)
{
    JntWork *work = JNT_SCRATCH_BASE;
    work->matrix_buffer = matrix_buffer;
    work->matrix_select = matrix_select;
}

void JNT_setInterpMatrix(JntMatrixBuffer matrix_buffer, float interpolation)
{
    JntWork *work = JNT_SCRATCH_BASE;
    work->interp_matrix = matrix_buffer;
    work->interpolation = interpolation;
}

void JNT_setCurve(void *static_value_records, void *value_records)
{
    JntWork *work = JNT_SCRATCH_BASE;
    work->static_value_records = static_value_records;
    work->value_records = value_records;
}

/*
 * Applies one channel to the model matrix tables: type 5 copies the current
 * matrix, type 2 multiplies it by the model matrix into the combined table.
 * Returns the next channel.
 */
JntStaticChannel *JNT_setMDLMatrix(JntWork *work, JntStaticChannel *channel)
{
    JntModelMatrixResource *model = work->model;
    JntStaticChannel *next_channel = channel + 1;

    if (model == 0) {
        return next_channel;
    }
    switch (channel->type) {
    case 0:
    case 1:
    case 3:
    case 4:
        break;
    case 5:
        MATRIX_convert4(work->matrix_buffer[work->current_index].elements,
                        work->matrix_buffer[work->current_index].elements);
        {
            unsigned short matrix_index = work->matrix_index;
            int current_index = work->current_index;
            unsigned char *matrices = (unsigned char *)model +
                model->matrices_offset;
            JntMatrix *destination = jntMatrixAt(matrices, matrix_index);
            JntMatrix *source = jntMatrixAt(work->matrix_buffer, current_index);

            jntCopyMatrix(destination, source);
        }
        work->matrix_index++;
        break;
    case 2:
        MATRIX_convert4(work->matrix_buffer[work->current_index].elements,
                        work->matrix_buffer[work->current_index].elements);
        xglMatrixMul(
            (float (*)[4])((unsigned char *)model + model->combined_matrices_offset +
                (work->matrix_index << 6)),
            work->matrix_buffer[work->current_index].elements,
            (float (*)[4])((unsigned char *)model + model->model_matrices_offset +
                (work->matrix_index << 6)));
        work->matrix_index++;
        break;
    case 6:
        work->matrix_index++;
        break;
    }
    work->current_index++;
    return next_channel;
}

/* Runs JNT_setMDLMatrix over the channels in two passes: up to the hair-id primary and secondary counts, then up to the channel limit. */
void JNT_setModelMatrix(void)
{
    JntWork *work = JNT_SCRATCH_BASE;
    JntStaticChannel *channel;
    int first_channel_count;
    int channel_limit;

    if (work->channels != 0) {
        channel = work->channels;
        work->current_index = 0;
        first_channel_count = work->hair_id_source->primary_id +
                              work->hair_id_source->secondary_id;
        work->matrix_index = 0;
        if (first_channel_count != 0) {
            do {
                channel = JNT_setMDLMatrix(work, channel);
            } while (work->current_index < first_channel_count);
        }

        channel_limit = work->hair_id_source->channel_limit;
        work->matrix_index = 0;
        while (work->current_index < channel_limit) {
            channel = JNT_setMDLMatrix(work, channel);
        }
    }
}

/*
 * Scans the move elements from the first one and returns one less than the
 * index of the first element whose type is not 1, or 0 when none is found.
 */
int JNT_getMoveElement(void *move)
{
    JntMoveElementHeader *header = move;
    int *element_offset = &header->element_offset;
    unsigned short element_count = header->element_count;
    JntMoveElement *element;
    int element_index = 0;

    element = (JntMoveElement *)((unsigned char *)element_offset + *element_offset);
    if (element_count != 0) {
        for (;;) {
            unsigned short type = element->type;
            int return_index;

            element++;
            return_index = element_index - 1;
            element_index++;
            if (type != 1) {
                return return_index;
            }
            if (element_index >= element_count) {
                break;
            }
        }
    }

    return 0;
}

void *JNT_getRootElement(void *joint)
{
    JntElementHeader *header = joint;
    int *root_element_offset = &header->root_element_offset;
    return (unsigned char *)root_element_offset + *root_element_offset;
}

/*
 * Sets the model and hair-id source and locates the channel table. Version 2
 * and later resources read their attribute header first, and the channels
 * follow at its offset.
 */
void JNT_setModel(void *model, JntHairIdSource *joint)
{
    JntWork *work = JNT_SCRATCH_BASE;

    work->model = model;
    work->hair_id_source = joint;
    work->attribute_key = 0;
    if (joint == 0) {
        work->channels = 0;
        return;
    }
    if (joint->version >= 2) {
        int *attribute_header = &joint->attribute_size;

        JNT_readAttribute(work, attribute_header);
        work->channels = (JntStaticChannel *)((unsigned char *)attribute_header + *attribute_header);
        return;
    }
    work->channels = (JntStaticChannel *)&joint->attribute_size;
}

/*
 * Starts an FCV pack for channel_index. A null pack sets JNT_WORK_NO_PACK, so
 * the consumers do nothing.
 */
void JNT_setFCurve(void *pack, unsigned int channel_index)
{
    JntWork *work = JNT_SCRATCH_BASE;

    work->channel_index = channel_index;
    work->root_translation.w = 1.0f;
    work->flags |= 1;
    work->root_rotation.w = 1.0f;
    work->root_scale.w = 1.0f;
    if (pack == 0) {
        work->flags |= JNT_WORK_NO_PACK;
    } else {
        FCV2_resetPack(&work->cursor, pack);
    }
    work->elements = &work->default_element;
}

/*
 * Starts an FCV pack for channel_index and sets the matrix buffer. Unlike
 * JNT_setFCurve, the pack is not checked for null.
 */
void JNT_setFCurve2(void *pack, void *matrix_buffer,
                    unsigned int channel_index)
{
    JntWork *work = JNT_SCRATCH_BASE;

    FCV2_resetPack(&work->cursor, pack);
    work->matrix_buffer = matrix_buffer;
    work->channel_index = channel_index;
    work->root_translation.w = 1.0f;
    work->flags |= 1;
    work->root_rotation.w = 1.0f;
    work->root_scale.w = 1.0f;
}

JntRootStorage JNT_getRootTrans(volatile JntRootSlot *destination)
{
    const JntRootScratch *work = JNT_SCRATCH_BASE;
    unsigned int workAddress = (unsigned int)work;
    const JntRootSlot *translation = (const JntRootSlot *)
        (workAddress + sizeof(work->unmodeled_00));
    JntRootStorage value = translation->storage;
    destination->storage = value;
    return value;
}

JntRootStorage JNT_getRootRotate(volatile JntRootSlot *destination)
{
    const JntRootScratch *work = JNT_SCRATCH_BASE;
    unsigned int workAddress = (unsigned int)work;
    const JntRootSlot *rotation = (const JntRootSlot *)
        (workAddress + sizeof(work->unmodeled_00) + sizeof(work->translation));
    JntRootStorage value = rotation->storage;
    destination->storage = value;
    return value;
}

JntRootStorage JNT_getRootScale(volatile JntRootSlot *destination)
{
    const JntRootScratch *work = JNT_SCRATCH_BASE;
    unsigned int workAddress = (unsigned int)work;
    const JntRootSlot *scale = (const JntRootSlot *)
        (workAddress + sizeof(work->unmodeled_00) + sizeof(work->translation) +
         sizeof(work->rotation));
    JntRootStorage value = scale->storage;
    destination->storage = value;
    return value;
}

/* Registers a consumer and its argument under consumer_id, reusing an existing entry with that id. Returns 0. */
int JNT_addConsumer(JntProducer *producer, int consumer_id,
                    JntProductionConsumer consumer, int argument)
{
    int count = producer->count;
    int entry = -1;
    int index;

    for (index = 0; index < count; index++) {
        if (producer->consumer_ids[index] == consumer_id) {
            entry = index;
            break;
        }
    }
    if (entry < 0) {
        entry = count;
        producer->count = entry + 1;
    }
    producer->consumer_ids[entry] = consumer_id;
    producer->consumers[entry] = consumer;
    producer->consumer_arguments[entry] = argument;
    return 0;
}

void JNT_initProducer(JntProducer *producer)
{
    int i;

    producer->pending_output = 0;
    producer->count = 0;
    producer->accessory_joints = 0;
    producer->move_element_id = -1;
    for (i = 7; i >= 0; i--) {
        producer->consumers[i] = 0;
    }
}

/*
 * Starts one production pass for actor at current_frame. Passes with flag
 * 0x08000000 produce nothing; otherwise the first consumer runs when its
 * address is inside the program image. Returns 0.
 */
int JNT_startProduction(float current_frame, JntProducer *producer, void *actor)
{
    JntWork *work = JNT_SCRATCH_BASE;
    JntProductionOwner *owner = actor;
    JntStaticChannel *channels = work->channels;
    JntProductionConsumer consumer;
    int consumer_argument;
    unsigned int consumer_address;

    work->channel_start_index = 0;
    work->current_index = 0;
    producer->deferred_count = 0;
    work->current_frame = current_frame;
    work->owner = owner;
    work->producer = producer;

    if ((work->animation_flags & 0x08000000) != 0) {
        producer->pending_output = 0;
        return 0;
    }

    if (producer->count > 0) {
        consumer = producer->consumers[0];
        consumer_argument = producer->consumer_arguments[0];
        consumer_address = (unsigned int)consumer;
        if (consumer_address - JNT_CONSUMER_CODE_BASE <= JNT_CONSUMER_CODE_SPAN &&
            (consumer_address & 3) == 0) {
            consumer(JNT_SCRATCH_BASE, channels, consumer_argument);
            if (owner != 0) {
                owner->matrix_buffer = work->matrix_buffer;
            }
        }
    }

    producer->pending_output = work->flags;
    return 0;
}

void *JNT_getElement(void *elements, int index)
{
    JntElement *element_array = elements;
    return &element_array[index];
}

void *JNT_nextElement(void *element)
{
    JntElement *current = element;
    return current + 1;
}

/*
 * Default interrupt consumer. Samples the hair channels from the pack, then
 * from static values once the pack ends, and blends them when flag 2 is set.
 * Returns the next channel, or 0 when there is no channel or pack, or when the
 * pack ends with flag 0x40 set.
 */
JntStaticChannel *JNT_defaultConsumer(JntWork *work,
                                      JntStaticChannel *channel,
                                      int consumer_argument)
{
    JntStaticChannel *cursor;
    unsigned int difference;
    float frame;
    int count;

    if (channel == 0) {
        return 0;
    }
    if (work->flags & JNT_WORK_NO_PACK) {
        return 0;
    }
    work->flags |= 0x18;
    cursor = channel;
    difference = work->requested_attribute_key - work->attribute_key;
    frame = work->current_frame;
    work->attribute_difference = difference;
    /* Matching attribute keys use JNT_getVal2 over the primary and secondary channels; other keys use JNT_getVal_HOGE2 for the primary ones. */
    if (difference == 0) {
        count = work->hair_id_source->primary_id;
        count += work->hair_id_source->secondary_id;
        while (work->current_index < count) {
            cursor = JNT_getVal2(work, cursor, frame);
            if (work->flags & JNT_WORK_PACK_ENDED) {
                break;
            }
        }
        if (work->flags & JNT_WORK_PACK_ENDED) {
            if (work->flags & 0x40) {
                return 0;
            }
            while (work->current_index < count) {
                cursor = JNT_getStaticVal2(work, cursor);
            }
        }
        if (work->flags & 2) {
            work->current_index = 0;
            cursor = channel;
            while (work->current_index < count) {
                cursor = JNT_interpolate(work, cursor);
            }
        }
        count += work->hair_id_source->additional_secondary_count;
        count += work->hair_id_source->additional_primary_count;
        if (work->flags & JNT_WORK_PACK_ENDED) {
            while (work->current_index < count) {
                cursor = JNT_getStaticVal2(work, cursor);
            }
            return cursor;
        }
        while (work->current_index < count) {
            cursor = JNT_getVal2(work, cursor, frame);
            if (work->flags & JNT_WORK_PACK_ENDED) {
                break;
            }
        }
        if (work->flags & JNT_WORK_PACK_ENDED) {
            work->flags &= ~JNT_WORK_PACK_ENDED;
            while (work->current_index < count) {
                cursor = JNT_getStaticVal2(work, cursor);
            }
        }
    } else {
        count = work->hair_id_source->primary_id;
        while (work->current_index < count) {
            cursor = JNT_getVal_HOGE2(work, cursor, frame);
            if (work->flags & JNT_WORK_PACK_ENDED) {
                break;
            }
        }
        if (work->flags & JNT_WORK_PACK_ENDED) {
            while (work->current_index < count) {
                cursor = JNT_getStaticVal2(work, cursor);
            }
        }
        count += work->hair_id_source->secondary_id;
        while (work->current_index < count) {
            cursor = JNT_getStaticVal2(work, cursor);
        }
        if (work->flags & 2) {
            work->current_index = 0;
            cursor = channel;
            while (work->current_index < count) {
                cursor = JNT_interpolate(work, cursor);
            }
        }
        count += work->hair_id_source->additional_secondary_count;
        count += work->hair_id_source->additional_primary_count;
        while (work->current_index < count) {
            cursor = JNT_getStaticVal2(work, cursor);
        }
    }
    return cursor;
}

/* Refreshes the secondary hair channels from their static values. */
void JNT_resetHair(void)
{
    JntWork *work = JNT_SCRATCH_BASE;
    JntHairIdSource *source = work->hair_id_source;
    int first = source->primary_id;
    int end;
    JntStaticChannel *channels = work->channels;
    JntStaticChannel *channel = &channels[first];

    work->current_index = first;
    end = first + source->secondary_id;
    if (first < end) {
        do {
            channel = JNT_getStaticVal2(JNT_SCRATCH_BASE, channel);
        } while (work->current_index < end);
    }
}

/*
 * Returns the channel count for the current tag: the primary count, plus the
 * secondary count when the tag's word count is 0 (it never is on the disc),
 * plus the tag value for hair-offset tags.
 */
static int JNT_hairID(JntWork *work)
{
    JntChannelTag *tag = work->current_value;
    JntHairIdSource *source = work->hair_id_source;
    int id;

    id = source->primary_id;
    if (tag->word_count == 0) {
        id += source->secondary_id;
    }
    if ((tag->tag & JNT_TAG_KIND_MASK) == JNT_TAG_KIND_HAIR_OFFSET) {
        id += tag->tag & JNT_TAG_VALUE_MASK;
    }
    return id;
}

/*
 * Computes the hair channel run for the current hair id, sampling the pack,
 * static values or EXM motion depending on the attribute key and animation
 * flags. Returns the next channel, or 0 when there is no channel or pack.
 */
JntStaticChannel *JNT_computeHair(JntWork *work, JntStaticChannel *channel,
                                 int consumer_argument)
{
    JntStaticChannel *cursor;
    unsigned int difference;
    float frame;
    int count;

    if (channel == 0) {
        return 0;
    }
    if (work->flags & JNT_WORK_NO_PACK) {
        return 0;
    }
    work->flags |= 0x18;
    cursor = channel;
    difference = work->requested_attribute_key - work->attribute_key;
    frame = work->current_frame;
    work->attribute_difference = difference;
    if (difference == 0) {
        count = JNT_hairID(work);
        while (work->current_index < count) {
            cursor = JNT_getVal2(work, cursor, frame);
            if (work->flags & JNT_WORK_PACK_ENDED) {
                break;
            }
        }
        if (work->flags & JNT_WORK_PACK_ENDED) {
            while (work->current_index < count) {
                cursor = JNT_getStaticVal2(work, cursor);
            }
        }
        /* Flag 0x80000000 samples the whole hair range from the pack; otherwise static values or EXM motion are used. */
        if (work->animation_flags & 0x80000000U) {
            count = work->hair_id_source->primary_id +
                    work->hair_id_source->secondary_id;
            while (work->current_index < count) {
                cursor = JNT_getVal2(work, cursor, frame);
                if (work->flags & JNT_WORK_PACK_ENDED) {
                    break;
                }
            }
        } else {
            count = work->hair_id_source->primary_id +
                    work->hair_id_source->secondary_id;
            if (work->animation_flags & 0x40010000U) {
                while (work->current_index < count) {
                    cursor = JNT_getStaticVal2(work, cursor);
                }
            } else {
                while (work->current_index < count) {
                    cursor = EXM_CalcExMotion(work, cursor);
                }
            }
        }
        if (work->flags & 2) {
            work->current_index = 0;
            cursor = channel;
            while (work->current_index < count) {
                cursor = JNT_interpolate(work, cursor);
            }
        }
        count = work->hair_id_source->channel_limit;
        if (work->flags & JNT_WORK_PACK_ENDED) {
            while (work->current_index < count) {
                cursor = JNT_getStaticVal2(work, cursor);
            }
            return cursor;
        }
        while (work->current_index < count) {
            cursor = JNT_getVal2(work, cursor, frame);
            if (work->flags & JNT_WORK_PACK_ENDED) {
                break;
            }
        }
        if (work->flags & JNT_WORK_PACK_ENDED) {
            work->flags &= ~JNT_WORK_PACK_ENDED;
            while (work->current_index < count) {
                cursor = JNT_getStaticVal2(work, cursor);
            }
        }
    } else {
        count = work->hair_id_source->primary_id;
        while (work->current_index < count) {
            cursor = JNT_getVal_HOGE2(work, cursor, frame);
            if (work->flags & JNT_WORK_PACK_ENDED) {
                break;
            }
        }
        if (work->flags & JNT_WORK_PACK_ENDED) {
            while (work->current_index < count) {
                cursor = JNT_getStaticVal2(work, cursor);
            }
        }
        count = JNT_hairID(work);
        while (work->current_index < count) {
            cursor = JNT_getStaticVal2(work, cursor);
        }
        count = JNT_hairID(work);
        if (work->animation_flags & 0x40010000U) {
            while (work->current_index < count) {
                cursor = JNT_getStaticVal2(work, cursor);
            }
        } else {
            while (work->current_index < count) {
                cursor = EXM_CalcExMotion(work, cursor);
            }
        }
        if (work->flags & 2) {
            work->current_index = 0;
            cursor = channel;
            while (work->current_index < count) {
                cursor = JNT_interpolate(work, cursor);
            }
        }
        count = work->hair_id_source->channel_limit;
        while (work->current_index < count) {
            cursor = JNT_getStaticVal2(work, cursor);
        }
    }
    return cursor;
}

/*
 * Points each element at the channel of every non-filter consumer id, and
 * returns how many were set.
 */
int JNT_setInterrupt(JntWork *work, JntProducer *producer)
{
    int count = producer->count;
    int result = 0;
    int channel;

    work->elements[0].interrupt_channel = 0;
    for (channel = 1; channel < count; channel++) {
        int element = producer->consumer_ids[channel];
        if (element != 0 && (element & JNT_CONSUMER_ID_FILTER) == 0) {
            work->elements[element].interrupt_channel = channel;
            result++;
        }
    }

    return result;
}

/*
 * Returns the first filter consumer, searched from index 1 of the producer's
 * consumer table. A filter is an entry whose consumer id has the 0x8000 bit
 * set; JNT_setInterrupt skips those ids.
 */
JntProductionConsumer JNT_getFilter(void *work, JntProducer *producer)
{
    int count = producer->count;
    int index = 1;

    if (index < count) {
        int remaining = count;
        JntProductionConsumer *filter = &producer->consumers[1];
        int *flag = &producer->consumer_ids[1];
        for (;;) {
            if ((*flag & JNT_CONSUMER_ID_FILTER) != 0) {
                return *filter;
            }
            count = remaining;
            flag++;
            index++;
            if (index >= count) {
                break;
            }
            filter++;
        }
    }
    return 0;
}

/*
 * Keeps the root matrix and multiplies each matrix in the hair range by it,
 * advancing past the static channels first. Returns the next channel.
 */
JntStaticChannel *JNT_resetMatrix(JntWork *work, JntStaticChannel *channel)
{
    JntMatrix base_matrix;
    JntMatrix result_matrix;
    JntMatrix source_matrix;
    JntStaticChannel *next;
    int matrix_end;
    int matrix_start;
    int has_matrix_range;

    jntCopyMatrix(&work->root_matrix, &work->matrix_buffer[0]);
    xglMatrixUnit4s(work->matrix_buffer[0].elements);
    next = channel;

    {
        JntHairIdSource *source = work->hair_id_source;
        matrix_start = work->current_index;
        matrix_end = source->primary_id;
        matrix_end += source->secondary_id;
        matrix_end += source->additional_primary_count;
    }
    while (matrix_start < matrix_end) {
        next = JNT_getStaticVal2(work, next);
        matrix_start = work->current_index;
    }

    matrix_end += work->hair_id_source->additional_secondary_count;
    has_matrix_range = matrix_start < matrix_end;
    if (has_matrix_range) {
        do {
            next = JNT_getStaticVal2(work, next);
        } while (work->current_index < matrix_end);
    }

    MATRIX_convert4(base_matrix.elements, work->root_matrix.elements);
    work->current_index = matrix_start;
    if (has_matrix_range) {
        do {
            MATRIX_convert4(source_matrix.elements,
                            work->matrix_buffer[work->current_index].elements);
            xglMatrixMul(result_matrix.elements, base_matrix.elements, source_matrix.elements);
            jntCopyMatrix(&work->matrix_buffer[work->current_index], &result_matrix);
            work->current_index++;
        } while (work->current_index < matrix_end);
    }
    return next;
}

/*
 * Computes the joint matrices of one channel run. The owner's movement sets a
 * follow speed, the inherit channel (flag 0x8000) is clip-tested, and the run
 * may be skipped by returning 0 when the clip test rejects it.
 */
JntStaticChannel *JNT_computeMatrix(JntWork *work, JntStaticChannel *channel)
{
    JntMatrix previous_matrix;
    JntMatrix dynamic_clip_basis;
    JntMatrix dynamic_clip_joint;
    JntMatrix dynamic_clip_result;
    Vector4 dynamic_clip_position;
    JntMatrix result_matrix;
    JntMatrix source_matrix;
    JntMatrix static_clip_basis;
    JntMatrix static_clip_joint;
    Vector4 static_clip_position;
    JntMatrix static_source_matrix;
    float distance;
    JntProductionOwner *owner;
    JntStaticChannel *next_channel;
    JntStaticChannel *saved_channel;
    JntMatrix *clip_joint;
    unsigned int animation_flags;
    float frame;
    float previous_speed;
    float direction;
    float speed;
    int end_index;
    int matrix_start;
    int has_matrix_range;
    int nmlModelCalcClip(Vector4 *position);
    extern float jointMovementFollowRate;

    owner = work->owner;
    if (channel == 0) {
        return 0;
    }
    if (work->flags & JNT_WORK_NO_PACK) {
        return 0;
    }
    work->flags |= 0x18;
    next_channel = channel;
    frame = work->current_frame;
    jntCopyMatrix(&work->root_matrix, &work->matrix_buffer[0]);
    jntCopyMatrix(&previous_matrix, &work->matrix_buffer[0]);
    xglMatrixUnit4s(work->matrix_buffer[0].elements);
    animation_flags = work->animation_flags;
    if (animation_flags & 0x10000) {
        if (animation_flags & 0x01000000) {
            animation_flags |= 0x40000000;
        } else if (flagSmoothHair || (animation_flags & 0x800000) ||
                   GameLoopState.mode == 2) {
            work->animation_flags &= ~0x10000U;
            animation_flags = work->animation_flags;
        } else if (!(animation_flags & 0x02000000)) {
            animation_flags |= 0x40000000;
        }
    }
    /* Derive the owner's follow speed from how far it moved since the last frame. */
    if (owner != 0) {
        previous_speed = owner->movement.y;
        speed = 0.0f;
        distance = speed;
        direction = speed;
        if ((owner->movement_flags & 1) && !(animation_flags & 0x40010000)) {
            owner->movement.x -= owner->position.x;
            owner->movement.y = speed;
            owner->movement.z -= owner->position.z;
            xglVectorLength(&distance, &owner->movement);
            direction = -xglCos(owner->rotation.y -
                                xglAtan2(owner->movement.x, owner->movement.z));
            if (direction < 0.0f) {
                direction = 0.0f;
            }
        } else {
            owner->movement.w = 0.0f;
            if (owner->movement_flags & 2) {
                previous_speed = owner->movement.w;
                owner->movement_flags = 1;
            }
        }
        speed = distance * direction * owner->movement_delay * 0.5f + previous_speed;
        distance = speed;
        if (speed < 0.0f) {
            speed = 0.0f;
            distance = speed;
        }
        owner->movement.y = (speed - owner->movement.w) * jointMovementFollowRate;
        owner->movement.w = distance;
        owner->movement.x = owner->position.x;
        owner->movement.z = owner->position.z;
    }
    work->channel_start_index = -1;
    work->attribute_difference = work->requested_attribute_key - work->attribute_key;
    /* Matching attribute keys sample the pack first; other keys take the static-chain path below. */
    if (work->attribute_difference == 0) {
        end_index = JNT_hairID(work);
        while (work->current_index < end_index) {
            next_channel = JNT_getVal(work, next_channel, frame);
            if ((work->flags & JNT_WORK_PACK_ENDED) || work->channel_start_index >= 0) {
                break;
            }
        }
        if (work->flags & JNT_WORK_PACK_ENDED) {
            while (work->current_index < end_index) {
                next_channel = JNT_getStaticVal(work, next_channel);
            }
        } else if (work->channel_start_index >= 0) {
            end_index = work->current_index;
            work->current_index = 1;
            next_channel = channel + 1;
            while (work->current_index < end_index) {
                next_channel = JNT_constructMatrix(work, next_channel);
            }
            clip_joint = &work->matrix_buffer[work->channel_start_index];
            dynamic_clip_position.x = dynamic_clip_position.y =
                dynamic_clip_position.z = 0.0f;
            if (work->clip_radius <= 0.0f) {
                dynamic_clip_position.w = 2.0f;
            } else {
                dynamic_clip_position.w = work->clip_radius;
            }
            MATRIX_convert4(dynamic_clip_basis.elements, work->root_matrix.elements);
            dynamic_clip_basis.elements[3][0] = 0.0f;
            dynamic_clip_basis.elements[3][1] = 0.0f;
            dynamic_clip_basis.elements[3][2] = 0.0f;
            MATRIX_convert4(dynamic_clip_joint.elements, clip_joint->elements);
            xglMatrixMul(dynamic_clip_result.elements, dynamic_clip_basis.elements,
                         dynamic_clip_joint.elements);
            work->root_translation.x = dynamic_clip_result.elements[3][0];
            work->root_translation.y = dynamic_clip_result.elements[3][1];
            work->root_translation.z = dynamic_clip_result.elements[3][2];
            dynamic_clip_position.x = dynamic_clip_result.elements[3][0] += work->root_matrix.elements[3][0];
            dynamic_clip_position.y = dynamic_clip_result.elements[3][1] += work->root_matrix.elements[3][1];
            dynamic_clip_position.z = dynamic_clip_result.elements[3][2] += work->root_matrix.elements[3][2];
            /* Clip test on the inherit channel: a failing test marks the run, and it returns early unless flag 0x100 is set. */
            if (nmlModelCalcClip(&dynamic_clip_position) != 0 || (work->flags & 0x200)) {
                if (!(work->flags & 0x100)) {
                    work->flags |= 0x40;
                    if ((animation_flags & 0x10000000) || (work->flags & 0x200)) {
                        work->flags |= 0x44;
                        return 0;
                    }
                }
            }
            end_index = JNT_hairID(work);
            while (work->current_index < end_index) {
                next_channel = JNT_getVal(work, next_channel, frame);
                if (work->flags & JNT_WORK_PACK_ENDED) {
                    break;
                }
            }
            if (work->flags & JNT_WORK_PACK_ENDED) {
                while (work->current_index < end_index) {
                    next_channel = JNT_getStaticVal(work, next_channel);
                }
            }
        }
        JNT_setInterrupt(work, work->producer);
        end_index = work->current_index;
        work->current_index = work->channel_start_index + 1;
        next_channel = &channel[work->channel_start_index + 1];
        while (work->current_index < end_index) {
            next_channel = JNT_constructMatrix(work, next_channel);
        }
        jntCopyMatrix(&work->matrix_buffer[0], &previous_matrix);
        /* The rest of the run uses the pack (flag 0x80000000), static values (0x40000000 or no owner matrix), or EXM motion. */
        if ((int)animation_flags < 0) {
            end_index = work->hair_id_source->primary_id + work->hair_id_source->secondary_id;
            while (work->current_index < end_index) {
                next_channel = JNT_getVal2(work, next_channel, frame);
                if (work->flags & JNT_WORK_PACK_ENDED) {
                    break;
                }
            }
            if (work->flags & 2) {
                work->current_index = 1;
                next_channel = channel + 1;
                while (work->current_index < end_index) {
                    next_channel = JNT_interpolate(work, next_channel);
                }
            }
        } else if ((animation_flags & 0x40000000U) ||
                   (owner != 0 && owner->matrix_buffer == 0)) {
            end_index = work->hair_id_source->primary_id + work->hair_id_source->secondary_id;
            while (work->current_index < end_index) {
                next_channel = JNT_getStaticVal2(work, next_channel);
            }
            if (work->flags & 2) {
                work->current_index = 1;
                next_channel = channel + 1;
                while (work->current_index < end_index) {
                    next_channel = JNT_interpolate(work, next_channel);
                }
            }
        } else {
            if (work->flags & 2) {
                work->current_index = 1;
                next_channel = channel + 1;
                while (work->current_index < end_index) {
                    next_channel = JNT_interpolate(work, next_channel);
                }
                end_index = work->hair_id_source->primary_id + work->hair_id_source->secondary_id;
                saved_channel = next_channel;
                matrix_start = work->current_index;
                while (work->current_index < end_index) {
                    next_channel = JNT_interpolate(work, next_channel);
                }
                work->current_index = matrix_start;
                next_channel = saved_channel;
            }
            EXM_PresetHair(work);
            end_index = work->hair_id_source->primary_id + work->hair_id_source->secondary_id;
            while (work->current_index < end_index) {
                next_channel = EXM_CalcExMotion(work, next_channel);
            }
        }
        end_index = work->hair_id_source->primary_id;
        end_index += work->hair_id_source->secondary_id;
        end_index += work->hair_id_source->additional_primary_count;
        if (work->flags & JNT_WORK_PACK_ENDED) {
            while (work->current_index < end_index) {
                next_channel = JNT_getStaticVal2(work, next_channel);
            }
        } else {
            while (work->current_index < end_index) {
                next_channel = JNT_getVal2(work, next_channel, frame);
                if (work->flags & JNT_WORK_PACK_ENDED) {
                    break;
                }
            }
            if (work->flags & JNT_WORK_PACK_ENDED) {
                work->flags &= ~JNT_WORK_PACK_ENDED;
                while (work->current_index < end_index) {
                    next_channel = JNT_getStaticVal2(work, next_channel);
                }
            }
        }
        matrix_start = work->current_index;
        end_index = work->hair_id_source->channel_limit;
        has_matrix_range = matrix_start < end_index;
        if (has_matrix_range) {
            do {
                next_channel = JNT_getStaticVal2(work, next_channel);
            } while (work->current_index < end_index);
        }
        work->current_index = matrix_start;
        MATRIX_convert4(previous_matrix.elements, previous_matrix.elements);
        work->current_index = matrix_start;
        if (has_matrix_range) {
            do {
                MATRIX_convert4(source_matrix.elements,
                                work->matrix_buffer[work->current_index].elements);
                xglMatrixMul(result_matrix.elements, previous_matrix.elements, source_matrix.elements);
                jntCopyMatrix(&work->matrix_buffer[work->current_index], &result_matrix);
                work->current_index++;
            } while (work->current_index < end_index);
        }
    } else {
        end_index = JNT_hairID(work);
        while (work->current_index < end_index) {
            next_channel = JNT_getVal_staticChain(work, next_channel, frame);
            if ((work->flags & JNT_WORK_PACK_ENDED) || work->channel_start_index >= 0) {
                break;
            }
        }
        if (work->flags & JNT_WORK_PACK_ENDED) {
            while (work->current_index < end_index) {
                next_channel = JNT_getStaticVal(work, next_channel);
            }
        } else if (work->channel_start_index >= 0) {
            end_index = work->current_index;
            next_channel = channel;
            work->current_index = 0;
            while (work->current_index < end_index) {
                next_channel = JNT_constructMatrix(work, next_channel);
            }
            clip_joint = &work->matrix_buffer[work->channel_start_index];
            static_clip_position.x = static_clip_position.y = static_clip_position.z = 0.0f;
            if (work->clip_radius <= 0.0f) {
                static_clip_position.w = 2.0f;
            } else {
                static_clip_position.w = work->clip_radius;
            }
            MATRIX_convert4(static_clip_basis.elements, work->root_matrix.elements);
            static_clip_basis.elements[3][0] = 0.0f;
            static_clip_basis.elements[3][1] = 0.0f;
            static_clip_basis.elements[3][2] = 0.0f;
            MATRIX_convert4(static_clip_joint.elements, clip_joint->elements);
            xglMatrixMul(source_matrix.elements, static_clip_basis.elements, static_clip_joint.elements);
            work->root_translation.x = source_matrix.elements[3][0];
            work->root_translation.y = source_matrix.elements[3][1];
            work->root_translation.z = source_matrix.elements[3][2];
            static_clip_position.x = source_matrix.elements[3][0] += work->root_matrix.elements[3][0];
            static_clip_position.y = source_matrix.elements[3][1] += work->root_matrix.elements[3][1];
            static_clip_position.z = source_matrix.elements[3][2] += work->root_matrix.elements[3][2];
            if (nmlModelCalcClip(&static_clip_position) != 0) {
                work->flags |= 0x40;
                if ((animation_flags & 0x10000000) || (work->flags & 0x200)) {
                    work->flags |= 0x44;
                    return 0;
                }
            }
            end_index = JNT_hairID(work);
            while (work->current_index < end_index) {
                next_channel = JNT_getVal_staticChain(work, next_channel, frame);
                if (work->flags & JNT_WORK_PACK_ENDED) {
                    break;
                }
            }
            if (work->flags & JNT_WORK_PACK_ENDED) {
                while (work->current_index < end_index) {
                    next_channel = JNT_getStaticVal(work, next_channel);
                }
            }
        }
        JNT_setInterrupt(work, work->producer);
        end_index = work->current_index;
        work->current_index = work->channel_start_index + 1;
        next_channel = &channel[work->channel_start_index + 1];
        while (work->current_index < end_index) {
            next_channel = JNT_constructMatrix(work, next_channel);
        }
        if ((animation_flags & 0x40000000U) ||
            (owner != 0 && owner->matrix_buffer == 0)) {
            end_index = work->hair_id_source->primary_id + work->hair_id_source->secondary_id;
            while (work->current_index < end_index) {
                next_channel = JNT_getStaticVal2(work, next_channel);
            }
            if (work->flags & 2) {
                work->current_index = 1;
                next_channel = channel + 1;
                while (work->current_index < end_index) {
                    next_channel = JNT_interpolate(work, next_channel);
                }
            }
        } else {
            if (work->flags & 2) {
                work->current_index = 1;
                next_channel = channel + 1;
                while (work->current_index < end_index) {
                    next_channel = JNT_interpolate(work, next_channel);
                }
                end_index = work->hair_id_source->primary_id + work->hair_id_source->secondary_id;
                saved_channel = next_channel;
                matrix_start = work->current_index;
                while (work->current_index < end_index) {
                    next_channel = JNT_interpolate(work, next_channel);
                }
                work->current_index = matrix_start;
                next_channel = saved_channel;
            }
            EXM_PresetHair(work);
            end_index = work->hair_id_source->primary_id + work->hair_id_source->secondary_id;
            while (work->current_index < end_index) {
                next_channel = EXM_CalcExMotion(work, next_channel);
            }
        }
        end_index += work->hair_id_source->additional_primary_count;
        while (work->current_index < end_index) {
            next_channel = JNT_getStaticVal2(work, next_channel);
        }
        matrix_start = work->current_index;
        end_index += work->hair_id_source->additional_secondary_count;
        has_matrix_range = matrix_start < end_index;
        if (has_matrix_range) {
            do {
                next_channel = JNT_getStaticVal2(work, next_channel);
            } while (work->current_index < end_index);
        }
        jntCopyMatrix(&work->matrix_buffer[0], &previous_matrix);
        MATRIX_convert4(previous_matrix.elements, previous_matrix.elements);
        work->current_index = matrix_start;
        if (has_matrix_range) {
            do {
                MATRIX_convert4(static_source_matrix.elements,
                                work->matrix_buffer[work->current_index].elements);
                xglMatrixMul(result_matrix.elements, previous_matrix.elements, static_source_matrix.elements);
                jntCopyMatrix(&work->matrix_buffer[work->current_index], &result_matrix);
                work->current_index++;
            } while (work->current_index < end_index);
        }
    }
    return next_channel;
}

void JNT_onSmoothHair(void)
{
    flagSmoothHair = 1;
}

void JNT_offSmoothHair(void)
{
    flagSmoothHair = 0;
}
