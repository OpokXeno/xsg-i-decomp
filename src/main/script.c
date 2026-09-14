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
extern SceneThread *stageVM;
extern SceneThread *evtVM[2];
extern int UseVMFlag;
extern int currentScriptDB;
extern SceneClass *classJava_xeno_Stage;
extern const char call_method_signature_void[4];
extern const char call_method_signature_int[5];
extern const char call_method_signature_int_int[6];
extern const char func_observer_debug_text[16];
extern void funcObserver(ScriptObserverTask *task);
int getEmptyVM(ScriptObserverTask *observer);
extern PadPrefix PadData;

extern unsigned int s_nScriptCfTime;

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

INCLUDE_ASM("asm/main/nonmatchings/script", initJS);

INCLUDE_ASM("asm/main/nonmatchings/script", SCRIPT_test);

INCLUDE_ASM("asm/main/nonmatchings/script", SCRIPT_talkIgnoreSet);

INCLUDE_ASM("asm/main/nonmatchings/script", SCRIPT_talkIgnoreClr);

INCLUDE_ASM("asm/main/nonmatchings/script", SCRIPT_talkIgnoreGet);

INCLUDE_ASM("asm/main/nonmatchings/script", SCRIPT_sceneChangeTimeInit);

INCLUDE_ASM("asm/main/nonmatchings/script", SCRIPT_sceneChangeTimeGet);

INCLUDE_ASM("asm/main/nonmatchings/script", SCRIPT_sceneChangeTimeSet);

INCLUDE_ASM("asm/main/nonmatchings/script", SCRIPT_sceneChangeTimeDec);

INCLUDE_ASM("asm/main/nonmatchings/script", SCRIPT_sendMovieSkipSignal);

INCLUDE_ASM("asm/main/nonmatchings/script", SCRIPT_clrEventActiveFlag);

INCLUDE_ASM("asm/main/nonmatchings/script", SCRIPT_setEventActiveFlag);

INCLUDE_ASM("asm/main/nonmatchings/script", SCRIPT_getEventActiveFlag);

INCLUDE_ASM("asm/main/nonmatchings/script", SCRIPT_eventFinish);

INCLUDE_ASM("asm/main/nonmatchings/script", SCRIPT_methodClearSet);

INCLUDE_ASM("asm/main/nonmatchings/script", SCRIPT_methodClearGet);

INCLUDE_ASM("asm/main/nonmatchings/script", SCRIPT_fade);

INCLUDE_ASM("asm/main/nonmatchings/script", SCRIPT_getFadeTime);

INCLUDE_ASM("asm/main/nonmatchings/script", SCRIPT_fadeGet);

int SCRIPT_getCfTime(void)
{
    return (int)s_nScriptCfTime;
}

void SCRIPT_incCfTime(void)
{
    s_nScriptCfTime += 1u;
}

INCLUDE_ASM("asm/main/nonmatchings/script", SCRIPT_frameLock2Battle);

INCLUDE_ASM("asm/main/nonmatchings/script", SCRIPT_reset);

INCLUDE_ASM("asm/main/nonmatchings/script", SCRIPT_init);

INCLUDE_ASM("asm/main/nonmatchings/script", SCRIPT_execTalkto);

INCLUDE_ASM("asm/main/nonmatchings/script", SCRIPT_execTouchto);

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

INCLUDE_ASM("asm/main/nonmatchings/script", SCRIPT_load);

INCLUDE_ASM("asm/main/nonmatchings/script", SCRIPT_load2);

INCLUDE_ASM("asm/main/nonmatchings/script", SCRIPT_load_DBG);

INCLUDE_ASM("asm/main/nonmatchings/script", getStrIndex_00262160);

INCLUDE_ASM("asm/main/nonmatchings/script", replacePathExt);

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

INCLUDE_ASM("asm/main/nonmatchings/script", XTK_setResourceID);

INCLUDE_ASM("asm/main/nonmatchings/script", XTK_getResourceID);

INCLUDE_ASM("asm/main/nonmatchings/script", XTK_setWindowOwner);

INCLUDE_ASM("asm/main/nonmatchings/script", XTK_getWindowOwner);

INCLUDE_ASM("asm/main/nonmatchings/script", XTK_findFile);
