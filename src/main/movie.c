#include "common.h"
#include "shared.h"
#include "main/jni.h"
#include "main/game_movie.h"
#include "main/game_snap_shot.h"
#include "main/string_utf_get_hash.h"

/*
 * The script VM's per-thread context. Every Java native receives
 * (thread, arguments, result).
 */
typedef struct JThread JThread;

/* Positions of the movie number's digits in "data\\movie\\full\\mv0000.ipu". */
#define MOVIE_PATH_DIGIT_ONES 21
#define MOVIE_PATH_DIGIT_TENS 20
#define MOVIE_PATH_DIGIT_HUNDREDS 19
#define MOVIE_PATH_DIGIT_THOUSANDS 18

/* Native argument block: the Movie update method reads its object pointer. */
typedef struct MovieObjectCall {
    unsigned char *object;
} MovieObjectCall;

extern JavaField *lookupClassField(void *class_object, void *name, int flags);
/* Defined in src/main/game_movie.c. */
extern void GameMovieInit(int size);

/* Calls GameMovieInit with the first argument converted from KiB to bytes. */
void Java_xeno_Movie_init__I(JThread *thread, int *arguments, unsigned int *result)
{
    GameMovieInit(arguments[1] << 10);
}

/* Plays the movie whose number is the first argument. */
void Java_xeno_Movie_start__I(JThread *thread, int *arguments,
                              unsigned int *result)
{
    /* Four decimal digits in the movie asset path are replaced at playback. */
    static char name[] = "data\\movie\\full\\mv0000.ipu";
    int movie_id = arguments[1];

    name[MOVIE_PATH_DIGIT_ONES] = movie_id % 10 + '0';
    movie_id /= 10;
    name[MOVIE_PATH_DIGIT_TENS] = movie_id % 10 + '0';
    movie_id /= 10;
    name[MOVIE_PATH_DIGIT_HUNDREDS] = movie_id % 10 + '0';
    movie_id /= 10;
    name[MOVIE_PATH_DIGIT_THOUSANDS] = movie_id % 10 + '0';

    GameMoviePlay(name);
}

void Java_xeno_Movie_stop__(JThread *thread, void *arguments, unsigned int *result)
{
    GameMovieStop();
}

/*
 * Writes GameMovieFrame into the Movie object's frame field, then reads
 * the transparent and alpha fields back into the GameMovie globals.
 */
void Java_xeno_Movie_update__(JThread *thread, MovieObjectCall *arguments,
                              unsigned int *result)
{
    JavaField *field;
    unsigned char *object = arguments->object;

    field = lookupClassField(classJava_xeno_Movie,
                             loadConstString("frame", -1), 0);
    /* The runtime's JavaField supplies the object's dynamic field offset. */
    *(int *)(object + field->offset) = GameMovieFrame;

    field = lookupClassField(classJava_xeno_Movie,
                             loadConstString("transparent", -1), 0);
    GameMovieTransparent = object[field->offset];

    field = lookupClassField(classJava_xeno_Movie,
                             loadConstString("alpha", -1), 0);
    GameMovieAlpha = object[field->offset];
}
