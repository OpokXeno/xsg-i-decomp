#include "common.h"
#include "shared.h"

/*
 * The script VM's per-thread context, recovered as `JThread` in
 * src/main/chr.h (main's chr TU). Every Java native receives
 * (thread, arguments, result) as the sibling xeno.util natives in
 * src/main/window.c and src/main/runtime.c show.
 */
typedef struct JThread JThread;

extern void *classJava_xeno_util_Input;

extern JavaField *lookupClassField(void *class_object, void *name, int flags);

extern SceneString *loadConstString(const char *bytes, int length);

/* The "id" field name every lookupClassField(classJava_xeno_util_Input, ...)
 * call below looks up. */
extern const char D_004DC0A8[];

/*
 * Call block of Java_xeno_util_Input_create__I: the controller number the
 * new xeno.util.Input instance is bound to, written straight into the "id"
 * field the getters below read back.
 */
typedef struct InputCreateCall {
    int id; /* +0x00 */
} InputCreateCall;

/* newObject is defined in main/tu227 (src/main/find_native_method.c). */
extern SceneObject newObject(void *class_object);

/*
 * Input.create(int) builds the xeno/util/Input instance the getters below
 * are called on: it allocates the instance, stores the requested controller
 * number in the class's "id" field and returns the new reference through
 * the call's result slot.
 *
 * That result slot keeps the plain word it is written as: the id-field
 * store (main 0x002f6ab4) stays ahead of the result store (main 0x002f6ab8)
 * only while both are accesses of one type, the same ordering
 * src/main/window.c relies on for UtilWindow.class_ref.
 */
void Java_xeno_util_Input_create__I(JThread *thread, InputCreateCall *arguments,
                                    int *result)
{
    SceneObject object;
    JavaField *id_field;

    object = newObject(classJava_xeno_util_Input);
    id_field = lookupClassField(classJava_xeno_util_Input,
                                loadConstString(D_004DC0A8, -1), 0);
    *(int *)(object + id_field->offset) = arguments->id;
    *result = (int)object;
}

/*
 * Call block of Java_xeno_util_Input_getButton__/getEdge__/getRepeat__:
 * `this`, the pad wrapper instance whose "id" field selects the PadData
 * entry the button state is read from.
 */
typedef struct InputObjectCall {
    SceneObject object; /* +0x00 */
} InputObjectCall;

/*
 * One controller's pad state, as far as this TU reads it: held/pressed keep
 * the names src/main/game.h's PadDataDebugLayout already established for
 * PadData's +0x28/+0x2a halfwords; +0x2c is read only here (getRepeat).
 *
 * The 0x68-byte stride below every "id"-indexed read is the compiled
 * multiply this TU's getButton__/getEdge__/getRepeat__ share (id * 0x68,
 * strength-reduced to shifts and adds); config/symbols/main.txt gives
 * PadData a 0xd0-byte extent, exactly two entries of that stride.
 *
 * header_divergence (recorded, allowed): a per-controller array view of the
 * same PadData symbol src/main/db_light_write.h, src/main/game_over.h,
 * src/main/party.h (PadPrefix), src/main/game.h (PadDataDebugLayout),
 * src/main/set_path.c (PadDataEffectLayout), src/main/vibration.c
 * (VibrationPadData) and src/main/yajima_test.c (PadDataRawView) each model
 * as their own single-object local view.
 */
typedef struct InputPadEntry {
    unsigned char unmodeled_00[0x28];
    u16 held;                       /* +0x28 */
    u16 pressed;                    /* +0x2a */
    u16 repeat;                     /* +0x2c */
    unsigned char unmodeled_2e[0x68 - 0x2e];
} InputPadEntry;

extern InputPadEntry PadData[];

void Java_xeno_util_Input_getButton__(JThread *thread, InputObjectCall *arguments,
                                      unsigned int *result)
{
    SceneObject object;
    JavaField *id_field;
    int id;

    object = arguments->object;
    id_field = lookupClassField(classJava_xeno_util_Input,
                                loadConstString(D_004DC0A8, -1), 0);
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
                                loadConstString(D_004DC0A8, -1), 0);
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
                                loadConstString(D_004DC0A8, -1), 0);
    id = *(int *)(object + id_field->offset);
    *result = PadData[id].repeat;
}
