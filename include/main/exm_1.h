#ifndef INCLUDE_MAIN_EXM_1_H
#define INCLUDE_MAIN_EXM_1_H

#include "shared.h"
#include "main/xgl_2.h"

typedef struct ExMotionJointState {
    unsigned char unmodeled_00[16];
    Vector4 translation;
    unsigned char unmodeled_20[16];
    Vector4 rotation;
} ExMotionJointState;

typedef struct ExMotionJointWork {
    struct JntProducer *producer; /* +0x00 */
    unsigned char unmodeled_004[12];
    unsigned int requested_attribute_key; /* +0x10 */
    unsigned char unmodeled_014[0x28 - 0x14];
    struct JntHairIdSource *hair_id_source;
    int current_index;
    unsigned char unmodeled_030[8];
    unsigned int flags;
    unsigned char unmodeled_03c[4];
    Vector4 root_translation; /* +0x40 */
    unsigned char unmodeled_050[0xa0 - 0x50];
    ExMotionJointState channels[16];
    unsigned char unmodeled_4a0[0x20];
    unsigned short matrix_index;
    union {
        short saved_index; /* joint group base, written by EXM_CalcExMotion */
        s16 hair_type_code; /* hair type letter read by GetHairTypeSelection */
    };
    unsigned char unmodeled_4c4[0x4d0 - 0x4c4];
    Matrix saved_matrix; /* +0x4d0 */
    unsigned char unmodeled_510[0x720 - 0x510];
    Matrix root_matrix; /* +0x720 */
    unsigned char unmodeled_760[0x7a0 - 0x760];
    struct JntMatrix *matrix_buffer;
    unsigned char unmodeled_7a4[0x7f0 - 0x7a4];
    unsigned int attribute_key;        /* +0x7f0 */
    unsigned int attribute_difference; /* +0x7f4 */
    unsigned char unmodeled_7f8[4];
    float current_frame; /* +0x7fc */
    unsigned char unmodeled_800[0x80c - 0x800];
    signed char *hair_joint_name;
    unsigned int animation_flags;
    float clip_radius; /* +0x814 */
    unsigned char unmodeled_818[4];
    int channel_start_index; /* +0x81c */
    struct Actor *owner;
} ExMotionJointWork;

void EXM_PresetHair(ExMotionJointWork *work);

#endif /* INCLUDE_MAIN_EXM_1_H */
