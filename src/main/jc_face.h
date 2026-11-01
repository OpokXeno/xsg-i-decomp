#ifndef SRC_MAIN_JC_FACE_H
#define SRC_MAIN_JC_FACE_H

#include "main/xgl_2.h"
#include "main/jnt.h"

/* Partial defining-owner declarations of the JNT scratchpad and matrix table. */
typedef struct JntMatrix {
    unsigned char unmodeled_00[0x40];
} JntMatrix;

typedef struct JntHairIdSource {
    unsigned char unmodeled_00[6];
    unsigned short channel_limit;
    unsigned short primary_id;
    unsigned short secondary_id;
} JntHairIdSource;

typedef struct JntWork JntWork;

struct JntWork {
    unsigned char unmodeled_000[0x28];
    JntHairIdSource *hair_id_source;
    int current_index;
    unsigned char unmodeled_030[8];
    unsigned int flags;
    unsigned char unmodeled_03c[0x7a0 - 0x3c];
    JntMatrix *matrix_buffer;
};

typedef struct JcChain {
    unsigned short index;
    unsigned short static_index;
    void **source;
} JcChain;

typedef struct JcWork {
    JntProducer *producer;
    unsigned char unmodeled_004[0x0c];
    unsigned int expected_attribute_key;
    unsigned char unmodeled_014[0x14];
    JntHairIdSource *hair_id_source;
    int current_index;
    unsigned char unmodeled_030[8];
    unsigned int flags;
    unsigned char unmodeled_03c[0x7a0 - 0x3c];
    Matrix *matrix_buffer;
    unsigned char unmodeled_7a4[0x4c];
    unsigned int attribute_key;
    unsigned char unmodeled_7f4[8];
    float current_frame;
} JcWork;

typedef struct JcElement {
    unsigned short type;
    unsigned char unmodeled_02[0x0c];
    unsigned short matrix_index;
    float translate[3];
    unsigned char unmodeled_1c[0x24];
} JcElement;

typedef void (*JcFilterFn)(void *work, void *param, int flag);

typedef struct JntFilterList JntFilterList;

typedef struct JntInterruptWork JntInterruptWork;

typedef struct JntChannelList JntChannelList;

extern JcFilterFn JNT_getFilter(void *work, JntFilterList *list);
extern int JNT_setInterrupt(JntInterruptWork *work, JntChannelList *list);
extern void xglMatrixUnit4s(Matrix *matrix);
extern JcElement *JNT_getStaticVal(JcWork *work, JcElement *channel);
extern JcElement *JNT_getVal2(JcWork *work, JcElement *channel, float frame);

extern JcElement *JNT_getVal_HOGE2(JcWork *work, JcElement *channel,
                                   float frame);

extern JcElement *JNT_getVal(JcWork *work, JcElement *channel, float frame);

extern JcElement *JNT_getVal_staticChain(JcWork *work, JcElement *channel,
                                         float frame);

extern JcElement *JNT_constructMatrix(JcWork *work, JcElement *channel);
extern JcElement *JNT_interpolate(JcWork *work, JcElement *channel);
extern void *JNT_getElement(void *elements, int index);
extern void *JNT_nextElement(void *element);

extern JntStaticChannel *JNT_getStaticVal2(JntWork *work,
                                        JntStaticChannel *channel);

void *JC_copyChain(JntWork *work, void *element, JcChain *chain);

#endif
