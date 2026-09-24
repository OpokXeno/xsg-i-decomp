#include "common.h"

INCLUDE_ASM("asm/main/nonmatchings/game_movie", GameMovieMain);

/* Defined in src/main/xgl_1.c, still INCLUDE_ASM there. */
extern int xglMovieClose(void *movie);

/*
 * Partial view of the movie handle xglMovieClose closes; only the
 * playback-state field GameMovieStop clears is modeled, the rest of the
 * layout is not recovered.
 */
typedef struct MovieInfo {
    unsigned char unmodeled_00[0x40];
    short state;
} MovieInfo;

extern MovieInfo mi;

void GameMovieStop(void)
{
    xglMovieClose(&mi);
    mi.state = 0;
}

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
