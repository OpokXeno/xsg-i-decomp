#include "common.h"

#include "shared.h"

#include "jnt.h"

typedef struct JntHairIdSource {
    unsigned char unmodeled_00[6];
    unsigned short channel_limit;
    unsigned short primary_id;   /* 0x08 */
    unsigned short secondary_id; /* 0x0A */
} JntHairIdSource;

typedef struct JntChannelTag {
    int flag; /* 0x00 */
    int tag;  /* 0x04 */
} JntChannelTag;

typedef struct JntChannelWork {
    unsigned char unmodeled_000[0x28];
    JntHairIdSource *hair_id_source; /* 0x28 */
    unsigned char unmodeled_02c[0x7dc];
    JntChannelTag *tag;              /* 0x808 */
} JntChannelWork;

typedef struct JntInterruptElement JntInterruptElement;

struct JntInterruptElement {
    float scale[3];
    unsigned char unmodeled_0c[4];
    float rotation[3];
    unsigned char unmodeled_1c[4];
    float translation[3];
    unsigned char unmodeled_2c[4];
    unsigned short key;
    unsigned short interrupt_channel; /* 0x32, the channel that interrupts this element */
    void *attribute_data;
    unsigned char unmodeled_38[8];
};

typedef struct JntInterruptWork JntInterruptWork;

struct JntInterruptWork {
    unsigned char unmodeled_000[0x818];
    JntInterruptElement *elements; /* 0x818 */
};

typedef struct JntChannelList JntChannelList;

struct JntChannelList {
    int count;                 /* 0x00 */
    int channel_elements[8];   /* 0x04, indexed by channel */
};

typedef struct JntAccessoryRecord {
    unsigned short size;
    unsigned short type;
} JntAccessoryRecord;

typedef struct JntMoveResource {
    unsigned char unmodeled_00[0x10];
    int accessory_size;
} JntMoveResource;

typedef void (*JntFilterFn)(void *work, void *param, int flag);

typedef struct JntFilterList {
    int count;
    int flags[8];
    JntFilterFn filters[8];
} JntFilterList;

/*
 * The JNT producer's work area is in the EE scratchpad (SPR, 0x70000000).
 * Two of its slots are named here, because this file's two recovered
 * functions are their writers; the area itself is not recovered, so the
 * slots are named relative to a base the caller materializes rather than
 * modeled as members (see the block comment on JNT_setMatrix).
 *
 * The base has to stay a value the function holds: folding it into each
 * store's address instead (`*(T *)0x700007a0`) makes 2.96 emit the two
 * absolute stores as separate li/sw pairs, four instructions in place of the
 * original's single shared `lui $2,0x7000` and two displaced stores.
 */

#define JNT_SCRATCH_BASE ((void *)0x70000000)

#define JNT_MATRIX_BUFFER_OFFSET 0x7A0

#define JNT_MATRIX_SELECT_OFFSET 0x4BC

#define JNT_MATRIX_BUFFER(scratch) (((JntWork *)(scratch))->matrix_buffer)

#define JNT_MATRIX_SELECT(scratch) (((JntWork *)(scratch))->matrix_select)

/*
 * Three more scratchpad slots share a fixed layout with the pair above, so
 * they are named with a partial struct instead of one offset macro per
 * field: JNT_setInterpMatrix stores its matrix-buffer argument at 0x7A4 and
 * its float argument at 0x800 (interp_matrix / interpolation below).
 *
 * JNT_setCurve stores its two pointer arguments at 0x7A8/0x7AC. Neither is
 * an end pointer: this same TU's JNT_getStaticVal (VA 0x0030EDC8) and
 * JNT_getVal (VA 0x0030F038) read them back as two independent per-element
 * 32-byte record tables, each gated on both slots being non-null and then
 * indexed by element_index*32 (the element's own 0x2C field):
 *   - JNT_getStaticVal: `sll $3,$11,5 / addu $3,$3,$5` with
 *     $5 = lw 0x7A8($10) -- 0x7A8 is the table it reads, static_value_records.
 *   - JNT_getVal: `sll $2,$6,5 / addu $3,$2,$3` with
 *     $3 = lw 0x7AC($17) -- 0x7AC is the table it reads, value_records.
 */

typedef struct JntProductionOwner JntProductionOwner;
typedef struct JntTransformRecord {
    Vector4 translation;
    Vector4 rotation;
} JntTransformRecord;

typedef struct JntWork {
    JntProducer *producer;         /* +0x000 */
    unsigned short *cursor;        /* +0x004: FCV2 packed-curve state */
    unsigned char unmodeled_008[0x28 - 0x08];
    JntHairIdSource *hair_id_source;
    /*
     * JNT_getStaticVal2/JNT_getVal2 read this as the default matrix slot for
     * their CUR_MATRIX_Get call and unconditionally increment it once per
     * call (`lw/sw ...,44($.)`).
     */
    int current_index; /* 0x2C */
    void *channels;                /* +0x030 */
    unsigned char unmodeled_034[4];
    unsigned int flags;            /* +0x038 */
    unsigned short channel_index;  /* +0x03c */
    unsigned char unmodeled_03e[2];
    Vector4 root_translation;      /* +0x040 */
    Vector4 root_rotation;         /* +0x050 */
    Vector4 root_scale;            /* +0x060 */
    unsigned char unmodeled_070[0x4bc - 0x70];
    JntMatrixSelect matrix_select;
    unsigned short matrix_index;   /* +0x4c0 */
    unsigned char unmodeled_4c2[0x7a0 - 0x4c2];
    /*
     * JNT_getStaticVal2/JNT_getVal2 read this as the matrix table their
     * CUR_MATRIX_Set/Get calls index by joint slot (`lw ...,1952($.)`);
     * JNT_setMatrix/JNT_setMatrix2 write it through JNT_MATRIX_BUFFER_OFFSET
     * above (same 0x7A0 slot, kept as a raw offset there for the codegen
     * reason given on that macro).
     */
    JntMatrix *matrix_buffer; /* 0x7A0 */
    JntMatrixBuffer interp_matrix; /* 0x7A4, JNT_setInterpMatrix */
    JntTransformRecord (*static_value_records)[];    /* 0x7A8, JNT_setCurve arg0; read by JNT_getStaticVal */
    void *value_records;           /* 0x7AC, JNT_setCurve arg1; read by JNT_getVal */
    unsigned char unmodeled_7b0[0x7f0 - 0x7b0];
    unsigned int attribute_key;
    unsigned char unmodeled_7f4[8];
    float current_frame;           /* +0x7fc */
    float interpolation; /* 0x800, JNT_setInterpMatrix */
    unsigned char unmodeled_804[4];
    unsigned char *current_value;  /* +0x808 */
    char *attribute_name;          /* +0x80c */
    unsigned int animation_flags;  /* +0x810 */
    unsigned char unmodeled_814[4];
    JntInterruptElement *elements;
    int channel_start_index;       /* +0x81c */
    JntProductionOwner *owner;     /* +0x820 */
} JntWork;

/*
 * A joint element is a fixed-size record: JNT_getElement indexes an array of
 * them and JNT_nextElement advances by exactly one. The layout beyond that
 * 0x40-byte stride is not recovered, so it stays a single unmodeled span
 * (docs/naming.md) rather than invented members.
 */

typedef struct JntElement {
    unsigned char unmodeled_00[0x40];
} JntElement;

/*
 * The joint resource header JNT_getRootElement reads: 0x10 bytes not
 * recovered here, then a self-relative offset field whose own address plus
 * its value is the joint's root element.
 */

typedef struct JntElementHeader {
    unsigned char unmodeled_00[0x10];
    int root_element_offset;
} JntElementHeader;

/*
 * Another partial view of the same scratchpad work area as JntWork (it is
 * one layout; JntWork does not model these slots): the general flags word
 * at 0x38, which JNT_setFlags writes and JNT_getFlags reads (`sw $4,56($1)`
 * / `lw $2,56($2)` off the 0x70000000 base). Callers ACT_resetMatrix,
 * ACT_updateMotionSub, ACT_modelDrawSub, ACT_setMotion and ACT_setMotion2
 * read/write it around the same joint update the matrix/curve slots above
 * serve.
 */

typedef struct JntFlagsWork {
    unsigned char unmodeled_000[0x38];
    int flags;
} JntFlagsWork;

/*
 * A view of the same work area for two adjacent slots past the end of
 * JntWork:
 * JNT_animSetFlags writes the animation flags word at 0x810
 * (`sw $4,2064($1)`) and JNT_setClipR writes the clipping radius right
 * after it at 0x814 (`swc1 $f12,2068($1)`); both are supplied by
 * ACT_updateMotionSub (JNT_animSetFlags also by ACT_modelDrawSub).
 */

typedef struct JntAnimWork {
    unsigned char unmodeled_000[0x810];
    int anim_flags;
    float clip_radius;
} JntAnimWork;

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

/*
 * The matrix_print current-matrix API transfers native 64-byte matrices,
 * loads a 16-byte translation payload, and transports rotation words into
 * VU0 without a numerical integer conversion.
 */

void CUR_MATRIX_Set(Matrix4 matrix);

void CUR_MATRIX_Get(Matrix4 matrix);

void CUR_MATRIX_Txyz4s(const Vector4 *translate);

void CUR_MATRIX_Rzyx4s(const int *rotate);

/* Static-value copying uses floating point, while the matrix API transports
 * these same angle representations as raw words into VU0. */
typedef union JntRotationPayload {
    float values[3];
    int bits[3];
} JntRotationPayload;

/*
 * A joint static-value channel: JNT_getStaticVal2 reads its type (dispatch,
 * only types 1-5 apply a transform), its target joint slot and the
 * translate/rotate triples it feeds to CUR_MATRIX_Txyz4s/Rzyx4s, then
 * returns the following channel (`channel + 1`), which fixes its size at
 * 0x40 bytes, the same stride as JntElement.
 */

typedef struct JntStaticChannel {
    unsigned short type;           /* 0x00 */
    unsigned char unmodeled_02[0x0C];
    unsigned short matrix_index;   /* 0x0E */
    float translate[3];            /* 0x10 */
    unsigned char unmodeled_1c[4];
    JntRotationPayload rotate;     /* 0x20 */
    unsigned char unmodeled_2C[0x14];
} JntStaticChannel;

struct JntStaticChannel;
typedef void *(*JntProductionConsumer)(JntWork *work,
                                      struct JntStaticChannel *channels,
                                      int argument);

struct JntProducer {
    int count;              /* 0x00 */
    unsigned char unmodeled_04[0x20];
    JntProductionConsumer consumers[8]; /* 0x24, called by JNT_startProduction */
    int consumer_arguments[8];         /* 0x44 */
    int pending_output;     /* 0x64 */
    int output_count;       /* 0x68, only evidenced as cleared by JNT_initProducer */
    int current_index;      /* 0x6C, -1 when none */
    int state;              /* 0x70, cleared by JNT_startProduction */
};

/*
 * Hair-smoothing toggle at VA 0x004DC248: JNT_computeMatrix reads it with
 * `lb $2,flagSmoothHair` (VA 0x00314B14) to decide whether to apply the
 * interpolated hair matrix; this pair only sets/clears it.
 */

static signed char flagSmoothHair = 0;

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

char *strncpy(char *destination, const char *source, unsigned int count);

float FCV2_getPackValue(unsigned short **cursor, float frame);

static unsigned char dummyAttr[0x10] = {0};

struct FcvPackAttribute {
    unsigned char unmodeled_00[6];
    unsigned short value_mask; /* 0x06 */
};

struct FcvPackAttribute *FCV2_getPackAttribute(unsigned short **cursor);

unsigned char dummyCntFree[0x10] = {0};

char dummyName[0x10] = {0};

unsigned short *JNT_setMDLMatrix(JntWork *work,
                                 unsigned short *channel);

void FCV2_resetPack(void *pack_state, void *pack);

struct JntProductionOwner {
    unsigned char unmodeled_000[0xA60];
    JntMatrix *matrix_buffer; /* 0xA60 */
};

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

void get_fcvpack_fix(float *destination, JntWork *pack,
                     unsigned int pack_mask, float default_value, float frame)
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

    switch (type) {
    case 5:
        {
            float *translation = element->translation;
            float *rotation = element->rotation;
            const float *translate = channel->translate;
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
        float *translation = element->translation;
        float *rotation = element->rotation;
        const float *translate = channel->translate;
        const float *rotate = channel->rotate.values;

        translation[0] = translate[0];
        translation[1] = translate[1];
        translation[2] = translate[2];
        rotation[0] = rotate[0];
        rotation[1] = rotate[1];
        rotation[2] = rotate[2];
        element->scale[2] = 1.0f;
        element->scale[1] = 1.0f;
        element->scale[0] = 1.0f;
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
        const float *translate = channel->translate;
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

INCLUDE_ASM("asm/main/nonmatchings/jnt", JNT_getVal);

INCLUDE_ASM("asm/main/nonmatchings/jnt", JNT_getVal_staticChain);

INCLUDE_ASM("asm/main/nonmatchings/jnt", JNT_constructMatrix);

void *JNT_getStaticVal2(JntWork *work, JntStaticChannel *channel)
{
    int type;
    const void *translate;
    const int *rotate;
    void *next;
    void *matrix;

    type = channel->type;
    translate = channel->translate;
    rotate = channel->rotate.bits;
    next = channel + 1;

    if (type < 5) {
        if (type == 0) {
            goto advance_channel;
        }
    } else {
        if (type == 5) {
            matrix = &work->matrix_buffer[channel->matrix_index];
            CUR_MATRIX_Set(matrix);
            CUR_MATRIX_Txyz4s(translate);
            CUR_MATRIX_Rzyx4s(rotate);
            matrix = &work->matrix_buffer[work->current_index];
            CUR_MATRIX_Get(matrix);
        }
        goto advance_channel;
    }
    matrix = &work->matrix_buffer[channel->matrix_index];
    CUR_MATRIX_Set(matrix);
    CUR_MATRIX_Txyz4s(translate);
    CUR_MATRIX_Rzyx4s(rotate);
    matrix = &work->matrix_buffer[work->current_index];
    CUR_MATRIX_Get(matrix);

advance_channel:
    work->current_index++;
    return next;
}

INCLUDE_ASM("asm/main/nonmatchings/jnt", JNT_getVal2);

INCLUDE_ASM("asm/main/nonmatchings/jnt", JNT_getVal_HOGE2);

static void liner_interpolate(JntMatrix *matrix, const JntMatrix *other_matrix, float blend)
{
    /* Move the scalar blend from COP1 through v1 into VU0's x broadcast. */
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

    /* Keep a nop in the return delay slot after the vector stores. */
    __asm__ __volatile__(
        "sqc2 $vf19, 0x00(%0)\n"
        "sqc2 $vf20, 0x10(%0)\n"
        "sqc2 $vf21, 0x20(%0)\n"
        "sqc2 $vf24, 0x30(%0)\n"
        "nop"
        :
        : "r"(matrix)
        : "memory");
}

INCLUDE_ASM("asm/main/nonmatchings/jnt", JNT_interpolate);

void JNT_setFlags(int flags)
{
    JntFlagsWork *work = (JntFlagsWork *)JNT_SCRATCH_BASE;
    work->flags = flags;
}

void JNT_setClipR(float clip_radius)
{
    JntAnimWork *work = (JntAnimWork *)JNT_SCRATCH_BASE;
    work->clip_radius = clip_radius;
}

void JNT_animSetFlags(int anim_flags)
{
    JntAnimWork *work = (JntAnimWork *)JNT_SCRATCH_BASE;
    work->anim_flags = anim_flags;
}

int JNT_getFlags(void)
{
    JntFlagsWork *work = (JntFlagsWork *)JNT_SCRATCH_BASE;
    return work->flags;
}

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

void JNT_readAttribute(void *scratch_address, void *attribute_buffer)
{
    typedef struct JntAttributeRecord {
        unsigned short size;
        unsigned short type;
        char value[8];
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
            workspace->current_value = (unsigned char *)record->value;
            break;
        case 1:
            {
                const unsigned int *value_words =
                    (const unsigned int *)record->value;

                workspace->attribute_key =
                    value_words[0] * 31 + value_words[1];
                strncpy(workspace->attribute_name, record->value, 8);
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
    unsigned char *scratch = JNT_SCRATCH_BASE;
    JNT_MATRIX_BUFFER(scratch) = matrix_buffer;
    JNT_MATRIX_SELECT(scratch) = 0;
}

void JNT_setMatrix2(JntMatrixBuffer matrix_buffer, JntMatrixSelect matrix_select)
{
    unsigned char *scratch = JNT_SCRATCH_BASE;
    JNT_MATRIX_BUFFER(scratch) = matrix_buffer;
    JNT_MATRIX_SELECT(scratch) = matrix_select;
}

void JNT_setInterpMatrix(JntMatrixBuffer matrix_buffer, float interpolation)
{
    JntWork *work = (JntWork *)JNT_SCRATCH_BASE;
    work->interp_matrix = matrix_buffer;
    work->interpolation = interpolation;
}

void JNT_setCurve(void *static_value_records, void *value_records)
{
    JntWork *work = (JntWork *)JNT_SCRATCH_BASE;
    work->static_value_records = static_value_records;
    work->value_records = value_records;
}

INCLUDE_ASM("asm/main/nonmatchings/jnt", JNT_setMDLMatrix);

void JNT_setModelMatrix(void)
{
    JntWork *work = JNT_SCRATCH_BASE;
    void *channel;
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
    int *root_element_offset = &((JntElementHeader *)joint)->root_element_offset;
    return (unsigned char *)root_element_offset + *root_element_offset;
}

INCLUDE_ASM("asm/main/nonmatchings/jnt", JNT_setModel);

INCLUDE_ASM("asm/main/nonmatchings/jnt", JNT_setFCurve);

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

INCLUDE_ASM("asm/main/nonmatchings/jnt", JNT_addConsumer);

void JNT_initProducer(JntProducer *producer)
{
    int i;

    producer->pending_output = 0;
    producer->count = 0;
    producer->output_count = 0;
    producer->current_index = -1;
    for (i = 7; i >= 0; i--) {
        producer->consumers[i] = 0;
    }
}

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
    producer->state = 0;
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
        if (consumer_address - 0x001F0000 <= 0x01E0FFFF &&
            (consumer_address & 3) == 0) {
            consumer((JntWork *)JNT_SCRATCH_BASE, channels, consumer_argument);
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
    return &((JntElement *)elements)[index];
}

void *JNT_nextElement(void *element)
{
    return (JntElement *)element + 1;
}

INCLUDE_ASM("asm/main/nonmatchings/jnt", JNT_defaultConsumer);

void JNT_resetHair(void)
{
    JntWork *work = (JntWork *)JNT_SCRATCH_BASE;
    JntHairIdSource *source = work->hair_id_source;
    int first = source->primary_id;
    int end;
    JntStaticChannel *channels = work->channels;
    JntStaticChannel *channel = &channels[first];

    work->current_index = first;
    end = first + source->secondary_id;
    if (first < end) {
        do {
            channel = JNT_getStaticVal2((JntWork *)JNT_SCRATCH_BASE, channel);
        } while (work->current_index < end);
    }
}

static int JNT_hairID(JntChannelWork *work)
{
    JntChannelTag *tag = work->tag;
    JntHairIdSource *source = work->hair_id_source;
    int id;

    id = source->primary_id;
    if (tag->flag == 0) {
        id += source->secondary_id;
    }
    if ((tag->tag & 0xFFFF0000) == 0x008A0000) {
        id += tag->tag & 0xFFFF;
    }
    return id;
}

INCLUDE_ASM("asm/main/nonmatchings/jnt", JNT_computeHair);

int JNT_setInterrupt(JntInterruptWork *work, JntChannelList *list)
{
    int count = list->count;
    int result = 0;
    int channel;

    work->elements[0].interrupt_channel = 0;
    for (channel = 1; channel < count; channel++) {
        int element = list->channel_elements[channel];
        if (element != 0 && (element & 0x8000) == 0) {
            work->elements[element].interrupt_channel = channel;
            result++;
        }
    }

    return result;
}

JntFilterFn JNT_getFilter(void *work, JntFilterList *list)
{
    int count = list->count;
    int index = 1;

    if (index < count) {
        int remaining = count;
        JntFilterFn *filter = &list->filters[1];
        int *flag = &list->flags[1];
        for (;;) {
            if ((*flag & 0x8000) != 0) {
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

INCLUDE_ASM("asm/main/nonmatchings/jnt", JNT_resetMatrix);

INCLUDE_ASM("asm/main/nonmatchings/jnt", JNT_computeMatrix);

void JNT_onSmoothHair(void)
{
    flagSmoothHair = 1;
}

void JNT_offSmoothHair(void)
{
    flagSmoothHair = 0;
}
