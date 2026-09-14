/*
 * OV02 original TU 2: 0x00a00858..0x00a017a0 (4 functions)
 */
#include "common.h"
#include "shared.h"
#include "main/xgl_2.h"
#include "title.h"

#define TITLE_PARTICLE_STRIDE 16
#define TITLE_PARTICLE_AT(storage, index) \
    ((TitleParticleView *)((u8 *)(storage) + ((index) * TITLE_PARTICLE_STRIDE)))

INCLUDE_ASM("asm/nonmatchings/ov02/title", copyframe);

INCLUDE_ASM("asm/nonmatchings/ov02/title", build_mipmap);

static void particle_reset(TitleParticleView *particles, int index)
{
    int random_value;
    int y;

    if (index & 1) {
        TITLE_PARTICLE_AT(particles, index)->x =
            (s16)((xglSRand() & 0x7ff) + 6656);
        TITLE_PARTICLE_AT(particles, index)->y =
            (s16)((xglSRand() & 0x7f) + 8);
    } else {
        u16 random_x = (u16)(xglSRand() & 0x7ff);
        TITLE_PARTICLE_AT(particles, index)->x =
            (u16)(random_x + (u16)0xfe00);
        TITLE_PARTICLE_AT(particles, index)->y =
            (s16)-((xglSRand() & 0x7f) + 8);
    }

    TITLE_PARTICLE_AT(particles, index)->z = (s16)(xglSRand() & 0x1fff);
    y = TITLE_PARTICLE_AT(particles, index)->y;
    random_value = TITLE_PARTICLE_AT(particles, index)->z - 3584;
    if (random_value < 0)
        random_value = TITLE_PARTICLE_AT(particles, index)->z - 3073;
    if (y < 0)
        y = -y;
    random_value >>= 9;
    random_value *= y;
    if (random_value < 0)
        random_value += 15;
    TITLE_PARTICLE_AT(particles, index)->depth = (s16)(random_value >> 4);
    TITLE_PARTICLE_AT(particles, index)->speed =
        (s16)((xglSRand() & 0x3f) + 16);
}

INCLUDE_ASM("asm/nonmatchings/ov02/title", Title);
