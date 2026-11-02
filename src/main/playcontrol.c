#include "common.h"
#include "shared.h"
#include "main/jni.h"
#include "main/play.h"
#include "playcontrol.h"
#include "main/init_vm.h"
#include "main/string_utf_get_hash.h"

/* Native integer arguments occupy complete 32-bit VM slots. The camera
 * controller takes the low halfword. The VM passes frame bounds as raw
 * operand words; copy their representation into signed Java integers before
 * converting frames to seconds, preserving negative frame values. */
typedef struct PlayControlIntegerCall {
    Play *control;
    int cameraIndex;
    int flags;
} PlayControlIntegerCall;

typedef struct PlayControlTimingCall {
    Play *control;
    int cameraIndex;
    int flags;
    unsigned int frameRangeBits[2];
    float frameStep;
} PlayControlTimingCall;

/*
 * The script VM's per-thread context, recovered as `JThread` in
 * src/main/chr.h (main's chr TU) and src/main/jni.h. Every Java native in
 * this TU receives it as its first, unused argument (a0), the same
 * (thread, arguments[, result]) convention src/main/runtime.c and
 * src/main/camera.c use.
 */
typedef struct JThread JThread;



typedef struct PlayControlStringStorage {
    unsigned char unmodeled_00[4];
    int length;
    const char *bytes;
} PlayControlStringStorage;

typedef struct PlayControlString {
    ObjectHeader header;
    PlayControlStringStorage *storage;
} PlayControlString;

typedef struct PlayControlSetObserverArguments {
    Play *control;
    int event_id;
    int event_key;
    SceneObject observer;
    PlayControlString *method_name;
} PlayControlSetObserverArguments;

typedef struct PlayControlSetChartObserverArguments {
    Play *control;
    int event_id;
    PlayControlString *chart_name;
    SceneObject observer;
    PlayControlString *method_name;
} PlayControlSetChartObserverArguments;

/* The call block of signal__I (java signature "(I)V"): the receiving
   PlayControl's native handle and the signal value to store. */
typedef struct PlayControlSignalArguments {
    Play *control; /* +0x0 */
    int value;                   /* +0x4 */
} PlayControlSignalArguments;

/* The call block of loadCamera__Ljava_lang_Object_ (java signature
   "(Ljava/lang/Object;)V"): the receiving PlayControl's native handle and
   the Object argument, stored verbatim into the handle's source field. */
typedef struct PlayControlLoadCameraArguments {
    Play *control; /* +0x0 */
    void *camera;                 /* +0x4 */
} PlayControlLoadCameraArguments;

/* The call block of loadTimeChart__Ljava_lang_Object_ (java signature
   "(Ljava/lang/Object;)V"): the receiving PlayControl's native handle and
   the Object argument, passed through to PLAY_setTimeChart unchanged. */
typedef struct PlayControlLoadTimeChartArguments {
    Play *control; /* +0x0 */
    void *time_chart;             /* +0x4 */
} PlayControlLoadTimeChartArguments;

typedef struct PlayControlCallBackParams PlayControlCallBackParams;

extern Play *PLAY_getCurrent(void);

extern void PLAY_setObserver(Play *play, int event_id, int event_key,
                             SceneObject observer, JavaMethod *method);

extern JavaMethod *findMethod(JavaClass *clazz, SceneString *name,
                               void *signature_or_type);

extern int TCH_getInfoID(void *chart, const char *name, int length);

/* Already recovered in src/main/play.c (main's play TU, VA 0x0026a770);
   restated here verbatim (a TU-local declaration cannot be included from
   another TU). Stores its second argument into the playback controller's
   timeChart field (play.h +0x08). */
extern void PLAY_setTimeChart(Play *play, void *time_chart);

extern PlayControlCallBackParams *PLAY_getCallBackParams(Play *play);

/* Create the native playback object and attach the class reference expected by
   the script VM's SceneObject header. */
void Java_xeno_PlayControl_create__(JThread *thread, void *arguments,
                                    int *result)
{
    JavaClass *window_class;
    Play *play;

    window_class = classJava_xeno_util_Window;
    play = PLAY_getCurrent();
    play->class_ref = (int)window_class->classPointer;
    *result = (int)play;
}

/*
 * Java_xeno_PlayControl_getSignal__ (main VA 0x002fa3e8, 16 bytes, GLOBAL
 * binding). Returns the playback controller's signal word.
 */
void Java_xeno_PlayControl_getSignal__(JThread *thread,
                                       Play **arguments,
                                       int *result)
{
    *result = (*arguments)->signal;
}

/*
 * Java_xeno_PlayControl_signal__I (main VA 0x002fa3f8, 16 bytes, GLOBAL
 * binding). Sets the playback controller's signal word.
 */
void Java_xeno_PlayControl_signal__I(JThread *thread,
                                     PlayControlSignalArguments *arguments)
{
    arguments->control->signal = arguments->value;
}

void Java_xeno_PlayControl_init__II(JThread *thread,
                                     const PlayControlIntegerCall *arguments)
{
    Play *play = arguments->control;
    Play *control = play;

    __builtin_memcpy(&control->cameraIndex, &arguments->cameraIndex, sizeof(control->cameraIndex));
    control->flags = arguments->flags;
    PLAY_setup(play);
}

void Java_xeno_PlayControl_init__IIIIF(JThread *thread,
                                        const PlayControlTimingCall *arguments)
{
    Play *control = arguments->control;
    int start_frame;
    int end_frame;
    float start_time;
    float frame_step;

    __builtin_memcpy(&control->cameraIndex, &arguments->cameraIndex, sizeof(control->cameraIndex));
    control->flags = arguments->flags;
    PLAY_setup(control);
    __builtin_memcpy(&start_frame, &arguments->frameRangeBits[0], sizeof(start_frame));
    start_time = (float)start_frame * PLAY_SECONDS_PER_FRAME;
    control->startTime = start_time;
    __builtin_memcpy(&end_frame, &arguments->frameRangeBits[1], sizeof(end_frame));
    control->endTime = (float)end_frame * PLAY_SECONDS_PER_FRAME;
    frame_step = arguments->frameStep;
    control->currentTime = start_time;
    control->frameStep = frame_step * PLAY_SECONDS_PER_FRAME;
}

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

void Java_xeno_PlayControl_loadTimeChart__Ljava_lang_Object_(
    JThread *thread, PlayControlLoadTimeChartArguments *arguments)
{
    PLAY_setTimeChart(arguments->control, arguments->time_chart);
}

void Java_xeno_PlayControl_start__(JThread *thread,
                                   Play **arguments)
{
    (*arguments)->flags |= 1;
}

void Java_xeno_PlayControl_stop__(JThread *thread,
                                  Play **arguments)
{
    (*arguments)->flags &= ~1;
}

void Java_xeno_PlayControl_setObserver__IILjava_lang_Object_Ljava_lang_String_(
    JThread *thread, PlayControlSetObserverArguments *arguments)
{
    SceneObject observer;
    ObjectHeader *observer_header;
    JavaClass *observer_class;
    PlayControlStringStorage *name_storage;
    SceneString *method_name;
    JavaMethod *method;

    observer = arguments->observer;
    observer_header = (ObjectHeader *)observer;
    observer_class = observer_header->classPointer->clazz;
    name_storage = arguments->method_name->storage;
    method_name = loadConstString(name_storage->bytes, name_storage->length);
    method = findMethod(observer_class, method_name, TYPE_Void);
    PLAY_setObserver(arguments->control, arguments->event_id,
                     arguments->event_key, observer, method);
}

void Java_xeno_PlayControl_setObserver__ILjava_lang_String_Ljava_lang_Object_Ljava_lang_String_(
    JThread *thread, PlayControlSetChartObserverArguments *arguments)
{
    Play *control;
    SceneObject observer;
    ObjectHeader *observer_header;
    JavaClass *observer_class;
    PlayControlStringStorage *chart_name_storage;
    PlayControlStringStorage *method_name_storage;
    SceneString *method_name;
    JavaMethod *method;
    int event_key;

    observer = arguments->observer;
    observer_header = (ObjectHeader *)observer;
    observer_class = observer_header->classPointer->clazz;
    method_name_storage = arguments->method_name->storage;
    method_name = loadConstString(method_name_storage->bytes,
                                  method_name_storage->length);
    method = findMethod(observer_class, method_name, TYPE_Void);

    control = arguments->control;
    chart_name_storage = arguments->chart_name->storage;
    event_key = TCH_getInfoID(control->timeChart,
                              chart_name_storage->bytes,
                              chart_name_storage->length);
    if (event_key < 0) {
        return;
    }

    PLAY_setObserver(control, arguments->event_id, event_key,
                     observer, method);
}

/*
 * Java_xeno_PlayControl_getParams__ (main VA 0x002fa648, 44 bytes, GLOBAL
 * binding). Returns the playback controller's callback-parameter object.
 */
void Java_xeno_PlayControl_getParams__(JThread *thread,
                                       Play **arguments,
                                       PlayControlCallBackParams **result)
{
    *result = PLAY_getCallBackParams(*arguments);
}
