#ifndef INCLUDE_MAIN_JNT_H
#define INCLUDE_MAIN_JNT_H

#include "shared.h"

typedef struct JntMatrix JntMatrix;

/*
 * JNT_getStaticVal2/JNT_getVal2 index this type by joint slot
 * (`matrix_buffer + index*sizeof(JntMatrix)`, `sll ...,0x6`), so its size is
 * evidenced as 0x40 bytes. The joint tables are read as 4x4 floats by the
 * motion code and as a flat float[16] by JC_face and friends (the w-column
 * stores at [3], [7], [11]). The original accessed them through a union of
 * both views (alias set 0): JC_face/JC_setParent* only reproduce the store
 * order against the quadword copies that way. Unlike xgl_2.h's Matrix there
 * is no quadword member, since that would 16-align the records inside
 * JntWork (shifting every later field).
 */
struct JntMatrix {
    union {
        Matrix4 elements;
        float flat[16];
    };
};

typedef JntMatrix *JntMatrixBuffer;

typedef unsigned int JntMatrixSelect;

typedef struct JntHairIdSource {
    unsigned char unmodeled_00[4];
    unsigned short version;
    unsigned short channel_limit;
    unsigned short primary_id;   /* 0x08 */
    unsigned short secondary_id; /* 0x0A */
    unsigned short additional_primary_count;   /* 0x0C */
    unsigned short additional_secondary_count; /* 0x0E */
    int channel_offset;
} JntHairIdSource;

typedef struct JntChainSlot {
    struct FcvPackAttribute *attribute; /* 0x00 */
    unsigned char unmodeled_04[0x0c];
    Vector4 translation;           /* 0x10 */
    Vector4 scale;                 /* 0x20 */
    Vector4 rotation;              /* 0x30 */
} JntChainSlot;

typedef struct JntProductionOwner JntProductionOwner;

typedef struct JntTransformRecord {
    Vector4 translation;
    Vector4 rotation;
} JntTransformRecord;

typedef struct JntProducer JntProducer;

typedef struct JntInterruptElement JntInterruptElement;

struct JntInterruptElement {
    Vector4 scale;                   /* 0x00 */
    float rotation[3];               /* 0x10 */
    unsigned char unmodeled_1c[4];
    Vector4 translation;             /* 0x20 */
    unsigned short key;
    unsigned short interrupt_channel; /* 0x32, the channel that interrupts this element */
    void *attribute_data;
    unsigned char unmodeled_38[8];
};

struct JntWork {
    JntProducer *producer;         /* +0x000 */
    unsigned short *cursor;        /* +0x004: FCV2 packed-curve state */
    unsigned char unmodeled_008[0x10 - 0x08];
    unsigned int requested_attribute_key; /* 0x10 */
    unsigned char unmodeled_014[0x28 - 0x14];
    struct JntHairIdSource *hair_id_source;
    /*
     * JNT_getStaticVal2/JNT_getVal2 read this as the default matrix slot for
     * their CUR_MATRIX_Get call and unconditionally increment it once per
     * call (`lw/sw ...,44($.)`).
     */
    int current_index; /* 0x2C */
    struct JntStaticChannel *channels; /* +0x030 */
    void *model;
    /*
     * General flags word: JNT_setFlags writes it and JNT_getFlags reads it
     * (`sw $4,56($1)` / `lw $2,56($2)` off the 0x70000000 base). Callers
     * ACT_resetMatrix, ACT_updateMotionSub, ACT_modelDrawSub, ACT_setMotion
     * and ACT_setMotion2 read and write it around the joint update.
     */
    unsigned int flags;            /* +0x038 */
    unsigned short channel_index;  /* +0x03c */
    unsigned char unmodeled_03e[2];
    Vector4 root_translation;      /* +0x040 */
    Vector4 root_rotation;         /* +0x050 */
    Vector4 root_scale;            /* +0x060 */
    Vector4 channel_translation; /* 0x070 */
    Vector4 channel_rotation;    /* 0x080 */
    Vector4 channel_scale;       /* 0x090 */
    /* 0x0A0: 0x40-byte chain records; logical capacity is unrecovered. */
    JntChainSlot chain_slots[16];
    unsigned char unmodeled_4a0[0x4bc - 0x4a0];
    JntMatrixSelect matrix_select;
    unsigned short matrix_index;   /* +0x4c0 */
    short chain_base_index;        /* 0x4C2 */
    unsigned char unmodeled_4c4[0x4d0 - 0x4c4];
    Matrix4 transform_basis; /* 0x4D0: basis and chain origin */
    unsigned char unmodeled_510[0x720 - 0x510];
    JntMatrix root_matrix;
    JntMatrix interpolation_root;
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
    Matrix4 root_correction;       /* 0x7B0 */
    unsigned int attribute_key;
    unsigned int attribute_difference; /* 0x7F4 */
    unsigned char unmodeled_7f8[4];
    float current_frame;           /* +0x7fc */
    float interpolation; /* 0x800, JNT_setInterpMatrix */
    float chain_length;            /* 0x804 */
    struct JntChannelTag *current_value; /* +0x808: an attribute's tag words */
    char *attribute_name;          /* +0x80c */
    /*
     * JNT_animSetFlags writes the animation flags word at 0x810 and
     * JNT_setClipR writes the clipping radius right after it at 0x814;
     * ACT_updateMotionSub supplies both (JNT_animSetFlags also from
     * ACT_modelDrawSub).
     */
    unsigned int animation_flags;  /* +0x810 */
    float clip_radius; /* 0x814 */
    JntInterruptElement *elements;
    int channel_start_index;       /* +0x81c */
    JntProductionOwner *owner;     /* +0x820 */
    unsigned char unmodeled_824[0x0c];
    JntInterruptElement default_element; /* +0x830: default element storage */
};

typedef struct JntWork JntWork;

typedef struct JntStaticChannel JntStaticChannel;

typedef void *(*JntProductionConsumer)(JntWork *work,
                                      JntStaticChannel *channels,
                                      int argument);

/*
 * The joint producer embedded in Actor at +0x790 (Actor.jointProducer, the
 * only producer JNT_initProducer is given). Its tail words are the actor's
 * per-model caches that JNT_initProducer resets: ACT_getAccessories caches
 * JNT_getAccessories in accessory_joints, ACT_getMoveElement caches
 * JNT_getMoveElement in move_element_id (-1 = not looked up yet).
 */
struct JntProducer {
    int count;              /* 0x00 */
    int consumer_ids[8];
    JntProductionConsumer consumers[8]; /* 0x24, called by JNT_startProduction */
    int consumer_arguments[8];         /* 0x44 */
    int pending_output;     /* 0x64: work->flags after a production; ACT_updateMotionSub tests 0x10/0x40 */
    struct ActorAccessoryJointTable *accessory_joints; /* 0x68 */
    int move_element_id;    /* 0x6C */
    /* 0x70: entries in deferred_limits; JNT_startProduction clears it and
     * JNT_getVal2/JNT_getVal_HOGE2 append the shape binding descriptors that
     * follow a channel's rotation-limit header, which ACT_modelDrawSub walks. */
    int deferred_count;
    struct ActorShapeBinding *deferred_limits[8]; /* 0x74 */
};


typedef struct JntMoveResource {
    unsigned char unmodeled_00[0x10];
    int accessory_size;
} JntMoveResource;

typedef union JntRotationPayload {
    float values[3];
    int bits[3];
} JntRotationPayload;

struct JntStaticChannel {
    unsigned short type;           /* 0x00 */
    unsigned short interpolation_flags; /* 0x02 */
    int rotation_limits_offset;
    unsigned char unmodeled_08[6];
    unsigned short matrix_index;   /* 0x0E */
    Vector4 translation;           /* 0x10: quadword translation payload */
    JntRotationPayload rotate;     /* 0x20 */
    unsigned char unmodeled_2C[0x0C];
    float translation_direction;  /* 0x38: sign selects the chain direction */
    unsigned char unmodeled_3c[4];
};

int JNT_addConsumer(JntProducer *producer, int consumer_id,
                    JntProductionConsumer consumer, int argument);

JntStaticChannel *JNT_resetMatrix(JntWork *work, JntStaticChannel *channel);

/* Functions and data used by other translation units. */
void JNT_animSetFlags(int anim_flags);
void JNT_initProducer(JntProducer *producer);
void *JNT_getRootElement(void *joint);
void JNT_setFlags(int flags);
void JNT_setClipR(float clip_radius);
void JNT_setCurve(void *static_value_records, void *value_records);
void *JNT_getElement(void *elements, int index);
void *JNT_nextElement(void *element);

#endif /* INCLUDE_MAIN_JNT_H */
