#include "common.h"

#include "shared.h"
#include "main/jni.h"

#include "script.h"

extern XglTaskPrefix *xglTaskEntryNext(XglTaskScheduler *scheduler,
                                       int (*callback)(XglTaskPrefix *task),
                                       XglTaskPrefix *entry);

extern int xglTaskRemove(XglTaskPrefix *task);

extern void xglFontDebugPrintf(int x, int y, const char *format, ...);

extern SceneString *loadConstString(const char *bytes, int length);

extern SceneMethod *findMethod(SceneClass *scene_class, SceneString *name,
                               void *signature_or_type);

extern void JNI_callMethod(SceneVm *vm, SceneMethod *method,
                           SceneObject *arguments, int *output);

extern int JNI_isInstanceOf(SceneObject object, SceneClass *target_class);

extern void JNI_initThread(SceneVm *vm);

extern void JNI_catchException(void);

extern int getScriptFlag(SceneObject object);

extern void XTK_setWindowOwner(int owner);

extern void talkCancel(ScriptObserverTask *task);

extern void actTalkAfter(SceneObject object);

extern void initVM(void);

SceneThread *defaultVM;

extern SceneThread *JNI_createThread(int kind, int stack_words,
                                     int frame_words);

extern void JNI_pushFrame(void);

extern void JNI_loadNativeClass(void);

int UseVMFlag;

SceneThread *stageVM;

SceneThread *evtVM[2];

extern void createTalkTask(void *actor, const char *method_name);

const char D_004C1D50[] = "setColor";

const char D_004C1D60[] = "setDirection2";

#define call_method_signature_void sig_2

#define call_method_signature_int sig_3

#define call_method_signature_int_int sig_4

/* EUC-JP text: イベント実行中 ("event in progress"). */

const char func_observer_debug_text[16] =
    "\xa5\xa4\xa5\xd9\xa5\xf3\xa5\xc8\xbc\xc2\xb9\xd4\xc3\xe6";

extern void funcObserver(ScriptObserverTask *task);

int getEmptyVM(ScriptObserverTask *observer);

extern ScriptPadRecord PadData[2];

static unsigned int s_nScriptFadeOutTime = 30;

static unsigned int s_nScriptFadeRequest = 0;

static unsigned char s_nScriptChangeTime = 0;

static unsigned int s_nScriptSequenceReset = 0;

static unsigned int s_nScriptFrameLockEntry = 0;

static unsigned int s_nScriptCfTime = 0;

static unsigned int s_nScriptEventFin = 0;

static unsigned int s_nScriptEventActive = 0;

static unsigned int s_nScriptTalkLock = 0;

extern const char D_004DA460[];
const char D_004DA460[8] = "light";

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
    u8 unmodeled_00[8];
    void *pdb;            /* +0x8: event data used for resource lookup */
    int active;          /* +0xC: nonzero when this script slot is bound (zero test only) */
    SceneThread *thread; /* +0x10: the script's VM thread; its own object is at +0x10 */
    u8 unmodeled_14[0x1C - 0x14];
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

/*
 * Drop the task from the scheduler and leave the task body. The usual
 * do/while (0) statement-macro idiom; its loop notes are part of the
 * original's code generation: written out plainly at the object check, cse
 * follows the equal branch and reads the class through the thread object
 * (s3/s4 swapped, attempt-4d6f277fff4a form 06). The two exits that first
 * release the claimed VM slot spell the removal out: the macro there too
 * moves their blocks (attempt-5684d45333e0 form 04, 85%).
 */

#define REMOVE_TASK_AND_RETURN(task) \
    do {                             \
        xglTaskRemove(&(task)->task); \
        return;                      \
    } while (0)

extern int loadScriptCD(ScriptDbEntry *entry, const char *path);

extern int loadScriptCD2(ScriptDbEntry *entry, const char *path);

extern char *strcpy(char *destination, const char *source);

extern unsigned int strlen(const char *string);

static char *getStrIndex(char *string, unsigned int length, int ch);

extern void TWIN_dispose(TComponent *window);

extern void attrObserver(ScriptObserverTask *task);

static void initJS(int state)
{
    int class_id;

    JS_init(state, 0x40, 8);
    class_id = JS_loadClass(D_004DA460);
    JS_classSetup(class_id, JS_classLight_getPeer);
    JS_classAddMethod(class_id, D_004C1D50, JS_classLight_setColor);
    JS_classAddMethod(class_id, D_004C1D60, JS_classLight_setDirection2);
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
    SceneThread **event_vm;
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

INCLUDE_ASM("asm/main/nonmatchings/script", SCRIPT_init);

void SCRIPT_execTalkto(void *actor)
{
    ScriptActorMethods *methods = (ScriptActorMethods *)actor;
    ScriptDbEntry *entry = &scriptDB[currentScriptDB];

    if (methods->talk_method_name != 0 && entry->active != 0 &&
        s_nScriptTalkLock == 0 &&
        JNI_isInstanceOf(methods->script_object, classJava_xeno_Chr) != 0) {
        do {
            if (!JNI_isInstanceOf(entry->thread->object, classJava_xeno_Stage))
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
            if (!JNI_isInstanceOf(entry->thread->object, classJava_xeno_Stage))
                break;
            createTalkTask(actor, methods->touch_method_name);
        } while (0);
    }
}

INCLUDE_ASM("asm/main/nonmatchings/script", createTalkTask);

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
        SceneObject object = entry->thread->object;

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
        SceneObject object = entry->thread->object;

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
        SceneObject object = entry->thread->object;

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
        SceneObject object = entry->thread->object;

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
    signed char vm_slot;
    SceneObject object;

    xglFontDebugPrintf(8, 24, func_observer_debug_text);

    owner_flags = task->owner->flags;
    if (owner_flags & 0x800)
        REMOVE_TASK_AND_RETURN(task);
    if ((task->state_flags & 1) == 0) {
        if (owner_flags & 0x400)
            REMOVE_TASK_AND_RETURN(task);
        task->state_flags |= 1;
        if (getEmptyVM(task) == 0)
            REMOVE_TASK_AND_RETURN(task);
    }

    entry = &scriptDB[currentScriptDB];
    vm_slot = task->vm_slot;
    if (entry->active == 0)
        return;

    /* The Stage object changed since CallMethod queued the task. */
    object = entry->thread->object;
    if (object != task->object)
        REMOVE_TASK_AND_RETURN(task);

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

INCLUDE_ASM("asm/main/nonmatchings/script", SCRIPT_load_DBG);

INCLUDE_ASM("asm/main/nonmatchings/script", getStrIndex_00262160);

static char *replacePathExt(char *path, const char *ext)
{
    return strcpy(getStrIndex(path, strlen(path), '.'), ext);
}

INCLUDE_ASM("asm/main/nonmatchings/script", attrObserver);

INCLUDE_ASM("asm/main/nonmatchings/script", PauseSkipCheck);

INCLUDE_ASM("asm/main/nonmatchings/script", sceneObserver2);

INCLUDE_ASM("asm/main/nonmatchings/script", sceneObserver);

INCLUDE_ASM("asm/main/nonmatchings/script", DB_light);

INCLUDE_ASM("asm/main/nonmatchings/script", DB_vmSwitch);

INCLUDE_ASM("asm/main/nonmatchings/script", DB_stageSelect);

INCLUDE_ASM("asm/main/nonmatchings/script", DB_memory);

INCLUDE_ASM("asm/main/nonmatchings/script", DB_lightSave);

INCLUDE_ASM("asm/main/nonmatchings/script", DB_scriptFileSelect);

INCLUDE_ASM("asm/main/nonmatchings/script", DB_wind);

INCLUDE_ASM("asm/main/nonmatchings/script", SCRIPT_debugMenu);

INCLUDE_ASM("asm/main/nonmatchings/script", loadScriptCD);

INCLUDE_ASM("asm/main/nonmatchings/script", loadScriptCD2);

INCLUDE_ASM("asm/main/nonmatchings/script", loadScript);

INCLUDE_ASM("asm/main/nonmatchings/script", DB_evtMonitor);

INCLUDE_ASM("asm/main/nonmatchings/script", SCRIPT_exec);

INCLUDE_ASM("asm/main/nonmatchings/script", SCRIPT_exec2);

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
