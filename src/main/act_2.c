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

INCLUDE_ASM("asm/main/nonmatchings/act_2", ACT_resetArms);

INCLUDE_ASM("asm/main/nonmatchings/act_2", ACT_setArms);

INCLUDE_ASM("asm/main/nonmatchings/act_2", ACT_setRelation);

INCLUDE_ASM("asm/main/nonmatchings/act_2", ACT_setFace);

void *ACT_animGetUserData(Actor *actor)
{
    return actor->animUserData;
}

int ACT_animCheckData(Actor *actor)
{
    return FCV2_checkData(actor->animData);
}

INCLUDE_ASM("asm/main/nonmatchings/act_2", ACT_updateMotionCore);

INCLUDE_ASM("asm/main/nonmatchings/act_2", ACT_updateMotion);

INCLUDE_ASM("asm/main/nonmatchings/act_2", ACT_updateMotionPause);

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

INCLUDE_ASM("asm/main/nonmatchings/act_2", ACT_animGetCurrent);

INCLUDE_ASM("asm/main/nonmatchings/act_2", ACT_loadMotion);

INCLUDE_ASM("asm/main/nonmatchings/act_2", ACT_initExMotion);

INCLUDE_ASM("asm/main/nonmatchings/act_2", ACT_setVisible);

INCLUDE_ASM("asm/main/nonmatchings/act_2", ACT_setHumanHand);
