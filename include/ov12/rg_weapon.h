#ifndef INCLUDE_OV12_RG_WEAPON_H
#define INCLUDE_OV12_RG_WEAPON_H

#include "shared.h"
#include "ov12/rg_equip_type.h"
#include "ov12/rg_weapon_db.h"
#include "ov12/xrg_actor.h"
#include "ov12/xrg_sound.h"
#include "ov12/rg_bgobj.h"

/* A shot a weapon keeps attached while it lasts (src/ov12/rg_shot.c). */
typedef struct RgShot RgShot;

/* The geometry group an attack weapon's collision ball joins (src/ov12/rg_geom_group.c). */
struct RgGeomGroup;

/*
 * The creation info CreateRgWeaponFromEssence hands to the essence's create
 * method: the owning robot, the equip type the weapon is mounted on
 * (asserted against RG_EQUIP_TYPE_NUM), the weapon id and the attack
 * geometry group.
 */
struct RgWeaponCreateInfo {
    int robot; /* an RgStatus * (measured: an int-typed member is needed in the record) */
    unsigned int equipType;
    int id; /* really the shot geometry group pointer (measured: a pointer-typed member reorders the stores in _InitWeaponCommon) */
    void *atkGeoms; /* the geometry group (struct RgGeomGroup) an attack weapon joins */
};

/* What RgWeaponGetRestUnit reports the remaining ammunition in. */
#define RG_WEAPON_REST_UNIT_COUNT 0   /* "shots/capacity" */
#define RG_WEAPON_REST_UNIT_PERCENT 1 /* percent of capacity */
#define RG_WEAPON_REST_UNIT_NONE 2    /* nothing to show */

/* Functions and data used by other translation units. */
void InitRgWeaponShotEssence(RgWeaponShotEssence *pEss);
void InitRgWeaponAttackEssence(RgWeaponAttackEssence *pEss);
void InitRgWeaponUnArmedEssence(RgWeaponUnArmedEssence *pEss);
void InitRgWeaponShieldEssence(RgWeaponShieldEssence *pEss);
void InitRgWeaponEnergyEssence(RgWeaponEnergyEssence *pEss);
RgWeapon *CreateRgWeaponFromEssence(RgWeaponEssence *pEss, RgWeaponCreateInfo *pInfo);
void DisposeRgWeapon(RgWeapon *weapon);
XrgActor *DisposeRgWeaponNotActor(RgWeapon *weapon);
void RgWeaponSetSound(RgWeapon *weapon, XrgSound *sound);
int RgWeaponIsLockedOn(RgWeapon *weapon);
int RgWeaponIsTargetting(RgWeapon *weapon);
float RgWeaponGetShotNum(RgWeapon *weapon);
int RgWeaponIsNotBusy(RgWeapon *weapon);
int RgWeaponGetRestUnit(RgWeapon *weapon);
int RgWeaponRobotShotMotion(RgWeapon *weapon, int eCharID);
XrgActor *RgWeaponGetActor(RgWeapon *weapon);
RgEquip *RgWeaponGetEquip(RgWeapon *weapon);
float RgWeaponGetWeight(RgWeapon *weapon);
int RgWeaponIsAttachedShot(RgWeapon *weapon);
int RgWeaponGetControlFlag(RgWeapon *weapon);
RgWeaponEssence *RgWeaponGetEss(RgWeapon *weapon);
unsigned int RgWeaponGetAttachedShot(RgWeapon *weapon, RgShot **shots);
int RgWeaponIsActiveLockOnSys(RgWeapon *weapon);
void RgWeaponPlayMotion(RgWeapon *weapon, int motion);
void RgWeaponPlayShotMotion(RgWeapon *weapon);
int RgWeaponShot(RgWeapon *weapon);
void RgWeaponSetup(RgWeapon *weapon);
void RgWeaponSetdown(RgWeapon *weapon);
void RgWeaponStopLockon(RgWeapon *weapon);
void RgWeaponRestartLockon(RgWeapon *weapon);
void RgWeaponDisposeLockon(RgWeapon *weapon);
void RgWeaponShotStop(RgWeapon *weapon);
void RgWeaponHitRobot(RgWeapon *weapon, RgStatus *robot, RgVector position);
void RgWeaponHitBG(RgWeapon *weapon, RgBgObj *bgObject, RgVector position);
void RgWeaponSubShotNum(RgWeapon *weapon, float delta);
void RgWeaponSetFree(RgWeapon *weapon);
void RgWeaponDisp(RgWeapon *weapon);
void RgWeaponPassTime(RgWeapon *weapon, float dt);
RgWeaponShotEssence *RgWeaponEssCastToShot(RgWeaponEssence *ess);
RgWeaponEnergyEssence *RgWeaponEssCastToEnergy(RgWeaponEssence *ess);
RgWeaponAttackEssence *RgWeaponEssCastToAttack(RgWeaponEssence *ess);
RgWeaponShieldEssence *RgWeaponEssCastToShield(RgWeaponEssence *ess);
RgWeaponUnArmedEssence *RgWeaponEssCastToUnArmed(RgWeaponEssence *ess);

#endif /* INCLUDE_OV12_RG_WEAPON_H */
