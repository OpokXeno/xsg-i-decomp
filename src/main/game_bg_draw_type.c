#include "common.h"

INCLUDE_ASM("asm/main/nonmatchings/game_bg_draw_type", clear);

/* Background draw type 0 draws nothing; Game calls it directly. */
void GameBgDrawType0(void)
{
}

/*
 * The do-nothing hook GameBgDrawType1Entry and GameBgDrawType2Entry install
 * at GameLoopState+0x30; GameDrawSync calls it with no arguments.
 */
static void nullfunc(void)
{
}

INCLUDE_ASM("asm/main/nonmatchings/game_bg_draw_type", GameBgDrawType1);

INCLUDE_ASM("asm/main/nonmatchings/game_bg_draw_type", GameBgDrawType1Entry);

INCLUDE_ASM("asm/main/nonmatchings/game_bg_draw_type", GameBgDrawType2);

INCLUDE_ASM("asm/main/nonmatchings/game_bg_draw_type", GameBgDrawType2Entry);
