#include "common.h"
#include "shared.h"
#include "jni.h"

void JNI_initSystem(xheap_block *heap, int size)
{
    initClassDB();
    xheap_init(0, heap, size);
}

INCLUDE_ASM("asm/main/nonmatchings/jni", JNI_loadNativeClass);

void JNI_pushFrame(void)
{
    xheap_push();
}

void JNI_popFrame(void)
{
    xheap_pop();
    xheap_current_clear();
}

int JNI_isInstanceOf(SceneObject object, SceneClass *target_class)
{
    int result = 0;

    if (object != 0) {
        result = instanceOf(((SceneObjectHeader *)object)->class_ref->scene_class,
                            target_class);
    }
    return result;
}

INCLUDE_ASM("asm/main/nonmatchings/jni", JNI_loadClassDB);

void JNI_callMethod(SceneVm *vm, SceneMethod *method, SceneObject *arguments,
                    int *output)
{
    JThread *thread = (JThread *)vm;
    u32 flags = thread->flags;

    /*
     * The three flag tests below are proven only by their effect on this
     * function's own control flow, not by any other reader of these bits:
     * bit 5 (0x20) marks a stale call this function resets before
     * dispatching a new one; bit 2 (0x4) means a call is already running
     * on the thread, so this function waits for it (JTHREAD_waitFor) and
     * gives up if it is still running afterwards; bits 0 and 2 together
     * (0x5) are the same "still busy" check taken again just before the
     * call would run.
     */
    if (flags & 0x20) {
        thread->frame_depth = 0;
        thread->stack_offset = 0;
        thread->flags &= ~0x23;
        flags = thread->flags;
    }
    if (flags & 4) {
        JTHREAD_waitFor();
        flags = thread->flags;
        if (flags & 4)
            return;
    }
    if (flags & 5)
        return;
    virtualMachine(vm, method, arguments, output);
}

void JNI_catchException(void)
{
    if (jthreadResetFunc != 0) {
        jthreadResetFunc();
        jthreadResetFunc = 0;
    }
}

INCLUDE_ASM("asm/main/nonmatchings/jni", JNI_createThread);

void JNI_threadException(void)
{
}

void JNI_initThread(SceneVm *vm)
{
    JThread *thread = (JThread *)vm;

    thread->resume_frames = 0;
    thread->flags = 0;
    thread->frame_depth = 0;
    thread->stack_offset = 0;
}

INCLUDE_ASM("asm/main/nonmatchings/jni", skipConstantPool);

INCLUDE_ASM("asm/main/nonmatchings/jni", checkClass);

INCLUDE_ASM("asm/main/nonmatchings/jni", getStrIndex_002F0A58);

INCLUDE_ASM("asm/main/nonmatchings/jni", JNI_searchClasses);

INCLUDE_ASM("asm/main/nonmatchings/jni", JNI_loadClassLibrary);

INCLUDE_ASM("asm/main/nonmatchings/jni", JNI_getRegister);
