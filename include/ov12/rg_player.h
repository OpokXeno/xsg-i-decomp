#ifndef INCLUDE_OV12_RG_PLAYER_H
#define INCLUDE_OV12_RG_PLAYER_H

#include "shared.h"
#include "ov12/rg_equip_type.h"
#include "ov12/rg_robot_spec.h"
#include "ov12/rg_weapon_db.h"

typedef struct RgRobotControl RgRobotControl;
typedef struct RgGeomGroup RgGeomGroup;
typedef struct RgCamera RgCamera;

/*
 * The player's set-up data: InitRgPlayerEssence initialises it and
 * _InitRgPlayer asserts "pDat != NIL" on it.
 */
typedef struct RgPlayerEssence {
    RgVector position;                                   /* +0x00 */
    RgVector direction;                                  /* +0x10 */
    RgRobotSpec *spec;                                   /* +0x20: the robot spec RgRobotSetSpec receives when non-NULL */
    RgWeaponEssence *weaponEssence[RG_EQUIP_TYPE_NUM];      /* +0x24 */
    RgWeaponEssence *spareWeaponEssence[RG_EQUIP_TYPE_NUM]; /* +0x30 */
    int actorID;                                         /* +0x3C: the actor ID InitXrgActorEssence receives */
} RgPlayerEssence;

/*
 * No function of this TU reads or writes byte offset 0x14 (_InitRgPlayer
 * creates the geometry groups at 0x08..0x10 and 0x18..0x1C and skips it), so
 * it stays an unmodeled span rather than an invented field.
 */
typedef struct RgPlayer {
    RgStatus *robot;
    RgRobotControl *control;
    RgGeomGroup *bodyGeoms;
    RgGeomGroup *shotGeoms;
    RgGeomGroup *atkGeoms;
    unsigned char unmodeled_14[4];
    RgGeomGroup *eyeGeoms;
    RgGeomGroup *advGeoms;
    RgCamera *traceCamera;
} RgPlayer;

/* Functions and data used by other translation units. */
void InitRgPlayerEssence(RgPlayerEssence *pDat);
RgPlayer *CreateRgPlayer(RgPlayerEssence *pDat);
void DisposeRgPlayer(RgPlayer *pPlayer);
void RgPlayerSetControl(RgPlayer *pPlayer, RgRobotControl *pControl);
void RgPlayerSetTraceCamera(RgPlayer *pPlayer, RgCamera *pCamera);
void RgPlayerSetLightCost(RgPlayer *pPlayer);
RgStatus *RgPlayerGetRobot(RgPlayer *pPlayer);
RgGeomGroup *RgPlayerGetBodyGeoms(RgPlayer *pPlayer);
RgGeomGroup *RgPlayerGetShotGeoms(RgPlayer *pPlayer);
RgGeomGroup *RgPlayerGetAtkGeoms(RgPlayer *pPlayer);
RgGeomGroup *RgPlayerGetEyeGeoms(RgPlayer *pPlayer);
RgGeomGroup *RgPlayerGetAdvGeoms(RgPlayer *pPlayer);
void RgPlayerControl(RgPlayer *pPlayer);
void RgPlayerDisp(RgPlayer *pPlayer);
void RgPlayerPassTime(RgPlayer *pPlayer, float deltaTime);

#endif /* INCLUDE_OV12_RG_PLAYER_H */
