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

/*
 * Java_xeno_util_Toolkit_loadResource__Ljava_lang_Object_I
 * (main VA 0x002f9c00, 340 bytes, GLOBAL binding).
 * Loads the id-selected resource into the peer of a xeno.Chr or xeno.Unit
 * object: a Chr peer gets ACT_loadResource/ACT_loadMotion (tail call,
 * using the toolkit's current resource id as the motion category); a Unit
 * peer whose "algorithm" field has none of bits 0x100-0x800 set gets
 * MAP_loadUnitResource (tail call); any other case returns that masked
 * algorithm value (0 for an object that is neither).
 */
int Java_xeno_util_Toolkit_loadResource__Ljava_lang_Object_I(
    JThread *thread, ToolkitResourceCall *arguments)
{
    JavaField *field;
    Actor *chr_peer;
    ToolkitUnitPeer *unit_peer;
    u8 *object;
    int id;
    int resource_id;
    int result;

    resource_id = XTK_getResourceID();
    object = arguments->object;
    id = arguments->id;

    if (JNI_isInstanceOf(object, classJava_xeno_Chr)) {
        field = lookupClassField(classJava_xeno_Chr,
                                 loadConstString(unit_field_peer, -1), 0);
        chr_peer = JAVA_OBJECT_FIELD(object, field);
        ACT_loadResource(chr_peer, id);
        return ACT_loadMotion(chr_peer, id, resource_id);
    }

    result = JNI_isInstanceOf(object, classJava_xeno_Unit);
    if (result != 0) {
        field = lookupClassField(classJava_xeno_Unit,
                                 loadConstString(unit_field_peer, -1), 0);
        unit_peer = JAVA_OBJECT_FIELD(object, field);
        field = lookupClassField(classJava_xeno_Unit,
                                 loadConstString(unit_field_algorithm, -1), 0);
        result = JAVA_INT_FIELD(object, field) & 0xF00;
        if (result == 0)
            return MAP_loadUnitResource(unit_peer, id);
    }
    return result;
}

/*
 * Java_xeno_util_Toolkit_loadResource__Ljava_lang_String_
 * (main VA 0x002f9d58, 52 bytes, GLOBAL binding).
 * Reads the byte pointer out of the Java String argument's storage record
 * and writes XTK_findFile's result through the output pointer.
 */
void Java_xeno_util_Toolkit_loadResource__Ljava_lang_String_(
    JThread *thread, ToolkitStringCall *arguments, int *result)
{
    *result = XTK_findFile(arguments->name->storage->bytes);
}

INCLUDE_ASM("asm/main/nonmatchings/toolkit", Java_xeno_util_Toolkit_loadResource__Ljava_lang_Object_Ljava_lang_Object_I);

/*
 * Java_xeno_util_Toolkit_loadResource__Ljava_lang_Object_II
 * (main VA 0x002f9eb0, 348 bytes, GLOBAL binding).
 * Same object-type dispatch as ...Object_I's, with a second int argument:
 * a Chr peer gets ACT_loadResource/ACT_loadMotion (tail call, with the
 * fixed motion category 3; the toolkit's current resource id is still
 * queried but its result is unused, matching the compiled call sequence).
 * A Unit peer whose "algorithm" field has none of bits 0x100-0x800 set
 * gets MAP_loadUnitResource, then RES_loadFile(-1, 2, resource_id +
 * 0x03000000, 0) with its result stored into the peer's +0xDC slot.
 */
int Java_xeno_util_Toolkit_loadResource__Ljava_lang_Object_II(
    JThread *thread, ToolkitResourceIndexCall *arguments)
{
    JavaField *field;
    Actor *chr_peer;
    ToolkitUnitPeer *unit_peer;
    u8 *object;
    int id;
    int resource_id;
    int result;

    XTK_getResourceID();
    object = arguments->object;
    id = arguments->id;
    resource_id = arguments->resource_id;

    if (JNI_isInstanceOf(object, classJava_xeno_Chr)) {
        field = lookupClassField(classJava_xeno_Chr,
                                 loadConstString(unit_field_peer, -1), 0);
        chr_peer = JAVA_OBJECT_FIELD(object, field);
        ACT_loadResource(chr_peer, id);
        return ACT_loadMotion(chr_peer, resource_id, 3);
    }

    result = JNI_isInstanceOf(object, classJava_xeno_Unit);
    if (result != 0) {
        field = lookupClassField(classJava_xeno_Unit,
                                 loadConstString(unit_field_peer, -1), 0);
        unit_peer = JAVA_OBJECT_FIELD(object, field);
        field = lookupClassField(classJava_xeno_Unit,
                                 loadConstString(unit_field_algorithm, -1), 0);
        result = JAVA_INT_FIELD(object, field) & 0xF00;
        if (result == 0) {
            MAP_loadUnitResource(unit_peer, id);
            result = RES_loadFile(-1, 2, resource_id + 0x03000000, 0);
            unit_peer->resource = result;
        }
    }
    return result;
}

INCLUDE_ASM("asm/main/nonmatchings/toolkit", Java_xeno_util_Toolkit_peerSetGroup__II);
