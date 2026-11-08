#include "common.h"
#include "shared.h"
#include "main/jni.h"
#include "main/find_native_method.h"
#include "main/string_utf_get_hash.h"

/*
 * The script VM's per-thread context. Every Java native receives
 * (thread, arguments, result).
 */
typedef struct JThread JThread;

/*
 * Call block of Java_xeno_util_Input_create__I: the controller number the
 * new xeno.util.Input instance is bound to, written straight into the "id"
 * field the getters below read back.
 */
typedef struct InputCreateCall {
    int id; /* +0x00 */
} InputCreateCall;

/*
 * Call block of Java_xeno_util_Input_getButton__/getEdge__/getRepeat__:
 * `this`, the pad wrapper instance whose "id" field selects the PadData
 * entry the button state is read from.
 */
typedef struct InputObjectCall {
    SceneObject object; /* +0x00 */
} InputObjectCall;

/*
 * One controller's pad state, as far as this file reads it: held and pressed
 * are the halfwords at PadData +0x28 and +0x2a; +0x2c (repeat) is read only
 * by Java_xeno_util_Input_getRepeat__.
 *
 * The 0x68-byte stride below is the multiply id * 0x68 that the getters
 * share (strength-reduced by the compiler to shifts and adds).
 *
 * This is a per-controller array view of the PadData symbol, declared
 * locally for this file only.
 */
typedef struct InputPadEntry {
    unsigned char unmodeled_00[0x28];
    u16 held;                       /* +0x28 */
    u16 pressed;                    /* +0x2a */
    u16 repeat;                     /* +0x2c */
    unsigned char unmodeled_2e[0x68 - 0x2e];
} InputPadEntry;

/* The "id" field name every lookupClassField(classJava_xeno_util_Input, ...)
 * call below looks up. */
extern const char inputIdFieldName[];

extern InputPadEntry PadData[];

extern JavaField *lookupClassField(void *class_object, void *name, int flags);
const char inputIdFieldName[] = "id";

/*
 * Input.create(int) builds the xeno/util/Input instance the getters below
 * are called on: it allocates the instance, stores the requested controller
 * number in the class's "id" field and returns the new reference through
 * the call's result slot.
 *
 * The result slot keeps the plain word type it is written as: the id-field
 * store stays ahead of the result store only while both are accesses of
 * one type.
 */
void Java_xeno_util_Input_create__I(JThread *thread, InputCreateCall *arguments,
                                    int *result)
{
    SceneObject object;
    JavaField *id_field;

    object = newObject(classJava_xeno_util_Input);
    id_field = lookupClassField(classJava_xeno_util_Input,
                                loadConstString(inputIdFieldName, -1), 0);
    *(int *)(object + id_field->offset) = arguments->id;
    *result = (int)object;
}

void Java_xeno_util_Input_getButton__(JThread *thread, InputObjectCall *arguments,
                                      unsigned int *result)
{
    SceneObject object;
    JavaField *id_field;
    int id;

    object = arguments->object;
    id_field = lookupClassField(classJava_xeno_util_Input,
                                loadConstString(inputIdFieldName, -1), 0);
    id = *(int *)(object + id_field->offset);
    *result = PadData[id].held;
}

void Java_xeno_util_Input_getEdge__(JThread *thread, InputObjectCall *arguments,
                                    unsigned int *result)
{
    SceneObject object;
    JavaField *id_field;
    int id;

    object = arguments->object;
    id_field = lookupClassField(classJava_xeno_util_Input,
                                loadConstString(inputIdFieldName, -1), 0);
    id = *(int *)(object + id_field->offset);
    *result = PadData[id].pressed;
}

void Java_xeno_util_Input_getRepeat__(JThread *thread, InputObjectCall *arguments,
                                      unsigned int *result)
{
    SceneObject object;
    JavaField *id_field;
    int id;

    object = arguments->object;
    id_field = lookupClassField(classJava_xeno_util_Input,
                                loadConstString(inputIdFieldName, -1), 0);
    id = *(int *)(object + id_field->offset);
    *result = PadData[id].repeat;
}
