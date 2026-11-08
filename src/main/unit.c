#include "common.h"
#include "shared.h"
#include "main/jni.h"
#include "main/jthread.h"
#include "main/unit.h"
#include "unit.h"
#include "main/init_vm.h"

#define UNIT_THREAD_KIND 5
#define UNIT_WAIT_UNIT 6
#define UNIT_FLAG_SUSPENDED 0x10

/* JThread.flags bits used by the Unit natives. */
#define UNIT_THREAD_FLAG_CAN_WAIT 0x40 /* the native may block the Java thread */
#define UNIT_THREAD_FLAG_WAITING 4 /* blocked on wait_target */

/* MapUnitRecord.flags bits set from the Unit setters. */
#define UNIT_PEER_FLAG_MAP_SHADOW 0x20
#define UNIT_PEER_FLAG_CLIP 0x40
#define UNIT_PEER_FLAG_COLLISION 0x100

/* UnitSequenceEntry.state: which of the motion / scale tracks are active. */
#define UNIT_SEQ_STATE_MOTION 0x10
#define UNIT_SEQ_STATE_SCALE_X 0x20
#define UNIT_SEQ_STATE_SCALE_Y 0x40
#define UNIT_SEQ_STATE_SCALE_Z 0x80
#define UNIT_SEQ_STATE_SCALE_ALL 0xe0

#define UNIT_FRAMES_PER_SECOND 30.0f
#define UNIT_DEGREES_HALF_TURN 180.0f
#define UNIT_PI 3.1415927f
#define UNIT_JOINT_ACCESSORY_FLAG 0x8000 /* parent_flags selects an accessory joint */
#define UNIT_JOINT_ACCESSORY_MASK 0x7fff

/* Partial native field views; unknown spans remain byte storage. */

typedef struct UnitVector3 {
    float x;
    float y;
    float z;
} UnitVector3;



typedef struct UnitMoveChannel {
    void *source;                                     /* +0x00 */
    int frames;                                       /* +0x04 */
    unsigned char unmodeled_08[0x20 - 0x08];
    float target_x;                                   /* +0x20 */
    unsigned char unmodeled_24[0x28 - 0x24];
    float target_z;                                   /* +0x28 */
    float step;                                       /* +0x2c */
} UnitMoveChannel;


typedef union UnitJavaValue {
    int integer;
    float floating;
} UnitJavaValue;

typedef struct UnitChrSequenceCall {
    u8 *object;
    u8 *chr;
    UnitJavaValue param;
    UnitJavaValue x;
    UnitJavaValue y;
    UnitJavaValue z;
} UnitChrSequenceCall;

typedef struct UnitParentCall {
    u8 *object;
    u8 *chr;
    int flags;
} UnitParentCall;

typedef struct UnitScaleCall {
    u8 *object;
    UnitJavaValue first;  /* +0x04 */
    UnitJavaValue second; /* +0x08 */
    u8 wait;                  /* +0x0c */
} UnitScaleCall;

typedef struct UnitStringValue {
    unsigned char unmodeled_00[4];
    int length;        /* +0x4 */
    const char *data;  /* +0x8 */
} UnitStringValue;

typedef struct UnitString {
    unsigned char unmodeled_00[4];
    UnitStringValue *value; /* +0x4 */
} UnitString;

typedef struct UnitStartCall {
    u8 *object;
    int mode;
    UnitString *method_name;
} UnitStartCall;

typedef union UnitMotionSlot {
    int integer;
    float floating;
    u16 half;
    u8 byte;
} UnitMotionSlot;

typedef struct UnitMotionCall {
    u8 *object;
    UnitMotionSlot slot[7];
} UnitMotionCall;

typedef union UnitCallSlot {
    int integer;
    float floating;
    u8 *object;
    u8 byte;
} UnitCallSlot;

typedef struct UnitRotCall {
    u8 *object;
    UnitCallSlot slot[3];
} UnitRotCall;

/*
 * The rotation natives that follow another unit take it as an Object
 * argument in the slot the other signatures use for the float or the frame
 * count.
 */
typedef struct UnitRotFramesFollowCall {
    u8 *object;
    int frames;  /* +0x04 */
    u8 *target;  /* +0x08 */
    u8 wait;     /* +0x0c */
} UnitRotFramesFollowCall;

typedef struct UnitRotRateFollowCall {
    u8 *object;
    u8 *target;  /* +0x04 */
    float rate;  /* +0x08 */
    u8 wait;     /* +0x0c */
} UnitRotRateFollowCall;

typedef struct UnitMoveCall {
    u8 *object;
    UnitCallSlot slot[4];
} UnitMoveCall;

typedef struct UnitSpline {
    unsigned char unmodeled_00[8];
    u16 frame_count; /* +0x8 */
} UnitSpline;

typedef struct UnitSplineCall {
    u8 *object;
    UnitSpline *spline;   /* +0x04 */
    UnitJavaValue flags;  /* +0x08 */
    u8 wait;              /* +0x0c */
} UnitSplineCall;

typedef struct UnitMotionRootCall {
    u8 *object;
    int type;
    UnitPivotVector *vector;
} UnitMotionRootCall;

extern const char unitPeerName[];
extern const char unitPositionYName[];
extern const char unitRotationXName[];
extern const char unitRotationYName[];
extern const char unitRotationZName[];
extern const char unitPositionXName[];
extern const char unitPositionZName[];
extern unsigned int GameLoopState[];

extern JavaMethod *findMethod(JavaClass *clazz, SceneString *name,
                              void *signature_or_type);

extern void JNI_initThread(SceneVm *vm);
/*
 * UNIT_rotY, UNIT_rotZ, UNIT_sclX, UNIT_sclY and UNIT_sclZ take the mode as a
 * new first argument. The JNI trampolines below insert it and forward thread,
 * arguments and failure_result unchanged.
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
 * UNIT_moveXZ, UNIT_rotX and UNIT_motion take the mode the same way. Their
 * JNI trampolines insert it ahead of thread, arguments and failure_result.
 */
static void UNIT_moveXZ(int mode, JThread *thread, void *arguments,
                 u32 *failure_result);

static void UNIT_rotX(int mode, JThread *thread, void *arguments,
               u32 *failure_result);

void UNIT_motion(int mode, JThread *thread, void *arguments,
                 u32 *failure_result);

/* Defined in src/main/map_create_unit_peer.c: applies `callback` to the flags of every unit in `group`. */
extern void MAP_callUnitGroup(int group, void (*callback)(int *flags));

extern void MAP_updateUnitDefault(MapUnitRecord *peer);
extern void MAP_updateUnitMPack(MapUnitRecord *peer);
extern void MAP_updateUnitSequence(MapUnitRecord *peer);
extern void MAP_updateUnitPartsSequence(MapUnitRecord *peer);
extern void MAP_updateUnitPartsSequence2(MapUnitRecord *peer);
extern void SEQ_rotYCNSUnitChr(void *unit);
extern void SEQ_transCNSUnitChr(void *unit);
extern void SEQ_scaleUnit(void *unit);
extern void SEQ_rotateUnit(void *unit);
extern void SEQ_moveUnitXZ(void *unit);
extern void SEQ_moveChr(void *unit);
extern void SEQ_motionUnit(void *unit);
extern void SEQ_moveUnitSPL(void *unit);
extern void SEQ_rotateUnitSPL(void *unit);
extern void SEQ_scaleUnitSPL(void *unit);

const char algorithmFieldName[10] = "algorithm";

UnitActorStorage actor[64] = { 0 };

static float defaultOffset_0043C1D0[4];

static float defaultOffset_0043C1D0[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

/* Sets the peer's update callback from bits 8-11 of the unit's algorithm field. */
static void UNIT_setUpdate(u8 *object, MapUnitRecord *unit)
{
    JavaField *state_field;
    int *state;
    u32 frame;

    state_field = lookupClassField(classJava_xeno_Unit,
                                   loadConstString(algorithmFieldName, -1), 0);
    state = (int *)(object + state_field->offset);
    switch ((*state >> 8) & 0xf) {
    case 0:
        unit->update = MAP_updateUnitSequence;
        break;
    case 1:
        unit->update_stamp = 0;
        unit->update = MAP_updateUnitPartsSequence;
        break;
    case 2:
        frame = GameLoopState[1];
        unit->flags |= 4;
        unit->update_stamp = frame;
        unit->update = MAP_updateUnitPartsSequence;
        break;
    case 3:
        unit->update_stamp = 0;
        unit->update = MAP_updateUnitPartsSequence2;
        break;
    }
}

/*
 * Native Unit.rotYCNS: starts a Y-rotation sequence on the unit's slot,
 * tracking the chr object and storing the int and three float arguments.
 */
void Java_xeno_Unit_rotYCNS__Ljava_lang_Object_IFFF(
    JThread *thread, UnitChrSequenceCall *arguments)
{
    JavaField *peer_field;
    u8 *object;
    MapUnitRecord *peer;
    UnitSequenceEntry *sequence;
    UnitRotChannel *block;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Unit,
                                  loadConstString(unitPeerName, -1), 0);
    peer = *(MapUnitRecord **)(object + peer_field->offset);
    UNIT_setUpdate(object, peer);
    sequence = &unitSequence[peer->slot];
    block = &sequence->rotation;
    sequence->state |= 4;
    sequence->rotateSequence = SEQ_rotYCNSUnitChr;
    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(unitPeerName, -1), 0);
    block->track.source = *(void **)(arguments->chr + peer_field->offset);
    block->frames[1] = arguments->param.integer;
    block->target[0] = arguments->x.floating;
    block->target[1] = arguments->y.floating;
    block->target[2] = arguments->z.floating;
}

/*
 * Native Unit.transCNS: starts a translation sequence on the unit's slot,
 * tracking the chr object and storing the int and three float arguments.
 */
void Java_xeno_Unit_transCNS__Ljava_lang_Object_IFFF(
    JThread *thread, UnitChrSequenceCall *arguments)
{
    JavaField *peer_field;
    u8 *object;
    MapUnitRecord *peer;
    UnitSequenceEntry *sequence;
    UnitTransSequence *block;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Unit,
                                  loadConstString(unitPeerName, -1), 0);
    peer = *(MapUnitRecord **)(object + peer_field->offset);
    UNIT_setUpdate(object, peer);
    sequence = &unitSequence[peer->slot];
    block = &sequence->trans;
    sequence->state |= 1;
    sequence->transSequence = SEQ_transCNSUnitChr;
    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(unitPeerName, -1), 0);
    block->track.source = *(void **)(arguments->chr + peer_field->offset);
    block->param = arguments->param.integer;
    block->x = arguments->x.floating;
    block->y = arguments->y.floating;
    block->z = arguments->z.floating;
}

/*
 * Helper for the Unit move natives: mode 0 takes an int frame count, mode 1 a
 * float speed per second, and modes 2 and 3 use a chr object as the track.
 */
static void UNIT_moveXZ(int mode, JThread *thread, void *arguments,
                u32 *failure_result)
{
    UnitMoveCall *call = arguments;
    JavaField *peer_field;
    JavaField *algorithm_field;
    MapUnitRecord *peer;
    float *source;
    UnitSequenceEntry *sequence;
    UnitMoveChannel *channel;
    void *chr_peer;
    u8 *object;
    int wait;

    wait = 0;
    object = call->object;
    if (JNI_isInstanceOf(object, classJava_xeno_Unit) == 0) {
        *failure_result = 0;
        return;
    }

    peer_field = lookupClassField(classJava_xeno_Unit,
                                  loadConstString(unitPeerName, -1), 0);
    peer = *(MapUnitRecord **)(object + peer_field->offset);
    sequence = &unitSequence[peer->slot];
    sequence->state |= 1;
    channel = (UnitMoveChannel *)&sequence->trans;
    algorithm_field = lookupClassField(classJava_xeno_Unit,
                                       loadConstString(algorithmFieldName, -1), 0);

    if (*(u32 *)(object + algorithm_field->offset) & 1)
        source = &peer->position.x;
    else
        source = defaultOffset_0043C1D0;

    switch (mode) {
    case 0:
        channel->frames = call->slot[0].integer;
        channel->target_x = call->slot[1].floating + source[0];
        channel->target_z = call->slot[2].floating + source[2];
        sequence->transSequence = SEQ_moveUnitXZ;
        wait = call->slot[3].byte;
        break;
    case 1:
        channel->frames = -1;
        channel->target_x = call->slot[0].floating + source[0];
        channel->target_z = call->slot[1].floating + source[2];
        channel->step = call->slot[2].floating / UNIT_FRAMES_PER_SECOND;
        sequence->transSequence = SEQ_moveUnitXZ;
        wait = call->slot[3].byte;
        break;
    case 2:
        peer_field = lookupClassField(classJava_xeno_Chr,
                                      loadConstString(unitPeerName, -1), 0);
        chr_peer = *(void **)(call->slot[1].object + peer_field->offset);
        channel->frames = call->slot[0].integer;
        channel->source = chr_peer;
        sequence->transSequence = SEQ_moveChr;
        wait = call->slot[2].byte;
        break;
    case 3:
        peer_field = lookupClassField(classJava_xeno_Chr,
                                      loadConstString(unitPeerName, -1), 0);
        chr_peer = *(void **)(call->slot[0].object + peer_field->offset);
        channel->source = chr_peer;
        channel->frames = -1;
        channel->step = call->slot[1].floating / UNIT_FRAMES_PER_SECOND;
        sequence->transSequence = SEQ_moveChr;
        wait = call->slot[2].byte;
        break;
    }

    /* Flags the peer and, when the native may wait, blocks the thread on it. */
    if (wait == 1) {
        peer->flags |= 2;
        if (thread->kind != UNIT_THREAD_KIND && (thread->flags & UNIT_THREAD_FLAG_CAN_WAIT) == 0)
            return;
        if ((thread->flags & UNIT_THREAD_FLAG_CAN_WAIT) != 0) {
            thread->wait_target.object = peer;
            thread->wait_kind = UNIT_WAIT_UNIT;
            thread->resume_frames = thread->frame_depth;
            thread->flags |= UNIT_THREAD_FLAG_WAITING;
        }
        thread->resume_frames = thread->frame_depth;
        thread->flags |= 1;
    }
}

/*
 * Helper for the Unit rotation natives about X; UNIT_rotY and UNIT_rotZ work
 * the same way for Y and Z. Mode 0 takes a frame count, mode 1 a rate per
 * second, and modes 2 and 3 follow another object's track. Angles given in
 * degrees are stored as radians.
 */
static void UNIT_rotX(int mode, JThread *thread, void *arguments,
               u32 *failure_result)
{
    UnitRotCall *call = arguments;
    JavaField *peer_field;
    JavaField *algorithm_field;
    MapUnitRecord *peer;
    float *source;
    UnitSequenceEntry *sequence;
    UnitRotChannel *channel;
    u8 *object;
    UnitRotFramesFollowCall *frames_follow = arguments;
    UnitRotRateFollowCall *rate_follow = arguments;
    void *chr_peer;
    int wait;

    wait = 0;
    object = call->object;
    peer_field = lookupClassField(classJava_xeno_Unit,
                                  loadConstString(unitPeerName, -1), 0);
    peer = *(MapUnitRecord **)(object + peer_field->offset);
    algorithm_field = lookupClassField(classJava_xeno_Unit,
                                       loadConstString(algorithmFieldName, -1), 0);

    if (*(u32 *)(object + algorithm_field->offset) & 1)
        source = &peer->rotation.x;
    else
        source = defaultOffset_0043C1D0;

    sequence = &unitSequence[peer->slot];
    sequence->rotateSequence = SEQ_rotateUnit;
    sequence->state |= 2;
    channel = &sequence->rotation;

    switch (mode) {
    case 0:
        channel->frames[0] = call->slot[0].integer;
        channel->flags &= ~1;
        channel->target[0] = (call->slot[1].floating + source[0]) / UNIT_DEGREES_HALF_TURN * UNIT_PI;
        wait = call->slot[2].byte;
        break;
    case 1:
        channel->frames[0] = -1;
        channel->flags &= ~1;
        channel->target[0] = (call->slot[0].floating + source[0]) / UNIT_DEGREES_HALF_TURN * UNIT_PI;
        channel->step[0] = call->slot[1].floating / UNIT_DEGREES_HALF_TURN * UNIT_PI / UNIT_FRAMES_PER_SECOND;
        wait = call->slot[2].byte;
        break;
    case 2:
        chr_peer = *(void **)(frames_follow->target + algorithm_field->offset);
        channel->frames[0] = call->slot[0].integer;
        channel->flags |= 1;
        channel->track.source = chr_peer;
        wait = call->slot[2].byte;
        break;
    case 3:
        chr_peer = *(void **)(rate_follow->target + algorithm_field->offset);
        channel->frames[0] = -1;
        channel->flags |= 1;
        channel->track.source = chr_peer;
        channel->step[0] = call->slot[1].floating / UNIT_DEGREES_HALF_TURN * UNIT_PI / UNIT_FRAMES_PER_SECOND;
        wait = call->slot[2].byte;
        break;
    }

    /* Flags the peer and, when the native may wait, blocks the thread on it. */
    if (wait == 1) {
        peer->flags |= 2;
        if (thread->kind != UNIT_THREAD_KIND && (thread->flags & UNIT_THREAD_FLAG_CAN_WAIT) == 0)
            return;
        if ((thread->flags & UNIT_THREAD_FLAG_CAN_WAIT) != 0) {
            thread->wait_target.object = peer;
            thread->wait_kind = UNIT_WAIT_UNIT;
            thread->resume_frames = thread->frame_depth;
            thread->flags |= UNIT_THREAD_FLAG_WAITING;
        }
        thread->resume_frames = thread->frame_depth;
        thread->flags |= 1;
    }
}

/* Y-axis variant of UNIT_rotX. */
static void UNIT_rotY(int mode, JThread *thread, void *arguments,
               u32 *failure_result)
{
    UnitRotCall *call = arguments;
    JavaField *peer_field;
    JavaField *algorithm_field;
    MapUnitRecord *peer;
    float *source;
    UnitSequenceEntry *sequence;
    UnitRotChannel *channel;
    void *chr_peer;
    u8 *object;
    int wait;

    wait = 0;
    object = call->object;
    peer_field = lookupClassField(classJava_xeno_Unit,
                                  loadConstString(unitPeerName, -1), 0);
    peer = *(MapUnitRecord **)(object + peer_field->offset);
    algorithm_field = lookupClassField(classJava_xeno_Unit,
                                       loadConstString(algorithmFieldName, -1), 0);

    if (*(u32 *)(object + algorithm_field->offset) & 1)
        source = &peer->rotation.x;
    else
        source = defaultOffset_0043C1D0;

    sequence = &unitSequence[peer->slot];
    sequence->rotateSequence = SEQ_rotateUnit;
    sequence->state |= 4;
    channel = &sequence->rotation;

    switch (mode) {
    case 0:
        channel->frames[1] = call->slot[0].integer;
        channel->flags &= ~2;
        channel->target[1] = (call->slot[1].floating + source[1]) / UNIT_DEGREES_HALF_TURN * UNIT_PI;
        wait = call->slot[2].byte;
        break;
    case 1:
        channel->frames[1] = -1;
        channel->flags &= ~2;
        channel->target[1] = (call->slot[0].floating + source[1]) / UNIT_DEGREES_HALF_TURN * UNIT_PI;
        channel->step[1] = call->slot[1].floating / UNIT_DEGREES_HALF_TURN * UNIT_PI / UNIT_FRAMES_PER_SECOND;
        wait = call->slot[2].byte;
        break;
    case 2:
        peer_field = lookupClassField(classJava_xeno_Chr,
                                      loadConstString(unitPeerName, -1), 0);
        chr_peer = *(void **)(call->slot[1].object + peer_field->offset);
        channel->frames[1] = call->slot[0].integer;
        channel->flags |= 2;
        channel->track.source = chr_peer;
        wait = call->slot[2].byte;
        break;
    case 3:
        peer_field = lookupClassField(classJava_xeno_Chr,
                                      loadConstString(unitPeerName, -1), 0);
        chr_peer = *(void **)(call->slot[0].object + peer_field->offset);
        channel->frames[1] = -1;
        channel->flags |= 2;
        channel->track.source = chr_peer;
        channel->step[1] = call->slot[1].floating / UNIT_DEGREES_HALF_TURN * UNIT_PI / UNIT_FRAMES_PER_SECOND;
        wait = call->slot[2].byte;
        break;
    }

    /* Flags the peer and, when the native may wait, blocks the thread on it. */
    if (wait == 1) {
        peer->flags |= 2;
        if (thread->kind != UNIT_THREAD_KIND && (thread->flags & UNIT_THREAD_FLAG_CAN_WAIT) == 0)
            return;
        if ((thread->flags & UNIT_THREAD_FLAG_CAN_WAIT) != 0) {
            thread->wait_target.object = peer;
            thread->wait_kind = UNIT_WAIT_UNIT;
            thread->resume_frames = thread->frame_depth;
            thread->flags |= UNIT_THREAD_FLAG_WAITING;
        }
        thread->resume_frames = thread->frame_depth;
        thread->flags |= 1;
    }
}

/* Z-axis variant of UNIT_rotX. */
static void UNIT_rotZ(int mode, JThread *thread, void *arguments,
               u32 *failure_result)
{
    UnitRotCall *call = arguments;
    JavaField *peer_field;
    JavaField *algorithm_field;
    MapUnitRecord *peer;
    float *source;
    UnitSequenceEntry *sequence;
    UnitRotChannel *channel;
    u8 *object;
    UnitRotFramesFollowCall *frames_follow = arguments;
    UnitRotRateFollowCall *rate_follow = arguments;
    void *chr_peer;
    int wait;

    wait = 0;
    object = call->object;
    if (JNI_isInstanceOf(object, classJava_xeno_Unit) == 0) {
        *failure_result = 0;
        return;
    }
    peer_field = lookupClassField(classJava_xeno_Unit,
                                  loadConstString(unitPeerName, -1), 0);
    peer = *(MapUnitRecord **)(object + peer_field->offset);
    algorithm_field = lookupClassField(classJava_xeno_Unit,
                                       loadConstString(algorithmFieldName, -1), 0);

    if (*(u32 *)(object + algorithm_field->offset) & 1)
        source = &peer->rotation.x;
    else
        source = defaultOffset_0043C1D0;

    sequence = &unitSequence[peer->slot];
    sequence->rotateSequence = SEQ_rotateUnit;
    sequence->state |= 8;
    channel = &sequence->rotation;

    switch (mode) {
    case 0:
        channel->frames[2] = call->slot[0].integer;
        channel->flags &= ~4;
        channel->target[2] = (call->slot[1].floating + source[2]) / UNIT_DEGREES_HALF_TURN * UNIT_PI;
        wait = call->slot[2].byte;
        break;
    case 1:
        channel->frames[2] = -1;
        channel->flags &= ~4;
        channel->target[2] = (call->slot[0].floating + source[2]) / UNIT_DEGREES_HALF_TURN * UNIT_PI;
        channel->step[2] = call->slot[1].floating / UNIT_DEGREES_HALF_TURN * UNIT_PI / UNIT_FRAMES_PER_SECOND;
        wait = call->slot[2].byte;
        break;
    case 2:
        chr_peer = *(void **)(frames_follow->target + algorithm_field->offset);
        channel->frames[2] = call->slot[0].integer;
        channel->flags |= 4;
        channel->track.source = chr_peer;
        wait = call->slot[2].byte;
        break;
    case 3:
        chr_peer = *(void **)(rate_follow->target + algorithm_field->offset);
        channel->frames[2] = -1;
        channel->flags |= 2;
        channel->track.source = chr_peer;
        channel->step[2] = call->slot[1].floating / UNIT_DEGREES_HALF_TURN * UNIT_PI / UNIT_FRAMES_PER_SECOND;
        wait = call->slot[2].byte;
        break;
    }

    /* Flags the peer and, when the native may wait, blocks the thread on it. */
    if (wait == 1) {
        peer->flags |= 2;
        if (thread->kind != UNIT_THREAD_KIND && (thread->flags & UNIT_THREAD_FLAG_CAN_WAIT) == 0)
            return;
        if ((thread->flags & UNIT_THREAD_FLAG_CAN_WAIT) != 0) {
            thread->wait_target.object = peer;
            thread->wait_kind = UNIT_WAIT_UNIT;
            thread->resume_frames = thread->frame_depth;
            thread->flags |= UNIT_THREAD_FLAG_WAITING;
        }
        thread->resume_frames = thread->frame_depth;
        thread->flags |= 1;
    }
}

/* Native Unit.getRotate: copies the peer's rotation into the rotationX/Y/Z fields, in degrees. */
void Java_xeno_Unit_getRotate__(JThread *thread, UnitObjectCall *arguments)
{
    JavaField *peer_field;
    JavaField *field;
    u8 *object;
    Vector4 *vector;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Unit,
                                  loadConstString(unitPeerName, -1), 0);
    vector = &(*(MapUnitRecord **)(object + peer_field->offset))->rotation;
    field = lookupClassField(classJava_xeno_Unit,
                             loadConstString(unitRotationXName, -1), 0);
    *(float *)(object + field->offset) = vector->x / UNIT_PI * UNIT_DEGREES_HALF_TURN;
    field = lookupClassField(classJava_xeno_Unit,
                             loadConstString(unitRotationYName, -1), 0);
    *(float *)(object + field->offset) = vector->y / UNIT_PI * UNIT_DEGREES_HALF_TURN;
    field = lookupClassField(classJava_xeno_Unit,
                             loadConstString(unitRotationZName, -1), 0);
    *(float *)(object + field->offset) = vector->z / UNIT_PI * UNIT_DEGREES_HALF_TURN;
}

/* Native Unit.getSignal: writes the peer's signal to the result, or 0 when the object is not a Unit. */
void Java_xeno_Unit_getSignal__(JThread *thread, UnitObjectCall *arguments,
                                u32 *failure_result)
{
    JavaField *peer_field;
    u8 *object;
    MapUnitRecord *peer;

    object = arguments->object;
    if (JNI_isInstanceOf(object, classJava_xeno_Unit) == 0) {
        *failure_result = 0;
        return;
    }
    peer_field = lookupClassField(classJava_xeno_Unit,
                                  loadConstString(unitPeerName, -1), 0);
    peer = *(MapUnitRecord **)(object + peer_field->offset);
    *failure_result = peer->signal;
}

/* Native Unit.getTranslate: copies the peer's position into the positionX/Y/Z fields. */
void Java_xeno_Unit_getTranslate__(JThread *thread, UnitObjectCall *arguments)
{
    JavaField *peer_field;
    JavaField *field;
    u8 *object;
    Vector4 *vector;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Unit,
                                  loadConstString(unitPeerName, -1), 0);
    vector = &(*(MapUnitRecord **)(object + peer_field->offset))->position;
    field = lookupClassField(classJava_xeno_Unit,
                             loadConstString(unitPositionXName, -1), 0);
    *(float *)(object + field->offset) = vector->x;
    field = lookupClassField(classJava_xeno_Unit,
                             loadConstString(unitPositionYName, -1), 0);
    *(float *)(object + field->offset) = vector->y;
    field = lookupClassField(classJava_xeno_Unit,
                             loadConstString(unitPositionZName, -1), 0);
    *(float *)(object + field->offset) = vector->z;
}

void Java_xeno_Unit_invalidate__(JThread *thread, void *arguments,
                                 u32 *failure_result)
{
}

void Java_xeno_Unit_move__FFFZ(JThread *thread, void *arguments,
                               u32 *failure_result)
{
    UNIT_moveXZ(1, thread, arguments, failure_result);
}

void Java_xeno_Unit_move__IFFZ(JThread *thread, void *arguments,
                               u32 *failure_result)
{
    UNIT_moveXZ(0, thread, arguments, failure_result);
}

/*
 * Native Unit.move(Spline, int, boolean): starts a spline move on the
 * translation sequence and blocks when the wait flag is set.
 */
void Java_xeno_Unit_move__Lxeno_util_Spline_IZ(JThread *thread,
                                           UnitSplineCall *arguments,
                                           u32 *failure_result)
{
    JavaField *peer_field;
    MapUnitRecord *peer;
    UnitSequenceEntry *sequence;
    UnitTrack *track;
    UnitSpline *spline;
    int flags;
    u8 wait;
    u8 *object;

    object = arguments->object;
    if (JNI_isInstanceOf(object, classJava_xeno_Unit) == 0) {
        *failure_result = 0;
        return;
    }

    peer_field = lookupClassField(classJava_xeno_Unit,
                                  loadConstString(unitPeerName, -1), 0);
    peer = *(MapUnitRecord **)(object + peer_field->offset);
    sequence = &unitSequence[peer->slot];
    track = &sequence->trans.track;
    sequence->state |= 0x1;
    lookupClassField(classJava_xeno_Unit, loadConstString(algorithmFieldName, -1), 0);
    spline = arguments->spline;
    flags = arguments->flags.integer;
    wait = arguments->wait;
    sequence->transSequence = SEQ_moveUnitSPL;
    track->flags = flags;
    track->source = spline;
    track->last_frame = spline->frame_count;
    if (wait == 1) {
        peer->flags |= 2;
        if (thread->kind != UNIT_THREAD_KIND && (thread->flags & UNIT_THREAD_FLAG_CAN_WAIT) == 0)
            return;
        if ((thread->flags & UNIT_THREAD_FLAG_CAN_WAIT) != 0) {
            thread->wait_target.object = peer;
            thread->wait_kind = UNIT_WAIT_UNIT;
            thread->resume_frames = thread->frame_depth;
            thread->flags |= UNIT_THREAD_FLAG_WAITING;
        }
        thread->resume_frames = thread->frame_depth;
        thread->flags |= 1;
    }
}

/*
 * Native Unit.rotate(Spline, int, boolean): starts a spline rotation on the
 * rotation sequence and blocks when the wait flag is set.
 */
void Java_xeno_Unit_rotate__Lxeno_util_Spline_IZ(JThread *thread,
                                           UnitSplineCall *arguments,
                                           u32 *failure_result)
{
    JavaField *peer_field;
    MapUnitRecord *peer;
    UnitSequenceEntry *sequence;
    UnitTrack *track;
    UnitSpline *spline;
    int flags;
    u8 wait;
    u8 *object;

    object = arguments->object;
    if (JNI_isInstanceOf(object, classJava_xeno_Unit) == 0) {
        *failure_result = 0;
        return;
    }

    peer_field = lookupClassField(classJava_xeno_Unit,
                                  loadConstString(unitPeerName, -1), 0);
    peer = *(MapUnitRecord **)(object + peer_field->offset);
    sequence = &unitSequence[peer->slot];
    track = &sequence->rotation.track;
    sequence->state |= 0xe;
    lookupClassField(classJava_xeno_Unit, loadConstString(algorithmFieldName, -1), 0);
    spline = arguments->spline;
    flags = arguments->flags.integer;
    wait = arguments->wait;
    sequence->rotateSequence = SEQ_rotateUnitSPL;
    track->flags = flags;
    track->source = spline;
    track->last_frame = spline->frame_count;
    if (wait == 1) {
        peer->flags |= 2;
        if (thread->kind != UNIT_THREAD_KIND && (thread->flags & UNIT_THREAD_FLAG_CAN_WAIT) == 0)
            return;
        if ((thread->flags & UNIT_THREAD_FLAG_CAN_WAIT) != 0) {
            thread->wait_target.object = peer;
            thread->wait_kind = UNIT_WAIT_UNIT;
            thread->resume_frames = thread->frame_depth;
            thread->flags |= UNIT_THREAD_FLAG_WAITING;
        }
        thread->resume_frames = thread->frame_depth;
        thread->flags |= 1;
    }
}

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

/*
 * Helper for Unit.mtn: mode 0 sets the motion, blend and speed; mode 1 also
 * sets the first and last frame and the option. Sets the motion sequence and
 * blocks the thread when the wait flag is set.
 */
void UNIT_motion(int mode, JThread *thread, void *arguments,
                 u32 *failure_result)
{
    UnitMotionCall *call = arguments;
    JavaField *peer_field;
    MapUnitRecord *peer;
    UnitSequenceEntry *sequence;
    UnitMotionChannel *motion;
    u8 *object;
    int wait;

    wait = 0;
    object = call->object;
    peer_field = lookupClassField(classJava_xeno_Unit,
                                  loadConstString(unitPeerName, -1), 0);
    peer = *(MapUnitRecord **)(object + peer_field->offset);
    sequence = &unitSequence[peer->slot];
    sequence->motionSequence = SEQ_motionUnit;
    sequence->state |= UNIT_SEQ_STATE_MOTION;
    motion = &sequence->motion;

    switch (mode) {
    case 0:
        motion->motion = call->slot[0].integer;
        motion->blend = call->slot[1].integer;
        motion->speed = call->slot[2].floating;
        wait = call->slot[3].byte;
        motion->first_frame = -1;
        motion->last_frame = -1;
        break;
    case 1:
        motion->motion = call->slot[0].integer;
        motion->first_frame = call->slot[1].half;
        motion->last_frame = call->slot[2].half;
        motion->option = call->slot[3].byte;
        motion->blend = call->slot[4].integer;
        motion->speed = call->slot[5].floating;
        wait = call->slot[6].byte;
        break;
    }

    if (wait == 1) {
        peer->flags |= 2;
        if (thread->kind != UNIT_THREAD_KIND && (thread->flags & UNIT_THREAD_FLAG_CAN_WAIT) == 0)
            return;
        if ((thread->flags & UNIT_THREAD_FLAG_CAN_WAIT) != 0) {
            thread->wait_target.object = peer;
            thread->wait_kind = UNIT_WAIT_UNIT;
            thread->resume_frames = thread->frame_depth;
            thread->flags |= UNIT_THREAD_FLAG_WAITING;
        }
        thread->resume_frames = thread->frame_depth;
        thread->flags |= 1;
    }
}

void Java_xeno_Unit_mtn__IIFZ(JThread *thread, void *arguments,
                              u32 *failure_result)
{
    UNIT_motion(0, thread, arguments, failure_result);
}

void Java_xeno_Unit_mtn__IIIIIFZ(JThread *thread, void *arguments,
                                 u32 *failure_result)
{
    UNIT_motion(1, thread, arguments, failure_result);
}

void Java_xeno_Unit_rotX__FFZ(JThread *thread, void *arguments,
                              u32 *failure_result)
{
    UNIT_rotX(1, thread, arguments, failure_result);
}

void Java_xeno_Unit_rotX__IFZ(JThread *thread, void *arguments,
                              u32 *failure_result)
{
    UNIT_rotX(0, thread, arguments, failure_result);
}

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

void Java_xeno_Unit_rotY__FFZ(JThread *thread, void *arguments,
                              u32 *failure_result)
{
    UNIT_rotY(1, thread, arguments, failure_result);
}

void Java_xeno_Unit_rotY__IFZ(JThread *thread, void *arguments,
                              u32 *failure_result)
{
    UNIT_rotY(0, thread, arguments, failure_result);
}

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

/*
 * Native Unit.scale: starts a spline scale on all three scale tracks and
 * blocks when the wait flag is set.
 */
void Java_xeno_Unit_scale__Lxeno_util_Spline_IZ(JThread *thread,
                                           UnitSplineCall *arguments,
                                           u32 *failure_result)
{
    JavaField *peer_field;
    MapUnitRecord *peer;
    UnitSequenceEntry *sequence;
    UnitTrack *track;
    UnitSpline *spline;
    u8 wait;
    u8 *object;

    object = arguments->object;
    if (JNI_isInstanceOf(object, classJava_xeno_Unit) == 0) {
        *failure_result = 0;
        return;
    }

    peer_field = lookupClassField(classJava_xeno_Unit,
                                  loadConstString(unitPeerName, -1), 0);
    peer = *(MapUnitRecord **)(object + peer_field->offset);
    sequence = &unitSequence[peer->slot];
    track = &sequence->scale.track;
    sequence->state |= UNIT_SEQ_STATE_SCALE_ALL;
    lookupClassField(classJava_xeno_Unit, loadConstString(algorithmFieldName, -1), 0);
    spline = arguments->spline;
    wait = arguments->wait;
    sequence->scaleSequence = SEQ_scaleUnitSPL;
    track->source = spline;
    track->last_frame = spline->frame_count;
    if (wait == 1) {
        peer->flags |= 2;
        if (thread->kind != UNIT_THREAD_KIND && (thread->flags & UNIT_THREAD_FLAG_CAN_WAIT) == 0)
            return;
        if ((thread->flags & UNIT_THREAD_FLAG_CAN_WAIT) != 0) {
            thread->wait_target.object = peer;
            thread->wait_kind = UNIT_WAIT_UNIT;
            thread->resume_frames = thread->frame_depth;
            thread->flags |= UNIT_THREAD_FLAG_WAITING;
        }
        thread->resume_frames = thread->frame_depth;
        thread->flags |= 1;
    }
}

/*
 * Helper for the Unit scale natives on X; UNIT_sclY and UNIT_sclZ work the
 * same way for Y and Z. Mode 0 takes a frame count and mode 1 a rate per
 * second.
 */
static void UNIT_sclX(int mode, JThread *thread, void *arguments,
                  u32 *failure_result)
{
    UnitScaleCall *call = arguments;
    JavaField *peer_field;
    JavaField *algorithm_field;
    MapUnitRecord *peer;
    float *scale_source;
    UnitSequenceEntry *sequence;
    UnitScaleChannel *scale;
    u8 *object;
    int wait;

    wait = 0;
    object = call->object;
    if (JNI_isInstanceOf(object, classJava_xeno_Unit) == 0) {
        *failure_result = 0;
        return;
    }

    peer_field = lookupClassField(classJava_xeno_Unit,
                                  loadConstString(unitPeerName, -1), 0);
    peer = *(MapUnitRecord **)(object + peer_field->offset);
    algorithm_field = lookupClassField(classJava_xeno_Unit,
                                       loadConstString(algorithmFieldName, -1), 0);

    scale_source = &peer->scale.x;
    if ((*(u32 *)(object + algorithm_field->offset) & 1) == 0)
        scale_source = defaultOffset_0043C1D0;

    sequence = &unitSequence[peer->slot];
    sequence->scaleSequence = SEQ_scaleUnit;
    sequence->state |= UNIT_SEQ_STATE_SCALE_X;
    scale = &sequence->scale;

    switch (mode) {
    case 0:
        scale->frames[0] = call->first.integer;
        scale->target[0] = call->second.floating + scale_source[0];
        wait = call->wait;
        break;
    case 1:
        scale->frames[0] = -1;
        scale->target[0] = call->first.floating + scale_source[0];
        scale->step[0] = call->second.floating / UNIT_FRAMES_PER_SECOND;
        wait = call->wait;
        break;
    }

    if (wait == 1) {
        peer->flags |= 2;
        if (thread->kind != UNIT_THREAD_KIND && (thread->flags & UNIT_THREAD_FLAG_CAN_WAIT) == 0)
            return;
        if ((thread->flags & UNIT_THREAD_FLAG_CAN_WAIT) != 0) {
            thread->wait_target.object = peer;
            thread->wait_kind = UNIT_WAIT_UNIT;
            thread->resume_frames = thread->frame_depth;
            thread->flags |= UNIT_THREAD_FLAG_WAITING;
        }
        thread->resume_frames = thread->frame_depth;
        thread->flags |= 1;
    }
}

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

/* Y-axis variant of UNIT_sclX. */
static void UNIT_sclY(int mode, JThread *thread, void *arguments,
                  u32 *failure_result)
{
    UnitScaleCall *call = arguments;
    JavaField *peer_field;
    JavaField *algorithm_field;
    MapUnitRecord *peer;
    float *scale_source;
    UnitSequenceEntry *sequence;
    UnitScaleChannel *scale;
    u8 *object;
    int wait;

    wait = 0;
    object = call->object;
    if (JNI_isInstanceOf(object, classJava_xeno_Unit) == 0) {
        *failure_result = 0;
        return;
    }

    peer_field = lookupClassField(classJava_xeno_Unit,
                                  loadConstString(unitPeerName, -1), 0);
    peer = *(MapUnitRecord **)(object + peer_field->offset);
    algorithm_field = lookupClassField(classJava_xeno_Unit,
                                       loadConstString(algorithmFieldName, -1), 0);

    scale_source = &peer->scale.x;
    if ((*(u32 *)(object + algorithm_field->offset) & 1) == 0)
        scale_source = defaultOffset_0043C1D0;

    sequence = &unitSequence[peer->slot];
    sequence->scaleSequence = SEQ_scaleUnit;
    sequence->state |= UNIT_SEQ_STATE_SCALE_Y;
    scale = &sequence->scale;

    switch (mode) {
    case 0:
        scale->frames[1] = call->first.integer;
        scale->target[1] = call->second.floating + scale_source[1];
        wait = call->wait;
        break;
    case 1:
        scale->frames[1] = -1;
        scale->target[1] = call->first.floating + scale_source[1];
        scale->step[1] = call->second.floating / UNIT_FRAMES_PER_SECOND;
        wait = call->wait;
        break;
    }

    if (wait == 1) {
        peer->flags |= 2;
        if (thread->kind != UNIT_THREAD_KIND && (thread->flags & UNIT_THREAD_FLAG_CAN_WAIT) == 0)
            return;
        if ((thread->flags & UNIT_THREAD_FLAG_CAN_WAIT) != 0) {
            thread->wait_target.object = peer;
            thread->wait_kind = UNIT_WAIT_UNIT;
            thread->resume_frames = thread->frame_depth;
            thread->flags |= UNIT_THREAD_FLAG_WAITING;
        }
        thread->resume_frames = thread->frame_depth;
        thread->flags |= 1;
    }
}

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

/* Z-axis variant of UNIT_sclX. */
static void UNIT_sclZ(int mode, JThread *thread, void *arguments,
                  u32 *failure_result)
{
    UnitScaleCall *call = arguments;
    JavaField *peer_field;
    JavaField *algorithm_field;
    MapUnitRecord *peer;
    float *scale_source;
    UnitSequenceEntry *sequence;
    UnitScaleChannel *scale;
    u8 *object;
    int wait;

    wait = 0;
    object = call->object;
    if (JNI_isInstanceOf(object, classJava_xeno_Unit) == 0) {
        *failure_result = 0;
        return;
    }

    peer_field = lookupClassField(classJava_xeno_Unit,
                                  loadConstString(unitPeerName, -1), 0);
    peer = *(MapUnitRecord **)(object + peer_field->offset);
    algorithm_field = lookupClassField(classJava_xeno_Unit,
                                       loadConstString(algorithmFieldName, -1), 0);

    scale_source = &peer->scale.x;
    if ((*(u32 *)(object + algorithm_field->offset) & 1) == 0)
        scale_source = defaultOffset_0043C1D0;

    sequence = &unitSequence[peer->slot];
    sequence->scaleSequence = SEQ_scaleUnit;
    sequence->state |= UNIT_SEQ_STATE_SCALE_Z;
    scale = &sequence->scale;

    switch (mode) {
    case 0:
        scale->frames[2] = call->first.integer;
        scale->target[2] = call->second.floating + scale_source[2];
        wait = call->wait;
        break;
    case 1:
        scale->frames[1] = -1;
        scale->target[2] = call->first.floating + scale_source[2];
        scale->step[2] = call->second.floating / UNIT_FRAMES_PER_SECOND;
        wait = call->wait;
        break;
    }

    if (wait == 1) {
        peer->flags |= 2;
        if (thread->kind != UNIT_THREAD_KIND && (thread->flags & UNIT_THREAD_FLAG_CAN_WAIT) == 0)
            return;
        if ((thread->flags & UNIT_THREAD_FLAG_CAN_WAIT) != 0) {
            thread->wait_target.object = peer;
            thread->wait_kind = UNIT_WAIT_UNIT;
            thread->resume_frames = thread->frame_depth;
            thread->flags |= UNIT_THREAD_FLAG_WAITING;
        }
        thread->resume_frames = thread->frame_depth;
        thread->flags |= 1;
    }
}

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

void Java_xeno_Unit_setCollision__Z(JThread *thread, UnitBoolCall *arguments)
{
    JavaField *peer_field;
    u8 *object;
    MapUnitRecord *peer;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Unit,
                                  loadConstString(unitPeerName, -1), 0);
    peer = *(MapUnitRecord **)(object + peer_field->offset);
    if (arguments->flag != 0) {
        peer->flags |= UNIT_PEER_FLAG_COLLISION;
    } else {
        peer->flags &= ~UNIT_PEER_FLAG_COLLISION;
    }
}

/*
 * Native Unit.setRotate: stores the rotationX/Y/Z fields into the peer's
 * rotation, converting degrees to radians.
 */
void Java_xeno_Unit_setRotate__(JThread *thread, UnitObjectCall *arguments)
{
    JavaField *peer_field;
    JavaField *field;
    u8 *object;
    Vector4 *vector;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Unit,
                                  loadConstString(unitPeerName, -1), 0);
    vector = &(*(MapUnitRecord **)(object + peer_field->offset))->rotation;
    field = lookupClassField(classJava_xeno_Unit,
                             loadConstString(unitRotationXName, -1), 0);
    vector->x = *(float *)(object + field->offset) / UNIT_DEGREES_HALF_TURN * UNIT_PI;
    field = lookupClassField(classJava_xeno_Unit,
                             loadConstString(unitRotationYName, -1), 0);
    vector->y = *(float *)(object + field->offset) / UNIT_DEGREES_HALF_TURN * UNIT_PI;
    field = lookupClassField(classJava_xeno_Unit,
                             loadConstString(unitRotationZName, -1), 0);
    vector->z = *(float *)(object + field->offset) / UNIT_DEGREES_HALF_TURN * UNIT_PI;
}

/* Native Unit.setTranslate: copies the positionX/Y/Z fields into the peer's position. */
void Java_xeno_Unit_setTranslate__(JThread *thread, UnitObjectCall *arguments)
{
    JavaField *peer_field;
    JavaField *field;
    u8 *object;
    Vector4 *vector;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Unit,
                                  loadConstString(unitPeerName, -1), 0);
    vector = &(*(MapUnitRecord **)(object + peer_field->offset))->position;
    field = lookupClassField(classJava_xeno_Unit,
                             loadConstString(unitPositionXName, -1), 0);
    vector->x = *(float *)(object + field->offset);
    field = lookupClassField(classJava_xeno_Unit,
                             loadConstString(unitPositionYName, -1), 0);
    vector->y = *(float *)(object + field->offset);
    field = lookupClassField(classJava_xeno_Unit,
                             loadConstString(unitPositionZName, -1), 0);
    vector->z = *(float *)(object + field->offset);
}

/* Native Unit.setVisible(int, boolean): passes the mode and visibility flag to MDL_setVisible for the unit's model. */
void Java_xeno_Unit_setVisible__IZ(JThread *thread, UnitVisibleCall *arguments)
{
    JavaField *peer_field;
    u8 *object;
    MapUnitRecord *peer;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Unit,
                                  loadConstString(unitPeerName, -1), 0);
    peer = *(MapUnitRecord **)(object + peer_field->offset);
    /* MDL_create builds the model record in place in the unit's byte block at
     * +0x240 (its 0x90 bytes are shorter than a full MdlHandle, so the block
     * cannot be declared as one). */
    MDL_setVisible((MdlHandle *)peer->modelBlock, arguments->mode, arguments->visible);
}

void Java_xeno_Unit_setVisible__Z(JThread *thread, UnitBoolCall *arguments)
{
    JavaField *peer_field;
    u8 *object;
    MapUnitRecord *peer;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Unit,
                                  loadConstString(unitPeerName, -1), 0);
    peer = *(MapUnitRecord **)(object + peer_field->offset);
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
    MapUnitRecord *peer;

    object = arguments->object;
    if (JNI_isInstanceOf(object, classJava_xeno_Unit) == 0) {
        *failure_result = 0;
        return;
    }
    peer_field = lookupClassField(classJava_xeno_Unit,
                                  loadConstString(unitPeerName, -1), 0);
    peer = *(MapUnitRecord **)(object + peer_field->offset);
    peer->signal = arguments->value;
}

/*
 * Native Unit.start(int, Object): sets the sequence mode. Mode 1 finds the
 * named method, attaches it to the unit's thread and clears the sequences.
 */
void Java_xeno_Unit_start__ILjava_lang_Object_(JThread *thread,
                                              UnitStartCall *arguments)
{
    JavaField *peer_field;
    u8 *object;
    int mode;
    MapUnitRecord *peer;
    UnitSequenceEntry *sequence;
    UnitString *method_name;
    UnitStringValue *value;
    JavaClass *scene_class;
    SceneString *name;
    JavaMethod *method;
    JThread *unit_thread;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Unit,
                                  loadConstString(unitPeerName, -1), 0);
    mode = arguments->mode;
    peer = *(MapUnitRecord **)(object + peer_field->offset);
    sequence = &unitSequence[peer->slot];
    sequence->mode = mode;
    switch (mode) {
    case 0:
        peer->update = MAP_updateUnitDefault;
        break;
    case 4:
        sequence->mode = mode;
        sequence->state = 0;
        UNIT_setUpdate(object, peer);
        break;
    case 5:
        sequence->state = 0;
        sequence->flags = 0;
        peer->update = MAP_updateUnitMPack;
        break;
    case 1:
        method_name = arguments->method_name;
        sequence->state = 0;
        value = method_name->value;
        scene_class = ((ObjectHeader *)object)->classPointer->clazz;
        name = loadConstString(value->data, value->length);
        method = findMethod(scene_class, name, TYPE_Void);
        sequence->method = method;
        unit_thread = JTHREAD_get(object);
        if (unit_thread != 0) {
            unit_thread->method = method;
            unit_thread->reset = JTHREAD_defaultUnit;
            JNI_initThread(unit_thread);
            unit_thread->flags |= UNIT_FLAG_SUSPENDED;
        }
        sequence->transSequence = 0;
        sequence->rotateSequence = 0;
        sequence->motionSequence = 0;
        sequence->scaleSequence = 0;
        sequence->mode = mode;
        UNIT_setUpdate(object, peer);
        break;
    }
}

/*
 * Native Unit.stop: clears the suspended flag on the unit's thread and copies
 * the peer's position and rotation into the Java fields.
 */
void Java_xeno_Unit_stop__(JThread *thread, UnitObjectCall *arguments)
{
    JavaField *peer_field;
    JavaField *field;
    u8 *object;
    MapUnitRecord *peer;
    JThread *unit_thread;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Unit,
                                  loadConstString(unitPeerName, -1), 0);
    peer = *(MapUnitRecord **)(object + peer_field->offset);
    unit_thread = JTHREAD_get(object);
    if (unit_thread != 0) {
        unit_thread->flags &= ~UNIT_FLAG_SUSPENDED;
    }
    field = lookupClassField(classJava_xeno_Unit,
                             loadConstString(unitPositionXName, -1), 0);
    *(float *)(object + field->offset) = peer->position.x;
    field = lookupClassField(classJava_xeno_Unit,
                             loadConstString(unitPositionYName, -1), 0);
    *(float *)(object + field->offset) = peer->position.y;
    field = lookupClassField(classJava_xeno_Unit,
                             loadConstString(unitPositionZName, -1), 0);
    *(float *)(object + field->offset) = peer->position.z;
    field = lookupClassField(classJava_xeno_Unit,
                             loadConstString(unitRotationXName, -1), 0);
    *(float *)(object + field->offset) = peer->rotation.x;
    field = lookupClassField(classJava_xeno_Unit,
                             loadConstString(unitRotationYName, -1), 0);
    *(float *)(object + field->offset) = peer->rotation.y;
    field = lookupClassField(classJava_xeno_Unit,
                             loadConstString(unitRotationZName, -1), 0);
    *(float *)(object + field->offset) = peer->rotation.z;
}

void Java_xeno_Unit_validate__(void)
{
}

/*
 * Native Unit.setParent(Object, int): sets the peer's parent to the chr's peer.
 * When the accessory flag is set, the joint index is resolved with
 * ACT_jointGetAccessories first.
 */
void Java_xeno_Unit_setParent__Ljava_lang_Object_I(
    JThread *thread, UnitParentCall *arguments)
{
    JavaField *peer_field;
    u8 *object;
    u8 *chr;
    int flags;
    MapUnitRecord *peer;
    void *parent_peer;

    object = arguments->object;
    chr = arguments->chr;
    flags = arguments->flags;
    peer_field = lookupClassField(classJava_xeno_Unit,
                                  loadConstString(unitPeerName, -1), 0);
    peer = *(MapUnitRecord **)(object + peer_field->offset);
    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(unitPeerName, -1), 0);
    parent_peer = *(void **)(chr + peer_field->offset);
    peer->parent_peer = parent_peer;
    if ((flags & UNIT_JOINT_ACCESSORY_FLAG) != 0) {
        peer->parent_flags = ACT_jointGetAccessories(parent_peer,
                                                     flags & UNIT_JOINT_ACCESSORY_MASK) | UNIT_JOINT_ACCESSORY_FLAG;
    } else {
        peer->parent_flags = flags;
    }
    UNIT_setUpdate(object, peer);
}

/* Copies `count` bytes from src to dst; does nothing when count is not positive. */
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
 * Native Unit.setArgs(int, int, int): writes `value` into the peer's argument
 * bytes at `offset`, for sizes 1 to 4.
 */
void Java_xeno_Unit_setArgs__III(JThread *thread, UnitArgsWordCall *arguments)
{
    JavaField *peer_field;
    MapUnitRecord *peer;
    int value;
    u8 *object;
    int offset;
    int size;

    value = arguments->value;
    object = arguments->object;
    offset = arguments->offset;
    size = arguments->size;
    peer_field = lookupClassField(classJava_xeno_Unit,
                                  loadConstString(unitPeerName, -1), 0);
    peer = *(MapUnitRecord **)(object + peer_field->offset);
    if ((unsigned int)(size - 1) < 4U) {
        copyArgs(peer->args + offset, (u8 *)&value, size);
    }
}

/*
 * Native Unit.getArgs(int, int): returns `size` bytes (1 to 4) of the peer's
 * arguments at `offset`.
 */
void Java_xeno_Unit_getArgs__II(JThread *thread, UnitArgsGetCall *arguments,
                                u32 *failure_result)
{
    JavaField *peer_field;
    u8 *object;
    MapUnitRecord *peer;
    int size;
    int offset;
    unsigned int check;
    int result;

    size = arguments->size;
    object = arguments->object;
    offset = arguments->offset;
    peer_field = lookupClassField(classJava_xeno_Unit,
                                  loadConstString(unitPeerName, -1), 0);
    peer = *(MapUnitRecord **)(object + peer_field->offset);
    check = (unsigned int)(size - 1);
    if (check < 4U) {
        copyArgs((u8 *)&result, peer->args + offset, size);
        *failure_result = (u32)result;
    }
}

/*
 * Native Unit.setArgs(int, Object, int): copies `size` bytes from the source
 * buffer into the peer's arguments.
 */
void Java_xeno_Unit_setArgs__ILjava_lang_Object_I(JThread *thread,
                                                  UnitArgsSetCall *arguments)
{
    JavaField *peer_field;
    u8 *object;
    MapUnitRecord *peer;
    UnitArgsSource *source;
    int size;

    object = arguments->object;
    source = arguments->source;
    size = arguments->size;
    peer_field = lookupClassField(classJava_xeno_Unit,
                                  loadConstString(unitPeerName, -1), 0);
    peer = *(MapUnitRecord **)(object + peer_field->offset);
    if (size > 0) {
        copyArgs(peer->args, source->data, size);
    }
}

/* Native Unit.setArgs (row form): stores the four words into argument row `row` of the peer. */
void Java_xeno_Unit_setArgs__IIIII(JThread *thread,
                                   UnitArgsRowCall *arguments)
{
    JavaField *peer_field;
    u8 *object;
    int row;
    UnitArgsPeer *peer;

    object = arguments->object;
    row = arguments->row;
    peer_field = lookupClassField(classJava_xeno_Unit,
                                  loadConstString(unitPeerName, -1), 0);
    peer = *(UnitArgsPeer **)(object + peer_field->offset);
    peer->rows[row].word[0] = arguments->word[0];
    peer->rows[row].word[1] = arguments->word[1];
    peer->rows[row].word[2] = arguments->word[2];
    peer->rows[row].word[3] = arguments->word[3];
}

/* Native Unit.getScale: returns the peer's scale as a Vector4f held in a static buffer. */
void Java_xeno_Unit_getScale__(JThread *thread, UnitObjectCall *arguments,
                               UnitResultValue *result)
{
    static UnitScaleVector scale;
    JavaField *peer_field;
    u8 *object;
    MapUnitRecord *peer;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Unit,
                                  loadConstString(unitPeerName, -1), 0);
    peer = *(MapUnitRecord **)(object + peer_field->offset);
    scale.class_ref = classJava_xeno_util_Vector4f->classPointer;
    scale.x = peer->scale.x;
    scale.y = peer->scale.y;
    scale.z = peer->scale.z;
    scale.w = peer->scale.w;
    result->object = &scale;
}

void Java_xeno_Unit_setScale__FFF(JThread *thread, UnitVector3Call *arguments)
{
    JavaField *peer_field;
    u8 *object;
    MapUnitRecord *peer;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Unit,
                                  loadConstString(unitPeerName, -1), 0);
    peer = *(MapUnitRecord **)(object + peer_field->offset);
    peer->scale.x = arguments->x;
    peer->scale.y = arguments->y;
    peer->scale.z = arguments->z;
}

void Java_xeno_Unit_getSerial__(JThread *thread, UnitObjectCall *arguments,
                                u32 *failure_result)
{
    JavaField *peer_field;
    u8 *object;
    MapUnitRecord *peer;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Unit,
                                  loadConstString(unitPeerName, -1), 0);
    peer = *(MapUnitRecord **)(object + peer_field->offset);
    *failure_result = peer->slot;
}

void Java_xeno_Unit_mtnSetMask__I(JThread *thread, UnitIntCall *arguments)
{
    JavaField *peer_field;
    u8 *object;
    MapUnitRecord *peer;
    int mask;

    mask = arguments->value;
    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Unit,
                                  loadConstString(unitPeerName, -1), 0);
    peer = *(MapUnitRecord **)(object + peer_field->offset);
    peer->motion_mask = mask;
}

/*
 * Native Unit.mtnGetRoot(int, Vector4f): copies the motion root selected by
 * `type` into the vector, converting type 1 from radians to degrees.
 * Does nothing for a null vector.
 */
void Java_xeno_Unit_mtnGetRoot__ILxeno_util_Vector4f_(
    JThread *thread, UnitMotionRootCall *arguments)
{
    JavaField *peer_field;
    u8 *object;
    MapUnitRecord *peer;
    UnitPivotVector *vector;
    int type;

    object = arguments->object;
    type = arguments->type;
    vector = arguments->vector;
    peer_field = lookupClassField(classJava_xeno_Unit,
                                  loadConstString(unitPeerName, -1), 0);
    peer = *(MapUnitRecord **)(object + peer_field->offset);
    if (vector != 0) {
        switch (type) {
        case 0:
            vector->x = peer->motion_root[0].x;
            vector->y = peer->motion_root[0].y;
            vector->z = peer->motion_root[0].z;
            break;
        case 1:
            vector->x = peer->motion_root[1].x / UNIT_PI * UNIT_DEGREES_HALF_TURN;
            vector->y = peer->motion_root[1].y / UNIT_PI * UNIT_DEGREES_HALF_TURN;
            vector->z = peer->motion_root[1].z / UNIT_PI * UNIT_DEGREES_HALF_TURN;
            break;
        case 2:
            vector->x = peer->motion_root[2].x;
            vector->y = peer->motion_root[2].y;
            vector->z = peer->motion_root[2].z;
            break;
        }
    }
}

void Java_xeno_Unit_getState__(JThread *thread, UnitObjectCall *arguments,
                               u32 *failure_result)
{
    JavaField *peer_field;
    u8 *object;
    MapUnitRecord *peer;
    UnitSequenceEntry *entry;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Unit,
                                  loadConstString(unitPeerName, -1), 0);
    peer = *(MapUnitRecord **)(object + peer_field->offset);
    entry = &unitSequence[peer->slot];
    *failure_result = entry->state;
}

/* Native Unit.getPivot: copies the sequence pivot into the vector when it is not null. */
void Java_xeno_Unit_getPivot__Lxeno_util_Vector4f_(JThread *thread,
                                                   UnitPivotOutCall *arguments)
{
    JavaField *peer_field;
    u8 *object;
    MapUnitRecord *peer;
    UnitSequenceEntry *entry;
    UnitPivotVector *vector;

    vector = arguments->vector;
    object = arguments->object;
    if (vector != 0) {
        peer_field = lookupClassField(classJava_xeno_Unit,
                                      loadConstString(unitPeerName, -1), 0);
        peer = *(MapUnitRecord **)(object + peer_field->offset);
        entry = &unitSequence[peer->slot];
        vector->x = entry->pivot_x;
        vector->y = entry->pivot_y;
        vector->z = entry->pivot_z;
    }
}

void Java_xeno_Unit_setPivot__FFF(JThread *thread, UnitVector3Call *arguments)
{
    JavaField *peer_field;
    u8 *object;
    MapUnitRecord *peer;
    UnitSequenceEntry *entry;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Unit,
                                  loadConstString(unitPeerName, -1), 0);
    peer = *(MapUnitRecord **)(object + peer_field->offset);
    entry = &unitSequence[peer->slot];
    entry->pivot_x = arguments->x;
    entry->pivot_y = arguments->y;
    entry->pivot_z = arguments->z;
}

/* Native Unit.getAxis: copies the sequence axis into the vector, with w set to 1, when it is not null. */
void Java_xeno_Unit_getAxis__Lxeno_util_Vector4f_(JThread *thread,
                                                  UnitAxisOutCall *arguments)
{
    JavaField *peer_field;
    u8 *object;
    MapUnitRecord *peer;
    UnitSequenceEntry *entry;
    UnitAxisVector *vector;

    vector = arguments->vector;
    object = arguments->object;
    if (vector != 0) {
        peer_field = lookupClassField(classJava_xeno_Unit,
                                      loadConstString(unitPeerName, -1), 0);
        peer = *(MapUnitRecord **)(object + peer_field->offset);
        entry = &unitSequence[peer->slot];
        vector->x = entry->axis_x;
        vector->y = entry->axis_y;
        vector->z = entry->axis_z;
        vector->w = 1.0f;
    }
}

void Java_xeno_Unit_setAxis__FFFF(JThread *thread, UnitAxisCall *arguments)
{
    JavaField *peer_field;
    u8 *object;
    MapUnitRecord *peer;
    UnitSequenceEntry *entry;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Unit,
                                  loadConstString(unitPeerName, -1), 0);
    peer = *(MapUnitRecord **)(object + peer_field->offset);
    entry = &unitSequence[peer->slot];
    entry->axis_x = arguments->x;
    entry->axis_y = arguments->y;
    entry->axis_z = arguments->z;
    entry->axis_w = arguments->w;
}

static void unit_suspend(int *flags)
{
    *flags |= UNIT_FLAG_SUSPENDED;
}

static void unit_resume(int *flags)
{
    *flags &= ~UNIT_FLAG_SUSPENDED;
}

void Java_xeno_Unit_suspend__I(JThread *thread, UnitGroupCall *arguments)
{
    MAP_callUnitGroup(arguments->group, unit_suspend);
}

void Java_xeno_Unit_resume__I(JThread *thread, UnitGroupCall *arguments)
{
    MAP_callUnitGroup(arguments->group, unit_resume);
}

/*
 * Native Unit.initElevatorFunc: sets the peer's elevator task to tyaElevatorTask
 * and runs it once with the positionY field's offset.
 */
void Java_xeno_Unit_initElevatorFunc__(JThread *thread,
                                       UnitObjectCall *arguments)
{
    JavaField *peer_field;
    u8 *object;
    JavaField *py_field;
    MapUnitRecord *peer;
    int py_offset;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Unit,
                                  loadConstString(unitPeerName, -1), 0);
    peer = *(MapUnitRecord **)(object + peer_field->offset);
    py_field = lookupClassField(classJava_xeno_Unit,
                                loadConstString(unitPositionYName, -1), 0);
    py_offset = py_field->offset;
    peer->elevator_task = tyaElevatorTask;
    peer->elevator_state = 0;
    peer->elevator_field = object + py_offset;
    tyaElevatorTask(peer, py_offset);
}

void Java_xeno_Unit_map_shadow__I(JThread *thread, UnitIntCall *arguments)
{
    JavaField *peer_field;
    u8 *object;
    MapUnitRecord *peer;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Unit,
                                  loadConstString(unitPeerName, -1), 0);
    peer = *(MapUnitRecord **)(object + peer_field->offset);
    if (arguments->value != 0) {
        peer->flags |= UNIT_PEER_FLAG_MAP_SHADOW;
    } else {
        peer->flags &= ~UNIT_PEER_FLAG_MAP_SHADOW;
    }
}

void Java_xeno_Unit_renderCommand__I(JThread *thread, UnitIntCall *arguments)
{
    JavaField *peer_field;
    u8 *object;
    MapUnitRecord *peer;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Unit,
                                  loadConstString(unitPeerName, -1), 0);
    peer = *(MapUnitRecord **)(object + peer_field->offset);
    peer->render_command = arguments->value;
}

/*
 * Native Unit.setFilter(int): the filter mode is 1 for value 2, 2 for value 3,
 * and 0 otherwise. Values 2 and 3 also set the result to 4.
 */
void Java_xeno_Unit_setFilter__I(JThread *thread, UnitIntCall *arguments,
                                 u32 *failure_result)
{
    JavaField *peer_field;
    u8 *object;
    MapUnitRecord *peer;
    int value;
    int offset;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Unit,
                                  loadConstString(unitPeerName, -1), 0);
    offset = peer_field->offset;
    value = arguments->value;
    peer = *(MapUnitRecord **)(object + offset);
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

/* Native Unit.setFilterParam(float[]): copies the array into the peer's filter parameters. */
void Java_xeno_Unit_setFilterParam__aF(JThread *thread,
                                       UnitFilterParamCall *arguments)
{
    JavaField *peer_field;
    u8 *object;
    MapUnitRecord *peer;
    UnitFloatArray *values;
    float *destination;
    unsigned int count;
    unsigned int i;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Unit,
                                  loadConstString(unitPeerName, -1), 0);
    values = arguments->values;
    count = values->length;
    peer = *(MapUnitRecord **)(object + peer_field->offset);
    destination = peer->filter_params;
    for (i = 0; i < count; i++)
        destination[i] = values->data[i];
}

void Java_xeno_Unit_setShadow__II(JThread *thread, UnitShadowCall *arguments)
{
    JavaField *peer_field;
    MapUnitRecord *peer;

    peer_field = lookupClassField(classJava_xeno_Unit,
                                  loadConstString(unitPeerName, -1), 0);
    peer = *(MapUnitRecord **)(arguments->object + peer_field->offset);
    peer->shadow_x = arguments->x;
    peer->shadow_y = arguments->y;
}

void Java_xeno_Unit_shadow_clip_scale__F(JThread *thread,
                                         UnitFloatCall *arguments)
{
    JavaField *peer_field;
    MapUnitRecord *peer;

    peer_field = lookupClassField(classJava_xeno_Unit,
                                  loadConstString(unitPeerName, -1), 0);
    peer = *(MapUnitRecord **)(arguments->object + peer_field->offset);
    peer->shadow_clip_scale = arguments->value;
}

/* Native Unit.shadow_map_id(int): appends the id to the peer's shadow map list, up to 8 entries. */
void Java_xeno_Unit_shadow_map_id__I(JThread *thread, UnitIntCall *arguments)
{
    JavaField *peer_field;
    MapUnitRecord *peer;

    peer_field = lookupClassField(classJava_xeno_Unit,
                                  loadConstString(unitPeerName, -1), 0);
    peer = *(MapUnitRecord **)(arguments->object + peer_field->offset);
    if (peer->shadow_map_count < 8) {
        peer->shadow_map_ids[peer->shadow_map_count++] = arguments->value;
    }
}

void Java_xeno_Unit_shadow_map_reset__(JThread *thread,
                                       UnitObjectCall *arguments)
{
    JavaField *peer_field;
    MapUnitRecord *peer;

    peer_field = lookupClassField(classJava_xeno_Unit,
                                  loadConstString(unitPeerName, -1), 0);
    peer = *(MapUnitRecord **)(arguments->object + peer_field->offset);
    peer->shadow_map_count = 0;
}

void Java_xeno_Unit_setSortOffset__F(JThread *thread, UnitFloatCall *arguments)
{
    JavaField *peer_field;
    MapUnitRecord *peer;

    peer_field = lookupClassField(classJava_xeno_Unit,
                                  loadConstString(unitPeerName, -1), 0);
    peer = *(MapUnitRecord **)(arguments->object + peer_field->offset);
    peer->sort_offset = (int) arguments->value;
}

void Java_xeno_Unit_setClip__I(JThread *thread, UnitIntCall *arguments)
{
    JavaField *peer_field;
    u8 *object;
    MapUnitRecord *peer;
    int value;

    peer_field = lookupClassField(classJava_xeno_Unit,
                                  loadConstString(unitPeerName, -1), 0);
    value = arguments->value;
    object = arguments->object;
    peer = *(MapUnitRecord **)(object + peer_field->offset);
    if (value != 0) {
        peer->flags |= UNIT_PEER_FLAG_CLIP;
    } else {
        peer->flags &= ~UNIT_PEER_FLAG_CLIP;
    }
}

void Java_xeno_Unit_setMonitorPrio__I(JThread *thread, UnitIntCall *arguments)
{
    JavaField *peer_field;
    u8 *object;
    MapUnitRecord *peer;
    int value;
    int offset;

    peer_field = lookupClassField(classJava_xeno_Unit,
                                  loadConstString(unitPeerName, -1), 0);
    value = arguments->value;
    offset = peer_field->offset;
    object = arguments->object;
    peer = *(MapUnitRecord **)(object + offset);
    if (value != 0) {
        peer->monitor_priority = 1;
        return;
    }
    peer->monitor_priority = 0;
}

/* Defined here, not with the data above: moving it before the functions that use it changes the original code or data order. */
/* The eight-byte name strings follow the function bodies, as in the original data order. */
const char unitPeerName[8] = "peer";

const char unitPositionYName[8] = "py";

const char unitRotationXName[8] = "rx";

const char unitRotationYName[8] = "ry";

const char unitRotationZName[8] = "rz";

const char unitPositionXName[8] = "px";

const char unitPositionZName[8] = "pz";
