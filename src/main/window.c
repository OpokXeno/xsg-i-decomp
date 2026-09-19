#include "common.h"

/*
 * The script VM's per-thread context, recovered as `JThread` in
 * src/main/chr.h (main's chr TU). Every Java native receives
 * (thread, arguments, result) as the sibling natives elsewhere in this
 * TU map (and in main/tu238, src/main/runtime.c) show.
 */
typedef struct JThread JThread;

INCLUDE_ASM("asm/main/nonmatchings/window", Java_xeno_util_Window_getSignal__);

INCLUDE_ASM("asm/main/nonmatchings/window", Java_xeno_util_Window_signal__I);

INCLUDE_ASM("asm/main/nonmatchings/window", Java_xeno_util_Window_clear__);

INCLUDE_ASM("asm/main/nonmatchings/window", Java_xeno_util_Window_close__);

INCLUDE_ASM("asm/main/nonmatchings/window", Java_xeno_util_Window_waitkey__I);

INCLUDE_ASM("asm/main/nonmatchings/window", Java_xeno_util_Window_wait__I);

INCLUDE_ASM("asm/main/nonmatchings/window", Java_xeno_util_Window_create__I);

INCLUDE_ASM("asm/main/nonmatchings/window", Java_xeno_util_Window_setSize__II);

INCLUDE_ASM("asm/main/nonmatchings/window", Java_xeno_util_Window_setLocation__II);

INCLUDE_ASM("asm/main/nonmatchings/window", Java_xeno_util_Window_print__Ljava_lang_String_);

INCLUDE_ASM("asm/main/nonmatchings/window", Java_xeno_util_Window_print__aLjava_lang_String_I);

void Java_xeno_util_Window_setName__Ljava_lang_String_(JThread *thread, void *arguments,
                                                        unsigned int *result)
{
}

INCLUDE_ASM("asm/main/nonmatchings/window", Java_xeno_util_Window_closeWaitKey__);
