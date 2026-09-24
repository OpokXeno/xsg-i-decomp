#include "common.h"

INCLUDE_ASM("asm/main/nonmatchings/playcontrol", Java_xeno_PlayControl_create__);

/*
 * The script VM's per-thread context, recovered as `JThread` in
 * src/main/chr.h (main's chr TU) and src/main/jni.h. Every Java native in
 * this TU receives it as its first, unused argument (a0), the same
 * (thread, arguments[, result]) convention src/main/runtime.c and
 * src/main/camera.c use.
 */
typedef struct JThread JThread;

/*
 * The engine's playback controller (`Play`, the single instance
 * `playControl` src/main/play.h declares for main's play TU), modeled here
 * only for the fields this TU's natives read or write directly: the signal
 * word at +0x04 (play.h calls the same word `state`; named `signal` here
 * from the accessor pair this TU defines, getSignal__/signal__I), the
 * source pointer at +0x0c (play.h's `source`, stored by loadCamera__) and
 * the flags word at +0x2c (play.h's `flags`, bit 0 set/cleared by
 * start__/stop__). play.h is TU-local to the play TU, so this TU keeps its
 * own tag for the object.
 */
typedef struct PlayControlHandle {
    unsigned char unmodeled_00[4];
    int signal;                    /* +0x04, lw/sw at 0x002fa3ec/0x002fa404 */
    unsigned char unmodeled_08[4]; /* +0x08: play.h's `timeChart`, only
                                       reached here through
                                       PLAY_setTimeChart below */
    void *source;                  /* +0x0c, sw at 0x002fa4bc */
    unsigned char unmodeled_10[0x2c - 0x10];
    unsigned int flags;            /* +0x2c, lw/sw at 0x002fa4e4/0x002fa4f0
                                       (start__), 0x002fa500/0x002fa50c
                                       (stop__) */
} PlayControlHandle;

/*
 * Java_xeno_PlayControl_getSignal__ (main VA 0x002fa3e8, 16 bytes, GLOBAL
 * binding). Returns the playback controller's signal word.
 */
void Java_xeno_PlayControl_getSignal__(JThread *thread,
                                       PlayControlHandle **arguments,
                                       int *result)
{
    *result = (*arguments)->signal;
}

/* The call block of signal__I (java signature "(I)V"): the receiving
   PlayControl's native handle and the signal value to store. */
typedef struct PlayControlSignalArguments {
    PlayControlHandle *control; /* +0x0 */
    int value;                   /* +0x4 */
} PlayControlSignalArguments;

/*
 * Java_xeno_PlayControl_signal__I (main VA 0x002fa3f8, 16 bytes, GLOBAL
 * binding). Sets the playback controller's signal word.
 */
void Java_xeno_PlayControl_signal__I(JThread *thread,
                                     PlayControlSignalArguments *arguments)
{
    arguments->control->signal = arguments->value;
}

INCLUDE_ASM("asm/main/nonmatchings/playcontrol", Java_xeno_PlayControl_init__II);

INCLUDE_ASM("asm/main/nonmatchings/playcontrol", Java_xeno_PlayControl_init__IIIIF);

/* The call block of loadCamera__Ljava_lang_Object_ (java signature
   "(Ljava/lang/Object;)V"): the receiving PlayControl's native handle and
   the Object argument, stored verbatim into the handle's source field. */
typedef struct PlayControlLoadCameraArguments {
    PlayControlHandle *control; /* +0x0 */
    void *camera;                 /* +0x4 */
} PlayControlLoadCameraArguments;

/*
 * Java_xeno_PlayControl_loadCamera__Ljava_lang_Object_ (main VA
 * 0x002fa4b0, 16 bytes, GLOBAL binding). Stores the given Object into the
 * playback controller's source field.
 */
void Java_xeno_PlayControl_loadCamera__Ljava_lang_Object_(
    JThread *thread, PlayControlLoadCameraArguments *arguments)
{
    arguments->control->source = arguments->camera;
}

/* Already recovered in src/main/play.c (main's play TU, VA 0x0026a770);
   restated here verbatim (a TU-local declaration cannot be included from
   another TU). Stores its second argument into the playback controller's
   timeChart field (play.h +0x08). */
extern void PLAY_setTimeChart(PlayControlHandle *play, void *time_chart);

/* The call block of loadTimeChart__Ljava_lang_Object_ (java signature
   "(Ljava/lang/Object;)V"): the receiving PlayControl's native handle and
   the Object argument, passed through to PLAY_setTimeChart unchanged. */
typedef struct PlayControlLoadTimeChartArguments {
    PlayControlHandle *control; /* +0x0 */
    void *time_chart;             /* +0x4 */
} PlayControlLoadTimeChartArguments;

/*
 * Java_xeno_PlayControl_loadTimeChart__Ljava_lang_Object_ (main VA
 * 0x002fa4c0, 32 bytes, GLOBAL binding). Hands the given Object to
 * PLAY_setTimeChart as the playback controller's time-chart resource.
 */
void Java_xeno_PlayControl_loadTimeChart__Ljava_lang_Object_(
    JThread *thread, PlayControlLoadTimeChartArguments *arguments)
{
    PLAY_setTimeChart(arguments->control, arguments->time_chart);
}

/*
 * Java_xeno_PlayControl_start__ (main VA 0x002fa4e0, 20 bytes, GLOBAL
 * binding). Sets the playback controller's running bit (0x1).
 */
void Java_xeno_PlayControl_start__(JThread *thread,
                                   PlayControlHandle **arguments)
{
    (*arguments)->flags |= 1;
}

/*
 * Java_xeno_PlayControl_stop__ (main VA 0x002fa4f8, 24 bytes, GLOBAL
 * binding). Clears the playback controller's running bit (0x1).
 */
void Java_xeno_PlayControl_stop__(JThread *thread,
                                  PlayControlHandle **arguments)
{
    (*arguments)->flags &= ~1;
}

INCLUDE_ASM("asm/main/nonmatchings/playcontrol", Java_xeno_PlayControl_setObserver__IILjava_lang_Object_Ljava_lang_String_);

INCLUDE_ASM("asm/main/nonmatchings/playcontrol", Java_xeno_PlayControl_setObserver__ILjava_lang_String_Ljava_lang_Object_Ljava_lang_String_);

/*
 * The callback-parameter object PLAY_getCallBackParams returns (`TCHParams`
 * in src/main/play.h for the play TU: the playback controller's embedded
 * +0x1cc member). play.h is TU-local to the play TU, so this TU keeps its
 * own opaque tag for the returned pointer; nothing here dereferences it.
 */
typedef struct PlayControlCallBackParams PlayControlCallBackParams;

/* Already recovered in src/main/play.c (main's play TU, VA 0x0026ab90);
   restated here verbatim (a TU-local declaration cannot be included from
   another TU). Returns the playback controller's own callback-parameter
   object. */
extern PlayControlCallBackParams *PLAY_getCallBackParams(PlayControlHandle *play);

/*
 * Java_xeno_PlayControl_getParams__ (main VA 0x002fa648, 44 bytes, GLOBAL
 * binding). Returns the playback controller's callback-parameter object.
 */
void Java_xeno_PlayControl_getParams__(JThread *thread,
                                       PlayControlHandle **arguments,
                                       PlayControlCallBackParams **result)
{
    *result = PLAY_getCallBackParams(*arguments);
}
