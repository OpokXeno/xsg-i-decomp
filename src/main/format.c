#include "common.h"
#include "shared.h"
#include "format.h"

FormatString *JAVA_tmpString = 0;

/*
 * The script VM's per-thread context, recovered as `JThread` in
 * src/main/chr.h (main's chr TU). Every Java native receives
 * (thread, arguments, result) as the sibling natives elsewhere in this
 * TU map (and in main/tu238, src/main/runtime.c) show.
 */
typedef struct JThread JThread;

void Java_xeno_util_Format_floatToIntBits__F(JThread *thread, int *arguments, unsigned int *result)
{
    result[0] = arguments[0];
}

void Java_xeno_util_Format_intBitsToFloat__I(JThread *thread, int *arguments, float *result)
{
    result[0] = (float) arguments[0];
}

void Java_xeno_util_Format_toInt__Ljava_lang_String_(JThread *thread, void *arguments,
                                                      unsigned int *result)
{
}

INCLUDE_ASM("asm/main/nonmatchings/format", Java_xeno_util_Format_toString__C);

INCLUDE_ASM("asm/main/nonmatchings/format", Java_xeno_util_Format_toString__F);

void Java_xeno_util_Format_toString__I(JThread *thread, int *arguments,
                                       int *result)
{
    unsigned char *end;
    FormatByteArray *source_array;
    FormatByteArray *stored_array;

    if (JAVA_tmpString == 0) {
        JAVA_tmpString = newObject(classString);
        JAVA_tmpString->value = newArray(classByte, 255);
    }

    source_array = JAVA_tmpString->value;
    end = STRING_int(source_array->data, arguments[0]);
    *end = 0;
    stored_array = JAVA_tmpString->value;
    stored_array->length = end - stored_array->data;
    *result = (int)JAVA_tmpString;
}

INCLUDE_ASM("asm/main/nonmatchings/format", Java_xeno_util_Format_toString__Z);
