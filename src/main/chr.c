#include "common.h"
#include "debug_break.h"
#include "chr.h"
#include "main/init_vm.h"
#include "main/jni.h"
#include "main/jthread.h"
#include "main/toolkit.h"
#include "quadword.h"

/* Bind a following target together with the enabled rotation axes. */
#define CHR_SET_FOLLOW_TARGET(rotation_value, target_value, flags_value) do { \
    (rotation_value)->target = (target_value); \
    (rotation_value)->face_flags = (flags_value); \
} while (0)

/*
 * The state byte at Actor+0xa40, past the recovered head of the struct:
 * growing the struct there would move data_header and every field after it,
 * which other functions read at their current offsets, so this access stays
 * a byte view, as ACTOR_SLOT_NUMBER_OFFSET does for +0x80. Only
 * Java_xeno_Chr_setTranslate__ uses it: it tests bit 0 and, when set,
 * stores the literal 2 back.
 */
#define ACTOR_TRANSLATE_STATE_OFFSET 0xa40

#define CLEAR_SEQUENCE_STATE_FLAGS(sequence) \
    do { (sequence)->state_flags = 0; } while (0)

#define SET_THREAD_ACTOR_WAIT(thread, actor) \
    do { \
        (thread)->wait_target.object = (actor); \
        (thread)->wait_kind = JTHREAD_WAIT_ACTOR; \
        (thread)->resume_frames = (thread)->frame_depth; \
        (thread)->flags |= 4; \
    } while (0)

#define SET_THREAD_WAIT_RESUME(thread) \
    do { \
        (thread)->resume_frames = (thread)->frame_depth; \
        (thread)->flags |= 1; \
    } while (0)

/* Name interning and class-field resolution are the two VM operations
 * performed by this native; its field descriptor is deliberately discarded. */
#define CHR_RESOLVE_PEER_DESCRIPTOR() do { \
    SceneString *peer_name = loadConstString(chr_peer_string, -1); \
    (void)lookupClassField(classJava_xeno_Chr, peer_name, 0); \
} while (0)


/* Script rates are per second; the sequences step once per frame. */
#define CHR_FRAMES_PER_SECOND 30.0f

/* ActSequence channel bits (state_flags and channel_mask): one per animated channel. */
enum {
    CHR_CHANNEL_MOVE = 0x1,
    CHR_CHANNEL_ROT_X = 0x2,
    CHR_CHANNEL_ROT_Y = 0x4,
    CHR_CHANNEL_ROT_Z = 0x8,
    CHR_CHANNEL_ROT_ALL = 0xe,
    CHR_CHANNEL_MOTION = 0x10,
    CHR_CHANNEL_SCALE_X = 0x20,
    CHR_CHANNEL_SCALE_Y = 0x40,
    CHR_CHANNEL_SCALE_Z = 0x80,
    CHR_CHANNEL_SCALE_ALL = 0xe0
};

/* Rotation record face_flags: the axes that are being driven. */
enum {
    CHR_FACE_AXIS_X = 0x1,
    CHR_FACE_AXIS_Y = 0x2,
    CHR_FACE_AXIS_Z = 0x4
};

/* Actor.flags bits the natives of this class set and clear. */
enum {
    CHR_ACTOR_BUSY = 0x2,               /* a sequence or script wait is running on the actor */
    CHR_ACTOR_HIDDEN = 0x8,             /* setVisible(false) */
    CHR_ACTOR_COLLISION = 0x40,         /* setCollision(true) */
    CHR_ACTOR_NO_RADAR = 0x80,          /* dispRadar(false) */
    CHR_ACTOR_MOTION_NO_UPDATE = 0x400, /* setMotNoUpdate(1) */
    CHR_ACTOR_POINT_LIGHT = 0x800       /* set by the point light setters, cleared by setFilter */
};

/* Actor.status_flags bits. */
enum {
    CHR_STATUS_EDGE_FALL = 0x8,         /* setEdgeFall */
    CHR_STATUS_ELEVATOR = 0x20          /* setElevatorMode */
};

/* Actor.drawEnv.flags bits. */
enum {
    CHR_DRAW_SYMMETRY_Y = 0x10,         /* setSymmetryY */
    CHR_DRAW_POINT_LIGHT = 0x20,        /* point light colours/positions are set */
    CHR_DRAW_MOTION_NO_UPDATE = 0x100,  /* setMotNoUpdate(2) */
    CHR_DRAW_CLIP = 0x200,              /* setClip */
    CHR_DRAW_IGNORE_SHAPE = 0x400       /* ignoreShape */
};

/* Actor.look_mode, set by the look_* natives. */
enum {
    CHR_LOOK_DEFAULT = 1,
    CHR_LOOK_CAMERA = 2,
    CHR_LOOK_POINT = 3,
    CHR_LOOK_CHAR = 4,
    CHR_LOOK_UNIT = 5
};

/* setFilter selectors with a draw handler. */
enum {
    CHR_FILTER_GUNO = 2,
    CHR_FILTER_STEALTH = 3
};

/* setParent: a joint with this bit set names a relation rather than a model joint. */
#define CHR_JOINT_RELATION 0x8000
/* ACT_setArms accessory id of the right-hand weapon. */
#define CHR_ARMS_WEAPON_R 0x108

#define CHR_POINT_LIGHT_COUNT 3
#define CHR_POINT_LIGHT_COUNT_UNSIGNED 3U
#define CHR_MAP_SHADOW_PART_MAX 8
#define CHR_PART_ALPHA_MAX 4
/* Motion numbers with a bit in the top byte are absolute ids; others are 1-based. */
#define CHR_MOTION_ABSOLUTE_MASK 0xff000000
#define CHR_MOTION_DEFAULT_BLEND 8

/* setPlayer: the player model's fixed resource layout (GameResource[0]) and matrix counts. */
#define CHR_PLAYER_RESOURCE_SIZE 0x12a000
#define CHR_PLAYER_MATRIX_COUNT 0xaa
#define CHR_PLAYER_SUB_MATRIX_COUNT 0x33
#define CHR_PLAYER_SUB_ID_BASE 0x10000

/* Runtime Java fields occupy one EE VM value word. The descriptor
 * supplies its byte offset; its declared Java/native kind selects
 * the integer, floating or reference representation below. No fixed
 * Java-object field positions are assumed. */
typedef union ChrFieldValue {
    Actor * actor;
    float floating;
    u32 bits;
    void * native_pointer;
    SceneObject object;
} ChrFieldValue;

/* system.evt's xeno/Chr.mtnGetRoot descriptor is
 * (ILxeno/util/Vector4f;)V. The receiver and its two explicit arguments
 * occupy consecutive VM words, as the Unit implementation also reads at
 * offsets 0, 4 and 8. The destination is a Java Vector4f object. */
typedef struct ChrMotionRootCall {
    SceneObject object;
    int root_kind;
    SceneObject destination;
} ChrMotionRootCall;

typedef union ChrMoveArgument {
    int integer;
    float floating;
    SceneObject object;
    u8 byte;
} ChrMoveArgument;

struct ChrMoveCall {
    SceneObject object;
    ChrMoveArgument argument[3];
    u8 wait;
};

typedef union {
    int integer;
    float floating;
    SceneObject object;
} ChrRotArgument;

/*
 * Call block of the rate-follow forms Java_xeno_Chr_rot{X,Z}__Ljava_lang_Object_FZ
 * (target object, then rate). CHR_rotX/CHR_rotZ read its target word as a
 * plain record field: a read through ChrRotCall's argument union (alias
 * set 0) is scheduled ahead of the JavaField offset load instead. Their
 * mode selects the record, so they take the call block untyped.
 */
typedef struct ChrRotRateFollowCall {
    SceneObject object;
    SceneObject target; /* +0x04 */
    float rate;         /* +0x08 */
    u8 wait;            /* +0x0c */
} ChrRotRateFollowCall;

struct ChrRotCall {
    SceneObject object;
    ChrRotArgument first;
    ChrRotArgument second;
    u8 wait;
};

typedef union {
    int integer;
    float floating;
    u16 half;
    u8 byte;
} ChrMotionArgument;

struct ChrMotionCall {
    SceneObject object;
    ChrMotionArgument argument[7];
};

typedef struct ChrSavedPartyPrefix {
    unsigned char unmodeled_00[0x100a4];
    u16 player_character_id;
} ChrSavedPartyPrefix;

/* JNI argument slots carry either object references or raw scalar bits. */
typedef union ChrVmArgumentSlot {
    SceneObject object;
    u32 bits;
    float floating;
} ChrVmArgumentSlot;


typedef struct ChrStartStringValue {
    unsigned char unmodeled_00[4];
    int length;
    const char *data;
} ChrStartStringValue;

typedef struct ChrStartString {
    unsigned char unmodeled_00[4];
    struct ChrStartStringValue *value;
} ChrStartString;

typedef struct ChrStartCall {
    SceneObject object;
    int mode;
    struct ChrStartString *method_name;
} ChrStartCall;

struct ChrDropShadowEntry {
    union {
        u32 key;
        struct {
            unsigned short mask;
            unsigned short value;
        } fields;
    } match;
    unsigned char *tokens;
};

struct ChrShadowByteArray {
    unsigned char unmodeled_00[8];
    const signed char *bytes;
};

struct ChrShadowBytesCall {
    SceneObject object;
    struct ChrShadowByteArray *bytes;
};

extern float chrRotXFramesDegToRad;
extern float chrRotXRateDegToRad;
extern float chrRotXStepDegToRad;
extern float chrRotZFramesDegToRad;
extern float chrRotZRateDegToRad;
extern float chrRotZStepDegToRad;

extern struct {
    u32 address;
    int size;
    int handle;
    int state;
} GameResource[128];

extern const char chrIdKey[];
extern float chrRotYFramesDegToRad;
extern float chrRotYRateDegToRad;
extern float chrRotYStepDegToRad;
extern struct ChrDropShadowEntry DropShadowDefineTable[10];

/*
 * CHR_moveXZ is local to this file. The move trampolines below call it with
 * a mode. Overload -> mode: IFFZ 0, FFFZ 1, I,Object,Z 2, Object,F,Z 3.
 */
static void CHR_moveXZ(int mode, JThread *thread, ChrMoveCall *arguments,
                       u32 *failure_result);

/*
 * CHR_rotX/CHR_rotY/CHR_rotZ are local to this file. The rotation trampolines
 * below call them with a mode. Overload -> mode: FFZ 1, IFZ 0, I,Object,Z 2,
 * Object,F,Z 3 (same as CHR_sclX).
 */
static void CHR_rotX(int mode, JThread *thread, void *call_block,
                     u32 *failure_result);

static void CHR_rotY(int mode, JThread *thread, ChrRotCall *arguments,
                     u32 *failure_result);

static void CHR_rotZ(int mode, JThread *thread, void *call_block,
                     u32 *failure_result);

/*
 * CHR_motion is local to this file. The mtn trampolines below call it with a
 * mode. Overload -> mode: IIIIIFZ 1, IIFZ 0.
 */
static void CHR_motion(int mode, JThread *thread, ChrMotionCall *arguments,
                       u32 *failure_result);

extern void ACT_setModelWrapper(void *actor, int enabled);
extern void ACT_setRelation(void *actor, void *parent, int relation, int enabled);
extern void ACT_setHumanHand(void *actor, int hand);
extern void ACT_setMotion(void *actor, int motion);
extern int ACT_initMotion(void *actor);
extern void ACT_setParent(Actor *actor, int type, Actor *parent,
                          int joint, int id);

extern void ACT_updateSequence(Actor *actor);
extern void ACT_filterGuno(Actor *actor);
extern void ACT_filterStealth(Actor *actor);
extern void SEQ_moveChr(Actor *actor);
extern void SEQ_moveXZ(Actor *actor);
extern void ACT_updateDefault(Actor *actor);
extern void ACT_updatePlayer(Actor *actor);
extern void ACT_updateMPack(Actor *actor);
extern int SCRIPT_methodClearGet(void);

extern JavaMethod *findMethod(JavaClass *clazz, SceneString *name,
                               void *signature_or_type);

extern void JNI_initThread(JThread *thread);
const char chr_algorithm_string[] = "algorithm";

static float defaultOffset[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

const char chr_peer_string[] = "peer";

const char chrTranslateXKey[] = "px";

const char chrTranslateYKey[] = "py";

const char chrTranslateZKey[] = "pz";

const char chrRotateXKey[] = "rx";

const char chrRotateYKey[] = "ry";

const char chrRotateZKey[] = "rz";

float chrRotXFramesDegToRad = 3.1415927410125732f;

float chrRotXRateDegToRad = 3.1415927410125732f;

float chrRotXStepDegToRad = 3.1415927410125732f;

float chrRotYFramesDegToRad = 3.1415927410125732f;

float chrRotYRateDegToRad = 3.1415927410125732f;

float chrRotYStepDegToRad = 3.1415927410125732f;

float chrRotZFramesDegToRad = 3.1415927410125732f;

float chrRotZRateDegToRad = 3.1415927410125732f;

float chrRotZStepDegToRad = 3.1415927410125732f;

const char chrIdKey[3] = "id";

/*
 * Stores GameLoopState[1] directly into the object's peer field. Unlike the
 * other Chr natives, it does not follow the peer pointer's target.
 */
void Java_xeno_Chr_getPlayer__(JThread *thread, ChrScaleCall *arguments)
{
    ChrFieldValue *field_value;
    JavaField *peer_field;
    SceneObject object;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(chr_peer_string, -1), 0);
    field_value = (void *)(object + peer_field->offset);
    field_value->bits = GameLoopState[1];
}

/*
 * Sets up the player and its companion actor when the player model resource
 * is loaded, otherwise reuses the stored peer. The player becomes
 * GameLoopState[1].
 */
void Java_xeno_Chr_setPlayer__(JThread *thread, ChrScaleCall *arguments)
{
    JavaField *field;
    Actor *player;
    Actor *created_actor;
    SceneObject object;
    u16 player_id;

    object = arguments->object;
    player_id = ((ChrSavedPartyPrefix *)SaveData)->player_character_id;

    if ((GameResource[0].size == CHR_PLAYER_RESOURCE_SIZE) &&
        (GameResource[1].state == 9)) {
        field = lookupClassField(
            classJava_xeno_Chr, loadConstString(chrIdKey, -1), 0);
        *(u32 *)(object + field->offset) = player_id;
        player = getPeer_Chr(object);

        ACTOR_RESOURCES(player)[0] = (void *)0x01000000;
        ACTOR_RESOURCES(player)[1] = (void *)0x01070800;
        ACTOR_RESOURCES(player)[2] = (void *)0x010b1000;
        ACTOR_RESOURCES(player)[3] = (void *)0x010e7000;
        ACT_allocMatrix(player, CHR_PLAYER_MATRIX_COUNT);
        ACT_setModelWrapper(player, 0);

        created_actor = ACT_create(-2, player_id + CHR_PLAYER_SUB_ID_BASE);
        ACTOR_RESOURCES(created_actor)[0] = (void *)0x010b4000;
        ACTOR_RESOURCES(created_actor)[2] = (void *)0x010e6000;
        ACTOR_RESOURCES(created_actor)[1] = (void *)0;
        ACT_allocMatrix(created_actor, CHR_PLAYER_SUB_MATRIX_COUNT);
        ACT_setModelWrapper(created_actor, 0);
        created_actor->shadow_kind = 0;
        created_actor->flags &= ~0x20;
        ACT_initMotion(created_actor);
        ACT_setRelation(created_actor, player, CHR_JOINT_RELATION | 0x200, 1);
        ACT_setHumanHand(player, 0);
        ACT_setMotion(player, 0x8000);
    } else {
        field = lookupClassField(
            classJava_xeno_Chr, loadConstString(chr_peer_string, -1), 0);
        player = *(Actor **)(object + field->offset);
    }

    player->update = 0;
    GameLoopState[1] = (u32)player;
}

/*
 * Queues a move of the Chr's actor in X and Z, or toward another Chr, using
 * the argument layout of the given mode. Waits on the thread when wait is set.
 */
static void CHR_moveXZ(int mode, JThread *thread, ChrMoveCall *arguments,
                       u32 *failure_result)
{
    JavaField *peer_field;
    JavaField *algorithm_field;
    Actor *peer;
    const float *source;
    ActSequenceEntry *entry;
    SequenceState *sequence;
    MoveTrack *move;
    Actor *chr_peer;
    ChrFieldValue *target_field;
    SceneObject object;
    u8 wait;

    wait = 0;
    object = arguments->object;
    if (JNI_isInstanceOf(object, classJava_xeno_Chr) == 0) {
        *failure_result = 0;
        return;
    }

    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(chr_peer_string, -1), 0);
    peer = *(Actor **)(object + peer_field->offset);
    entry = &actSequence[((u8 *)peer)[ACTOR_SLOT_NUMBER_OFFSET]];
    sequence = &entry->state;
    move = &entry->move_channel.move;
    sequence->channel_mask = CHR_CHANNEL_MOVE;
    sequence->state_flags |= CHR_CHANNEL_MOVE;

    algorithm_field = lookupClassField(
        classJava_xeno_Chr, loadConstString(chr_algorithm_string, -1), 0);
    if (*(u32 *)(object + algorithm_field->offset) & 1) {
        source = &peer->position.x;
    } else {
        source = defaultOffset;
    }

    switch (mode) {
    case 0:
        move->frames = arguments->argument[0].integer;
        move->target.x = arguments->argument[1].floating + source[0];
        move->target.z = arguments->argument[2].floating + source[2];
        sequence->handler[0] = SEQ_moveXZ;
        wait = arguments->wait;
        break;
    case 1:
        move->frames = -1;
        move->target.x = arguments->argument[0].floating + source[0];
        move->target.z = arguments->argument[1].floating + source[2];
        move->target.w = arguments->argument[2].floating / CHR_FRAMES_PER_SECOND;
        sequence->handler[0] = SEQ_moveXZ;
        wait = arguments->wait;
        break;
    case 2:
        peer_field = lookupClassField(
            classJava_xeno_Chr, loadConstString(chr_peer_string, -1), 0);
        chr_peer = *(Actor **)(arguments->argument[1].object + peer_field->offset);
        move->frames = arguments->argument[0].integer;
        move->target_actor = chr_peer;
        sequence->handler[0] = SEQ_moveChr;
        wait = arguments->argument[2].byte;
        break;
    case 3:
        peer_field = lookupClassField(
            classJava_xeno_Chr, loadConstString(chr_peer_string, -1), 0);
        target_field = (void *)(arguments->argument[0].object + peer_field->offset);
        move->frames = -1;
        chr_peer = target_field->actor;
        move->target_actor = chr_peer;
        move->target.w = arguments->argument[1].floating / CHR_FRAMES_PER_SECOND;
        sequence->handler[0] = SEQ_moveChr;
        wait = arguments->argument[2].byte;
        break;
    }

    if (wait == 1) {
        peer->flags |= CHR_ACTOR_BUSY;
        if (thread->kind != wait && (thread->flags & 0x40) == 0) {
            return;
        }
        if ((thread->flags & 0x40) != 0) {
            SET_THREAD_ACTOR_WAIT(thread, peer);
        }
        SET_THREAD_WAIT_RESUME(thread);
    }
}

void Java_xeno_Chr_move__IFFZ(JThread *thread, ChrMoveCall *arguments,
                              u32 *failure_result)
{
    CHR_moveXZ(0, thread, arguments, failure_result);
}

void Java_xeno_Chr_move__FFFZ(JThread *thread, ChrMoveCall *arguments,
                              u32 *failure_result)
{
    CHR_moveXZ(1, thread, arguments, failure_result);
}

void Java_xeno_Chr_move__ILjava_lang_Object_Z(JThread *thread,
                                              ChrMoveCall *arguments,
                                              u32 *failure_result)
{
    CHR_moveXZ(2, thread, arguments, failure_result);
}

void Java_xeno_Chr_move__Ljava_lang_Object_FZ(JThread *thread,
                                              ChrMoveCall *arguments,
                                              u32 *failure_result)
{
    CHR_moveXZ(3, thread, arguments, failure_result);
}

/* Starts a spline move of the Chr's actor over frames; waits when wait is set. */
void Java_xeno_Chr_move__Lxeno_util_Spline_IZ(JThread *thread, ChrSplineCall *arguments,
                                             u32 *failure_result)
{
    ChrFieldValue *field_value;
    JavaField *peer_field;
    Actor *peer;
    ActSequenceEntry *entry;
    SequenceState *sequence;
    SplineTrack *track;
    SceneObject object;
    ChrSpline *spline;
    int frames;
    int wait;

    object = arguments->object;
    if (JNI_isInstanceOf(object, classJava_xeno_Chr) == 0) {
        *failure_result = 0;
        return;
    }
    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(chr_peer_string, -1), 0);
    field_value = (void *)(object + peer_field->offset);
    peer = field_value->actor;
    entry = &actSequence[((u8 *)peer)[ACTOR_SLOT_NUMBER_OFFSET]];
    sequence = &entry->state;
    track = &entry->move_channel.spline;
    sequence->channel_mask = CHR_CHANNEL_MOVE;
    sequence->state_flags |= CHR_CHANNEL_MOVE;
    lookupClassField(classJava_xeno_Chr,
                     loadConstString(chr_algorithm_string, -1), 0);
    spline = arguments->spline;
    frames = arguments->frames;
    wait = arguments->wait;
    track->flags = frames;
    sequence->handler[0] = SEQ_moveSPL;
    track->spline = spline;
    track->last_frame = spline->last_frame;
    if (wait == 1) {
        peer->flags |= CHR_ACTOR_BUSY;
        if (thread->kind != wait && (thread->flags & 0x40) == 0)
            return;
        if ((thread->flags & 0x40) != 0) {
            thread->wait_target.object = peer;
            thread->wait_kind = JTHREAD_WAIT_ACTOR;
            thread->resume_frames = thread->frame_depth;
            thread->flags |= 4;
        }
        thread->resume_frames = thread->frame_depth;
        thread->flags |= 1;
    }
}

/* Starts a spline rotation of the Chr's actor on all rotation axes; waits when wait is set. */
void Java_xeno_Chr_rotate__Lxeno_util_Spline_IZ(JThread *thread, ChrSplineCall *arguments,
                                               u32 *failure_result)
{
    ChrFieldValue *field_value;
    JavaField *peer_field;
    Actor *peer;
    ActSequenceEntry *entry;
    SequenceState *sequence;
    SplineTrack *track;
    SceneObject object;
    ChrSpline *spline;
    int frames;
    int wait;

    object = arguments->object;
    if (JNI_isInstanceOf(object, classJava_xeno_Chr) == 0) {
        *failure_result = 0;
        return;
    }
    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(chr_peer_string, -1), 0);
    field_value = (void *)(object + peer_field->offset);
    peer = field_value->actor;
    entry = &actSequence[((u8 *)peer)[ACTOR_SLOT_NUMBER_OFFSET]];
    sequence = &entry->state;
    track = &entry->rotation_channel.spline;
    sequence->channel_mask = CHR_CHANNEL_ROT_ALL;
    sequence->state_flags |= CHR_CHANNEL_ROT_ALL;
    lookupClassField(classJava_xeno_Chr,
                     loadConstString(chr_algorithm_string, -1), 0);
    spline = arguments->spline;
    frames = arguments->frames;
    wait = arguments->wait;
    track->flags = frames;
    sequence->handler[1] = SEQ_rotateSPL;
    track->spline = spline;
    track->last_frame = spline->last_frame;
    if (wait == 1) {
        peer->flags |= CHR_ACTOR_BUSY;
        if (thread->kind != wait && (thread->flags & 0x40) == 0)
            return;
        if ((thread->flags & 0x40) != 0) {
            thread->wait_target.object = peer;
            thread->wait_kind = JTHREAD_WAIT_ACTOR;
            thread->resume_frames = thread->frame_depth;
            thread->flags |= 4;
        }
        thread->resume_frames = thread->frame_depth;
        thread->flags |= 1;
    }
}

/*
 * Sets the X-axis rotation of the Chr's actor for the given mode. Waits on
 * the thread when wait is set.
 */
static void CHR_rotX(int mode, JThread *thread, void *call_block,
                     u32 *failure_result)
{
    ChrRotCall *arguments = call_block;
    ChrRotRateFollowCall *rate_follow = call_block;
    JavaField *peer_field;
    JavaField *algorithm_field;
    Actor *peer;
    float *rotation_source;
    SequenceState *sequence;
    ActSequenceEntry *entry;
    RotationTrack *rotation;
    SceneObject object;
    Actor *target_peer;
    u8 wait;

    wait = 0;
    object = arguments->object;
    if (JNI_isInstanceOf(object, classJava_xeno_Chr) == 0) {
        *failure_result = 0;
        return;
    }

    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(chr_peer_string, -1), 0);
    peer = *(Actor **)(object + peer_field->offset);
    algorithm_field = lookupClassField(
        classJava_xeno_Chr, loadConstString(chr_algorithm_string, -1), 0);

    rotation_source =
        (*(u32 *)(object + algorithm_field->offset) & 1) != 0
            ? &peer->rotation.x
            : defaultOffset;

    entry = &actSequence[((u8 *)peer)[ACTOR_SLOT_NUMBER_OFFSET]];
    sequence = &entry->state;
    SEQUENCE_ROTATION_HANDLER(sequence) = SEQ_rotate;
    sequence->state_flags |= CHR_CHANNEL_ROT_X;
    rotation = &entry->rotation_channel.rotation;

    switch (mode) {
    case 0:
        rotation->duration[0] = arguments->first.integer;
        rotation->face_flags &= ~CHR_FACE_AXIS_X;
        rotation->goal[0] =
            ((arguments->second.floating + rotation_source[0]) / 180.0f) *
            chrRotXFramesDegToRad;
        wait = arguments->wait;
        break;
    case 1:
        rotation->duration[0] = -1;
        rotation->face_flags &= ~CHR_FACE_AXIS_X;
        rotation->goal[0] =
            ((arguments->first.floating + rotation_source[0]) / 180.0f) *
            chrRotXRateDegToRad;
        rotation->step[0] =
            ((arguments->second.floating / 180.0f) * chrRotXRateDegToRad) / CHR_FRAMES_PER_SECOND;
        wait = arguments->wait;
        break;
    case 2: {
        u32 updated_axis_flags;

        target_peer = *(Actor **)(arguments->second.object + algorithm_field->offset);
        updated_axis_flags = rotation->face_flags | CHR_FACE_AXIS_X;
        rotation->duration[0] = arguments->first.integer;
        CHR_SET_FOLLOW_TARGET(rotation, target_peer, updated_axis_flags);
        wait = arguments->wait;
        break;
    }
    case 3:
        target_peer = *(Actor **)(rate_follow->target + algorithm_field->offset);
        rotation->face_flags |= CHR_FACE_AXIS_X;
        rotation->duration[0] = -1;
        rotation->target = target_peer;
        rotation->step[0] =
            ((arguments->second.floating / 180.0f) * chrRotXStepDegToRad) / CHR_FRAMES_PER_SECOND;
        wait = arguments->wait;
        break;
    }

    if (wait == 1) {
        peer->flags |= CHR_ACTOR_BUSY;
        if (thread->kind != wait && (thread->flags & 0x40) == 0)
            return;
        if ((thread->flags & 0x40) != 0) {
            thread->wait_target.object = peer;
            thread->wait_kind = JTHREAD_WAIT_ACTOR;
            thread->resume_frames = thread->frame_depth;
            thread->flags |= 4;
        }
        thread->resume_frames = thread->frame_depth;
        thread->flags |= 1;
    }
}

/*
 * Sets the Y-axis rotation of the Chr's actor for the given mode. Waits on
 * the thread when wait is set.
 */
static void CHR_rotY(int mode, JThread *thread, ChrRotCall *arguments,
                     u32 *failure_result)
{
    JavaField *peer_field;
    JavaField *algorithm_field;
    Actor *peer;
    float *rotation_source;
    SequenceState *sequence;
    ActSequenceEntry *entry;
    RotationTrack *rotation;
    SceneObject object;
    Actor *target_peer;
    u8 wait;

    wait = 0;
    object = arguments->object;
    if (JNI_isInstanceOf(object, classJava_xeno_Chr) == 0) {
        *failure_result = 0;
        return;
    }

    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(chr_peer_string, -1), 0);
    peer = *(Actor **)(object + peer_field->offset);
    algorithm_field = lookupClassField(
        classJava_xeno_Chr, loadConstString(chr_algorithm_string, -1), 0);

    rotation_source =
        (*(u32 *)(object + algorithm_field->offset) & 1) != 0
            ? &peer->rotation.x
            : defaultOffset;

    entry = &actSequence[((u8 *)peer)[ACTOR_SLOT_NUMBER_OFFSET]];
    sequence = &entry->state;
    SEQUENCE_ROTATION_HANDLER(sequence) = SEQ_rotate;
    sequence->state_flags |= CHR_CHANNEL_ROT_Y;
    rotation = &entry->rotation_channel.rotation;

    switch (mode) {
    case 0:
        rotation->duration[1] = arguments->first.integer;
        rotation->face_flags &= ~CHR_FACE_AXIS_Y;
        rotation->goal[1] =
            ((arguments->second.floating + rotation_source[1]) / 180.0f) *
            chrRotYFramesDegToRad;
        wait = arguments->wait;
        break;
    case 1:
        rotation->duration[1] = -1;
        rotation->face_flags &= ~CHR_FACE_AXIS_Y;
        rotation->goal[1] =
            ((arguments->first.floating + rotation_source[1]) / 180.0f) *
            chrRotYRateDegToRad;
        rotation->step[1] =
            ((arguments->second.floating / 180.0f) * chrRotYRateDegToRad) / CHR_FRAMES_PER_SECOND;
        wait = arguments->wait;
        break;
    case 2: {
        u32 updated_axis_flags;

        peer_field = lookupClassField(
            classJava_xeno_Chr, loadConstString(chr_peer_string, -1), 0);
        target_peer = *(Actor **)(arguments->second.object + peer_field->offset);
        updated_axis_flags = rotation->face_flags | CHR_FACE_AXIS_Y;
        rotation->duration[1] = arguments->first.integer;
        CHR_SET_FOLLOW_TARGET(rotation, target_peer, updated_axis_flags);
        wait = arguments->wait;
        break;
    }
    case 3:
        peer_field = lookupClassField(
            classJava_xeno_Chr, loadConstString(chr_peer_string, -1), 0);
        target_peer = *(Actor **)(arguments->first.object + peer_field->offset);
        rotation->duration[1] = -1;
        rotation->face_flags |= CHR_FACE_AXIS_Y;
        rotation->target = target_peer;
        rotation->step[1] =
            ((arguments->second.floating / 180.0f) * chrRotYStepDegToRad) / CHR_FRAMES_PER_SECOND;
        wait = arguments->wait;
        break;
    }

    if (wait == 1) {
        peer->flags |= CHR_ACTOR_BUSY;
        if (thread->kind != wait && (thread->flags & 0x40) == 0)
            return;
        if ((thread->flags & 0x40) != 0) {
            thread->wait_target.object = peer;
            thread->wait_kind = JTHREAD_WAIT_ACTOR;
            thread->resume_frames = thread->frame_depth;
            thread->flags |= 4;
        }
        thread->resume_frames = thread->frame_depth;
        thread->flags |= 1;
    }
}

/*
 * Sets the Z-axis rotation of the Chr's actor for the given mode. Waits on
 * the thread when wait is set.
 */
static void CHR_rotZ(int mode, JThread *thread, void *call_block,
                     u32 *failure_result)
{
    ChrRotCall *arguments = call_block;
    ChrRotRateFollowCall *rate_follow = call_block;
    JavaField *peer_field;
    JavaField *algorithm_field;
    Actor *peer;
    float *rotation_source;
    SequenceState *sequence;
    ActSequenceEntry *entry;
    RotationTrack *rotation;
    SceneObject object;
    Actor *target_peer;
    u8 wait;

    wait = 0;
    object = arguments->object;
    if (JNI_isInstanceOf(object, classJava_xeno_Chr) == 0) {
        *failure_result = 0;
        return;
    }

    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(chr_peer_string, -1), 0);
    peer = *(Actor **)(object + peer_field->offset);
    algorithm_field = lookupClassField(
        classJava_xeno_Chr, loadConstString(chr_algorithm_string, -1), 0);

    rotation_source =
        (*(u32 *)(object + algorithm_field->offset) & 1) != 0
            ? &peer->rotation.x
            : defaultOffset;

    entry = &actSequence[((u8 *)peer)[ACTOR_SLOT_NUMBER_OFFSET]];
    sequence = &entry->state;
    SEQUENCE_ROTATION_HANDLER(sequence) = SEQ_rotate;
    sequence->state_flags |= CHR_CHANNEL_ROT_Z;
    rotation = &entry->rotation_channel.rotation;

    switch (mode) {
    case 0:
        rotation->duration[2] = arguments->first.integer;
        rotation->face_flags &= ~CHR_FACE_AXIS_Z;
        rotation->goal[2] =
            ((arguments->second.floating + rotation_source[2]) / 180.0f) *
            chrRotZFramesDegToRad;
        wait = arguments->wait;
        break;
    case 1:
        rotation->duration[2] = -1;
        rotation->face_flags &= ~CHR_FACE_AXIS_Z;
        rotation->goal[2] =
            ((arguments->first.floating + rotation_source[2]) / 180.0f) *
            chrRotZRateDegToRad;
        rotation->step[2] =
            ((arguments->second.floating / 180.0f) * chrRotZRateDegToRad) / CHR_FRAMES_PER_SECOND;
        wait = arguments->wait;
        break;
    case 2: {
        u32 updated_axis_flags;

        target_peer = *(Actor **)(arguments->second.object + algorithm_field->offset);
        updated_axis_flags = rotation->face_flags | CHR_FACE_AXIS_Z;
        rotation->duration[2] = arguments->first.integer;
        CHR_SET_FOLLOW_TARGET(rotation, target_peer, updated_axis_flags);
        wait = arguments->wait;
        break;
    }
    case 3:
        target_peer = *(Actor **)(rate_follow->target + algorithm_field->offset);
        rotation->face_flags |= 2;
        rotation->duration[2] = -1;
        rotation->target = target_peer;
        rotation->step[2] =
            ((arguments->second.floating / 180.0f) * chrRotZStepDegToRad) / CHR_FRAMES_PER_SECOND;
        wait = arguments->wait;
        break;
    }

    if (wait == 1) {
        peer->flags |= CHR_ACTOR_BUSY;
        if (thread->kind != wait && (thread->flags & 0x40) == 0)
            return;
        if ((thread->flags & 0x40) != 0) {
            thread->wait_target.object = peer;
            thread->wait_kind = JTHREAD_WAIT_ACTOR;
            thread->resume_frames = thread->frame_depth;
            thread->flags |= 4;
        }
        thread->resume_frames = thread->frame_depth;
        thread->flags |= 1;
    }
}

void Java_xeno_Chr_rotX__FFZ(JThread *thread, ChrRotCall *arguments,
                             u32 *failure_result)
{
    CHR_rotX(1, thread, arguments, failure_result);
}

void Java_xeno_Chr_rotX__IFZ(JThread *thread, ChrRotCall *arguments,
                             u32 *failure_result)
{
    CHR_rotX(0, thread, arguments, failure_result);
}

void Java_xeno_Chr_rotX__ILjava_lang_Object_Z(JThread *thread,
                                              ChrRotCall *arguments,
                                              u32 *failure_result)
{
    CHR_rotX(2, thread, arguments, failure_result);
}

void Java_xeno_Chr_rotX__Ljava_lang_Object_FZ(JThread *thread,
                                              ChrRotRateFollowCall *arguments,
                                              u32 *failure_result)
{
    CHR_rotX(3, thread, arguments, failure_result);
}

void Java_xeno_Chr_rotY__FFZ(JThread *thread, ChrRotCall *arguments,
                             u32 *failure_result)
{
    CHR_rotY(1, thread, arguments, failure_result);
}

void Java_xeno_Chr_rotY__IFZ(JThread *thread, ChrRotCall *arguments,
                             u32 *failure_result)
{
    CHR_rotY(0, thread, arguments, failure_result);
}

void Java_xeno_Chr_rotY__ILjava_lang_Object_Z(JThread *thread,
                                              ChrRotCall *arguments,
                                              u32 *failure_result)
{
    CHR_rotY(2, thread, arguments, failure_result);
}

void Java_xeno_Chr_rotY__Ljava_lang_Object_FZ(JThread *thread,
                                              ChrRotCall *arguments,
                                              u32 *failure_result)
{
    CHR_rotY(3, thread, arguments, failure_result);
}

void Java_xeno_Chr_rotZ__FFZ(JThread *thread, ChrRotCall *arguments,
                             u32 *failure_result)
{
    CHR_rotZ(1, thread, arguments, failure_result);
}

void Java_xeno_Chr_rotZ__IFZ(JThread *thread, ChrRotCall *arguments,
                             u32 *failure_result)
{
    CHR_rotZ(0, thread, arguments, failure_result);
}

void Java_xeno_Chr_rotZ__ILjava_lang_Object_Z(JThread *thread,
                                              ChrRotCall *arguments,
                                              u32 *failure_result)
{
    CHR_rotZ(2, thread, arguments, failure_result);
}

void Java_xeno_Chr_rotZ__Ljava_lang_Object_FZ(JThread *thread,
                                              ChrRotRateFollowCall *arguments,
                                              u32 *failure_result)
{
    CHR_rotZ(3, thread, arguments, failure_result);
}

/*
 * Selects the actor's update handler for the start mode. Mode 1 also binds
 * the named script method to the Chr's thread.
 */
void Java_xeno_Chr_start__ILjava_lang_Object_(JThread *thread,
                                               ChrStartCall *arguments,
                                               u32 *failure_result)
{
    int mode;
    SceneObject object;
    JavaField *peer_field;
    ChrFieldValue *peer_value;
    Actor *peer;
    SequenceState *state;

    (void)thread;
    object = arguments->object;
    if (JNI_isInstanceOf(object, classJava_xeno_Chr) == 0) {
        *failure_result = 0;
        return;
    }

    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(chr_peer_string, -1), 0);
    peer_value = (void *)(object + peer_field->offset);
    peer = peer_value->actor;
    state = &actSequence[peer->number].state;
    mode = arguments->mode;
    state->cleared_on_init = mode;
    switch (mode) {
    case 0:
        peer->update = ACT_updateDefault;
        break;
    case 4:
        state->cleared_on_init = 4;
        peer->update = ACT_updateSequence;
        state->state_flags = 0;
        return;
    /* Mode 1 runs a script method on the Chr's thread. */
    case 1: {
        DispatchTable *class_ref = ((ObjectHeader *)object)->classPointer;
        ChrStartString *method_name = arguments->method_name;
        JavaClass *scene_class;
        ChrStartStringValue *value;
        JavaMethod *method;
        JThread *chr_thread;

        scene_class = class_ref->clazz;
        CLEAR_SEQUENCE_STATE_FLAGS(state);
        if (SCRIPT_methodClearGet() != 0) {
            state->handler[0] = 0;
            state->handler[1] = 0;
            peer->flags &= ~CHR_ACTOR_BUSY;
            state->handler[2] = 0;
            state->flags = 0;
            state->handler[3] = 0;
        }
        value = method_name->value;
        method = findMethod(scene_class,
                            loadConstString(value->data, value->length),
                            TYPE_Void);
        state->cleared_on_init_run[0] = method;
        chr_thread = JTHREAD_get(object);
        if (chr_thread != 0) {
            chr_thread->method = method;
            chr_thread->reset = JTHREAD_defaultChr;
            JNI_initThread(chr_thread);
            chr_thread->flags |= 0x10;
        }
        state->cleared_on_init = 1;
        peer->update = ACT_updateSequence;
        state->handler[1] = 0;
        state->handler[2] = 0;
        state->handler[3] = 0;
        state->handler[0] = 0;
        return;
    }
    case 2:
        state->cleared_on_init = 2;
        peer->update = ACT_updatePlayer;
        break;
    case 3:
        break;
    case 5:
        state->state_flags = 0;
        state->flags = 0;
        peer->update = ACT_updateMPack;
        break;
    }
}

/*
 * Stops the Chr: clears the 0x10 flag of its thread and copies the actor's
 * position and rotation into the translate and rotate fields.
 */
void Java_xeno_Chr_stop__(JThread *thread, ChrObjectCall *arguments,
                          u32 *failure_result)
{
    JavaField *field;
    Actor *peer;
    JThread *chr_thread;
    SceneObject object;

    object = arguments->object;
    if (JNI_isInstanceOf(object, classJava_xeno_Chr) == 0) {
        *failure_result = 0;
        return;
    }
    field = lookupClassField(classJava_xeno_Chr,
                             loadConstString(chr_peer_string, -1), 0);
    peer = *(Actor **)(object + field->offset);
    chr_thread = JTHREAD_get(object);
    if (chr_thread != 0) {
        chr_thread->flags &= ~0x10;
    }
    field = lookupClassField(classJava_xeno_Chr,
                             loadConstString(chrTranslateXKey, -1), 0);
    *(float *)(object + field->offset) = peer->position.x;
    field = lookupClassField(classJava_xeno_Chr,
                             loadConstString(chrTranslateYKey, -1), 0);
    *(float *)(object + field->offset) = peer->position.y;
    field = lookupClassField(classJava_xeno_Chr,
                             loadConstString(chrTranslateZKey, -1), 0);
    *(float *)(object + field->offset) = peer->position.z;
    field = lookupClassField(classJava_xeno_Chr,
                             loadConstString(chrRotateXKey, -1), 0);
    *(float *)(object + field->offset) = peer->rotation.x;
    field = lookupClassField(classJava_xeno_Chr,
                             loadConstString(chrRotateYKey, -1), 0);
    *(float *)(object + field->offset) = peer->rotation.y;
    field = lookupClassField(classJava_xeno_Chr,
                             loadConstString(chrRotateZKey, -1), 0);
    *(float *)(object + field->offset) = peer->rotation.z;
}

/*
 * Queues a motion on the Chr's actor. Mode 0 passes the motion, flags, speed
 * and wait; mode 1 also passes the loop range and blend. Waits on the thread
 * when wait is set.
 */
static void CHR_motion(int mode, JThread *thread, ChrMotionCall *arguments,
                       u32 *failure_result)
{
    ChrFieldValue *field_value;
    JavaField *peer_field;
    Actor *peer;
    ActSequenceEntry *entry;
    SequenceState *sequence;
    MotionTrack *track;
    SceneObject object;
    int wait;
    int number;

    wait = 0;
    object = arguments->object;
    if (JNI_isInstanceOf(object, classJava_xeno_Chr) == 0) {
        *failure_result = 0;
        return;
    }
    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(chr_peer_string, -1), 0);
    field_value = (void *)(object + peer_field->offset);
    peer = field_value->actor;
    entry = &actSequence[((u8 *)peer)[ACTOR_SLOT_NUMBER_OFFSET]];
    sequence = &entry->state;
    sequence->handler[2] = SEQ_motion;
    sequence->state_flags |= CHR_CHANNEL_MOTION;
    track = &entry->motion;

    switch (mode) {
    case 0:
        number = arguments->argument[0].integer;
        if (number & CHR_MOTION_ABSOLUTE_MASK)
            track->motion = number;
        else
            track->motion = number - 1;
        track->flags = arguments->argument[1].integer;
        track->speed = arguments->argument[2].floating;
        wait = arguments->argument[3].byte;
        track->loop_start = -1;
        track->loop_end = -1;
        track->blend = CHR_MOTION_DEFAULT_BLEND;
        break;
    case 1:
        number = arguments->argument[0].integer;
        if (number & CHR_MOTION_ABSOLUTE_MASK)
            track->motion = number;
        else
            track->motion = number - 1;
        track->loop_start = arguments->argument[1].half;
        track->loop_end = arguments->argument[2].half;
        track->blend = arguments->argument[3].byte;
        track->flags = arguments->argument[4].integer;
        track->speed = arguments->argument[5].floating;
        wait = arguments->argument[6].byte;
        break;
    }

    if (wait == 1) {
        peer->flags |= CHR_ACTOR_BUSY;
        if (thread->kind != wait && (thread->flags & 0x40) == 0)
            return;
        if ((thread->flags & 0x40) != 0) {
            thread->wait_target.object = peer;
            thread->wait_kind = JTHREAD_WAIT_ACTOR;
            thread->resume_frames = thread->frame_depth;
            thread->flags |= 4;
        }
        thread->resume_frames = thread->frame_depth;
        thread->flags |= 1;
    }
}

void Java_xeno_Chr_mtn__IIIIIFZ(JThread *thread, ChrMotionCall *arguments,
                                u32 *failure_result)
{
    CHR_motion(1, thread, arguments, failure_result);
}

void Java_xeno_Chr_mtn__IIFZ(JThread *thread, ChrMotionCall *arguments,
                             u32 *failure_result)
{
    CHR_motion(0, thread, arguments, failure_result);
}

void Java_xeno_Chr_signal__I(JThread *thread, ChrSignalCall *arguments,
                             u32 *failure_result)
{
    ChrFieldValue *field_value;
    JavaField *peer_field;
    Actor *peer;
    SceneObject object;

    object = arguments->object;
    if (JNI_isInstanceOf(object, classJava_xeno_Chr) == 0) {
        *failure_result = 0;
        return;
    }
    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(chr_peer_string, -1), 0);
    field_value = (void *)(object + peer_field->offset);
    peer = field_value->actor;
    peer->signal = arguments->value;
}

void Java_xeno_Chr_getSignal__(JThread *thread, ChrObjectCall *arguments,
                               u32 *failure_result)
{
    ChrFieldValue *field_value;
    JavaField *peer_field;
    Actor *peer;
    SceneObject object;

    object = arguments->object;
    if (JNI_isInstanceOf(object, classJava_xeno_Chr) == 0) {
        *failure_result = 0;
        return;
    }
    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(chr_peer_string, -1), 0);
    field_value = (void *)(object + peer_field->offset);
    peer = field_value->actor;
    *failure_result = peer->signal;
}

void Java_xeno_Chr_setRotate__(JThread *thread, ChrObjectCall *arguments)
{
    JavaField *field;
    Actor *peer;
    Vector4 *rotation;
    SceneObject object;

    object = arguments->object;
    field = lookupClassField(classJava_xeno_Chr,
                             loadConstString(chr_peer_string, -1), 0);
    peer = *(Actor **)(object + field->offset);
    rotation = &peer->rotation;

    field = lookupClassField(classJava_xeno_Chr,
                             loadConstString(chrRotateXKey, -1), 0);
    rotation->x = *(float *)(object + field->offset) / 180.0f * 3.1415927f;
    field = lookupClassField(classJava_xeno_Chr,
                             loadConstString(chrRotateYKey, -1), 0);
    rotation->y = *(float *)(object + field->offset) / 180.0f * 3.1415927f;
    field = lookupClassField(classJava_xeno_Chr,
                             loadConstString(chrRotateZKey, -1), 0);
    rotation->z = *(float *)(object + field->offset) / 180.0f * 3.1415927f;
}

/*
 * Copies the Chr's translate fields into the actor's position and sets status
 * bit 0x1000.
 */
void Java_xeno_Chr_setTranslate__(JThread *thread, ChrObjectCall *arguments)
{
    JavaField *field;
    Actor *peer;
    Vector4 *position;
    SceneObject object;
    float z;

    object = arguments->object;
    field = lookupClassField(classJava_xeno_Chr,
                             loadConstString(chr_peer_string, -1), 0);
    peer = *(Actor **)(object + field->offset);
    position = &peer->position;

    field = lookupClassField(classJava_xeno_Chr,
                             loadConstString(chrTranslateXKey, -1), 0);
    position->x = *(float *)(object + field->offset);
    field = lookupClassField(classJava_xeno_Chr,
                             loadConstString(chrTranslateYKey, -1), 0);
    position->y = *(float *)(object + field->offset);
    field = lookupClassField(classJava_xeno_Chr,
                             loadConstString(chrTranslateZKey, -1), 0);
    z = *(float *)(object + field->offset);

    peer->status_flags |= 0x1000;
    position->z = z;
    peer->translate_y = position->y;
    if (((u8 *)peer)[ACTOR_TRANSLATE_STATE_OFFSET] & 1) {
        ((u8 *)peer)[ACTOR_TRANSLATE_STATE_OFFSET] = 2;
    }
}

void Java_xeno_Chr_getRotate__(JThread *thread, ChrScaleCall *arguments,
                               u32 *failure_result)
{
    JavaField *field;
    Actor *peer;
    Vector4 *rotation;
    SceneObject object;

    object = arguments->object;
    field = lookupClassField(classJava_xeno_Chr,
                             loadConstString(chr_peer_string, -1), 0);
    peer = *(Actor **)(object + field->offset);
    rotation = &peer->rotation;

    field = lookupClassField(classJava_xeno_Chr,
                             loadConstString(chrRotateXKey, -1), 0);
    *(float *)(object + field->offset) = rotation->x / 3.1415927f * 180.0f;
    field = lookupClassField(classJava_xeno_Chr,
                             loadConstString(chrRotateYKey, -1), 0);
    *(float *)(object + field->offset) = rotation->y / 3.1415927f * 180.0f;
    field = lookupClassField(classJava_xeno_Chr,
                             loadConstString(chrRotateZKey, -1), 0);
    *(float *)(object + field->offset) = rotation->z / 3.1415927f * 180.0f;
}

void Java_xeno_Chr_getTranslate__(JThread *thread, ChrScaleCall *arguments,
                                  u32 *failure_result)
{
    JavaField *field;
    Actor *peer;
    Vector4 *position;
    SceneObject object;

    /* Each Java field has a runtime class offset, rather than a fixed C member. */
    object = arguments->object;
    field = lookupClassField(classJava_xeno_Chr,
                             loadConstString(chr_peer_string, -1), 0);
    peer = *(Actor **)(object + field->offset);
    position = &peer->position;

    field = lookupClassField(classJava_xeno_Chr,
                             loadConstString(chrTranslateXKey, -1), 0);
    *(float *)(object + field->offset) = position->x;
    field = lookupClassField(classJava_xeno_Chr,
                             loadConstString(chrTranslateYKey, -1), 0);
    *(float *)(object + field->offset) = position->y;
    field = lookupClassField(classJava_xeno_Chr,
                             loadConstString(chrTranslateZKey, -1), 0);
    *(float *)(object + field->offset) = position->z;
}

void Java_xeno_Chr_setVisible__Z(JThread *thread, ChrBoolCall *arguments)
{
    ChrFieldValue *field_value;
    JavaField *peer_field;
    Actor *peer;
    SceneObject object;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(chr_peer_string, -1), 0);
    field_value = (void *)(object + peer_field->offset);
    peer = field_value->actor;
    if (arguments->flag != 0) {
        peer->flags &= ~CHR_ACTOR_HIDDEN;
    } else {
        peer->flags |= CHR_ACTOR_HIDDEN;
    }
}

void Java_xeno_Chr_setVisible__IZ(JThread *thread, ChrVisibleCall *arguments)
{
    ChrFieldValue *field_value;
    JavaField *peer_field;
    Actor *peer;
    SceneObject object;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(chr_peer_string, -1), 0);
    field_value = (void *)(object + peer_field->offset);
    peer = field_value->actor;
    ACT_setVisible(peer, arguments->part, arguments->visible);
}

void Java_xeno_Chr_setCollision__Z(JThread *thread, ChrBoolCall *arguments)
{
    ChrFieldValue *field_value;
    JavaField *peer_field;
    Actor *peer;
    SceneObject object;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(chr_peer_string, -1), 0);
    field_value = (void *)(object + peer_field->offset);
    peer = field_value->actor;
    if (arguments->flag != 0) {
        peer->flags |= CHR_ACTOR_COLLISION;
    } else {
        peer->flags &= ~CHR_ACTOR_COLLISION;
    }
}

void Java_xeno_Chr_setHand__I(JThread *thread, ChrScaleCall *arguments)
{
    ChrFieldValue *field_value;
    JavaField *peer_field;
    Actor *peer;
    SceneObject object;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(chr_peer_string, -1), 0);
    field_value = (void *)(object + peer_field->offset);
    peer = field_value->actor;
    ACT_setHand(peer, arguments->first.integer);
}

/* Copies count bytes from src to dst. */
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

/*
 * Copies size bytes of value into the actor's argument area at offset. Sizes
 * outside 1 to 4 write nothing.
 */
void Java_xeno_Chr_setArgs__III(JThread *thread, ChrArgsWordCall *arguments)
{
    JavaField *peer_field;
    Actor *peer;
    int value;
    SceneObject object;
    int offset;
    int size;

    value = arguments->value;
    object = arguments->object;
    offset = arguments->offset;
    size = arguments->size;
    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(chr_peer_string, -1), 0);
    peer = *(Actor **)(object + peer_field->offset);
    if ((unsigned int)(size - 1) < 4U) {
        copyArgs(ACTOR_ARGS(peer) + offset, (u8 *)&value, size);
    }
}

/*
 * Reads size bytes (1 to 4) from the actor's argument area at offset into
 * *result. Other sizes leave *result unchanged.
 */
void Java_xeno_Chr_getArgs__II(JThread *thread, ChrArgsReadCall *arguments,
                               int *result)
{
    JavaField *peer_field;
    Actor *peer;
    int value;
    SceneObject object;
    int offset;
    int size;

    object = arguments->object;
    offset = arguments->offset;
    size = arguments->size;
    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(chr_peer_string, -1), 0);
    peer = *(Actor **)(object + peer_field->offset);
    if ((unsigned int)(size - 1) < 4U) {
        copyArgs((u8 *)&value, ACTOR_ARGS(peer) + offset, size);
        *result = value;
    }
}

/*
 * Stores the source call's second and first integers as two words at offset
 * in the actor's argument area.
 */
void Java_xeno_Chr_setArgs__ILjava_lang_Object_I(JThread *thread,
                                                 ChrArgsObjectCall *arguments)
{
    ChrFieldValue *field_value;
    JavaField *peer_field;
    Actor *peer;
    ChrScaleCall *source;
    int offset;
    SceneObject object;
    int second;
    int first;

    source = arguments->source;
    offset = arguments->offset;
    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(chr_peer_string, -1), 0);
    second = source->second.integer;
    field_value = (void *)(object + peer_field->offset);
    peer = field_value->actor;
    first = source->first.integer;
    ((int *)(ACTOR_ARGS(peer) + offset))[0] = second;
    ((int *)(ACTOR_ARGS(peer) + offset))[1] = first;
}

void Java_xeno_Chr_getSerial__(JThread *thread, ChrScaleCall *arguments,
                               unsigned int *result)
{
    ChrFieldValue *field_value;
    JavaField *peer_field;
    Actor *peer;
    SceneObject object;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(chr_peer_string, -1), 0);
    field_value = (void *)(object + peer_field->offset);
    peer = field_value->actor;
    result[0] = ((u8 *)peer)[ACTOR_SLOT_NUMBER_OFFSET];
}

void Java_xeno_Chr_getState__(JThread *thread, ChrObjectCall *arguments,
                              u32 *failure_result)
{
    ChrFieldValue *field_value;
    JavaField *peer_field;
    Actor *peer;
    unsigned char slot;
    SceneObject object;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(chr_peer_string, -1), 0);
    field_value = (void *)(object + peer_field->offset);
    peer = field_value->actor;
    slot = ((u8 *)peer)[ACTOR_SLOT_NUMBER_OFFSET];
    *failure_result = actSequence[slot].state.state_flags;
}

/* Resolves the peer field descriptor and discards it; no motion root is written. */
void Java_xeno_Chr_mtnGetRoot__ILxeno_util_Vector4f_(JThread *thread,
                                                     ChrMotionRootCall *arguments,
                                                     ChrResultValue *result)
{
    CHR_RESOLVE_PEER_DESCRIPTOR();
}

void Java_xeno_Chr_setScale__FFF(JThread *thread, ChrVector3Call *arguments)
{
    ChrFieldValue *field_value;
    JavaField *peer_field;
    Actor *peer;
    SceneObject object;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(chr_peer_string, -1), 0);
    field_value = (void *)(object + peer_field->offset);
    peer = field_value->actor;
    peer->scale.x = arguments->x;
    peer->scale.y = arguments->y;
    peer->scale.z = arguments->z;
}

void Java_xeno_Chr_getScale__(JThread *thread, ChrObjectCall *arguments,
                              ChrResultValue *result)
{
    ChrFieldValue *field_value;
    static ChrScaleVector scale;
    JavaField *peer_field;
    Actor *peer;
    SceneObject object;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(chr_peer_string, -1), 0);
    field_value = (void *)(object + peer_field->offset);
    peer = field_value->actor;
    scale.class_ref = classJava_xeno_util_Vector4f->classPointer;
    scale.x = peer->scale.x;
    scale.y = peer->scale.y;
    scale.z = peer->scale.z;
    scale.w = peer->scale.w;
    result->vector = &scale;
}

/* Starts a spline scale of the Chr's actor on all scale axes; waits when wait is set. */
void Java_xeno_Chr_scale__Lxeno_util_Spline_IZ(JThread *thread, ChrSplineCall *arguments,
                                              u32 *failure_result)
{
    ChrFieldValue *field_value;
    JavaField *peer_field;
    Actor *peer;
    ActSequenceEntry *entry;
    SequenceState *sequence;
    SplineTrack *track;
    SceneObject object;
    ChrSpline *spline;
    int wait;

    object = arguments->object;
    if (JNI_isInstanceOf(object, classJava_xeno_Chr) == 0) {
        *failure_result = 0;
        return;
    }
    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(chr_peer_string, -1), 0);
    field_value = (void *)(object + peer_field->offset);
    peer = field_value->actor;
    entry = &actSequence[((u8 *)peer)[ACTOR_SLOT_NUMBER_OFFSET]];
    sequence = &entry->state;
    track = &entry->scale_channel.spline;
    sequence->channel_mask = CHR_CHANNEL_SCALE_ALL;
    sequence->state_flags |= CHR_CHANNEL_SCALE_ALL;
    lookupClassField(classJava_xeno_Chr,
                     loadConstString(chr_algorithm_string, -1), 0);
    spline = arguments->spline;
    wait = arguments->wait;
    sequence->handler[3] = SEQ_scaleSPL;
    track->spline = spline;
    track->last_frame = spline->last_frame;
    if (wait == 1) {
        peer->flags |= CHR_ACTOR_BUSY;
        if (thread->kind != wait && (thread->flags & 0x40) == 0)
            return;
        if ((thread->flags & 0x40) != 0) {
            thread->wait_target.object = peer;
            thread->wait_kind = JTHREAD_WAIT_ACTOR;
            thread->resume_frames = thread->frame_depth;
            thread->flags |= 4;
        }
        thread->resume_frames = thread->frame_depth;
        thread->flags |= 1;
    }
}

/*
 * Sets the X scale of the Chr's actor for the given mode. The goal is offset
 * by the actor's current scale when the algorithm flag bit is set. Waits on
 * the thread when wait is set.
 */
static void CHR_sclX(int mode, JThread *thread, ChrScaleCall *arguments,
                     u32 *failure_result)
{
    JavaField *peer_field;
    JavaField *algorithm_field;
    Actor *peer;
    float *scale_source;
    SequenceState *sequence;
    ActSequenceEntry *entry;
    ScaleTrack *scale;
    SceneObject object;
    int wait;

    wait = 0;
    object = arguments->object;
    if (JNI_isInstanceOf(object, classJava_xeno_Chr) == 0) {
        *failure_result = 0;
        return;
    }

    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(chr_peer_string, -1), 0);
    /*
     * `peer` and `algorithm` are fields of the Java class xeno.Chr, so their
     * position inside the object is the offset the JVM hands back at run time
     * and not a layout this file can name.
     */
    peer = *(Actor **)(object + peer_field->offset);
    algorithm_field = lookupClassField(
        classJava_xeno_Chr, loadConstString(chr_algorithm_string, -1), 0);

    scale_source = &peer->scale.x;
    if ((*(u32 *)(object + algorithm_field->offset) & 1) == 0)
        scale_source = defaultOffset;

    entry = &actSequence[((u8 *)peer)[ACTOR_SLOT_NUMBER_OFFSET]];
    sequence = &entry->state;
    SEQUENCE_SCALE_HANDLER(sequence) = SEQ_scale;
    sequence->state_flags |= CHR_CHANNEL_SCALE_X;
    scale = &entry->scale_channel.scale;

    /* Mode 0 runs for a frame count; mode 1 scales at a rate per second. */
    switch (mode) {
    case 0:
        scale->duration[0] = arguments->first.integer;
        scale->goal[0] = arguments->second.floating + scale_source[0];
        wait = arguments->wait;
        break;
    case 1:
        scale->duration[0] = -1;
        scale->goal[0] = arguments->first.floating + scale_source[0];
        scale->step[0] = arguments->second.floating / CHR_FRAMES_PER_SECOND;
        wait = arguments->wait;
        break;
    }

    if (wait == 1) {
        peer->flags |= CHR_ACTOR_BUSY;
        /*
         * `wait` is 1 here, and kind 1 is the thread getPeer_Chr creates for a
         * character: a blocking sclX only suspends the character's own thread,
         * or a thread carrying flag 0x40.
         */
        if (thread->kind != wait && (thread->flags & 0x40) == 0)
            return;
        if ((thread->flags & 0x40) != 0) {
            thread->wait_target.object = peer;
            thread->wait_kind = JTHREAD_WAIT_ACTOR;
            thread->resume_frames = thread->frame_depth;
            thread->flags |= 4;
        }
        thread->resume_frames = thread->frame_depth;
        thread->flags |= 1;
    }
}

void Java_xeno_Chr_sclX__FFZ(JThread *thread, ChrScaleCall *arguments,
                             u32 *failure_result)
{
    CHR_sclX(1, thread, arguments, failure_result);
}

void Java_xeno_Chr_sclX__IFZ(JThread *thread, ChrScaleCall *arguments,
                             u32 *failure_result)
{
    CHR_sclX(0, thread, arguments, failure_result);
}

static void CHR_sclY(int mode, JThread *thread, ChrScaleCall *arguments,
                     u32 *failure_result)
{
    JavaField *peer_field;
    JavaField *algorithm_field;
    Actor *peer;
    float *scale_source;
    SequenceState *sequence;
    ActSequenceEntry *entry;
    ScaleTrack *scale;
    SceneObject object;
    int wait;

    wait = 0;
    object = arguments->object;
    if (JNI_isInstanceOf(object, classJava_xeno_Chr) == 0) {
        *failure_result = 0;
        return;
    }

    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(chr_peer_string, -1), 0);
    /*
     * `peer` and `algorithm` are fields of the Java class xeno.Chr, so their
     * position inside the object is the offset the JVM hands back at run time
     * and not a layout this file can name.
     */
    peer = *(Actor **)(object + peer_field->offset);
    algorithm_field = lookupClassField(
        classJava_xeno_Chr, loadConstString(chr_algorithm_string, -1), 0);

    scale_source = &peer->scale.x;
    if ((*(u32 *)(object + algorithm_field->offset) & 1) == 0)
        scale_source = defaultOffset;

    entry = &actSequence[((u8 *)peer)[ACTOR_SLOT_NUMBER_OFFSET]];
    sequence = &entry->state;
    SEQUENCE_SCALE_HANDLER(sequence) = SEQ_scale;
    sequence->state_flags |= CHR_CHANNEL_SCALE_Y;
    scale = &entry->scale_channel.scale;

    switch (mode) {
    case 0:
        scale->duration[1] = arguments->first.integer;
        scale->goal[1] = arguments->second.floating + scale_source[1];
        wait = arguments->wait;
        break;
    case 1:
        scale->duration[1] = -1;
        scale->goal[1] = arguments->first.floating + scale_source[1];
        scale->step[1] = arguments->second.floating / CHR_FRAMES_PER_SECOND;
        wait = arguments->wait;
        break;
    }

    if (wait == 1) {
        peer->flags |= CHR_ACTOR_BUSY;
        /*
         * `wait` is 1 here, and kind 1 is the thread getPeer_Chr creates for a
         * character: a blocking sclY only suspends the character's own thread,
         * or a thread carrying flag 0x40.
         */
        if (thread->kind != wait && (thread->flags & 0x40) == 0)
            return;
        if ((thread->flags & 0x40) != 0) {
            thread->wait_target.object = peer;
            thread->wait_kind = JTHREAD_WAIT_ACTOR;
            thread->resume_frames = thread->frame_depth;
            thread->flags |= 4;
        }
        thread->resume_frames = thread->frame_depth;
        thread->flags |= 1;
    }
}

void Java_xeno_Chr_sclY__FFZ(JThread *thread, ChrScaleCall *arguments,
                             u32 *failure_result)
{
    CHR_sclY(1, thread, arguments, failure_result);
}

void Java_xeno_Chr_sclY__IFZ(JThread *thread, ChrScaleCall *arguments,
                             u32 *failure_result)
{
    CHR_sclY(0, thread, arguments, failure_result);
}

static void CHR_sclZ(int mode, JThread *thread, ChrScaleCall *arguments,
                     u32 *failure_result)
{
    JavaField *peer_field;
    JavaField *algorithm_field;
    Actor *peer;
    float *scale_source;
    SequenceState *sequence;
    ActSequenceEntry *entry;
    ScaleTrack *scale;
    SceneObject object;
    int wait;

    wait = 0;
    object = arguments->object;
    if (JNI_isInstanceOf(object, classJava_xeno_Chr) == 0) {
        *failure_result = 0;
        return;
    }

    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(chr_peer_string, -1), 0);
    /*
     * `peer` and `algorithm` are fields of the Java class xeno.Chr, so their
     * position inside the object is the offset the JVM hands back at run time
     * and not a layout this file can name.
     */
    peer = *(Actor **)(object + peer_field->offset);
    algorithm_field = lookupClassField(
        classJava_xeno_Chr, loadConstString(chr_algorithm_string, -1), 0);

    scale_source = &peer->scale.x;
    if ((*(u32 *)(object + algorithm_field->offset) & 1) == 0)
        scale_source = defaultOffset;

    entry = &actSequence[((u8 *)peer)[ACTOR_SLOT_NUMBER_OFFSET]];
    sequence = &entry->state;
    SEQUENCE_SCALE_HANDLER(sequence) = SEQ_scale;
    sequence->state_flags |= CHR_CHANNEL_SCALE_Z;
    scale = &entry->scale_channel.scale;

    switch (mode) {
    case 0:
        scale->duration[2] = arguments->first.integer;
        scale->goal[2] = arguments->second.floating + scale_source[2];
        wait = arguments->wait;
        break;
    case 1:
        scale->duration[2] = -1;
        scale->goal[2] = arguments->first.floating + scale_source[2];
        scale->step[2] = arguments->second.floating / CHR_FRAMES_PER_SECOND;
        wait = arguments->wait;
        break;
    }

    if (wait == 1) {
        peer->flags |= CHR_ACTOR_BUSY;
        /*
         * `wait` is 1 here, and kind 1 is the thread getPeer_Chr creates for a
         * character: a blocking sclZ only suspends the character's own thread,
         * or a thread carrying flag 0x40.
         */
        if (thread->kind != wait && (thread->flags & 0x40) == 0)
            return;
        if ((thread->flags & 0x40) != 0) {
            thread->wait_target.object = peer;
            thread->wait_kind = JTHREAD_WAIT_ACTOR;
            thread->resume_frames = thread->frame_depth;
            thread->flags |= 4;
        }
        thread->resume_frames = thread->frame_depth;
        thread->flags |= 1;
    }
}

void Java_xeno_Chr_sclZ__FFZ(JThread *thread, ChrScaleCall *arguments,
                             u32 *failure_result)
{
    CHR_sclZ(1, thread, arguments, failure_result);
}

void Java_xeno_Chr_sclZ__IFZ(JThread *thread, ChrScaleCall *arguments,
                             u32 *failure_result)
{
    CHR_sclZ(1, thread, arguments, failure_result);
}

void Java_xeno_Chr_rotCNS__ILjava_lang_Object_(JThread *thread,
                                               ChrScaleCall *arguments,
                                               u32 *failure_result)
{
}

void Java_xeno_Chr_rotCNS__IFFF(JThread *thread, ChrScaleCall *arguments,
                                u32 *failure_result)
{
    ChrFieldValue *field_value;
    JavaField *peer_field;
    Actor *peer;

    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(chr_peer_string, -1), 0);
    field_value = (void *)(arguments->object + peer_field->offset);
    peer = field_value->actor;
}

void Java_xeno_Chr_setRotCNSParam__IFFFFF(JThread *thread,
                                          ChrScaleCall *arguments,
                                          u32 *failure_result)
{
    ChrFieldValue *field_value;
    JavaField *peer_field;
    Actor *peer;

    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(chr_peer_string, -1), 0);
    field_value = (void *)(arguments->object + peer_field->offset);
    peer = field_value->actor;
}

void Java_xeno_Chr_relax__II(JThread *thread, ChrScaleCall *arguments,
                             u32 *failure_result)
{
    ChrFieldValue *field_value;
    JavaField *peer_field;
    Actor *peer;

    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(chr_peer_string, -1), 0);
    field_value = (void *)(arguments->object + peer_field->offset);
    peer = field_value->actor;
}

void Java_xeno_Chr_getFlags__(JThread *thread, ChrScaleCall *arguments,
                              unsigned int *result)
{
    ChrFieldValue *field_value;
    JavaField *peer_field;
    Actor *peer;

    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(chr_peer_string, -1), 0);
    field_value = (void *)(arguments->object + peer_field->offset);
    peer = field_value->actor;
    result[0] = peer->flags;
}

void Java_xeno_Chr_setFlags__I(JThread *thread, ChrScaleCall *arguments)
{
    ChrFieldValue *field_value;
    JavaField *peer_field;
    Actor *peer;

    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(chr_peer_string, -1), 0);
    field_value = (void *)(arguments->object + peer_field->offset);
    peer = field_value->actor;
    peer->flags = arguments->first.integer;
}

void Java_xeno_Chr_setEdgeFall__I(JThread *thread, ChrIntCall *arguments)
{
    ChrFieldValue *field_value;
    JavaField *peer_field;
    Actor *peer;

    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(chr_peer_string, -1), 0);
    field_value = (void *)(arguments->object + peer_field->offset);
    peer = field_value->actor;
    if (arguments->value == 0) {
        /* Also clears every other status_flags bit: the mask is 8, not ~8. */
        peer->status_flags &= CHR_STATUS_EDGE_FALL;
    } else {
        peer->status_flags |= CHR_STATUS_EDGE_FALL;
    }
}

void Java_xeno_Chr_setShadow__II(JThread *thread, ChrShadowCall *arguments)
{
    ChrFieldValue *field_value;
    JavaField *peer_field;
    Actor *peer;

    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(chr_peer_string, -1), 0);
    field_value = (void *)(arguments->object + peer_field->offset);
    peer = field_value->actor;
    peer->flags |= 0x20;
    peer->shadow_kind = arguments->kind;
    peer->shadowSize = arguments->size;
}

/*
 * Selects an unused DropShadowDefineTable entry (key 0xFFFF0000), tags it with
 * the actor's inUseId and copies the shadow byte tokens into it up to the -1
 * terminator. Hits DEBUG_BREAK() when no entry is selected.
 */
void Java_xeno_Chr_setShadow__aB(JThread *thread,
                                 struct ChrShadowBytesCall *arguments)
{
    ChrFieldValue *field_value;
    JavaField *peer_field;
    Actor *peer;
    struct ChrDropShadowEntry *entry;
    struct ChrDropShadowEntry *selected;
    const signed char *source;
    unsigned char *destination;
    unsigned short shadow_flags;

    (void)thread;
    source = arguments->bytes->bytes;
    destination = 0;
    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(chr_peer_string, -1), 0);
    field_value = (void *)(arguments->object + peer_field->offset);
    peer = field_value->actor;
    shadow_flags = peer->inUseId;
    selected = 0;
    for (entry = DropShadowDefineTable; entry->tokens != 0; entry++) {
        if (entry->match.fields.mask == 0xFFFF &&
            entry->match.fields.value == shadow_flags) {
            break;
        }
        if (entry->match.key == 0xFFFF0000) {
            selected = entry;
        }
    }
    if (selected == 0) {
        DEBUG_BREAK();
    } else {
        selected->match.fields.value = shadow_flags;
        selected->match.fields.mask = 0xFFFF;
        destination = selected->tokens;
    }

    if (*source != -1) {
        int terminator = -1;
        do {
            destination[0] = (unsigned char)source[0];
            destination[1] = (unsigned char)source[1];
            destination[2] = (unsigned char)source[2];
            source += 3;
            destination += 3;
        } while (source[-3] != terminator || *source != terminator);
    }
    destination[0] = (unsigned char)source[0];
    destination[1] = (unsigned char)source[1];
    destination[2] = (unsigned char)source[2];
}

void Java_xeno_Chr_setID__I(JThread *thread, ChrScaleCall *arguments)
{
    ChrFieldValue *field_value;
    JavaField *peer_field;
    Actor *peer;

    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(chr_peer_string, -1), 0);
    field_value = (void *)(arguments->object + peer_field->offset);
    peer = field_value->actor;
    peer->data_header = UnduDataGetHeader(0, arguments->first.integer);
    peer->status_flags |= 0x1000;
}

void Java_xeno_Chr_setElevatorMode__I(JThread *thread, ChrIntCall *arguments)
{
    ChrFieldValue *field_value;
    JavaField *peer_field;
    Actor *peer;

    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(chr_peer_string, -1), 0);
    field_value = (void *)(arguments->object + peer_field->offset);
    peer = field_value->actor;
    if (arguments->value == 0) {
        /* Also clears every other status_flags bit: the mask is 0x20, not ~0x20. */
        peer->status_flags &= CHR_STATUS_ELEVATOR;
    } else {
        peer->status_flags |= CHR_STATUS_ELEVATOR;
    }
}

/*
 * Links the Chr's actor to the other Chr's actor: as a relation when the joint
 * has CHR_JOINT_RELATION set, otherwise as a parent.
 */
void Java_xeno_Chr_setParent__Lxeno_Chr_III(JThread *thread,
                                            ChrParentCall *arguments)
{
    JavaField *peer_field;
    Actor *peer;
    Actor *other;
    SceneObject object;
    int joint;
    int type;
    int id;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(chr_peer_string, -1), 0);
    joint = arguments->joint;
    id = arguments->id;
    peer = *(Actor **)(object + peer_field->offset);
    type = arguments->type;
    object = arguments->other;
    other = *(Actor **)(object + peer_field->offset);
    if (joint & CHR_JOINT_RELATION) {
        ACT_setRelation(peer, other, joint, id);
    } else {
        ACT_setParent(peer, type, other, joint, id);
    }
}

void Java_xeno_Chr_setMotionFlags__IZ(JThread *thread,
                                      ChrMotionFlagsCall *arguments)
{
    ChrFieldValue *field_value;
    JavaField *peer_field;
    Actor *peer;
    SceneObject object;
    int mask;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(chr_peer_string, -1), 0);
    mask = arguments->mask;
    field_value = (void *)(object + peer_field->offset);
    peer = field_value->actor;
    if (arguments->enabled != 0) {
        peer->animation.slot.flags |= mask;
    } else {
        peer->animation.slot.flags &= ~mask;
    }
}

/*
 * Sets the actor's draw filter for the selector and clears its point light
 * flag. result receives the word for the selector.
 */
void Java_xeno_Chr_setFilter__I(JThread *thread, ChrIntCall *arguments,
                                ChrResultValue *result)
{
    ChrFieldValue *field_value;
    JavaField *peer_field;
    Actor *peer;
    SceneObject object;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(chr_peer_string, -1), 0);
    field_value = (void *)(object + peer_field->offset);
    peer = field_value->actor;
    switch (arguments->value) {
    case 0:
        peer->draw = 0;
        peer->flags &= ~CHR_ACTOR_POINT_LIGHT;
        result->word = -1;
        break;
    case 1:
        peer->flags &= ~CHR_ACTOR_POINT_LIGHT;
        result->word = -1;
        break;
    case CHR_FILTER_GUNO:
        peer->flags &= ~CHR_ACTOR_POINT_LIGHT;
        peer->draw = ACT_filterGuno;
        result->word = 4;
        break;
    case CHR_FILTER_STEALTH:
        peer->flags &= ~CHR_ACTOR_POINT_LIGHT;
        peer->draw = ACT_filterStealth;
        result->word = 4;
        break;
    }
}

void Java_xeno_Chr_setFilterParam__aF(JThread *thread,
                                      ChrFilterParamCall *arguments)
{
    ChrFieldValue *field_value;
    JavaField *peer_field;
    Actor *peer;
    ChrFilterParamValue *value;
    SceneObject object;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(chr_peer_string, -1), 0);
    field_value = (void *)(object + peer_field->offset);
    peer = field_value->actor;
    value = arguments->array->value;
    peer->drawEnv.transparency = value->components[0];
    peer->drawEnv.filter_param[0] = value->components[3];
    peer->drawEnv.filter_param[1] = value->components[2];
    peer->drawEnv.filter_param[2] = value->components[1];
}

void Java_xeno_Chr_setClip__I(JThread *thread, ChrIntCall *arguments)
{
    ChrFieldValue *field_value;
    JavaField *peer_field;
    Actor *peer;
    SceneObject object;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(chr_peer_string, -1), 0);
    field_value = (void *)(object + peer_field->offset);
    peer = field_value->actor;
    if (arguments->value != 0) {
        peer->drawEnv.flags |= CHR_DRAW_CLIP;
    } else {
        peer->drawEnv.flags &= ~CHR_DRAW_CLIP;
    }
}

void Java_xeno_Chr_setSymmetryY__I(JThread *thread, ChrIntCall *arguments)
{
    ChrFieldValue *field_value;
    JavaField *peer_field;
    Actor *peer;
    SceneObject object;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(chr_peer_string, -1), 0);
    field_value = (void *)(object + peer_field->offset);
    peer = field_value->actor;
    if (arguments->value != 0) {
        peer->drawEnv.flags |= CHR_DRAW_SYMMETRY_Y;
    } else {
        peer->drawEnv.flags &= ~CHR_DRAW_SYMMETRY_Y;
    }
}

void Java_xeno_Chr_setSortOffset__F(JThread *thread, ChrScaleCall *arguments)
{
    ChrFieldValue *field_value;
    JavaField *peer_field;
    Actor *peer;
    SceneObject object;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(chr_peer_string, -1), 0);
    field_value = (void *)(object + peer_field->offset);
    peer = field_value->actor;
    peer->sortOffset = arguments->first.floating;
}

/*
 * Sets the point light colour at the index and enables the point light.
 * Indices outside 0 to 2 do nothing.
 */
void Java_xeno_Chr_setPointLightCol__IFFF(
    JThread *thread, struct ChrPointLightCall *arguments)
{
    ChrFieldValue *field_value;
    JavaField *peer_field;
    Actor *peer;
    SceneObject object;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(chr_peer_string, -1), 0);
    field_value = (void *)(object + peer_field->offset);
    peer = field_value->actor;
    if ((unsigned int)arguments->index.integer < CHR_POINT_LIGHT_COUNT_UNSIGNED) {
        u32 actor_flags;
        int blue_index;
        float blue_value;

        actor_flags = peer->flags;
        peer->flags = actor_flags | CHR_ACTOR_POINT_LIGHT;
        peer->drawEnv.lightColor[arguments->index.integer][0] = arguments->x.floating;
        peer->drawEnv.lightColor[arguments->index.integer][1] = arguments->y.floating;
        blue_index = arguments->index.integer;
        blue_value = arguments->z.floating;
        peer->drawEnv.flags |= CHR_DRAW_POINT_LIGHT;
        peer->drawEnv.lightColor[blue_index][2] = blue_value;
    }
}

/*
 * Sets the point light position at the index and enables the point light.
 * Indices outside 0 to 2 do nothing.
 */
void Java_xeno_Chr_setPointLightPos__IFFF(
    JThread *thread, struct ChrPointLightCall *arguments)
{
    ChrFieldValue *field_value;
    JavaField *peer_field;
    Actor *peer;
    SceneObject object;

    lookupClassField(classJava_xeno_Chr,
                     loadConstString(chr_peer_string, -1), 0);
    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(chr_peer_string, -1), 0);
    field_value = (void *)(object + peer_field->offset);
    peer = field_value->actor;
    if ((unsigned int)arguments->index.integer < CHR_POINT_LIGHT_COUNT_UNSIGNED) {
        u32 actor_flags;
        int blue_index;
        float blue_value;

        actor_flags = peer->flags;
        peer->flags = actor_flags | CHR_ACTOR_POINT_LIGHT;
        peer->drawEnv.lightPosition[arguments->index.integer][0] = arguments->x.floating;
        peer->drawEnv.lightPosition[arguments->index.integer][1] = arguments->y.floating;
        blue_index = arguments->index.integer;
        blue_value = arguments->z.floating;
        peer->drawEnv.flags |= CHR_DRAW_POINT_LIGHT;
        peer->drawEnv.lightPosition[blue_index][2] = blue_value;
    }
}

/* Clears the point light colours and the point light draw flag. */
void Java_xeno_Chr_setPointLightReset__(JThread *thread, ChrObjectCall *arguments)
{
    ChrFieldValue *field_value;
    JavaField *peer_field;
    Actor *peer;
    SceneObject object;
    int index;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(chr_peer_string, -1), 0);
    field_value = (void *)(object + peer_field->offset);
    peer = field_value->actor;
    for (index = 0; index < CHR_POINT_LIGHT_COUNT; index++) {
        QuadwordZero(&peer->drawEnv.lightColor[index]);
    }
    peer->drawEnv.flags &= ~CHR_DRAW_POINT_LIGHT;
}

void Java_xeno_Chr_talkto__Ljava_lang_String_(JThread *thread,
                                              ChrTalkCall *arguments)
{
    ChrFieldValue *field_value;
    JavaField *peer_field;
    Actor *peer;
    SceneObject object;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(chr_peer_string, -1), 0);
    field_value = (void *)(object + peer_field->offset);
    peer = field_value->actor;
    peer->talk_message = arguments->message->value->message_id;
}

void Java_xeno_Chr_touchto__Ljava_lang_String_(JThread *thread,
                                               ChrTalkCall *arguments)
{
    ChrFieldValue *field_value;
    JavaField *peer_field;
    Actor *peer;
    SceneObject object;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(chr_peer_string, -1), 0);
    field_value = (void *)(object + peer_field->offset);
    peer = field_value->actor;
    peer->touch_message = arguments->message->value->message_id;
}

/*
 * Finds the first child actor of kind 1 when the value is 0x1000000. The
 * result word is cleared to 0 first.
 */
void Java_xeno_Chr_childGetPeer__II(JThread *thread, ChrIntCall *arguments,
                                    ChrResultValue *result)
{
    ChrFieldValue *field_value;
    JavaField *peer_field;
    Actor *peer;
    Actor *child;
    SceneObject object;
    int i;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(chr_peer_string, -1), 0);
    field_value = (void *)(object + peer_field->offset);
    peer = field_value->actor;
    result->word = 0;
    if (arguments->value == 0x1000000) {
        for (i = 0; i < peer->childCount; i++) {
            child = peer->children[i];
            if (child->kind == 1) {
                result->actor = child;
                break;
            }
        }
    }
}

/*
 * Attaches the other Chr's actor as this Chr's peer and clears its sequence
 * state. Does nothing for a null peer.
 */
void Java_xeno_Chr_setPeer__Ljava_lang_Object_(JThread *thread,
                                               ChrWeaponCall *arguments)
{
    ChrFieldValue *field_value;
    JavaField *peer_field;
    Actor *peer;
    SequenceState *sequence;
    SceneObject object;

    object = arguments->object;
    peer = arguments->other.peer;
    if (peer != 0) {
        peer_field = lookupClassField(classJava_xeno_Chr,
                                      loadConstString(chr_peer_string, -1), 0);
        peer->update = ACT_updateSequence;
        peer->frame_counter = 0;
        field_value = (void *)(object + peer_field->offset);
        field_value->actor = peer;
        peer->signal = 0;
        sequence = &actSequence[((u8 *)peer)[ACTOR_SLOT_NUMBER_OFFSET]].state;
        peer->java_object = object;
        sequence->flags = 0;
        sequence->state_flags = 0;
        sequence->cleared_on_init = 0;
        sequence->cleared_on_init_run[0] = 0;
        sequence->cleared_on_init_run[1] = 0;
        sequence->cleared_on_init_run[2] = 0;
        sequence->cleared_on_init_run[3] = 0;
        sequence->handler[0] = 0;
        sequence->handler[1] = 0;
        sequence->handler[2] = 0;
        sequence->handler[3] = 0;
    }
}

void Java_xeno_Chr_dispRadar__Z(JThread *thread, ChrBoolCall *arguments)
{
    ChrFieldValue *field_value;
    JavaField *peer_field;
    Actor *peer;
    SceneObject object;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(chr_peer_string, -1), 0);
    field_value = (void *)(object + peer_field->offset);
    peer = field_value->actor;
    if (arguments->flag != 0) {
        peer->flags &= ~CHR_ACTOR_NO_RADAR;
    } else {
        peer->flags |= CHR_ACTOR_NO_RADAR;
    }
}

void Java_xeno_Chr_look_camera__(JThread *thread, ChrScaleCall *arguments)
{
    ChrFieldValue *field_value;
    JavaField *peer_field;
    Actor *peer;
    SceneObject object;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(chr_peer_string, -1), 0);
    field_value = (void *)(object + peer_field->offset);
    peer = field_value->actor;
    peer->look_mode = CHR_LOOK_CAMERA;
}

void Java_xeno_Chr_look_char__Ljava_lang_Object_(JThread *thread,
                                                 ChrWeaponCall *arguments)
{
    JavaField *peer_field;
    Actor *peer;
    Actor *target;
    SceneObject object;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(chr_peer_string, -1), 0);
    peer = *(Actor **)(object + peer_field->offset);
    target = *(Actor **)(arguments->other.object + peer_field->offset);
    peer->look_mode = CHR_LOOK_CHAR;
    peer->look_target = target;
}

void Java_xeno_Chr_look_unit__Ljava_lang_Object_(JThread *thread,
                                                 ChrWeaponCall *arguments)
{
    JavaField *peer_field;
    JavaField *unit_peer_field;
    Actor *peer;
    void *target;
    SceneObject object;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(chr_peer_string, -1), 0);
    peer = *(Actor **)(object + peer_field->offset);
    unit_peer_field = lookupClassField(classJava_xeno_Unit,
                                       loadConstString(chr_peer_string, -1), 0);
    target = *(void **)(arguments->other.object + unit_peer_field->offset);
    peer->look_mode = CHR_LOOK_UNIT;
    peer->look_target = target;
}

/*
 * Sets the look mode to default and clears the look target, eye control and
 * look vectors. The look and eye speeds become 1.
 */
void Java_xeno_Chr_look_default__(JThread *thread, ChrObjectCall *arguments)
{
    ChrFieldValue *field_value;
    JavaField *peer_field;
    Actor *peer;
    SceneObject object;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(chr_peer_string, -1), 0);
    field_value = (void *)(object + peer_field->offset);
    peer = field_value->actor;
    peer->look_mode = CHR_LOOK_DEFAULT;
    peer->look_eye_control = 0;
    peer->look_target = 0;
    QuadwordZero(&peer->look_clear_600[0]);
    QuadwordZero(&peer->look_clear_600[4]);
    QuadwordZero(&peer->look_point[0]);
    QuadwordZero(&peer->look_clear_640[0]);
    QuadwordZero(&peer->look_clear_640[4]);
    QuadwordZero(&peer->look_eye_angle[0]);
    peer->look_speed = 1.0f;
    peer->look_eye_speed = 1.0f;
}

void Java_xeno_Chr_look_point__FFF(JThread *thread, ChrVmArgumentSlot *arguments)
{
    ChrFieldValue *field_value;
    JavaField *peer_field;
    Actor *peer;
    SceneObject object;
    float x;
    float y;
    float z;

    object = arguments[0].object;
    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(chr_peer_string, -1), 0);
    field_value = (void *)(object + peer_field->offset);
    peer = field_value->actor;
    peer->look_mode = CHR_LOOK_POINT;
    __builtin_memcpy(&x, &arguments[1].bits, sizeof(x));
    peer->look_point[0] = x;
    __builtin_memcpy(&y, &arguments[2].bits, sizeof(y));
    peer->look_point[1] = y;
    __builtin_memcpy(&z, &arguments[3].bits, sizeof(z));
    peer->look_point[2] = z;
}

void Java_xeno_Chr_look_eye_set__FF(JThread *thread, ChrScaleCall *arguments)
{
    ChrFieldValue *field_value;
    JavaField *peer_field;
    Actor *peer;
    SceneObject object;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(chr_peer_string, -1), 0);
    field_value = (void *)(object + peer_field->offset);
    peer = field_value->actor;
    peer->look_eye_angle[0] = arguments->first.floating * 3.1415927f / 180.0f;
    peer->look_eye_angle[1] = arguments->second.floating * 3.1415927f / 180.0f;
    peer->look_eye_control = 14;
}

void Java_xeno_Chr_look_eye_control__I(JThread *thread, ChrScaleCall *arguments)
{
    ChrFieldValue *field_value;
    JavaField *peer_field;
    Actor *peer;
    SceneObject object;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(chr_peer_string, -1), 0);
    field_value = (void *)(object + peer_field->offset);
    peer = field_value->actor;
    peer->look_eye_control = arguments->first.integer;
}

void Java_xeno_Chr_look_speed__F(JThread *thread, ChrScaleCall *arguments)
{
    ChrFieldValue *field_value;
    JavaField *peer_field;
    Actor *peer;
    SceneObject object;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(chr_peer_string, -1), 0);
    field_value = (void *)(object + peer_field->offset);
    peer = field_value->actor;
    peer->look_speed = arguments->first.floating;
}

void Java_xeno_Chr_look_eye_speed__F(JThread *thread, ChrScaleCall *arguments)
{
    ChrFieldValue *field_value;
    JavaField *peer_field;
    Actor *peer;
    SceneObject object;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(chr_peer_string, -1), 0);
    field_value = (void *)(object + peer_field->offset);
    peer = field_value->actor;
    peer->look_eye_speed = arguments->first.floating;
}

void Java_xeno_Chr_renderCommand__I(JThread *thread, ChrScaleCall *arguments)
{
    ChrFieldValue *field_value;
    JavaField *peer_field;
    Actor *peer;
    SceneObject object;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(chr_peer_string, -1), 0);
    field_value = (void *)(object + peer_field->offset);
    peer = field_value->actor;
    peer->drawEnv.renderLevel = arguments->first.integer;
}

void Java_xeno_Chr_shadow_clip_scale__F(JThread *thread, ChrScaleCall *arguments)
{
    ChrFieldValue *field_value;
    JavaField *peer_field;
    Actor *peer;
    SceneObject object;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(chr_peer_string, -1), 0);
    field_value = (void *)(object + peer_field->offset);
    peer = field_value->actor;
    peer->shadowClipScale = arguments->first.floating;
}

void Java_xeno_Chr_shadow_map_id__I(JThread *thread,
                                    ChrShadowMapIdCall *arguments)
{
    ChrFieldValue *field_value;
    JavaField *peer_field;
    Actor *peer;
    SceneObject object;
    int *count;
    int index;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(chr_peer_string, -1), 0);
    field_value = (void *)(object + peer_field->offset);
    peer = field_value->actor;
    if (peer->mapShadowPartCount < CHR_MAP_SHADOW_PART_MAX) {
        count = &peer->mapShadowPartCount;
        index = *count;
        *count = index + 1;
        peer->mapShadowPartIds[index] = arguments->id;
    }
}

void Java_xeno_Chr_shadow_map_reset__(JThread *thread, ChrScaleCall *arguments)
{
    ChrFieldValue *field_value;
    JavaField *peer_field;
    Actor *peer;
    SceneObject object;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(chr_peer_string, -1), 0);
    field_value = (void *)(object + peer_field->offset);
    peer = field_value->actor;
    peer->mapShadowPartCount = 0;
}

void Java_xeno_Chr_hairStop__II(JThread *thread, ChrScaleCall *arguments)
{
    ChrFieldValue *field_value;
    JavaField *peer_field;
    Actor *peer;
    SceneObject object;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(chr_peer_string, -1), 0);
    field_value = (void *)(object + peer_field->offset);
    peer = field_value->actor;
    peer->transitionFrames = arguments->first.integer;
    peer->holdFrames = arguments->second.integer;
}

void Java_xeno_Chr_pixelAlpha__I(JThread *thread, ChrScaleCall *arguments)
{
    ChrFieldValue *field_value;
    JavaField *peer_field;
    Actor *peer;
    SceneObject object;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(chr_peer_string, -1), 0);
    field_value = (void *)(object + peer_field->offset);
    peer = field_value->actor;
    peer->drawEnv.pixelAlpha = arguments->first.integer;
}

void Java_xeno_Chr_pixelAlphaParts__II(JThread *thread, ChrScaleCall *arguments)
{
    ChrFieldValue *field_value;
    JavaField *peer_field;
    Actor *peer;
    SceneObject object;
    int count;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(chr_peer_string, -1), 0);
    field_value = (void *)(object + peer_field->offset);
    peer = field_value->actor;
    count = peer->drawEnv.partAlphaCount;
    if (count < CHR_PART_ALPHA_MAX) {
        peer->drawEnv.partAlpha[count][0] = arguments->second.integer;
        peer->drawEnv.partAlpha[count][1] = arguments->first.integer;
        peer->drawEnv.partAlphaCount = count + 1;
    }
}

void Java_xeno_Chr_pixelAlphaPartsReset__(JThread *thread, ChrScaleCall *arguments)
{
    ChrFieldValue *field_value;
    JavaField *peer_field;
    Actor *peer;
    SceneObject object;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(chr_peer_string, -1), 0);
    field_value = (void *)(object + peer_field->offset);
    peer = field_value->actor;
    peer->drawEnv.partAlphaCount = 0;
}

/*
 * Sets the motion-no-update state: 1 sets the actor flag, 2 sets the draw
 * flag, and 0 clears both.
 */
void Java_xeno_Chr_setMotNoUpdate__I(JThread *thread, ChrIntCall *arguments)
{
    ChrFieldValue *field_value;
    JavaField *peer_field;
    Actor *peer;
    SceneObject object;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(chr_peer_string, -1), 0);
    field_value = (void *)(object + peer_field->offset);
    peer = field_value->actor;
    switch (arguments->value) {
    case 0:
        peer->flags &= ~CHR_ACTOR_MOTION_NO_UPDATE;
        peer->drawEnv.flags &= ~CHR_DRAW_MOTION_NO_UPDATE;
        break;
    case 1:
        peer->flags |= CHR_ACTOR_MOTION_NO_UPDATE;
        peer->drawEnv.flags &= ~CHR_DRAW_MOTION_NO_UPDATE;
        break;
    case 2:
        peer->flags &= ~CHR_ACTOR_MOTION_NO_UPDATE;
        peer->drawEnv.flags |= CHR_DRAW_MOTION_NO_UPDATE;
        break;
    }
}

/* Attaches the other Chr's actor to this Chr as the right-hand weapon. */
void Java_xeno_Chr_setWeaponR__Lxeno_Chr_(JThread *thread, ChrWeaponCall *arguments)
{
    JavaField *peer_field;
    Actor *peer;
    Actor *other_peer;
    SceneObject object;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(chr_peer_string, -1), 0);
    peer = *(Actor **)(object + peer_field->offset);
    object = arguments->other.object;
    other_peer = *(Actor **)(object + peer_field->offset);
    ACT_setArms(peer, other_peer, CHR_ARMS_WEAPON_R, 2);
}

/* Detaches the other Chr's actor from this Chr's right-hand weapon. */
void Java_xeno_Chr_resetWeaponR__Lxeno_Chr_(JThread *thread, ChrWeaponCall *arguments)
{
    JavaField *peer_field;
    Actor *peer;
    Actor *other_peer;
    SceneObject object;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(chr_peer_string, -1), 0);
    peer = *(Actor **)(object + peer_field->offset);
    object = arguments->other.object;
    other_peer = *(Actor **)(object + peer_field->offset);
    ACT_resetArms(peer, other_peer, CHR_ARMS_WEAPON_R);
}

void Java_xeno_Chr_resetHand__(void)
{
}

void Java_xeno_Chr_resetEnv__(void)
{
}

void Java_xeno_Chr_ignoreShape__I(JThread *thread, ChrIntCall *arguments)
{
    ChrFieldValue *field_value;
    JavaField *peer_field;
    Actor *peer;
    SceneObject object;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(chr_peer_string, -1), 0);
    field_value = (void *)(object + peer_field->offset);
    peer = field_value->actor;
    if (arguments->value != 0) {
        peer->drawEnv.flags |= CHR_DRAW_IGNORE_SHAPE;
    } else {
        peer->drawEnv.flags &= ~CHR_DRAW_IGNORE_SHAPE;
    }
}
