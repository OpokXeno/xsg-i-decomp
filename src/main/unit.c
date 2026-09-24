#include "common.h"
#include "shared.h"
#include "unit.h"

/*
 * The Java VM's per-thread execution context, already recovered as
 * `JThread` in src/main/chr.h (main's chr TU). This TU forwards a pointer
 * to it without touching any member, so only the tag is declared here.
 */
typedef struct JThread JThread;

/*
 * UNIT_rotY/UNIT_rotZ/UNIT_sclX/UNIT_sclY/UNIT_sclZ are this TU's own
 * INCLUDE_ASM functions (0x00301d58, 0x003020a8, 0x00302ef8, 0x00303168,
 * 0x003033d8). Their JNI trampolines below insert the interpolation-mode
 * literal as a new first argument and forward `thread`/`arguments`/
 * `failure_result` unchanged -- the same shape as Java_xeno_Chr_sclX__FFZ /
 * CHR_sclX in src/main/chr.c, which names the same three parameters.
 */
static void UNIT_rotY(int mode, JThread *thread, void *arguments,
               u32 *failure_result);
static void UNIT_rotZ(int mode, JThread *thread, void *arguments,
               u32 *failure_result);
static void UNIT_sclX(int mode, JThread *thread, void *arguments,
               u32 *failure_result);
static void UNIT_sclY(int mode, JThread *thread, void *arguments,
               u32 *failure_result);
static void UNIT_sclZ(int mode, JThread *thread, void *arguments,
               u32 *failure_result);

/*
 * UNIT_moveXZ/UNIT_rotX/UNIT_motion are this TU's own INCLUDE_ASM functions
 * (0x00301710, 0x00301a38, 0x00302a10). Their JNI trampolines below are the
 * same shape as UNIT_rotY/UNIT_rotZ/UNIT_sclX/UNIT_sclY/UNIT_sclZ above:
 * insert the mode literal ahead of `thread`/`arguments`/`failure_result` and
 * forward those three unchanged.
 */
static void UNIT_moveXZ(int mode, JThread *thread, void *arguments,
                 u32 *failure_result);
static void UNIT_rotX(int mode, JThread *thread, void *arguments,
               u32 *failure_result);
void UNIT_motion(int mode, JThread *thread, void *arguments,
                 u32 *failure_result);

INCLUDE_ASM("asm/main/nonmatchings/unit", UNIT_setUpdate);

INCLUDE_ASM("asm/main/nonmatchings/unit", Java_xeno_Unit_rotYCNS__Ljava_lang_Object_IFFF);

INCLUDE_ASM("asm/main/nonmatchings/unit", Java_xeno_Unit_transCNS__Ljava_lang_Object_IFFF);

INCLUDE_ASM("asm/main/nonmatchings/unit", UNIT_moveXZ);

INCLUDE_ASM("asm/main/nonmatchings/unit", UNIT_rotX);

INCLUDE_ASM("asm/main/nonmatchings/unit", UNIT_rotY);

INCLUDE_ASM("asm/main/nonmatchings/unit", UNIT_rotZ);

INCLUDE_ASM("asm/main/nonmatchings/unit", Java_xeno_Unit_getRotate__);

/* The "peer" field name every lookupClassField(classJava_xeno_Unit, ...)
 * call below looks up. */
extern const char D_004DC1D0[];

void Java_xeno_Unit_getSignal__(JThread *thread, UnitObjectCall *arguments,
                                u32 *failure_result)
{
    JavaField *peer_field;
    u8 *object;
    UnitPeer *peer;

    object = arguments->object;
    if (JNI_isInstanceOf(object, classJava_xeno_Unit) == 0) {
        *failure_result = 0;
        return;
    }
    peer_field = lookupClassField(classJava_xeno_Unit,
                                  loadConstString(D_004DC1D0, -1), 0);
    peer = *(UnitPeer **)(object + peer_field->offset);
    *failure_result = peer->signal;
}

INCLUDE_ASM("asm/main/nonmatchings/unit", Java_xeno_Unit_getTranslate__);

/*
 * An empty native (`jr $31; nop`): the script VM still calls it with the
 * native signature every Java_xeno_* entry receives, but it reads nothing.
 */
void Java_xeno_Unit_invalidate__(JThread *thread, void *arguments,
                                 u32 *failure_result)
{
}

/*
 * Interpolation mode one (a float value): forward thread/arguments/
 * failure_result unchanged and insert mode=1 ahead of them.
 */
void Java_xeno_Unit_move__FFFZ(JThread *thread, void *arguments,
                               u32 *failure_result)
{
    UNIT_moveXZ(1, thread, arguments, failure_result);
}

/* Interpolation mode zero (an int value), otherwise identical. */
void Java_xeno_Unit_move__IFFZ(JThread *thread, void *arguments,
                               u32 *failure_result)
{
    UNIT_moveXZ(0, thread, arguments, failure_result);
}

INCLUDE_ASM("asm/main/nonmatchings/unit", Java_xeno_Unit_move__Lxeno_util_Spline_IZ);

INCLUDE_ASM("asm/main/nonmatchings/unit", Java_xeno_Unit_rotate__Lxeno_util_Spline_IZ);

/*
 * The Object-argument overloads of move are empty natives: they take the
 * native signature but read none of it.
 */
void Java_xeno_Unit_move__ILjava_lang_Object_Z(JThread *thread,
                                               void *arguments,
                                               u32 *failure_result)
{
}

void Java_xeno_Unit_move__Ljava_lang_Object_FZ(JThread *thread,
                                               void *arguments,
                                               u32 *failure_result)
{
}

INCLUDE_ASM("asm/main/nonmatchings/unit", UNIT_motion);

/*
 * Motion mode zero (the short, two-int overload): forward thread/arguments/
 * failure_result unchanged and insert mode=0 ahead of them.
 */
void Java_xeno_Unit_mtn__IIFZ(JThread *thread, void *arguments,
                              u32 *failure_result)
{
    UNIT_motion(0, thread, arguments, failure_result);
}

/* Motion mode one (the long, five-int overload), otherwise identical. */
void Java_xeno_Unit_mtn__IIIIIFZ(JThread *thread, void *arguments,
                                 u32 *failure_result)
{
    UNIT_motion(1, thread, arguments, failure_result);
}

/*
 * Interpolation mode one (a float value): forward thread/arguments/
 * failure_result unchanged and insert mode=1 ahead of them.
 */
void Java_xeno_Unit_rotX__FFZ(JThread *thread, void *arguments,
                              u32 *failure_result)
{
    UNIT_rotX(1, thread, arguments, failure_result);
}

/* Interpolation mode zero (an int value), otherwise identical. */
void Java_xeno_Unit_rotX__IFZ(JThread *thread, void *arguments,
                              u32 *failure_result)
{
    UNIT_rotX(0, thread, arguments, failure_result);
}

/*
 * The Object-argument overloads of rotX are empty natives: they take the
 * native signature but read none of it.
 */
void Java_xeno_Unit_rotX__ILjava_lang_Object_Z(JThread *thread,
                                               void *arguments,
                                               u32 *failure_result)
{
}

void Java_xeno_Unit_rotX__Ljava_lang_Object_FZ(JThread *thread,
                                               void *arguments,
                                               u32 *failure_result)
{
}

/*
 * Interpolation mode one (a float value): forward thread/arguments/
 * failure_result unchanged and insert mode=1 ahead of them.
 */
void Java_xeno_Unit_rotY__FFZ(JThread *thread, void *arguments,
                              u32 *failure_result)
{
    UNIT_rotY(1, thread, arguments, failure_result);
}

/* Interpolation mode zero (an int value), otherwise identical. */
void Java_xeno_Unit_rotY__IFZ(JThread *thread, void *arguments,
                              u32 *failure_result)
{
    UNIT_rotY(0, thread, arguments, failure_result);
}

/*
 * The Object-argument overloads of rotY and rotZ are empty natives, like
 * rotX's above: they take the native signature but read none of it.
 */
void Java_xeno_Unit_rotY__ILjava_lang_Object_Z(JThread *thread,
                                               void *arguments,
                                               u32 *failure_result)
{
}

void Java_xeno_Unit_rotY__Ljava_lang_Object_FZ(JThread *thread,
                                               void *arguments,
                                               u32 *failure_result)
{
}

void Java_xeno_Unit_rotZ__FFZ(JThread *thread, void *arguments,
                              u32 *failure_result)
{
    UNIT_rotZ(1, thread, arguments, failure_result);
}

void Java_xeno_Unit_rotZ__IFZ(JThread *thread, void *arguments,
                              u32 *failure_result)
{
    UNIT_rotZ(0, thread, arguments, failure_result);
}

void Java_xeno_Unit_rotZ__ILjava_lang_Object_Z(JThread *thread,
                                               void *arguments,
                                               u32 *failure_result)
{
}

void Java_xeno_Unit_rotZ__Ljava_lang_Object_FZ(JThread *thread,
                                               void *arguments,
                                               u32 *failure_result)
{
}

INCLUDE_ASM("asm/main/nonmatchings/unit", Java_xeno_Unit_scale__Lxeno_util_Spline_IZ);

INCLUDE_ASM("asm/main/nonmatchings/unit", UNIT_sclX);

void Java_xeno_Unit_sclX__FFZ(JThread *thread, void *arguments,
                              u32 *failure_result)
{
    UNIT_sclX(1, thread, arguments, failure_result);
}

void Java_xeno_Unit_sclX__IFZ(JThread *thread, void *arguments,
                              u32 *failure_result)
{
    UNIT_sclX(0, thread, arguments, failure_result);
}

INCLUDE_ASM("asm/main/nonmatchings/unit", UNIT_sclY);

void Java_xeno_Unit_sclY__FFZ(JThread *thread, void *arguments,
                              u32 *failure_result)
{
    UNIT_sclY(1, thread, arguments, failure_result);
}

void Java_xeno_Unit_sclY__IFZ(JThread *thread, void *arguments,
                              u32 *failure_result)
{
    UNIT_sclY(0, thread, arguments, failure_result);
}

INCLUDE_ASM("asm/main/nonmatchings/unit", UNIT_sclZ);

void Java_xeno_Unit_sclZ__FFZ(JThread *thread, void *arguments,
                              u32 *failure_result)
{
    UNIT_sclZ(1, thread, arguments, failure_result);
}

void Java_xeno_Unit_sclZ__IFZ(JThread *thread, void *arguments,
                              u32 *failure_result)
{
    UNIT_sclZ(0, thread, arguments, failure_result);
}

/*
 * Unconditional: unlike getSignal__/signal__I above, this native never
 * checks JNI_isInstanceOf before dereferencing the peer field.
 */
void Java_xeno_Unit_setCollision__Z(JThread *thread, UnitBoolCall *arguments)
{
    JavaField *peer_field;
    u8 *object;
    UnitPeer *peer;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Unit,
                                  loadConstString(D_004DC1D0, -1), 0);
    peer = *(UnitPeer **)(object + peer_field->offset);
    if (arguments->flag != 0) {
        peer->flags |= 0x100;
    } else {
        peer->flags &= ~0x100;
    }
}

INCLUDE_ASM("asm/main/nonmatchings/unit", Java_xeno_Unit_setRotate__);

INCLUDE_ASM("asm/main/nonmatchings/unit", Java_xeno_Unit_setTranslate__);

void Java_xeno_Unit_setVisible__IZ(JThread *thread, UnitVisibleCall *arguments)
{
    JavaField *peer_field;
    u8 *object;
    UnitPeer *peer;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Unit,
                                  loadConstString(D_004DC1D0, -1), 0);
    peer = *(UnitPeer **)(object + peer_field->offset);
    MDL_setVisible(peer->model, arguments->mode, arguments->visible);
}

/* Unconditional, like setCollision__Z above. */
void Java_xeno_Unit_setVisible__Z(JThread *thread, UnitBoolCall *arguments)
{
    JavaField *peer_field;
    u8 *object;
    UnitPeer *peer;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Unit,
                                  loadConstString(D_004DC1D0, -1), 0);
    peer = *(UnitPeer **)(object + peer_field->offset);
    if (arguments->flag != 0) {
        peer->flags &= ~4;
    } else {
        peer->flags |= 4;
    }
}

void Java_xeno_Unit_signal__I(JThread *thread, UnitSignalCall *arguments,
                              u32 *failure_result)
{
    JavaField *peer_field;
    u8 *object;
    UnitPeer *peer;

    object = arguments->object;
    if (JNI_isInstanceOf(object, classJava_xeno_Unit) == 0) {
        *failure_result = 0;
        return;
    }
    peer_field = lookupClassField(classJava_xeno_Unit,
                                  loadConstString(D_004DC1D0, -1), 0);
    peer = *(UnitPeer **)(object + peer_field->offset);
    peer->signal = arguments->value;
}

INCLUDE_ASM("asm/main/nonmatchings/unit", Java_xeno_Unit_start__ILjava_lang_Object_);

INCLUDE_ASM("asm/main/nonmatchings/unit", Java_xeno_Unit_stop__);

void Java_xeno_Unit_validate__(void)
{
}

INCLUDE_ASM("asm/main/nonmatchings/unit", Java_xeno_Unit_setParent__Ljava_lang_Object_I);

static void copyArgs(u8 *dst, u8 *src, int count)
{
    u8 byte;

    if (count > 0) {
        count--;
        if (count >= 0) {
            do {
                byte = *src;
                src++;
                count--;
                *dst = byte;
                dst++;
            } while (count >= 0);
        }
    }
}

INCLUDE_ASM("asm/main/nonmatchings/unit", Java_xeno_Unit_setArgs__III);

void Java_xeno_Unit_getArgs__II(JThread *thread, UnitArgsGetCall *arguments,
                                u32 *failure_result)
{
    JavaField *peer_field;
    u8 *object;
    UnitPeer *peer;
    int size;
    int offset;
    unsigned int check;
    int result;

    size = arguments->size;
    object = arguments->object;
    offset = arguments->offset;
    peer_field = lookupClassField(classJava_xeno_Unit,
                                  loadConstString(D_004DC1D0, -1), 0);
    peer = *(UnitPeer **)(object + peer_field->offset);
    check = (unsigned int)(size - 1);
    if (check < 4U) {
        copyArgs((u8 *)&result, peer->args + offset, size);
        *failure_result = (u32)result;
    }
}

/*
 * The Object overload of setArgs: it always writes to the start of the
 * peer's argument buffer, taking `size` bytes from the Object argument's
 * data.
 */
void Java_xeno_Unit_setArgs__ILjava_lang_Object_I(JThread *thread,
                                                  UnitArgsSetCall *arguments)
{
    JavaField *peer_field;
    u8 *object;
    UnitPeer *peer;
    UnitArgsSource *source;
    int size;

    object = arguments->object;
    source = arguments->source;
    size = arguments->size;
    peer_field = lookupClassField(classJava_xeno_Unit,
                                  loadConstString(D_004DC1D0, -1), 0);
    peer = *(UnitPeer **)(object + peer_field->offset);
    if (size > 0) {
        copyArgs(peer->args, source->data, size);
    }
}

INCLUDE_ASM("asm/main/nonmatchings/unit", Java_xeno_Unit_setArgs__IIIII);

INCLUDE_ASM("asm/main/nonmatchings/unit", Java_xeno_Unit_getScale__);

/* Unconditional, like setCollision__Z above. */
void Java_xeno_Unit_setScale__FFF(JThread *thread, UnitVector3Call *arguments)
{
    JavaField *peer_field;
    u8 *object;
    UnitPeer *peer;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Unit,
                                  loadConstString(D_004DC1D0, -1), 0);
    peer = *(UnitPeer **)(object + peer_field->offset);
    peer->scale_x = arguments->x;
    peer->scale_y = arguments->y;
    peer->scale_z = arguments->z;
}

/* Unconditional, like setCollision__Z above. */
void Java_xeno_Unit_getSerial__(JThread *thread, UnitObjectCall *arguments,
                                u32 *failure_result)
{
    JavaField *peer_field;
    u8 *object;
    UnitPeer *peer;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Unit,
                                  loadConstString(D_004DC1D0, -1), 0);
    peer = *(UnitPeer **)(object + peer_field->offset);
    *failure_result = peer->serial;
}

/* Unconditional, like setCollision__Z above. */
void Java_xeno_Unit_mtnSetMask__I(JThread *thread, UnitIntCall *arguments)
{
    JavaField *peer_field;
    u8 *object;
    UnitPeer *peer;
    int mask;

    mask = arguments->value;
    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Unit,
                                  loadConstString(D_004DC1D0, -1), 0);
    peer = *(UnitPeer **)(object + peer_field->offset);
    peer->motion_mask = mask;
}

INCLUDE_ASM("asm/main/nonmatchings/unit", Java_xeno_Unit_mtnGetRoot__ILxeno_util_Vector4f_);

/* Unconditional, like setCollision__Z above. */
void Java_xeno_Unit_getState__(JThread *thread, UnitObjectCall *arguments,
                               u32 *failure_result)
{
    JavaField *peer_field;
    u8 *object;
    UnitPeer *peer;
    UnitSequenceEntry *entry;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Unit,
                                  loadConstString(D_004DC1D0, -1), 0);
    peer = *(UnitPeer **)(object + peer_field->offset);
    entry = &unitSequence[peer->serial];
    *failure_result = entry->state;
}

/* Unconditional, like setCollision__Z above. */
void Java_xeno_Unit_getPivot__Lxeno_util_Vector4f_(JThread *thread,
                                                   UnitPivotOutCall *arguments)
{
    JavaField *peer_field;
    u8 *object;
    UnitPeer *peer;
    UnitSequenceEntry *entry;
    UnitPivotVector *vector;

    vector = arguments->vector;
    object = arguments->object;
    if (vector != 0) {
        peer_field = lookupClassField(classJava_xeno_Unit,
                                      loadConstString(D_004DC1D0, -1), 0);
        peer = *(UnitPeer **)(object + peer_field->offset);
        entry = &unitSequence[peer->serial];
        vector->x = entry->pivot_x;
        vector->y = entry->pivot_y;
        vector->z = entry->pivot_z;
    }
}

/* Unconditional, like setCollision__Z above. */
void Java_xeno_Unit_setPivot__FFF(JThread *thread, UnitVector3Call *arguments)
{
    JavaField *peer_field;
    u8 *object;
    UnitPeer *peer;
    UnitSequenceEntry *entry;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Unit,
                                  loadConstString(D_004DC1D0, -1), 0);
    peer = *(UnitPeer **)(object + peer_field->offset);
    entry = &unitSequence[peer->serial];
    entry->pivot_x = arguments->x;
    entry->pivot_y = arguments->y;
    entry->pivot_z = arguments->z;
}

/* Unconditional peer lookup, but the vector-null check below guards the
 * writes, like getPivot__Lxeno_util_Vector4f_ above. */
void Java_xeno_Unit_getAxis__Lxeno_util_Vector4f_(JThread *thread,
                                                  UnitAxisOutCall *arguments)
{
    JavaField *peer_field;
    u8 *object;
    UnitPeer *peer;
    UnitSequenceEntry *entry;
    UnitAxisVector *vector;

    vector = arguments->vector;
    object = arguments->object;
    if (vector != 0) {
        peer_field = lookupClassField(classJava_xeno_Unit,
                                      loadConstString(D_004DC1D0, -1), 0);
        peer = *(UnitPeer **)(object + peer_field->offset);
        entry = &unitSequence[peer->serial];
        vector->x = entry->axis_x;
        vector->y = entry->axis_y;
        vector->z = entry->axis_z;
        vector->w = 1.0f;
    }
}

/* Unconditional, like setCollision__Z above. */
void Java_xeno_Unit_setAxis__FFFF(JThread *thread, UnitAxisCall *arguments)
{
    JavaField *peer_field;
    u8 *object;
    UnitPeer *peer;
    UnitSequenceEntry *entry;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Unit,
                                  loadConstString(D_004DC1D0, -1), 0);
    peer = *(UnitPeer **)(object + peer_field->offset);
    entry = &unitSequence[peer->serial];
    entry->axis_x = arguments->x;
    entry->axis_y = arguments->y;
    entry->axis_z = arguments->z;
    entry->axis_w = arguments->w;
}

/*
 * Callback shape for MAP_callUnitGroup's per-unit dispatch (src/main/
 * map_create_unit_peer.c, still INCLUDE_ASM): it forwards a pointer to a
 * flags word within the group's unit record. Only that one word's bit 0x10
 * is evidenced, so the record itself stays unmodeled here.
 */
#define UNIT_FLAG_SUSPENDED 0x10

static void unit_suspend(int *flags)
{
    *flags |= UNIT_FLAG_SUSPENDED;
}

static void unit_resume(int *flags)
{
    *flags &= ~UNIT_FLAG_SUSPENDED;
}

/* MAP_callUnitGroup is still asm elsewhere (src/main/map_create_unit_peer.c):
 * it applies `callback` to every unit record of group `group`. */
extern void MAP_callUnitGroup(int group, void (*callback)(int *flags));

void Java_xeno_Unit_suspend__I(JThread *thread, UnitGroupCall *arguments)
{
    MAP_callUnitGroup(arguments->group, unit_suspend);
}

void Java_xeno_Unit_resume__I(JThread *thread, UnitGroupCall *arguments)
{
    MAP_callUnitGroup(arguments->group, unit_resume);
}

/* The "py" field name initElevatorFunc__ looks up: a field on the same
 * Java xeno.Unit object, reused as backing storage for the elevator
 * callback's own state instead of the field's ordinary role. */
extern const char D_004DC1F8[];

/* Unconditional, like setCollision__Z above. */
void Java_xeno_Unit_initElevatorFunc__(JThread *thread,
                                       UnitObjectCall *arguments)
{
    JavaField *peer_field;
    u8 *object;
    JavaField *py_field;
    UnitPeer *peer;
    int py_offset;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Unit,
                                  loadConstString(D_004DC1D0, -1), 0);
    peer = *(UnitPeer **)(object + peer_field->offset);
    py_field = lookupClassField(classJava_xeno_Unit,
                                loadConstString(D_004DC1F8, -1), 0);
    py_offset = py_field->offset;
    peer->elevator_task = tyaElevatorTask;
    peer->elevator_state = 0;
    peer->elevator_field = object + py_offset;
    tyaElevatorTask(peer, py_offset);
}

/* Unconditional, like setCollision__Z above. */
void Java_xeno_Unit_map_shadow__I(JThread *thread, UnitIntCall *arguments)
{
    JavaField *peer_field;
    u8 *object;
    UnitPeer *peer;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Unit,
                                  loadConstString(D_004DC1D0, -1), 0);
    peer = *(UnitPeer **)(object + peer_field->offset);
    if (arguments->value != 0) {
        peer->flags |= 0x20;
    } else {
        peer->flags &= ~0x20;
    }
}

/* Unconditional, like setCollision__Z above. */
void Java_xeno_Unit_renderCommand__I(JThread *thread, UnitIntCall *arguments)
{
    JavaField *peer_field;
    u8 *object;
    UnitPeer *peer;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Unit,
                                  loadConstString(D_004DC1D0, -1), 0);
    peer = *(UnitPeer **)(object + peer_field->offset);
    peer->render_command = arguments->value;
}

/* Unconditional, like setCollision__Z above. */
void Java_xeno_Unit_setFilter__I(JThread *thread, UnitIntCall *arguments,
                                 u32 *failure_result)
{
    JavaField *peer_field;
    u8 *object;
    UnitPeer *peer;
    int value;
    int offset;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Unit,
                                  loadConstString(D_004DC1D0, -1), 0);
    offset = peer_field->offset;
    value = arguments->value;
    peer = *(UnitPeer **)(object + offset);
    peer->filter_mode = 0;
    switch (value) {
    case 2:
        peer->filter_mode = 1;
        *failure_result = 4;
        return;
    case 3:
        peer->filter_mode = 2;
        *failure_result = 4;
        return;
    }
}

INCLUDE_ASM("asm/main/nonmatchings/unit", Java_xeno_Unit_setFilterParam__aF);

/* Unconditional, like setCollision__Z above. */
void Java_xeno_Unit_setShadow__II(JThread *thread, UnitShadowCall *arguments)
{
    JavaField *peer_field;
    UnitPeer *peer;

    peer_field = lookupClassField(classJava_xeno_Unit,
                                  loadConstString(D_004DC1D0, -1), 0);
    peer = *(UnitPeer **)(arguments->object + peer_field->offset);
    peer->shadow_x = arguments->x;
    peer->shadow_y = arguments->y;
}

/* Unconditional, like setCollision__Z above. */
void Java_xeno_Unit_shadow_clip_scale__F(JThread *thread,
                                         UnitFloatCall *arguments)
{
    JavaField *peer_field;
    UnitPeer *peer;

    peer_field = lookupClassField(classJava_xeno_Unit,
                                  loadConstString(D_004DC1D0, -1), 0);
    peer = *(UnitPeer **)(arguments->object + peer_field->offset);
    peer->shadow_clip_scale = arguments->value;
}

INCLUDE_ASM("asm/main/nonmatchings/unit", Java_xeno_Unit_shadow_map_id__I);

/* Unconditional, like setCollision__Z above. */
void Java_xeno_Unit_shadow_map_reset__(JThread *thread,
                                       UnitObjectCall *arguments)
{
    JavaField *peer_field;
    UnitPeer *peer;

    peer_field = lookupClassField(classJava_xeno_Unit,
                                  loadConstString(D_004DC1D0, -1), 0);
    peer = *(UnitPeer **)(arguments->object + peer_field->offset);
    peer->shadow_map_count = 0;
}

/* Unconditional, like setCollision__Z above. */
void Java_xeno_Unit_setSortOffset__F(JThread *thread, UnitFloatCall *arguments)
{
    JavaField *peer_field;
    UnitPeer *peer;

    peer_field = lookupClassField(classJava_xeno_Unit,
                                  loadConstString(D_004DC1D0, -1), 0);
    peer = *(UnitPeer **)(arguments->object + peer_field->offset);
    peer->sort_offset = (int) arguments->value;
}

/* Unconditional, like setCollision__Z above. */
void Java_xeno_Unit_setClip__I(JThread *thread, UnitIntCall *arguments)
{
    JavaField *peer_field;
    u8 *object;
    UnitPeer *peer;
    int value;

    peer_field = lookupClassField(classJava_xeno_Unit,
                                  loadConstString(D_004DC1D0, -1), 0);
    value = arguments->value;
    object = arguments->object;
    peer = *(UnitPeer **)(object + peer_field->offset);
    if (value != 0) {
        peer->flags |= 0x40;
    } else {
        peer->flags &= ~0x40;
    }
}

/* Unconditional, like setCollision__Z above. */
void Java_xeno_Unit_setMonitorPrio__I(JThread *thread, UnitIntCall *arguments)
{
    JavaField *peer_field;
    u8 *object;
    UnitPeer *peer;
    int value;
    int offset;

    peer_field = lookupClassField(classJava_xeno_Unit,
                                  loadConstString(D_004DC1D0, -1), 0);
    value = arguments->value;
    offset = peer_field->offset;
    object = arguments->object;
    peer = *(UnitPeer **)(object + offset);
    if (value != 0) {
        peer->monitor_priority = 1;
        return;
    }
    peer->monitor_priority = 0;
}
