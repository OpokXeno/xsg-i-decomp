#include "common.h"

#include "act_2.h"

extern Actor actor[64];

/*
 * JNT_getMoveElement (main/tu261, src/main/jnt.c) is not yet recovered as C.
 * It dereferences its argument (main:0x00313b50, addiu v1,a0,16; lhu
 * a3,8(a0)), so the parameter is a pointer.
 */

extern int JNT_getMoveElement(void *move);

/*
 * FCV2_checkData is not yet recovered as C. It dereferences its argument to
 * compare a signature word (main:0x0030d8c8..0x0030d8d8), so the parameter is
 * a pointer.
 */

extern int FCV2_checkData(void *data);

/* One of the eight Java camera records embedded at `tcamera + n * 0x12c0`.
 * TCAMERA_update establishes the four 0x4a0 channel strides and TCAMERA_init
 * establishes the 0x12c0 record stride. The channel union names the fields
 * proved by the channel callers; the rest stays explicitly unmodeled storage
 * until another access gives it meaning. */

typedef struct CameraChannelSpline {
    unsigned char unmodeled_00[4];
    unsigned short weight_mode;
    unsigned short first_key;
    unsigned short last_key;
    unsigned short sample_count;
    unsigned int component_count;
    const float *samples;
} CameraChannelSpline;

typedef union CameraChannelData {
    CameraChannelSpline spline;
    struct {
        Vector4 offset;
        int mode;
        float **constraint;
    } constant;
    struct {
        unsigned char unmodeled_00[0x18];
        unsigned char unmodeled_18[0x4a0 - 0x18];
    } unmodeled;
} CameraChannelData;

typedef struct CameraRecord {
    unsigned int class_header;       /* +0x00, passed to Java as the object */
    int camera_id;                   /* +0x04, selects the studio camera */
    unsigned int unmodeled_08;
    int mode[4];                     /* +0x0c, TCAMERA_update dispatch */
    int frame[4];                    /* +0x1c, per-channel frame counters */
    float debug_cursor;              /* +0x2c, edited by the camera cursor */
    CameraChannelData translate;     /* +0x30 */
    CameraChannelData view;          /* +0x4d0 */
    CameraChannelData roll;          /* +0x970 */
    CameraChannelData fov;           /* +0xe10 */
    SceneObject peer;                /* +0x12b0, seeded by Camera_start */
    float initial_fov;               /* +0x12b4, seeded by Camera_create */
    unsigned char unmodeled_12b8[8]; /* +0x12b8 to the established 0x12c0 stride */
} CameraRecord;

CameraRecord tcamera[8] = {0};

extern void ACT_resetParent(Actor *actor, Actor *other);

extern void ACT_setHumanHand(Actor *actor, int hand);

/*
 * acc_id 0x108 is the only accessory kind reset here: ACT_resetParent(actor,
 * other) drops the parent link the accessory got from ACT_setArms, then
 * ACT_setHumanHand(other, 0) puts the character's own hand back to its
 * default (main:0x00306ae8..0x00306b1c).
 */

extern Actor *ACT_setParent(Actor *actor, int type, Actor *parent, int joint, int id);

extern void ACT_setVisible(Actor *actor, int part, int visible);

/*
 * Attaches `actor` to `parent` at joint 0x30 (type 2, id `faceId`); on
 * success (main:0x00306d40..0x00306d64) it hides the base model's own FACE
 * part with ACT_setVisible(parent, 0x46414345, 0) - 0x46414345 is "FACE"
 * read from most- to least-significant byte, matching lui 0x4641/ori 0x4345
 * at 0x00306d44/0x00306d5c.
 */

static void ACT_updateMotionCore(Actor *actor, int pause);

/* main:0x00307c38 is `jr ra` / nop: no argument is read and no state changes. */

/* main:0x00307c40 is `jr ra` / nop: no argument is read and no state changes. */

extern void *PACK_getEntry(void *table, int id);

extern void ANM_getEntry(ActorAnimSlot *slot, void *entry);

extern void MDL_setGroupVisible(void *model, int part, int visible);

extern void MDL_setVisible(void *model, int part, int visible);

static unsigned int tblHand[9] = {
    0x54453030, 0x54453031, 0x54453032, 0x54453033,
    0x54453034, 0x54453035, 0x54453036, 0x54453037, 0
};

struct JntProducer;

extern void JNT_initProducer(struct JntProducer *producer);

extern void JNT_addConsumer(struct JntProducer *producer, int order,
                            void (*filter)(void *work, void *parameter, int flags), void *parameter);

extern void JC_setParent(void *work, void *parameter, int flags);

extern void JC_copyChain(void *work, void *parameter, int flags);

struct JntMoveResource;


extern void *JNT_getAccessories(struct JntMoveResource *move);

extern void JC_face(void *work, void *parameter, int flags);

extern Actor *ACT_setParent2(Actor *actor, int type, Actor *parent, int joint, int flags,
                            void (*filter)(void *work, void *parameter, int flags));


extern void nmlModelSetZwrite(int enabled);

extern void nmlModelSetStencil(int enabled);

extern void nmlModelSetToumei(int enabled);

extern void nmlModelSetAlpha(unsigned long long alpha);

extern void nmlModelSetTransparency(float transparency);

extern void nmlModelSetFogCol(float *color);

extern void nmlModelSetFogDist(float nearDistance, float farDistance, float nearIntensity, float farIntensity);

extern void nmlModelSetPointLight(int index, float *position, float *color);

extern int MDL_create(void *model, void *resource);

extern void *JNT_getRootElement(void *joint);

/* main:0x00307c38 is `jr ra` / nop: no argument is read and no state changes. */

/* main:0x00307c40 is `jr ra` / nop: no argument is read and no state changes. */

extern void ACT_resetMatrix(Actor *actor);

extern void JNT_setFlags(int flags);

extern void JNT_setModel(void *model, void *move);

extern void JNT_setMatrix(void *matrices);

extern void JNT_setModelMatrix(void);

extern void *ANM_reset(ActorAnimSlot *slot, void *pack);

extern unsigned int GameLoopState[];

extern int RES_loadFile(int command, int callback, int resource_id, int flags);

extern int MDL_setNameVisible(void *model, const void *pattern, int visible);

enum { HAND_NAME, HAND_SUFFIX, HAND_NAME_MASK, HAND_SUFFIX_MASK };

INCLUDE_ASM("asm/main/nonmatchings/act_2", ACT_resetMatrix);

void ACT_resetParent(Actor *actor, Actor *parent)
{
    int childIndex;
    int followingIndex;
    void *producer = actor->jointProducer;

    JNT_initProducer(producer);
    for (childIndex = 0; childIndex < parent->childCount; childIndex++) {
        if (parent->children[childIndex] == actor) {
            if (actor->attachmentIndex >= 0) {
                parent->attachmentUsed[actor->attachmentIndex] = 0;
                parent->attachmentCount--;
            }
            actor->attachmentIndex = -1;
            for (followingIndex = childIndex; followingIndex < parent->childCount - 1; followingIndex++) {
                parent->children[followingIndex] = parent->children[followingIndex + 1];
            }
            parent->childCount--;
            break;
        }
    }
    actor->parent = 0;
}

Actor *ACT_setParent(Actor *actor, int type, Actor *parent, int joint, int flags)
{
    int slot;
    int slotIndex;
    void *producer;
    ActorAttachment *attachment;

    actor->parent = parent;
    parent->children[parent->childCount] = actor;
    parent->childCount++;
    if ((flags & 1) != 0) {
        actor->matrixResource = parent->matrixResource;
        if ((actor->flags & 0x4000) != 0) {
            actor->model[0].model.matrixResource = parent->matrixResource;
        }
    }
    if (parent->attachmentCount < 8) {
        slot = -1;
        for (slotIndex = 0; slotIndex < 8; slotIndex++) {
            if (parent->attachmentUsed[slotIndex] == 0) {
                slot = slotIndex;
                break;
            }
        }
        if (slot >= 0) {
            parent->attachmentUsed[slot] = 1;
            actor->attachmentIndex = slot;
            attachment = &parent->attachments[slot];
            producer = actor->jointProducer;
            attachment->type = type;
            attachment->joint = joint;
            parent->attachmentCount++;
            attachment->matrix = &parent->jointMatrices;
            JNT_initProducer(producer);
            if ((flags & 2) != 0) {
                JNT_addConsumer(producer, 0, JC_copyChain, attachment);
            } else {
                JNT_addConsumer(producer, 0, JC_setParent, attachment);
            }
        }
    }
    return actor;
}

INCLUDE_ASM("asm/main/nonmatchings/act_2", ACT_setParent2);

int ACT_jointGetMoveElementID(Actor *actor)
{
    if (actor->moveElementId < 0) {
        if (actor->move != 0) {
            actor->moveElementId = JNT_getMoveElement(actor->move);
        } else {
            actor->moveElementId = 0;
        }
    }
    return actor->moveElementId;
}

int ACT_jointGetAccessories(Actor *actor, unsigned int accessoryId)
{
    ActorAccessoryJointTable *accessories;
    ActorAccessoryJointTable *emptyTable;
    int result = -1;

    accessories = actor->accessoryJoints;
    if (accessories == 0) {
        actor->accessoryJoints = JNT_getAccessories(actor->move);
        accessories = actor->accessoryJoints;
    }
    emptyTable = 0;
    if (accessories != emptyTable && accessoryId < 0x29) {
        result = accessories->jointId[accessoryId];
    }
    return result;
}

void ACT_resetArms(Actor *actor, Actor *other, int acc_id)
{
    if (acc_id == 0x108) {
        ACT_resetParent(actor, other);
        ACT_setHumanHand(other, 0);
    }
}

Actor *ACT_setArms(Actor *actor, Actor *parent, unsigned int accessoryId, int flags)
{
    int joint;

    if ((accessoryId & 0xff00) == 0) {
        joint = ACT_jointGetAccessories(parent, accessoryId);
        if (joint > 0) {
            actor = ACT_setParent(actor, 0, parent, joint, flags);
        } else {
            actor = 0;
        }
    } else if (accessoryId == 0x108) {
        actor = ACT_setParent(actor, 2, parent, 0x42, flags | 2);
        ACT_setHumanHand(parent, 0x8007);
    }
    return actor;
}

Actor *ACT_setRelation(Actor *actor, Actor *parent, int relation, int flags)
{
    int joint;
    int type;
    void (*filter)(void *work, void *parameter, int flags);

    if ((relation & 0x8000) != 0) {
        switch (relation & 0x7f00) {
        case 0:
            joint = ACT_jointGetAccessories(parent, relation & 0x7fff);
            type = 0;
            filter = JC_setParent;
            break;
        case 0x100:
            filter = JC_copyChain;
            joint = 0x42;
            type = 2;
            ACT_setHumanHand(parent, 0x8007);
            break;
        case 0x200:
            filter = JC_face;
            joint = 0x30;
            type = 2;
            ACT_setVisible(parent, 0x46414345, 0);
            break;
        default:
            joint = 0;
            type = 0;
            filter = JC_setParent;
            break;
        }
    } else {
        joint = relation & 0x7fff;
        type = 0;
        filter = JC_setParent;
    }
    return ACT_setParent2(actor, type, parent, joint, flags, filter);
}

Actor *ACT_setFace(Actor *actor, Actor *parent, int faceId)
{
    Actor *result;

    result = ACT_setParent(actor, 2, parent, 0x30, faceId);
    if (result != 0) {
        ACT_setVisible(parent, 0x46414345, 0);
    }
    return result;
}

void *ACT_animGetUserData(Actor *actor)
{
    return actor->animation.animUserData;
}

int ACT_animCheckData(Actor *actor)
{
    return FCV2_checkData(actor->animation.animData);
}

static void ACT_updateMotionCore(Actor *actor, int pause)
{
    int childIndex;
    Actor **children;

    if (actor->parent != 0) {
        return;
    }
    ACT_updateMotionSub(actor, pause);
    if (actor->childCount > 0) {
        if ((actor->flags & 0x1000) == 0) {
            children = actor->children;
            for (childIndex = 0; childIndex < actor->childCount; childIndex++) {
                ACT_updateMotionSub(children[childIndex], pause);
            }
        } else {
            children = actor->children;
            for (childIndex = 0; childIndex < actor->childCount; childIndex++) {
                children[childIndex]->flags |= 0x1000;
            }
        }
    }
}

void ACT_updateMotion(Actor *actor)
{
    ACT_updateMotionCore(actor, 0);
}

void ACT_updateMotionPause(Actor *actor)
{
    ACT_updateMotionCore(actor, 1);
}

INCLUDE_ASM("asm/main/nonmatchings/act_2", ACT_updateMotionSub);

void ACT_setDrawEnv(Actor *actor)
{
    ActorDrawEnv *environment = &actor->drawEnv;
    unsigned int flags = environment->flags;
    int lightIndex;

    nmlModelSetZwrite(flags & 2);
    nmlModelSetStencil(flags & 4);
    if ((flags & 1) != 0) {
        nmlModelSetToumei(flags & 1);
        nmlModelSetAlpha(environment->alpha);
        nmlModelSetTransparency(environment->transparency);
    }
    if ((flags & 8) != 0) {
        nmlModelSetFogCol(environment->fogColor);
        nmlModelSetFogDist(environment->fogDistance[0], environment->fogDistance[1],
                           environment->fogDistance[2], environment->fogDistance[3]);
    }
    if ((flags & 0x20) != 0) {
        for (lightIndex = 0; lightIndex < 3; lightIndex++) {
            nmlModelSetPointLight(lightIndex, environment->lightPosition[lightIndex],
                                 environment->lightColor[lightIndex]);
        }
    }
}

void ACT_modelDraw(Actor *actor)
{
    Actor **children;
    int childIndex;

    if (actor->parent == 0) {
        ACT_modelDrawSub(actor);
        if ((actor->flags & 0x1008) == 0) {
            if (actor->childCount > 0) {
                children = actor->children;
                for (childIndex = 0; childIndex < actor->childCount; childIndex++) {
                    ACT_modelDrawSub(children[childIndex]);
                }
            }
        }
    }
}

INCLUDE_ASM("asm/main/nonmatchings/act_2", ACT_modelDrawSub);

void ACT_setModelWrapper(Actor *actor)
{
    ActorModelState *modelState = actor->model;
    void *resource = actor->modelResource;
    ActorMoveHeader *move = actor->move;
    void *matrixResource = actor->matrixResource;
    Matrix4 *rootMatrices;

    if (resource != 0) {
        if (move == 0) {
            MDL_create(&modelState->model, resource);
            return;
        }
        MDL_create(&modelState->model, resource);
        rootMatrices = JNT_getRootElement(move);
        rootMatrices = &rootMatrices[move->baseJointCount + move->extraJointCount];
        modelState->model.matrixResource = matrixResource;
        actor->flags |= 0x4000;
        modelState->jointMatrices = rootMatrices;
    }
}

INCLUDE_ASM("asm/main/nonmatchings/act_2", ACT_loadResource);

void ACT_initMTNResource(void)
{
}

void ACT_resourceInit(void)
{
}

INCLUDE_ASM("asm/main/nonmatchings/act_2", ACT_initMotion);

void ACT_setMotion(Actor *actor, int motionId)
{
    ActorAnimationState *animation = &actor->animation;
    ActorAnimSlot *slot = &animation->slot;

    if (motionId == 0x80000000U) {
        void *move;
        void *model;

        ACT_resetMatrix(actor);
        animation->animPack = 0;
        animation->animData = 0;
        animation->animUserData = 0;
        animation->slot.flags &= 0xdfffeff6;
        animation->slot.speed = 0.033333335f;
        animation->slot.currentDataId = 0;
        animation->slot.frame = 0;
        animation->slot.reverseLimit = 0;
        animation->slot.forwardLimit = 0;
        move = actor->move;
        model = actor->modelResource;
        JNT_setFlags(0x8000);
        JNT_setModel(model, move);
        JNT_setMatrix(actor->jointMatrices);
        JNT_setModelMatrix();
        return;
    }
    if (slot->currentDataId == motionId && (slot->flags & 0x20000000) == 0) {
        return;
    }
    slot->currentDataId = motionId & 0x7ff;
    slot->nextDataId = 0xffff;
    slot->animQueuedFlags = 0;
    ANM_reset(slot, actor->animPackTables[(motionId >> 8) & 7]);
    slot->flags = (slot->flags & 0xdffffff6) | 0x10000;
}

INCLUDE_ASM("asm/main/nonmatchings/act_2", ACT_setMotion2);

void *ACT_animGetData(Actor *actor, unsigned int dataId)
{
    dataId &= 0x7ff;
    return PACK_getEntry(actor->animPackTables[dataId >> 8], dataId & 0xff);
}

void ACT_animGetCurrent(Actor *actor)
{
    ActorAnimSlot *slot = &actor->animation.slot;

    ANM_getEntry(slot, actor->animPackTables[(slot->currentDataId >> 8) & 7]);
}

void ACT_loadMotion(Actor *actor, int id, int bank)
{
    unsigned short kind;
    int kindFlags;
    void *pack;

    if ((id & 0x00ff0000) != 0) {
        return;
    }
    id &= 0x00ffffff;
    kindFlags = actor->resourceKind & 0xf000;
    kind = actor->resourceKind;
    if (kindFlags == 0x5000 || kindFlags == 0x6000) {
        actor->animPackTables[0] = 0;
        ACT_setMotion(actor, 0x8000);
        return;
    }
    bank = bank == 2 ? 1 : bank;
    pack = 0;
    if ((int)GameLoopState[6] <= 0x0fffffff) {
        pack = (void *)RES_loadFile(-1, 2, ((unsigned int)bank << 24) + id, 0);
        if (pack == 0) {
            pack = (void *)RES_loadFile(-1, 2, 0x01000001, 0);
            kind = actor->resourceKind;
        } else {
            kind = actor->resourceKind;
        }
    }
    actor->animPackTables[0] = pack;
    if ((kind & 0xf000) != 0x5000 && (kind & 0xf000) != 0x6000) {
        ACT_setMotion(actor, 0x8000);
    }
}

void ACT_initExMotion(Actor *target, unsigned int slot, void *motion)
{
    int i;

    if (target == 0) {
        for (i = 0; i < 64; i++) {
            actor[i].animPackTables[slot] = motion;
        }
        return;
    }

    target->animPackTables[slot] = motion;
}

void ACT_setVisible(Actor *actor, int part, int visible)
{
    if ((unsigned int)part >= 128) {
        MDL_setGroupVisible(actor->model, part, visible);
    } else {
        MDL_setVisible(actor->model, part, visible);
    }
}

void ACT_setHumanHand(Actor *actor, int hand)
{
    int left;
    int right;
    int handNameIndex;
    unsigned int pattern[4];

    if ((actor->flags & 0x4000) != 0) {
        right = hand & 0x87;
        left = (hand >> 8) & 0x87;
        if (left != right) {
            if (left != 0x80) {
                for (handNameIndex = 0; tblHand[handNameIndex] != 0; handNameIndex++) {
                    pattern[HAND_SUFFIX] = 0x5f4c0000;
                    pattern[HAND_NAME_MASK] = 0xffffffff;
                    pattern[HAND_SUFFIX_MASK] = 0xffff0000;
                    pattern[HAND_NAME] = tblHand[handNameIndex];
                    MDL_setNameVisible(actor->model, &pattern, 0);
                }
                pattern[HAND_NAME] = tblHand[left];
                pattern[HAND_SUFFIX] = 0x5f4c0000;
                pattern[HAND_NAME_MASK] = 0xffffffff;
                pattern[HAND_SUFFIX_MASK] = 0xffff0000;
                MDL_setNameVisible(actor->model, &pattern, 1);
            }
            if (right != 0x80) {
                for (handNameIndex = 0; tblHand[handNameIndex] != 0; handNameIndex++) {
                    pattern[HAND_SUFFIX] = 0x5f520000;
                    pattern[HAND_NAME_MASK] = 0xffffffff;
                    pattern[HAND_SUFFIX_MASK] = 0xffff0000;
                    pattern[HAND_NAME] = tblHand[handNameIndex];
                    MDL_setNameVisible(actor->model, &pattern, 0);
                }
                pattern[HAND_NAME] = tblHand[right];
                pattern[HAND_SUFFIX] = 0x5f520000;
                pattern[HAND_NAME_MASK] = 0xffffffff;
                pattern[HAND_SUFFIX_MASK] = 0xffff0000;
                MDL_setNameVisible(actor->model, &pattern, 1);
            }
        } else if (left != 0x80) {
            for (handNameIndex = 0; tblHand[handNameIndex] != 0; handNameIndex++) {
                pattern[HAND_SUFFIX] = 0;
                pattern[HAND_NAME_MASK] = 0xffffffff;
                pattern[HAND_SUFFIX_MASK] = 0;
                pattern[HAND_NAME] = tblHand[handNameIndex];
                MDL_setNameVisible(actor->model, &pattern, 0);
            }
            pattern[HAND_NAME] = tblHand[left];
            pattern[HAND_SUFFIX] = 0;
            pattern[HAND_NAME_MASK] = 0xffffffff;
            pattern[HAND_SUFFIX_MASK] = 0;
            MDL_setNameVisible(actor->model, &pattern, 1);
        }
    }
}
