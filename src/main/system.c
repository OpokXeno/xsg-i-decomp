#include "common.h"
#include "shared.h"
#include "system.h"

#define SYSTEM_MARK_SUSPENDED(thread, flagsValue) do { \
    (thread)->flags = (flagsValue); \
} while (0)

#define SYSTEM_COMMIT_WAIT_STATE(thread, targetValue, kind, frameValue, flagsValue) do { \
    (thread)->wait_target = (targetValue); \
    (thread)->wait_kind = (kind); \
    (thread)->resume_frames = (frameValue); \
    SYSTEM_MARK_SUSPENDED(thread, flagsValue); \
} while (0)

struct SystemClassName {
    unsigned char unmodeled_00[8];
    signed char *characters;
};

struct SystemArrayClass {
    unsigned char unmodeled_00[4];
    struct SystemClassName *name;
    unsigned char unmodeled_08[2];
    unsigned short flags;
    unsigned char unmodeled_0c[0x2e];
    unsigned char element_width;
    unsigned char unmodeled_3b;
    struct SystemArrayClass *element_class;
};

struct SystemArrayReference {
    struct SystemArrayClass *klass;
};

struct SystemArray {
    struct SystemArrayReference *reference;
    unsigned char unmodeled_04[4];
    unsigned char *data;
};

struct SystemArrayCopyEndpoint {
    struct SystemArray *array;
    int index;
};

struct SystemArrayCopyArguments {
    struct SystemArrayCopyEndpoint source;
    struct SystemArrayCopyEndpoint destination;
    unsigned int count;
};

extern void *memcpy(void *destination, const void *source, unsigned int size);

/*
 * The script VM's per-thread context, recovered as `JThread` in
 * src/main/chr.h (main's chr TU). Every Java native receives
 * (thread, arguments, result) as the sibling natives elsewhere in this
 * TU map show.
 */
static void System_waitFor(JThread *vmThread, void *wait_target, int wait_kind,
                           int wait_parameter);

extern void SCRIPT_methodClearSet(void);

const char systemFloatFormat[6] = "%8.2f";

static char tmpString[512];

void *Java_xeno_vm_System_arraycopy__Ljava_lang_Object_ILjava_lang_Object_II(
    void *thread, void *argument_block)
{
    struct SystemArrayCopyArguments *arguments = argument_block;
    struct SystemArrayClass *element_class;
    struct SystemArray *destination = arguments->destination.array;
    struct SystemArray *source = arguments->source.array;
    int source_index = arguments->source.index;
    int destination_index = arguments->destination.index;
    unsigned int count = arguments->count;
    unsigned int element_width;

    element_class = destination->reference->klass;
    while (element_class->name->characters[0] == '[') {
        element_class = element_class->element_class;
    }

    element_width = 4;
    if (element_class->flags & 0x100) {
        element_width = element_class->element_width;
    }

    {
        int source_offset = source_index * element_width;
        unsigned int byte_count = count * element_width;
        int destination_offset = destination_index * element_width;
        unsigned char *source_bytes = source->data;
        unsigned char *destination_bytes = destination->data;

        source_bytes = &source_bytes[source_offset];
        destination_bytes = &destination_bytes[destination_offset];
        return memcpy(destination_bytes, source_bytes, byte_count);
    }
}

void Java_xeno_vm_System_sleep__I(SceneVm *vm, const VMSlot *arguments,
                                  VMSlot *result)
{
    int frames;
    struct JThread *thread = vm;

    if ((thread->flags & 4) == 0) {
        u16 suspendedFrame;

        thread->wait_kind = 1;
        suspendedFrame = thread->frame_depth;

        thread->flags |= 4;
        thread->resume_frames = suspendedFrame;
        thread->wait_target.elapsed_frames = 0;
        frames = arguments[0].i;
        if (frames < 0) {
            frames = 0;
        }
        thread->wait_parameter = frames;
    }
}

static const char *getStrIndex(const char *start, int length, int character)
{
    int index = 0;

    if (length > 0) {
        const char *current;
        long value;
        int more;

        do {
            current = &start[index];
            more = ++index < length;
            value = *current;
            if (value == character || value == '\0') {
                break;
            }
        } while (more);
        if (value == character) {
            return current;
        }
    }
    return 0;
}

void Java_xeno_vm_System_println__Ljava_lang_String_(
    struct JThread *thread, SceneString **arguments, unsigned int *result)
{
    const SceneString *string = *arguments;
    const struct SceneStringStorage *storage = string->storage;
    const char *text = storage->bytes;
    int remaining = storage->length;
    const char *end = text + remaining;
    int openingBracket = '[';

    for (;;) {
        const char *slash;
        const char *substitution;
        const char *closing;

        slash = getStrIndex(text, remaining, '/');
        if (slash == 0) {
            return;
        }
        substitution = slash + 2;
        if (slash[1] != openingBracket) {
            continue;
        }
        {
            int segmentLength = slash - text;

            (void)strncpy(tmpString, text, segmentLength);
            remaining -= segmentLength;
            tmpString[segmentLength] = 0;
            closing = getStrIndex(substitution, remaining, ']');
            text = closing;
            if (closing == 0) {
                return;
            }
            segmentLength = closing - slash - 2;

            (void)strncpy(tmpString, substitution, segmentLength);
            if (tmpString[0] == '$') {
                ++text;
            } else {
                if (tmpString[0] == '#') {
                    int registerIndex = PARSE_int(tmpString + 1,
                                                  segmentLength - 1, 10);
                    unsigned int registerBits = JNI_getRegister(registerIndex);
                    float registerValue;
                    char formattedValue[16];

                __builtin_memcpy(&registerValue, &registerBits,
                                 sizeof(registerValue));
                    sprintf(formattedValue, systemFloatFormat,
                            (double)registerValue);
                }
                ++text;
            }
            remaining -= segmentLength;
            if (text < end) {
                continue;
            }
            return;
        }
    }
}

static void System_waitFor(JThread *vmThread, void *wait_target, int wait_kind,
                           int wait_parameter)
{
    struct JThreadNode *thread = (struct JThreadNode *)vmThread;
    SceneClass *waitClass;
    struct JThreadNode *targetThread;

    if (JNI_isInstanceOf(wait_target, classJava_xeno_Chr) == 1) {
        waitClass = classJava_xeno_Chr;
    } else if (JNI_isInstanceOf(wait_target, classJava_xeno_Stage) == 1) {
        waitClass = classJava_xeno_Stage;
    } else if (JNI_isInstanceOf(wait_target, classJava_xeno_util_Menu) == 1) {
        int suspendedKind = 3;
        unsigned int suspendedFlags = thread->flags | 4;
        unsigned short resumeFrames = thread->frame_depth;
        SYSTEM_COMMIT_WAIT_STATE(thread, wait_target, suspendedKind, resumeFrames, suspendedFlags);
        return;
    } else if (JNI_isInstanceOf(wait_target, classJava_xeno_util_Window) == 1) {
        int suspendedKind = 3;
        unsigned int suspendedFlags = thread->flags | 4;
        unsigned short resumeFrames = thread->frame_depth;
        SYSTEM_COMMIT_WAIT_STATE(thread, wait_target, suspendedKind, resumeFrames, suspendedFlags);
        return;
    } else {
        waitClass = 0;
    }

    if (waitClass != 0) {
        targetThread = JTHREAD_get(wait_target);

        switch (wait_kind) {
        case 0:
            {
            unsigned int suspendedFlags;
            unsigned short resumeFrames;
            if (targetThread == thread) {
                break;
            }
            suspendedFlags = thread->flags | 4;
            resumeFrames = thread->frame_depth;
            SYSTEM_COMMIT_WAIT_STATE(thread, targetThread, 2, resumeFrames, suspendedFlags);
            break;
            }
        case 5:
            {
                unsigned int suspendedFlags = thread->flags | 4;
                thread->wait_parameter = wait_parameter;
                thread->wait_kind = wait_kind;
                thread->resume_frames = thread->frame_depth;
                SYSTEM_MARK_SUSPENDED(thread, suspendedFlags);
                thread->wait_target = targetThread;
                break;
            }
        }
    }
}

void Java_xeno_vm_System_waitFor__Ljava_lang_Object_(JThread *thread, void *arguments,
                                                      unsigned int *result)
{
    void **object = arguments;

    System_waitFor(thread, *object, 0, 0);
}

void Java_xeno_vm_System_methodSignal__I(JThread *thread, void *arguments,
                                         unsigned int *result)
{
    int *flag = arguments;

    if (*flag != 0) {
        SCRIPT_methodClearSet();
    }
}
