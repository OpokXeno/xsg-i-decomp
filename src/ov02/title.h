/*
 * TU-local declarations of ov02/tu002 (src/ov02/title.c).
 */

#ifndef SRC_OV02_TITLE_H
#define SRC_OV02_TITLE_H

#include "shared.h"

typedef signed short s16;

/* Only the ten bytes touched by particle_reset are recovered here. */
typedef struct TitleParticleView {
    u16 x;
    s16 z;
    s16 y;
    s16 depth;
    s16 speed;
} TitleParticleView;

#endif /* SRC_OV02_TITLE_H */
