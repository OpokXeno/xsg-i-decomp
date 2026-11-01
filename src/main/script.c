#include "common.h"
#include "init_class_db.h"
#include "string_utf_get_hash.h"
#include "map_2.h"
#include "unit.h"

#include "shared.h"
#include "main/jni.h"

#include "main/xgl_cd.h"
#include "main/xgl_thread.h"
#include "main/xgl_2.h"

/* GNU EE native TI storage type: one 16-byte store clears the debug menu storage. */
typedef unsigned int Quadword __attribute__((mode(TI)));

#include "script.h"

extern XglTaskPrefix *xglTaskEntryNext(XglTaskScheduler *scheduler,
                                       int (*callback)(XglTaskPrefix *task),
                                       XglTaskPrefix *entry);

extern int xglTaskRemove(XglTaskPrefix *task);

extern void xglFontDebugPrintf(int x, int y, const char *format, ...);

extern SceneString *loadConstString(const char *bytes, int length);

extern void *PLAY_getCurrent(void);
extern void PLAY_setupDefault(void *play);
extern void TCAMERA_init(void);
extern void *newObject(SceneClass *scene_class);
extern SceneClass *loadClass(SceneString *name, int initialize);
extern void STAGE_instance(SceneVm *vm, SceneObject stage_object);
extern void SCENE_instance(SceneVm *vm, SceneObject scene_object);
extern void SCENE_start(SceneVm *vm, SceneObject scene_object, int mode,
                        const char *method_name);

extern SceneMethod *findMethod(SceneClass *scene_class, SceneString *name,
                               void *signature_or_type);

extern void JNI_callMethod(SceneVm *vm, SceneMethod *method,
                           SceneObject *arguments, int *output);

extern int JNI_isInstanceOf(SceneObject object, SceneClass *target_class);

extern void JNI_initThread(SceneVm *vm);

extern void JNI_catchException(void);

extern int getScriptFlag(SceneObject object);

void XTK_setWindowOwner(int owner);

void talkCancel(ScriptObserverTask *task);

extern void actTalkAfter(SceneObject object);

extern void initVM(void);

extern void nmlModelSetFadeOut(int frames, int option);

extern void nmlModelSetFadeIn(int frames, int option);

extern void FX_ScreenMask(int unused, int color, int frames, int mode);

extern void nmlModelSetBackBufferToBattle(int owner_index);

extern void nmlModelSetFadeInCancel(int frames);

extern void nmlModelSetFadeOutCancel(int frames);

extern void nmlModelSetEffectWrite(int enabled);

extern void nmlModelSendSignalEventFinish(void);

extern void nmlModelSetGlobalPointLightReset(void);

extern void MAP_initUnit(void);

extern void ACT_initScene(void);

extern void ACT_initMTNResource(void);

extern void TWSYS_init(void);

extern void MSG_init(void);

extern void xglRenderClearColor(u32 color);

extern void SCENE_cleanup(SceneVm *vm, SceneObject scene_object);

extern void xglStudioGetCamera(StudioCamera **camera_out, int camera_index);

extern void xglCameraClipRangeDefault(StudioCamera *camera);

JThread *defaultVM;

extern JThread *JNI_createThread(int kind, int stack_words,
                                     int frame_words);

extern void JNI_pushFrame(void);

extern void JNI_loadNativeClass(void);

int UseVMFlag;

JThread *stageVM;

JThread *evtVM[2];

void createTalkTask(void *actor, const char *method_name);

void sceneObserver2(ScriptSceneTask *task);

static char sig0_0[16] ;

static char sig1_1[34] ;

extern void Before_Talk(Actor *actor);

extern void TWIN_initCF(void *window);

extern SceneWindow *TWIN_create2(int component_id);

void talktoObserver(ScriptObserverTask *task);

const char lightSetColorMethodName[] = "setColor";

const char lightSetDirection2MethodName[] = "setDirection2";

#define call_method_signature_void sig_2

#define call_method_signature_int sig_3

#define call_method_signature_int_int sig_4

/* EUC-JP text below means "event in progress". */

/* Event running. */
const char func_observer_debug_text[16] =
    "\xa5\xa4\xa5\xd9\xa5\xf3\xa5\xc8\xbc\xc2\xb9\xd4\xc3\xe6";

void funcObserver(ScriptObserverTask *task);

int getEmptyVM(ScriptObserverTask *observer);

extern ScriptPadRecord PadData[2];

extern const char debugBaseDirPath[];

static const char *debugBasePath = debugBaseDirPath;

static unsigned int s_nScriptFadeOutTime = 30;

static unsigned int s_nScriptFadeRequest = 0;

static unsigned char s_nScriptChangeTime = 0;

static unsigned int s_nScriptSequenceReset = 0;

static unsigned int s_nScriptFrameLockEntry = 0;

static unsigned int s_nScriptEventSkipEntry = 0;

static unsigned int s_nScriptCfTime = 0;

static unsigned int s_nScriptEventFin = 0;

static unsigned int s_nScriptEventActive = 0;

static unsigned int s_nScriptTalkLock = 0;

extern const char lightClassName[];
const char lightClassName[8] = "light";

static const char sig_2[4] = "()V";

static const char sig_3[5] = "(I)V";

static const char sig_4[6] = "(II)V";

static int windowOwner = 0;

typedef void (*JSNativeMethod)(void);

extern void JS_init(int state, int class_capacity, int method_capacity);

extern int JS_loadClass(const char *class_name);

extern void JS_classSetup(int class_id, JSNativeMethod get_peer);

extern void JS_classAddMethod(int class_id, const char *name,
                              JSNativeMethod method);

extern void JS_classLight_getPeer(void);

extern void JS_classLight_setColor(void);

extern void JS_classLight_setDirection2(void);

/* Only the halfword at GameLoopState+0xC is evidenced by
 * SCRIPT_sendMovieSkipSignal; the rest of the game-state record remains
 * scaffold-owned. */

typedef struct GameLoopMovieStatePrefix {
    u8 unmodeled_00[0xC];
    unsigned short movie_state;
} GameLoopMovieStatePrefix;

static int currentScriptDB;

static int resourceID;

/* TU-local observer and CallMethod declarations. */

/* Proposal: the script table indexed by currentScriptDB. Only the two fields
 * this allocation touches are modeled; scriptDB's element stride (0x1C bytes,
 * evidenced by *7*4 in every CallMethod family function) is fixed by that
 * indexing arithmetic, not invented. */

typedef struct ScriptDbEntry {
    void *class_image;   /* +0x0: class database image read from the script file */
    void *motion_data;   /* +0x4: extra-motion pack read next to the script file (.fpk) */
    void *pdb;            /* +0x8: event data used for resource lookup */
    const char *active;   /* +0xC: first class name returned by JNI_searchClasses */
    SceneVm *thread;     /* +0x10: opaque handle of the JNI-created VM thread */
    int class_slot;      /* +0x14: class database slot the image is loaded into */
    int skip_count;      /* +0x18: class-name skip count JNI_searchClasses reported */
} ScriptDbEntry;

static ScriptDbEntry scriptDB[2];

typedef struct ScriptPdbFile {
    u8 unmodeled_00[8];
    int data;
} ScriptPdbFile;

extern ScriptPdbFile *PDB_findFile(void *pdb, const char *path);

/*
 * TU-local partial view of the engine's actor record (the 0xa70-strided
 * `actor` array at main 0x0043c1e0; other units keep their own scoped views
 * of the same record, e.g. src/main/near_dir.h). SCRIPT_execTalkto and
 * SCRIPT_execTouchto are handed one entry and read only the three members
 * below; every other byte of the record is untouched by this allocation and
 * stays an unmodeled span.
 *
 *   +0x4c0 script_object      the native actor peer passed to the talk
 *                             method as its first actor argument; the
 *                             talk and touch entry points check its
 *                             xeno/Chr class before queuing a task.
 *   +0x9f4 talk_method_name   the JNI method name of the actor's talk
 *                             script method, null when it has none. Both
 *                             entry points gate on it before queuing a talk
 *                             task, and SCRIPT_execTalkto passes it straight
 *                             through as the method to call. createTalkTask
 *                             (0x00261620, still assembler) passes its own
 *                             second argument to loadConstString(bytes,
 *                             length) (0x00261698), so the field is a name
 *                             string, not an integer flag.
 *   +0x9f8 touch_method_name  the method SCRIPT_execTouchto passes instead,
 *                             once the same +0x9f4 gate lets it through.
 */

typedef struct ScriptActorMethods {
    u8 unmodeled_00[0x4c0];
    SceneObject script_object;      /* +0x4c0 */
    u8 unmodeled_4c4[0x9f4 - 0x4c4];
    const char *talk_method_name;   /* +0x9f4 */
    const char *touch_method_name;  /* +0x9f8 */
} ScriptActorMethods;

/* Reused TU-local declaration (src/core/main-0025a9c0/private.h and every
 * other GameLoopState-family accepted source in this unit). */

extern ScriptGameLoopState GameLoopState;

/* Proposal: per-function JNI method-signature literals. Each is a duplicate
 * function-local `static const char sig[]`, so the assembler disambiguates
 * with the original ELF's own dup-static suffixing: "sig.2"/"sig.3"/"sig.4". */

/* "(II)V"  @ 0x004da478, ELF name "sig.4" */

/* PadData (0x00490d90, 0xd0 bytes) read as one doubleword at +0x28, the
 * shared half_28/half_2a pair (original: lui 0x49; ld 3512). */

#define PAD_DATA_DOUBLEWORD_AT_28 (PadData[0].packed_input)

/* Drop the task from the scheduler and leave the task body. The usual
 * do/while (0) statement-macro idiom; its loop notes are part of the
 * original's code generation. */

int loadScriptCD(ScriptDbEntry *entry, const char *path);

int loadScriptCD2(ScriptDbEntry *entry, const char *path);

void SCRIPT_load_DBG(const char *path);

static int PauseSkipCheck(void);

extern char *strcpy(char *destination, const char *source);

extern unsigned int strlen(const char *string);

extern ScriptRenderState sRender;

/* Queue a debug menu task for `callback` and give it the owner and an empty
 * window, the way every debug entry point does. */

int rcFile;

static int lightFileNum;

static ScriptFileSelect * fileSelect;

extern ScriptActorRecord actor[64];

extern void TWSYS_setGRPStatus(int group, int status);

extern ScriptWindow *TMENU_create(int component_id);

extern ScriptWindow *TSLIDER_create(int component_id);

extern void TMENU_addItem(ScriptWindow *menu, const char *text);

extern void initCursor(void);

extern int sprintf(char *buffer, const char *format, ...);

extern void DB_reset(int x, int y);

extern void DB_println(const char *format, ...);

extern void DB_lightWrite(int descriptor);

extern void RSRC_init(int rc);

extern char *RSRC_loadFile2(int rc, const char *directory, const char *name);

extern int *RSRC_getItem(int rc, char *handle);

extern void JS_exec(char *begin, char *end);

extern int xglCdFileSelect(ScriptFileSelect *state);

extern int strncmp(const char *a, const char *b, unsigned int length);

extern int MapGetNo(void);

extern char *MapGetName(int map_no);

extern void LoadMap2(int map_no, int entrance);

ScriptDbEntry *SCRIPT_exec(void);

static char *getStrIndex(char *string, unsigned int length, int ch);

extern void TWIN_dispose(TComponent *window);

void attrObserver(ScriptObserverTask *task);

extern int sceOpen(const char *path, int flags, ...);

extern int sceClose(int descriptor);

extern char *strrchr(const char *string, int ch);

extern const char systemEvtPath[];

extern const char baseEvtPath[];

typedef struct ScriptEventDebugState {
    int mode;
    unsigned int flags;
    int selected;
} ScriptEventDebugState;

static ScriptEventDebugState evtDebug;

extern unsigned char xglFontLoad(int font_id, int flags);

const char *loadScript(ScriptDbEntry *entry, const char *path_base, const char *path);

extern void GamePauseDispEvent(void);

extern void PartyTimePauseEnd(void);

extern void PartyTimePauseStart(void);

extern void xglRenderCopyDisp2Draw(void);

extern void xglSoundStreamMute(void);

extern void xglSoundStreamStop(int stream);

extern void xglSoundEffectNormalDirect(int effect_id);

extern u32 xheap_info(u8 *heap_info);

static u32 heapUsed;

static unsigned char heapInfo[136];

extern const unsigned char memoryUsageFormat[];

extern void drawBox2(Matrix4 *matrix, int color);

extern void drawRect2(Matrix4 *matrix, int color);

extern ScriptActorRecord *ACT_create(int resource_id, int flags);

extern int ACT_initMotion(void *actor);

extern int ACT_loadMotion(ScriptActorRecord *actor, int resource_id, int flags);

extern void ACT_loadResource(ScriptActorRecord *actor, int resource_id);

extern void ACT_matrixInit(void);

extern void InitCfSystem(void);

extern void UnduInit(unsigned char *state);

extern void ACT_init(void);

extern void nmlModelInit(void);

extern void GameResourceReset(int mode);

void sceneObserver(XglTaskPrefix *task_prefix);

extern unsigned char scriptSceneStartName[];

extern void GameMovieStop(void);

extern void GameDefocusSet(int a, int b, int c);

extern void MapChange2(int map_no);

static int lightSave;

static int lightActFlags[4];

extern void initLight2(void);

extern void initLight3(void);

extern int updateLight(int mode);

extern int DebugMenuResourceSizeAdd;

extern char *MAP_getPath(void);

extern void ACT_initExMotion(int, int, void *);

extern const char *JNI_searchClasses(void *, const char *, int, int *);

extern const char sceneStageClassPath[];

extern char fpkExtension[];

extern char arcExtension[];

typedef union ScriptPathPrefix {
    char bytes[16];
    unsigned long long words[2];
} ScriptPathPrefix;

typedef union ScriptPathBuffer {
    char bytes[0x100];
    unsigned long long words[0x100 / 8];
} ScriptPathBuffer;

extern const ScriptPathPrefix mtnpackPathPrefix;

extern void *RES_loadFile(int, int, const char *, int);

extern int sceRead(int descriptor, void *buffer, unsigned int size);

extern int sceLseek(int descriptor, int offset, int whence);

extern void DB_cmode(int mode);

extern void RES_GetMdlFileName(char *path, int resource_id);

extern SceneThread *JTHREAD_get(SceneObject object);

extern const char evtMonitorLabel[];

extern const char evtActorFormat[];

extern const char evtFlagsFormat[];

extern const char evtPositionFormat[];

extern const char evtRotationFormat[];

extern const char evtUnitFormat[];

extern char unknownBaseName[];

extern char cmdLabel[];

extern char moveLabel[];

extern char separatorLabel[];

enum ScriptDebugWord {
    SCRIPT_DEBUG_MODE,
    SCRIPT_DEBUG_FLAGS,
    SCRIPT_DEBUG_SELECTION
};

extern void JNI_initSystem(void *heap_base, int heap_size);

extern const char evtFrameFormat[];

extern char classMethodFormat[];

static void initJS(int state)
{
    int class_id;

    JS_init(state, 0x40, 8);
    class_id = JS_loadClass(lightClassName);
    JS_classSetup(class_id, JS_classLight_getPeer);
    JS_classAddMethod(class_id, lightSetColorMethodName, JS_classLight_setColor);
    JS_classAddMethod(class_id, lightSetDirection2MethodName, JS_classLight_setDirection2);
}

void SCRIPT_test(SceneObject argument, SceneMethod *method)
{
    JNI_initThread(stageVM);
    if (method != 0) {
        SceneObject arguments[1];

        arguments[0] = argument;
        JNI_callMethod(stageVM, method, arguments, 0);
    }
}

void SCRIPT_talkIgnoreSet(void)
{
    s_nScriptTalkLock = 1;
}

void SCRIPT_talkIgnoreClr(void)
{
    s_nScriptTalkLock = 0;
}

int SCRIPT_talkIgnoreGet(void)
{
    return s_nScriptTalkLock;
}

void SCRIPT_sceneChangeTimeInit(void)
{
    s_nScriptChangeTime = 0;
}

int SCRIPT_sceneChangeTimeGet(void)
{
    return s_nScriptChangeTime;
}

void SCRIPT_sceneChangeTimeSet(int value)
{
    s_nScriptChangeTime = value;
}

void SCRIPT_sceneChangeTimeDec(void)
{
    if (s_nScriptChangeTime != 0)
        s_nScriptChangeTime -= 1;
    else
        s_nScriptChangeTime = 0;
}

void SCRIPT_sendMovieSkipSignal(void)
{
    GameLoopMovieStatePrefix *game_state = (GameLoopMovieStatePrefix *)&GameLoopState;

    if (game_state->movie_state == 3)
        s_nScriptEventFin = 1;
}

void SCRIPT_clrEventActiveFlag(void)
{
    s_nScriptEventActive = 0;
}

void SCRIPT_setEventActiveFlag(void)
{
    s_nScriptEventActive = 1;
}

int SCRIPT_getEventActiveFlag(void)
{
    return s_nScriptEventActive;
}

void SCRIPT_eventFinish(void)
{
    s_nScriptEventFin = 1;
}

void SCRIPT_methodClearSet(void)
{
    s_nScriptSequenceReset = 1;
}

int SCRIPT_methodClearGet(void)
{
    return s_nScriptSequenceReset;
}

void SCRIPT_fade(int time)
{
    s_nScriptFadeRequest = 1;
    if (time < 0)
        time = 0;
    s_nScriptFadeOutTime = time;
}

int SCRIPT_getFadeTime(void)
{
    return s_nScriptFadeOutTime;
}

int SCRIPT_fadeGet(void)
{
    if (s_nScriptFadeRequest == 0)
        return 0;
    return s_nScriptFadeOutTime;
}

int SCRIPT_getCfTime(void)
{
    return (int)s_nScriptCfTime;
}

void SCRIPT_incCfTime(void)
{
    s_nScriptCfTime += 1u;
}

void SCRIPT_frameLock2Battle(void)
{
    s_nScriptFrameLockEntry = 1;
}

void SCRIPT_reset(void)
{
    JThread **event_vm;
    int remaining_vms;

    event_vm = evtVM;
    remaining_vms = 1;
    initVM();
    defaultVM = JNI_createThread(0, 8, 0x80);
    stageVM = JNI_createThread(0, 8, 0x46);
    do {
        --remaining_vms;
        *event_vm = JNI_createThread(0, 8, 0x40);
        ++event_vm;
    } while (remaining_vms >= 0);
    UseVMFlag = 0;
    JNI_pushFrame();
    JNI_pushFrame();
    JNI_loadNativeClass();
}

extern void JNI_loadClassDB(int slot, void *image);

void *SCRIPT_init(void *heap_base)
{
    unsigned char *cursor = heap_base;
    unsigned char *system_file_buffer;
    unsigned char *base_file_buffer;
    JThread **event_thread = &evtVM[0];
    ScriptDbEntry *entry = scriptDB;
    int remaining_vms = 1;

    evtDebug.flags = 0;
    cursor += 0x40000;
    system_file_buffer = cursor;
    cursor += 0x8000;
    base_file_buffer = cursor;
    cursor += 0x8000;
    entry->motion_data = (void *)0x05000000;
    entry->pdb = (void *)0x06100000;
    entry->class_slot = 2;
    entry->skip_count = 0;
    entry->class_image = cursor;
    cursor += 0x2D000;
    entry->active = 0;
    entry->thread = 0;
    ++entry;
    {
    int *current_db = &currentScriptDB;
    *current_db = 0;
    }
    entry->active = 0;
    entry->thread = 0;
    entry->skip_count = 0;
    entry->class_image = cursor;
    cursor += 0x2D000;
    entry->motion_data = (void *)0x06500000;
    entry->pdb = (void *)0x06900000;
    entry->class_slot = 3;

    xglCdReadFile(systemEvtPath, system_file_buffer, 0, 0);
    xglCdReadFile(baseEvtPath, base_file_buffer, 0, 0);
    JNI_initSystem(heap_base, 0x40000);
    JNI_loadClassDB(0, system_file_buffer);
    JNI_loadClassDB(1, base_file_buffer);
    initVM();
    defaultVM = JNI_createThread(0, 8, 0x80);
    stageVM = JNI_createThread(0, 8, 0x46);
    while (remaining_vms >= 0) {
        --remaining_vms;
        event_thread[0] = JNI_createThread(0, 8, 0x40);
        ++event_thread;
    }
    UseVMFlag = 0;
    JNI_pushFrame();
    JNI_pushFrame();
    JNI_loadNativeClass();
    return cursor;
}

void SCRIPT_execTalkto(void *actor)
{
    ScriptActorMethods *methods = (ScriptActorMethods *)actor;
    ScriptDbEntry *entry = &scriptDB[currentScriptDB];

    if (methods->talk_method_name != 0 && entry->active != 0 &&
        s_nScriptTalkLock == 0 &&
        JNI_isInstanceOf(methods->script_object, classJava_xeno_Chr) != 0) {
        do {
            JThread *thread = entry->thread;

            if (!JNI_isInstanceOf(thread->object, classJava_xeno_Stage))
                break;
            createTalkTask(actor, methods->talk_method_name);
        } while (0);
    }
}

void SCRIPT_execTouchto(void *actor)
{
    ScriptActorMethods *methods = (ScriptActorMethods *)actor;
    ScriptDbEntry *entry = &scriptDB[currentScriptDB];

    if (methods->talk_method_name != 0 && entry->active != 0 &&
        s_nScriptTalkLock == 0 &&
        JNI_isInstanceOf(methods->script_object, classJava_xeno_Chr) != 0) {
        do {
            JThread *thread = entry->thread;

            if (!JNI_isInstanceOf(thread->object, classJava_xeno_Stage))
                break;
            createTalkTask(actor, methods->touch_method_name);
        } while (0);
    }
}

void createTalkTask(void *actor, const char *method_name)
{
    JThread *thread;
    SceneObject receiver;
    SceneClass *scene_class;
    SceneMethod *method;
    ScriptObserverTask *task;
    XglTaskScheduler *scheduler;

    if (GameLoopState.flags & 0x20)
        return;
    thread = scriptDB[currentScriptDB].thread;
    receiver = thread->object;
    scene_class = ((SceneObjectHeader *)receiver)->class_ref->scene_class;
    method = findMethod(scene_class, loadConstString(method_name, -1),
                        loadConstString(sig0_0, -1));
    if (method != 0) {
        scheduler = GameLoopState.task_scheduler;
        task = (ScriptObserverTask *)xglTaskEntryNext(
            scheduler, (int (*)(XglTaskPrefix *))talktoObserver,
            scheduler != 0 ? scheduler->active_tail : 0);
        if (task != 0) {
            task->state_flags = 0;
            task->owner = &GameLoopState;
            task->object = 0;
        }
        task->argument2 = 0;
        task->method_signature = sig0_0;
    } else {
        TComponent *window;

        method = findMethod(scene_class, loadConstString(method_name, -1),
                            loadConstString(sig1_1, -1));
        if (method == 0)
            return;
        scheduler = GameLoopState.task_scheduler;
        task = (ScriptObserverTask *)xglTaskEntryNext(
            scheduler, (int (*)(XglTaskPrefix *))talktoObserver,
            scheduler != 0 ? scheduler->active_tail : 0);
        if (task != 0) {
            task->state_flags = 0;
            task->owner = &GameLoopState;
            task->object = 0;
        }
        task->method_signature = sig1_1;
        window = (TComponent *)TWIN_create2(-1);
        window->class_ref = classJava_xeno_util_Window->instance_class_ref;
        TWIN_initCF(window);
        window->flags &= ~0x40;
        window->flags &= ~0x80;
        task->argument2 = (int)window;
    }
    task->receiver = receiver;
    task->method_name = method_name;
    task->method = method;
    task->object = actor;
    Before_Talk(actor);
    GameLoopState.flags |= 0x9000;
    XTK_setWindowOwner((int)actor);
    JNI_initThread(stageVM);
    talktoObserver(task);
}

void talktoObserver(ScriptObserverTask *task)
{
    unsigned int owner_flags = task->owner->flags;
    SceneObject receiver;
    SceneObject object;
    SceneMethod *method;
    int script_flags;
    SceneObject arguments[3];

    if (owner_flags & 0x800) {
        xglTaskRemove(&task->task);
        return;
    }
    if (owner_flags & 0x20) {
        xglTaskRemove(&task->task);
        return;
    }

    receiver = task->receiver;
    {
        SceneObjectHeader *header = (SceneObjectHeader *)receiver;
        SceneClass *scene_class = header->class_ref->scene_class;
        SceneString *name = loadConstString(task->method_name, -1);
        SceneString *signature = loadConstString(task->method_signature, -1);

        task->method = findMethod(scene_class, name, signature);
    }
    if (task->method == 0) {
        xglTaskRemove(&task->task);
        return;
    }

    object = task->object;
    script_flags = getScriptFlag(object);
    if (task->argument2 != 0 && (script_flags & 8)) {
        if (UseVMFlag != 0 || (PAD_DATA_DOUBLEWORD_AT_28 & 0x600000) == 0x400000) {
            XTK_setWindowOwner(0);
            talkCancel(task);
            return;
        }
    }

    /*
     * The method is read back from the task before the argument array is
     * filled: the original loads it (lw a1,60(s2)) ahead of the stageVM
     * argument, which only happens when the array stores separate the read
     * from the call (a read in the call itself, attempt-4d6f277fff4a form
     * 08, is scheduled after the stageVM load).
     */
    method = task->method;
    arguments[0] = receiver;
    arguments[1] = ((ScriptActorMethods *)object)->script_object;
    if (task->argument2 != 0)
        arguments[2] = (SceneObject)task->argument2;
    JNI_callMethod(stageVM, method, arguments, 0);
    if (stageVM->flags & SCENE_THREAD_CALL_PENDING) {
        XTK_setWindowOwner(0);
        actTalkAfter(object);
        GameLoopState.flags &= ~0x1000;
        GameLoopState.flags &= ~0x8000;
        GameLoopState.flags &= ~0x10000;
        xglTaskRemove(&task->task);
    }
}

void talkCancel(ScriptObserverTask *task)
{
    TComponent *window;
    ScriptGameLoopState *game_state;

    actTalkAfter(task->object);
    window = (TComponent *)task->argument2;
    game_state = &GameLoopState;
    window->flags |= 0x80;
    window->flags &= ~2;
    game_state->flags &= ~0x1000;
    game_state->flags &= ~0x8000;
    game_state->flags &= ~0x10000;
    if (window != 0)
        window->state = 2;
    TWIN_dispose(window);
    xglTaskRemove(&task->task);
}

void SCRIPT_execEntered(int attribute)
{
    ScriptDbEntry *entry = &scriptDB[currentScriptDB];

    if (entry->active != 0) {
        JThread *thread = entry->thread;
        SceneObject object = thread->object;

        if (!(GameLoopState.flags & 0x400) &&
            JNI_isInstanceOf(object, classJava_xeno_Stage)) {
            XglTaskScheduler *scheduler = GameLoopState.task_scheduler;
            ScriptObserverTask *task = (ScriptObserverTask *)xglTaskEntryNext(
                scheduler, (int (*)(XglTaskPrefix *))attrObserver,
                scheduler != 0 ? scheduler->active_tail : 0);

            if (task != 0) {
                task->owner = &GameLoopState;
                task->state_flags = 0;
                task->object = 0;
            }
            task->object = object;
            task->argument1 = attribute;
            GameLoopState.flags |= 0x100000;
        }
    }
}

void CallMethod(const char *method_name)
{
    ScriptDbEntry *entry = &scriptDB[currentScriptDB];

    if (entry->active != 0) {
        JThread *thread = entry->thread;
        SceneObject object = thread->object;

        if (JNI_isInstanceOf(object, classJava_xeno_Stage) != 0) {
            XglTaskScheduler *scheduler = GameLoopState.task_scheduler;
            ScriptObserverTask *task = (ScriptObserverTask *)xglTaskEntryNext(
                scheduler, (int (*)(XglTaskPrefix *))funcObserver,
                scheduler != 0 ? scheduler->active_tail : 0);
            unsigned char started;

            if (task != 0) {
                task->owner = &GameLoopState;
                task->state_flags = 0;
                task->object = 0;
            }
            task->object = object;
            task->method_name = method_name;
            task->method_signature = call_method_signature_void;
            task->invocation_state = 0;
            funcObserver(task);
            /* separate load keeps the funcObserver call out of tail position:
             * without it (form 11) GCC emits a sibling `j funcObserver` */
            started = task->invocation_state;
        }
    }
}

void CallMethod_I(const char *method_name, int argument1)
{
    ScriptDbEntry *entry = &scriptDB[currentScriptDB];

    if (entry->active != 0) {
        JThread *thread = entry->thread;
        SceneObject object = thread->object;

        if (JNI_isInstanceOf(object, classJava_xeno_Stage) != 0) {
            XglTaskScheduler *scheduler = GameLoopState.task_scheduler;
            ScriptObserverTask *task = (ScriptObserverTask *)xglTaskEntryNext(
                scheduler, (int (*)(XglTaskPrefix *))funcObserver,
                scheduler != 0 ? scheduler->active_tail : 0);
            unsigned char started;

            if (task != 0) {
                task->owner = &GameLoopState;
                task->state_flags = 0;
                task->object = 0;
            }
            task->object = object;
            task->method_name = method_name;
            task->method_signature = call_method_signature_int;
            task->argument1 = argument1;
            task->invocation_state = 0;
            funcObserver(task);
            /* separate load keeps the funcObserver call out of tail position:
             * without it (form 11) GCC emits a sibling `j funcObserver` */
            started = task->invocation_state;
        }
    }
}

void CallMethod_II(const char *method_name, int argument1, int argument2)
{
    ScriptDbEntry *entry = &scriptDB[currentScriptDB];

    if (entry->active != 0) {
        JThread *thread = entry->thread;
        SceneObject object = thread->object;

        if (JNI_isInstanceOf(object, classJava_xeno_Stage) != 0) {
            XglTaskScheduler *scheduler = GameLoopState.task_scheduler;
            ScriptObserverTask *task = (ScriptObserverTask *)xglTaskEntryNext(
                scheduler, (int (*)(XglTaskPrefix *))funcObserver,
                scheduler != 0 ? scheduler->active_tail : 0);
            unsigned char started;

            if (task != 0) {
                task->owner = &GameLoopState;
                task->state_flags = 0;
                task->object = 0;
            }
            task->object = object;
            task->method_name = method_name;
            task->method_signature = call_method_signature_int_int;
            task->argument1 = argument1;
            task->argument2 = argument2;
            task->invocation_state = 0;
            funcObserver(task);
            /* separate load keeps the funcObserver call out of tail position:
             * without it (form 11) GCC emits a sibling `j funcObserver` */
            started = task->invocation_state;
        }
    }
}

void funcObserver(ScriptObserverTask *task)
{
    unsigned int owner_flags;
    ScriptDbEntry *entry;
    JThread *thread;
    signed char vm_slot;
    SceneObject object;

    xglFontDebugPrintf(8, 24, func_observer_debug_text);

    owner_flags = task->owner->flags;
    if (owner_flags & 0x800)
        do
        {
            xglTaskRemove(&(task)->task);
            return;
        }
        while (0);
    if ((task->state_flags & 1) == 0) {
        if (owner_flags & 0x400)
            do
            {
                xglTaskRemove(&(task)->task);
                return;
            }
            while (0);
        task->state_flags |= 1;
        if (getEmptyVM(task) == 0)
            do
            {
                xglTaskRemove(&(task)->task);
                return;
            }
            while (0);
    }

    entry = &scriptDB[currentScriptDB];
    vm_slot = task->vm_slot;
    if (entry->active == 0)
        return;

    /* The Stage object changed since CallMethod queued the task. */
    thread = entry->thread;
    object = thread->object;
    if (object != task->object)
        do
        {
            xglTaskRemove(&(task)->task);
            return;
        }
        while (0);

    {
        SceneObjectHeader *header = (SceneObjectHeader *)task->object;
        SceneClass *scene_class = header->class_ref->scene_class;
        SceneString *name = loadConstString(task->method_name, -1);
        SceneString *signature = loadConstString(task->method_signature, -1);
        SceneMethod *method = findMethod(scene_class, name, signature);
        SceneObject arguments[3];

        task->method = method;
        if (method == 0) {
            UseVMFlag &= ~(1 << vm_slot);
            xglTaskRemove(&task->task);
            return;
        }

        arguments[0] = object;
        arguments[1] = (SceneObject)task->argument1;
        arguments[2] = (SceneObject)task->argument2;
        JNI_callMethod(evtVM[vm_slot], method, arguments, 0);
        if (evtVM[vm_slot]->flags & SCENE_THREAD_CALL_PENDING) {
            JNI_catchException();
            UseVMFlag &= ~(1 << vm_slot);
            xglTaskRemove(&task->task);
            return;
        }
    }
    GameLoopState.flags |= 0x200000;
}

int getEmptyVM(ScriptObserverTask *observer)
{
    int slot;

    observer->vm_slot = -1;
    for (slot = 0; slot < 2; ++slot) {
        if (((UseVMFlag >> slot) & 1) == 0) {
            JNI_initThread(evtVM[slot]);
            observer->state_flags |= 1;
            UseVMFlag |= 1 << slot;
            observer->vm_slot = (signed char)slot;
            return 1;
        }
    }
    return observer->vm_slot != -1;
}

int SCRIPT_load(const char *path)
{
    return loadScriptCD(&scriptDB[(currentScriptDB + 1) & 1], path);
}

int SCRIPT_load2(const char *path)
{
    return loadScriptCD2(&scriptDB[(currentScriptDB + 1) & 1], path);
}

void SCRIPT_load_DBG(const char *path)
{
    ScriptDbEntry *entry;
    ScriptDbEntry *nextEntry;
    const char *base;
    const char **path_slot;

    xglFontLoad(0, 0);
    path_slot = &debugBasePath;
    entry = &scriptDB[0];
    entry->motion_data = (void *)0x05000000;
    entry->pdb = (void *)0x06100000;
    entry = scriptDB + 1;
    entry->motion_data = (void *)0x06500000;
    entry->pdb = (void *)0x06900000;
    nextEntry = &scriptDB[(currentScriptDB + 1) & 1];
    base = *path_slot;
    loadScript(nextEntry, base, path);
}

static char *getStrIndex(char *string, unsigned int length, int ch)
{
    int index = (int)(length - 1U);

    while (index >= 0) {
        char *cursor = &string[index];
        long current_character = *cursor;

        --index;
        if (current_character == ch)
            return cursor;
    }
    return 0;
}

static char *replacePathExt(char *path, const char *ext)
{
    return strcpy(getStrIndex(path, strlen(path), '.'), ext);
}

void attrObserver(ScriptObserverTask *task)
{
    SceneObject object = task->object;
    SceneMethod *method;
    int vm_slot;
    ScriptMethodArgument arguments[2];

    if ((task->state_flags & 1) == 0) {
        if (task->owner->flags & 0x400) {
            xglTaskRemove(&task->task);
            return;
        }
        task->owner->flags |= 0x400;
        if (getEmptyVM(task) == 0) {
            xglTaskRemove(&task->task);
            return;
        }
        task->state_flags |= 1;
    }
    {
        SceneObjectHeader *header = (SceneObjectHeader *)object;
        SceneClass *scene_class = header->class_ref->scene_class;
        SceneString *name = loadConstString("entered", -1);
        SceneString *signature = loadConstString("(I)V", -1);

        method = findMethod(scene_class, name, signature);
    }
    if (method == 0) {
        task->owner->flags &= ~0x400;
        xglTaskRemove(&task->task);
        return;
    }
    arguments[0].reference = object;
    vm_slot = task->vm_slot;
    arguments[1].integer = task->argument1;
    JNI_callMethod(evtVM[vm_slot], method, (SceneObject *)arguments, 0);
    if (evtVM[vm_slot]->flags & SCENE_THREAD_CALL_PENDING) {
        JNI_catchException();
        task->owner->flags &= ~0x400;
        UseVMFlag &= ~(1 << vm_slot);
        xglTaskRemove(&task->task);
        GameLoopState.flags &= ~0x100000;
    }
}

static int PauseSkipCheck(void)
{
    if ((GameLoopState.flags & 0x90000000) != 0)
        return 1;
    if (GameLoopState.input_delay != 0 ||
        (((ScriptPadButtons *)PadData)->pressed & 0x800) == 0)
        return 0;

    GameLoopState.input_delay = 0x20;
    xglSoundEffectNormalDirect(4);
    sRender.scene_disabled = 1;
    xglSleep();
    xglRenderCopyDisp2Draw();
    GamePauseDispEvent();
    xglSleep();
    GamePauseDispEvent();
    xglSleep();
    PartyTimePauseStart();

    if (!(((ScriptPadButtons *)PadData)->pressed & 0x10)) {
        for (;;) {
            if (GameLoopState.input_delay != 0) {
                GameLoopState.input_delay--;
            } else if ((((ScriptPadButtons *)PadData)->pressed & 0x800) != 0) {
                break;
            }
            xglSleep();
            if (((ScriptPadButtons *)PadData)->pressed & 0x10)
                break;
        }
    }

    PartyTimePauseEnd();
    xglSoundEffectNormalDirect(4);
    sRender.scene_disabled = 0;
    GameLoopState.input_delay = 0x20;
    if (((ScriptPadButtons *)PadData)->pressed & 0x10) {
        GameLoopState.input_delay = 0x40;
        if (s_nScriptFrameLockEntry == 0)
            s_nScriptEventSkipEntry = 1;
        xglSoundStreamStop(0);
        return 1;
    }
    xglSoundStreamMute();
    return 0;
}

#define SCRIPT_FADE_COLOR(red, green, blue)                                    \
    (0x7F000000 + ((int)((red) * 127.0f) << 16) +                              \
     ((int)((green) * 127.0f) << 8) + (int)((blue) * 127.0f))

void sceneObserver2(ScriptSceneTask *task)
{
    SceneThread *thread;
    StudioCamera *camera;
    int camera_index;

    if (!(task->state_flags & 1)) {
        task->state_flags |= 1;
        task->fade_started = 0;
        task->elapsed_frames = 0;
    }
    thread = (SceneThread *)task->thread;
    if (PauseSkipCheck() != 0 || s_nScriptEventFin != 0) {
        thread->flags = (thread->flags & ~0x10) | SCENE_THREAD_CALL_PENDING;
        s_nScriptEventFin = 0;
    }
    if (!(thread->flags & SCENE_THREAD_CALL_PENDING))
        return;
    if (task->fade_started == 0) {
        task->fade_started = 1;
        if (s_nScriptFrameLockEntry == 0) {
            if ((int)GameLoopState.fade_out_frames > 0) {
                nmlModelSetFadeOut((int)GameLoopState.fade_out_frames, 0);
                FX_ScreenMask(0,
                              SCRIPT_FADE_COLOR(GameLoopState.fade_out_red,
                                                GameLoopState.fade_out_green,
                                                GameLoopState.fade_out_blue),
                              (int)GameLoopState.fade_out_frames, 2);
            }
        } else {
            GameLoopState.frame_lock_wait = s_nScriptFadeOutTime + 4;
        }
        if (s_nScriptEventSkipEntry != 0 && s_nScriptFrameLockEntry == 0)
            nmlModelSetBackBufferToBattle(sRender.owner_index);
        s_nScriptChangeTime = (int)GameLoopState.fade_out_frames + 1;
    }
    task->elapsed_frames++;
    if ((int)s_nScriptFadeOutTime >= task->elapsed_frames)
        return;
    if (s_nScriptFrameLockEntry != 0) {
        nmlModelSetBackBufferToBattle(sRender.owner_index);
        nmlModelSetFadeInCancel(30);
        nmlModelSetFadeOutCancel(30);
    }
    if (s_nScriptEventSkipEntry != 0) {
        nmlModelSendSignalEventFinish();
        s_nScriptEventSkipEntry = 0;
    }
    s_nScriptFadeOutTime = 30;
    s_nScriptChangeTime = 25;
    camera_index = 0;
    s_nScriptEventActive = 0;
    s_nScriptFrameLockEntry = 0;
    s_nScriptFadeRequest = 0;
    s_nScriptTalkLock = 0;
    MAP_initUnit();
    ACT_initScene();
    ACT_initMTNResource();
    TWSYS_init();
    MSG_init();
    s_nScriptSequenceReset = 0;
    xglRenderClearColor(0x80000000);
    GameLoopState.kind = 2;
    s_nScriptCfTime = 0;
    nmlModelSetGlobalPointLightReset();
    SCENE_cleanup(stageVM, thread->object);
    JNI_catchException();
    task->owner->flags &= ~0x800;
    do {
        xglStudioGetCamera(&camera, camera_index);
        camera_index++;
        xglCameraClipRangeDefault(camera);
    } while (camera_index < 8);
    nmlModelSetGlobalPointLightReset();
    nmlModelSetEffectWrite(1);
    nmlModelSetFadeIn((int)GameLoopState.fade_frames, 0);
    FX_ScreenMask(0,
                  SCRIPT_FADE_COLOR(GameLoopState.fade_red,
                                    GameLoopState.fade_green,
                                    GameLoopState.fade_blue),
                  (int)GameLoopState.fade_frames, 3);
    xglTaskRemove(&task->task);
}

extern int MapGetNo(void);

void sceneObserver(XglTaskPrefix *task_prefix)
{
    ScriptSceneTask *task = (ScriptSceneTask *)task_prefix;
    JThread *thread;
    ScriptDbEntry *entry;
    StudioCamera *camera;
    int camera_index;

    if (!(task->state_flags & 1)) {
        SceneObject object;

        task->state_flags |= 1;
        task->fade_started = 0;
        task->elapsed_frames = 0;
        entry = &scriptDB[currentScriptDB];
        object = newObject(loadClass(loadConstString((const char *)entry->active,
                                                     -1), 0));
        thread = JNI_createThread(2, 6, 0x46);
        {
            SceneVm *instance_vm = defaultVM;

            thread->object = object;
            entry->thread = thread;
            task->thread = entry->thread;
            SCENE_instance(instance_vm, thread->object);
            SCENE_start(defaultVM, thread->object, 8,
                        (const char *)scriptSceneStartName);
        }
    }

    thread = task->thread;
    if (thread->stack == 0)
        xglTaskRemove(task_prefix);
    if (PauseSkipCheck() != 0 || s_nScriptEventFin != 0) {
        thread->flags = (thread->flags & ~0x10) | SCENE_THREAD_CALL_PENDING;
        s_nScriptEventFin = 0;
    }
    if (!(thread->flags & SCENE_THREAD_CALL_PENDING))
        return;

    if (task->fade_started == 0) {
        task->fade_started = 1;
        if (s_nScriptFrameLockEntry == 0) {
            if ((int)GameLoopState.fade_out_frames > 0) {
                nmlModelSetFadeOut((int)GameLoopState.fade_out_frames, 0);
                FX_ScreenMask(0,
                              SCRIPT_FADE_COLOR(GameLoopState.fade_out_red,
                                                GameLoopState.fade_out_green,
                                                GameLoopState.fade_out_blue),
                              (int)GameLoopState.fade_out_frames, 2);
            }
        } else {
            GameLoopState.frame_lock_wait = s_nScriptFadeOutTime + 4;
        }
        if (s_nScriptEventSkipEntry != 0 && s_nScriptFrameLockEntry == 0)
            nmlModelSetBackBufferToBattle(sRender.owner_index);
        s_nScriptChangeTime = (int)GameLoopState.fade_out_frames + 1;
    }
    task->elapsed_frames++;
    if ((int)s_nScriptFadeOutTime >= task->elapsed_frames)
        return;

    if (s_nScriptFrameLockEntry != 0) {
        nmlModelSetBackBufferToBattle(sRender.owner_index);
        nmlModelSetFadeInCancel(30);
        nmlModelSetFadeOutCancel(30);
    }
    if (s_nScriptEventSkipEntry != 0) {
        nmlModelSendSignalEventFinish();
        s_nScriptEventSkipEntry = 0;
    }
    s_nScriptFadeOutTime = 30;
    s_nScriptChangeTime = 25;
    s_nScriptFrameLockEntry = 0;
    s_nScriptFadeRequest = 0;
    s_nScriptEventActive = 0;
    s_nScriptTalkLock = 0;
    xglSoundStreamStop(0);
    GameMovieStop();
    GameDefocusSet(-1, 0, 0);
    MAP_initUnit();
    ACT_init();
    ACT_initMTNResource();
    TWSYS_init();
    MSG_init();
    sefInitEffectCf();
    JNI_popFrame();
    GameLoopState.kind = 2;
    entry = &scriptDB[currentScriptDB ^ 1];
    currentScriptDB ^= 1;
    nmlModelSetGlobalPointLightReset();
    s_nScriptCfTime = 0;
    s_nScriptSequenceReset = 0;
    xglRenderClearColor(0x80000000U);
    nmlModelInit();
    GameResourceReset(0);
    GameLoopState.scene_reset_counter = 0;
    MapChange2(MapGetNo());
    for (camera_index = 0; camera_index < 8; camera_index++) {
        StudioCamera *configured_camera;

        xglStudioGetCamera(&camera, camera_index);
        xglCameraClipRangeDefault(camera);
        configured_camera = camera;
        if (camera_index == 0) {
            configured_camera->state = 4;
            configured_camera->active = 1;
        } else {
            configured_camera->active = 0;
        }
    }
    if (entry->thread == 0) {
        task->owner->flags &= ~0x800;
        JNI_loadClassDB(entry->class_slot, 0);
        entry = &scriptDB[(currentScriptDB + 1) & 1];
        entry->thread = 0;
    } else {
        JNI_loadClassDB(entry->class_slot, entry->class_image);
        task->owner->flags &= ~0x800;
        entry = &scriptDB[(currentScriptDB + 1) & 1];
        JNI_loadClassDB(entry->class_slot, 0);
    }
    nmlModelSetGlobalPointLightReset();
    nmlModelSetEffectWrite(1);
    nmlModelSetFadeIn((int)GameLoopState.fade_frames, 0);
    FX_ScreenMask(0,
                  SCRIPT_FADE_COLOR(GameLoopState.fade_red,
                                    GameLoopState.fade_green,
                                    GameLoopState.fade_blue),
                  (int)GameLoopState.fade_frames, 3);
    xglTaskRemove(task_prefix);
}

extern int vmDebugMask;

static const char * vmSwitchStr[12];

extern void TMENU_setItem(ScriptWindow *menu, int index, const char *text);

int DB_lightSave(ScriptFileSelect *state);

void DB_evtMonitor(ScriptMenuTask *task);

static void DB_light(ScriptMenuTask *task)
{
    ScriptPadButtons *pad = (ScriptPadButtons *)PadData;
    unsigned int state_flags;

    state_flags = task->state_flags;
    if (!(state_flags & 1)) {
        int i;

        task->state_flags = state_flags | 1;
        if (pad->held & 0x80)
            initLight3();
        else
            initLight2();

        for (i = 0; i < 64; i++) {
            int word = i >> 5;
            int shift = i & 31;
            if (actor[i].flags & 0x8000)
                lightActFlags[word] |= 1U << shift;
            else
                lightActFlags[word] &= ~(1U << shift);
            actor[i].flags &= ~0x8000;
        }
    }

    if (lightSave != 0) {
        int result = DB_lightSave(fileSelect);

        lightSave = result != 0 ? 0 : lightSave;
        pad->pressed = 0;
    } else {
        if (pad->pressed & 0x10)
            lightSave = 1;
        updateLight(0);
    }

    if (pad->pressed & 0x40) {
        int i;

        for (i = 0; i < 64; i++) {
            if ((lightActFlags[i >> 5] >> (i & 31)) & 1)
                actor[i].flags |= 0x8000;
        }
        task->owner->flags &= ~1;
        task->owner->kind = task->owner->saved_kind;
        TWSYS_setGRPStatus(0, 0);
        xglTaskRemove(&task->task);
    }
}

static void DB_vmSwitch(ScriptMenuTask *task)
{
    ScriptWindow *menu = task->window;
    int item;
    int on;
    int i;
    int bit;
    int item_bit;

    if (!(task->state_flags & 1)) {
        menu = TMENU_create(-1);
        menu->options = 0;
        menu->mode |= 0x10;
        for (i = 0, bit = 1; i < 8 && vmSwitchStr[i * 2] != 0; i++, bit = 1 << (i & 0x1f)) {
            on = (vmDebugMask & bit) != 0;
            TMENU_setItem(menu, -1, vmSwitchStr[i * 2 + on]);
        }
        task->window = menu;
        task->state_flags |= 1;
    }
    if (menu->state != 0)
        return;
    item = menu->selected_item;
    if (menu->cancel_item < 0) {
        menu->state = 2;
        task->owner->flags &= ~1;
        task->owner->kind = task->owner->saved_kind;
        TWSYS_setGRPStatus(0, 0);
        xglTaskRemove(&task->task);
        return;
    }
    item_bit = 1 << (item & 0x1f);
    if (vmDebugMask & item_bit) {
        vmDebugMask &= ~item_bit;
        on = 0;
    } else {
        vmDebugMask |= item_bit;
        on = 1;
    }
    TMENU_setItem(menu, item, vmSwitchStr[item * 2 + on]);
    menu->state = 0xe;
    if (item == 4) {
        ScriptMenuTask *monitor;

        do {
            XglTaskScheduler *scheduler = GameLoopState.task_scheduler;
            monitor = (ScriptMenuTask *)xglTaskEntryNext(
                scheduler, (int (*)(XglTaskPrefix *))DB_evtMonitor,
                scheduler != 0 ? scheduler->active_tail : 0);
            if (monitor != 0) {
                monitor->state_flags = 0;
                monitor->window = 0;
                monitor->owner = &GameLoopState;
            }
        } while (0);
    }
}

static int DB_stageSelect(ScriptMenuTask *task)
{
    ScriptWindow *slider;
    const char *map_name;
    int value;

    if (!(task->state_flags & 1)) {
        task->state_flags |= 1;
        slider = TSLIDER_create(-1);
        slider->max_value = 2000;
        slider->mode |= 0x10;
        slider->x = 256.0f;
        slider->y = 224.0f;
        slider->value = MapGetNo();
        task->window = slider;
        task->phase = 0;
        task->loaded_map = 0;
    }
    slider = task->window;
    switch (task->phase) {
    case 0:
        if (slider->value >= 0) {
            map_name = MapGetName(slider->value);
            if (map_name != 0)
                xglFontDebugPrintf(8, 32, map_name);
        }
        if (slider->state == 2) {
            if (slider->value >= 0) {
                task->phase = 1;
                task->map_no = slider->value;
                slider = TSLIDER_create(-1);
                task->window = slider;
                slider->x = 256.0f;
                slider->y = 224.0f;
                slider->mode |= 0x10;
                break;
            }
            task->owner->flags &= ~1;
            task->owner->kind = task->owner->saved_kind;
            TWSYS_setGRPStatus(0, 0);
            return xglTaskRemove(&task->task);
        }
        break;
    case 1:
        xglFontDebugPrintf(8, 32, "Entrance:");
        if (slider->state == 2) {
            value = slider->value;
            if (value >= 0) {
                task->loaded_map = value;
                LoadMap2(task->map_no, value);
            }
            task->owner->flags &= ~1;
            task->owner->kind = task->owner->saved_kind;
            TWSYS_setGRPStatus(0, 0);
            return xglTaskRemove(&task->task);
        }
    }
}

void DB_memory(ScriptMenuTask *task)
{
    ScriptPadButtons *pad = (ScriptPadButtons *)PadData;
    char text[0x100];
    float matrix[4][4];
    float scale[4];
    float translation[4];
    float heap_bytes;

    if ((task->state_flags & 1) == 0) {
        task->state_flags = task->state_flags | 1;
        heapUsed = xheap_info(heapInfo);
    }

    sprintf(text, (const char *)memoryUsageFormat, heapUsed >> 10, 0x100);
    xglFontDebugPrintf(0x10, 0x40, text);
    heap_bytes = (float)heapUsed;
    scale[0] = heap_bytes * 128.0f * 0.0000038146973f;
    scale[2] = 1.0f;
    scale[3] = 1.0f;
    scale[1] = 24.0f;
    translation[0] = -64.0f;
    translation[1] = 64.0f;
    translation[2] = 0.0f;
    translation[3] = 1.0f;
    xglMatrixStackUnit();
    xglMatrixStackTrans(translation);
    xglMatrixStackScale(scale);
    xglMatrixStackSave(matrix);
    if (heapUsed <= 0x2aaa9)
        drawRect2(&matrix, 1);
    else
        drawRect2(&matrix, 0);

    scale[0] = 132.0f;
    scale[1] = 28.0f;
    scale[2] = 1.0f;
    scale[3] = 1.0f;
    translation[0] = 0.0f;
    translation[1] = 52.0f;
    translation[2] = 0.0f;
    translation[3] = 1.0f;
    xglMatrixStackUnit();
    xglMatrixStackTrans(translation);
    xglMatrixStackScale(scale);
    xglMatrixStackSave(matrix);
    drawBox2(&matrix, 6);

    if (pad->pressed & 0x40) {
        task->owner->flags &= ~1;
        task->owner->kind = task->owner->saved_kind;
        TWSYS_setGRPStatus(0, 0);
        xglTaskRemove(&task->task);
    }
}

int DB_lightSave(ScriptFileSelect *state)
{
    char path[512];
    int result;
    char *handle;
    int file;
    ScriptPadButtons *pad = (ScriptPadButtons *)PadData;

    result = xglCdFileSelect(state);
    switch (result) {
    case 0:
        break;
    case 1:
        RSRC_init(rcFile);
        handle = RSRC_loadFile2(rcFile, state->directory, state->file_path);
        JS_exec(handle, handle + *RSRC_getItem(rcFile, handle));
        return 0;
    case 2:
        return 1;
    }
    if (pad->repeat & 0x8000)
        lightFileNum--;
    if (pad->repeat & 0x2000)
        lightFileNum++;
    if (lightFileNum < 0)
        lightFileNum = 99;
    if (lightFileNum >= 100)
        lightFileNum = 0;
    DB_reset(32, 160);
    /* Circle button: load.
     * EN: Filled circle button: Load */
    DB_println("\xa1\xfc\xa5\xdc\xa5\xbf\xa5\xf3\xa1\xa7\xa5\xed\xa1\xbc\xa5\xc9");
    /* Square button: save.
     * EN: Filled square button: Save */
    DB_println("\xa2\xa3\xa5\xdc\xa5\xbf\xa5\xf3\xa1\xa7\xa5\xbb\xa1\xbc\xa5\xd6");
    DB_println("lgt%02d.rc", lightFileNum);
    if (pad->pressed & 0x80) {
        sprintf(path, "%s%slgt%02d.rc", state->directory, state->file_path, lightFileNum);
        file = sceOpen(path, 0x602);
        if (file >= 0) {
            DB_lightWrite(file);
            sceClose(file);
        }
        return 1;
    }
    return 0;
}

static int DB_scriptFileSelect(ScriptMenuTask *task)
{
    ScriptFileSelect *browser;
    char *extension;
    char *handle;

    if (!(task->state_flags & 1)) {
        fileSelect->selection = 0;
        task->window = fileSelect;
        task->state_flags |= 1;
    }
    browser = task->window;
    switch (xglCdFileSelect(browser)) {
    case 0:
        break;
    case 1:
        extension = strrchr(browser->file_path, '.');
        if (extension != 0) {
            if (strncmp(extension, ".evt", 4) == 0) {
                SCRIPT_load_DBG(browser->file_path);
                SCRIPT_exec();
            } else if (strncmp(extension, ".rc", 3) == 0) {
                RSRC_init(rcFile);
                handle = RSRC_loadFile2(rcFile, browser->directory, browser->file_path);
                JS_exec(handle, handle + *RSRC_getItem(rcFile, handle));
                task->owner->flags &= ~1;
                task->owner->kind = task->owner->saved_kind;
            }
        }
        task->owner->flags &= ~1;
        TWSYS_setGRPStatus(0, 0);
        return xglTaskRemove(&task->task);
    case 2:
        task->owner->flags &= ~1;
        task->owner->kind = task->owner->saved_kind;
        TWSYS_setGRPStatus(0, 0);
        return xglTaskRemove(&task->task);
    }
}

struct ScriptWindParams {
    char mode;
    unsigned char unmodeled_01[7];
    unsigned long long unmodeled_08;
    Vector4 direction;
    float shake_power;
    float shake_time;
    float shake_rad;
    unsigned char unmodeled_2c[4];
};

static ScriptWindParams dbgWind;

extern void updateWind(ScriptWindParams *params);

extern void EXM_GetWindPara(ScriptWindParams *params);

extern void EXM_SetWindStruct(ScriptWindParams *params);

extern void EXM_ClearWindStruct(void);

extern void EXM_SetWindPara(ScriptWindParams *params);

static int DB_wind(ScriptMenuTask *task)
{
    ScriptPadButtons *pad = (ScriptPadButtons *)&PadData[1];

    if (!(task->state_flags & 1)) {
        task->state_flags |= 1;
        EXM_GetWindPara(&dbgWind);
        EXM_SetWindStruct(&dbgWind);
        GameLoopState.flags &= ~1;
        GameLoopState.debug_menu_storage = 0;
        GameLoopState.kind = GameLoopState.saved_kind;
    }
    updateWind(&dbgWind);
    if (pad->pressed & 0x40) {
        EXM_ClearWindStruct();
        EXM_SetWindPara(&dbgWind);
        task->owner->flags &= ~1;
        task->owner->kind = task->owner->saved_kind;
        TWSYS_setGRPStatus(0, 0);
        return xglTaskRemove(&task->task);
    }
}

void SCRIPT_debugMenu(ScriptMenuTask *task)
{
    ScriptWindow *menu = task->window;
    ScriptMenuTask *created;

    if (!(task->state_flags & 1)) {
        TWSYS_setGRPStatus(0, 3);
        menu = TMENU_create(-1);
        menu->options = 0;
        menu->mode |= 0x10;
        TMENU_addItem(menu, "ScriptExec\nBgChange\nVirtualM\nTool\nLight\nMemoryInfo\nWind");
        menu->flags &= ~0x40;
        task->window = menu;
        task->state_flags |= 1;
    }
    if (menu->state != 0)
        return;
    menu->state = 2;
    switch (menu->selected_item) {
    case 0:
        do
        {
            XglTaskScheduler *scheduler = GameLoopState.task_scheduler;
            (created) = (ScriptMenuTask *)xglTaskEntryNext( scheduler, (int (*)(XglTaskPrefix *))(DB_scriptFileSelect), scheduler != 0 ? scheduler->active_tail : 0);
            if ((created) != 0)
            {
                (created)->state_flags = 0;
                (created)->window = 0;
                (created)->owner = &GameLoopState;
            }

        }
        while (0);
        break;
    case 1:
        do
        {
            XglTaskScheduler *scheduler = GameLoopState.task_scheduler;
            (created) = (ScriptMenuTask *)xglTaskEntryNext( scheduler, (int (*)(XglTaskPrefix *))(DB_stageSelect), scheduler != 0 ? scheduler->active_tail : 0);
            if ((created) != 0)
            {
                (created)->state_flags = 0;
                (created)->window = 0;
                (created)->owner = &GameLoopState;
            }

        }
        while (0);
        break;
    case 2:
        do
        {
            XglTaskScheduler *scheduler = GameLoopState.task_scheduler;
            (created) = (ScriptMenuTask *)xglTaskEntryNext( scheduler, (int (*)(XglTaskPrefix *))(DB_vmSwitch), scheduler != 0 ? scheduler->active_tail : 0);
            if ((created) != 0)
            {
                (created)->state_flags = 0;
                (created)->window = 0;
                (created)->owner = &GameLoopState;
            }

        }
        while (0);
        break;
    case 3:
        initCursor();
        task->owner->kind = 7;
        break;
    case 4:
        do
        {
            XglTaskScheduler *scheduler = GameLoopState.task_scheduler;
            (created) = (ScriptMenuTask *)xglTaskEntryNext( scheduler, (int (*)(XglTaskPrefix *))(DB_light), scheduler != 0 ? scheduler->active_tail : 0);
            if ((created) != 0)
            {
                (created)->state_flags = 0;
                (created)->window = 0;
                (created)->owner = &GameLoopState;
            }

        }
        while (0);
        break;
    case 5:
        do
        {
            XglTaskScheduler *scheduler = GameLoopState.task_scheduler;
            (created) = (ScriptMenuTask *)xglTaskEntryNext( scheduler, (int (*)(XglTaskPrefix *))(DB_memory), scheduler != 0 ? scheduler->active_tail : 0);
            if ((created) != 0)
            {
                (created)->state_flags = 0;
                (created)->window = 0;
                (created)->owner = &GameLoopState;
            }

        }
        while (0);
        created->owner->saved_kind = task->owner->saved_kind;
        break;
    case 6:
        do
        {
            XglTaskScheduler *scheduler = GameLoopState.task_scheduler;
            (created) = (ScriptMenuTask *)xglTaskEntryNext( scheduler, (int (*)(XglTaskPrefix *))(DB_wind), scheduler != 0 ? scheduler->active_tail : 0);
            if ((created) != 0)
            {
                (created)->state_flags = 0;
                (created)->window = 0;
                (created)->owner = &GameLoopState;
            }

        }
        while (0);
        break;
    default:
        if (menu->cancel_item < 0) {
            task->owner->flags &= ~1;
            task->owner->kind = task->owner->saved_kind;
            TWSYS_setGRPStatus(0, 0);
        }
        break;
    }
    xglTaskRemove(&task->task);
}

int loadScriptCD(ScriptDbEntry *entry, const char *path)
{
    char full_path[0x100];
    int result;
    int resource_read_size;
    const char *active;
    int skip_count;

    DebugMenuResourceSizeAdd = 0;
    strcpy(full_path, MAP_getPath());
    strcat(full_path, path);
    result = xglCdReadFile(full_path, entry->class_image, 0, 1);
    if (result < 0) {
        entry->skip_count = (skip_count = 0);
        entry->active = 0;
    } else {
        active = JNI_searchClasses(entry->class_image, sceneStageClassPath,
                                   2, &skip_count);
        replacePathExt(full_path, fpkExtension);
        resource_read_size = xglCdReadFile(full_path, entry->motion_data, 0, 1);
        if (resource_read_size < 0) {
            ACT_initExMotion(0, 1, 0);
        } else {
            DebugMenuResourceSizeAdd += resource_read_size;
            ACT_initExMotion(0, 1, entry->motion_data);
        }

        replacePathExt(full_path, arcExtension);
        resource_read_size = xglCdReadFile(full_path, entry->pdb, 0, 1);
        if (resource_read_size >= 0)
            DebugMenuResourceSizeAdd += resource_read_size;
        result = skip_count;
        entry->active = active;
        entry->skip_count = result;
    }
    return result;
}

int loadScriptCD2(ScriptDbEntry *entry, const char *path)
{
    ScriptPathBuffer resource_path;
    const char *base_name;
    int read_size;
    int skip_count;
    const char *active;

    DebugMenuResourceSizeAdd = 0;
    strcpy(resource_path.bytes, path);
    if ((read_size = xglCdReadFile(resource_path.bytes, entry->class_image, 0, 1)) < 0) {
        entry->skip_count = (skip_count = 0);
        entry->active = 0;
        return read_size;
    }
    active = JNI_searchClasses(entry->class_image, sceneStageClassPath, 2, &skip_count);
    base_name = strrchr(path, '\\');
    if (base_name == 0)
        base_name = strrchr(path, '/');
    resource_path.words[0] = mtnpackPathPrefix.words[0];
    resource_path.bytes[8] = mtnpackPathPrefix.bytes[8];
    if (base_name != 0)
        strcat(resource_path.bytes, base_name + 1);
    replacePathExt(resource_path.bytes, fpkExtension);
    entry->motion_data = RES_loadFile(-1, 5, resource_path.bytes, 0);
    ACT_initExMotion(0, 1, entry->motion_data);
    replacePathExt(resource_path.bytes, arcExtension);
    entry->pdb = RES_loadFile(-1, 5, resource_path.bytes, 0);
    entry->active = active;
    entry->skip_count = skip_count;
    return (int)entry->pdb;
}

extern int sceOpen(const char *path, int flags, ...);

extern int sceClose(int descriptor);

const char *loadScript(ScriptDbEntry *entry, const char *path_base, const char *path)
{
    struct ScriptImageHeaderBytes {
        unsigned char bytes[12];
    };
    union ScriptClassImageHeader {
        struct {
            unsigned int unmodeled_00[2];
            unsigned int image_size;
        } fields;
        struct ScriptImageHeaderBytes raw;
    };
    char full_path[0x100];
    union ScriptClassImageHeader header_storage;
    int fd;
    int resource_fd;
    const char *result = 0;
    int class_size;
    int skip_count;

    DebugMenuResourceSizeAdd = 0;
    if (path_base != 0) {
        strcpy(full_path, path_base);
        strcat(full_path, path);
        fd = sceOpen(full_path, 1);
        if (fd < 0)
            return 0;
        {
            union ScriptClassImageHeader *header = &header_storage;

            sceRead(fd, header, sizeof(*header));
            {
                union ScriptClassImageHeader *image_header = entry->class_image;
                image_header->raw = header->raw;
            }
            class_size = (int)header->fields.image_size - sizeof(*header);
            if (class_size != sceRead(fd, (unsigned char *)entry->class_image + sizeof(*header),
                                     (unsigned int)class_size))
                return 0;
        }
        sceClose(fd);
        replacePathExt(full_path, fpkExtension);
        resource_fd = sceOpen(full_path, 1);
        if (resource_fd >= 0) {
            int file_size = sceLseek(resource_fd, 0, 2);

            sceLseek(resource_fd, 0, 0);
            sceRead(resource_fd, entry->motion_data, (unsigned int)file_size);
            sceClose(resource_fd);
            ACT_initExMotion(0, 1, entry->motion_data);
            DebugMenuResourceSizeAdd += file_size;
        } else {
            ACT_initExMotion(0, 1, 0);
        }
        replacePathExt(full_path, arcExtension);
        resource_fd = sceOpen(full_path, 1);
        if (resource_fd >= 0) {
            int file_size = sceLseek(resource_fd, 0, 2);

            sceLseek(resource_fd, 0, 0);
            sceRead(resource_fd, entry->pdb, (unsigned int)file_size);
            sceClose(resource_fd);
            DebugMenuResourceSizeAdd += file_size;
        }
    }

    result = JNI_searchClasses(entry->class_image,
                               sceneStageClassPath,
                               2, &skip_count);
    entry->active = result;
    entry->skip_count = skip_count;
    return result;
}

void DB_evtMonitor(ScriptMenuTask *task)
{
    char model_path[256];
    ScriptEventDebugState *debug = &evtDebug;
    ScriptPadButtons *pad;
    SceneThread *thread;
    int mode;
    int selected;

    DB_cmode(1);
    if (!(vmDebugMask & 0x10)) {
        debug->flags &= ~1;
        xglTaskRemove(&task->task);
        return;
    }
    if (!(task->state_flags & 1)) {
        if (debug->flags & 1) {
            xglTaskRemove(&task->task);
            return;
        }
        task->state_flags |= 1;
        debug->flags |= 1;
    }
    DB_reset(0, 0);
    DB_println(evtMonitorLabel);
    thread = scriptDB[currentScriptDB].thread;
    {
    SceneThread *debug_thread = ((thread));
    if (debug_thread != 0 && debug_thread->object != 0) {
        {
        SceneClass *debug_class =
            ((SceneObjectHeader *)((debug_thread->object)))->class_ref->scene_class;
        SceneMethod *debug_method = ((debug_thread))->method;
        if ((((debug_thread))->flags & 0x10) && debug_method != 0) {
            DB_println(classMethodFormat, debug_class->name->binary_name,
                       ((ConstString *)debug_method->name)->bytes);
            if (((debug_thread))->frame_depth > 0) {
                int frame_index;
                for (frame_index = (((0)));
                     frame_index < ((debug_thread))->frame_depth;
                     ++frame_index) {
                    ScriptCallFrame *frame =
                        &((debug_thread))->frames[frame_index];
                    DB_println(evtFrameFormat, frame->program_counter,
                               ((ConstString *)frame->method->name)->bytes);
                }
            } else {
                DB_println(classMethodFormat, debug_class->name->binary_name,
                           ((ConstString *)debug_method->name)->bytes);
            }
        }
};
    }
};

    mode = debug->mode;
    switch (mode) {
    case 0: {
        ScriptActorRecord *actor_record;
        const char *base_name;
        float half_turn;
        SceneObject object;

        pad = (ScriptPadButtons *)PadData;
        if (pad->pressed & 2)
            debug->mode = 1;
        if (pad->repeat & 0x8000)
            --debug->selected;
        if (pad->repeat & 0x2000)
            ++debug->selected;
        if (debug->selected < 0)
            debug->selected = 63;
        if (debug->selected >= 64)
            debug->selected = 0;
        selected = debug->selected;
        actor_record = &actor[selected];
        RES_GetMdlFileName(model_path, actor_record->in_use_id);
        base_name = strrchr(model_path, '\\');
        if (base_name == 0)
            base_name = unknownBaseName;
        ++base_name;
        DB_reset(0, 10);
        DB_println(evtActorFormat, debug->selected, actor_record->in_use_id,
                   base_name);
        half_turn = 3.1415927f;
        DB_println(evtFlagsFormat, actor_record->flags);
        DB_println(evtPositionFormat, actor_record->position[0],
                   actor_record->position[1], actor_record->position[2]);
        DB_println(evtRotationFormat,
                   actor_record->rotation[0] / half_turn * 180.0f,
                   actor_record->rotation[1] / half_turn * 180.0f,
                   actor_record->rotation[2] / half_turn * 180.0f);
        object = actor_record->script_object;
        if (object != 0) {
            thread = JTHREAD_get(object);
            if (thread != 0) {
                object = thread->object;
                if (object != 0) {
                    {
        SceneClass *debug_class =
            ((SceneObjectHeader *)((object)))->class_ref->scene_class;
        SceneMethod *debug_method = ((thread))->method;
        if ((((thread))->flags & 0x10) && debug_method != 0) {
            DB_println(classMethodFormat, debug_class->name->binary_name,
                       ((ConstString *)debug_method->name)->bytes);
            if (((thread))->frame_depth > 0) {
                int frame_index;
                for (frame_index = ((mode));
                     frame_index < ((thread))->frame_depth;
                     ++frame_index) {
                    ScriptCallFrame *frame =
                        &((thread))->frames[frame_index];
                    DB_println(evtFrameFormat, frame->program_counter,
                               ((ConstString *)frame->method->name)->bytes);
                }
            } else {
                DB_println(classMethodFormat, debug_class->name->binary_name,
                           ((ConstString *)debug_method->name)->bytes);
            }
        }
};
                }
            }
        }
        break;
    }
    default: {
        MapUnitSlot *unit;
        UnitSequenceEntry *sequence;
        SceneObject object;
        float half_turn;

        pad = (ScriptPadButtons *)PadData;
        if (pad->pressed & 2)
            debug->mode = (mode + 1) & 1;
        if (pad->repeat & 0x8000)
            --debug->selected;
        if (pad->repeat & 0x2000)
            ++debug->selected;
        if (debug->selected < 0)
            debug->selected = 63;
        if (debug->selected >= 64)
            debug->selected = 0;
        selected = debug->selected;
        unit = &MapUnit[selected];
        DB_reset(0, 10);
        sequence = &unitSequence[unit->slot];
        DB_println(evtUnitFormat, debug->selected, unit->serial);
        DB_println(evtFlagsFormat, unit->flags);
        DB_println(cmdLabel);
        if (sequence->state & 1)
            DB_println(moveLabel);
        else
            DB_println(separatorLabel);
        half_turn = 3.1415927f;
        DB_println(evtPositionFormat, unit->position[0], unit->position[1], unit->position[2]);
        DB_println(evtRotationFormat, unit->rotation[0] / half_turn * 180.0f,
                   unit->rotation[1] / half_turn * 180.0f,
                   unit->rotation[2] / half_turn * 180.0f);
        object = unit->sequence;
        if (object != 0) {
            thread = JTHREAD_get(object);
            if (thread != 0) {
                object = thread->object;
                if (object != 0) {
                    {
        SceneClass *debug_class =
            ((SceneObjectHeader *)((object)))->class_ref->scene_class;
        SceneMethod *debug_method = ((thread))->method;
        if ((((thread))->flags & 0x10) && debug_method != 0) {
            DB_println(classMethodFormat, debug_class->name->binary_name,
                       ((ConstString *)debug_method->name)->bytes);
            if (((thread))->frame_depth > 0) {
                int frame_index;
                for (frame_index = ((0));
                     frame_index < ((thread))->frame_depth;
                     ++frame_index) {
                    ScriptCallFrame *frame =
                        &((thread))->frames[frame_index];
                    DB_println(evtFrameFormat, frame->program_counter,
                               ((ConstString *)frame->method->name)->bytes);
                }
            } else {
                DB_println(classMethodFormat, debug_class->name->binary_name,
                           ((ConstString *)debug_method->name)->bytes);
            }
        }
};
                }
            }
        }
        break;
    }
    }
    DB_cmode(0);
}

ScriptDbEntry *SCRIPT_exec(void)
{
    ScriptDbEntry *entry;
    SceneObject object;
    JThread *thread;
    ScriptSceneTask *task;
    XglTaskScheduler *scheduler;
    JThread *vm;
    StudioCamera *stage_camera;
    StudioCamera *scene_camera;
    int camera_index;

    XTK_setWindowOwner(0);
    currentScriptDB ^= 1;
    entry = &scriptDB[(currentScriptDB + 1) & 1];
    JNI_loadClassDB(entry->class_slot, 0);
    entry = &scriptDB[currentScriptDB];
    PLAY_setupDefault(PLAY_getCurrent());
    if (entry->active == 0) {
        ScriptActorRecord *actor;

        JNI_popFrame();
        XTK_setResourceID(1);
        GameLoopState.kind = 2;
        nmlModelSetGlobalPointLightReset();
        s_nScriptCfTime = 0;
        TCAMERA_init();
        TWSYS_init();
        MSG_init();
        ACT_init();
        MAP_initUnit();
        InitCfSystem();
        actor = ACT_create(0, 1);
        ACT_initMotion(actor);
        ACT_loadMotion(actor, actor->in_use_id, 1);
        ACT_loadResource(actor, actor->in_use_id);
        GameLoopState.main_actor = actor;
        entry->skip_count = 1;
        entry->thread = 0;
        JNI_loadClassDB(entry->class_slot, 0);
        JNI_pushFrame();
        JNI_loadNativeClass();
        return 0;
    }
    if (GameLoopState.flags & 0x800) {
        JNI_popFrame();
        TCAMERA_init();
        TWSYS_init();
        MSG_init();
        ACT_init();
        MAP_initUnit();
        JNI_pushFrame();
        JNI_loadNativeClass();
    }
    if (entry->skip_count != 0) {
        if (entry->skip_count == 1) {
            ScriptActorRecord *actor;
            int camera_index;

            camera_index = 0;
            JNI_popFrame();
            XTK_setResourceID(1);
            GameLoopState.kind = 2;
            nmlModelSetGlobalPointLightReset();
            s_nScriptCfTime = 0;
            while (camera_index < 8) {
                StudioCamera *configured_camera;

                xglStudioGetCamera(&stage_camera, camera_index);
                xglCameraClipRangeDefault(stage_camera);
                configured_camera = stage_camera;
                if (camera_index == 0) {
                    configured_camera->state = 4;
                    configured_camera->active = 1;
                } else {
                    configured_camera->active = 0;
                }
                ++camera_index;
                configured_camera->position.z = 1000000.0f;
                configured_camera->position.y = 1000000.0f;
                configured_camera->position.x = 1000000.0f;
                configured_camera->rotation.y = 1.5707964f;
                configured_camera->rotation.x = 0.0f;
                configured_camera->rotation.z = 0.0f;
            }
            s_nScriptTalkLock = 0;
            TCAMERA_init();
            TWSYS_init();
            MSG_init();
            nmlModelInit();
            ACT_init();
            MAP_initUnit();
            InitCfSystem();
            s_nScriptSequenceReset = 0;
            xglRenderClearColor(0x80000000);
            actor = ACT_create(0, 1);
            ACT_initMotion(actor);
            ACT_loadMotion(actor, actor->in_use_id, 1);
            ACT_loadResource(actor, actor->in_use_id);
            GameLoopState.main_actor = actor;
            JNI_pushFrame();
            JNI_loadNativeClass();
            JNI_loadClassDB(entry->class_slot, entry->class_image);
            object = newObject(loadClass(
                loadConstString((const char *)entry->active, -1), 0));
            thread = JNI_createThread(2, 6, 0x46);
            *(void **)&thread->object = object;
            vm = defaultVM;
            entry->thread = thread;
            STAGE_instance(vm, object);
        }
        return entry;
    }

    XTK_setResourceID(2);
    nmlModelSetGlobalPointLightReset();
    nmlModelSetFadeIn((int)GameLoopState.fade_frames, 0);
    FX_ScreenMask(0,
                  SCRIPT_FADE_COLOR(GameLoopState.fade_red,
                                    GameLoopState.fade_green,
                                    GameLoopState.fade_blue),
                  (int)GameLoopState.fade_frames, 3);
    {
        int camera_index = 0;

        while (camera_index < 8) {
            StudioCamera *configured_camera;

            xglStudioGetCamera(&scene_camera, camera_index);
            xglCameraClipRangeDefault(scene_camera);
            configured_camera = scene_camera;
            if (camera_index == 0) {
                configured_camera->state = 4;
                configured_camera->active = 1;
            } else {
                configured_camera->active = 0;
            }
            ++camera_index;
            configured_camera->position.z = 1000000.0f;
            configured_camera->position.y = 1000000.0f;
            configured_camera->position.x = 1000000.0f;
            configured_camera->rotation.y = 1.5707964f;
            configured_camera->rotation.x = 0.0f;
            configured_camera->rotation.z = 0.0f;
        }
    }
    s_nScriptTalkLock = 0;
    TCAMERA_init();
    TWSYS_init();
    MSG_init();
    {
    unsigned int *event_active = &s_nScriptEventActive;
    unsigned int *sequence_reset = &s_nScriptSequenceReset;
    GameLoopState.kind = 3;
    *event_active = 1;
    GameLoopState.flags |= 0x800;
    *sequence_reset = 0;
    }
    xglRenderClearColor(0x80000000);
    nmlModelInit();
    GameResourceReset(0);
    GameLoopState.scene_reset_counter = 0;
    UnduInit(0);
    ACT_matrixInit();
    ACT_initScene();
    MAP_initUnit();
    JNI_pushFrame();
    JNI_loadNativeClass();
    JNI_loadClassDB(entry->class_slot, entry->class_image);
    scheduler = GameLoopState.task_scheduler;
    task = (ScriptSceneTask *)xglTaskEntryNext(
        scheduler, (int (*)(XglTaskPrefix *))sceneObserver,
        scheduler != 0 ? scheduler->active_tail : 0);
    if (task != 0) {
        task->thread = 0;
        task->owner = &GameLoopState;
        task->state_flags = 0;
    }
    return entry;
}

ScriptDbEntry *SCRIPT_exec2(void)
{
    ScriptDbEntry *entry;
    SceneObject object;
    SceneThread *thread;
    ScriptSceneTask *task;
    XglTaskScheduler *scheduler;
    StudioCamera *camera;
    StudioCamera *target;
    int camera_index;
    SceneThread *vm;
    int (*callback)(XglTaskPrefix *);

    XTK_setWindowOwner(0);
    currentScriptDB ^= 1;
    entry = &scriptDB[(currentScriptDB + 1) & 1];
    JNI_loadClassDB(entry->class_slot, 0);
    entry = &scriptDB[currentScriptDB];
    PLAY_setupDefault(PLAY_getCurrent());
    if (entry->active == 0) {
        s_nScriptChangeTime = 0x19;
        JNI_popFrame();
        XTK_setResourceID(1);
        GameLoopState.kind = 2;
        nmlModelSetGlobalPointLightReset();
        s_nScriptCfTime = 0;
        s_nScriptTalkLock = 0;
        TCAMERA_init();
        TWSYS_init();
        MSG_init();
        entry->thread = 0;
        JNI_loadClassDB(entry->class_slot, 0);
        JNI_pushFrame();
        JNI_loadNativeClass();
        return entry;
    }
    if (GameLoopState.flags & 0x800) {
        s_nScriptTalkLock = 0;
        TCAMERA_init();
        TWSYS_init();
        MSG_init();
    }
    if (entry->skip_count != 0) {
        if (entry->skip_count == 1) {
            camera_index = 0;
            JNI_popFrame();
            XTK_setResourceID(1);
            GameLoopState.kind = 2;
            nmlModelSetGlobalPointLightReset();
            s_nScriptCfTime = 0;
            do {
                xglStudioGetCamera(&camera, camera_index);
                target = camera;
                if (camera_index == 0) {
                    target->state = 4;
                    target->active = 1;
                } else {
                    target->active = 0;
                }
                camera_index++;
            } while (camera_index < 8);
            s_nScriptTalkLock = 0;
            TCAMERA_init();
            TWSYS_init();
            MSG_init();
            s_nScriptChangeTime = 0x19;
            JNI_pushFrame();
            JNI_loadNativeClass();
            JNI_loadClassDB(entry->class_slot, entry->class_image);
            object = newObject(
                loadClass(loadConstString((const char *)entry->active, -1), 0));
            thread = JNI_createThread(2, 6, 0x46);
            entry->thread = thread;
            thread->object = object;
            STAGE_instance(defaultVM, object);
        }
        return entry;
    }
    XTK_setResourceID(2);
    GameLoopState.kind = 3;
    GameLoopState.flags |= 0x800;
    s_nScriptEventActive = 1;
    nmlModelSetGlobalPointLightReset();
    nmlModelSetFadeIn((int)GameLoopState.fade_frames, 0);
    FX_ScreenMask(0,
                  SCRIPT_FADE_COLOR(GameLoopState.fade_red,
                                    GameLoopState.fade_green,
                                    GameLoopState.fade_blue),
                  (int)GameLoopState.fade_frames, 3);
    s_nScriptChangeTime = 0x19;
    s_nScriptTalkLock = 0;
    TCAMERA_init();
    TWSYS_init();
    MSG_init();
    JNI_popFrame();
    JNI_pushFrame();
    JNI_loadNativeClass();
    JNI_loadClassDB(entry->class_slot, entry->class_image);
    object = newObject(
        loadClass(loadConstString((const char *)entry->active, -1), 0));
    thread = JNI_createThread(2, 6, 0x46);
    callback = (int (*)(XglTaskPrefix *))sceneObserver2;
    scheduler = GameLoopState.task_scheduler;
    thread->object = object;
    entry->thread = thread;
    task = (ScriptSceneTask *)xglTaskEntryNext(
        scheduler, callback,
        scheduler != 0 ? scheduler->active_tail : 0);
    if (task != 0) {
        task->state_flags = 0;
        task->owner = &GameLoopState;
        task->thread = 0;
    }
    vm = defaultVM;
    task->thread = thread;
    SCENE_instance(vm, object);
    SCENE_start(defaultVM, object, 8, "play");
    return entry;
}

void XTK_setResourceID(int id)
{
    resourceID = id;
}

int XTK_getResourceID(void)
{
    return resourceID;
}

void XTK_setWindowOwner(int owner)
{
    windowOwner = owner;
}

int XTK_getWindowOwner(void)
{
    return windowOwner;
}

int XTK_findFile(const char *path)
{
    ScriptPdbFile *file;

    file = PDB_findFile(scriptDB[currentScriptDB].pdb, path);
    if (file != 0)
        return file->data;
    return 0;
}

extern const char fileOffText[24];
extern const char execOffText[24];
extern const char execOnText[24];
extern const char monitorOffText[24];
extern const char initOnText[24];
extern const char memoryOnText[24];
extern const char memoryOffText[24];
extern const char initOffText[24];
extern const char monitorOnText[24];
extern const char fileOnText[24];

static char sig0_0[16] = "(Lxeno/Enepc;)V";

static char sig1_1[34] = "(Lxeno/Enepc;Lxeno/util/Window;)V";

static const char * vmSwitchStr[12] = { memoryOffText, memoryOnText, initOffText, initOnText, fileOffText, fileOnText, execOffText, execOnText, monitorOffText, monitorOnText, 0, 0 };

const char systemEvtPath[16] = "data\\system.evt";

const char baseEvtPath[16] = "data\\base.evt";

/* JP: ＭＯＮＩＴＯＲ　ＯＮ
 * EN: MONITOR ON */
const char monitorOnText[24] = "\243\315\243\317\243\316\243\311\243\324\243\317\243\322\241\241\243\317\243\316";

/* JP: ＭＯＮＩＴＯＲ　ＯＦＦ
 * EN: MONITOR OFF */
const char monitorOffText[24] = "\243\315\243\317\243\316\243\311\243\324\243\317\243\322\241\241\243\317\243\306\243\306";

/* JP: ＥＸＥＣ  ＯＮ
 * EN: EXEC  ON */
const char execOnText[24] = "\243\305\243\330\243\305\243\303\241\241\241\241\241\241\241\241\243\317\243\316";

/* JP: ＥＸＥＣ  ＯＦＦ
 * EN: EXEC  OFF */
const char execOffText[24] = "\243\305\243\330\243\305\243\303\241\241\241\241\241\241\241\241\243\317\243\306\243\306";

/* JP: ＦＩＬＥ  ＯＮ
 * EN: FILE  ON */
const char fileOnText[24] = "\243\306\243\311\243\314\243\305\241\241\241\241\241\241\241\241\243\317\243\316";

/* JP: ＦＩＬＥ  ＯＦＦ
 * EN: FILE  OFF */
const char fileOffText[24] = "\243\306\243\311\243\314\243\305\241\241\241\241\241\241\241\241\243\317\243\306\243\306";

/* JP: ＩＮＩＴ  ＯＮ
 * EN: INIT  ON */
const char initOnText[24] = "\243\311\243\316\243\311\243\324\241\241\241\241\241\241\241\241\243\317\243\316";

/* JP: ＩＮＩＴ  ＯＦＦ
 * EN: INIT  OFF */
const char initOffText[24] = "\243\311\243\316\243\311\243\324\241\241\241\241\241\241\241\241\243\317\243\306\243\306";

/* JP: ＭＥＭＯＲＹ  ＯＮ
 * EN: MEMORY  ON */
const char memoryOnText[24] = "\243\315\243\305\243\315\243\317\243\322\243\331\241\241\241\241\243\317\243\316";

/* JP: ＭＥＭＯＲＹ  ＯＦＦ
 * EN: MEMORY  OFF */
const char memoryOffText[24] = "\243\315\243\305\243\315\243\317\243\322\243\331\241\241\241\241\243\317\243\306\243\306";

const unsigned char memoryUsageFormat[16] = "MEM %3d / %d K\n";

const char sceneStageClassPath[24] = "xeno/Scene:xeno/Stage";

const ScriptPathPrefix mtnpackPathPrefix = { .bytes = "mtnpack\\" };

const char evtMonitorLabel[16] = "evtMonitor";

const char evtFrameFormat[16] = "%04x: %s ";

const char evtActorFormat[24] = "CHR[%02x] %04x:%s";

const char evtFlagsFormat[16] = "flags %08x";

const char evtPositionFormat[32] = "pos(%6.3f, %6.3f, %6.3f)";

const char evtRotationFormat[32] = "rot(%6.3f, %6.3f, %6.3f)";

const char evtUnitFormat[16] = "UNIT[%02x] %04x";

static int lightSave = 0;

char fpkExtension[8] = ".fpk";

char arcExtension[8] = ".arc";

char classMethodFormat[8] = "%s.%s";

char unknownBaseName[8] = "\\\?\?\?\?";

char cmdLabel[8] = "cmd";

char moveLabel[8] = "move";

char separatorLabel[8] = "----";

const char DBCursorModeFcvLabel[8] = "FCV ";

const char DBCursorModeXyLabel[8] = "XY  ";

const char DBCursorModeXzLabel[8] = "XZ  ";

const char DBCursorModeYzLabel[8] = "YZ  ";

const char DBCursorModeFreeLabel[8] = "FREE";

