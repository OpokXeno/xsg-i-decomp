#include "common.h"

#include "shared.h"
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

INCLUDE_ASM("asm/main/nonmatchings/system", Java_xeno_vm_System_sleep__I);

INCLUDE_ASM("asm/main/nonmatchings/system", getStrIndex_002F6210);

INCLUDE_ASM("asm/main/nonmatchings/system", Java_xeno_vm_System_println__Ljava_lang_String_);

INCLUDE_ASM("asm/main/nonmatchings/system", System_waitFor);

/*
 * The script VM's per-thread context, recovered as `JThread` in
 * src/main/chr.h (main's chr TU). Every Java native receives
 * (thread, arguments, result) as the sibling natives elsewhere in this
 * TU map show.
 */
typedef struct JThread JThread;

extern void System_waitFor(JThread *thread, void *wait_target, int wait_kind,
                           int wait_parameter);

void Java_xeno_vm_System_waitFor__Ljava_lang_Object_(JThread *thread, void *arguments,
                                                      unsigned int *result)
{
    void **object = arguments;

    System_waitFor(thread, *object, 0, 0);
}

extern void SCRIPT_methodClearSet(void);

void Java_xeno_vm_System_methodSignal__I(JThread *thread, void *arguments,
                                         unsigned int *result)
{
    int *flag = arguments;

    if (*flag != 0) {
        SCRIPT_methodClearSet();
    }
}
