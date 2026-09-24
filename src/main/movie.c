#include "common.h"

/* GameMovieStop is defined in main/tu125 (src/main/game_movie.c), not yet
 * recovered. */
extern void GameMovieStop(void);

/*
 * The script VM's per-thread context, recovered as `JThread` in
 * src/main/chr.h (main's chr TU). Every Java native receives
 * (thread, arguments, result) as the sibling natives elsewhere in this
 * TU map (and in main/tu238, src/main/runtime.c) show.
 */
typedef struct JThread JThread;

/* GameMovieInit is defined in main/tu125 (src/main/game_movie.c), not yet
 * recovered. */
extern void GameMovieInit(int size);

void Java_xeno_Movie_init__I(JThread *thread, int *arguments, unsigned int *result)
{
    GameMovieInit(arguments[1] << 10);
}

INCLUDE_ASM("asm/main/nonmatchings/movie", Java_xeno_Movie_start__I);

void Java_xeno_Movie_stop__(JThread *thread, void *arguments, unsigned int *result)
{
    GameMovieStop();
}

INCLUDE_ASM("asm/main/nonmatchings/movie", Java_xeno_Movie_update__);
