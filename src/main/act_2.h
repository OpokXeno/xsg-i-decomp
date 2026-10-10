/*
 * TU-local declarations of main/tu255 (src/main/act_2.c).
 */

#ifndef SRC_MAIN_ACT_2_H
#define SRC_MAIN_ACT_2_H

#include "shared.h"

/*
 * The engine's actor record. `actor` is the 64-entry array at main
 * 0x0043c1e0 with a 0xa70 stride (readelf: 0043c1e0 OBJECT GLOBAL actor
 * size 0x29c00 = 64 * 0xa70). src/main/near_dir.h, src/main/chr.h and
 * src/main/set_motion.h each carry the same recovered +0x00..+0x70 head,
 * with the field-by-field evidence for it; it is repeated verbatim here.
 * The selected ACT_* bodies add fields at their original displacements below.
 */

/*
 * +0x8dc animPackTables: eight per-category animation pack-table pointers,
 * right after move. ACT_animGetData selects one with bits 8-10 of its packed
 * id (main:0x003081c8, andi a1,a1,0x7ff; srl v0,a1,0x8; sll v0,v0,0x2; lw
 * a0,0x8dc(v0)) and tail calls PACK_getEntry(table, id & 0xff) on it, so
 * each slot is a pointer.
 */

/*
 * +0x6f0 animSlot: ACT_animGetCurrent takes its address (main:0x003081fc,
 * addiu a0,v1,0x6F0) and hands it to ANM_getEntry as the entry's owning
 * slot, so it is an embedded record, not a pointer. Only its +0x14 halfword
 * is read (main:0x00308200, lhu v0,20(a0)): bits 8-10 of that packed value
 * select an animPackTables slot the same way ACT_animGetData's dataId does
 * (srl v0,v0,0x6; andi v0,v0,0x1c is (currentDataId>>8&7)<<2 folded into one
 * shift), so it caches a dataId like the one ACT_animGetData is called with.
 * ACT_setMotion also writes the following halfword at +0x16 as the next data
 * id (main:0x00307ee0..0x00307ee4).
 */
/* ACT_setMotion (main:0x00307e80..0x00307f50) also uses the flags, playback
 * bounds, speed, and frame in the prefix. The queued-animation flags halfword
 * at slot+0x22 is included from the original store's slot-relative address;
 * the ten bytes before it remain unmodeled.
 */
typedef struct ActorAnimSlot {
    unsigned int flags;
    float frame;
    float speed;
    float reverseLimit;
    float forwardLimit;
    unsigned short currentDataId;
    unsigned short nextDataId;
    unsigned char unmodeled_18[0x22 - 0x18];
    unsigned short animQueuedFlags;
} ActorAnimSlot;

/* The adjacent actor animation pointers are addressed from the embedded slot
 * base by ACT_setMotion (main:0x00307e80..0x00307f50): animData at +0x2c,
 * animUserData at +0x30, and animPack at +0x34. The eight bytes before them
 * remain unmodeled.
 */
typedef struct ActorAnimationState {
    ActorAnimSlot slot;
    unsigned char unmodeled_24[0x2c - 0x24];
    void *animData;
    void *animUserData;
    void *animPack;
} ActorAnimationState;

typedef struct ActorAccessoryJointTable {
    unsigned char unmodeled_00[0x20];
    int jointId[0x29];
} ActorAccessoryJointTable;

typedef struct ActorDrawEnv {
    float fogDistance[4];
    float fogColor[4];
    float lightPosition[3][4];
    float lightColor[3][4];
    unsigned int flags;
    unsigned char unmodeled_84[0xa0 - 0x84];
    float transparency;
    unsigned char unmodeled_a4[0xb8 - 0xa4];
    unsigned long long alpha;
} ActorDrawEnv;

typedef struct ActorAttachment {
    short type;
    short joint;
    void **matrix;
} ActorAttachment;

/*
 * ACT_setParent reads matrixResource at actor+0x894 through this 0x58-byte
 * view. The adjacent actor+0x898 pointer is separately evidenced as the
 * joint-matrix base; neither field extends the model view.
 */
typedef struct ActorModel {
    unsigned char unmodeled_00[0x54];
    void *matrixResource;
} ActorModel;

typedef struct ActorModelState {
    ActorModel model;
    Matrix4 *jointMatrices;
} ActorModelState;

typedef struct ActorMoveHeader {
    unsigned char unmodeled_00[8];
    unsigned short baseJointCount;
    unsigned short extraJointCount;
} ActorMoveHeader;

typedef struct Actor {
    u32 flags;
    void (*update)(struct Actor *actor);
    void (*draw)(struct Actor *actor);
    u32 quadword_alignment_gap;
    Vector4 position;
    Vector4 previous_position;
    Vector4 velocity;
    Vector4 acceleration;
    Vector4 rotation;
    Vector4 scale;
    unsigned char unmodeled_70[0x86 - 0x70];
    short resourceKind;
    unsigned char unmodeled_88[0x698 - 0x88];
    int attachmentCount;
    int attachmentIndex;
    ActorAttachment attachments[8];
    signed char attachmentUsed[8];
    unsigned char unmodeled_6e8[0x6f0 - 0x6e8];
    ActorAnimationState animation;
    unsigned char unmodeled_728[0x790 - 0x728];
    unsigned char jointProducer[0x7f8 - 0x790];
    ActorAccessoryJointTable *accessoryJoints;
    int moveElementId;
    unsigned char unmodeled_800[0x824 - 0x800];
    void *jointMatrices;
    void *previousJointMatrices;
    void *staticValueRecords;
    void *valueRecords;
    unsigned char unmodeled_834[0x840 - 0x834];
    ActorModelState model[1]; /* +0x840: model and its joint matrices */
    unsigned char unmodeled_89c[0x8d0 - 0x89c];
    void *modelResource;
    void *matrixResource;
    void *move;
    void *animPackTables[8];
    struct Actor *parent;
    int childCount;
    struct Actor *children[7];
    ActorDrawEnv drawEnv;
    unsigned char unmodeled_9e0[0xa70 - 0x9e0];
} Actor;

/* ACT_setVisible's third argument is forwarded to both MDL tail calls. */
extern void ACT_setVisible(Actor *actor, int part, int visible);
extern void ACT_modelDrawSub(Actor *actor);
extern void ACT_updateMotionSub(Actor *actor, int pause);

int ACT_jointGetMoveElementID(Actor *actor);

void ACT_resetArms(Actor *actor, Actor *other, int acc_id);

Actor *ACT_setFace(Actor *actor, Actor *parent, int faceId);

void *ACT_animGetUserData(Actor *actor);

int ACT_animCheckData(Actor *actor);

void ACT_initMTNResource(void);

void ACT_resourceInit(void);

void ACT_animGetCurrent(Actor *actor);

#endif /* SRC_MAIN_ACT_2_H */
