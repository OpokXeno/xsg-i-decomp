#include "common.h"
#include "shared.h"
#include "jni.h"

const u8 D_004CCA70[16] = "xeno/vm/System";
const u8 D_004CCA80[24] = "xeno/util/Format";
const u8 D_004CCA98[16] = "xeno/util/Menu";
const u8 D_004CCAA8[24] = "xeno/util/Window";
const u8 D_004CCAC0[16] = "xeno/util/Input";
const u8 D_004CCAD0[24] = "xeno/util/Layout";
const u8 D_004CCAE8[24] = "xeno/util/Runtime";
const u8 D_004CCB00[24] = "xeno/util/Toolkit";
const u8 D_004CCB18[24] = "xeno/util/TCHParams";
const u8 D_004CCB30[24] = "xeno/util/Spline";
const u8 D_004CCB48[24] = "xeno/util/Vector4f";
const u8 D_004CCB60[16] = "xeno/Camera";
const u8 D_004CCB70[16] = "xeno/Effect";
const u8 D_004CCB80[16] = "xeno/Light";
const u8 D_004CCB90[16] = "xeno/Chr";
const u8 D_004CCBA0[16] = "xeno/Enepc";
const u8 D_004CCBB0[16] = "xeno/Unit";
const u8 D_004CCBC0[16] = "xeno/Uwamono";
const u8 D_004CCBD0[16] = "xeno/Stage";
const u8 D_004CCBE0[16] = "xeno/Scene";
const u8 D_004CCBF0[24] = "xeno/PlayControl";
const u8 D_004CCC08[24] = "xeno/Movie";

extern DataBufferByte DataBuffer_getUByteAt(DataBuffer *buffer);
extern unsigned short DataBuffer_getUShortAt(DataBuffer *buffer);
extern DataBufferWord DataBuffer_getUIntAt(DataBuffer *buffer);
extern void DataBuffer_seek(DataBuffer *buffer, int offset);

void JNI_initSystem(xheap_block *heap, int size)
{
    initClassDB();
    xheap_init(0, heap, size);
}

void JNI_loadNativeClass(void)
{
    loadStaticClass(&classJava_xeno_vm_System, (u8 *)D_004CCA70);
    loadStaticClass(&classJava_xeno_util_Format, (u8 *)D_004CCA80);
    loadStaticClass(&classJava_xeno_util_Menu, (u8 *)D_004CCA98);
    loadStaticClass(&classJava_xeno_util_Window, (u8 *)D_004CCAA8);
    loadStaticClass(&classJava_xeno_util_Input, (u8 *)D_004CCAC0);
    loadStaticClass(&classJava_xeno_util_Layout, (u8 *)D_004CCAD0);
    loadStaticClass(&classJava_xeno_util_Runtime, (u8 *)D_004CCAE8);
    loadStaticClass(&classJava_xeno_util_Toolkit, (u8 *)D_004CCB00);
    loadStaticClass(&classJava_xeno_util_TCHParams, (u8 *)D_004CCB18);
    loadStaticClass(&classJava_xeno_util_Spline, (u8 *)D_004CCB30);
    loadStaticClass(&classJava_xeno_util_Vector4f, (u8 *)D_004CCB48);
    loadStaticClass(&classJava_xeno_Camera, (u8 *)D_004CCB60);
    loadStaticClass(&classJava_xeno_Effect, (u8 *)D_004CCB70);
    loadStaticClass(&classJava_xeno_Light, (u8 *)D_004CCB80);
    loadStaticClass(&classJava_xeno_Chr, (u8 *)D_004CCB90);
    loadStaticClass(&classJava_xeno_Enepc, (u8 *)D_004CCBA0);
    loadStaticClass(&classJava_xeno_Unit, (u8 *)D_004CCBB0);
    loadStaticClass(&classJava_xeno_Uwamono, (u8 *)D_004CCBC0);
    loadStaticClass(&classJava_xeno_Stage, (u8 *)D_004CCBD0);
    loadStaticClass(&classJava_xeno_Scene, (u8 *)D_004CCBE0);
    loadStaticClass(&classJava_xeno_PlayControl, (u8 *)D_004CCBF0);
    loadStaticClass(&classJava_xeno_Movie, (u8 *)D_004CCC08);
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

static int skipConstantPool(DataBuffer *buffer, int bound)
{
    unsigned int pool_count;
    unsigned short index;
    int tag;

    pool_count = DataBuffer_getUShortAt(buffer);
    if (bound > 0)
        pool_count = (bound < (int)pool_count) ? (unsigned short)bound : pool_count;

    index = 1;
    tag = 0;
    while (index < pool_count) {
        tag = DataBuffer_getUByteAt(buffer);
        switch (tag) {
        case 1:
            DataBuffer_seek(buffer, DataBuffer_getUShortAt(buffer));
            break;
        case 7:
        case 8:
            DataBuffer_getUShortAt(buffer);
            break;
        case 9:
        case 10:
        case 11:
        case 12:
            DataBuffer_getUShortAt(buffer);
            DataBuffer_getUShortAt(buffer);
            break;
        case 3:
        case 4:
            DataBuffer_getUIntAt(buffer);
            break;
        case 5:
        case 6:
            DataBuffer_getUIntAt(buffer);
            DataBuffer_getUIntAt(buffer);
            index++;
            break;
        }
        index++;
    }
    return tag;
}

INCLUDE_ASM("asm/main/nonmatchings/jni", checkClass);

static const char *getStrIndex(const char *name, int delimiter)
{
    char value;

    while ((value = *name++) != '\0') {
        if (value == delimiter)
            return name;
    }
    return 0;
}

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

void JNI_loadClassLibrary(int class_id)
{
    DataBuffer buffer;
    void *groups;
    int group_count;
    int group_index;
    PdbClassGroup *group;
    PdbClassEntry *entry;
    int entry_count;
    int class_name;
    void *class_slot;

    PDB_getEntry(class_id, &groups, &group_count);
    group_index = 0;
    if (group_count > 0) {
        do {
            group = groups;
            entry_count = group->entry_count;
            entry = (PdbClassEntry *)((u8 *)group + 8);
            if (entry_count > 0) {
                do {
                    DataBuffer_init(&buffer, entry->data, entry->length, 1);
                    class_name = checkClass(&buffer, 0, 0);
                    if (class_name != 0)
                        loadStaticClass(&class_slot, (u8 *)class_name);
                    entry_count--;
                } while (entry_count > 0);
            }
            group_index++;
        } while (group_index < group_count);
    }
}

int JNI_getRegister(int register_index)
{
    return VMRegister[register_index & 0x1f];
}

/* Cached native Java class handles populated by JNI_loadNativeClass. */
void *classJava_xeno_Enepc;
void *classJava_xeno_Scene;
void *classJava_xeno_util_Window;
void *classJava_xeno_Uwamono;
void *classJava_xeno_Chr;
void *classJava_xeno_util_Format;
void *classJava_xeno_Unit;
void *classJava_xeno_vm_System;
void *classJava_xeno_util_Vector4f;
void *classJava_xeno_util_Runtime;
void *classJava_xeno_util_Layout;
void *classJava_xeno_Effect;
void *classJava_xeno_Light;
void *classJava_xeno_util_Menu;
void *classJava_xeno_util_Toolkit;
void *classJava_xeno_Stage;
void *classJava_xeno_PlayControl;
void *classJava_xeno_util_TCHParams;
void *classJava_xeno_util_Spline;
void *classJava_xeno_util_Input;
void *classJava_xeno_Camera;
void *classJava_xeno_Movie;
