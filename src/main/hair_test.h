#ifndef INCLUDE_MAIN_HAIR_TEST_H
#define INCLUDE_MAIN_HAIR_TEST_H

#include "common.h"
#include "main/xgl_2.h"

/* Canonical matrix storage from the matrix operation owner. */
/* Match the defining owner's storage type and field layout. */
typedef union PpVector4 {
    Vector4 vector;
    unsigned long long words[2];
} PpVector4;

typedef struct PpParticle {
    PpVector4 position;
    PpVector4 previous_position;
    PpVector4 velocity;
    float gravity;
    float damping;
} PpParticle;

#endif
