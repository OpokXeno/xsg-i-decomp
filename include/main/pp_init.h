#ifndef INCLUDE_MAIN_PP_INIT_H
#define INCLUDE_MAIN_PP_INIT_H

#include "shared.h"

/* The particle's vectors are only 8-aligned (hair_test's cpos .bss is
 * 0x38 bytes at 2**3), so the ld/sd copy type is the shared
 * DoublewordAlignedVector4, not AlignedVector4. */
typedef struct PpParticle {
    DoublewordAlignedVector4 position;
    DoublewordAlignedVector4 previous_position;
    DoublewordAlignedVector4 velocity;
    float gravity;
    float damping;
} PpParticle;

/* Functions and data used by other translation units. */
void ppSetPos(PpParticle *particle, float x, float y, float z);
void ppNextStart(PpParticle *particle);
void ppNextEnd(PpParticle *particle);
void ppInit(PpParticle *particle);

#endif /* INCLUDE_MAIN_PP_INIT_H */
