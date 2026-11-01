#ifndef INCLUDE_MAIN_HAIR_TEST_H
#define INCLUDE_MAIN_HAIR_TEST_H

#include "shared.h"
#include "main/xgl_2.h"

typedef struct JntHairDescriptor {
    unsigned char unmodeled_00[6];
    unsigned short channelCount;
    u16 hairID;
    unsigned short extraMotionCount;
    unsigned short secondaryCount;
    unsigned short staticCount;
} JntHairDescriptor;

typedef struct JntHairJoint {
    void *producer;
    unsigned char unmodeled_04[12];
    unsigned int attributeKey;
    unsigned char unmodeled_14[20];
    JntHairDescriptor *hairDescriptor;
    int currentIndex;
    unsigned char unmodeled_30[8];
    unsigned int flags;
    unsigned char unmodeled_3c[4];
    Vector4 rootTranslation;
    unsigned char unmodeled_50[0x720 - 0x50];
    Matrix rootMatrix;
    unsigned char unmodeled_760[0x7a0 - 0x760];
    Matrix *matrixBuffer;
    unsigned char unmodeled_7a4[0x7f0 - 0x7a4];
    unsigned int previousAttributeKey;
    unsigned int attributeDifference;
    unsigned char unmodeled_7f8[4];
    float currentFrame;
    unsigned char unmodeled_800[16];
    int animationFlags;
    float clipRadius;
    unsigned char unmodeled_818[4];
    int channelStartIndex;
    struct Actor *owner;
} JntHairJoint;

#endif /* INCLUDE_MAIN_HAIR_TEST_H */
