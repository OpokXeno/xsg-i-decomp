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

static const char *checkClass(void *buffer, const char *name, int target);

static const char *getStrIndex(const char *name, int delimiter);

/* Cached native Java class handles populated by JNI_loadNativeClass. */

SceneClass *classJava_xeno_Enepc;

SceneClass *classJava_xeno_Scene;

SceneClass *classJava_xeno_util_Window;

SceneClass *classJava_xeno_Uwamono;

SceneClass *classJava_xeno_Chr;

SceneClass *classJava_xeno_util_Format;

SceneClass *classJava_xeno_Unit;

SceneClass *classJava_xeno_vm_System;

SceneClass *classJava_xeno_util_Vector4f;

SceneClass *classJava_xeno_util_Runtime;

SceneClass *classJava_xeno_util_Layout;

SceneClass *classJava_xeno_Effect;

SceneClass *classJava_xeno_Light;

SceneClass *classJava_xeno_util_Menu;

SceneClass *classJava_xeno_util_Toolkit;

SceneClass *classJava_xeno_Stage;

SceneClass *classJava_xeno_PlayControl;

SceneClass *classJava_xeno_util_TCHParams;

SceneClass *classJava_xeno_util_Spline;

SceneClass *classJava_xeno_util_Input;

SceneClass *classJava_xeno_Camera;

SceneClass *classJava_xeno_Movie;

extern int DataBuffer_getPos(DataBuffer *buffer);

extern void DataBuffer_setPos(DataBuffer *buffer, int offset);

extern int memcmp(const void *a, const void *b, unsigned int size);

/* Cached native Java class handles populated by JNI_loadNativeClass. */

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

JThread *JNI_createThread(int kind, int stack_words, int frame_words)
{
    JThread *thread;
    u8 *frames;
    u8 *stack_end;
    int frames_size = stack_words * 24;

    thread = xmalloc(frame_words * 4 + frames_size + sizeof(JThread), 8);
    thread->flags = 0;
    thread->frame_limit = stack_words;
    thread->stack_limit = frame_words;
    thread->kind = kind;
    thread->frame_depth = 0;
    frames = (u8 *)(thread + 1);
    thread->frames = frames;
    thread->stack_offset = 0;
    stack_end = frames + frames_size;
    thread->stack = (u32 *)(stack_end + 24);
    if (jthreadTop == 0) {
        jthreadTop = thread;
        jthreadCurrent = thread;
        thread->previous = 0;
        return thread;
    }
    ((JThread *)jthreadCurrent)->next = thread;
    thread->previous = jthreadCurrent;
    thread->next = 0;
    jthreadCurrent = thread;
    return thread;
}

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

static const char *checkClass(void *buffer, const char *name, int target)
{
    DataBuffer *data = buffer;
    int class_start;
    unsigned int index;
    unsigned int super_class;
    const char *class_name;
    int name_length;

    if (DataBuffer_getUIntAt(data) != 0xCAFEBABE)
        return 0;
    name_length = 0;
    DataBuffer_getUShortAt(data);
    DataBuffer_getUShortAt(data);
    class_start = DataBuffer_getPos(data);
    skipConstantPool(data, 0);
    DataBuffer_getUShortAt(data);
    index = DataBuffer_getUShortAt(data);
    super_class = DataBuffer_getUShortAt(data);

    class_name = 0;
    if (index != 0) {
        DataBuffer_setPos(data, class_start);
        skipConstantPool(data, index);
        if (DataBuffer_getUByteAt(data) == 7)
            index = DataBuffer_getUShortAt(data);
        else
            index = 0xFFFF;
        DataBuffer_setPos(data, class_start);
        if (index != 0) {
            skipConstantPool(data, index);
            if (DataBuffer_getUByteAt(data) == 1) {
                name_length = DataBuffer_getUShortAt(data);
                class_name = (const char *)data->position;
            }
        }
    }

    if (target != 2) {
        if (target == 0)
            return class_name;
        if (memcmp(class_name, name, name_length - 1) != 0)
            class_name = 0;
        return class_name;
    }

    if (super_class != 0) {
        DataBuffer_setPos(data, class_start);
        skipConstantPool(data, super_class);
        if (DataBuffer_getUByteAt(data) == 7)
            index = DataBuffer_getUShortAt(data);
        else
            index = 0xFFFF;
        DataBuffer_setPos(data, class_start);
        if (index != 0) {
            skipConstantPool(data, index);
            if (DataBuffer_getUByteAt(data) == 1) {
                if (memcmp(data->position, name, DataBuffer_getUShortAt(data) - 1) == 0)
                    return class_name;
            }
        }
    }
    return 0;
}

static const char *getStrIndex(const char *name, int delimiter)
{
    char value;

    while ((value = *name++) != '\0') {
        if (value == delimiter)
            return name;
    }
    return 0;
}

const char *JNI_searchClasses(int class_id, const char *names, int target, int *skip_count)
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
    const char *result;
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
    const char *class_name;
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
                        loadStaticClass(&class_slot, class_name);
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
