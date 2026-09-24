#include "common.h"
#include "shared.h"
#include "jni.h"

void JNI_initSystem(xheap_block *heap, int size)
{
    initClassDB();
    xheap_init(0, heap, size);
}

void JNI_loadNativeClass(void)
{
    loadStaticClass(&classJava_xeno_vm_System, D_004CCA70);
    loadStaticClass(&classJava_xeno_util_Format, D_004CCA80);
    loadStaticClass(&classJava_xeno_util_Menu, D_004CCA98);
    loadStaticClass(&classJava_xeno_util_Window, D_004CCAA8);
    loadStaticClass(&classJava_xeno_util_Input, D_004CCAC0);
    loadStaticClass(&classJava_xeno_util_Layout, D_004CCAD0);
    loadStaticClass(&classJava_xeno_util_Runtime, D_004CCAE8);
    loadStaticClass(&classJava_xeno_util_Toolkit, D_004CCB00);
    loadStaticClass(&classJava_xeno_util_TCHParams, D_004CCB18);
    loadStaticClass(&classJava_xeno_util_Spline, D_004CCB30);
    loadStaticClass(&classJava_xeno_util_Vector4f, D_004CCB48);
    loadStaticClass(&classJava_xeno_Camera, D_004CCB60);
    loadStaticClass(&classJava_xeno_Effect, D_004CCB70);
    loadStaticClass(&classJava_xeno_Light, D_004CCB80);
    loadStaticClass(&classJava_xeno_Chr, D_004CCB90);
    loadStaticClass(&classJava_xeno_Enepc, D_004CCBA0);
    loadStaticClass(&classJava_xeno_Unit, D_004CCBB0);
    loadStaticClass(&classJava_xeno_Uwamono, D_004CCBC0);
    loadStaticClass(&classJava_xeno_Stage, D_004CCBD0);
    loadStaticClass(&classJava_xeno_Scene, D_004CCBE0);
    loadStaticClass(&classJava_xeno_PlayControl, D_004CCBF0);
    loadStaticClass(&classJava_xeno_Movie, D_004CCC08);
}

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

int JNI_loadClassDB(int class_id, int value)
{
    int slot;

    if (class_id < 0) {
        for (slot = 0; slot < 8; slot++) {
            if (classDB[slot] == 0) {
                class_id = slot;
                break;
            }
        }
        if (class_id < 0) {
            return 0;
        }
    }

    if (class_id >= 0x100) {
        value = classDB[class_id & 0xFF];
    } else {
        classDB[class_id] = value;
    }

    return value;
}

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

static int checkClass(void *buffer, const char *name, int target);

static const char *getStrIndex(const char *name, int delimiter);

int JNI_searchClasses(int class_id, const char *names, int target, int *skip_count)
{
    PdbClassGroup *group;
    PdbClassEntry *entry;
    void *groups;
    void *data;
    int group_count;
    int group_index;
    int length;
    int remaining;
    int buffer[8];
    int result;
    const char *current_name;

    current_name = names;
    PDB_getEntry(class_id, &groups, &group_count);
    *skip_count = 0;

    for (;;) {
        group_index = 0;
        group = groups;
        if (group_count > 0) {
            do {
                remaining = group->entry_count;
                entry = (PdbClassEntry *)((unsigned char *)group + 8);
                if (remaining > 0) {
                    do {
                        data = entry->data;
                        length = entry->length;
                        entry++;
                        DataBuffer_init((DataBuffer *)buffer, data, length, 1);
                        result = checkClass(buffer, current_name, target);
                        if (result != 0) {
                            return result;
                        }
                        remaining--;
                    } while (remaining > 0);
                }
                group_index++;
                group = (PdbClassGroup *)entry;
            } while (group_index < group_count);
        }
        current_name = getStrIndex(current_name, ':');
        result = 0;
        if (current_name != 0) {
            (*skip_count)++;
            continue;
        }
        break;
    }

    return result;
}

INCLUDE_ASM("asm/main/nonmatchings/jni", JNI_loadClassLibrary);

int JNI_getRegister(int register_index)
{
    return VMRegister[register_index & 0x1f];
}
