#ifndef INCLUDE_OV02_TITLE_GAME_INIT_H
#define INCLUDE_OV02_TITLE_GAME_INIT_H

#include "shared.h"

/* Size of the game loop's global record. */
#define GAME_LOOP_STATE_SIZE 0x2A030

/*
 * The game loop's global record (GAME_LOOP_STATE_SIZE bytes, cleared by TitleGameInit), as
 * far as the OV02 translation units read it: the halfword and word the title
 * screen sets, and the runtime flags word the umn screens test.
 */
typedef struct Ov02GameLoopState {
    unsigned char unmodeled_00[14];
    short half_0e;                      /* +0x0E: title menu result */
    u32 flags;                          /* +0x10: GAME_FLAG_* bits (main/game.h) */
    unsigned char unmodeled_14[4];
    u32 word_18;                        /* +0x18 */
} Ov02GameLoopState;

/* Functions and data used by other translation units. */
extern Ov02GameLoopState GameLoopState;
void TitleGameInit(void);

#endif /* INCLUDE_OV02_TITLE_GAME_INIT_H */
