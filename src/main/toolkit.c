#include "common.h"

#include "shared.h"
#include "main/jni.h"

#include "toolkit.h"

/* Partial views of the map-unit records read by getPeer_Unit. Every named
 * member is accessed in the original function; the intervening storage is
 * explicitly unmodeled. Sequence indexing uses the original 0x260 stride. */
typedef struct ToolkitPeerCall { void *object; } ToolkitPeerCall;
typedef struct ToolkitUnitTransform { float m[4][4]; } ToolkitUnitTransform;
typedef struct ToolkitMapUnitPeer {
    unsigned char unmodeled_00[0x10];
    float position_x, position_y, position_z;
    unsigned char unmodeled_1c[4];
    float rotation_x, rotation_y, rotation_z;
    unsigned char unmodeled_2c[0x40 - 0x2c];
    ToolkitUnitTransform transform;
    unsigned char unmodeled_80[0xa0 - 0x80];
    unsigned char slot_number;
    unsigned char unmodeled_a1[3];
    short serial;
    unsigned char unmodeled_a6[0xd0 - 0xa6];
    void *java_object;
} ToolkitMapUnitPeer;
typedef struct ToolkitUnitSequenceEntry {
    unsigned char unmodeled_00[0x10];
    unsigned short group;
    unsigned char unmodeled_12[0x240 - 0x12];
    float pivot_x, pivot_y, pivot_z;
    unsigned char unmodeled_24c[0x260 - 0x24c];
} ToolkitUnitSequenceEntry;
typedef struct ToolkitMapModel {
    unsigned char unmodeled_00[0x50];
    unsigned int part_transform_offset;
} ToolkitMapModel;
typedef struct ToolkitMapDisplay {
    unsigned char unmodeled_00[4];
    ToolkitMapModel *model;
} ToolkitMapDisplay;
typedef struct ToolkitGameLoopState {
    unsigned char unmodeled_00[0x54];
    ToolkitMapDisplay *map_display;
} ToolkitGameLoopState;
typedef struct ToolkitActorSequenceEntry {
    unsigned char unmodeled_00[0x240];
    float pivot[3];
    unsigned char unmodeled_24c[0x260 - 0x24c];
} ToolkitActorSequenceEntry;

extern ToolkitGameLoopState GameLoopState;
extern ToolkitUnitSequenceEntry unitSequence[];
extern ToolkitActorSequenceEntry actSequence[64];
extern ToolkitMapUnitPeer *MAP_createUnitPeer(int identifier, int unit_id);
extern ToolkitChrPeer *ACT_createChr(int identifier, int character_id);
extern void ACT_updateNPC(ToolkitChrPeer *peer);
static void *getPeer_Unit(void *java_unit);
extern void *getPeer_Enepc(void *java_character);
extern void *getPeer_Chr(void *java_character);
static void *getPeer_Effect(void *java_effect);
static void *getPeer_Stage(void *java_stage);

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

/* Used by getPeer_Stage for its short-valued Java id field. */

#define JAVA_USHORT_FIELD(object, field) \
    (*(unsigned short *)((char *)(object) + (field)->offset))

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

static inline void writeUnitPosition(void *java_unit, void *unit_class,
                                     ToolkitMapUnitPeer *peer)
{
    JavaField *field;

    field = lookupClassField(unit_class,
                             loadConstString(unit_field_px, -1), 0);
    JAVA_FLOAT_FIELD(java_unit, field) = peer->position_x;
    field = lookupClassField(unit_class,
                             loadConstString(unit_field_py, -1), 0);
    JAVA_FLOAT_FIELD(java_unit, field) = peer->position_y;
    field = lookupClassField(unit_class,
                             loadConstString(unit_field_pz, -1), 0);
    JAVA_FLOAT_FIELD(java_unit, field) = peer->position_z;
}

typedef struct ToolkitPeerGroupCall {
    int group;
    /* The operand stack transports the Java int as one raw 32-bit word. */
    unsigned int valueBits;
} ToolkitPeerGroupCall;

extern unsigned short XTK_peerGroup[4];

void Java_xeno_util_Toolkit_getPeer__Ljava_lang_Object_(
    JThread *thread, ToolkitPeerCall *arguments, void **result)
{
    void *object;

    object = arguments->object;
    if (JNI_isInstanceOf(object, classJava_xeno_Chr) == 1) {
        if (JNI_isInstanceOf(object, classJava_xeno_Enepc) == 1)
            *result = getPeer_Enepc(object);
        else
            *result = getPeer_Chr(object);
    } else if (JNI_isInstanceOf(object, classJava_xeno_Unit) == 1) {
        if (JNI_isInstanceOf(object, classJava_xeno_Uwamono) == 1)
            *result = getPeer_Uwamono(object);
        else
            *result = getPeer_Unit(object);
    } else if (JNI_isInstanceOf(object, classJava_xeno_Effect) == 1) {
        *result = getPeer_Effect(object);
    } else if (JNI_isInstanceOf(object, classJava_xeno_Stage) == 1) {
        *result = getPeer_Stage(object);
    } else {
        *result = 0;
    }
}

static void *getPeer_Unit(void *java_unit)
{
    void *unit_class;
    JavaField *field;
    ToolkitMapUnitPeer *peer;
    ToolkitUnitSequenceEntry *sequence;
    JavaThread *thread;
    int unit_id;
    int algorithm;

    unit_class = classJava_xeno_Unit;
    field = lookupClassField(unit_class,
                             loadConstString(unit_field_id, -1), 0);
    unit_id = JAVA_INT_FIELD(java_unit, field);
    field = lookupClassField(unit_class,
                             loadConstString(unit_field_algorithm, -1), 0);
    algorithm = JAVA_INT_FIELD(java_unit, field);

    peer = MAP_createUnitPeer(-1, unit_id);
    sequence = &unitSequence[peer->slot_number];
    sequence->group = XTK_peerGroup[1];
    thread = JNI_createThread(5, 4, 48);
    thread->java_object = java_unit;
    peer->java_object = java_unit;

    switch (algorithm & 0xF00) {
    case 0x300: {
        ToolkitMapDisplay *display;
        ToolkitMapModel *model;
        ToolkitUnitTransform *table;
        ToolkitUnitTransform *source;
        ToolkitUnitTransform *transform;

        display = GameLoopState.map_display;
        if (display != 0) {
            model = display->model;
            if (model != 0) {
                table = (ToolkitUnitTransform *)
                    ((unsigned char *)model + model->part_transform_offset);
                source = &table[peer->serial];
                transform = &peer->transform;
                __asm__ __volatile__("lq $2, 0(%1)\n"
                                     "sq $2, 0(%0)\n"
                                     "lq $2, 16(%1)\n"
                                     "sq $2, 16(%0)\n"
                                     "lq $2, 32(%1)\n"
                                     "sq $2, 32(%0)\n"
                                     "lq $2, 48(%1)\n"
                                     "sq $2, 48(%0)\n"
                                     : : "r"(transform), "r"(source)
                                     : "$2", "memory");
                sequence->pivot_x = peer->transform.m[3][0];
                sequence->pivot_y = peer->transform.m[3][1];
                sequence->pivot_z = peer->transform.m[3][2];
            }
        }
        writeUnitPosition(java_unit, unit_class, peer);
        break;
    }
    case 0x100: {
        ToolkitMapDisplay *display;
        ToolkitMapModel *model;
        ToolkitUnitTransform *table;

        display = GameLoopState.map_display;
        if (display != 0) {
            model = display->model;
            if (model != 0) {
                table = (ToolkitUnitTransform *)
                    ((unsigned char *)model + model->part_transform_offset);
                peer->position_x = table[peer->serial].m[3][0];
                peer->position_y = table[peer->serial].m[3][1];
                peer->position_z = table[peer->serial].m[3][2];
            }
        }
        writeUnitPosition(java_unit, unit_class, peer);
        break;
    }
    case 0:
    default: {
        field = lookupClassField(unit_class,
                                 loadConstString(unit_field_px, -1), 0);
        peer->position_x = JAVA_FLOAT_FIELD(java_unit, field);
        field = lookupClassField(unit_class,
                                 loadConstString(unit_field_py, -1), 0);
        peer->position_y = JAVA_FLOAT_FIELD(java_unit, field);
        field = lookupClassField(unit_class,
                                 loadConstString(unit_field_pz, -1), 0);
        peer->position_z = JAVA_FLOAT_FIELD(java_unit, field);
        break;
    }
    }

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
    JAVA_OBJECT_FIELD(java_unit, field) = peer;

    peer->rotation_x = peer->rotation_x / 180.0f * 3.1415927f;
    peer->rotation_y = peer->rotation_y / 180.0f * 3.1415927f;
    peer->rotation_z = peer->rotation_z / 180.0f * 3.1415927f;
    return peer;
}

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
    enemy_id = JAVA_INT_FIELD(java_character, field);
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
    peer->position_x = JAVA_FLOAT_FIELD(java_character, field);
    field = lookupClassField(character_class,
                             loadConstString(unit_field_py, -1), 0);
    peer->position_y = JAVA_FLOAT_FIELD(java_character, field);
    field = lookupClassField(character_class,
                             loadConstString(unit_field_pz, -1), 0);
    peer->position_z = JAVA_FLOAT_FIELD(java_character, field);
    field = lookupClassField(character_class,
                             loadConstString(unit_field_rx, -1), 0);
    peer->rotation_x = JAVA_FLOAT_FIELD(java_character, field);
    field = lookupClassField(character_class,
                             loadConstString(unit_field_ry, -1), 0);
    peer->rotation_y = JAVA_FLOAT_FIELD(java_character, field);
    field = lookupClassField(character_class,
                             loadConstString(unit_field_rz, -1), 0);
    peer->rotation_z = JAVA_FLOAT_FIELD(java_character, field);
    field = lookupClassField(character_class,
                             loadConstString(unit_field_peer, -1), 0);
    JAVA_OBJECT_FIELD(java_character, field) = peer;

    peer->rotation_x = peer->rotation_x / 180.0f * 3.1415927f;
    peer->rotation_y = peer->rotation_y / 180.0f * 3.1415927f;
    peer->rotation_z = peer->rotation_z / 180.0f * 3.1415927f;

    field = lookupClassField(character_class,
                             loadConstString(D_004DC118, -1), 0);
    light_object = newObject(classJava_xeno_Light);
    JAVA_OBJECT_FIELD(java_character, field) = light_object;
    field = lookupClassField(classJava_xeno_Light,
                             loadConstString(unit_field_peer, -1), 0);
    JAVA_OBJECT_FIELD(light_object, field) = &peer->light_peer;
    return peer;
}

/* Both native peer constructors import the Java position and Euler rotation. */
#define TOOLKIT_IMPORT_TRANSFORM(class_object, java_object, px, py, pz, rx, ry, rz) { \
    JavaField *component_field; \
    component_field = lookupClassField((class_object), \
        loadConstString(unit_field_px, -1), 0); \
    (px) = JAVA_FLOAT_FIELD((java_object), component_field); \
    component_field = lookupClassField((class_object), \
        loadConstString(unit_field_py, -1), 0); \
    (py) = JAVA_FLOAT_FIELD((java_object), component_field); \
    component_field = lookupClassField((class_object), \
        loadConstString(unit_field_pz, -1), 0); \
    (pz) = JAVA_FLOAT_FIELD((java_object), component_field); \
    component_field = lookupClassField((class_object), \
        loadConstString(unit_field_rx, -1), 0); \
    (rx) = JAVA_FLOAT_FIELD((java_object), component_field); \
    component_field = lookupClassField((class_object), \
        loadConstString(unit_field_ry, -1), 0); \
    (ry) = JAVA_FLOAT_FIELD((java_object), component_field); \
    component_field = lookupClassField((class_object), \
        loadConstString(unit_field_rz, -1), 0); \
    (rz) = JAVA_FLOAT_FIELD((java_object), component_field); \
}

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

void *getPeer_Chr(void *java_character)
{
    void *character_class;
    JavaField *field;
    ToolkitChrPeer *peer;
    JavaThread *thread;
    SceneObject light_object;
    int character_id;
    int algorithm;
    ToolkitActorSequenceEntry *sequence;
    /* The original keeps the 180.0 divisor in $f4. */
    register float half_turn;
    float rotation_x;

    character_class = classJava_xeno_Chr;
    field = lookupClassField(character_class,
                             loadConstString(unit_field_id, -1), 0);
    character_id = JAVA_INT_FIELD(java_character, field);
    field = lookupClassField(character_class,
                             loadConstString(unit_field_algorithm, -1), 0);
    algorithm = JAVA_INT_FIELD(java_character, field);

    peer = ACT_createChr(-1, character_id);
    ACT_initMotion(peer);
    thread = JNI_createThread(1, 4, 48);
    thread->java_object = java_character;
    peer->java_object = java_character;

    TOOLKIT_IMPORT_TRANSFORM(character_class, java_character,
        peer->position.x, peer->position.y, peer->position.z,
        peer->rotation.x, peer->rotation.y, peer->rotation.z);
    field = lookupClassField(character_class,
                             loadConstString(unit_field_peer, -1), 0);
    JAVA_OBJECT_FIELD(java_character, field) = peer;

    rotation_x = peer->rotation.x;
    half_turn = 180.0f;
    peer->rotation.x = rotation_x / half_turn * 3.1415927f;
    peer->rotation.y = peer->rotation.y / half_turn * 3.1415927f;
    peer->rotation.z = peer->rotation.z / half_turn * 3.1415927f;

    if ((algorithm & 0xF00) != 0) {
        peer->update = ACT_updateNPC;
        sequence = &actSequence[peer->slot_number];
        sequence->pivot[0] = peer->position.x;
        sequence->pivot[1] = peer->position.y;
        sequence->pivot[2] = peer->position.z;
    }

    field = lookupClassField(character_class,
                             loadConstString(D_004DC118, -1), 0);
    light_object = newObject(classJava_xeno_Light);
    JAVA_OBJECT_FIELD(java_character, field) = light_object;
    field = lookupClassField(classJava_xeno_Light,
                             loadConstString(unit_field_peer, -1), 0);
    JAVA_OBJECT_FIELD(light_object, field) = &peer->light_peer;
    return peer;
}

/* The CF helper copies one complete 16-byte record. Its x/y/z prefix is
 * modeled by the imported interface; the original leaves the fourth word
 * unspecified, so it remains raw representation rather than a float value. */
typedef struct ToolkitEffectVectorStorage {
    ToolkitEffectVector xyz;
    unsigned char unmodeled_0c[4];
} ToolkitEffectVectorStorage;

static void *getPeer_Effect(void *java_effect)
{
    SceneClass *effect_class;
    JavaField *field;
    ToolkitEffectPeer *peer;
    ToolkitEffectVectorStorage position;
    ToolkitEffectVectorStorage orientation;
    int effect_id;

    effect_class = classJava_xeno_Effect;
    field = lookupClassField(effect_class,
                             loadConstString(unit_field_id, -1), 0);
    effect_id = JAVA_INT_FIELD(java_effect, field);
    field = lookupClassField(effect_class,
                             loadConstString(unit_field_px, -1), 0);
    sefLoadEffectCf(0, effect_id);
    position.xyz.x = JAVA_FLOAT_FIELD(java_effect, field);

    field = lookupClassField(effect_class,
                             loadConstString(unit_field_py, -1), 0);
    position.xyz.y = JAVA_FLOAT_FIELD(java_effect, field);
    field = lookupClassField(effect_class,
                             loadConstString(unit_field_pz, -1), 0);
    position.xyz.z = JAVA_FLOAT_FIELD(java_effect, field);

    field = lookupClassField(effect_class,
                             loadConstString(unit_field_ry, -1), 0);
    orientation.xyz.x = 0.0f;
    orientation.xyz.z = 0.0f;
    orientation.xyz.y = JAVA_FLOAT_FIELD(java_effect, field);
    peer = sefCreateEffectCf(effect_id, &position.xyz, &orientation.xyz);

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

    if (JNI_isInstanceOf(java_object, classJava_xeno_Chr) != 0) {
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

void Java_xeno_util_Toolkit_peerSetGroup__II(JThread *thread,
                                              ToolkitPeerGroupCall *arguments)
{
    int value;

    /* Decode the signed Java integer before the peer table narrows it. */
    __builtin_memcpy(&value, &arguments->valueBits, sizeof(value));
    XTK_peerGroup[arguments->group & 3] = value;
}
