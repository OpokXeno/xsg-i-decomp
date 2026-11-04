#ifndef INCLUDE_OV12_RG_ROBOT_H
#define INCLUDE_OV12_RG_ROBOT_H

#include "shared.h"

#include "ov12/rg_draw.h"
#include "ov12/rg_robot_spec.h"
#include "ov12/xrg_actor.h"
#include "ov12/xrg_sound.h"

struct RgWeapon;

/* Functions and data used by other translation units. */
RgStatus *CreateRgRobot(void);
void DisposeRgRobot(RgStatus *pRobot);
void RgRobotPassTime(RgStatus *pRobot, float deltaTime);
void RgRobotDisp(RgStatus *pRobot);
void RgRobotSetSpec(RgStatus *pRobot, RgRobotSpec *spec);
void RgRobotSetGeom(RgStatus *pRobot, RgGeomPoint *geometry);
void RgRobotSetEyeGeom(RgStatus *pRobot, RgGeomPoint *eyeGeometry);
void RgRobotSetAdvanceGeom(RgStatus *pRobot, RgGeomPoint *advanceGeometry);
void RgRobotSetSoundDriver(RgStatus *pRobot, XrgSound *sound);
void RgRobotSetActor(RgStatus *pRobot, XrgActorEssence *actor);
void RgRobotSetTarget(RgStatus *pRobot, RgGeomPoint *target);
void RgRobotSetRgDrawView(RgStatus *pRobot, RgDrawView *view);
void RgRobotSetMotSmooth(RgStatus *pRobot, int motSmooth);
void RgRobotSetConfuse(RgStatus *pRobot, float confuse);

void RgRobotSetWeapon(RgStatus *pRobot, int eSide, struct RgWeapon *weapon);

void RgRobotSetSpareWeapon(RgStatus *pRobot, int eSide, struct RgWeapon *weapon);

RgGeomPoint *RgRobotGetGeom(RgStatus *pRobot);
struct RgWeapon *RgRobotGetWeapon(RgStatus *pRobot, unsigned int eSide);
RgGeomPoint *RgRobotGetTarget(RgStatus *pRobot);
int RgRobotIsDead(RgStatus *pRobot);
float RgRobotGetLife(RgStatus *pRobot);
float RgRobotGetLifeMax(RgStatus *pRobot);
XrgActor *RgRobotGetActor(RgStatus *pRobot);
int RgRobotGetCharID(RgStatus *pRobot);
int RgRobotIsInvalidAttack(RgStatus *pRobot, RgVector direction);
float RgRobotGetDashTime(RgStatus *pRobot);
RgRobotSpec *RgRobotGetSpec(RgStatus *pRobot);
int RgRobotIsConfused(RgStatus *pRobot);
int RgRobotIsAcceptedCommand(RgStatus *pRobot, int command);
unsigned int RgRobotGetStatusFlags(RgStatus *pRobot);
void RgRobotAccelarate(RgStatus *pRobot, RgVector velocity);
void RgRobotAccelarateRotate(RgStatus *pRobot, float rotate);
void RgRobotShot(RgStatus *pRobot, unsigned int eType);
void RgRobotTargetting(RgStatus *pRobot);
void RgRobotBreak(RgStatus *pRobot);
void RgRobotDash(RgStatus *pRobot, RgVector direction);
void RgRobotDashContinue(RgStatus *pRobot);
void RgRobotGiveDamage(RgStatus *pRobot, RgVector direction, int damageType,
                       float damage);
void RgRobotGiveWeakDamage(RgStatus *pRobot, RgVector direction, float damage,
                           float threshold, float windowSeed,
                           float weakHitMotion, float weakHitPower);
void RgRobotHitByBody(RgStatus *pRobot, RgVector direction, RgVector position,
                      float damage, float scale);
void RgRobotDropWeapon(RgStatus *pRobot, unsigned int eType);
void RgRobotHitWeaponAttack(RgStatus *pRobot, int param);
void RgRobotInvalidAttack(RgStatus *pRobot, RgVector direction, float scale);
void RgRobotHitBG(RgStatus *pRobot, RgVector direction);
void RgRobotNearBG(RgStatus *pRobot, RgVector direction);

#endif /* INCLUDE_OV12_RG_ROBOT_H */
