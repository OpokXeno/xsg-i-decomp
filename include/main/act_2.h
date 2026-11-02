#ifndef INCLUDE_MAIN_ACT_2_H
#define INCLUDE_MAIN_ACT_2_H

typedef struct Actor Actor;

int ACT_jointGetAccessories(Actor *actor, unsigned int accessoryId);
Actor *ACT_setArms(Actor *actor, Actor *parent, unsigned int accessoryId, int flags);
void *ACT_animGetData(Actor *actor, unsigned int dataId);

/* Functions and data used by other translation units. */
void ACT_resetParent(Actor *actor, Actor *parent);
void ACT_modelDraw(Actor *actor);
void ACT_resetMatrix(Actor *actor);
void ACT_setHumanHand(Actor *actor, int hand);
/*
 * acc_id 0x108 is the only accessory kind reset here: ACT_resetParent(actor,
 * other) drops the parent link the accessory got from ACT_setArms, then
 * ACT_setHumanHand(other, 0) puts the character's own hand back to its
 * default (main:0x00306ae8..0x00306b1c).
 */
Actor *ACT_setParent(Actor *actor, int type, Actor *parent, int joint, int flags);
/* ACT_setVisible's third argument is forwarded to both MDL tail calls. */
void ACT_setVisible(Actor *actor, int part, int visible);
extern void ACT_initMotion(Actor *actor);
void ACT_setMotion2(Actor *actor, int motionId, unsigned int flags);
void ACT_resetArms(Actor *actor, Actor *other, int acc_id);
Actor *ACT_setFace(Actor *actor, Actor *parent, int faceId);
void ACT_initMTNResource(void);
void ACT_resourceInit(void);
void ACT_setModelWrapper(Actor *actor, int unused);
void ACT_setMotion(Actor *actor, int motionId);
void ACT_updateMotion(Actor *actor);
void ACT_initExMotion(Actor *target, unsigned int slot, void *motion);
void ACT_loadMotion(Actor *actor, int id, int bank);
void ACT_loadResource(Actor *actor, int resourceId);

#endif /* INCLUDE_MAIN_ACT_2_H */
