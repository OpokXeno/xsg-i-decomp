#include "common.h"
#include "shared.h"
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
extern SceneThread *defaultVM;
extern SceneThread *JNI_createThread(int kind, int stack_words,
                                     int frame_words);
extern void JNI_pushFrame(void);
extern void JNI_loadNativeClass(void);
extern SceneThread *stageVM;
extern SceneThread *evtVM[2];
extern int UseVMFlag;
extern int currentScriptDB;
extern SceneClass *classJava_xeno_Stage;
extern SceneClass *classJava_xeno_Chr;
extern void createTalkTask(void *actor, const char *method_name);
extern const char call_method_signature_void[4];
extern const char call_method_signature_int[5];
extern const char call_method_signature_int_int[6];
/* These TU-local signature literals remain assembler-owned data. Their ELF
 * local-symbol names are required by the CallMethod-family relocations. */
extern const char sig_2[4];
extern const char sig_3[5];
extern const char sig_4[6];
#define call_method_signature_void sig_2
#define call_method_signature_int sig_3
#define call_method_signature_int_int sig_4
extern const char func_observer_debug_text[16];
extern void funcObserver(ScriptObserverTask *task);
int getEmptyVM(ScriptObserverTask *observer);
extern PadPrefix PadData;

extern unsigned int s_nScriptCfTime;
extern unsigned int s_nScriptFadeOutTime;
extern unsigned int s_nScriptFadeRequest;
extern unsigned char s_nScriptChangeTime;
extern unsigned int s_nScriptSequenceReset;
extern unsigned int s_nScriptEventFin;
extern unsigned int s_nScriptEventActive;
extern unsigned int s_nScriptTalkLock;
extern unsigned int s_nScriptFrameLockEntry;
extern int resourceID;
extern int windowOwner;

typedef void (*JSNativeMethod)(void);
extern void JS_init(int state, int class_capacity, int method_capacity);
extern int JS_loadClass(const char *class_name);
extern void JS_classSetup(int class_id, JSNativeMethod get_peer);
extern void JS_classAddMethod(int class_id, const char *name,
                              JSNativeMethod method);
extern const char D_004DA460[];
extern const char D_004C1D50[];
extern const char D_004C1D60[];
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

/* TU-local declarations for the main-00261860 observer/CallMethod allocation
 * (talktoObserver, CallMethod, CallMethod_I, CallMethod_II, funcObserver).
 * New-shared-name proposals are marked below; the integrator reconciles them
 * with config/header-canon.json on promotion.
 */

/* Proposal: the script table indexed by currentScriptDB. Only the two fields
 * this allocation touches are modeled; scriptDB's element stride (0x1C bytes,
 * evidenced by *7*4 in every CallMethod family function) is fixed by that
 * indexing arithmetic, not invented. */
typedef struct ScriptDbEntry {
    u8 unmodeled_00[0xC];
    int active;          /* +0xC: nonzero when this script slot is bound (zero test only) */
    SceneThread *thread; /* +0x10: the script's VM thread; its own object is at +0x10 */
    u8 unmodeled_14[0x1C - 0x14];
} ScriptDbEntry;
extern ScriptDbEntry scriptDB[];

/*
 * TU-local partial view of the engine's actor record (the 0xa70-strided
 * `actor` array at main 0x0043c1e0; other units keep their own scoped views
 * of the same record, e.g. src/main/near_dir.h). SCRIPT_execTalkto and
 * SCRIPT_execTouchto are handed one entry and read only the three members
 * below; every other byte of the record is untouched by this allocation and
 * stays an unmodeled span.
 *
 *   +0x4c0 script_object      the same byte talktoObserver already reaches
 *                             through ACTOR_SCRIPT_OBJECT_OFFSET (this
 *                             allocation's own two call sites name it as a
 *                             member instead, so as not to add more
 *                             offset-cast findings than that one already
 *                             accepted use).
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
extern GameLoopStateAddressView GameLoopState;

/* Proposal: per-function JNI method-signature literals. Each is a duplicate
 * function-local `static const char sig[]`, so the assembler disambiguates
 * with the original ELF's own dup-static suffixing: "sig.2"/"sig.3"/"sig.4". */
                /* "(II)V"  @ 0x004da478, ELF name "sig.4" */

/* PadData (0x00490d90, 0xd0 bytes) read as one doubleword at +0x28, the
 * shared half_28/half_2a pair (original: lui 0x49; ld 3512). */
#define PAD_DATA_DOUBLEWORD_AT_28 (*(u64 *)&PadData.half_28)

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
    GameLoopMovieStatePrefix *game_state = (GameLoopMovieStatePrefix *)GameLoopState;

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

/* Rebuild the default, stage, and two event VMs after a script runtime jump. */
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

/*
 * Queue the actor's talk task when it has a talk method, its owning script
 * is active, no other talk is in progress, the actor is a xeno/Chr instance
 * and the current script's VM thread is running on a Stage object.
 */
void SCRIPT_execTalkto(void *actor)
{
    ScriptActorMethods *methods = (ScriptActorMethods *)actor;
    ScriptDbEntry *entry = &scriptDB[currentScriptDB];

    if (methods->talk_method_name != 0 && entry->active != 0 &&
        s_nScriptTalkLock == 0 &&
        JNI_isInstanceOf(methods->script_object, classJava_xeno_Chr) != 0 &&
        JNI_isInstanceOf(entry->thread->object, classJava_xeno_Stage) != 0) {
        /*
         * The original calls createTalkTask with jal and returns through
         * the shared epilogue instead of a sibling jump. Under this TU's
         * compiler the call stays out of tail position only inside a loop
         * construct, which is the shape a do/while (0) statement macro
         * gives it.
         */
        do {
            createTalkTask(actor, methods->talk_method_name);
        } while (0);
    }
}

/*
 * Queue the actor's touch task under the same gate as SCRIPT_execTalkto,
 * but call the actor's touch method instead of its talk method.
 */
void SCRIPT_execTouchto(void *actor)
{
    ScriptActorMethods *methods = (ScriptActorMethods *)actor;
    ScriptDbEntry *entry = &scriptDB[currentScriptDB];

    if (methods->talk_method_name != 0 && entry->active != 0 &&
        s_nScriptTalkLock == 0 &&
        JNI_isInstanceOf(methods->script_object, classJava_xeno_Chr) != 0 &&
        JNI_isInstanceOf(entry->thread->object, classJava_xeno_Stage) != 0) {
        /*
         * Same shape as SCRIPT_execTalkto: the original keeps a real jal
         * to createTalkTask, which only a loop construct reproduces here.
         */
        do {
            createTalkTask(actor, methods->touch_method_name);
        } while (0);
    }
}

INCLUDE_ASM("asm/main/nonmatchings/script", createTalkTask);

/*
 * Task body queued by createTalkTask: resolve the talk method on the
 * receiver object and run it on the Stage VM, cancelling the talk when the
 * event VMs are busy or the pad state forbids it.
 */
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
    arguments[1] = *(SceneObject *)((SceneByte *)object + ACTOR_SCRIPT_OBJECT_OFFSET);
    if (task->argument2 != 0)
        arguments[2] = (SceneObject)task->argument2;
    JNI_callMethod(stageVM, method, arguments, 0);
    if (stageVM->flags & SCENE_THREAD_CALL_PENDING) {
        XTK_setWindowOwner(0);
        actTalkAfter(object);
        ((GameLoopFlagsPrefix *)GameLoopState)->flags &= ~0x1000;
        ((GameLoopFlagsPrefix *)GameLoopState)->flags &= ~0x8000;
        ((GameLoopFlagsPrefix *)GameLoopState)->flags &= ~0x10000;
        xglTaskRemove(&task->task);
    }
}

INCLUDE_ASM("asm/main/nonmatchings/script", talkCancel);

INCLUDE_ASM("asm/main/nonmatchings/script", SCRIPT_execEntered);

/*
 * Queue a funcObserver task that invokes the no-argument script method
 * `method_name` ("()V") on the current Stage object, then run it once.
 * Every caller ignores $v0, so the entry point returns nothing.
 */
void CallMethod(const char *method_name)
{
    ScriptDbEntry *entry = &scriptDB[currentScriptDB];

    if (entry->active != 0) {
        SceneObject object = entry->thread->object;

        if (JNI_isInstanceOf(object, classJava_xeno_Stage) != 0) {
            XglTaskScheduler *scheduler = GameLoopState[2];
            ScriptObserverTask *task = (ScriptObserverTask *)xglTaskEntryNext(
                scheduler, (int (*)(XglTaskPrefix *))funcObserver,
                scheduler != 0 ? scheduler->active_tail : 0);
            unsigned char started;

            if (task != 0) {
                task->owner = (GameLoopFlagsPrefix *)GameLoopState;
                task->state_flags = 0;
                task->object = 0;
            }
            task->object = object;
            task->method_name = method_name;
            task->method_signature = call_method_signature_void;
            task->unmodeled_39 = 0;
            funcObserver(task);
            /* separate load keeps the funcObserver call out of tail position:
             * without it (form 11) GCC emits a sibling `j funcObserver` */
            started = task->unmodeled_39;
        }
    }
}

/*
 * Queue a funcObserver task that invokes the one-int-argument script method
 * `method_name` ("(I)V") on the current Stage object, then run it once.
 * Every caller ignores $v0, so the entry point returns nothing.
 */
void CallMethod_I(const char *method_name, int arg1)
{
    ScriptDbEntry *entry = &scriptDB[currentScriptDB];

    if (entry->active != 0) {
        SceneObject object = entry->thread->object;

        if (JNI_isInstanceOf(object, classJava_xeno_Stage) != 0) {
            XglTaskScheduler *scheduler = GameLoopState[2];
            ScriptObserverTask *task = (ScriptObserverTask *)xglTaskEntryNext(
                scheduler, (int (*)(XglTaskPrefix *))funcObserver,
                scheduler != 0 ? scheduler->active_tail : 0);
            unsigned char started;

            if (task != 0) {
                task->owner = (GameLoopFlagsPrefix *)GameLoopState;
                task->state_flags = 0;
                task->object = 0;
            }
            task->object = object;
            task->method_name = method_name;
            task->method_signature = call_method_signature_int;
            task->argument1 = arg1;
            task->unmodeled_39 = 0;
            funcObserver(task);
            /* separate load keeps the funcObserver call out of tail position:
             * without it (form 11) GCC emits a sibling `j funcObserver` */
            started = task->unmodeled_39;
        }
    }
}

/*
 * Queue a funcObserver task that invokes the two-int-argument script method
 * `method_name` ("(II)V") on the current Stage object, then run it once.
 * Every caller ignores $v0, so the entry point returns nothing.
 */
void CallMethod_II(const char *method_name, int arg1, int arg2)
{
    ScriptDbEntry *entry = &scriptDB[currentScriptDB];

    if (entry->active != 0) {
        SceneObject object = entry->thread->object;

        if (JNI_isInstanceOf(object, classJava_xeno_Stage) != 0) {
            XglTaskScheduler *scheduler = GameLoopState[2];
            ScriptObserverTask *task = (ScriptObserverTask *)xglTaskEntryNext(
                scheduler, (int (*)(XglTaskPrefix *))funcObserver,
                scheduler != 0 ? scheduler->active_tail : 0);
            unsigned char started;

            if (task != 0) {
                task->owner = (GameLoopFlagsPrefix *)GameLoopState;
                task->state_flags = 0;
                task->object = 0;
            }
            task->object = object;
            task->method_name = method_name;
            task->method_signature = call_method_signature_int_int;
            task->argument1 = arg1;
            task->argument2 = arg2;
            task->unmodeled_39 = 0;
            funcObserver(task);
            /* separate load keeps the funcObserver call out of tail position:
             * without it (form 11) GCC emits a sibling `j funcObserver` */
            started = task->unmodeled_39;
        }
    }
}

/*
 * Task body queued by CallMethod/CallMethod_I/CallMethod_II: claim a free
 * event VM, resolve the named method on the Stage object that queued it and
 * invoke it with up to two int arguments. The task is dropped when the
 * owner flags forbid it, no VM is free, the Stage object changed, the
 * method is missing or the call left an exception pending; after a
 * successful call it stays queued and GameLoopState flag 0x200000 is set.
 */
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
    ((GameLoopFlagsPrefix *)GameLoopState)->flags |= 0x200000;
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

extern int loadScriptCD(ScriptDbEntry *entry, const char *path);

int SCRIPT_load(const char *path)
{
    return loadScriptCD(&scriptDB[(currentScriptDB + 1) & 1], path);
}

extern int loadScriptCD2(ScriptDbEntry *entry, const char *path);

int SCRIPT_load2(const char *path)
{
    return loadScriptCD2(&scriptDB[(currentScriptDB + 1) & 1], path);
}

INCLUDE_ASM("asm/main/nonmatchings/script", SCRIPT_load_DBG);

INCLUDE_ASM("asm/main/nonmatchings/script", getStrIndex_00262160);

extern char *strcpy(char *destination, const char *source);
extern unsigned int strlen(const char *string);
static char *getStrIndex(char *string, unsigned int length, int ch);

/*
 * loadScriptCD/loadScriptCD2/loadScript all capture this call's return
 * value ($v0 into $s0, e.g. build/main/asm/main/nonmatchings/script/
 * loadScriptCD.s "jal replacePathExt" / "daddu $16, $2, $0"): the tail
 * call to strcpy leaves its return (the extension write position) in v0,
 * and the caller reads it, so this returns that pointer instead of void.
 */
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

INCLUDE_ASM("asm/main/nonmatchings/script", XTK_findFile);
