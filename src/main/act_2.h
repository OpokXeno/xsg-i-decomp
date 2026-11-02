/*
 * TU-local declarations of main/tu255 (src/main/act_2.c).
 */

#ifndef SRC_MAIN_ACT_2_H
#define SRC_MAIN_ACT_2_H

#include "shared.h"
#include "main/act_1.h"
#include "main/act_2.h"

typedef struct ActorMoveHeader {
    unsigned char unmodeled_00[8];
    unsigned short baseJointCount;
    unsigned short extraJointCount;
} ActorMoveHeader;


static void ACT_updateMotionSub(Actor *actor, int pause);
int ACT_jointGetMoveElementID(Actor *actor);
void *ACT_animGetUserData(Actor *actor);
int ACT_animCheckData(Actor *actor);
void ACT_animGetCurrent(Actor *actor);

#endif /* SRC_MAIN_ACT_2_H */
