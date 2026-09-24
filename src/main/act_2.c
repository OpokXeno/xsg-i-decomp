#include "common.h"
#include "act_2.h"

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

INCLUDE_ASM("asm/main/nonmatchings/act_2", ACT_resetMatrix);

INCLUDE_ASM("asm/main/nonmatchings/act_2", ACT_resetParent);

INCLUDE_ASM("asm/main/nonmatchings/act_2", ACT_setParent);

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

INCLUDE_ASM("asm/main/nonmatchings/act_2", ACT_jointGetAccessories);

extern void ACT_resetParent(Actor *actor, Actor *other);
extern void ACT_setHumanHand(Actor *actor, int hand);

/*
 * acc_id 0x108 is the only accessory kind reset here: ACT_resetParent(actor,
 * other) drops the parent link the accessory got from ACT_setArms, then
 * ACT_setHumanHand(other, 0) puts the character's own hand back to its
 * default (main:0x00306ae8..0x00306b1c).
 */
void ACT_resetArms(Actor *actor, Actor *other, int acc_id)
{
    if (acc_id == 0x108) {
        ACT_resetParent(actor, other);
        ACT_setHumanHand(other, 0);
    }
}

INCLUDE_ASM("asm/main/nonmatchings/act_2", ACT_setArms);

INCLUDE_ASM("asm/main/nonmatchings/act_2", ACT_setRelation);

extern int ACT_setParent(Actor *actor, int type, Actor *parent, int joint, int id);
extern void ACT_setVisible(Actor *actor, int part, unsigned char visible);

/*
 * Attaches `actor` to `parent` at joint 0x30 (type 2, id `faceId`); on
 * success (main:0x00306d40..0x00306d64) it hides the base model's own FACE
 * part with ACT_setVisible(parent, 0x46414345, 0) - 0x46414345 is "FACE"
 * read from most- to least-significant byte, matching lui 0x4641/ori 0x4345
 * at 0x00306d44/0x00306d5c.
 */
int ACT_setFace(Actor *actor, Actor *parent, int faceId)
{
    int result;

    result = ACT_setParent(actor, 2, parent, 0x30, faceId);
    if (result != 0) {
        ACT_setVisible(parent, 0x46414345, 0);
    }
    return result;
}

void *ACT_animGetUserData(Actor *actor)
{
    return actor->animUserData;
}

int ACT_animCheckData(Actor *actor)
{
    return FCV2_checkData(actor->animData);
}

INCLUDE_ASM("asm/main/nonmatchings/act_2", ACT_updateMotionCore);

extern void ACT_updateMotionCore(Actor *actor, int pause);

void ACT_updateMotion(Actor *actor)
{
    ACT_updateMotionCore(actor, 0);
}

void ACT_updateMotionPause(Actor *actor)
{
    ACT_updateMotionCore(actor, 1);
}

INCLUDE_ASM("asm/main/nonmatchings/act_2", ACT_updateMotionSub);

INCLUDE_ASM("asm/main/nonmatchings/act_2", ACT_setDrawEnv);

INCLUDE_ASM("asm/main/nonmatchings/act_2", ACT_modelDraw);

INCLUDE_ASM("asm/main/nonmatchings/act_2", ACT_modelDrawSub);

INCLUDE_ASM("asm/main/nonmatchings/act_2", ACT_setModelWrapper);

INCLUDE_ASM("asm/main/nonmatchings/act_2", ACT_loadResource);

/* main:0x00307c38 is `jr ra` / nop: no argument is read and no state changes. */
void ACT_initMTNResource(void)
{
}

/* main:0x00307c40 is `jr ra` / nop: no argument is read and no state changes. */
void ACT_resourceInit(void)
{
}

INCLUDE_ASM("asm/main/nonmatchings/act_2", ACT_initMotion);

INCLUDE_ASM("asm/main/nonmatchings/act_2", ACT_setMotion);

INCLUDE_ASM("asm/main/nonmatchings/act_2", ACT_setMotion2);

extern void *PACK_getEntry(void *table, int id);

void *ACT_animGetData(Actor *actor, unsigned int dataId)
{
    dataId &= 0x7ff;
    return PACK_getEntry(actor->animPackTables[dataId >> 8], dataId & 0xff);
}

extern void ANM_getEntry(ActorAnimSlot *slot, void *entry);

void ACT_animGetCurrent(Actor *actor)
{
    ActorAnimSlot *slot = &actor->animSlot;

    ANM_getEntry(slot, actor->animPackTables[(slot->currentDataId >> 8) & 7]);
}

INCLUDE_ASM("asm/main/nonmatchings/act_2", ACT_loadMotion);

INCLUDE_ASM("asm/main/nonmatchings/act_2", ACT_initExMotion);

INCLUDE_ASM("asm/main/nonmatchings/act_2", ACT_setVisible);

INCLUDE_ASM("asm/main/nonmatchings/act_2", ACT_setHumanHand);
