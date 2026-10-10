#include "common.h"

#include "shared.h"
#include "main/jni.h"

#include "unit.h"

/* Partial native field views; unknown spans remain byte storage. */
typedef struct JThreadHandle JThreadHandle;
typedef struct JThreadNode JThreadNode;
struct JThread {
    unsigned int : 32;                  /* +0x00 */
    struct JThread *previous;           /* +0x04 */
    struct JThread *next;               /* +0x08 */
    u8 kind;                            /* +0x0c */
    u8 wait_kind;                       /* +0x0d */
    unsigned short : 16;                /* +0x0e */
    void *object;                       /* +0x10 */
    void (*reset)(JThreadHandle *thread); /* +0x14 */
    void *method;                       /* +0x18 */
    unsigned int : 32;                  /* +0x1c */
    unsigned int : 32;                  /* +0x20 */
    u32 flags;                          /* +0x24 */
    u32 *stack;                         /* +0x28 */
    void *frames;                       /* +0x2c */
    void *wait_target;                  /* +0x30 */
    int wait_parameter;                 /* +0x34 */
    u16 stack_offset;                   /* +0x38 */
    short stack_limit;                  /* +0x3a */
    u16 resume_frames;                  /* +0x3c */
    u16 frame_depth;                    /* +0x3e */
    short frame_limit;                  /* +0x40 */
};

#define UNIT_THREAD_KIND 5
#define UNIT_WAIT_UNIT 6
extern SceneString *TYPE_Void;
extern SceneMethod *findMethod(SceneClass *scene_class, SceneString *name,
                              void *signature_or_type);
extern void JNI_initThread(SceneVm *vm);
extern void JTHREAD_defaultUnit(JThreadHandle *thread);
extern JThreadNode *JTHREAD_get(SceneObject object);

typedef struct UnitVector3 {
    float x;
    float y;
    float z;
} UnitVector3;

typedef struct UnitMotionVector {
    float x;
    float y;
    float z;
    unsigned char unmodeled_0c[4];
} UnitMotionVector;

typedef struct UnitNativePeer {
    u32 flags;                                       /* +0x00 */
    void (*update)(struct UnitPeer *peer);            /* +0x04 */
    unsigned char unmodeled_08[0x0c - 0x08];
    int (*elevator_task)(void *peer, int field_offset); /* +0x0c */
    UnitVector3 position;                             /* +0x10 */
    unsigned char unmodeled_1c[0x20 - 0x1c];
    UnitVector3 rotation;                             /* +0x20 */
    unsigned char unmodeled_2c[0x30 - 0x2c];
    float scale_x;                                   /* +0x30 */
    float scale_y;                                   /* +0x34 */
    float scale_z;                                   /* +0x38 */
    float scale_w;                                   /* +0x3c */
    unsigned char unmodeled_40[0xa0 - 0x40];
    u8 serial;                                        /* +0xa0 */
    u8 signal;                                        /* +0xa1 */
    unsigned char unmodeled_a2[0xa6 - 0xa2];
    short monitor_priority;                           /* +0xa6 */
    unsigned char unmodeled_a8[0xd4 - 0xa8];
    u32 update_stamp;                                 /* +0xd4 */
    unsigned char unmodeled_d8[0xf8 - 0xd8];
    u32 parent_flags;                                 /* +0xf8 */
    void *parent_peer;                                /* +0xfc */
    unsigned char unmodeled_100[0x118 - 0x100];
    short motion_mask;                                /* +0x118 */
    unsigned char unmodeled_11a[0x170 - 0x11a];
    UnitMotionVector motion_root[3];                  /* +0x170 */
    unsigned char args[0x1b8 - 0x1a0];                /* +0x1a0 */
    void *elevator_field;                             /* +0x1b8 */
    unsigned char unmodeled_1bc[0x1c0 - 0x1bc];
    int elevator_state;                               /* +0x1c0 */
    unsigned char unmodeled_1c4[0x234 - 0x1c4];
    int shadow_x;                                     /* +0x234 */
    int shadow_y;                                     /* +0x238 */
    float shadow_clip_scale;                          /* +0x23c */
    unsigned char model[0x2d0 - 0x240];
    unsigned short shadow_map_ids[8];               /* +0x240 */
    int shadow_map_count;                             /* +0x2e0 */
    int render_command;                               /* +0x2e4 */
    int filter_mode;                                  /* +0x2e8 */
    int sort_offset;                                  /* +0x2ec */
} UnitNativePeer;

typedef struct UnitTrack {
    void *source;                                     /* +0x00 */
    short frame;                                      /* +0x04 */
    u16 last_frame;                                   /* +0x06 */
    u16 flags;                                        /* +0x08 */
} UnitTrack;

typedef struct UnitTransSequence {
    UnitTrack track;                                  /* +0x00 */
    unsigned char unmodeled_0c[0x10 - 0x0c];
    float x;                                          /* +0x10 */
    float y;                                          /* +0x14 */
    float z;                                          /* +0x18 */
    unsigned char unmodeled_1c[0x40 - 0x1c];
    int param;                                        /* +0x40 */
} UnitTransSequence;

typedef struct UnitRotChannel {
    UnitTrack track;                                  /* +0x00 */
    unsigned char unmodeled_0c[0x10 - 0x0c];
    int frames[3];                                    /* +0x10 */
    u32 flags;                                        /* +0x1c */
    unsigned char unmodeled_20[0x30 - 0x20];
    float target[3];                                  /* +0x30 */
    unsigned char unmodeled_3c[0x40 - 0x3c];
    float step[3];                                    /* +0x40 */
} UnitRotChannel;

typedef struct UnitScaleChannel {
    UnitTrack track;                                  /* +0x00 */
    unsigned char unmodeled_0c[0x10 - 0x0c];
    int frames[3];                                    /* +0x10 */
    unsigned char unmodeled_1c[0x30 - 0x1c];
    float target[3];                                  /* +0x30 */
    unsigned char unmodeled_3c[0x40 - 0x3c];
    float step[3];                                    /* +0x40 */
} UnitScaleChannel;

typedef struct UnitMotionChannel {
    unsigned char unmodeled_00[0x08];
    int motion;                                       /* +0x08 */
    unsigned char unmodeled_0c[0x10 - 0x0c];
    short first_frame;                                /* +0x10 */
    short last_frame;                                 /* +0x12 */
    u8 option;                                        /* +0x14 */
    unsigned char unmodeled_15[0x18 - 0x15];
    int blend;                                        /* +0x18 */
    float speed;                                      /* +0x1c */
} UnitMotionChannel;

typedef struct UnitMoveChannel {
    void *source;                                     /* +0x00 */
    int frames;                                       /* +0x04 */
    unsigned char unmodeled_08[0x20 - 0x08];
    float target_x;                                   /* +0x20 */
    unsigned char unmodeled_24[0x28 - 0x24];
    float target_z;                                   /* +0x28 */
    float step;                                       /* +0x2c */
} UnitMoveChannel;

typedef struct UnitNativeSequence {
    u32 flags;                                        /* +0x00 */
    int state;                                        /* +0x04 */
    unsigned char unmodeled_08[0x0c - 0x08];
    int mode;                                         /* +0x0c */
    unsigned char unmodeled_10[0x14 - 0x10];
    SceneMethod *method;                              /* +0x14 */
    unsigned char unmodeled_18[0x24 - 0x18];
    void (*transSequence)(void *unit);                /* +0x24 */
    void (*rotateSequence)(void *unit);                 /* +0x28 */
    void (*motionSequence)(void *unit);               /* +0x2c */
    void (*scaleSequence)(void *unit);                /* +0x30 */
    unsigned char unmodeled_34[0x38 - 0x34];
    UnitTransSequence trans;                          /* +0x38 */
    unsigned char unmodeled_7c[0xb8 - 0x7c];
    UnitRotChannel rotation;                          /* +0xb8 */
    unsigned char unmodeled_104[0x138 - 0x104];
    UnitMotionChannel motion;                         /* +0x138 */
    unsigned char unmodeled_158[0x1b8 - 0x158];
    UnitScaleChannel scale;                           /* +0x1b8 */
    unsigned char unmodeled_204[0x240 - 0x204];
    float pivot_x;                                    /* +0x240 */
    float pivot_y;                                    /* +0x244 */
    float pivot_z;                                    /* +0x248 */
    unsigned char unmodeled_24c[0x250 - 0x24c];
    float axis_x;                                      /* +0x250 */
    float axis_y;                                      /* +0x254 */
    float axis_z;                                      /* +0x258 */
    float axis_w;                                      /* +0x25c */
} UnitNativeSequence;

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


/* The original global actor table contains 64 records of 0xa70 bytes
 * (symbol-table size 0x29c00). These are this TU's storage view: the known
 * member extents come from the independent actor call sites documented in
 * src/main/act_2.h; unnamed spans remain byte storage. */

typedef struct UnitActorAnimSlot {
    unsigned char unmodeled_00[0x14];
    unsigned short currentDataId;
} UnitActorAnimSlot;

typedef struct UnitActorStorage {
    u32 flags;
    void (*update)(struct UnitActorStorage *actor);
    void (*draw)(struct UnitActorStorage *actor);
    u32 quadword_alignment_gap;
    Vector4 position;
    Vector4 previous_position;
    Vector4 velocity;
    Vector4 acceleration;
    Vector4 rotation;
    Vector4 scale;
    unsigned char unmodeled_70[0x6f0 - 0x70];
    UnitActorAnimSlot animSlot;
    unsigned char unmodeled_706[0x71c - 0x706];
    void *animData;
    void *animUserData;
    unsigned char unmodeled_724[0x7fc - 0x724];
    int moveElementId;
    unsigned char unmodeled_800[0x840 - 0x800];
    unsigned char model[0x58];
    unsigned char unmodeled_898[0x8d8 - 0x898];
    void *move;
    void *animPackTables[8];
    unsigned char unmodeled_8fc[0xa70 - 0x8fc];
} UnitActorStorage;

UnitActorStorage actor[64] = { 0 };

extern const char D_004DC1D0[];

extern const char D_004DC1F8[];

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

#define UNIT_FLAG_SUSPENDED 0x10

/* MAP_callUnitGroup is still asm elsewhere (src/main/map_create_unit_peer.c):
 * it applies `callback` to every unit record of group `group`. */

extern void MAP_callUnitGroup(int group, void (*callback)(int *flags));

const char D_004D16F8[10] = "algorithm";

extern const char D_004DC1D8[];

extern const char D_004DC1E0[];

extern const char D_004DC1E8[];

extern const char D_004DC1F0[];

extern const char D_004DC200[];

extern float defaultOffset_0043C1D0[4];

extern unsigned int GameLoopState[];

extern void MAP_updateUnitDefault(UnitPeer *peer);

extern void MAP_updateUnitMPack(UnitPeer *peer);

extern void MAP_updateUnitSequence(UnitPeer *peer);

extern void MAP_updateUnitPartsSequence(UnitPeer *peer);

extern void MAP_updateUnitPartsSequence2(UnitPeer *peer);

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

static void UNIT_setUpdate(u8 *object, UnitNativePeer *unit)
{
    JavaField *state_field;
    int *state;
    u32 frame;

    state_field = lookupClassField(classJava_xeno_Unit,
                                   loadConstString(D_004D16F8, -1), 0);
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

void Java_xeno_Unit_rotYCNS__Ljava_lang_Object_IFFF(
    JThread *thread, UnitChrSequenceCall *arguments)
{
    JavaField *peer_field;
    u8 *object;
    UnitNativePeer *peer;
    UnitNativeSequence *sequence;
    UnitRotChannel *block;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Unit,
                                  loadConstString(D_004DC1D0, -1), 0);
    peer = *(UnitNativePeer **)(object + peer_field->offset);
    UNIT_setUpdate(object, peer);
    sequence = (UnitNativeSequence *)&unitSequence[peer->serial];
    block = &sequence->rotation;
    sequence->state |= 4;
    sequence->rotateSequence = SEQ_rotYCNSUnitChr;
    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(D_004DC1D0, -1), 0);
    block->track.source = *(void **)(arguments->chr + peer_field->offset);
    block->frames[1] = arguments->param.integer;
    block->target[0] = arguments->x.floating;
    block->target[1] = arguments->y.floating;
    block->target[2] = arguments->z.floating;
}

void Java_xeno_Unit_transCNS__Ljava_lang_Object_IFFF(
    JThread *thread, UnitChrSequenceCall *arguments)
{
    JavaField *peer_field;
    u8 *object;
    UnitNativePeer *peer;
    UnitNativeSequence *sequence;
    UnitTransSequence *block;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Unit,
                                  loadConstString(D_004DC1D0, -1), 0);
    peer = *(UnitNativePeer **)(object + peer_field->offset);
    UNIT_setUpdate(object, peer);
    sequence = (UnitNativeSequence *)&unitSequence[peer->serial];
    block = &sequence->trans;
    sequence->state |= 1;
    sequence->transSequence = SEQ_transCNSUnitChr;
    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(D_004DC1D0, -1), 0);
    block->track.source = *(void **)(arguments->chr + peer_field->offset);
    block->param = arguments->param.integer;
    block->x = arguments->x.floating;
    block->y = arguments->y.floating;
    block->z = arguments->z.floating;
}

static void UNIT_moveXZ(int mode, JThread *thread, void *arguments,
                u32 *failure_result)
{
    UnitMoveCall *call = arguments;
    JavaField *peer_field;
    JavaField *algorithm_field;
    UnitNativePeer *peer;
    float *source;
    UnitNativeSequence *sequence;
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
                                  loadConstString(D_004DC1D0, -1), 0);
    peer = *(UnitNativePeer **)(object + peer_field->offset);
    sequence = (UnitNativeSequence *)&unitSequence[peer->serial];
    sequence->state |= 1;
    channel = (UnitMoveChannel *)&sequence->trans;
    algorithm_field = lookupClassField(classJava_xeno_Unit,
                                       loadConstString(D_004D16F8, -1), 0);

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
        channel->step = call->slot[2].floating / 30.0f;
        sequence->transSequence = SEQ_moveUnitXZ;
        wait = call->slot[3].byte;
        break;
    case 2:
        peer_field = lookupClassField(classJava_xeno_Chr,
                                      loadConstString(D_004DC1D0, -1), 0);
        chr_peer = *(void **)(call->slot[1].object + peer_field->offset);
        channel->frames = call->slot[0].integer;
        channel->source = chr_peer;
        sequence->transSequence = SEQ_moveChr;
        wait = call->slot[2].byte;
        break;
    case 3:
        peer_field = lookupClassField(classJava_xeno_Chr,
                                      loadConstString(D_004DC1D0, -1), 0);
        chr_peer = *(void **)(call->slot[0].object + peer_field->offset);
        channel->source = chr_peer;
        channel->frames = -1;
        channel->step = call->slot[1].floating / 30.0f;
        sequence->transSequence = SEQ_moveChr;
        wait = call->slot[2].byte;
        break;
    }

    if (wait == 1) {
        peer->flags |= 2;
        if (thread->kind != UNIT_THREAD_KIND && (thread->flags & 0x40) == 0)
            return;
        if ((thread->flags & 0x40) != 0) {
            thread->wait_target = peer;
            thread->wait_kind = UNIT_WAIT_UNIT;
            thread->resume_frames = thread->frame_depth;
            thread->flags |= 4;
        }
        thread->resume_frames = thread->frame_depth;
        thread->flags |= 1;
    }
}

INCLUDE_ASM("asm/main/nonmatchings/unit", UNIT_rotX);

static void UNIT_rotY(int mode, JThread *thread, void *arguments,
               u32 *failure_result)
{
    UnitRotCall *call = arguments;
    JavaField *peer_field;
    JavaField *algorithm_field;
    UnitNativePeer *peer;
    float *source;
    UnitNativeSequence *sequence;
    UnitRotChannel *channel;
    void *chr_peer;
    u8 *object;
    int wait;

    wait = 0;
    object = call->object;
    peer_field = lookupClassField(classJava_xeno_Unit,
                                  loadConstString(D_004DC1D0, -1), 0);
    peer = *(UnitNativePeer **)(object + peer_field->offset);
    algorithm_field = lookupClassField(classJava_xeno_Unit,
                                       loadConstString(D_004D16F8, -1), 0);

    if (*(u32 *)(object + algorithm_field->offset) & 1)
        source = &peer->rotation.x;
    else
        source = defaultOffset_0043C1D0;

    sequence = (UnitNativeSequence *)&unitSequence[peer->serial];
    sequence->rotateSequence = SEQ_rotateUnit;
    sequence->state |= 4;
    channel = &sequence->rotation;

    switch (mode) {
    case 0:
        channel->frames[1] = call->slot[0].integer;
        channel->flags &= ~2;
        channel->target[1] = (call->slot[1].floating + source[1]) / 180.0f * 3.1415927f;
        wait = call->slot[2].byte;
        break;
    case 1:
        channel->frames[1] = -1;
        channel->flags &= ~2;
        channel->target[1] = (call->slot[0].floating + source[1]) / 180.0f * 3.1415927f;
        channel->step[1] = call->slot[1].floating / 180.0f * 3.1415927f / 30.0f;
        wait = call->slot[2].byte;
        break;
    case 2:
        peer_field = lookupClassField(classJava_xeno_Chr,
                                      loadConstString(D_004DC1D0, -1), 0);
        chr_peer = *(void **)(call->slot[1].object + peer_field->offset);
        channel->frames[1] = call->slot[0].integer;
        channel->flags |= 2;
        channel->track.source = chr_peer;
        wait = call->slot[2].byte;
        break;
    case 3:
        peer_field = lookupClassField(classJava_xeno_Chr,
                                      loadConstString(D_004DC1D0, -1), 0);
        chr_peer = *(void **)(call->slot[0].object + peer_field->offset);
        channel->frames[1] = -1;
        channel->flags |= 2;
        channel->track.source = chr_peer;
        channel->step[1] = call->slot[1].floating / 180.0f * 3.1415927f / 30.0f;
        wait = call->slot[2].byte;
        break;
    }

    if (wait == 1) {
        peer->flags |= 2;
        if (thread->kind != UNIT_THREAD_KIND && (thread->flags & 0x40) == 0)
            return;
        if ((thread->flags & 0x40) != 0) {
            thread->wait_target = peer;
            thread->wait_kind = UNIT_WAIT_UNIT;
            thread->resume_frames = thread->frame_depth;
            thread->flags |= 4;
        }
        thread->resume_frames = thread->frame_depth;
        thread->flags |= 1;
    }
}

INCLUDE_ASM("asm/main/nonmatchings/unit", UNIT_rotZ);

void Java_xeno_Unit_getRotate__(JThread *thread, UnitObjectCall *arguments)
{
    JavaField *peer_field;
    JavaField *field;
    u8 *object;
    UnitVector3 *vector;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Unit,
                                  loadConstString(D_004DC1D0, -1), 0);
    vector = &(*(UnitNativePeer **)(object + peer_field->offset))->rotation;
    field = lookupClassField(classJava_xeno_Unit,
                             loadConstString(D_004DC1D8, -1), 0);
    *(float *)(object + field->offset) = vector->x / 3.1415927f * 180.0f;
    field = lookupClassField(classJava_xeno_Unit,
                             loadConstString(D_004DC1E0, -1), 0);
    *(float *)(object + field->offset) = vector->y / 3.1415927f * 180.0f;
    field = lookupClassField(classJava_xeno_Unit,
                             loadConstString(D_004DC1E8, -1), 0);
    *(float *)(object + field->offset) = vector->z / 3.1415927f * 180.0f;
}

void Java_xeno_Unit_getSignal__(JThread *thread, UnitObjectCall *arguments,
                                u32 *failure_result)
{
    JavaField *peer_field;
    u8 *object;
    UnitNativePeer *peer;

    object = arguments->object;
    if (JNI_isInstanceOf(object, classJava_xeno_Unit) == 0) {
        *failure_result = 0;
        return;
    }
    peer_field = lookupClassField(classJava_xeno_Unit,
                                  loadConstString(D_004DC1D0, -1), 0);
    peer = *(UnitNativePeer **)(object + peer_field->offset);
    *failure_result = peer->signal;
}

void Java_xeno_Unit_getTranslate__(JThread *thread, UnitObjectCall *arguments)
{
    JavaField *peer_field;
    JavaField *field;
    u8 *object;
    UnitVector3 *vector;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Unit,
                                  loadConstString(D_004DC1D0, -1), 0);
    vector = &(*(UnitNativePeer **)(object + peer_field->offset))->position;
    field = lookupClassField(classJava_xeno_Unit,
                             loadConstString(D_004DC1F0, -1), 0);
    *(float *)(object + field->offset) = vector->x;
    field = lookupClassField(classJava_xeno_Unit,
                             loadConstString(D_004DC1F8, -1), 0);
    *(float *)(object + field->offset) = vector->y;
    field = lookupClassField(classJava_xeno_Unit,
                             loadConstString(D_004DC200, -1), 0);
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

void Java_xeno_Unit_move__Lxeno_util_Spline_IZ(JThread *thread,
                                           UnitSplineCall *arguments,
                                           u32 *failure_result)
{
    JavaField *peer_field;
    UnitNativePeer *peer;
    UnitNativeSequence *sequence;
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
                                  loadConstString(D_004DC1D0, -1), 0);
    peer = *(UnitNativePeer **)(object + peer_field->offset);
    sequence = (UnitNativeSequence *)&unitSequence[peer->serial];
    track = &sequence->trans.track;
    sequence->state |= 0x1;
    lookupClassField(classJava_xeno_Unit, loadConstString(D_004D16F8, -1), 0);
    spline = arguments->spline;
    flags = arguments->flags.integer;
    wait = arguments->wait;
    sequence->transSequence = SEQ_moveUnitSPL;
    track->flags = flags;
    track->source = spline;
    track->last_frame = spline->frame_count;
    if (wait == 1) {
        peer->flags |= 2;
        if (thread->kind != UNIT_THREAD_KIND && (thread->flags & 0x40) == 0)
            return;
        if ((thread->flags & 0x40) != 0) {
            thread->wait_target = peer;
            thread->wait_kind = UNIT_WAIT_UNIT;
            thread->resume_frames = thread->frame_depth;
            thread->flags |= 4;
        }
        thread->resume_frames = thread->frame_depth;
        thread->flags |= 1;
    }
}

void Java_xeno_Unit_rotate__Lxeno_util_Spline_IZ(JThread *thread,
                                           UnitSplineCall *arguments,
                                           u32 *failure_result)
{
    JavaField *peer_field;
    UnitNativePeer *peer;
    UnitNativeSequence *sequence;
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
                                  loadConstString(D_004DC1D0, -1), 0);
    peer = *(UnitNativePeer **)(object + peer_field->offset);
    sequence = (UnitNativeSequence *)&unitSequence[peer->serial];
    track = &sequence->rotation.track;
    sequence->state |= 0xe;
    lookupClassField(classJava_xeno_Unit, loadConstString(D_004D16F8, -1), 0);
    spline = arguments->spline;
    flags = arguments->flags.integer;
    wait = arguments->wait;
    sequence->rotateSequence = SEQ_rotateUnitSPL;
    track->flags = flags;
    track->source = spline;
    track->last_frame = spline->frame_count;
    if (wait == 1) {
        peer->flags |= 2;
        if (thread->kind != UNIT_THREAD_KIND && (thread->flags & 0x40) == 0)
            return;
        if ((thread->flags & 0x40) != 0) {
            thread->wait_target = peer;
            thread->wait_kind = UNIT_WAIT_UNIT;
            thread->resume_frames = thread->frame_depth;
            thread->flags |= 4;
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

void UNIT_motion(int mode, JThread *thread, void *arguments,
                 u32 *failure_result)
{
    UnitMotionCall *call = arguments;
    JavaField *peer_field;
    UnitNativePeer *peer;
    UnitNativeSequence *sequence;
    UnitMotionChannel *motion;
    u8 *object;
    int wait;

    wait = 0;
    object = call->object;
    peer_field = lookupClassField(classJava_xeno_Unit,
                                  loadConstString(D_004DC1D0, -1), 0);
    peer = *(UnitNativePeer **)(object + peer_field->offset);
    sequence = (UnitNativeSequence *)&unitSequence[peer->serial];
    sequence->motionSequence = SEQ_motionUnit;
    sequence->state |= 0x10;
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
        if (thread->kind != UNIT_THREAD_KIND && (thread->flags & 0x40) == 0)
            return;
        if ((thread->flags & 0x40) != 0) {
            thread->wait_target = peer;
            thread->wait_kind = UNIT_WAIT_UNIT;
            thread->resume_frames = thread->frame_depth;
            thread->flags |= 4;
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

void Java_xeno_Unit_scale__Lxeno_util_Spline_IZ(JThread *thread,
                                           UnitSplineCall *arguments,
                                           u32 *failure_result)
{
    JavaField *peer_field;
    UnitNativePeer *peer;
    UnitNativeSequence *sequence;
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
                                  loadConstString(D_004DC1D0, -1), 0);
    peer = *(UnitNativePeer **)(object + peer_field->offset);
    sequence = (UnitNativeSequence *)&unitSequence[peer->serial];
    track = &sequence->scale.track;
    sequence->state |= 0xe0;
    lookupClassField(classJava_xeno_Unit, loadConstString(D_004D16F8, -1), 0);
    spline = arguments->spline;
    wait = arguments->wait;
    sequence->scaleSequence = SEQ_scaleUnitSPL;
    track->source = spline;
    track->last_frame = spline->frame_count;
    if (wait == 1) {
        peer->flags |= 2;
        if (thread->kind != UNIT_THREAD_KIND && (thread->flags & 0x40) == 0)
            return;
        if ((thread->flags & 0x40) != 0) {
            thread->wait_target = peer;
            thread->wait_kind = UNIT_WAIT_UNIT;
            thread->resume_frames = thread->frame_depth;
            thread->flags |= 4;
        }
        thread->resume_frames = thread->frame_depth;
        thread->flags |= 1;
    }
}

static void UNIT_sclX(int mode, JThread *thread, void *arguments,
                  u32 *failure_result)
{
    UnitScaleCall *call = arguments;
    JavaField *peer_field;
    JavaField *algorithm_field;
    UnitNativePeer *peer;
    float *scale_source;
    UnitNativeSequence *sequence;
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
                                  loadConstString(D_004DC1D0, -1), 0);
    peer = *(UnitNativePeer **)(object + peer_field->offset);
    algorithm_field = lookupClassField(classJava_xeno_Unit,
                                       loadConstString(D_004D16F8, -1), 0);

    scale_source = &peer->scale_x;
    if ((*(u32 *)(object + algorithm_field->offset) & 1) == 0)
        scale_source = defaultOffset_0043C1D0;

    sequence = (UnitNativeSequence *)&unitSequence[peer->serial];
    sequence->scaleSequence = SEQ_scaleUnit;
    sequence->state |= 0x20;
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
        scale->step[0] = call->second.floating / 30.0f;
        wait = call->wait;
        break;
    }

    if (wait == 1) {
        peer->flags |= 2;
        if (thread->kind != UNIT_THREAD_KIND && (thread->flags & 0x40) == 0)
            return;
        if ((thread->flags & 0x40) != 0) {
            thread->wait_target = peer;
            thread->wait_kind = UNIT_WAIT_UNIT;
            thread->resume_frames = thread->frame_depth;
            thread->flags |= 4;
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

static void UNIT_sclY(int mode, JThread *thread, void *arguments,
                  u32 *failure_result)
{
    UnitScaleCall *call = arguments;
    JavaField *peer_field;
    JavaField *algorithm_field;
    UnitNativePeer *peer;
    float *scale_source;
    UnitNativeSequence *sequence;
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
                                  loadConstString(D_004DC1D0, -1), 0);
    peer = *(UnitNativePeer **)(object + peer_field->offset);
    algorithm_field = lookupClassField(classJava_xeno_Unit,
                                       loadConstString(D_004D16F8, -1), 0);

    scale_source = &peer->scale_x;
    if ((*(u32 *)(object + algorithm_field->offset) & 1) == 0)
        scale_source = defaultOffset_0043C1D0;

    sequence = (UnitNativeSequence *)&unitSequence[peer->serial];
    sequence->scaleSequence = SEQ_scaleUnit;
    sequence->state |= 0x40;
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
        scale->step[1] = call->second.floating / 30.0f;
        wait = call->wait;
        break;
    }

    if (wait == 1) {
        peer->flags |= 2;
        if (thread->kind != UNIT_THREAD_KIND && (thread->flags & 0x40) == 0)
            return;
        if ((thread->flags & 0x40) != 0) {
            thread->wait_target = peer;
            thread->wait_kind = UNIT_WAIT_UNIT;
            thread->resume_frames = thread->frame_depth;
            thread->flags |= 4;
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

static void UNIT_sclZ(int mode, JThread *thread, void *arguments,
                  u32 *failure_result)
{
    UnitScaleCall *call = arguments;
    JavaField *peer_field;
    JavaField *algorithm_field;
    UnitNativePeer *peer;
    float *scale_source;
    UnitNativeSequence *sequence;
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
                                  loadConstString(D_004DC1D0, -1), 0);
    peer = *(UnitNativePeer **)(object + peer_field->offset);
    algorithm_field = lookupClassField(classJava_xeno_Unit,
                                       loadConstString(D_004D16F8, -1), 0);

    scale_source = &peer->scale_x;
    if ((*(u32 *)(object + algorithm_field->offset) & 1) == 0)
        scale_source = defaultOffset_0043C1D0;

    sequence = (UnitNativeSequence *)&unitSequence[peer->serial];
    sequence->scaleSequence = SEQ_scaleUnit;
    sequence->state |= 0x80;
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
        scale->step[2] = call->second.floating / 30.0f;
        wait = call->wait;
        break;
    }

    if (wait == 1) {
        peer->flags |= 2;
        if (thread->kind != UNIT_THREAD_KIND && (thread->flags & 0x40) == 0)
            return;
        if ((thread->flags & 0x40) != 0) {
            thread->wait_target = peer;
            thread->wait_kind = UNIT_WAIT_UNIT;
            thread->resume_frames = thread->frame_depth;
            thread->flags |= 4;
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
    UnitNativePeer *peer;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Unit,
                                  loadConstString(D_004DC1D0, -1), 0);
    peer = *(UnitNativePeer **)(object + peer_field->offset);
    if (arguments->flag != 0) {
        peer->flags |= 0x100;
    } else {
        peer->flags &= ~0x100;
    }
}

void Java_xeno_Unit_setRotate__(JThread *thread, UnitObjectCall *arguments)
{
    JavaField *peer_field;
    JavaField *field;
    u8 *object;
    UnitVector3 *vector;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Unit,
                                  loadConstString(D_004DC1D0, -1), 0);
    vector = &(*(UnitNativePeer **)(object + peer_field->offset))->rotation;
    field = lookupClassField(classJava_xeno_Unit,
                             loadConstString(D_004DC1D8, -1), 0);
    vector->x = *(float *)(object + field->offset) / 180.0f * 3.1415927f;
    field = lookupClassField(classJava_xeno_Unit,
                             loadConstString(D_004DC1E0, -1), 0);
    vector->y = *(float *)(object + field->offset) / 180.0f * 3.1415927f;
    field = lookupClassField(classJava_xeno_Unit,
                             loadConstString(D_004DC1E8, -1), 0);
    vector->z = *(float *)(object + field->offset) / 180.0f * 3.1415927f;
}

void Java_xeno_Unit_setTranslate__(JThread *thread, UnitObjectCall *arguments)
{
    JavaField *peer_field;
    JavaField *field;
    u8 *object;
    UnitVector3 *vector;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Unit,
                                  loadConstString(D_004DC1D0, -1), 0);
    vector = &(*(UnitNativePeer **)(object + peer_field->offset))->position;
    field = lookupClassField(classJava_xeno_Unit,
                             loadConstString(D_004DC1F0, -1), 0);
    vector->x = *(float *)(object + field->offset);
    field = lookupClassField(classJava_xeno_Unit,
                             loadConstString(D_004DC1F8, -1), 0);
    vector->y = *(float *)(object + field->offset);
    field = lookupClassField(classJava_xeno_Unit,
                             loadConstString(D_004DC200, -1), 0);
    vector->z = *(float *)(object + field->offset);
}

void Java_xeno_Unit_setVisible__IZ(JThread *thread, UnitVisibleCall *arguments)
{
    JavaField *peer_field;
    u8 *object;
    UnitNativePeer *peer;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Unit,
                                  loadConstString(D_004DC1D0, -1), 0);
    peer = *(UnitNativePeer **)(object + peer_field->offset);
    MDL_setVisible(peer->model, arguments->mode, arguments->visible);
}

void Java_xeno_Unit_setVisible__Z(JThread *thread, UnitBoolCall *arguments)
{
    JavaField *peer_field;
    u8 *object;
    UnitNativePeer *peer;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Unit,
                                  loadConstString(D_004DC1D0, -1), 0);
    peer = *(UnitNativePeer **)(object + peer_field->offset);
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
    UnitNativePeer *peer;

    object = arguments->object;
    if (JNI_isInstanceOf(object, classJava_xeno_Unit) == 0) {
        *failure_result = 0;
        return;
    }
    peer_field = lookupClassField(classJava_xeno_Unit,
                                  loadConstString(D_004DC1D0, -1), 0);
    peer = *(UnitNativePeer **)(object + peer_field->offset);
    peer->signal = arguments->value;
}

void Java_xeno_Unit_start__ILjava_lang_Object_(JThread *thread,
                                              UnitStartCall *arguments)
{
    JavaField *peer_field;
    u8 *object;
    int mode;
    UnitNativePeer *peer;
    UnitNativeSequence *sequence;
    UnitString *method_name;
    UnitStringValue *value;
    SceneClass *scene_class;
    SceneString *name;
    SceneMethod *method;
    JThread *unit_thread;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Unit,
                                  loadConstString(D_004DC1D0, -1), 0);
    mode = arguments->mode;
    peer = *(UnitNativePeer **)(object + peer_field->offset);
    sequence = (UnitNativeSequence *)&unitSequence[peer->serial];
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
        scene_class = ((SceneObjectHeader *)object)->class_ref->scene_class;
        name = loadConstString(value->data, value->length);
        method = findMethod(scene_class, name, TYPE_Void);
        sequence->method = method;
        unit_thread = (void *)JTHREAD_get(object);
        if (unit_thread != 0) {
            unit_thread->method = method;
            unit_thread->reset = JTHREAD_defaultUnit;
            JNI_initThread(unit_thread);
            unit_thread->flags |= 0x10;
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

void Java_xeno_Unit_stop__(JThread *thread, UnitObjectCall *arguments)
{
    JavaField *peer_field;
    JavaField *field;
    u8 *object;
    UnitNativePeer *peer;
    JThread *unit_thread;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Unit,
                                  loadConstString(D_004DC1D0, -1), 0);
    peer = *(UnitNativePeer **)(object + peer_field->offset);
    unit_thread = (void *)JTHREAD_get(object);
    if (unit_thread != 0) {
        unit_thread->flags &= ~0x10;
    }
    field = lookupClassField(classJava_xeno_Unit,
                             loadConstString(D_004DC1F0, -1), 0);
    *(float *)(object + field->offset) = peer->position.x;
    field = lookupClassField(classJava_xeno_Unit,
                             loadConstString(D_004DC1F8, -1), 0);
    *(float *)(object + field->offset) = peer->position.y;
    field = lookupClassField(classJava_xeno_Unit,
                             loadConstString(D_004DC200, -1), 0);
    *(float *)(object + field->offset) = peer->position.z;
    field = lookupClassField(classJava_xeno_Unit,
                             loadConstString(D_004DC1D8, -1), 0);
    *(float *)(object + field->offset) = peer->rotation.x;
    field = lookupClassField(classJava_xeno_Unit,
                             loadConstString(D_004DC1E0, -1), 0);
    *(float *)(object + field->offset) = peer->rotation.y;
    field = lookupClassField(classJava_xeno_Unit,
                             loadConstString(D_004DC1E8, -1), 0);
    *(float *)(object + field->offset) = peer->rotation.z;
}

void Java_xeno_Unit_validate__(void)
{
}

void Java_xeno_Unit_setParent__Ljava_lang_Object_I(
    JThread *thread, UnitParentCall *arguments)
{
    JavaField *peer_field;
    u8 *object;
    u8 *chr;
    int flags;
    UnitNativePeer *peer;
    void *parent_peer;

    object = arguments->object;
    chr = arguments->chr;
    flags = arguments->flags;
    peer_field = lookupClassField(classJava_xeno_Unit,
                                  loadConstString(D_004DC1D0, -1), 0);
    peer = *(UnitNativePeer **)(object + peer_field->offset);
    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(D_004DC1D0, -1), 0);
    parent_peer = *(void **)(chr + peer_field->offset);
    peer->parent_peer = parent_peer;
    if ((flags & 0x8000) != 0) {
        peer->parent_flags = ACT_jointGetAccessories(parent_peer,
                                                     flags & 0x7fff) | 0x8000;
    } else {
        peer->parent_flags = flags;
    }
    UNIT_setUpdate(object, peer);
}

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

void Java_xeno_Unit_setArgs__III(JThread *thread, UnitArgsWordCall *arguments)
{
    JavaField *peer_field;
    UnitNativePeer *peer;
    int value;
    u8 *object;
    int offset;
    int size;

    value = arguments->value;
    object = arguments->object;
    offset = arguments->offset;
    size = arguments->size;
    peer_field = lookupClassField(classJava_xeno_Unit,
                                  loadConstString(D_004DC1D0, -1), 0);
    peer = *(UnitNativePeer **)(object + peer_field->offset);
    if ((unsigned int)(size - 1) < 4U) {
        copyArgs(peer->args + offset, (u8 *)&value, size);
    }
}

void Java_xeno_Unit_getArgs__II(JThread *thread, UnitArgsGetCall *arguments,
                                u32 *failure_result)
{
    JavaField *peer_field;
    u8 *object;
    UnitNativePeer *peer;
    int size;
    int offset;
    unsigned int check;
    int result;

    size = arguments->size;
    object = arguments->object;
    offset = arguments->offset;
    peer_field = lookupClassField(classJava_xeno_Unit,
                                  loadConstString(D_004DC1D0, -1), 0);
    peer = *(UnitNativePeer **)(object + peer_field->offset);
    check = (unsigned int)(size - 1);
    if (check < 4U) {
        copyArgs((u8 *)&result, peer->args + offset, size);
        *failure_result = (u32)result;
    }
}

void Java_xeno_Unit_setArgs__ILjava_lang_Object_I(JThread *thread,
                                                  UnitArgsSetCall *arguments)
{
    JavaField *peer_field;
    u8 *object;
    UnitNativePeer *peer;
    UnitArgsSource *source;
    int size;

    object = arguments->object;
    source = arguments->source;
    size = arguments->size;
    peer_field = lookupClassField(classJava_xeno_Unit,
                                  loadConstString(D_004DC1D0, -1), 0);
    peer = *(UnitNativePeer **)(object + peer_field->offset);
    if (size > 0) {
        copyArgs(peer->args, source->data, size);
    }
}

INCLUDE_ASM("asm/main/nonmatchings/unit", Java_xeno_Unit_setArgs__IIIII);

void Java_xeno_Unit_getScale__(JThread *thread, UnitObjectCall *arguments,
                               UnitResultValue *result)
{
    static UnitScaleVector scale;
    JavaField *peer_field;
    u8 *object;
    UnitNativePeer *peer;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Unit,
                                  loadConstString(D_004DC1D0, -1), 0);
    peer = *(UnitNativePeer **)(object + peer_field->offset);
    scale.class_ref = classJava_xeno_util_Vector4f->instance_class_ref;
    scale.x = peer->scale_x;
    scale.y = peer->scale_y;
    scale.z = peer->scale_z;
    scale.w = peer->scale_w;
    result->object = &scale;
}

void Java_xeno_Unit_setScale__FFF(JThread *thread, UnitVector3Call *arguments)
{
    JavaField *peer_field;
    u8 *object;
    UnitNativePeer *peer;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Unit,
                                  loadConstString(D_004DC1D0, -1), 0);
    peer = *(UnitNativePeer **)(object + peer_field->offset);
    peer->scale_x = arguments->x;
    peer->scale_y = arguments->y;
    peer->scale_z = arguments->z;
}

void Java_xeno_Unit_getSerial__(JThread *thread, UnitObjectCall *arguments,
                                u32 *failure_result)
{
    JavaField *peer_field;
    u8 *object;
    UnitNativePeer *peer;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Unit,
                                  loadConstString(D_004DC1D0, -1), 0);
    peer = *(UnitNativePeer **)(object + peer_field->offset);
    *failure_result = peer->serial;
}

void Java_xeno_Unit_mtnSetMask__I(JThread *thread, UnitIntCall *arguments)
{
    JavaField *peer_field;
    u8 *object;
    UnitNativePeer *peer;
    int mask;

    mask = arguments->value;
    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Unit,
                                  loadConstString(D_004DC1D0, -1), 0);
    peer = *(UnitNativePeer **)(object + peer_field->offset);
    peer->motion_mask = mask;
}

void Java_xeno_Unit_mtnGetRoot__ILxeno_util_Vector4f_(
    JThread *thread, UnitMotionRootCall *arguments)
{
    JavaField *peer_field;
    u8 *object;
    UnitNativePeer *peer;
    UnitPivotVector *vector;
    int type;

    object = arguments->object;
    type = arguments->type;
    vector = arguments->vector;
    peer_field = lookupClassField(classJava_xeno_Unit,
                                  loadConstString(D_004DC1D0, -1), 0);
    peer = *(UnitNativePeer **)(object + peer_field->offset);
    if (vector != 0) {
        switch (type) {
        case 0:
            vector->x = peer->motion_root[0].x;
            vector->y = peer->motion_root[0].y;
            vector->z = peer->motion_root[0].z;
            break;
        case 1:
            vector->x = peer->motion_root[1].x / 3.1415927f * 180.0f;
            vector->y = peer->motion_root[1].y / 3.1415927f * 180.0f;
            vector->z = peer->motion_root[1].z / 3.1415927f * 180.0f;
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
    UnitNativePeer *peer;
    UnitNativeSequence *entry;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Unit,
                                  loadConstString(D_004DC1D0, -1), 0);
    peer = *(UnitNativePeer **)(object + peer_field->offset);
    entry = (UnitNativeSequence *)&unitSequence[peer->serial];
    *failure_result = entry->state;
}

void Java_xeno_Unit_getPivot__Lxeno_util_Vector4f_(JThread *thread,
                                                   UnitPivotOutCall *arguments)
{
    JavaField *peer_field;
    u8 *object;
    UnitNativePeer *peer;
    UnitNativeSequence *entry;
    UnitPivotVector *vector;

    vector = arguments->vector;
    object = arguments->object;
    if (vector != 0) {
        peer_field = lookupClassField(classJava_xeno_Unit,
                                      loadConstString(D_004DC1D0, -1), 0);
        peer = *(UnitNativePeer **)(object + peer_field->offset);
        entry = (UnitNativeSequence *)&unitSequence[peer->serial];
        vector->x = entry->pivot_x;
        vector->y = entry->pivot_y;
        vector->z = entry->pivot_z;
    }
}

void Java_xeno_Unit_setPivot__FFF(JThread *thread, UnitVector3Call *arguments)
{
    JavaField *peer_field;
    u8 *object;
    UnitNativePeer *peer;
    UnitNativeSequence *entry;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Unit,
                                  loadConstString(D_004DC1D0, -1), 0);
    peer = *(UnitNativePeer **)(object + peer_field->offset);
    entry = (UnitNativeSequence *)&unitSequence[peer->serial];
    entry->pivot_x = arguments->x;
    entry->pivot_y = arguments->y;
    entry->pivot_z = arguments->z;
}

void Java_xeno_Unit_getAxis__Lxeno_util_Vector4f_(JThread *thread,
                                                  UnitAxisOutCall *arguments)
{
    JavaField *peer_field;
    u8 *object;
    UnitNativePeer *peer;
    UnitNativeSequence *entry;
    UnitAxisVector *vector;

    vector = arguments->vector;
    object = arguments->object;
    if (vector != 0) {
        peer_field = lookupClassField(classJava_xeno_Unit,
                                      loadConstString(D_004DC1D0, -1), 0);
        peer = *(UnitNativePeer **)(object + peer_field->offset);
        entry = (UnitNativeSequence *)&unitSequence[peer->serial];
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
    UnitNativePeer *peer;
    UnitNativeSequence *entry;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Unit,
                                  loadConstString(D_004DC1D0, -1), 0);
    peer = *(UnitNativePeer **)(object + peer_field->offset);
    entry = (UnitNativeSequence *)&unitSequence[peer->serial];
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

void Java_xeno_Unit_initElevatorFunc__(JThread *thread,
                                       UnitObjectCall *arguments)
{
    JavaField *peer_field;
    u8 *object;
    JavaField *py_field;
    UnitNativePeer *peer;
    int py_offset;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Unit,
                                  loadConstString(D_004DC1D0, -1), 0);
    peer = *(UnitNativePeer **)(object + peer_field->offset);
    py_field = lookupClassField(classJava_xeno_Unit,
                                loadConstString(D_004DC1F8, -1), 0);
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
    UnitNativePeer *peer;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Unit,
                                  loadConstString(D_004DC1D0, -1), 0);
    peer = *(UnitNativePeer **)(object + peer_field->offset);
    if (arguments->value != 0) {
        peer->flags |= 0x20;
    } else {
        peer->flags &= ~0x20;
    }
}

void Java_xeno_Unit_renderCommand__I(JThread *thread, UnitIntCall *arguments)
{
    JavaField *peer_field;
    u8 *object;
    UnitNativePeer *peer;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Unit,
                                  loadConstString(D_004DC1D0, -1), 0);
    peer = *(UnitNativePeer **)(object + peer_field->offset);
    peer->render_command = arguments->value;
}

void Java_xeno_Unit_setFilter__I(JThread *thread, UnitIntCall *arguments,
                                 u32 *failure_result)
{
    JavaField *peer_field;
    u8 *object;
    UnitNativePeer *peer;
    int value;
    int offset;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Unit,
                                  loadConstString(D_004DC1D0, -1), 0);
    offset = peer_field->offset;
    value = arguments->value;
    peer = *(UnitNativePeer **)(object + offset);
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

void Java_xeno_Unit_setShadow__II(JThread *thread, UnitShadowCall *arguments)
{
    JavaField *peer_field;
    UnitNativePeer *peer;

    peer_field = lookupClassField(classJava_xeno_Unit,
                                  loadConstString(D_004DC1D0, -1), 0);
    peer = *(UnitNativePeer **)(arguments->object + peer_field->offset);
    peer->shadow_x = arguments->x;
    peer->shadow_y = arguments->y;
}

void Java_xeno_Unit_shadow_clip_scale__F(JThread *thread,
                                         UnitFloatCall *arguments)
{
    JavaField *peer_field;
    UnitNativePeer *peer;

    peer_field = lookupClassField(classJava_xeno_Unit,
                                  loadConstString(D_004DC1D0, -1), 0);
    peer = *(UnitNativePeer **)(arguments->object + peer_field->offset);
    peer->shadow_clip_scale = arguments->value;
}

void Java_xeno_Unit_shadow_map_id__I(JThread *thread, UnitIntCall *arguments)
{
    JavaField *peer_field;
    UnitNativePeer *peer;

    peer_field = lookupClassField(classJava_xeno_Unit,
                                  loadConstString(D_004DC1D0, -1), 0);
    peer = *(UnitNativePeer **)(arguments->object + peer_field->offset);
    if (peer->shadow_map_count < 8) {
        peer->shadow_map_ids[peer->shadow_map_count++] = arguments->value;
    }
}

void Java_xeno_Unit_shadow_map_reset__(JThread *thread,
                                       UnitObjectCall *arguments)
{
    JavaField *peer_field;
    UnitNativePeer *peer;

    peer_field = lookupClassField(classJava_xeno_Unit,
                                  loadConstString(D_004DC1D0, -1), 0);
    peer = *(UnitNativePeer **)(arguments->object + peer_field->offset);
    peer->shadow_map_count = 0;
}

void Java_xeno_Unit_setSortOffset__F(JThread *thread, UnitFloatCall *arguments)
{
    JavaField *peer_field;
    UnitNativePeer *peer;

    peer_field = lookupClassField(classJava_xeno_Unit,
                                  loadConstString(D_004DC1D0, -1), 0);
    peer = *(UnitNativePeer **)(arguments->object + peer_field->offset);
    peer->sort_offset = (int) arguments->value;
}

void Java_xeno_Unit_setClip__I(JThread *thread, UnitIntCall *arguments)
{
    JavaField *peer_field;
    u8 *object;
    UnitNativePeer *peer;
    int value;

    peer_field = lookupClassField(classJava_xeno_Unit,
                                  loadConstString(D_004DC1D0, -1), 0);
    value = arguments->value;
    object = arguments->object;
    peer = *(UnitNativePeer **)(object + peer_field->offset);
    if (value != 0) {
        peer->flags |= 0x40;
    } else {
        peer->flags &= ~0x40;
    }
}

void Java_xeno_Unit_setMonitorPrio__I(JThread *thread, UnitIntCall *arguments)
{
    JavaField *peer_field;
    u8 *object;
    UnitNativePeer *peer;
    int value;
    int offset;

    peer_field = lookupClassField(classJava_xeno_Unit,
                                  loadConstString(D_004DC1D0, -1), 0);
    value = arguments->value;
    offset = peer_field->offset;
    object = arguments->object;
    peer = *(UnitNativePeer **)(object + offset);
    if (value != 0) {
        peer->monitor_priority = 1;
        return;
    }
    peer->monitor_priority = 0;
}

/* Preserve the frozen compiled form's declarations before consumers and
 * original eight-byte definitions after the function bodies. */
const char D_004DC1D0[8] = "peer";
const char D_004DC1F8[8] = "py";
