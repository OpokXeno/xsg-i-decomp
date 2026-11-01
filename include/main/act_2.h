#ifndef INCLUDE_MAIN_ACT_2_H
#define INCLUDE_MAIN_ACT_2_H

typedef struct Actor Actor;

int ACT_jointGetAccessories(Actor *actor, unsigned int accessoryId);
Actor *ACT_setArms(Actor *actor, Actor *parent, unsigned int accessoryId, int flags);
void *ACT_animGetData(Actor *actor, unsigned int dataId);

#endif /* INCLUDE_MAIN_ACT_2_H */
