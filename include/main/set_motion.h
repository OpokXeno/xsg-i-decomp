#ifndef INCLUDE_MAIN_SET_MOTION_H
#define INCLUDE_MAIN_SET_MOTION_H

#include "shared.h"

/* Functions and data used by other translation units. */
extern float LocaterAngle[16];
extern long EneTerritoryID[20];
extern short EfTab_Code[];
extern short EfTab_Intr[];
extern float EfTab_Scal[];
extern UnduTestStorage UnduTest;
extern UnduParam UnduTemp;
extern short EfTab_Loop[15];
void Check_Encount(EncountActor *enemy, signed char enable, signed char command);
short Get_DefaultMotion(struct Actor *actor, short motion_number);
extern void Set_Motion(struct Actor *actor, int motion, unsigned int mode);
void Homing_Search(ActorEnemyDetectView *enemy);
extern void Homing_Add(int actor_index, float x, float y, float z,
                       short target_index);
void Actor_LookAt(struct Actor *viewer);
extern void Check_Discovery(struct Actor *enemy);
int Actor_LookAt_Set(struct Actor *actor, signed char duration, const Vector4 *target_position);
int Actor_LookAt_Release(struct Actor *actor, signed char duration);
/*
 * Sound_FootStep and Enemy_ActionReady always pass 1 for `play`; when it is
 * not 1 the positional call below is skipped and the function has no other
 * effect. `ignore_se_type` is 0 at Enemy_ActionReady's one sound_index == 1
 * call and 1 everywhere else (main:0x002d3930, 0x002d396c, 0x002d3a84,
 * 0x002d06d8, 0x002d0710, 0x002d0744); when it is 0 and sound_index is 2 or
 * 4, a bank whose RES_GetEnemySeType is 1 is skipped entirely.
 */
extern void EnemySound(struct Actor *actor, short sound_index, signed char play,
                       signed char ignore_se_type);
void Actor_LookAt_Init(struct Actor *actor);
void EnemySound_StopAll(signed char which);
void Enemy_FindByEar(ActorEnemyDetectView *enemy, const Vector4 *sound_position);

#endif /* INCLUDE_MAIN_SET_MOTION_H */
