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
    unsigned char unmodeled_000[0x28];
    struct JntHairIdSource *hair_id_source;
    int current_index;
    unsigned char unmodeled_030[8];
    unsigned int flags;
    unsigned char unmodeled_03c[0xa0 - 0x3c];
    ExMotionJointState channels[16];
    unsigned char unmodeled_4a0[0x20];
    unsigned short matrix_index;
    short saved_index;
    unsigned char unmodeled_4c4[0x4d0 - 0x4c4];
    Matrix saved_matrix; /* +0x4d0 */
    unsigned char unmodeled_510[0x720 - 0x510];
    Matrix root_matrix; /* +0x720 */
    unsigned char unmodeled_760[0x7a0 - 0x760];
    struct JntMatrix *matrix_buffer;
    unsigned char unmodeled_7a4[0x80c - 0x7a4];
    signed char *hair_joint_name;
    unsigned int animation_flags;
    unsigned char unmodeled_814[0x820 - 0x814];
    struct Actor *owner;
} ExMotionJointWork;

void EXM_PresetHair(ExMotionJointWork *work);

#endif /* INCLUDE_MAIN_EXM_1_H */
