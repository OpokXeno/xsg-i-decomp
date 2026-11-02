#ifndef INCLUDE_MAIN_GAME_OVER_H
#define INCLUDE_MAIN_GAME_OVER_H

#include "shared.h"

/*
 * sRender's destination-buffer selector: folded (with a fixed high bit)
 * into the register DrawImage primes at TestEnv0[4] before the two-part
 * image transfer below. Only this halfword is evidenced here (see
 * src/main/map_1.c's MapRenderState for the same object's callback member
 * and src/main/window_tex_load.c's UmnRenderSize for the screen-size
 * halfwords).
 */
/* DrawBack reads the framebuffer-page halfword at sRender +0x14. */
typedef struct {
    u8 unmodeled_00[0x10];
    u16 width;
    u16 height;
    u16 framebuffer_page;
    u8 unmodeled_16[0x0a];
    u16 buffer_select;
} DrawImageRenderState;

/* Functions and data used by other translation units. */
/* The game-over screen: show the game-over image with its stream, then
 * fade picture and stream volume out together. */
void GameOver(void);

#endif /* INCLUDE_MAIN_GAME_OVER_H */
