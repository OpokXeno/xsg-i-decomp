/*
 * OV12 original TU 26: 0x00a19388..0x00a1c7d8 (90 functions)
 */
#include "common.h"
#include "shared.h"
#include "ov12/assert_prog.h"
#include "ov12/rg_geom_ball.h"
#include "ov12/rg_geom_point.h"
#include "ov12/rg_geom_robot.h"
#include "ov12/rg_heap.h"
#include "ov12/rg_matrices_effector.h"
#include "ov12/xrg_rand_int.h"
#include "ov12/xrg_sound.h"
#include "rg_weapon.h"
#include "ov12/rg_bgobj.h"
#include "ov12/rg_error.h"
#include "ov12/rg_geom_group.h"
#include "ov12/rg_robot.h"
#include "ov12/rg_shot.h"
#include "ov12/create_rg.h"

/* Distance in front of the weapon at which a locked-on weapon aims its constraint. */
#define RG_WEAPON_LOCKON_AIM_DISTANCE 100.0f

/* Radius of the collision ball of an attack weapon. */
#define RG_WEAPON_ATTACK_BALL_RADIUS 1.5f

/* Default melee damage, and the energy weapon defaults. */
#define RG_WEAPON_ATTACK_DEFAULT_DAMAGE 10.0f
#define RG_WEAPON_ENERGY_DEFAULT_CAPACITY 200.0f
#define RG_WEAPON_ENERGY_DEFAULT_UP_TEMP 100.0f
#define RG_WEAPON_ENERGY_DEFAULT_LIMIT_TEMP 100.0f
#define RG_WEAPON_ENERGY_DEFAULT_DOWN_TEMP 50.0f

/* Declared here: functions of other translation units. */

/* Declared here: strings of the OV12 data block used by the assert and warning calls. */
extern const char weaponAssertAttachNumInRange[]; /* "pAttach->m_uAttachNum < ATTACH_MAX" */
extern const char weaponAssertEquipTypeInRange[]; /* "RG_EQUIP_TYPE_MIN <= (eEquipType) && (eEquipType) < RG_EQUIP_TYPE_NUM" */
extern const char weaponAssertShotTypePNotNil[]; /* assert string of _ShotTypeShot */
extern const char weaponAssertRobotNotNilEffector[]; /* assert string of _InitWeaponCommonEffector */
extern const char weaponAssertAttackGeomNotNil[]; /* assert string of _InitWeaponAttack */
extern const char weaponAssertWeaponPointerNotNil[]; /* assert string of RgWeaponGetRestUnit */
extern const char weaponNoEssenceWarnFormat[]; /* RgWarn format of RgWeaponGetRestUnit */

/* Prototypes of the functions defined below that are used before their definition. */
static int _GetNumAttach(RgWeaponAttach *pAttach);
void _CommonDispose(RgWeapon *weapon);
void _CommonPassTime(RgWeapon *weapon, float dt);
void _CommonDisp(RgWeapon *weapon);
void _InitWeaponCommon(RgWeapon *weapon, RgWeaponEssence *base, RgWeaponCreateInfo *info);
void _DestructWeaponCommon(RgWeapon *weapon);
void _InitEssenceCommon(RgWeaponEssence *ess);
void _InitWeaponCommonEffector(RgWeapon *weapon, RgStatus *robot, unsigned int equipType);
int _ShotTypeShot(RgWeapon *pWeapon);
void _InitWeaponShot(RgWeaponShotType *weapon, RgWeaponShotEssence *essence, RgWeaponCreateInfo *info);
void _InitWeaponAttack(RgWeaponAttackType *weapon, RgWeaponAttackEssence *essence, RgWeaponCreateInfo *info);
void _InitWeaponUnArmedType(RgWeaponAttackType *weapon, RgWeaponUnArmedEssence *essence, RgWeaponCreateInfo *info);
void _InitWeaponShield(RgWeapon *weapon, RgWeaponShieldEssence *essence, RgWeaponCreateInfo *info);
void _InitEnergyType(RgWeaponEnergyType *weapon, RgWeaponEnergyEssence *essence, RgWeaponCreateInfo *info);
void _PlayShotMotion(RgWeapon *weapon);
void _CheckLockOn(RgWeapon *weapon);
int _GetParentCharID(RgWeapon *weapon);
int _GetDefShotPosDir(void *posOwner, RgVector position, RgVector direction);
void _GetDefShotPosDirFunc(RgWeapon *weapon, RgVector position, RgVector direction);
void _GetShotInfo(RgWeapon *weapon, RgShotRequest *info);

const char weaponAssertAttachNotNil[16] = "pAttach != NIL";

const char weaponSourcePath[24] = "../rg_weapon.euc.c";

const char weaponAssertWeaponNotNil[16] = "pWpn != NIL";

const char weaponAssertEssenceNotNil[16] = "pEss != NIL";

const char weaponAssertInfoNotNil[16] = "pInfo != NIL";

const char weaponHitEffectFallbackName[16] = "eff15.ptcl";

const char weaponRobotHitEffectName[16] = "eff11.ptcl";

const char weaponAssertEssenceAndInfoNotNil[32] = "pEss != NIL && pInfo != NIL";

const char weaponAssertCreateMethodNotNil[32] = "pEss->m_pCreateMethod != NIL";

const char weaponAssertCharIdInRobotRange[48] = "0 <= eCharID && eCharID < RG_ACTOR_CHAR_ROBNUM";

const char weaponAssertRobotNotNil[16] = "pRobot != NIL";

static void _InitAttach(RgWeaponAttach *pAttach)
{
    if (pAttach == 0) {
        assert_prog(weaponAssertAttachNotNil, weaponSourcePath, 38);
    }
    pAttach->m_uAttachNum = 0;
}

static void _ReleaseAttach(RgWeaponAttach *pAttach)
{
    unsigned int i;

    if (pAttach == 0) {
        assert_prog(weaponAssertAttachNotNil, weaponSourcePath, 46);
    }
    for (i = 0; i < pAttach->m_uAttachNum; i++) {
        RgShotRelease(pAttach->shots[i]);
    }
}

static int _GetNumAttach(RgWeaponAttach *pAttach)
{
    if (pAttach == 0) {
        assert_prog(weaponAssertAttachNotNil, weaponSourcePath, 54);
    }
    return pAttach->m_uAttachNum;
}

static unsigned int _GetAllAttach(RgWeaponAttach *pAttach, RgShot **shots)
{
    unsigned int i;

    if (pAttach == 0) {
        assert_prog(weaponAssertAttachNotNil, weaponSourcePath, 62);
    }
    for (i = 0; i < pAttach->m_uAttachNum; i++) {
        shots[i] = pAttach->shots[i];
    }
    return pAttach->m_uAttachNum;
}

static void _AddShotAttach(RgWeaponAttach *pAttach, RgShot *shot)
{
    if (pAttach == 0) {
        assert_prog(weaponAssertAttachNotNil, weaponSourcePath, 71);
    }
    if (shot == 0) {
        return;
    }
    if (pAttach->m_uAttachNum >= ATTACH_MAX) {
        assert_prog(weaponAssertAttachNumInRange, weaponSourcePath, 73);
    }
    pAttach->shots[pAttach->m_uAttachNum++] = shot;
}

/* Removes released shots from the attach list, keeping the order of the live ones. */
static void _CheckAttach(RgWeaponAttach *pAttach)
{
    RgShot *alive[ATTACH_MAX];
    RgShot *shot;
    unsigned int i;
    unsigned int aliveNum;

    if (pAttach == 0) {
        assert_prog(weaponAssertAttachNotNil, weaponSourcePath, 83);
    }
    aliveNum = 0;
    for (i = 0; i < pAttach->m_uAttachNum; i++) {
        shot = pAttach->shots[i];
        if (RgShotIsReleased(shot) == 0) {
            alive[aliveNum++] = shot;
        }
    }
    for (i = 0; i < aliveNum; i++) {
        pAttach->shots[i] = alive[i];
    }
    pAttach->m_uAttachNum = aliveNum;
}

void _CommonDispose(RgWeapon *weapon)
{
}

/*
 * Switches the actor to the shot end motion once its current motion has ended,
 * then drops released shots from the attach list.
 */
void _CommonPassTime(RgWeapon *weapon, float dt)
{
    int charID;
    int motion;
    int offset;

    charID = _GetParentCharID(weapon);
    if (weapon->actor != 0 && 0 <= charID && charID < RG_ACTOR_CHAR_ROBNUM) {
        offset = weapon->shotMotionOffset;
        motion = weapon->shotEndMotionTable[charID][offset];
        if (motion != -1 && XrgActorIsEndOfMotion(weapon->actor) != 0) {
            XrgActorSetMotion(weapon->actor, motion);
        }
    }
    _CheckAttach(&weapon->shotAttach);
}

/*
 * Updates the lock-on effector each frame. It follows the locked target, or aims
 * ahead of an equipped weapon that has the lock-on control flag set. Otherwise it
 * is deactivated.
 */
void _CommonDisp(RgWeapon *weapon)
{
    RgVector position;
    RgVector fromPosition;
    RgVector toPosition;
    RgVector aimPoint;
    RgGeomPoint *target;
    RgStatus *robot;

    robot = weapon->robot;
    _CheckLockOn(weapon);
    if (weapon->lockedOn != 0) {
        target = RgRobotGetTarget(robot);
        if (target != 0) {
            __RgGeomPointGetPos(target, position, weaponSourcePath, 287);
        } else {
            XrgClearVector(position);
        }
        if (weapon->lockOnEffector != 0) {
            RgMatricesEffectorSetActivity(weapon->lockOnEffector, 1);
            RgMatEffConstraintSetTarget((MatrixConstraint *) weapon->lockOnEffector, position);
        }
    } else if (weapon->lockOnEffector != 0) {
        if (weapon->equipped != 0 && (weapon->controlFlags & RG_WEAPON_CONTROL_LOCKON)) {
            _GetDefShotPosDirFunc(weapon, fromPosition, toPosition);
            XrgSubVector(aimPoint, toPosition, fromPosition);
            XrgNormalizeVector(aimPoint, aimPoint);
            XrgScaleVector(aimPoint, aimPoint, RG_WEAPON_LOCKON_AIM_DISTANCE);
            XrgAddVector(aimPoint, fromPosition, aimPoint);
            RgMatricesEffectorSetActivity(weapon->lockOnEffector, 1);
            RgMatEffConstraintSetTarget((MatrixConstraint *) weapon->lockOnEffector, aimPoint);
        } else {
            RgMatricesEffectorSetActivity(weapon->lockOnEffector, 0);
        }
    }
}

/*
 * Fills a weapon from its essence and the create info: state flags, motion tables,
 * equip and sound settings, and the lock-on angles for the equip side.
 */
void _InitWeaponCommon(RgWeapon *weapon, RgWeaponEssence *base, RgWeaponCreateInfo *info)
{
    int i;
    int j;
    unsigned int equipType;
    enum RgActorFlag actorFlag;
    float angle;

    equipType = info->equipType;
    if (equipType >= RG_EQUIP_TYPE_NUM) {
        assert_prog(weaponAssertEquipTypeInRange, weaponSourcePath, 329);
    }
    weapon->id = info->id;
    weapon->shotNum = base->capacity;
    weapon->shotRequested = 0;
    weapon->lockedOn = 0;
    weapon->targeting = 0;
    weapon->controlFlags = base->controlFlags;
    weapon->robot = (RgStatus *) info->robot;
    weapon->shotEssence = base->shotEssence;
    weapon->atkGeoms = info->atkGeoms;
    weapon->dispose = _CommonDispose;
    weapon->disp = _CommonDisp;
    weapon->weight = base->mass;
    weapon->ess = base;
    actorFlag = base->actorFlag;
    weapon->shoot = 0;
    weapon->passTime = 0;
    weapon->shotStop = 0;
    weapon->hitRobot = 0;
    weapon->hitBG = 0;
    weapon->isBusy = 0;
    weapon->setFree = 0;
    do {
        weapon->shotMotionOffset = equipType;
    } while (0);
    if (actorFlag != 0) {
        weapon->actor = CreateXrgActor(&base->actorSpec[equipType]);
    } else {
        weapon->actor = 0;
    }
    _InitAttach(&weapon->shotAttach);
    weapon->aimJointCursor = 0;
    CopyRgEquip(&weapon->equip, &base->equip);
    weapon->useMask = base->useMask;
    weapon->lockOnEffector = 0;
    /* Copy the motion tables for every character and equip type. */
    for (i = 0; i < RG_EQUIP_TYPE_NUM; i++) {
        for (j = 0; j < RG_ACTOR_CHAR_ROBNUM; j++) {
            weapon->shotMotionTable[j][i] = base->shotMotionTable[j][i];
            weapon->shotStartMotionTable[j][i] = base->shotStartMotionTable[j][i];
            weapon->shotEndMotionTable[j][i] = base->shotEndMotionTable[j][i];
        }
    }
    /* Lock-on angle limits, wrapped into [-pi, pi]; the right side mirrors them. */
    if (info->equipType == RG_EQUIP_TYPE_LEFT) {
        angle = base->lockOnAngleMin;
        while (angle > XRG_PI) {
            angle -= XRG_TWO_PI;
        }
        while (angle < -XRG_PI) {
            angle += XRG_TWO_PI;
        }
        weapon->lockOnAngleMin = angle;
        angle = base->lockOnAngleMax;
        while (angle > XRG_PI) {
            angle -= XRG_TWO_PI;
        }
        while (angle < -XRG_PI) {
            angle += XRG_TWO_PI;
        }
        weapon->lockOnAngleMax = angle;
    } else if (info->equipType == RG_EQUIP_TYPE_RIGHT) {
        angle = -base->lockOnAngleMax;
        while (angle > XRG_PI) {
            angle -= XRG_TWO_PI;
        }
        while (angle < -XRG_PI) {
            angle += XRG_TWO_PI;
        }
        weapon->lockOnAngleMin = angle;
        angle = -base->lockOnAngleMin;
        while (angle > XRG_PI) {
            angle -= XRG_TWO_PI;
        }
        while (angle < -XRG_PI) {
            angle += XRG_TWO_PI;
        }
        weapon->lockOnAngleMax = angle;
    } else {
        angle = base->lockOnAngleMin;
        while (angle > XRG_PI) {
            angle -= XRG_TWO_PI;
        }
        while (angle < -XRG_PI) {
            angle += XRG_TWO_PI;
        }
        weapon->lockOnAngleMin = angle;
        angle = base->lockOnAngleMax;
        while (angle > XRG_PI) {
            angle -= XRG_TWO_PI;
        }
        while (angle < -XRG_PI) {
            angle += XRG_TWO_PI;
        }
        weapon->lockOnAngleMax = angle;
    }
    weapon->robotHitSound = base->robotHitSound;
    weapon->equipped = 0;
    weapon->lockOnStopped = 0;
    weapon->bgHitSound = base->bgHitSound;
    weapon->sound = 0;
    weapon->lifeSound = base->lifeSound;
    weapon->shotSound = base->shotSound;
    weapon->robotHitVolume = base->robotHitVolume;
    weapon->bgHitVolume = base->bgHitVolume;
    weapon->lifeVolume = base->lifeVolume;
    weapon->shotVolume = base->shotVolume;
}

void _DestructWeaponCommon(RgWeapon *weapon)
{
    XrgActor *actor;

    _ReleaseAttach(&weapon->shotAttach);
    actor = weapon->actor;
    if (actor != 0) {
        DisposeXrgActor(actor);
    }
    if (weapon->lockOnEffector == 0) {
        return;
    }
    if (weapon->robot == 0) {
        return;
    }
    XrgActorDisposeEffector(RgRobotGetActor(weapon->robot), weapon->lockOnEffector);
}

/* Sets every field of a weapon essence to its default. */
void _InitEssenceCommon(RgWeaponEssence *ess)
{
    unsigned int i;
    unsigned int j;
    float weight = 1.0f;

    ess->aimAxis = RG_WEAPON_AIM_AXIS_NONE;
    ess->lockOnAngleMin = -XRG_EIGHTH_PI;
    ess->lockOnAngleMax = XRG_THREE_EIGHTH_PI;
    ess->capacity = RG_WEAPON_DEFAULT_CAPACITY;
    ess->m_pCreateMethod = 0;
    ess->controlFlags = 0;
    ess->shotEssence = 0;
    ess->shotPitch = 0.0f;
    ess->shotYaw = 0.0f;
    ess->shotPntNum = 0;
    ess->unmodeled_68 = 0;
    InitRgEquip(&ess->equip);
    ess->useMask = -1;
    for (i = 0; i < RG_ACTOR_CHAR_ROBNUM; i++) {
        ess->shotMotionTable[i][RG_EQUIP_TYPE_LEFT] = RG_WEAPON_MOTION_SHOT_LEFT;
        ess->shotMotionTable[i][RG_EQUIP_TYPE_RIGHT] = RG_WEAPON_MOTION_SHOT_RIGHT;
        ess->shotMotionTable[i][RG_EQUIP_TYPE_BACK] = -1;
        for (j = 0; j < RG_EQUIP_TYPE_NUM; j++) {
            ess->shotStartMotionTable[i][j] = -1;
            ess->shotEndMotionTable[i][j] = -1;
        }
    }
    ess->actorFlag = 1;
    InitXrgActorEssence(&ess->actorSpec[0], -1);
    InitXrgActorEssence(&ess->actorSpec[1], -1);
    InitXrgActorEssence(&ess->actorSpec[2], -1);
    ess->mass = weight;
    ess->shotSound = RG_WEAPON_SOUND_SHOT;
    ess->isDefault = 0;
    ess->displayName[0] = 0;
    ess->databaseName[0] = 0;
    ess->robotHitSound = RG_WEAPON_SOUND_HIT;
    ess->bgHitSound = RG_WEAPON_SOUND_HIT;
    ess->lifeSound = 0;
    ess->robotHitVolume = RG_WEAPON_VOLUME_MAX;
    ess->bgHitVolume = RG_WEAPON_VOLUME_MAX;
    ess->lifeVolume = RG_WEAPON_VOLUME_MAX;
    ess->eventLevel = -1;
    ess->shotVolume = 1;
}

/*
 * Fires a shot when the cooldown has run out and no shot is still attached. Returns
 * 1 if a shot was fired, else 0. A firing starts the cooldown and uses one shot
 * unless the weapon has infinite shots.
 */
int _ShotTypeShot(RgWeapon *pWeapon)
{
    RgWeaponShotType *weapon = (RgWeaponShotType *) pWeapon;

    if (weapon == 0) {
        assert_prog(weaponAssertShotTypePNotNil, weaponSourcePath, 520);
    }
    if (weapon->cooldown > 0.0f || _GetNumAttach(&weapon->common.shotAttach) != 0) {
        return 0;
    }
    {
        RgShotRequest info;

        _GetShotInfo(&weapon->common, &info);
        if (weapon->common.shotEssence != 0) {
            RgShot *shot;

            shot = CreateRgShotFromEssence(weapon->common.shotEssence, &info);
            _AddShotAttach(&weapon->common.shotAttach, shot);
            RgShotSetSoundDriver(shot, weapon->common.sound);
            RgShotSetHitSound(shot, weapon->common.robotHitSound, weapon->common.robotHitVolume);
            RgShotSetBgHitSound(shot, weapon->common.bgHitSound, weapon->common.bgHitVolume);
            RgShotSetNoLifeSound(shot, weapon->common.lifeSound, weapon->common.lifeVolume);
            RgShotSetHideSound(shot, weapon->common.shotSound);
            XrgSoundRingVol(weapon->common.sound, weapon->common.shotSound, weapon->common.shotVolume);
            if (weapon->common.shotNum != RG_WEAPON_SHOT_INFINITE) {
                weapon->common.shotNum = weapon->common.shotNum - 1.0f;
            }
            weapon->cooldown = weapon->busyTime;
            return 1;
        }
    }
    return 0;
}

int _ShotTypeIsBusy(RgWeapon *pWeapon)
{
    RgWeaponShotType *weapon = (RgWeaponShotType *) pWeapon;

    if (weapon == 0) {
        assert_prog(weaponAssertWeaponNotNil, weaponSourcePath, 572);
    }
    return weapon->cooldown > 0.0f;
}

void _ShotTypeFree(RgWeapon *pWeapon)
{
    RgWeaponShotType *weapon = (RgWeaponShotType *) pWeapon;

    if (weapon == 0) {
        assert_prog(weaponAssertWeaponNotNil, weaponSourcePath, 580);
    }
    weapon->cooldown = 0.0f;
}

/* Counts the cooldown down while no shot is attached. */
void _ShotTypePassTime(RgWeapon *pWeapon, float dt)
{
    RgWeaponShotType *weapon = (RgWeaponShotType *) pWeapon;
    float cooldown;

    if (_GetNumAttach(&weapon->common.shotAttach) == 0) {
        cooldown = weapon->cooldown - dt;
        weapon->cooldown = cooldown;
        if (cooldown < 0.0f) {
            weapon->cooldown = 0.0f;
        }
    }
}

/*
 * Creates the lock-on constraint effector for the left or right equip, unless the
 * control flags exclude it.
 */
void _InitWeaponCommonEffector(RgWeapon *weapon, RgStatus *robot,
                               unsigned int equipType)
{
    XrgActor *actor;
    XrgActor *owner;

    if (weapon == 0) {
        assert_prog(weaponAssertWeaponNotNil, weaponSourcePath, 607);
    }
    if (robot == 0) {
        assert_prog(weaponAssertRobotNotNilEffector, weaponSourcePath, 608);
    }
    if ((weapon->controlFlags & RG_WEAPON_CONTROL_KIND_MASK) == RG_WEAPON_CONTROL_KIND_FULL_MOT) {
        return;
    }
    if ((weapon->controlFlags & RG_WEAPON_CONTROL_NO_ROBMOT) != 0) {
        return;
    }
    actor = RgRobotGetActor(robot);
    owner = weapon->actor;
    if (equipType == RG_EQUIP_TYPE_LEFT) {
        weapon->lockOnEffector = XrgActorCreateEffConstraint(actor, 0, owner);
    } else if (equipType == RG_EQUIP_TYPE_RIGHT) {
        weapon->lockOnEffector = XrgActorCreateEffConstraint(actor, 1, owner);
    }
}

void _InitWeaponShot(RgWeaponShotType *weapon, RgWeaponShotEssence *essence,
                     RgWeaponCreateInfo *info)
{
    _InitWeaponCommon(&weapon->common, &essence->common, info);
    weapon->common.shoot = _ShotTypeShot;
    weapon->common.passTime = _ShotTypePassTime;
    weapon->common.isBusy = _ShotTypeIsBusy;
    weapon->common.setFree = _ShotTypeFree;
    weapon->busyTime = essence->busyTime;
    weapon->cooldown = 0.0f;
    _InitWeaponCommonEffector(&weapon->common, (RgStatus *) info->robot, info->equipType);
}

RgWeapon *_CreateWeaponShotType(void *pEss, RgWeaponCreateInfo *pInfo)
{
    RgWeaponShotType *pWpn;

    pWpn = RgHeapAlloc(InstanceOfRgHeap(), sizeof(RgWeaponShotType), weaponSourcePath, 668);
    if (pEss == 0) {
        assert_prog(weaponAssertEssenceNotNil, weaponSourcePath, 671);
    }
    if (pWpn == 0) {
        assert_prog(weaponAssertWeaponNotNil, weaponSourcePath, 672);
    }
    if (pInfo == 0) {
        assert_prog(weaponAssertInfoNotNil, weaponSourcePath, 673);
    }
    _InitWeaponShot(pWpn, pEss, pInfo);
    return &pWpn->common;
}

/* Sets a shot-type essence to its defaults: lock-on control and a busy time of 0.4. */
void InitRgWeaponShotEssence(RgWeaponShotEssence *pEss)
{
    if (pEss == 0) {
        assert_prog(weaponAssertEssenceNotNil, weaponSourcePath, 684);
    }
    _InitEssenceCommon(&pEss->common);
    pEss->common.m_pCreateMethod = _CreateWeaponShotType;
    pEss->common.controlFlags = RG_WEAPON_CONTROL_LOCKON;
    pEss->busyTime = 0.4f;
}

int _ShotAttackType(RgWeapon *pWeapon)
{
    RgWeaponAttackType *weapon = (RgWeaponAttackType *) pWeapon;

    if (weapon == 0) {
        assert_prog(weaponAssertWeaponNotNil, weaponSourcePath, 725);
    }
    if (weapon->shotSoundPlaying == 0) {
        weapon->shotSoundPlaying = 1;
        XrgSoundRingVol(weapon->common.sound, weapon->common.shotSound,
                        weapon->common.shotVolume);
    }
    return 0;
}

void _ShotStopAttackType(RgWeapon *pWeapon)
{
    RgWeaponAttackType *weapon = (RgWeaponAttackType *) pWeapon;

    if (weapon == 0) {
        assert_prog(weaponAssertWeaponNotNil, weaponSourcePath, 737);
    }
    weapon->shotSoundPlaying = 0;
    weapon->active = 1;
}

/*
 * Moves the attack geometry with the followed joint while the attack is active,
 * and resets its status otherwise.
 */
void _PassTimeAttackType(RgWeapon *pWeapon, float dt)
{
    RgWeaponAttackType *weapon = (RgWeaponAttackType *) pWeapon;
    /*
     * XrgActorGetJointLocal fills the whole 4x4 local transform of the
     * followed joint; only its translation row (the last four floats) is a
     * position, read below through &jointLocal[12].
     */
    RgMatrix jointLocal;

    if (weapon->followActor != 0 &&
            XrgActorGetJointLocal(weapon->followActor, weapon->followJoint, jointLocal) != 0) {
        /*
         * shotRequested and active are two separate early-exit tests in the original,
         * not one if / else if condition. Combining them changes the guard scheduling,
         * so the shotRequested test keeps its own goto to the shared reset call.
         */
        if (weapon->common.shotRequested == 0) {
            goto resetStatus;
        }
        if (weapon->active != 0) {
            if (!(RgGeomGetStatus(&weapon->geom->point.geom) & RG_GEOM_STATUS_ACTIVE)) {
                RgGeomSetStatus(&weapon->geom->point.geom, RG_GEOM_STATUS_ACTIVE);
                XrgCopyVector(weapon->followPosition, &jointLocal[12]);
            }
            RgGeomPointSetPos(&weapon->geom->point, weapon->followPosition);
            RgGeomPointMovePos(&weapon->geom->point, &jointLocal[12]);
            XrgCopyVector(weapon->followPosition, &jointLocal[12]);
        } else {
resetStatus:
            RgGeomResetStatus(&weapon->geom->point.geom, RG_GEOM_STATUS_ACTIVE);
        }
    }
}

void _DestructAttackType(RgWeapon *pWeapon)
{
    RgWeaponAttackType *weapon = (RgWeaponAttackType *) pWeapon;
    RgGeomFree(&weapon->geom->point.geom);
}

void _HitBgAttackType(RgWeapon *pWeapon, RgBgObj *bgObject, RgVector position)
{
    RgWeaponAttackType *weapon = (RgWeaponAttackType *) pWeapon;

    if (RgBgObjTryToBreak(bgObject, weapon->damage) != 0) {
        weapon->active = 0;
    }
}

/*
 * Handles a hit on a robot. A valid attack deals damage and plays the hit effect;
 * an invalid one gives the robot an invalid-attack reaction.
 */
void _HitRobotAttackType(RgWeapon *pWeapon, RgStatus *robot, RgVector position)
{
    RgWeaponAttackType *weapon = (RgWeaponAttackType *) pWeapon;

    RgVector ownPosition;
    RgVector targetPosition;
    RgVector direction;
    RgGeomPoint *ownGeom;
    RgGeomPoint *targetGeom;
    RgWeaponAttackEssence *attackEssence;

    if (weapon == 0) {
        assert_prog(weaponAssertWeaponNotNil, weaponSourcePath, 802);
    }
    if (weapon->common.robot != 0) {
        ownGeom = RgRobotGetGeom(weapon->common.robot);
        targetGeom = RgRobotGetGeom(robot);
        __RgGeomPointGetPos(ownGeom, ownPosition, weaponSourcePath, 811);
        __RgGeomPointGetPos(targetGeom, targetPosition, weaponSourcePath, 812);
        weapon->active = 0;
        XrgSubVector(direction, ownPosition, targetPosition);
        if (RgRobotIsInvalidAttack(robot, direction) == 0) {
            attackEssence = RgWeaponEssCastToAttack(weapon->common.ess);
            if (attackEssence != 0) {
                CreateRgHitEffectPosDir(position, direction, attackEssence->hitEffectName, weaponHitEffectFallbackName);
            }
            RgRobotGiveDamage(robot, direction, 0, weapon->damage);
            XrgSoundRingVol(weapon->common.sound, weapon->common.robotHitSound, weapon->common.robotHitVolume);
            return;
        }
        CreateRgHitEffectPosDir(position, direction, weaponRobotHitEffectName, 0);
        RgRobotInvalidAttack(robot, direction, weapon->damage);
    }
}

/*
 * Initializes an attack weapon: its callbacks, follow joint and damage, and its
 * collision ball, made from the attack geometry group or a default ball.
 */
void _InitWeaponAttack(RgWeaponAttackType *weapon, RgWeaponAttackEssence *essence,
                       RgWeaponCreateInfo *info)
{
    void *group;

    _InitWeaponCommon(&weapon->common, &essence->common, info);
    *(void **) &weapon->followJoint = *(void **) &essence->followJoint;
    weapon->common.shotNum = 1.0f;
    weapon->common.passTime = _PassTimeAttackType;
    weapon->common.dispose = _DestructAttackType;
    weapon->common.shoot = _ShotAttackType;
    weapon->common.shotStop = _ShotStopAttackType;
    weapon->common.hitRobot = _HitRobotAttackType;
    weapon->common.hitBG = _HitBgAttackType;
    group = info->atkGeoms;
    weapon->followExtra = (int) *(char **) &essence->followExtra;
    weapon->damage = essence->damage;
    weapon->followActor = weapon->common.actor;
    weapon->active = 1;
    weapon->common.lockedOn = 0;
    weapon->common.targeting = 0;
    weapon->common.shotEssence = 0;
    weapon->shotSoundPlaying = 0;
    if (group != 0) {
        weapon->geom = RgGeomGroupCreateBall(group, &weapon->common);
    } else {
        weapon->geom = CreateRgGeomBall(RG_WEAPON_ATTACK_BALL_RADIUS, 1.0f);
    }
    if (weapon->geom == 0) {
        assert_prog(weaponAssertAttackGeomNotNil, weaponSourcePath, 880);
    }
    RgGeomBallSetRadius(weapon->geom, RG_WEAPON_ATTACK_BALL_RADIUS);
    RgGeomResetStatus(&weapon->geom->point.geom, RG_GEOM_STATUS_ACTIVE);
}

RgWeapon *_CreateWeaponAttackType(void *pEss, RgWeaponCreateInfo *pInfo)
{
    RgWeaponAttackType *pWpn;

    pWpn = RgHeapAlloc(InstanceOfRgHeap(), sizeof(RgWeaponAttackType), weaponSourcePath, 889);
    if (pEss == 0) {
        assert_prog(weaponAssertEssenceNotNil, weaponSourcePath, 892);
    }
    if (pWpn == 0) {
        assert_prog(weaponAssertWeaponNotNil, weaponSourcePath, 893);
    }
    if (pInfo == 0) {
        assert_prog(weaponAssertInfoNotNil, weaponSourcePath, 894);
    }
    _InitWeaponAttack(pWpn, pEss, pInfo);
    return &pWpn->common;
}

/* Sets an attack essence to its defaults: attack motions, default damage and control flags. */
void InitRgWeaponAttackEssence(RgWeaponAttackEssence *pEss)
{
    unsigned int i;

    if (pEss == 0) {
        assert_prog(weaponAssertEssenceNotNil, weaponSourcePath, 905);
    }
    _InitEssenceCommon(&pEss->common);
    pEss->common.m_pCreateMethod = _CreateWeaponAttackType;
    for (i = 0; i < RG_ACTOR_CHAR_ROBNUM; i++) {
        pEss->common.shotMotionTable[i][RG_EQUIP_TYPE_LEFT] = RG_WEAPON_MOTION_ATTACK_LEFT;
        pEss->common.shotMotionTable[i][RG_EQUIP_TYPE_RIGHT] = RG_WEAPON_MOTION_ATTACK_RIGHT;
        pEss->common.shotMotionTable[i][RG_EQUIP_TYPE_BACK] = -1;
    }
    pEss->followJoint = 8;
    pEss->followExtra = 8;
    pEss->common.controlFlags = RG_WEAPON_CONTROL_ADV | RG_WEAPON_CONTROL_TARGET_WTH | RG_WEAPON_CONTROL_KIND_ATTACK;
    pEss->damage = RG_WEAPON_ATTACK_DEFAULT_DAMAGE;
    pEss->hitEffectName[0] = '\0';
}

/* Initializes an unarmed attack weapon. The follow joint depends on the equip side and the character. */
void _InitWeaponUnArmedType(RgWeaponAttackType *weapon, RgWeaponUnArmedEssence *essence,
                            RgWeaponCreateInfo *info)
{
    int charID;

    _InitWeaponAttack(weapon, essence, info);
    weapon->followActor = RgRobotGetActor((RgStatus *) info->robot);
    charID = RgRobotGetCharID((RgStatus *) info->robot);
    if (charID == 3) {
        if (info->equipType == RG_EQUIP_TYPE_LEFT) {
            weapon->followJoint = 2;
        } else if (info->equipType == RG_EQUIP_TYPE_RIGHT) {
            weapon->followJoint = charID;
        } else {
            weapon->followJoint = 20;
        }
    } else {
        if (info->equipType == RG_EQUIP_TYPE_LEFT) {
            weapon->followJoint = 0;
        } else if (info->equipType == RG_EQUIP_TYPE_RIGHT) {
            weapon->followJoint = info->equipType;
        } else {
            weapon->followJoint = 20;
        }
    }
}

RgWeapon *_CreateWeaponUnArmedType(void *pEss, RgWeaponCreateInfo *pInfo)
{
    RgWeaponAttackType *pWpn;

    pWpn = RgHeapAlloc(InstanceOfRgHeap(), sizeof(RgWeaponAttackType), weaponSourcePath, 969);
    if (pEss == 0) {
        assert_prog(weaponAssertEssenceNotNil, weaponSourcePath, 972);
    }
    if (pWpn == 0) {
        assert_prog(weaponAssertWeaponNotNil, weaponSourcePath, 973);
    }
    if (pInfo == 0) {
        assert_prog(weaponAssertInfoNotNil, weaponSourcePath, 974);
    }
    _InitWeaponUnArmedType(pWpn, pEss, pInfo);
    return &pWpn->common;
}

/* Sets an unarmed attack essence to its defaults, with no actor. */
void InitRgWeaponUnArmedEssence(RgWeaponUnArmedEssence *pEss)
{
    unsigned int i;

    if (pEss == 0) {
        assert_prog(weaponAssertEssenceNotNil, weaponSourcePath, 988);
    }
    InitRgWeaponAttackEssence(pEss);
    pEss->common.m_pCreateMethod = _CreateWeaponUnArmedType;
    pEss->common.actorFlag = 0;
    pEss->common.controlFlags = RG_WEAPON_CONTROL_ADV | RG_WEAPON_CONTROL_TARGET_WTH | RG_WEAPON_CONTROL_KIND_UNARMED;
    for (i = 0; i < RG_ACTOR_CHAR_ROBNUM; i++) {
        pEss->common.shotMotionTable[i][RG_EQUIP_TYPE_LEFT] = RG_WEAPON_MOTION_UNARMED_LEFT;
        pEss->common.shotMotionTable[i][RG_EQUIP_TYPE_RIGHT] = RG_WEAPON_MOTION_UNARMED_RIGHT;
        pEss->common.shotMotionTable[i][RG_EQUIP_TYPE_BACK] = -1;
    }
    pEss->followJoint = -1;
}

int _ShotShieldType(RgWeapon *weapon)
{
    if (weapon == 0) {
        assert_prog(weaponAssertWeaponNotNil, weaponSourcePath, 1026);
    }
    return 0;
}

void _ShotStopShieldType(RgWeapon *weapon)
{
    if (weapon == 0) {
        assert_prog(weaponAssertWeaponNotNil, weaponSourcePath, 1033);
    }
}

void _PassTimeShieldType(RgWeapon *weapon, float dt)
{
}

void _DestructShieldType(RgWeapon *weapon)
{
}

void _InitWeaponShield(RgWeapon *weapon, RgWeaponShieldEssence *essence,
                       RgWeaponCreateInfo *info)
{
    _InitWeaponCommon(weapon, &essence->common, info);
    weapon->lockedOn = 0;
    weapon->targeting = 0;
    weapon->shotEssence = 0;
    weapon->passTime = _PassTimeShieldType;
    weapon->dispose = _DestructShieldType;
    weapon->shoot = _ShotShieldType;
    weapon->shotStop = _ShotStopShieldType;
}

RgWeapon *_CreateWeaponShieldType(void *pEss, RgWeaponCreateInfo *pInfo)
{
    RgWeapon *pWpn;

    pWpn = RgHeapAlloc(InstanceOfRgHeap(), sizeof(RgWeapon), weaponSourcePath, 1068);
    if (pEss == 0) {
        assert_prog(weaponAssertEssenceNotNil, weaponSourcePath, 1071);
    }
    if (pWpn == 0) {
        assert_prog(weaponAssertWeaponNotNil, weaponSourcePath, 1072);
    }
    if (pInfo == 0) {
        assert_prog(weaponAssertInfoNotNil, weaponSourcePath, 1073);
    }
    _InitWeaponShield(pWpn, pEss, pInfo);
    return pWpn;
}

void InitRgWeaponShieldEssence(RgWeaponShieldEssence *pEss)
{
    unsigned int i;

    if (pEss == 0) {
        assert_prog(weaponAssertEssenceNotNil, weaponSourcePath, 1084);
    }
    _InitEssenceCommon(&pEss->common);
    pEss->common.m_pCreateMethod = _CreateWeaponShieldType;
    for (i = 0; i < RG_ACTOR_CHAR_ROBNUM; i++) {
        pEss->common.shotMotionTable[i][RG_EQUIP_TYPE_LEFT] = RG_WEAPON_MOTION_ATTACK_LEFT;
        pEss->common.shotMotionTable[i][RG_EQUIP_TYPE_RIGHT] = RG_WEAPON_MOTION_ATTACK_RIGHT;
        pEss->common.shotMotionTable[i][RG_EQUIP_TYPE_BACK] = -1;
    }
    pEss->common.controlFlags = RG_WEAPON_CONTROL_KIND_SHIELD;
}

int _EnergyIsBusy(RgWeapon *pWeapon)
{
    RgWeaponEnergyType *weapon = (RgWeaponEnergyType *) pWeapon;

    if (weapon == 0) {
        assert_prog(weaponAssertWeaponNotNil, weaponSourcePath, 1120);
    }
    if (_GetNumAttach(&weapon->common.shotAttach) != 0) {
        return 1;
    }
    return weapon->active != 0;
}

void _EnergyFree(RgWeapon *pWeapon)
{
    RgWeaponEnergyType *weapon = (RgWeaponEnergyType *) pWeapon;

    if (weapon == 0) {
        assert_prog(weaponAssertWeaponNotNil, weaponSourcePath, 1138);
    }
    weapon->active = 0;
    weapon->energyCharge = 0;
}

/*
 * The energy type's shoot method: records the request for _EnergyPassTime
 * and reports whether a new shot can start (no attached shot and not
 * already active).
 */
int _EnergyShot(RgWeapon *pWeapon)
{
    RgWeaponEnergyType *weapon = (RgWeaponEnergyType *) pWeapon;

    weapon->shotPending = 1;
    if (_GetNumAttach(&weapon->common.shotAttach) != 0) {
        return 0;
    }
    return weapon->active == 0;
}

/*
 * Advances the energy weapon one frame. A pending request that can fire starts a
 * shot. While shooting, the shot count drains and the charge rises, becoming active
 * at the limit. Otherwise the charge cools down.
 */
void _EnergyPassTime(RgWeapon *pWeapon, float dt)
{
    RgWeaponEnergyType *weapon = (RgWeaponEnergyType *) pWeapon;
    RgShotRequest info;
    RgShot *shot;
    int startShot;

    startShot = 1;
    if (weapon->active != 0) {
        _ReleaseAttach(&weapon->common.shotAttach);
    } else if (_GetNumAttach(&weapon->common.shotAttach) != 0) {
        if (weapon->shotPending != 0) {
            startShot = 0;
        } else {
            _ReleaseAttach(&weapon->common.shotAttach);
        }
    } else if (weapon->shotPending != 0 && weapon->common.shotRequested != 0) {
        _GetShotInfo(&weapon->common, &info);
        shot = CreateRgShotFromEssence(weapon->common.shotEssence, &info);
        RgShotSetSoundDriver(shot, weapon->common.sound);
        RgShotSetHitSound(shot, weapon->common.robotHitSound, weapon->common.robotHitVolume);
        RgShotSetBgHitSound(shot, weapon->common.bgHitSound, weapon->common.bgHitVolume);
        RgShotSetNoLifeSound(shot, weapon->common.lifeSound, weapon->common.lifeVolume);
        RgShotSetHideSound(shot, weapon->common.shotSound);
        XrgSoundRingVol(weapon->common.sound, weapon->common.shotSound, weapon->common.shotVolume);
        if (shot != 0 && RgShotIsReleased(shot) == 0) {
            _AddShotAttach(&weapon->common.shotAttach, shot);
        }
        startShot = 0;
    }

    if (startShot == 0) {
        float newCharge;

        if (weapon->common.shotNum != RG_WEAPON_SHOT_INFINITE) {
            weapon->common.shotNum = weapon->common.shotNum - weapon->shotEnergy * dt;
        }
        newCharge = weapon->energyCharge + weapon->upTemp * dt;
        weapon->energyCharge = newCharge;
        if (weapon->limitTemp < newCharge) {
            weapon->active = 1;
        }
    } else {
        float newCharge;

        newCharge = weapon->energyCharge - weapon->downTemp * dt;
        weapon->energyCharge = newCharge;
        if (newCharge < 0.0f) {
            weapon->energyCharge = 0.0f;
            weapon->active = 0;
        }
    }
    weapon->shotPending = 0;
}

void _InitEnergyType(RgWeaponEnergyType *weapon, RgWeaponEnergyEssence *essence,
                     RgWeaponCreateInfo *info)
{
    RgWeapon *common;

    _InitWeaponCommon(&weapon->common, &essence->common, info);
    weapon->energyCharge = 0;
    common = &weapon->common;
    common->shoot = _EnergyShot;
    common->passTime = _EnergyPassTime;
    common->isBusy = _EnergyIsBusy;
    common->setFree = _EnergyFree;
    weapon->shotEnergy = essence->shotEnergy;
    weapon->upTemp = essence->upTemp;
    weapon->limitTemp = essence->limitTemp;
    *(void **) &weapon->active = 0;
    weapon->downTemp = essence->downTemp;
    *(void **) &weapon->shotPending = 0;
    _InitWeaponCommonEffector(common, (RgStatus *) info->robot, info->equipType);
}

RgWeapon *_CreateWeaponEnergyType(void *pEss, RgWeaponCreateInfo *pInfo)
{
    RgWeaponEnergyType *pWpn;

    pWpn = RgHeapAlloc(InstanceOfRgHeap(), sizeof(RgWeaponEnergyType), weaponSourcePath, 1302);
    if (pEss == 0) {
        assert_prog(weaponAssertEssenceNotNil, weaponSourcePath, 1305);
    }
    if (pWpn == 0) {
        assert_prog(weaponAssertWeaponNotNil, weaponSourcePath, 1306);
    }
    if (pInfo == 0) {
        assert_prog(weaponAssertInfoNotNil, weaponSourcePath, 1307);
    }
    _InitEnergyType(pWpn, pEss, pInfo);
    return &pWpn->common;
}

void InitRgWeaponEnergyEssence(RgWeaponEnergyEssence *pEss)
{
    if (pEss == 0) {
        assert_prog(weaponAssertEssenceNotNil, weaponSourcePath, 1318);
    }
    _InitEssenceCommon(&pEss->common);
    pEss->common.m_pCreateMethod = _CreateWeaponEnergyType;
    pEss->common.capacity = RG_WEAPON_ENERGY_DEFAULT_CAPACITY;
    pEss->common.controlFlags = RG_WEAPON_CONTROL_LOCKON;
    pEss->shotEnergy = 1.0f;
    pEss->upTemp = RG_WEAPON_ENERGY_DEFAULT_UP_TEMP;
    pEss->limitTemp = RG_WEAPON_ENERGY_DEFAULT_LIMIT_TEMP;
    pEss->downTemp = RG_WEAPON_ENERGY_DEFAULT_DOWN_TEMP;
}

RgWeapon *CreateRgWeaponFromEssence(RgWeaponEssence *pEss, RgWeaponCreateInfo *pInfo)
{
    if (pEss == 0 || pInfo == 0) {
        assert_prog(weaponAssertEssenceAndInfoNotNil, weaponSourcePath, 1340);
    }
    if (pEss->m_pCreateMethod == 0) {
        assert_prog(weaponAssertCreateMethodNotNil, weaponSourcePath, 1341);
    }
    return pEss->m_pCreateMethod(pEss, pInfo);
}

void DisposeRgWeapon(RgWeapon *weapon)
{
    if (weapon == 0) {
        assert_prog(weaponAssertWeaponNotNil, weaponSourcePath, 1348);
    }
    if (weapon->dispose != 0) {
        weapon->dispose(weapon);
    }
    _DestructWeaponCommon(weapon);
    RgHeapFree(InstanceOfRgHeap(), weapon, weaponSourcePath, 1352);
}

/* Disposes a weapon like DisposeRgWeapon, but returns its actor instead of disposing it. */
XrgActor *DisposeRgWeaponNotActor(RgWeapon *weapon)
{
    XrgActor *actor;

    if (weapon == 0) {
        assert_prog(weaponAssertWeaponNotNil, weaponSourcePath, 0x551);
    }
    if (weapon->dispose != 0) {
        weapon->dispose(weapon);
    }
    actor = weapon->actor;
    weapon->actor = 0;
    _DestructWeaponCommon(weapon);
    RgHeapFree(InstanceOfRgHeap(), weapon, weaponSourcePath, 0x557);
    return actor;
}

void RgWeaponSetSound(RgWeapon *weapon, XrgSound *sound)
{
    if (weapon == 0) {
        assert_prog(weaponAssertWeaponNotNil, weaponSourcePath, 0x585);
    }
    weapon->sound = sound;
}

int RgWeaponIsLockedOn(RgWeapon *weapon)
{
    if (weapon == 0) {
        assert_prog(weaponAssertWeaponNotNil, weaponSourcePath, 0x58F);
    }
    return weapon->lockedOn;
}

int RgWeaponIsTargetting(RgWeapon *weapon)
{
    if (weapon == 0) {
        assert_prog(weaponAssertWeaponNotNil, weaponSourcePath, 0x597);
    }
    return weapon->targeting;
}

float RgWeaponGetShotNum(RgWeapon *weapon)
{
    if (weapon == 0) {
        assert_prog(weaponAssertWeaponNotNil, weaponSourcePath, 0x59F);
    }
    return weapon->shotNum;
}

int RgWeaponIsNotBusy(RgWeapon *weapon)
{
    if (weapon == 0) {
        assert_prog(weaponAssertWeaponNotNil, weaponSourcePath, 0x5A8);
    }
    if (weapon->isBusy != 0) {
        return weapon->isBusy(weapon) == 0;
    }
    return 1;
}

/*
 * Returns the unit of the weapon's rest gauge: none for infinite or attack weapons,
 * a count for shot weapons, a percentage otherwise. A weapon without an essence
 * warns and returns none.
 */
int RgWeaponGetRestUnit(RgWeapon *weapon)
{
    RgWeaponEssence *ess;
    int result;

    if (weapon == 0) {
        assert_prog(weaponAssertWeaponPointerNotNil, weaponSourcePath, 1461);
    }
    ess = RgWeaponGetEss(weapon);
    result = RG_WEAPON_REST_UNIT_NONE;
    if (ess == 0) {
        RgWarn(weaponNoEssenceWarnFormat, weaponSourcePath, 1466, weapon, weapon->shotMotionOffset);
        return RG_WEAPON_REST_UNIT_NONE;
    }
    if (ess->capacity == RG_WEAPON_SHOT_INFINITE) {
        return result;
    }
    if (RgWeaponEssCastToShot(ess) != 0) {
        return RG_WEAPON_REST_UNIT_COUNT;
    }
    if (RgWeaponEssCastToAttack(ess) != 0) {
        result = RG_WEAPON_REST_UNIT_NONE;
    } else {
        result = RG_WEAPON_REST_UNIT_PERCENT;
    }
    return result;
}

int RgWeaponRobotShotMotion(RgWeapon *weapon, int eCharID)
{
    if (!(0 <= eCharID && eCharID < RG_ACTOR_CHAR_ROBNUM)) {
        assert_prog(weaponAssertCharIdInRobotRange, weaponSourcePath, 0x5D2);
    }
    if (weapon == 0) {
        assert_prog(weaponAssertWeaponNotNil, weaponSourcePath, 0x5D4);
    }
    return weapon->shotMotionTable[eCharID][weapon->shotMotionOffset];
}

XrgActor *RgWeaponGetActor(RgWeapon *weapon)
{
    if (weapon == 0) {
        assert_prog(weaponAssertWeaponNotNil, weaponSourcePath, 0x5DC);
    }
    return weapon->actor;
}

RgEquip *RgWeaponGetEquip(RgWeapon *weapon)
{
    if (weapon == 0) {
        assert_prog(weaponAssertWeaponNotNil, weaponSourcePath, 0x5E4);
    }
    return &weapon->equip;
}

float RgWeaponGetWeight(RgWeapon *weapon)
{
    if (weapon == 0) {
        assert_prog(weaponAssertWeaponNotNil, weaponSourcePath, 0x5EC);
    }
    return weapon->weight;
}

int RgWeaponIsAttachedShot(RgWeapon *weapon)
{
    if (weapon == 0) {
        assert_prog(weaponAssertWeaponNotNil, weaponSourcePath, 1525);
    }
    return _GetNumAttach(&weapon->shotAttach) != 0;
}

int RgWeaponGetControlFlag(RgWeapon *weapon)
{
    if (weapon == 0) {
        assert_prog(weaponAssertWeaponNotNil, weaponSourcePath, 0x5FE);
    }
    return weapon->controlFlags;
}

RgWeaponEssence *RgWeaponGetEss(RgWeapon *weapon)
{
    if (weapon == 0) {
        assert_prog(weaponAssertWeaponNotNil, weaponSourcePath, 0x606);
    }
    return weapon->ess;
}

unsigned int RgWeaponGetAttachedShot(RgWeapon *weapon, RgShot **shots)
{
    if (weapon == 0) {
        assert_prog(weaponAssertWeaponNotNil, weaponSourcePath, 0x624);
    }
    if (shots == 0) {
        return _GetNumAttach(&weapon->shotAttach);
    }
    return _GetAllAttach(&weapon->shotAttach, shots);
}

int RgWeaponIsActiveLockOnSys(RgWeapon *weapon)
{
    if (weapon == 0) {
        assert_prog(weaponAssertWeaponNotNil, weaponSourcePath, 0x62F);
    }
    return weapon->lockOnEffector != 0;
}

void RgWeaponPlayMotion(RgWeapon *weapon, int motion)
{
    XrgActor *actor;

    if (weapon == 0) {
        assert_prog(weaponAssertWeaponNotNil, weaponSourcePath, 0x63E);
    }
    actor = weapon->actor;
    if (actor != 0) {
        XrgActorSetMotion(actor, motion);
    }
}

void RgWeaponPlayShotMotion(RgWeapon *weapon)
{
    if (weapon == 0) {
        assert_prog(weaponAssertWeaponNotNil, weaponSourcePath, 0x649);
    }
    _PlayShotMotion(weapon);
}

/*
 * Requests a shot. Returns the shoot callback's result, or 0 when no shots are left
 * or there is no callback.
 */
int RgWeaponShot(RgWeapon *weapon)
{
    if (weapon == 0) {
        assert_prog(weaponAssertWeaponNotNil, weaponSourcePath, 0x651);
    }
    weapon->shotRequested = 1;
    if (weapon->shotNum > 0.0f) {
        if (weapon->shoot != 0) {
            return weapon->shoot(weapon);
        }
    }
    return 0;
}

void RgWeaponSetup(RgWeapon *weapon)
{
    if (weapon == 0) {
        assert_prog(weaponAssertWeaponNotNil, weaponSourcePath, 0x660);
    }
    weapon->equipped = 1;
}

void RgWeaponSetdown(RgWeapon *weapon)
{
    if (weapon == 0) {
        assert_prog(weaponAssertWeaponNotNil, weaponSourcePath, 0x669);
    }
    weapon->equipped = 0;
}

void RgWeaponStopLockon(RgWeapon *weapon)
{
    if (weapon == 0) {
        assert_prog(weaponAssertWeaponNotNil, weaponSourcePath, 0x672);
    }
    weapon->lockOnStopped = 1;
}

void RgWeaponRestartLockon(RgWeapon *weapon)
{
    if (weapon == 0) {
        assert_prog(weaponAssertWeaponNotNil, weaponSourcePath, 0x67B);
    }
    weapon->lockOnStopped = 0;
}

void RgWeaponDisposeLockon(RgWeapon *weapon)
{
    if (weapon == 0) {
        assert_prog(weaponAssertWeaponNotNil, weaponSourcePath, 0x684);
    }
    if (weapon->lockOnEffector != 0) {
        XrgActorDisposeEffector(RgRobotGetActor(weapon->robot), weapon->lockOnEffector);
        weapon->lockOnEffector = 0;
    }
}

/* Stops the shot, runs the shot-stop callback, and unequips the weapon. */
void RgWeaponShotStop(RgWeapon *weapon)
{
    /* A separate load keeps the shot-stop callback in its own register. */
    void (*shotStop)(RgWeapon *weapon);

    if (weapon == 0) {
        assert_prog(weaponAssertWeaponNotNil, weaponSourcePath, 0x691);
    }
    shotStop = weapon->shotStop;
    weapon->shotRequested = 0;
    if (shotStop != 0) {
        shotStop(weapon);
    }
    weapon->equipped = 0;
}

void RgWeaponHitRobot(RgWeapon *weapon, RgStatus *robot, RgVector position)
{
    if (weapon == 0) {
        assert_prog(weaponAssertWeaponNotNil, weaponSourcePath, 0x69E);
    }
    if (weapon->hitRobot != 0) {
        weapon->hitRobot(weapon, robot, position);
    }
}

void RgWeaponHitBG(RgWeapon *weapon, RgBgObj *bgObject, RgVector position)
{
    if (weapon == 0) {
        assert_prog(weaponAssertWeaponNotNil, weaponSourcePath, 0x6A9);
    }
    if (weapon->hitBG != 0) {
        weapon->hitBG(weapon, bgObject, position);
    }
    XrgSoundRingVol(weapon->sound, weapon->bgHitSound, weapon->bgHitSound);
}

/* Subtracts delta from the shot count, clamped at zero. Infinite counts are kept. */
void RgWeaponSubShotNum(RgWeapon *weapon, float delta)
{
    float shotNum;
    float newShotNum;

    if (weapon == 0) {
        assert_prog(weaponAssertWeaponNotNil, weaponSourcePath, 0x6B5);
    }
    shotNum = weapon->shotNum;
    if (shotNum != RG_WEAPON_SHOT_INFINITE) {
        newShotNum = shotNum - delta;
        weapon->shotNum = newShotNum;
        if (newShotNum < 0.0f) {
            weapon->shotNum = 0.0f;
        }
    }
}

void RgWeaponSetFree(RgWeapon *weapon)
{
    if (weapon == 0) {
        assert_prog(weaponAssertWeaponNotNil, weaponSourcePath, 0x6C3);
    }
    if (weapon->setFree != 0) {
        weapon->setFree(weapon);
    }
}

void RgWeaponDisp(RgWeapon *weapon)
{
    if (weapon == 0) {
        assert_prog(weaponAssertWeaponNotNil, weaponSourcePath, 0x6CD);
    }
    weapon->disp(weapon);
    if (weapon->actor != 0) {
        XrgActorDraw(weapon->actor);
    }
}

void RgWeaponPassTime(RgWeapon *weapon, float dt)
{
    if (weapon == 0) {
        assert_prog(weaponAssertWeaponNotNil, weaponSourcePath, 0x6D9);
    }
    _CommonPassTime(weapon, dt);
    if (weapon->passTime != 0) {
        weapon->passTime(weapon, dt);
    }
    if (weapon->actor != 0) {
        XrgActorPassTime(weapon->actor, dt);
    }
}

/*
 * Updates the lock-on state from the robot's target. The target's position in the
 * robot's local space gives an angle, and the weapon locks on when that angle is
 * inside its lock-on window.
 */
void _CheckLockOn(RgWeapon *weapon)
{
    RgMatrix local;
    RgMatrix invLocal;
    RgVector targetLocal;
    RgVector targetWorld;
    float angle;
    RgStatus *robot;
    unsigned int statusFlags;
    int wasLockedOn;
    int lockedOn;
    RgGeomPoint *target;

    robot = weapon->robot;
    if (weapon == 0) {
        assert_prog(weaponAssertWeaponNotNil, weaponSourcePath, 1779);
    }
    wasLockedOn = weapon->lockedOn;
    weapon->targeting = 0;
    weapon->lockedOn = 0;
    statusFlags = RgRobotGetStatusFlags(robot);
    if (robot != 0 && (statusFlags & 1) && RgRobotIsConfused(robot) == 0 &&
            !(statusFlags & 0x100) && weapon->lockOnStopped == 0) {
        RgGeomRobotCalcLocal(&RgRobotGetGeom(robot)->geom, local);
        XrgInvMatrix(invLocal, local);
        target = RgRobotGetTarget(robot);
        if (target != 0) {
            __RgGeomPointGetPos(target, targetWorld, weaponSourcePath, 1819);
            __asm__ __volatile__(
                "lqc2 $vf2, 0(%0)\n\t"
                "lqc2 $vf3, 0(%1)\n\t"
                "lqc2 $vf4, 16(%1)\n\t"
                "lqc2 $vf5, 32(%1)\n\t"
                "lqc2 $vf6, 48(%1)\n\t"
                "vmulax.xyzw ACCxyzw, vf3xyzw, vf2x\n\t"
                "vmadday.xyzw ACCxyzw, vf4xyzw, vf2y\n\t"
                "vmaddaz.xyzw ACCxyzw, vf5xyzw, vf2z\n\t"
                "vmaddw.xyzw vf2xyzw, vf6xyzw, vf2w\n\t"
                "sqc2 $vf2, 0(%2)\n\t"
                : : "r"(targetWorld), "r"(invLocal), "r"(targetLocal) : "memory");
            angle = atan2f(targetLocal[0], targetLocal[2]) + XRG_PI;
            while (angle > XRG_PI) {
                angle -= XRG_TWO_PI;
            }
            if (angle < -XRG_PI) {
                do {
                    angle += XRG_TWO_PI;
                } while (angle < -XRG_PI);
            }
            /* Lock on when the angle lies inside the weapon's lock-on window. */
            lockedOn = 0;
            if (weapon->lockOnAngleMin <= angle && angle <= weapon->lockOnAngleMax) {
                lockedOn = 1;
            }
            weapon->targeting = lockedOn;
            if (weapon->controlFlags & RG_WEAPON_CONTROL_LOCKON) {
                weapon->lockedOn = lockedOn;
            }
            XrgCopyVector(weapon->targetPosition, targetWorld);
        }
        if (wasLockedOn == 0 && _GetNumAttach(&weapon->shotAttach) != 0 && weapon->lockedOn != 0) {
            weapon->lockedOn = 0;
        }
    }
}

int _GetParentCharID(RgWeapon *weapon)
{
    if (weapon == 0) {
        assert_prog(weaponAssertWeaponNotNil, weaponSourcePath, 0x73A);
    }
    if (weapon->robot != 0) {
        return RgRobotGetCharID(weapon->robot);
    }
    return -1;
}

void _PlayShotMotion(RgWeapon *weapon)
{
    /* A separate load keeps shotMotionOffset live across the call below. */
    int shotMotionOffset;
    int charID;
    int motion;

    if (weapon == 0) {
        assert_prog(weaponAssertWeaponNotNil, weaponSourcePath, 0x747);
    }
    shotMotionOffset = weapon->shotMotionOffset;
    charID = _GetParentCharID(weapon);
    if (charID != -1) {
        motion = weapon->shotStartMotionTable[charID][shotMotionOffset];
        if (motion != -1) {
            if (weapon->actor != 0) {
                XrgActorSetMotion(weapon->actor, motion);
            }
        }
    }
}

int _GetDefShotPosDir(void *posOwner, RgVector position, RgVector direction)
{
    RgWeapon *pWpn = posOwner;

    if (pWpn == 0) {
        assert_prog(weaponAssertWeaponNotNil, weaponSourcePath, 0x756);
    }
    if (pWpn->shotRequested == 0) {
        return 0;
    }
    _GetDefShotPosDirFunc(pWpn, position, direction);
    return 1;
}

/*
 * Computes a shot's position and direction from the weapon's joint. The direction
 * runs along the aim axis, toward the locked target, or away from the robot. The
 * pitch and yaw offsets then rotate an unlocked aim.
 */
void _GetDefShotPosDirFunc(RgWeapon *weapon, RgVector position, RgVector direction)
{
    /*
     * jointLocal is the actor joint's local transform (or a copy of robotLocal
     * when no joint is followed); its last row is the joint's position.
     * robotLocal is the robot geometry's local transform.
     */
    RgMatrix jointLocal;
    RgMatrix robotLocal;
    RgVector aimVector;
    RgWeaponEssence *ess;
    float length;
    int joint;
    int found;
    RgStatus *robot;

    robot = weapon->robot;
    ess = weapon->ess;
    found = 0;
    if (robot == 0) {
        assert_prog(weaponAssertRobotNotNil, weaponSourcePath, 1892);
    }
    RgGeomRobotCalcLocal(&RgRobotGetGeom(robot)->geom, robotLocal);
    /* Without an actor the joint is the robot itself. Otherwise the next aim joint in the cycle is used, or joint 8. */
    if (weapon->actor == 0) {
        XrgCopyMatrix(jointLocal, robotLocal);
    } else {
        if (ess->shotPntNum != 0) {
            unsigned int cursor;
            unsigned int count;
            do {
                cursor = weapon->aimJointCursor;
                weapon->aimJointCursor = cursor + 1;
                joint = ess->shotPnt[cursor];
            } while (0);
            count = ess->shotPntNum;
            if (cursor + 1 >= count) {
                weapon->aimJointCursor = 0;
            }
        } else {
            joint = 8;
        }
        found = XrgActorGetJointLocal(weapon->actor, joint, jointLocal);
        if (found == 0) {
            XrgCopyMatrix(jointLocal, robotLocal);
        }
    }
    XrgCopyVector(position, &jointLocal[12]);
    if (ess->aimAxis != RG_WEAPON_AIM_AXIS_NONE && found != 0) {
        XrgAddVector(direction, position, &jointLocal[ess->aimAxis * 4]);
    } else if (weapon->lockedOn != 0) {
        XrgCopyVector(direction, weapon->targetPosition);
    } else {
        XrgNegateVector(direction, &robotLocal[8]);
        XrgAddVector(direction, direction, position);
    }
    if (weapon->lockedOn != 0) {
        return;
    }
    if (ess->shotPitch == 0.0f && ess->shotYaw == 0.0f) {
        return;
    }
    /* Pitch and yaw rotate the aim about the joint, keeping its length. */
    XrgSubVector(aimVector, direction, position);
    length = XrgLengthVector(aimVector);
    XrgCalcMatrixZtoY(jointLocal, aimVector, XrgVectorY());
    XrgRotMatrixY(jointLocal, jointLocal, ess->shotYaw);
    XrgRotMatrixX(jointLocal, jointLocal, ess->shotPitch);
    XrgScaleVector(aimVector, &jointLocal[8], length);
    XrgAddVector(direction, position, aimVector);
}

void _GetShotInfo(RgWeapon *weapon, RgShotRequest *info)
{
    /* A separate load keeps the owning robot in its own register. It is read
     * once and passed to both RgRobotGetTarget and RgRobotIsConfused. */
    RgStatus *robot;

    robot = weapon->robot;
    if (robot == 0) {
        assert_prog(weaponAssertRobotNotNil, weaponSourcePath, 0x7A5);
    }
    info->target = RgRobotGetTarget(robot);
    info->shotGroup = (struct RgGeomGroup *) weapon->id;
    info->posOwner = weapon;
    info->getPos = _GetDefShotPosDir;
    info->targetConfused = RgRobotIsConfused(robot);
}

RgWeaponShotEssence *RgWeaponEssCastToShot(RgWeaponEssence *ess)
{
    if (ess != 0 && ess->m_pCreateMethod == _CreateWeaponShotType) {
        return (RgWeaponShotEssence *) ess;
    }
    return 0;
}

RgWeaponEnergyEssence *RgWeaponEssCastToEnergy(RgWeaponEssence *ess)
{
    if (ess != 0 && ess->m_pCreateMethod == _CreateWeaponEnergyType) {
        return (RgWeaponEnergyEssence *) ess;
    }
    return 0;
}

RgWeaponAttackEssence *RgWeaponEssCastToAttack(RgWeaponEssence *ess)
{
    if (ess != 0) {
        if (ess->m_pCreateMethod == _CreateWeaponAttackType) {
            return (RgWeaponAttackEssence *) ess;
        }
        if (ess->m_pCreateMethod == _CreateWeaponUnArmedType) {
            return (RgWeaponAttackEssence *) ess;
        }
    }
    return 0;
}

RgWeaponShieldEssence *RgWeaponEssCastToShield(RgWeaponEssence *ess)
{
    if (ess != 0 && ess->m_pCreateMethod == _CreateWeaponShieldType) {
        return (RgWeaponShieldEssence *) ess;
    }
    return 0;
}

RgWeaponUnArmedEssence *RgWeaponEssCastToUnArmed(RgWeaponEssence *ess)
{
    if (ess != 0 && ess->m_pCreateMethod == _CreateWeaponUnArmedType) {
        return (RgWeaponUnArmedEssence *) ess;
    }
    return 0;
}



const char weaponAssertAttachNumInRange[40] = "pAttach->m_uAttachNum < ATTACH_MAX";

const char weaponAssertEquipTypeInRange[72] = "RG_EQUIP_TYPE_MIN <= (eEquipType) && (eEquipType) < RG_EQUIP_TYPE_NUM";

const char weaponAssertShotTypePNotNil[16] = "p != NIL";

const char weaponAssertRobotNotNilEffector[16] = "pRob != NIL";

const char weaponAssertAttackGeomNotNil[24] = "pWpn->m_pAtkGeom != NIL";

const char weaponAssertWeaponPointerNotNil[16] = "pWeapon != NIL";

const char weaponNoEssenceWarnFormat[32] = "weapon not have essence %p(%d)";
