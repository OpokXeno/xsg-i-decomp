#include "common.h"

INCLUDE_ASM("asm/main/nonmatchings/system", Java_xeno_vm_System_arraycopy__Ljava_lang_Object_ILjava_lang_Object_II);

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
