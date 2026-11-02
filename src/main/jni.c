#include "common.h"
#include "shared.h"
#include "main/jni.h"
#include "jni.h"
#include "main/data_buffer.h"

static const char *checkClass(void *buffer, const char *name, int target);
static const char *getStrIndex(const char *name, int delimiter);
extern int DataBuffer_getPos(DataBuffer *buffer);
extern void DataBuffer_setPos(DataBuffer *buffer, int offset);
extern int memcmp(const void *a, const void *b, unsigned int size);

const char xenoVmSystemClassName[16] = "xeno/vm/System";

const char xenoUtilFormatClassName[24] = "xeno/util/Format";

const char xenoUtilMenuClassName[16] = "xeno/util/Menu";

const char xenoUtilWindowClassName[24] = "xeno/util/Window";

const char xenoUtilInputClassName[16] = "xeno/util/Input";

const char xenoUtilLayoutClassName[24] = "xeno/util/Layout";

const char xenoUtilRuntimeClassName[24] = "xeno/util/Runtime";

const char xenoUtilToolkitClassName[24] = "xeno/util/Toolkit";

const char xenoUtilTchParamsClassName[24] = "xeno/util/TCHParams";

const char xenoUtilSplineClassName[24] = "xeno/util/Spline";

const char xenoUtilVector4fClassName[24] = "xeno/util/Vector4f";

const char xenoCameraClassName[16] = "xeno/Camera";

const char xenoEffectClassName[16] = "xeno/Effect";

const char xenoLightClassName[16] = "xeno/Light";

const char xenoChrClassName[16] = "xeno/Chr";

const char xenoEnepcClassName[16] = "xeno/Enepc";

const char xenoUnitClassName[16] = "xeno/Unit";

const char xenoUwamonoClassName[16] = "xeno/Uwamono";

const char xenoStageClassName[16] = "xeno/Stage";

const char xenoSceneClassName[16] = "xeno/Scene";

const char xenoPlayControlClassName[24] = "xeno/PlayControl";

const char xenoMovieClassName[24] = "xeno/Movie";

/* Cached native Java class handles populated by JNI_loadNativeClass. */
JavaClass *classJava_xeno_Enepc;

JavaClass *classJava_xeno_Scene;

JavaClass *classJava_xeno_util_Window;

JavaClass *classJava_xeno_Uwamono;

JavaClass *classJava_xeno_Chr;

JavaClass *classJava_xeno_util_Format;

JavaClass *classJava_xeno_Unit;

JavaClass *classJava_xeno_vm_System;

JavaClass *classJava_xeno_util_Vector4f;

JavaClass *classJava_xeno_util_Runtime;

JavaClass *classJava_xeno_util_Layout;

JavaClass *classJava_xeno_Effect;

JavaClass *classJava_xeno_Light;

JavaClass *classJava_xeno_util_Menu;

JavaClass *classJava_xeno_util_Toolkit;

JavaClass *classJava_xeno_Stage;

JavaClass *classJava_xeno_PlayControl;

JavaClass *classJava_xeno_util_TCHParams;

JavaClass *classJava_xeno_util_Spline;

JavaClass *classJava_xeno_util_Input;

JavaClass *classJava_xeno_Camera;

JavaClass *classJava_xeno_Movie;

void JNI_initSystem(xheap_block *heap, int size)
{
    initClassDB();
    xheap_init(0, heap, size);
}

void JNI_loadNativeClass(void)
{
    loadStaticClass(&classJava_xeno_vm_System, xenoVmSystemClassName);
    loadStaticClass(&classJava_xeno_util_Format, xenoUtilFormatClassName);
    loadStaticClass(&classJava_xeno_util_Menu, xenoUtilMenuClassName);
    loadStaticClass(&classJava_xeno_util_Window, xenoUtilWindowClassName);
    loadStaticClass(&classJava_xeno_util_Input, xenoUtilInputClassName);
    loadStaticClass(&classJava_xeno_util_Layout, xenoUtilLayoutClassName);
    loadStaticClass(&classJava_xeno_util_Runtime, xenoUtilRuntimeClassName);
    loadStaticClass(&classJava_xeno_util_Toolkit, xenoUtilToolkitClassName);
    loadStaticClass(&classJava_xeno_util_TCHParams, xenoUtilTchParamsClassName);
    loadStaticClass(&classJava_xeno_util_Spline, xenoUtilSplineClassName);
    loadStaticClass(&classJava_xeno_util_Vector4f, xenoUtilVector4fClassName);
    loadStaticClass(&classJava_xeno_Camera, xenoCameraClassName);
    loadStaticClass(&classJava_xeno_Effect, xenoEffectClassName);
    loadStaticClass(&classJava_xeno_Light, xenoLightClassName);
    loadStaticClass(&classJava_xeno_Chr, xenoChrClassName);
    loadStaticClass(&classJava_xeno_Enepc, xenoEnepcClassName);
    loadStaticClass(&classJava_xeno_Unit, xenoUnitClassName);
    loadStaticClass(&classJava_xeno_Uwamono, xenoUwamonoClassName);
    loadStaticClass(&classJava_xeno_Stage, xenoStageClassName);
    loadStaticClass(&classJava_xeno_Scene, xenoSceneClassName);
    loadStaticClass(&classJava_xeno_PlayControl, xenoPlayControlClassName);
    loadStaticClass(&classJava_xeno_Movie, xenoMovieClassName);
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

int JNI_isInstanceOf(SceneObject object, JavaClass *target_class)
{
    int result = 0;

    if (object != 0) {
        result = instanceOf(((ObjectHeader *)object)->classPointer->clazz,
                            target_class);
    }
    return result;
}

int JNI_loadClassDB(int class_id, int value)
{
    int slot;

    if (class_id < 0) {
        for (slot = 0; slot < VM_CLASS_DB_SLOTS; slot++) {
            if (classDB[slot] == 0) {
                class_id = slot;
                break;
            }
        }
        if (class_id < 0) {
            return 0;
        }
    }

    if (class_id >= VM_CLASS_DB_READ) {
        value = classDB[class_id & VM_CLASS_DB_SLOT_MASK];
    } else {
        classDB[class_id] = value;
    }

    return value;
}

void JNI_callMethod(SceneVm *vm, JavaMethod *method, SceneObject *arguments,
                    int *output)
{
    JThread *thread = vm;
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
    if (flags & JTHREAD_FLAG_STALE_CALL) {
        thread->frame_depth = 0;
        thread->stack_offset = 0;
        thread->flags &= ~(JTHREAD_FLAG_STALE_CALL | JTHREAD_FLAG_WOKEN | JTHREAD_FLAG_WAITING);
        flags = thread->flags;
    }
    if (flags & JTHREAD_FLAG_CALL_RUNNING) {
        JTHREAD_waitFor();
        flags = thread->flags;
        if (flags & JTHREAD_FLAG_CALL_RUNNING)
            return;
    }
    if (flags & JTHREAD_FLAGS_BUSY)
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
    VMFrame *frames;
    VMFrame *stack_end;
    int frames_size = stack_words * sizeof(VMFrame);

    thread = xmalloc(frame_words * 4 + frames_size + sizeof(JThread), 8);
    thread->flags = 0;
    thread->frame_limit = stack_words;
    thread->stack_limit = frame_words;
    thread->kind = kind;
    thread->frame_depth = 0;
    frames = (VMFrame *)(thread + 1);
    thread->frames = frames;
    thread->stack_offset = 0;
    stack_end = frames + stack_words;
    thread->stack = (VMSlot *)stack_end + 6;
    if (jthreadTop == 0) {
        jthreadTop = thread;
        jthreadCurrent = thread;
        thread->previous = 0;
        return thread;
    }
    jthreadCurrent->next = thread;
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
    JThread *thread = vm;

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
        case JVM_CONSTANT_UTF8:
            DataBuffer_seek(buffer, DataBuffer_getUShortAt(buffer));
            break;
        case JVM_CONSTANT_CLASS:
        case JVM_CONSTANT_STRING:
            DataBuffer_getUShortAt(buffer);
            break;
        case JVM_CONSTANT_FIELDREF:
        case JVM_CONSTANT_METHODREF:
        case JVM_CONSTANT_INTERFACE_METHODREF:
        case JVM_CONSTANT_NAME_AND_TYPE:
            DataBuffer_getUShortAt(buffer);
            DataBuffer_getUShortAt(buffer);
            break;
        case JVM_CONSTANT_INTEGER:
        case JVM_CONSTANT_FLOAT:
            DataBuffer_getUIntAt(buffer);
            break;
        case JVM_CONSTANT_LONG:
        case JVM_CONSTANT_DOUBLE:
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

    if (DataBuffer_getUIntAt(data) != JVM_CLASS_MAGIC)
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
        if (DataBuffer_getUByteAt(data) == JVM_CONSTANT_CLASS)
            index = DataBuffer_getUShortAt(data);
        else
            index = 0xFFFF;
        DataBuffer_setPos(data, class_start);
        if (index != 0) {
            skipConstantPool(data, index);
            if (DataBuffer_getUByteAt(data) == JVM_CONSTANT_UTF8) {
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
        if (DataBuffer_getUByteAt(data) == JVM_CONSTANT_CLASS)
            index = DataBuffer_getUShortAt(data);
        else
            index = 0xFFFF;
        DataBuffer_setPos(data, class_start);
        if (index != 0) {
            skipConstantPool(data, index);
            if (DataBuffer_getUByteAt(data) == JVM_CONSTANT_UTF8) {
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
    DataBuffer buffer;
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
                entry = group->entries;
                if (remaining > 0) {
                    do {
                        data = entry->data;
                        length = entry->length;
                        entry++;
                        DataBuffer_init(&buffer, data, length, 1);
                        result = checkClass(&buffer, current_name, target);
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
            entry = group->entries;
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
    return VMRegister[register_index & VM_REGISTER_MASK];
}

/* Defined here, not with the data above: in the original object it follows data that the functions above emit into the same section. */
const char SdvOpcodeNameInstanceofFast[16] = "instanceof_fast";

const char SdvOpcodeNameCheckcastFast[16] = "checkcast_fast";

const char SdvOpcodeNameBreakpoint[16] = "breakpoint";

const char SdvOpcodeNameIfnonnull[16] = "ifnonnull";

const char SdvOpcodeNameMultianewarray[16] = "multianewarray";

const char SdvOpcodeNameMonitorexit[16] = "monitorexit";

const char SdvOpcodeNameMonitorenter[16] = "monitorenter";

const char SdvOpcodeNameInstanceof[16] = "instanceof";

const char SdvOpcodeNameCheckcast[16] = "checkcast";

const char SdvOpcodeNameArraylength[16] = "arraylength";

const char SdvOpcodeNameAnewarray[16] = "anewarray";

const char SdvOpcodeNameNewarray[16] = "newarray";

const char SdvOpcodeNameInvokeinterface[16] = "invokeinterface";

const char SdvOpcodeNameInvokestatic[16] = "invokestatic";

const char SdvOpcodeNameInvokespecial[16] = "invokespecial";

const char SdvOpcodeNameInvokevirtual[16] = "invokevirtual";

const char SdvOpcodeNamePutfield[16] = "putfield";

const char SdvOpcodeNameGetfield[16] = "getfield";

const char SdvOpcodeNamePutstatic[16] = "putstatic";

const char SdvOpcodeNameGetstatic[16] = "getstatic";

const char SdvOpcodeNameLookupswitch[16] = "lookupswitch";

const char SdvOpcodeNameTableswitch[16] = "tableswitch";

const char SdvOpcodeNameIfAcmpne[16] = "if_acmpne";

const char SdvOpcodeNameIfAcmpeq[16] = "if_acmpeq";

const char SdvOpcodeNameIfIcmple[16] = "if_icmple";

const char SdvOpcodeNameIfIcmpgt[16] = "if_icmpgt";

const char SdvOpcodeNameIfIcmpge[16] = "if_icmpge";

const char SdvOpcodeNameIfIcmplt[16] = "if_icmplt";

const char SdvOpcodeNameIfIcmpne[16] = "if_icmpne";

const char SdvOpcodeNameIfIcmpeq[16] = "if_icmpeq";

const char SdvOpcodeNameInt2short[16] = "int2short";

const char SdvOpcodeNameInt2char[16] = "int2char";

const char SdvOpcodeNameInt2byte[16] = "int2byte";

const char SdvOpcodeNameAstore3[16] = "astore_3";

const char SdvOpcodeNameAstore2[16] = "astore_2";

const char SdvOpcodeNameAstore1[16] = "astore_1";

const char SdvOpcodeNameAstore0[16] = "astore_0";

const char SdvOpcodeNameDstore3[16] = "dstore_3";

const char SdvOpcodeNameDstore2[16] = "dstore_2";

const char SdvOpcodeNameDstore1[16] = "dstore_1";

const char SdvOpcodeNameDstore0[16] = "dstore_0";

const char SdvOpcodeNameFstore3[16] = "fstore_3";

const char SdvOpcodeNameFstore2[16] = "fstore_2";

const char SdvOpcodeNameFstore1[16] = "fstore_1";

const char SdvOpcodeNameFstore0[16] = "fstore_0";

const char SdvOpcodeNameLstore3[16] = "lstore_3";

const char SdvOpcodeNameLstore2[16] = "lstore_2";

const char SdvOpcodeNameLstore1[16] = "lstore_1";

const char SdvOpcodeNameLstore0[16] = "lstore_0";

const char SdvOpcodeNameIstore3[16] = "istore_3";

const char SdvOpcodeNameIstore2[16] = "istore_2";

const char SdvOpcodeNameIstore1[16] = "istore_1";

const char SdvOpcodeNameIstore0[16] = "istore_0";

const char SdvOpcodeNameDconst1[16] = "dconst_1";

const char SdvOpcodeNameDconst0[16] = "dconst_0";

const char SdvOpcodeNameFconst2[16] = "fconst_2";

const char SdvOpcodeNameFconst1[16] = "fconst_1";

const char SdvOpcodeNameFconst0[16] = "fconst_0";

const char SdvOpcodeNameLconst1[16] = "lconst_1";

const char SdvOpcodeNameLconst0[16] = "lconst_0";

const char SdvOpcodeNameIconst5[16] = "iconst_5";

const char SdvOpcodeNameIconst4[16] = "iconst_4";

const char SdvOpcodeNameIconst3[16] = "iconst_3";

const char SdvOpcodeNameIconst2[16] = "iconst_2";

const char SdvOpcodeNameIconst1[16] = "iconst_1";

const char SdvOpcodeNameIconst0[16] = "iconst_0";

const char SdvOpcodeNameIconstM1[16] = "iconst_m1";

const char SdvOpcodeNameAconstNull[16] = "aconst_null";
