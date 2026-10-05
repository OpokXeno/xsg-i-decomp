#include "common.h"
#include "shared.h"
#include "toolkit.h"

/* Referenced original data recovered from this translation unit. */
const char unit_field_algorithm[16] = "algorithm";
const char unit_field_id[8] = "id";
const char unit_field_px[8] = "px";
const char unit_field_py[8] = "py";
const char unit_field_pz[8] = "pz";
const char unit_field_rx[8] = "rx";
const char unit_field_ry[8] = "ry";
const char unit_field_rz[8] = "rz";
const char unit_field_peer[8] = "peer";
const char D_004DC118[8] = "light";

/* These field accesses are needed before the later toolkit helper block. */
#define TOOLKIT_JAVA_INT_FIELD(object, field) \
    (*(int *)((char *)(object) + (field)->offset))
#define TOOLKIT_JAVA_FLOAT_FIELD(object, field) \
    (*(float *)((char *)(object) + (field)->offset))
#define TOOLKIT_JAVA_OBJECT_FIELD(object, field) \
    (*(void **)((char *)(object) + (field)->offset))

/* Used by getPeer_Stage for its short-valued Java id field. */
#define JAVA_USHORT_FIELD(object, field) \
    (*(unsigned short *)((char *)(object) + (field)->offset))

INCLUDE_ASM("asm/main/nonmatchings/toolkit", Java_xeno_util_Toolkit_getPeer__Ljava_lang_Object_);

INCLUDE_ASM("asm/main/nonmatchings/toolkit", getPeer_Unit);

void *getPeer_Enepc(void *java_character)
{
    void *character_class;
    JavaField *field;
    ToolkitEnemyPeer *peer;
    JavaThread *thread;
    SceneObject light_object;
    int enemy_id;

    character_class = classJava_xeno_Chr;
    field = lookupClassField(character_class,
                             loadConstString(unit_field_id, -1), 0);
    enemy_id = TOOLKIT_JAVA_INT_FIELD(java_character, field);
    field = lookupClassField(character_class,
                             loadConstString(unit_field_algorithm, -1), 0);
    (void)field;

    peer = ACT_createEnemy(-1, enemy_id);
    ACT_initMotion(peer);
    thread = JNI_createThread(1, 4, 48);
    thread->java_object = java_character;
    peer->java_object = java_character;

    field = lookupClassField(character_class,
                             loadConstString(unit_field_px, -1), 0);
    peer->position_x = TOOLKIT_JAVA_FLOAT_FIELD(java_character, field);
    field = lookupClassField(character_class,
                             loadConstString(unit_field_py, -1), 0);
    peer->position_y = TOOLKIT_JAVA_FLOAT_FIELD(java_character, field);
    field = lookupClassField(character_class,
                             loadConstString(unit_field_pz, -1), 0);
    peer->position_z = TOOLKIT_JAVA_FLOAT_FIELD(java_character, field);
    field = lookupClassField(character_class,
                             loadConstString(unit_field_rx, -1), 0);
    peer->rotation_x = TOOLKIT_JAVA_FLOAT_FIELD(java_character, field);
    field = lookupClassField(character_class,
                             loadConstString(unit_field_ry, -1), 0);
    peer->rotation_y = TOOLKIT_JAVA_FLOAT_FIELD(java_character, field);
    field = lookupClassField(character_class,
                             loadConstString(unit_field_rz, -1), 0);
    peer->rotation_z = TOOLKIT_JAVA_FLOAT_FIELD(java_character, field);
    field = lookupClassField(character_class,
                             loadConstString(unit_field_peer, -1), 0);
    TOOLKIT_JAVA_OBJECT_FIELD(java_character, field) = peer;

    peer->rotation_x = peer->rotation_x / 180.0f * 3.141592741f;
    peer->rotation_y = peer->rotation_y / 180.0f * 3.141592741f;
    peer->rotation_z = peer->rotation_z / 180.0f * 3.141592741f;

    field = lookupClassField(character_class,
                             loadConstString(D_004DC118, -1), 0);
    light_object = newObject(classJava_xeno_Light);
    TOOLKIT_JAVA_OBJECT_FIELD(java_character, field) = light_object;
    field = lookupClassField(classJava_xeno_Light,
                             loadConstString(unit_field_peer, -1), 0);
    TOOLKIT_JAVA_OBJECT_FIELD(light_object, field) = &peer->light_peer;
    return peer;
}

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
    peer->rotation_x = peer->rotation_x / 180.0f * 3.141592741f;
    peer->rotation_y = peer->rotation_y / 180.0f * 3.141592741f;
    peer->rotation_z = peer->rotation_z / 180.0f * 3.141592741f;
    JAVA_OBJECT_FIELD(java_unit, field) = peer;
    return peer;
}

INCLUDE_ASM("asm/main/nonmatchings/toolkit", getPeer_Chr);

static void *getPeer_Effect(void *java_effect)
{
    SceneClass *effect_class;
    JavaField *field;
    ToolkitEffectPeer *peer;
    ToolkitEffectVector position;
    ToolkitEffectVector orientation;
    int effect_id;

    effect_class = classJava_xeno_Effect;
    field = lookupClassField(effect_class,
                             loadConstString(unit_field_id, -1), 0);
    effect_id = JAVA_INT_FIELD(java_effect, field);
    field = lookupClassField(effect_class,
                             loadConstString(unit_field_px, -1), 0);
    sefLoadEffectCf(0, effect_id);
    position.x = JAVA_FLOAT_FIELD(java_effect, field);

    field = lookupClassField(effect_class,
                             loadConstString(unit_field_py, -1), 0);
    position.y = JAVA_FLOAT_FIELD(java_effect, field);
    field = lookupClassField(effect_class,
                             loadConstString(unit_field_pz, -1), 0);
    position.z = JAVA_FLOAT_FIELD(java_effect, field);

    field = lookupClassField(effect_class,
                             loadConstString(unit_field_ry, -1), 0);
    orientation.x = 0.0f;
    orientation.z = 0.0f;
    orientation.y = JAVA_FLOAT_FIELD(java_effect, field);
    peer = sefCreateEffectCf(effect_id, &position, &orientation);

    if (peer == 0) {
        field = lookupClassField(effect_class,
                                 loadConstString(unit_field_peer, -1), 0);
        JAVA_OBJECT_FIELD(java_effect, field) = 0;
        return 0;
    }

    peer->java_object = java_effect;
    field = lookupClassField(effect_class,
                             loadConstString(unit_field_peer, -1), 0);
    JAVA_OBJECT_FIELD(java_effect, field) = peer;
    return peer;
}

static void *getPeer_Stage(void *java_stage)
{
    SceneClass *stage_class;
    JavaField *field;
    ToolkitStagePeer *peer;
    JavaThread *thread;

    stage_class = classJava_xeno_Stage;
    if (!JNI_isInstanceOf(java_stage, stage_class))
        return 0;

    field = lookupClassField(stage_class,
                             loadConstString(unit_field_id, -1), 0);
    peer = STAGE_create(JAVA_USHORT_FIELD(java_stage, field));
    thread = JNI_createThread(2, 8, 48);
    thread->java_object = java_stage;
    peer->java_object = java_stage;

    field = lookupClassField(stage_class,
                             loadConstString(unit_field_peer, -1), 0);
    JAVA_OBJECT_FIELD(java_stage, field) = peer;
    return peer;
}

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

void Java_xeno_util_Toolkit_loadResource__Ljava_lang_Object_Ljava_lang_Object_I(
    JavaThread *thread, ToolkitResourceIndexCall *arguments)
{
    JavaField *field;
    ToolkitChrPeer *character_peer;
    ToolkitUnitResourcePeerView *unit_peer;
    u8 *java_object;
    int resource_value;
    int resource_index;

    java_object = arguments->object;
    resource_value = arguments->id;
    resource_index = arguments->resource_id;

    if (JNI_isInstanceOf(java_object, (SceneClass *)classJava_xeno_Chr) != 0) {
        field = lookupClassField(classJava_xeno_Chr,
                                 loadConstString(unit_field_peer, -1), 0);
        character_peer = JAVA_OBJECT_FIELD(java_object, field);
        if (resource_index < 11)
            character_peer->resource_handles[resource_index] = resource_value;
    } else if (JNI_isInstanceOf(java_object, classJava_xeno_Unit) != 0) {
        field = lookupClassField(classJava_xeno_Unit,
                                 loadConstString(unit_field_peer, -1), 0);
        unit_peer = JAVA_OBJECT_FIELD(java_object, field);
        field = lookupClassField(classJava_xeno_Unit,
                                 loadConstString(unit_field_algorithm, -1), 0);
        (void)field;

        if (resource_index == 0) {
            unit_peer->resource_words[1] = resource_value;
            return;
        }
        --resource_index;
        if (resource_index < 2)
            unit_peer->resource_words[resource_index + 2] = resource_value;
    }
}

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

typedef struct ToolkitPeerGroupCall {
    int group;
    /* The original fetches the complete value word before narrowing it. */
    volatile int value;
} ToolkitPeerGroupCall;

extern unsigned short XTK_peerGroup[4];

void Java_xeno_util_Toolkit_peerSetGroup__II(JThread *thread,
                                              ToolkitPeerGroupCall *arguments)
{
    XTK_peerGroup[arguments->group & 3] = arguments->value;
}
