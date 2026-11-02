#include "common.h"
#include "act_2.h"
#include "main/act_1.h"
#include "main/res_get_path.h"
#include "main/jnt.h"
#include "main/jc_face.h"
#include "main/enemy_1.h"
#include "main/act_2.h"
#include "main/fcv2.h"
#include "main/look.h"
#include "main/matrix_print.h"
#include "main/xgl_light.h"
#include "main/xgl_studio.h"
#include "quadword.h"

/* Partial shape binding: original lhu loads +2 for the 0x40-byte joint
 * matrix index and +4 for the shape identifier passed to the model renderer. */
typedef struct ActorShapeBinding {
    unsigned char unmodeled_00[2];
    unsigned short jointIndex;
    unsigned short shapeId;
} ActorShapeBinding;

typedef unsigned int ActorMatrixRow __attribute__((mode(TI)));

typedef struct ActorJointMatrix {
    ActorMatrixRow row0;
    ActorMatrixRow row1;
    ActorMatrixRow row2;
    ActorMatrixRow row3;
} ActorJointMatrix;

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

typedef unsigned int Quadword __attribute__((mode(TI)));

struct JntProducer;

struct JntMoveResource;

enum { HAND_NAME, HAND_SUFFIX, HAND_NAME_MASK, HAND_SUFFIX_MASK };

extern Actor actor[64];
extern unsigned int GameLoopState[];
extern EnemyWorkEntry enepc[16];

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
static void ACT_updateMotionCore(Actor *actor, int pause);
static void ACT_modelDrawSub(Actor *actor);
extern void *PACK_getEntry(void *table, int id);
extern void ANM_getEntry(ActorAnimSlot *slot, void *entry);
extern void MDL_setGroupVisible(void *model, int part, int visible);
extern void MDL_setVisible(void *model, int part, int visible);
extern void JC_setParent(void *work, void *parameter, int flags);
extern void *JNT_getAccessories(struct JntMoveResource *move);
extern void JC_face(void *work, void *parameter, int flags);

Actor *ACT_setParent2(Actor *actor, int type, Actor *parent, int joint, int flags,
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
extern void JNT_setModel(void *model, void *move);
extern void JNT_setMatrix(void *matrices);
extern void JNT_setModelMatrix(void);
extern void *ANM_reset(ActorAnimSlot *slot, void *pack);
extern int RES_loadFile(int command, int callback, int resource_id, int flags);
extern int MDL_setNameVisible(void *model, const void *pattern, int visible);
extern int JNT_startProduction(float frame, JntProducer *producer, void *actor);
extern void JNT_setInterpMatrix(void *matrixBuffer, float interpolation);
extern void JNT_setFCurve(void *fcurve, int slot);
extern void JNT_getRootTrans(void *translation);
extern void ANM_update(void *slot);
extern char *strstr(const char *haystack, const char *needle);
static void ACT_updateMotionSub(Actor *actor, int pause);
extern void nmlModelSetLight(void *light, void *colors);
extern void nmlModelSetShapeId(int shapeId, float weight);
void ACT_setDrawEnv(Actor *actor);
extern void nmlModelSetScale(void *scale);
extern void nmlModelClear(void);
extern void nmlModelSetParent(void *parent);
extern void nmlModelSetWindow(unsigned short window);
extern void nmlModelSetRenderLevel(unsigned int level);
extern void nmlModelSetAxisSymmetry(float symmetry);
extern void nmlModelSetFaceModel(int enabled);
extern void nmlModelSetHumanModel(unsigned char human);
extern void nmlModelSetSortOffset(float offset);
extern void nmlModelSetPixelAlpha(unsigned int alpha);
extern void nmlModelSetPartsPixelAlpha(short part, short alpha);
extern void nmlModelSetClip(int clip);
extern void nmlModelSetShadowHeight(float height);
extern void nmlModelSetTexture(const char *texture);
extern void nmlModelSetPlace(void *matrix);
struct NmlModel;
extern void nmlModelEntry(struct NmlModel *model);
extern void nmlModelGetGblPosition(void *position);
extern void MDL_partsSetVisible(void *model);
extern void MDL_draw(void *model, void *matrix);
extern float xglPointLengthXZ(const Vector4 *point1, const Vector4 *point2);
extern int nmlModelAskNonLinearCamera(void);
extern void nmlModelSetMapShadowParts(int partId);
extern void nmlModelSetTexMap(int mode, int size, float weight);
extern void *xglStudioGetLight2(void);
void *JNT_computeMatrix(JntWork *work, JntStaticChannel *channels, int argument);
void *LOOK_sebo_cont(JntWork *work, JntStaticChannel *channels, int argument);
void *LOOK_head_cont(JntWork *work, JntStaticChannel *channels, int argument);
void *LOOK_eyeL_cont(JntWork *work, JntStaticChannel *channels, int argument);
void *LOOK_eyeR_cont(JntWork *work, JntStaticChannel *channels, int argument);

CameraRecord tcamera[8] = {0};

static unsigned int tblHand[9] = {
    0x54453030, 0x54453031, 0x54453032, 0x54453033,
    0x54453034, 0x54453035, 0x54453036, 0x54453037, 0
};

float newsToDirection[16] = { 0.0f, 3.1415927410125732f, 1.5707963705062866f, 2.3561944961547852f, -1.5707963705062866f, -2.3561944961547852f, 0.0f, 0.0f, 0.0f, 0.0f, 0.78539818525314331f, 0.0f, -0.78539818525314331f, 0.0f, 0.0f, 0.0f };

void ACT_resetMatrix(Actor *actor)
{
    ActorJointMatrix *matrices = actor->jointMatrices;
    void *move = actor->move;
    void *model = actor->modelResource;
    ActorJointMatrix *previous;
    JntProducer *producer;
    JntWork *work = (JntWork *)0x70000000u;
    void *channels;

    if (!(move && matrices)) {
        return;
    }
    CUR_MATRIX_Unit4s( );
    CUR_MATRIX_SetT(&(actor->position));
    CUR_MATRIX_Rx4s((const int *)&(actor->rotation.x));
    CUR_MATRIX_Ry4s((const int *)&(actor->rotation.y));
    CUR_MATRIX_Rz4s((const int *)&(actor->rotation.z));
    CUR_MATRIX_Sxyz4s(&actor->scale);
    CUR_MATRIX_Get((void *)matrices);
    previous = actor->previousJointMatrices;
    QuadwordCopyMatrix(previous, matrices);
    {
        unsigned int flagsBits = 0x8000;

        do { JNT_setFlags(flagsBits); } while (0);
    }
    do {
        JNT_setModel(model, move);
        producer = &actor->jointProducer;
    } while (0);
    JNT_setMatrix((actor)->jointMatrices);
    work->channel_start_index = 0u;
    work->current_index = 0u;
    producer->deferred_count = 0u;
    work->producer = producer;
    work->current_frame = 0u;
    JNT_resetMatrix(work, work->channels);
    JNT_setMatrix((actor)->previousJointMatrices);
    work->channel_start_index = 0;
    work->current_index = 0;
    producer->deferred_count = 0;
    work->producer = producer;
    work->current_frame = 0;
    JNT_resetMatrix(work, work->channels);
}

void ACT_resetParent(Actor *actor, Actor *parent)
{
    int childIndex;
    int followingIndex;
    JntProducer *producer = &actor->jointProducer;

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
    JntProducer *producer;
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
            producer = &actor->jointProducer;
            attachment->type = type;
            attachment->joint = joint;
            parent->attachmentCount++;
            attachment->matrix = &parent->jointMatrices;
            JNT_initProducer(producer);
            if ((flags & 2) != 0) {
                JNT_addConsumer(producer, 0, (JntProductionConsumer)JC_copyChain, (int)attachment);
            } else {
                JNT_addConsumer(producer, 0, (JntProductionConsumer)JC_setParent, (int)attachment);
            }
        }
    }
    return actor;
}

Actor *ACT_setParent2(Actor *actor, int type, Actor *parent, int joint, int flags,
                      void (*filter)(void *work, void *parameter, int flags))
{
    int slot;
    int slotIndex;
    JntProducer *producer;
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
            producer = &actor->jointProducer;
            attachment->type = type;
            attachment->joint = joint;
            parent->attachmentCount++;
            attachment->matrix = &parent->jointMatrices;
            JNT_addConsumer(producer, 0, (JntProductionConsumer)filter, (int)attachment);
        }
    }
    return actor;
}

int ACT_jointGetMoveElementID(Actor *actor)
{
    if (actor->jointProducer.move_element_id < 0) {
        if (actor->move != 0) {
            actor->jointProducer.move_element_id = JNT_getMoveElement(actor->move);
        } else {
            actor->jointProducer.move_element_id = 0;
        }
    }
    return actor->jointProducer.move_element_id;
}

int ACT_jointGetAccessories(Actor *actor, unsigned int accessoryId)
{
    ActorAccessoryJointTable *accessories;
    ActorAccessoryJointTable *emptyTable;
    int result = -1;

    accessories = actor->jointProducer.accessory_joints;
    if (accessories == 0) {
        actor->jointProducer.accessory_joints = JNT_getAccessories(actor->move);
        accessories = actor->jointProducer.accessory_joints;
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
            filter = (void (*)(void *, void *, int))JC_copyChain;
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

/*
 * Attaches `actor` to `parent` at joint 0x30 (type 2, id `faceId`); on
 * success (main:0x00306d40..0x00306d64) it hides the base model's own FACE
 * part with ACT_setVisible(parent, 0x46414345, 0) - 0x46414345 is "FACE"
 * read from most- to least-significant byte, matching lui 0x4641/ori 0x4345
 * at 0x00306d44/0x00306d5c.
 */
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

static inline void ACT_runQueuedMotion(Actor *actor, ActorAnimSlot *queue)
{
    if ((queue->flags & 0x1000) && queue->nextDataId != 0xffff) {
        if ((unsigned int)actor != GameLoopState[1] || enepc[0].encount_lock == 0) {
            ACT_setMotion2(actor, queue->nextDataId, queue->animQueuedFlags);
        }
    }
}

static void ACT_updateMotionSub(Actor *actor, int pause)
{
    ActorAnimationState *animation = &actor->animation;
    ActorAnimSlot *slot = &animation->slot;
    ActorAnimSlot *pending = slot;
    unsigned int slotFlags = slot->flags;
    unsigned int flags = slotFlags;
    void *matrices;
    void *model;
    void *move;
    float frame;

    if (pause == 0) {
        if (actor->pauseCounter > 0) {
            actor->pauseCounter--;
        } else {
            actor->pauseCounter = 0;
        }
    }
    if (0x408 == (*(unsigned long long *)actor & 0x408)) {
        return;
    }
    if (0 != actor->transitionFrames) {
        actor->transitionFrames -= 1;
        actor->animation.slot.flags &= 0xbfffffff;
        flags = pending->flags;
    } else if (actor->holdFrames >= 0) {
        actor->holdFrames = actor->holdFrames + -1;
        actor->animation.slot.flags |= 0x40000000;
        if (0 > actor->holdFrames) {
            actor->animation.slot.flags &= 0xbfffffff;
        }
        flags = pending->flags;
    }
    ACT_runQueuedMotion(actor, pending);
    slot = &animation->slot;
    model = actor->modelResource;
    move = actor->move;
    matrices = actor->jointMatrices;
    if (matrices == 0
        || (model != 0 && (*(unsigned long long *)model & 0xffffff) != 0x78656c)
        || (0 != move && (*(unsigned short *)move != 0x4e4a || ((char *)move)[2] != 'T'))) {
        return;
    }
    CUR_MATRIX_Unit4s();
    CUR_MATRIX_SetT(&actor->position);
    CUR_MATRIX_Rx4s((const int *)&actor->rotation.x);
    CUR_MATRIX_Ry4s((const int *)&actor->rotation.y);
    CUR_MATRIX_Rz4s((const int *)&actor->rotation.z);
    CUR_MATRIX_Sxyz4s(&actor->scale);
    CUR_MATRIX_Get(matrices);
    actor->flags &= ~0x1000;
    if ((slotFlags & 0x10000) != 0) {
        void *swapTmp;
        void *valueTmp;

        swapTmp = actor->jointMatrices;
        valueTmp = actor->valueRecords;
        actor->jointMatrices = actor->previousJointMatrices;
        actor->previousJointMatrices = swapTmp;
        actor->animation.slot.bufferIndex = (actor->animation.slot.bufferIndex + 1u) & 1u;
        swapTmp = actor->staticValueRecords;
        actor->staticValueRecords = valueTmp;
        actor->valueRecords = swapTmp;
        QuadwordCopyMatrix(actor->jointMatrices, *&actor->previousJointMatrices);
    }
    FCV2_setStep(slot->speed);
    JNT_animSetFlags(slot->flags);
    if ((slotFlags & 2) != 0 && actor->pauseCounter == 0) {
        unsigned int pausedFlags = 0x8002u;

        if (1 == actor->kind) {
            pausedFlags = 0x8082u;
        }
        if ((actor->flags & 0x200) != 0) {
            pausedFlags |= 0x100;
        }
        if ((actor->flags & 8) != 0 && (actor->drawEnv.flags & 0x100) == 0) {
            pausedFlags |= 0x200;
        }
        if (model != 0) {
            if (strstr((char *)model + 0x10, "chaos") != 0) {
                pausedFlags |= 0x40000;
            }
            if (strstr((char *)model + 0x10, "boss_016") != 0
                || strstr((char *)model + 0x10, "boss_019") != 0) {
                pausedFlags |= 0x20000;
            }
        }
        JNT_setFlags(pausedFlags);
        frame = 1.0f - (animation->blendEndFrame - slot->frame) * (1.0f / animation->blendDuration);
        JNT_setInterpMatrix(actor->previousJointMatrices, frame);
        JNT_setCurve(actor->staticValueRecords, actor->valueRecords);
    } else {
        unsigned int plainFlags = 0x8000u;

        if ((actor->flags & 0x200) != 0) {
            plainFlags = 0x8100;
        }

        if ((actor->flags & 8) != 0 && (actor->drawEnv.flags & 0x100) == 0) {
            plainFlags |= 0x200;
        }
        JNT_setFlags(plainFlags);
    }
    JNT_setClipR(animation->blendWork[3]);
    if (FCV2_checkData(animation->animData) != 0 && animation->animData != 0) {
        animation->animData = 0;
    }
    JNT_setModel(model, move);
    JNT_setFCurve(animation->animData, slot->channelMask);
    JNT_setMatrix(actor->jointMatrices);
    JNT_setCurve(actor->staticValueRecords, actor->valueRecords);
    frame = slot->frame;
    if (0.0f <= slot->speed) {
        if (slot->forwardLimit < frame) {
            frame = slot->forwardLimit;
        }
    } else {
        if (frame < slot->reverseLimit) {
            frame = slot->reverseLimit;
        }
    }
    JNT_startProduction(frame, &actor->jointProducer, actor);
    JNT_getRootTrans(&actor->rootTranslation);
    if ((actor->jointProducer.pending_output & 0x10) != 0) {
        actor->flags |= 0x2000;
    }
    flags = slot->flags;
    if ((flags & 0x10000000) != 0 && (actor->jointProducer.pending_output & 0x40) != 0) {
        actor->flags = (actor->flags | 0x1000) & 0xffffdfff;
    }
    if ((slotFlags & 0x10000) != 0) {
        slot->flags = slot->flags & 0xfffeffff;
    }
    ANM_update(slot);
}

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

static void ACT_modelDrawSub(Actor *actor)
{
    void *model;
    void *matrixResource;
    void *move;
    f32 (*matrices)[4];
    Actor *parent;
    unsigned int flagsNow;
    Matrix4 matrixBuffer;
    AlignedVector4 scaleVector;
    void (*drawHook)(Actor *);
    int lightFlag;
    int i;
    short (*partAlpha)[2];
    unsigned int texMode;
    unsigned int parentFlags;

    if ((actor->flags & 0x1008) != 0) {
        return;
    }
    model = actor->modelResource;
    matrixResource = actor->matrixResource;
    move = actor->move;
    if (model == 0) {
        return;
    }
    if (move != 0 && (actor->flags & 0x2000) != 0) {
            JNT_animSetFlags(actor->animation.slot.flags);
            JNT_setFlags(0x8000);
            JNT_setModel(model, move);
            JNT_setMatrix(actor->jointMatrices);
            JNT_setModelMatrix();
            i = 0u;
            if (1 <= actor->jointProducer.deferred_count) {
                while (actor->jointProducer.deferred_count > i) {
                    ActorShapeBinding *shapeRecord = actor->jointProducer.deferred_limits[i];
                    Matrix4 *jointRow = (Matrix4 *)actor->jointMatrices +
                        shapeRecord->jointIndex;

                    if ((actor->drawEnv.flags & 0x400) == 0) {
                        nmlModelSetShapeId(shapeRecord->shapeId,
                                           (*jointRow)[3][2]);
                    }
                    i = i + 1;
                }
            }
            actor->flags &= 0xffffdfffu;
            matrices = actor->jointMatrices;
    } else {
        matrices = actor->jointMatrices;
    }
    lightFlag = 1;
    MATRIX_convert4(matrixBuffer, matrices);
    drawHook = actor->draw;
    flagsNow = actor->flags;
    parent = actor->parent;
    if ((flagsNow & 0x800) == 0) {
        if (((unsigned int)drawHook - 0x1f0000 < 0x1e10000) && (((unsigned int)drawHook & 3) == 0)) {
            drawHook(actor);
            flagsNow = actor->flags;
        } else if (parent != 0) {
            parentFlags = parent->flags;
            if ((parentFlags & 0x8000) != 0) {
                nmlModelSetLight(&parent->light.color_modifier_matrix, &parent->light.normal_matrix);
                lightFlag = 0;
                parentFlags = parent->flags;
            }
            drawHook = parent->draw;
            if ((parentFlags & 0x800) != 0) {
                ACT_setDrawEnv(parent);
            } else {
                if (((unsigned int)drawHook - 0x1f0000 < 0x1e10000) && (((unsigned int)drawHook & 3) == 0)) {
                    drawHook(parent);
                }
            }
            flagsNow = actor->flags;
        }
    } else {
        ACT_setDrawEnv(actor);
        flagsNow = actor->flags;
    }
    if ((flagsNow & 0x8000) != 0) {
        xglLightCalcMatrix(&actor->light);
        nmlModelSetLight(&actor->light.color_modifier_matrix, &actor->light.normal_matrix);
    } else if (lightFlag) {
        StudioLight *light = xglStudioGetLight2();
        xglLightCalcMatrix(light);
        nmlModelSetLight(&light->color_modifier_matrix, &light->normal_matrix);
    }
    scaleVector = actor->scale;
    if (parent != 0) {
        __asm__ __volatile__(
            "lqc2 vf3, 0(%0)\n\t"
            "lqc2 vf2, 0(%1)\n\t"
            "vmul.xyz vf2, vf2, vf3\n\t"
            "sqc2 vf2, 0(%0)"
            :
            : "r"(&scaleVector), "r"(&parent->scale)
            : "memory");
    }
    nmlModelSetScale(&scaleVector);
    if (((actor->drawEnv.renderLevel & 0x80) != 0) && nmlModelAskNonLinearCamera() != 0) {
        StudioCamera *camera = xglStudioSelectGetActiveCamera(0);
        actor->drawEnv.renderLevel = actor->drawEnv.renderLevel & 0xfffffeff;
        if (actor->drawEnv.cameraCullDistance < xglPointLengthXZ(&actor->globalPosition, &camera->position.vector)) {
            actor->drawEnv.renderLevel = actor->drawEnv.renderLevel | 0x100;
        }
    }
    if (parent != 0) {
        if (actor->kind == 1 && (parent->drawEnv.flags & 0x80) != 0) {
            nmlModelClear();
            return;
        }
        nmlModelSetParent(parent->modelResource);
        nmlModelSetWindow(parent->windowId);
        nmlModelSetRenderLevel(parent->drawEnv.renderLevel);
        if ((parent->drawEnv.flags & 0x10) != 0) {
            nmlModelSetAxisSymmetry(parent->position.y);
        }
    } else {
        for (i = 0; i < actor->mapShadowPartCount; i = i + 1) {
            nmlModelSetMapShadowParts(actor->mapShadowPartIds[i]);
        }
        texMode = 0;
        switch (actor->shadow_kind) {
        case 5:
            nmlModelSetTexMap(1, actor->shadowSize, actor->shadowClipScale);
            break;
        case 10:
            texMode = 0x40000;
            nmlModelSetTexMap(texMode | 1, actor->shadowSize, actor->shadowClipScale);
            break;
        case 6:
            nmlModelSetTexMap(3, actor->shadowSize, actor->shadowClipScale);
            break;
        case 7:
            texMode = 0x10000;
            nmlModelSetTexMap(texMode | 1, actor->shadowSize, actor->shadowClipScale);
            break;
        case 8:
            texMode = 0x20000;
            nmlModelSetTexMap(texMode | 1, actor->shadowSize, actor->shadowClipScale);
            break;
        case 9:
            nmlModelSetTexMap(0x30000, actor->shadowSize, actor->shadowClipScale);
            break;
        default:
            break;
        }
        nmlModelSetWindow(actor->windowId);
        nmlModelSetRenderLevel(actor->drawEnv.renderLevel);
        if ((actor->drawEnv.flags & 0x10) != 0) {
            nmlModelSetAxisSymmetry(actor->position.y);
        }
    }
    nmlModelSetFaceModel(actor->kind == 1);
    nmlModelSetHumanModel(actor->humanKind);
    nmlModelSetSortOffset(actor->sortOffset);
    nmlModelSetPixelAlpha(actor->drawEnv.pixelAlpha);
    i = 0;
    if (actor->drawEnv.partAlphaCount >= 1) {
        partAlpha = actor->drawEnv.partAlpha;
        do {
            nmlModelSetPartsPixelAlpha((*partAlpha)[1], (*partAlpha)[0]);
            i = i + 1;
            partAlpha++;
        } while (i < actor->drawEnv.partAlphaCount);
    }
    if ((actor->drawEnv.flags & 0x200) != 0) {
        nmlModelSetClip(1);
    }
    if ((actor->drawEnv.flags & 0x40) != 0) {
        nmlModelSetShadowHeight(0.0f);
    }
    if ((actor->flags & 0x4000) != 0) {
        MDL_draw(&actor->model[0].model, matrixBuffer);
        nmlModelGetGblPosition(&actor->globalPosition);
    } else {
        nmlModelSetTexture(matrixResource);
        nmlModelSetPlace(matrixBuffer);
        MDL_partsSetVisible(&actor->model[0].model);
        nmlModelEntry(model);
        nmlModelGetGblPosition(&actor->globalPosition);
    }
    (void)move;
    (void)scaleVector;
}

void ACT_setModelWrapper(Actor *actor, int unused)
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

void ACT_loadResource(Actor *actor, int resourceId)
{
    int fileId = resourceId & 0x00ffffff;
    ActorResource *resource = (ActorResource *)RES_loadFile(-1, 0, fileId, 0);
    void *model;
    void *matrices;
    void *move;
    Actor *face;

    if (0 == resource) {
        if ((fileId & 0x00ff0000) == 0) {
            resource = (ActorResource *)RES_loadFile(-1, 0, fileId & 0x00ffff00, 0);
        }
    }
    if (0 != resource) {
        model = resource->model;
        matrices = resource->matrices;
        move = resource->move;
    } else {
        model = 0;
        matrices = 0;
        move = 0;
    }
    actor->modelResource = model;
    actor->matrixResource = matrices;
    actor->move = move;
    ACT_allocMatrix(actor, -1);
    ACT_setModelWrapper(actor, 0);
    if (0 == (resourceId & 0x00fff000u)) {
        int kind = actor->inUseId;

        {
        if (0x600 >= (kind & 0x00ffff00)) {
            face = ACT_create(-2, kind + 0x10000);
            ACT_loadResource(face, kind + 0x10000);
            if (face->modelResource != 0) {
                face->shadow_kind = 0u;
                face->flags &= ~0x20;
                ACT_initMotion(face);
                ACT_setRelation(face, actor, 0x8200u, 1);
            } else {
                ACT_dispose2(face, 1u);
            }
        }
        }
        ACT_setHumanHand(actor, 0);
        if (0 == move) {
            actor->shadow_kind = 0;
            actor->flags &= ~0x20;
        }
    }
}

/* main:0x00307c38 is `jr ra` / nop: no argument is read and no state changes. */
/* main:0x00307c40 is `jr ra` / nop: no argument is read and no state changes. */
void ACT_initMTNResource(void)
{
}

void ACT_resourceInit(void)
{
}

void ACT_initMotion(Actor *actor)
{
    static float min[3];
    static float max[3];
    ActorAnimationState *animation = &actor->animation;
    ActorAnimSlot *slot = &animation->slot;
    int enemyIndex;
    Vector4 *cleared = actor->cleared600;
    unsigned short channelMask = 0x1ff;
    int flags = 8;

    slot->currentDataId = 0xffff;
    slot->nextDataId = 0xffff;
    slot->flags = flags;
    slot->channelMask = channelMask;
    slot->speed = 0.033333335f;
    animation->blendDuration = 0.26666668f;
    animation->blendWork[2] = 0;
    animation->blendWork[0] = 0;
    animation->blendWork[1] = 0;
    animation->blendWork[3] = 0;
    animation->animData = 0;
    QuadwordZero(&cleared[0]);
    QuadwordZero(&cleared[1]);
    actor->attachmentCount = 0;
    JNT_initProducer(&actor->jointProducer);
    JNT_addConsumer(&actor->jointProducer, 0, JNT_computeMatrix, 0);

    do {
        if (actor->modelResource != 0 && strstr((char *)actor->modelResource + 0x10, "_face") != 0) {
            JNT_addConsumer(&actor->jointProducer, 5, LOOK_eyeL_cont, (int)actor);
            JNT_addConsumer(&actor->jointProducer, 8, LOOK_eyeR_cont, (int)actor);
            break;
        }

        enemyIndex = actor->number;
        if (enemyIndex < 16) {
            enepc[enemyIndex].look_target.lookat_point = 0;
            enepc[enemyIndex].look_target.control = 0;
            enepc[enemyIndex].look_angles.x = enepc[enemyIndex].look_angles.y = enepc[enemyIndex].look_angles.z = 0.0f;
            enepc[enemyIndex].look_limits.minimum = min;
            enepc[enemyIndex].look_limits.maximum = max;
            min[0] = -0.52359879f;
            min[1] = -1.39626348f;
            min[2] = max[0] = 0.52359879f;
            max[1] = 1.39626348f;
            max[2] = 1.91986215f;
        }
        JNT_addConsumer(&actor->jointProducer, 36, LOOK_sebo_cont, (int)actor);
        JNT_addConsumer(&actor->jointProducer, 48, LOOK_head_cont, (int)actor);
    } while (0);
}

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

void ACT_setMotion2(Actor *actor, int motionId, unsigned int flags)
{
    ActorAnimationState *animation = &actor->animation;
    ActorAnimSlot *slot = &animation->slot;
    unsigned int slotFlags;
    unsigned int bank;

    if (motionId == 0x80000000U) {
        void *move;
        void *model;

        ACT_resetMatrix(actor);
        slot->currentDataId = 0u;
        animation->animPack = (void *)0;
        animation->animData = 0u;
        animation->animUserData = 0u;
        slot->flags &= 0xdfffeff6u;
        slot->frame = 0.0f;
        slot->speed = 0.033333335f;
        slot->reverseLimit = 0.0f;
        slot->forwardLimit = 0.0f;
        move = actor->move;
        model = actor->modelResource;
        JNT_setFlags(0x8000u);
        JNT_setModel(model, move);
        JNT_setMatrix(actor->jointMatrices);
        JNT_setModelMatrix();
    } else {
        if (slot->currentDataId != motionId || (flags & 0x20000000) != 0) {
            if (slot->currentDataId == motionId) {
                slotFlags = slot->flags;
            } else {
                slotFlags = slot->flags;
                if (0 == (slotFlags & 0x2)) {
                    flags |= 0x10000;
                } else if (slot->nextDataId == 0xffff) {
                    void *swapTmp;
                    void *valueTmp;

                    if (0 == (slotFlags & 0x20u)) {
                        slot->nextDataId = motionId;
                        slot->animQueuedFlags = flags;
                        slot->forwardLimit = animation->blendEndFrame;
                        return;
                    }
                    swapTmp = actor->jointMatrices;
                    valueTmp = actor->valueRecords;
                    actor->jointMatrices = actor->previousJointMatrices;
                    actor->previousJointMatrices = swapTmp;
                    actor->animation.slot.bufferIndex = (actor->animation.slot.bufferIndex + 1) & 1;
                    swapTmp = actor->staticValueRecords;
                    actor->staticValueRecords = valueTmp;
                    actor->valueRecords = swapTmp;
                    QuadwordCopyMatrix(actor->jointMatrices, actor->previousJointMatrices);
                    animation->previousDataId = slot->currentDataId;
                    slot->nextDataId = 0xffff;
                    slot->currentDataId = motionId & 0x7ff;
                    flags &= 0xdfffffff;
                    slot->flags = (slot->flags & 0xdffffff7) | flags;
                    slot->animQueuedFlags = 0;
                    bank = (motionId >> 8) & 7;
                    ANM_reset(slot, actor->animPackTables[bank]);
                    return;
                }
            }
            animation->previousDataId = slot->currentDataId;
            slot->nextDataId = 0xffff;
            slot->currentDataId = motionId & 0x7ff;
            flags &= 0xdfffffff;
            slot->flags = (slotFlags & 0xdffffff6) | flags;
            slot->animQueuedFlags = 0;
            bank = (motionId >> 8) & 7;
            ANM_reset(slot, actor->animPackTables[bank]);
        } else {
            flags &= 0xdfffffff;
            slot->flags |= flags;
        }
    }
}

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
    kindFlags = actor->inUseId & 0xf000;
    kind = actor->inUseId;
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
            kind = actor->inUseId;
        } else {
            kind = actor->inUseId;
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
