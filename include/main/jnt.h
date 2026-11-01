#ifndef INCLUDE_MAIN_JNT_H
#define INCLUDE_MAIN_JNT_H

typedef struct JntWork JntWork;

typedef struct JntProducer JntProducer;

typedef struct JntStaticChannel JntStaticChannel;

typedef void *(*JntProductionConsumer)(JntWork *work,
                                      JntStaticChannel *channels,
                                      int argument);

typedef struct JntInterruptElement JntInterruptElement;

struct JntInterruptWork {
    unsigned char unmodeled_000[0x818];
    JntInterruptElement *elements; /* 0x818 */
};

struct JntChannelList {
    int count;                 /* 0x00 */
    int channel_elements[8];   /* 0x04, indexed by channel */
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
    float translate[4];            /* 0x10: quadword translation payload */
    JntRotationPayload rotate;     /* 0x20 */
    unsigned char unmodeled_2C[0x0C];
    float translation_direction;  /* 0x38: sign selects the chain direction */
    unsigned char unmodeled_3c[4];
};

int JNT_addConsumer(JntProducer *producer, int consumer_id,
                    JntProductionConsumer consumer, int argument);

JntStaticChannel *JNT_resetMatrix(JntWork *work, JntStaticChannel *channel);

#endif /* INCLUDE_MAIN_JNT_H */
