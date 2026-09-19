#include "common.h"
#include "shared.h"
#include "toolkit.h"

INCLUDE_ASM("asm/main/nonmatchings/toolkit", Java_xeno_util_Toolkit_getPeer__Ljava_lang_Object_);

INCLUDE_ASM("asm/main/nonmatchings/toolkit", getPeer_Unit);

INCLUDE_ASM("asm/main/nonmatchings/toolkit", getPeer_Enepc);

/*
 * A field of a Java object, reached through the handle `lookupClassField`
 * returned for its name.  The offset is not a member of a C layout: the VM
 * chose it when it laid the class out, and `JavaField.offset` (+0x10 of the
 * handle) is where it reports it, so the readable name of each access is the
 * field-name string the preceding lookup resolved, not a constant.
 */
#define JAVA_INT_FIELD(object, field) \
    (*(int *)((char *)(object) + (field)->offset))
#define JAVA_FLOAT_FIELD(object, field) \
    (*(float *)((char *)(object) + (field)->offset))
#define JAVA_OBJECT_FIELD(object, field) \
    (*(void **)((char *)(object) + (field)->offset))

/*
 * getPeer_Uwamono (main VA 0x002f93f8, 588 bytes, GLOBAL binding).
 * Native peer helper: reads the unit id Java field, creates the Uwamono
 * peer, attaches a JNI thread ward, copies position/rotation Java fields
 * into the peer, normalizes rotations from degrees to radians, and stores
 * the peer back into the Java peer field.
 */
void *getPeer_Uwamono(void *java_unit)
{
    NativeUnitPeer *peer;
    void *unit_class = classJava_xeno_Unit;
    JavaField *field;
    JavaField *algorithm_field;
    JavaThread *thread;
    int unit_id;

    field = lookupClassField(unit_class,
                             loadConstString(unit_field_id, -1), 0);
    unit_id = JAVA_INT_FIELD(java_unit, field);
    /*
     * The algorithm-field lookup is emitted by the original (second
     * jal lookupClassField at 0x2f9458) but its result is never read:
     * the next call reuses a0=-1/a1=unit_id for Unit_CreateUwamono.
     * The call is preserved here for exact call-sequence equality; the
     * value has no data effect.
     */
    algorithm_field = lookupClassField(unit_class,
                                       loadConstString(unit_field_algorithm,
                                                       -1), 0);
    (void)algorithm_field;
    peer = Unit_CreateUwamono(-1, unit_id);
    thread = JNI_createThread(5, 4, 48);
    thread->java_object = java_unit;
    peer->java_object = java_unit;

    field = lookupClassField(unit_class,
                             loadConstString(unit_field_px, -1), 0);
    peer->position_x = JAVA_FLOAT_FIELD(java_unit, field);
    field = lookupClassField(unit_class,
                             loadConstString(unit_field_py, -1), 0);
    peer->position_y = JAVA_FLOAT_FIELD(java_unit, field);
    field = lookupClassField(unit_class,
                             loadConstString(unit_field_pz, -1), 0);
    peer->position_z = JAVA_FLOAT_FIELD(java_unit, field);
    field = lookupClassField(unit_class,
                             loadConstString(unit_field_rx, -1), 0);
    peer->rotation_x = JAVA_FLOAT_FIELD(java_unit, field);
    field = lookupClassField(unit_class,
                             loadConstString(unit_field_ry, -1), 0);
    peer->rotation_y = JAVA_FLOAT_FIELD(java_unit, field);
    field = lookupClassField(unit_class,
                             loadConstString(unit_field_rz, -1), 0);
    peer->rotation_z = JAVA_FLOAT_FIELD(java_unit, field);
    field = lookupClassField(unit_class,
                             loadConstString(unit_field_peer, -1), 0);
    peer->rotation_x = peer->rotation_x / 180.0f * pi;
    peer->rotation_y = peer->rotation_y / 180.0f * pi;
    peer->rotation_z = peer->rotation_z / 180.0f * pi;
    JAVA_OBJECT_FIELD(java_unit, field) = peer;
    return peer;
}

INCLUDE_ASM("asm/main/nonmatchings/toolkit", getPeer_Chr);

INCLUDE_ASM("asm/main/nonmatchings/toolkit", getPeer_Effect);

INCLUDE_ASM("asm/main/nonmatchings/toolkit", getPeer_Stage);

void Java_xeno_util_Toolkit_call__Ljava_lang_Object_Ljava_lang_String_(void)
{
}

INCLUDE_ASM("asm/main/nonmatchings/toolkit", Java_xeno_util_Toolkit_loadResource__Ljava_lang_Object_I);

INCLUDE_ASM("asm/main/nonmatchings/toolkit", Java_xeno_util_Toolkit_loadResource__Ljava_lang_String_);

INCLUDE_ASM("asm/main/nonmatchings/toolkit", Java_xeno_util_Toolkit_loadResource__Ljava_lang_Object_Ljava_lang_Object_I);

INCLUDE_ASM("asm/main/nonmatchings/toolkit", Java_xeno_util_Toolkit_loadResource__Ljava_lang_Object_II);

INCLUDE_ASM("asm/main/nonmatchings/toolkit", Java_xeno_util_Toolkit_peerSetGroup__II);
