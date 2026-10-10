#include "common.h"
#include "shared.h"
#include "main/jni.h"

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

/* Native argument block: the Movie update method reads its object pointer. */
typedef struct MovieObjectCall {
    unsigned char *object;
} MovieObjectCall;

/* Runtime class-field helpers are still assembly; keep the established ABI. */
extern SceneString *loadConstString(const char *bytes, int length);
extern JavaField *lookupClassField(void *class_object, void *name, int flags);

extern short GameMovieFrame;
extern unsigned char GameMovieTransparent;
extern unsigned char GameMovieAlpha;

extern void GameMoviePlay(char *path);

/* GameMovieInit is defined in main/tu125 (src/main/game_movie.c), not yet
 * recovered. */
extern void GameMovieInit(int size);

void Java_xeno_Movie_init__I(JThread *thread, int *arguments, unsigned int *result)
{
    GameMovieInit(arguments[1] << 10);
}

void Java_xeno_Movie_start__I(JThread *thread, int *arguments,
                              unsigned int *result)
{
    /* Four decimal digits in the movie asset path are replaced at playback. */
    static char name[] = "data\\movie\\full\\mv0000.ipu";
    int movie_id = arguments[1];

    name[21] = (char)(movie_id % 10 + '0');
    movie_id /= 10;
    name[20] = (char)(movie_id % 10 + '0');
    movie_id /= 10;
    name[19] = (char)(movie_id % 10 + '0');
    movie_id /= 10;
    name[18] = (char)(movie_id % 10 + '0');

    GameMoviePlay(name);
}

void Java_xeno_Movie_stop__(JThread *thread, void *arguments, unsigned int *result)
{
    GameMovieStop();
}

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
