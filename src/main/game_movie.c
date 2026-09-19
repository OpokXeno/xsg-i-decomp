#include "common.h"

INCLUDE_ASM("asm/main/nonmatchings/game_movie", GameMovieMain);

INCLUDE_ASM("asm/main/nonmatchings/game_movie", GameMovieStop);

INCLUDE_ASM("asm/main/nonmatchings/game_movie", GameMoviePlay);

extern int SCRIPT_getEventActiveFlag(void);
extern int SCRIPT_getFadeTime(void);
extern void SCRIPT_sendMovieSkipSignal(void);
extern void nmlModelSetActiveFadeInCancel(int fadeTime);

static void movie_skip(int *mode)
{
    if (SCRIPT_getEventActiveFlag() != 0) {
        if (*mode != 1) {
            *mode = 2;
        }

        SCRIPT_sendMovieSkipSignal();
        nmlModelSetActiveFadeInCancel(SCRIPT_getFadeTime());
    }
}

INCLUDE_ASM("asm/main/nonmatchings/game_movie", caption_nextline);

INCLUDE_ASM("asm/main/nonmatchings/game_movie", caption_getparam);

INCLUDE_ASM("asm/main/nonmatchings/game_movie", caption_convert);

INCLUDE_ASM("asm/main/nonmatchings/game_movie", GameMpeg2Play);

INCLUDE_ASM("asm/main/nonmatchings/game_movie", GameMovieInit);
