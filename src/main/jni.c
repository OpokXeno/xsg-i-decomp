#include "common.h"
#include "shared.h"
#include "jni.h"

extern DataBufferByte DataBuffer_getUByteAt(DataBuffer *buffer);
extern unsigned short DataBuffer_getUShortAt(DataBuffer *buffer);
extern DataBufferWord DataBuffer_getUIntAt(DataBuffer *buffer);
extern void DataBuffer_seek(DataBuffer *buffer, int offset);
static const char *checkClass(void *buffer, const char *name, int target);
static const char *getStrIndex(const char *name, int delimiter);
extern int DataBuffer_getPos(DataBuffer *buffer);
extern void DataBuffer_setPos(DataBuffer *buffer, int offset);
extern int memcmp(const void *a, const void *b, unsigned int size);

const u8 xenoVmSystemClassName[16] = "xeno/vm/System";

const u8 xenoUtilFormatClassName[24] = "xeno/util/Format";

const u8 xenoUtilMenuClassName[16] = "xeno/util/Menu";

const u8 xenoUtilWindowClassName[24] = "xeno/util/Window";

const u8 xenoUtilInputClassName[16] = "xeno/util/Input";

const u8 xenoUtilLayoutClassName[24] = "xeno/util/Layout";

const u8 xenoUtilRuntimeClassName[24] = "xeno/util/Runtime";

const u8 xenoUtilToolkitClassName[24] = "xeno/util/Toolkit";

const u8 xenoUtilTchParamsClassName[24] = "xeno/util/TCHParams";

const u8 xenoUtilSplineClassName[24] = "xeno/util/Spline";

const u8 xenoUtilVector4fClassName[24] = "xeno/util/Vector4f";

const u8 xenoCameraClassName[16] = "xeno/Camera";

const u8 xenoEffectClassName[16] = "xeno/Effect";

const u8 xenoLightClassName[16] = "xeno/Light";

const u8 xenoChrClassName[16] = "xeno/Chr";

const u8 xenoEnepcClassName[16] = "xeno/Enepc";

const u8 xenoUnitClassName[16] = "xeno/Unit";

const u8 xenoUwamonoClassName[16] = "xeno/Uwamono";

const u8 xenoStageClassName[16] = "xeno/Stage";

const u8 xenoSceneClassName[16] = "xeno/Scene";

const u8 xenoPlayControlClassName[24] = "xeno/PlayControl";

const u8 xenoMovieClassName[24] = "xeno/Movie";

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
