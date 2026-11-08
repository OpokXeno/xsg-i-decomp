#include "common.h"
#include "shared.h"
#include "format.h"
#include "main/init_vm.h"

/*
 * The script VM's per-thread context (JThread, declared in src/main/chr.h).
 * These natives never read the context.
 */
typedef struct JThread JThread;

/* Byte capacity of the buffer each Format.toString native formats into. */
#define FORMAT_STRING_CAPACITY 255

extern unsigned int strlen(const char *string);

/* Shared temporary String that the toString natives format into and return. */
FormatString *JAVA_tmpString = 0;

const char integerFormat[4] = "%d";

const char floatFormat[8] = "%f";

void Java_xeno_util_Format_floatToIntBits__F(JThread *thread, int *arguments, unsigned int *result)
{
    result[0] = arguments[0];
}

void Java_xeno_util_Format_intBitsToFloat__I(JThread *thread, int *arguments, float *result)
{
    result[0] = (float) arguments[0];
}

/* Empty native: it neither computes nor stores a result. */
void Java_xeno_util_Format_toInt__Ljava_lang_String_(JThread *thread, void *arguments,
                                                      unsigned int *result)
{
}

/* Formats the char argument as a decimal number into a new String. */
void Java_xeno_util_Format_toString__C(JThread *thread, VMSlot *arguments,
                                       VMSlot *result)
{
    FormatString *string;
    FormatByteArray *allocated_array;
    FormatByteArray *stored_array;
    int character;

    string = newObject(classString);
    allocated_array = newArray(classByte, FORMAT_STRING_CAPACITY);
    string->value = allocated_array;
    character = arguments[0].s;
    sprintf((char *)allocated_array->data, integerFormat, character);
    stored_array = string->value;
    stored_array->length = strlen((char *)stored_array->data);
    result[0].ref = string;
}

/* Formats the float argument into the shared temporary String and returns it. */
void Java_xeno_util_Format_toString__F(JThread *thread, float *arguments,
                                       int *result)
{
    FormatByteArray *array;
    FormatString **string_slot;

    if (JAVA_tmpString == 0) {
        JAVA_tmpString = newObject(classString);
        JAVA_tmpString->value = newArray(classByte, FORMAT_STRING_CAPACITY);
    }

    sprintf((char *)JAVA_tmpString->value->data, floatFormat, (double)arguments[0]);
    array = JAVA_tmpString->value;
    array->length = strlen((char *)array->data);
    string_slot = &JAVA_tmpString;
    *result = (int)*string_slot;
}

/* Formats the int argument into the shared temporary String and returns it. */
void Java_xeno_util_Format_toString__I(JThread *thread, int *arguments,
                                       int *result)
{
    unsigned char *end;
    FormatByteArray *source_array;
    FormatByteArray *stored_array;

    if (JAVA_tmpString == 0) {
        JAVA_tmpString = newObject(classString);
        JAVA_tmpString->value = newArray(classByte, FORMAT_STRING_CAPACITY);
    }

    source_array = JAVA_tmpString->value;
    end = STRING_int(source_array->data, arguments[0]);
    *end = 0;
    stored_array = JAVA_tmpString->value;
    stored_array->length = end - stored_array->data;
    *result = (int)JAVA_tmpString;
}

/* Formats the argument as a decimal integer into the shared temporary String and returns it. */
void Java_xeno_util_Format_toString__Z(JThread *thread,
                                       unsigned char *arguments,
                                       int *result)
{
    FormatString *string;
    FormatString **string_slot;
    FormatByteArray *array;

    string = JAVA_tmpString;
    if (string == 0) {
        JAVA_tmpString = newObject(classString);
        JAVA_tmpString->value = newArray(classByte, FORMAT_STRING_CAPACITY);
        string = JAVA_tmpString;
    }

    sprintf((char *)string->value->data, integerFormat, arguments[0]);
    array = JAVA_tmpString->value;
    array->length = strlen((char *)array->data);
    string_slot = &JAVA_tmpString;
    *result = (int)*string_slot;
}
