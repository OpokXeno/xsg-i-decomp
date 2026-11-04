/*
 * OV12 original TU 26: 0x00a19388..0x00a1c7d8 (90 functions)
 */
#include "common.h"
#include "shared.h"
#include "rg_weapon.h"
#include "ov12/rg_matrices_effector.h"

extern void assert_prog(const char *expression, const char *source_file,
                        int line);

const char D_00A53648[16] = "pAttach != NIL";

const char D_00A53658[24] = "../rg_weapon.euc.c";

const char D_00A536F0[16] = "pWpn != NIL";

const char D_00A53710[16] = "pEss != NIL";

const char D_00A53720[16] = "pInfo != NIL";

const char D_00A53730[16] = "eff15.ptcl";

const char D_00A53740[16] = "eff11.ptcl";

const char D_00A53768[32] = "pEss != NIL && pInfo != NIL";

const char D_00A53788[32] = "pEss->m_pCreateMethod != NIL";

const char D_00A537D8[48] = "0 <= eCharID && eCharID < RG_ACTOR_CHAR_ROBNUM";

const char D_00A53808[16] = "pRobot != NIL";

/*
 * Scaffold-owned (.rodata still owner: asm, config/tu-build.json
 * data_ownership window 0x00a53642..0x00a53818): the assert_prog expression
 * and source-file strings this allocation's assertions cite (docs/naming.md,
 * "Scaffold-owned data keeps its splat name"). ov12:0x00a536f0 holds
 * "pWpn != NIL", ov12:0x00a53658 holds "../rg_weapon.euc.c", ov12:0x00a537d8
 * holds "0 <= eCharID && eCharID < RG_ACTOR_CHAR_ROBNUM".
 */
extern const char D_00A536F0[];
extern const char D_00A53658[];
extern const char D_00A537D8[];
/*
 * Scaffold-owned (.rodata still owner: asm, config/tu-build.json
 * data_ownership window 0x00a53642..0x00a53818): the assert_prog expression
 * strings the ov12:0x00a1a6e8..0x00a1b52c allocation's create-function
 * assertions cite. ov12:0x00a53710 holds "pEss != NIL", ov12:0x00a53720
 * holds "pInfo != NIL".
 */
extern const char D_00A53710[];
extern const char D_00A53720[];
/*
 * Scaffold-owned (.rodata still owner: asm, config/tu-build.json
 * data_ownership window 0x00a53642..0x00a53818): the assert_prog
 * expression string _GetShotInfo's assertion cites. ov12:0x00a53808 holds
 * "pRobot != NIL".
 */
extern const char D_00A53808[];
extern RgHeap *InstanceOfRgHeap(void);
extern void *RgHeapAlloc(RgHeap *heap, unsigned int size,
                         const char *source_file, int line);
extern void RgHeapFree(RgHeap *heap, void *ptr, const char *source_file,
                       int line);
extern void _DestructWeaponCommon(RgWeapon *weapon);
extern void _PlayShotMotion(RgWeapon *weapon);
extern void XrgActorSetMotion(XrgActor *actor, int motion);
extern XrgActor *RgRobotGetActor(int robotId);
extern void XrgActorDisposeEffector(XrgActor *actor, int effector);
extern int RgBgObjTryToBreak(int bgObject, float damage);
extern void _InitWeaponAttack(RgWeaponAttackType *weapon,
                               RgWeaponAttackEssence *essence,
                               RgWeaponCreateInfo *info);
void _InitWeaponUnArmedType(RgWeapon *weapon, RgWeaponUnArmedEssence *essence, RgWeaponCreateInfo *info);
void _InitWeaponShield(RgWeapon *weapon, RgWeaponShieldEssence *essence, RgWeaponCreateInfo *info);
extern void _InitEnergyType(RgWeaponEnergyType *weapon,
                             RgWeaponEnergyEssence *essence,
                             RgWeaponCreateInfo *info);
void _InitEssenceCommon(void *essence);
static int _GetNumAttach(RgWeaponAttach *pAttach);
extern void _CommonPassTime(RgWeapon *weapon, float dt);
extern int _GetDefShotPosDir(RgWeapon *pWpn, RgVector position, RgVector direction);
extern int RgRobotGetCharID(int robotId);
extern void *RgRobotGetTarget(int robotId);
extern int RgRobotIsConfused(int robotId);
extern void XrgSoundRingVol(int driver, int soundId, int volume);
extern void XrgActorPassTime(XrgActor *actor, float dt);
/*
 * Scaffold-owned (.rodata still owner: asm, config/tu-build.json
 * data_ownership window 0x00a53642..0x00a53818): the assert_prog expression
 * string _InitAttach/_GetNumAttach's assertions cite. ov12:0x00a53648 holds
 * "pAttach != NIL".
 */
extern const char D_00A53648[];

static void _InitAttach(RgWeaponAttach *pAttach)
{
    if (pAttach == 0) {
        assert_prog(D_00A53648, D_00A53658, 38);
    }
    pAttach->m_uAttachNum = 0;
}

extern void RgShotRelease(RgShot *shot);

static void _ReleaseAttach(RgWeaponAttach *pAttach)
{
    unsigned int i;

    if (pAttach == 0) {
        assert_prog(D_00A53648, D_00A53658, 46);
    }
    for (i = 0; i < pAttach->m_uAttachNum; i++) {
        RgShotRelease(pAttach->shots[i]);
    }
}

static int _GetNumAttach(RgWeaponAttach *pAttach)
{
    if (pAttach == 0) {
        assert_prog(D_00A53648, D_00A53658, 54);
    }
    return pAttach->m_uAttachNum;
}

static unsigned int _GetAllAttach(RgWeaponAttach *pAttach, RgShot **shots)
{
    unsigned int i;

    if (pAttach == 0) {
        assert_prog(D_00A53648, D_00A53658, 62);
    }
    for (i = 0; i < pAttach->m_uAttachNum; i++) {
        shots[i] = pAttach->shots[i];
    }
    return pAttach->m_uAttachNum;
}

/*
 * Scaffold-owned (.rodata still owner: asm, config/tu-build.json
 * data_ownership window 0x00a53642..0x00a53818): the assert_prog expression
 * string _AddShotAttach's bound check cites. ov12:0x00a53670 holds
 * "pAttach->m_uAttachNum < ATTACH_MAX".
 */
extern const char D_00A53670[];

static void _AddShotAttach(RgWeaponAttach *pAttach, RgShot *shot)
{
    if (pAttach == 0) {
        assert_prog(D_00A53648, D_00A53658, 71);
    }
    if (shot == 0) {
        return;
    }
    if (pAttach->m_uAttachNum >= ATTACH_MAX) {
        assert_prog(D_00A53670, D_00A53658, 73);
    }
    pAttach->shots[pAttach->m_uAttachNum++] = shot;
}

extern int RgShotIsReleased(RgShot *shot);

static void _CheckAttach(RgWeaponAttach *pAttach)
{
    RgShot *alive[ATTACH_MAX];
    RgShot *shot;
    unsigned int i;
    unsigned int aliveNum;

    if (pAttach == 0) {
        assert_prog(D_00A53648, D_00A53658, 83);
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

void _CommonDispose(void)
{
}

extern int XrgActorIsEndOfMotion(XrgActor *pActor);

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

/* Opaque here: rg_matrices_effector.c (ov12/tu068) owns MatrixConstraint's definition. */
typedef struct MatrixConstraint MatrixConstraint;

extern void RgMatricesEffectorSetActivity(MatrixEffector *effector, int activity);
extern void RgMatEffConstraintSetTarget(MatrixConstraint *constraint, RgVector target);
extern void XrgClearVector(RgVector vector);
extern void XrgAddVector(RgVector destination, RgVector first, RgVector second);
extern void XrgNormalizeVector(RgVector destination, RgVector source);
extern void XrgScaleVector(RgVector destination, RgVector source, float scale);
extern void XrgSubVector(RgVector destination, RgVector first, RgVector second);

/* Opaque here: rg_geom_point.c (ov12/tu052) owns the RgPointVector definition. */
typedef struct RgPointVector RgPointVector;

extern void __RgGeomPointGetPos(RgGeomPoint *point, RgPointVector *destination,
                                const char *source_file, int source_line);
void _GetDefShotPosDirFunc(RgWeapon *weapon, RgVector position, RgVector direction);
void _CheckLockOn(RgWeapon *weapon);

void _CommonDisp(RgWeapon *weapon)
{
    RgVector position;
    RgVector fromPosition;
    RgVector toPosition;
    RgVector aimPoint;
    void *target;
    int robotId;

    robotId = weapon->robotId;
    _CheckLockOn(weapon);
    if (weapon->lockedOn != 0) {
        target = RgRobotGetTarget(robotId);
        if (target != 0) {
            __RgGeomPointGetPos(target, (RgPointVector *) position, D_00A53658, 287);
        } else {
            XrgClearVector(position);
        }
        if (weapon->lockOnEffector != 0) {
            RgMatricesEffectorSetActivity((MatrixEffector *) weapon->lockOnEffector, 1);
            RgMatEffConstraintSetTarget((MatrixConstraint *) weapon->lockOnEffector, position);
        }
    } else if (weapon->lockOnEffector != 0) {
        if (weapon->equipped != 0 && (weapon->controlFlags & 0x20)) {
            _GetDefShotPosDirFunc(weapon, fromPosition, toPosition);
            XrgSubVector(aimPoint, toPosition, fromPosition);
            XrgNormalizeVector(aimPoint, aimPoint);
            XrgScaleVector(aimPoint, aimPoint, 100.0f);
            XrgAddVector(aimPoint, fromPosition, aimPoint);
            RgMatricesEffectorSetActivity((MatrixEffector *) weapon->lockOnEffector, 1);
            RgMatEffConstraintSetTarget((MatrixConstraint *) weapon->lockOnEffector, aimPoint);
        } else {
            RgMatricesEffectorSetActivity((MatrixEffector *) weapon->lockOnEffector, 0);
        }
    }
}

#include "ov12/rg_weapon_db.h"

/*
 * Scaffold-owned (.rodata still owner: asm, config/tu-build.json
 * data_ownership window 0x00a53642..0x00a53818): the assert_prog expression
 * string _InitWeaponCommon's equip-type check cites. ov12:0x00a53698 holds
 * "RG_EQUIP_TYPE_MIN <= (eEquipType) && (eEquipType) < RG_EQUIP_TYPE_NUM".
 */
extern const char D_00A53698[];
extern void CopyRgEquip(RgEquip *destination, RgEquip *source);
extern XrgActor *CreateXrgActor(int actorID);
void _InitWeaponCommon(RgWeapon *weapon, void *essence, RgWeaponCreateInfo *info);

void _InitWeaponCommon(RgWeapon *weapon, void *essence, RgWeaponCreateInfo *info)
{
    RgWeaponBaseEssence *base;
    int i;
    int j;
    unsigned int equipType;
    enum RgActorFlag actorFlag;
    float angle;

    equipType = info->equipType;
    if (equipType >= RG_EQUIP_TYPE_NUM) {
        assert_prog(D_00A53698, D_00A53658, 329);
    }
    base = (RgWeaponBaseEssence *) essence;
    weapon->id = info->id;
    weapon->shotNum = base->capacity;
    weapon->shotRequested = 0;
    weapon->lockedOn = 0;
    weapon->targeting = 0;
    weapon->controlFlags = base->controlMode;
    weapon->robotId = info->robotId;
    weapon->shotEssence = base->shotEssence;
    weapon->atkGeoms = info->atkGeoms;
    weapon->dispose = _CommonDispose;
    weapon->disp = _CommonDisp;
    weapon->weight = base->weight;
    weapon->ess = (int) base;
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
        weapon->actor = CreateXrgActor((int) base->actorSpec[equipType]);
    } else {
        weapon->actor = 0;
    }
    _InitAttach(&weapon->shotAttach);
    weapon->aimJointCursor = 0;
    CopyRgEquip(&weapon->equip, &base->equip);
    weapon->equip.equipExtra = base->equip.equipExtra;
    weapon->lockOnEffector = 0;
    for (i = 0; i < 3; i++) {
        for (j = 0; j < RG_ACTOR_CHAR_ROBNUM; j++) {
            weapon->shotMotionTable[j][i] = base->shotMotionTable[j][i];
            weapon->shotStartMotionTable[j][i] = base->shotStartMotionTable[j][i];
            weapon->shotEndMotionTable[j][i] = base->shotEndMotionTable[j][i];
        }
    }
    if (info->equipType == 0) {
        angle = base->angleMin;
        while (angle > 3.1415927f) {
            angle -= 6.2831855f;
        }
        while (angle < -3.1415927f) {
            angle += 6.2831855f;
        }
        weapon->lockOnAngleMin = angle;
        angle = base->angleMax;
        while (angle > 3.1415927f) {
            angle -= 6.2831855f;
        }
        while (angle < -3.1415927f) {
            angle += 6.2831855f;
        }
        weapon->lockOnAngleMax = angle;
    } else if (info->equipType == 1) {
        angle = -base->angleMax;
        while (angle > 3.1415927f) {
            angle -= 6.2831855f;
        }
        while (angle < -3.1415927f) {
            angle += 6.2831855f;
        }
        weapon->lockOnAngleMin = angle;
        angle = -base->angleMin;
        while (angle > 3.1415927f) {
            angle -= 6.2831855f;
        }
        while (angle < -3.1415927f) {
            angle += 6.2831855f;
        }
        weapon->lockOnAngleMax = angle;
    } else {
        angle = base->angleMin;
        while (angle > 3.1415927f) {
            angle -= 6.2831855f;
        }
        while (angle < -3.1415927f) {
            angle += 6.2831855f;
        }
        weapon->lockOnAngleMin = angle;
        angle = base->angleMax;
        while (angle > 3.1415927f) {
            angle -= 6.2831855f;
        }
        while (angle < -3.1415927f) {
            angle += 6.2831855f;
        }
        weapon->lockOnAngleMax = angle;
    }
    weapon->robotHitSound = base->robotHitSound;
    weapon->equipped = 0;
    weapon->lockOnStopped = 0;
    weapon->bgHitSound = base->bgHitSound;
    weapon->sound = 0;
    weapon->bgHitVolume = base->bgHitVolume;
    weapon->shotSound = base->shotSound;
    weapon->robotHitVolume = base->robotHitVolume;
    weapon->extraVolume0 = base->extraVolume0;
    weapon->extraVolume1 = base->extraVolume1;
    weapon->shotVolume = base->shotVolume;
}

extern void DisposeXrgActor(XrgActor *actor);

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
    if (weapon->robotId == 0) {
        return;
    }
    XrgActorDisposeEffector(RgRobotGetActor(weapon->robotId), weapon->lockOnEffector);
}

extern void InitRgEquip(void *pEquip);
extern int InitXrgActorEssence(void *pEss, int actorID);

void _InitEssenceCommon(void *essence)
{
    RgWeaponBaseEssence *ess;
    unsigned int i;
    unsigned int j;
    float weight = 1.0f;

    ess = (RgWeaponBaseEssence *) essence;
    ess->aimAxis = 3;
    ess->angleMin = -0.3926991f;
    ess->angleMax = 1.1780972f;
    ess->capacity = 50.0f;
    ess->m_pCreateMethod = 0;
    ess->controlMode = 0;
    ess->shotEssence = 0;
    ess->shotPitch = 0.0f;
    ess->shotYaw = 0.0f;
    ess->aimJointCount = 0;
    ess->unmodeled68 = 0;
    InitRgEquip(&ess->equip);
    ess->equip.equipExtra = -1;
    for (i = 0; i < RG_ACTOR_CHAR_ROBNUM; i++) {
        ess->shotMotionTable[i][0] = 39;
        ess->shotMotionTable[i][1] = 40;
        ess->shotMotionTable[i][2] = -1;
        for (j = 0; j < 3; j++) {
            ess->shotStartMotionTable[i][j] = -1;
            ess->shotEndMotionTable[i][j] = -1;
        }
    }
    ess->actorFlag = 1;
    InitXrgActorEssence(ess->actorSpec[0], -1);
    InitXrgActorEssence(ess->actorSpec[1], -1);
    InitXrgActorEssence(ess->actorSpec[2], -1);
    ess->weight = weight;
    ess->shotSound = 21;
    ess->unmodeled3a4 = 0;
    ess->unmodeled350 = 0;
    ess->unmodeled330 = 0;
    ess->robotHitSound = 22;
    ess->bgHitSound = 22;
    ess->bgHitVolume = 0;
    ess->robotHitVolume = 128;
    ess->extraVolume0 = 128;
    ess->extraVolume1 = 128;
    ess->unmodeled3a0 = -1;
    ess->shotVolume = 1;
}

extern const char D_00A536E0[];
extern void _GetShotInfo(RgWeapon *weapon, RgWeaponShotRequest *info);
extern RgShot *CreateRgShotFromEssence(struct RgShotEssence *ess, RgWeaponShotRequest *info);
extern void RgShotSetSoundDriver(RgShot *shot, int sound);
extern void RgShotSetHitSound(RgShot *shot, int sound, int volume);
extern void RgShotSetBgHitSound(RgShot *shot, int sound, int volume);
extern void RgShotSetNoLifeSound(RgShot *shot, int sound, int volume);
extern void RgShotSetHideSound(RgShot *shot, int sound);

int _ShotTypeShot(RgWeaponShotType *weapon)
{
    if (weapon == 0) {
        assert_prog(D_00A536E0, D_00A53658, 520);
    }
    if (weapon->cooldown > 0.0f || _GetNumAttach(&weapon->common.shotAttach) != 0) {
        return 0;
    }
    {
        RgWeaponShotRequest info;

        _GetShotInfo(&weapon->common, &info);
        if (weapon->common.shotEssence != 0) {
            RgShot *shot;

            shot = CreateRgShotFromEssence(weapon->common.shotEssence, &info);
            _AddShotAttach(&weapon->common.shotAttach, shot);
            RgShotSetSoundDriver(shot, weapon->common.sound);
            RgShotSetHitSound(shot, weapon->common.robotHitSound, weapon->common.robotHitVolume);
            RgShotSetBgHitSound(shot, weapon->common.bgHitSound, weapon->common.extraVolume0);
            RgShotSetNoLifeSound(shot, weapon->common.bgHitVolume, weapon->common.extraVolume1);
            RgShotSetHideSound(shot, weapon->common.shotSound);
            XrgSoundRingVol(weapon->common.sound, weapon->common.shotSound, weapon->common.shotVolume);
            if (weapon->common.shotNum != 1.0e8f) {
                weapon->common.shotNum = weapon->common.shotNum - 1.0f;
            }
            weapon->cooldown = weapon->busyTime;
            return 1;
        }
    }
    return 0;
}

int _ShotTypeIsBusy(RgWeaponShotType *weapon)
{
    if (weapon == 0) {
        assert_prog(D_00A536F0, D_00A53658, 572);
    }
    return weapon->cooldown > 0.0f;
}

void _ShotTypeFree(RgWeaponShotType *weapon)
{
    if (weapon == 0) {
        assert_prog(D_00A536F0, D_00A53658, 580);
    }
    weapon->cooldown = 0.0f;
}

void _ShotTypePassTime(RgWeaponShotType *weapon, float dt)
{
    float cooldown;

    if (_GetNumAttach(&weapon->common.shotAttach) == 0) {
        cooldown = weapon->cooldown - dt;
        weapon->cooldown = cooldown;
        if (cooldown < 0.0f) {
            weapon->cooldown = 0.0f;
        }
    }
}

extern const char D_00A53700[];
extern int XrgActorCreateEffConstraint(XrgActor *actor, int equipType,
                                       XrgActor *owner);

void _InitWeaponCommonEffector(RgWeapon *weapon, int robotId,
                               unsigned int equipType)
{
    XrgActor *actor;
    XrgActor *owner;

    if (weapon == 0) {
        assert_prog(D_00A536F0, D_00A53658, 607);
    }
    if (robotId == 0) {
        assert_prog(D_00A53700, D_00A53658, 608);
    }
    if ((weapon->controlFlags & 7) == 1) {
        return;
    }
    if ((weapon->controlFlags & 0x80) != 0) {
        return;
    }
    actor = RgRobotGetActor(robotId);
    owner = weapon->actor;
    if (equipType == 0) {
        weapon->lockOnEffector = XrgActorCreateEffConstraint(actor, 0, owner);
    } else if (equipType == 1) {
        weapon->lockOnEffector = XrgActorCreateEffConstraint(actor, 1, owner);
    }
}

void _InitWeaponCommonEffector(RgWeapon *weapon, int robotId, unsigned int equipType);
int _ShotTypeShot(RgWeaponShotType *weapon);

void _InitWeaponShot(RgWeaponShotType *weapon, RgWeaponShotEssence *essence,
                     RgWeaponCreateInfo *info)
{
    _InitWeaponCommon(&weapon->common, essence, info);
    weapon->common.shoot = _ShotTypeShot;
    weapon->common.passTime = _ShotTypePassTime;
    weapon->common.isBusy = _ShotTypeIsBusy;
    weapon->common.setFree = _ShotTypeFree;
    weapon->busyTime = essence->busyTime;
    weapon->cooldown = 0.0f;
    _InitWeaponCommonEffector(&weapon->common, info->robotId, info->equipType);
}

void _InitWeaponShot(RgWeaponShotType *weapon, RgWeaponShotEssence *essence, RgWeaponCreateInfo *info);

RgWeaponShotType *_CreateWeaponShotType(RgWeaponShotEssence *pEss,
                                         RgWeaponCreateInfo *pInfo)
{
    RgWeaponShotType *pWpn;

    pWpn = RgHeapAlloc(InstanceOfRgHeap(), 0x200, D_00A53658, 668);
    if (pEss == 0) {
        assert_prog(D_00A53710, D_00A53658, 671);
    }
    if (pWpn == 0) {
        assert_prog(D_00A536F0, D_00A53658, 672);
    }
    if (pInfo == 0) {
        assert_prog(D_00A53720, D_00A53658, 673);
    }
    _InitWeaponShot(pWpn, pEss, pInfo);
    return pWpn;
}

void InitRgWeaponShotEssence(RgWeaponShotEssence *pEss)
{
    RgWeaponShotEssenceInit *init = (RgWeaponShotEssenceInit *) pEss;

    if (pEss == 0) {
        assert_prog(D_00A53710, D_00A53658, 684);
    }
    _InitEssenceCommon(pEss);
    init->m_pCreateMethod = _CreateWeaponShotType;
    init->controlMode = 0x20;
    init->busyTime = 0.4f;
}

int _ShotAttackType(RgWeaponAttackType *weapon)
{
    if (weapon == 0) {
        assert_prog(D_00A536F0, D_00A53658, 725);
    }
    if (weapon->shotSoundPlaying == 0) {
        weapon->shotSoundPlaying = 1;
        XrgSoundRingVol(weapon->common.sound, weapon->common.shotSound,
                        weapon->common.shotVolume);
    }
    return 0;
}

void _ShotStopAttackType(RgWeaponAttackType *weapon)
{
    if (weapon == 0) {
        assert_prog(D_00A536F0, D_00A53658, 737);
    }
    weapon->shotSoundPlaying = 0;
    weapon->active = 1;
}

extern int RgGeomGetStatus(int geom);
extern void RgGeomPointMovePos(int geom, RgVector position);
extern void RgGeomPointSetPos(int geom, RgVector position);
extern void RgGeomResetStatus(int geom, int status);
extern void RgGeomSetStatus(int geom, int status);
extern int XrgActorGetJointLocal(XrgActor *actor, int joint, RgMatrix local);
extern void XrgCopyVector(RgVector destination, RgVector source);

void _PassTimeAttackType(RgWeaponAttackType *weapon)
{
    /*
     * XrgActorGetJointLocal fills the whole 4x4 local transform of the
     * followed joint; only its translation row (the last four floats) is a
     * position, read below through &jointLocal[12].
     */
    RgMatrix jointLocal;

    if (weapon->followActor != 0 &&
            XrgActorGetJointLocal(weapon->followActor, weapon->followJoint, jointLocal) != 0) {
        /*
         * shotRequested and active are the original's two separate early-exit
         * tests, not one "if / else if" condition: folding them into a single
         * else-if compiles the guard scheduling for this pair differently, so
         * shotRequested's test keeps its own goto to the shared reset call.
         */
        if (weapon->common.shotRequested == 0) {
            goto resetStatus;
        }
        if (weapon->active != 0) {
            if (!(RgGeomGetStatus(weapon->geom) & 1)) {
                RgGeomSetStatus(weapon->geom, 1);
                XrgCopyVector(weapon->followPosition, &jointLocal[12]);
            }
            RgGeomPointSetPos(weapon->geom, weapon->followPosition);
            RgGeomPointMovePos(weapon->geom, &jointLocal[12]);
            XrgCopyVector(weapon->followPosition, &jointLocal[12]);
        } else {
resetStatus:
            RgGeomResetStatus(weapon->geom, 1);
        }
    }
}

extern void RgGeomFree(int geom);

void _DestructAttackType(RgWeaponAttackType *weapon)
{
    RgGeomFree(weapon->geom);
}

void _HitBgAttackType(RgWeaponAttackType *weapon, int bgObject)
{
    if (RgBgObjTryToBreak(bgObject, weapon->damage) != 0) {
        weapon->active = 0;
    }
}

extern void CreateRgHitEffectPosDir(int id, RgVector direction, const void *, const void *);
extern int RgRobotGetGeom(int robotId);
extern void RgRobotGiveDamage(int robotId, RgVector direction, int flags, float damage);
extern void RgRobotInvalidAttack(int robotId, RgVector direction, float damage);
extern int RgRobotIsInvalidAttack(int robotId, RgVector direction);
extern RgWeaponAttackEssence *RgWeaponEssCastToAttack(RgWeaponEssenceCommon *ess);
extern const char D_00A53730[];
extern const char D_00A53740[];

void _HitRobotAttackType(RgWeaponAttackType *weapon, int robotId, int damage)
{
    RgVector ownPosition;
    RgVector targetPosition;
    RgVector direction;
    int ownGeom;
    int targetGeom;
    unsigned char *attackEssence;

    if (weapon == 0) {
        assert_prog(D_00A536F0, D_00A53658, 802);
    }
    if (weapon->common.robotId != 0) {
        ownGeom = RgRobotGetGeom(weapon->common.robotId);
        targetGeom = RgRobotGetGeom(robotId);
        __RgGeomPointGetPos((RgGeomPoint *) ownGeom, (RgPointVector *) ownPosition, D_00A53658, 811);
        __RgGeomPointGetPos((RgGeomPoint *) targetGeom, (RgPointVector *) targetPosition, D_00A53658, 812);
        weapon->active = 0;
        XrgSubVector(direction, ownPosition, targetPosition);
        if (RgRobotIsInvalidAttack(robotId, direction) == 0) {
            attackEssence = (unsigned char *) RgWeaponEssCastToAttack((RgWeaponEssenceCommon *) weapon->common.ess);
            if (attackEssence != 0) {
                CreateRgHitEffectPosDir(damage, direction, attackEssence + 0x3bc, D_00A53730);
            }
            RgRobotGiveDamage(robotId, direction, 0, weapon->damage);
            XrgSoundRingVol(weapon->common.sound, weapon->common.robotHitSound, weapon->common.robotHitVolume);
            return;
        }
        CreateRgHitEffectPosDir(damage, direction, D_00A53740, 0);
        RgRobotInvalidAttack(robotId, direction, weapon->damage);
    }
}

extern const char D_00A53750[];
extern int RgGeomGroupCreateBall(void *group, RgWeapon *weapon);
extern int CreateRgGeomBall(float radius, float scale);
extern void RgGeomBallSetRadius(int geom, float radius);

void _InitWeaponAttack(RgWeaponAttackType *weapon, RgWeaponAttackEssence *essence,
                       RgWeaponCreateInfo *info)
{
    RgWeaponAttackEssenceInit *ess;
    void *group;

    ess = (RgWeaponAttackEssenceInit *) essence;
    _InitWeaponCommon((RgWeapon *) weapon, essence, info);
    *(void **) &weapon->followJoint = *(void **) &ess->followJoint;
    weapon->common.shotNum = 1.0f;
    weapon->common.passTime = _PassTimeAttackType;
    weapon->common.dispose = _DestructAttackType;
    weapon->common.shoot = _ShotAttackType;
    weapon->common.shotStop = _ShotStopAttackType;
    weapon->common.hitRobot = _HitRobotAttackType;
    weapon->common.hitBG = _HitBgAttackType;
    group = info->atkGeoms;
    weapon->followExtra = (int) *(char **) &ess->followExtra;
    weapon->damage = ess->damage;
    weapon->followActor = weapon->common.actor;
    weapon->active = 1;
    weapon->common.lockedOn = 0;
    weapon->common.targeting = 0;
    *(int *) ((unsigned char *) weapon + 0x38) = 0;
    weapon->shotSoundPlaying = 0;
    if (group != 0) {
        weapon->geom = RgGeomGroupCreateBall(group, (RgWeapon *) weapon);
    } else {
        weapon->geom = CreateRgGeomBall(1.5f, 1.0f);
    }
    if (weapon->geom == 0) {
        assert_prog(D_00A53750, D_00A53658, 880);
    }
    RgGeomBallSetRadius(weapon->geom, 1.5f);
    RgGeomResetStatus(weapon->geom, 1);
}

RgWeaponAttackType *_CreateWeaponAttackType(RgWeaponAttackEssence *pEss,
                                             RgWeaponCreateInfo *pInfo)
{
    RgWeaponAttackType *pWpn;

    pWpn = RgHeapAlloc(InstanceOfRgHeap(), 0x230, D_00A53658, 889);
    if (pEss == 0) {
        assert_prog(D_00A53710, D_00A53658, 892);
    }
    if (pWpn == 0) {
        assert_prog(D_00A536F0, D_00A53658, 893);
    }
    if (pInfo == 0) {
        assert_prog(D_00A53720, D_00A53658, 894);
    }
    _InitWeaponAttack(pWpn, pEss, pInfo);
    return pWpn;
}

void InitRgWeaponAttackEssence(RgWeaponAttackEssence *pEss)
{
    RgWeaponAttackEssenceInit *init = (RgWeaponAttackEssenceInit *) pEss;
    unsigned int i;

    if (pEss == 0) {
        assert_prog(D_00A53710, D_00A53658, 905);
    }
    _InitEssenceCommon(pEss);
    init->m_pCreateMethod = _CreateWeaponAttackType;
    for (i = 0; i < RG_ACTOR_CHAR_ROBNUM; i++) {
        init->shotMotionTable[i][0] = 0x2B;
        init->shotMotionTable[i][1] = 0x2C;
        init->shotMotionTable[i][2] = -1;
    }
    init->followJoint = 8;
    init->followExtra = 8;
    init->controlMode = 0x4A;
    init->damage = 10.0f;
    init->hitEffectName[0] = '\0';
}

void _InitWeaponUnArmedType(RgWeapon *weapon, RgWeaponUnArmedEssence *essence,
                            RgWeaponCreateInfo *info)
{
    int charID;

    _InitWeaponAttack((RgWeaponAttackType *) weapon,
                      (RgWeaponAttackEssence *) essence, info);
    ((RgWeaponAttackType *) weapon)->followActor =
        RgRobotGetActor(info->robotId);
    charID = RgRobotGetCharID(info->robotId);
    if (charID == 3) {
        if (info->equipType == 0) {
            ((RgWeaponAttackType *) weapon)->followJoint = 2;
        } else if (info->equipType == 1) {
            ((RgWeaponAttackType *) weapon)->followJoint = charID;
        } else {
            ((RgWeaponAttackType *) weapon)->followJoint = 20;
        }
    } else {
        if (info->equipType == 0) {
            ((RgWeaponAttackType *) weapon)->followJoint = 0;
        } else if (info->equipType == 1) {
            ((RgWeaponAttackType *) weapon)->followJoint = info->equipType;
        } else {
            ((RgWeaponAttackType *) weapon)->followJoint = 20;
        }
    }
}

RgWeapon *_CreateWeaponUnArmedType(RgWeaponUnArmedEssence *pEss,
                                    RgWeaponCreateInfo *pInfo)
{
    RgWeapon *pWpn;

    pWpn = RgHeapAlloc(InstanceOfRgHeap(), 0x230, D_00A53658, 969);
    if (pEss == 0) {
        assert_prog(D_00A53710, D_00A53658, 972);
    }
    if (pWpn == 0) {
        assert_prog(D_00A536F0, D_00A53658, 973);
    }
    if (pInfo == 0) {
        assert_prog(D_00A53720, D_00A53658, 974);
    }
    _InitWeaponUnArmedType(pWpn, pEss, pInfo);
    return pWpn;
}

void InitRgWeaponUnArmedEssence(RgWeaponUnArmedEssence *pEss)
{
    /*
     * pEss really addresses the RgWeaponUnArmedEssenceInit layout this
     * allocation evidences (see the header: the tag RgWeaponUnArmedEssence
     * itself is left opaque because ov12/tu027 already completes it for its
     * own, unrelated, non-overlapping field layout).
     */
    RgWeaponUnArmedEssenceInit *init = (RgWeaponUnArmedEssenceInit *) pEss;
    unsigned int i;

    if (pEss == 0) {
        assert_prog(D_00A53710, D_00A53658, 988);
    }
    InitRgWeaponAttackEssence((RgWeaponAttackEssence *) pEss);
    init->m_pCreateMethod = _CreateWeaponUnArmedType;
    init->resetFlag = 0;
    init->controlMode = 76;
    for (i = 0; i < RG_ACTOR_CHAR_ROBNUM; i++) {
        init->shotMotionTable[i][0] = 0x15;
        init->shotMotionTable[i][1] = 0x16;
        init->shotMotionTable[i][2] = -1;
    }
    init->terminator = -1;
}

int _ShotShieldType(RgWeapon *weapon)
{
    if (weapon == 0) {
        assert_prog(D_00A536F0, D_00A53658, 1026);
    }
    return 0;
}

void _ShotStopShieldType(RgWeapon *weapon)
{
    if (weapon == 0) {
        assert_prog(D_00A536F0, D_00A53658, 1033);
    }
}

void _PassTimeShieldType(RgWeapon *weapon)
{
}

void _DestructShieldType(RgWeapon *weapon)
{
}

void _InitWeaponShield(RgWeapon *weapon, RgWeaponShieldEssence *essence,
                       RgWeaponCreateInfo *info)
{
    _InitWeaponCommon(weapon, essence, info);
    weapon->lockedOn = 0;
    weapon->targeting = 0;
    weapon->shotEssence = 0;
    weapon->passTime = _PassTimeShieldType;
    weapon->dispose = _DestructShieldType;
    weapon->shoot = _ShotShieldType;
    weapon->shotStop = _ShotStopShieldType;
}

RgWeapon *_CreateWeaponShieldType(RgWeaponShieldEssence *pEss,
                                   RgWeaponCreateInfo *pInfo)
{
    RgWeapon *pWpn;

    pWpn = RgHeapAlloc(InstanceOfRgHeap(), sizeof(RgWeapon), D_00A53658, 1068);
    if (pEss == 0) {
        assert_prog(D_00A53710, D_00A53658, 1071);
    }
    if (pWpn == 0) {
        assert_prog(D_00A536F0, D_00A53658, 1072);
    }
    if (pInfo == 0) {
        assert_prog(D_00A53720, D_00A53658, 1073);
    }
    _InitWeaponShield(pWpn, pEss, pInfo);
    return pWpn;
}

void InitRgWeaponShieldEssence(RgWeaponShieldEssence *pEss)
{
    unsigned int i;

    if (pEss == 0) {
        assert_prog(D_00A53710, D_00A53658, 1084);
    }
    _InitEssenceCommon(pEss);
    pEss->m_pCreateMethod = _CreateWeaponShieldType;
    for (i = 0; i < RG_ACTOR_CHAR_ROBNUM; i++) {
        pEss->shotMotionTable[i][0] = 0x2B;
        pEss->shotMotionTable[i][1] = 0x2C;
        pEss->shotMotionTable[i][2] = -1;
    }
    pEss->controlMode = 3;
}

int _EnergyIsBusy(RgWeaponEnergyType *weapon)
{
    if (weapon == 0) {
        assert_prog(D_00A536F0, D_00A53658, 1120);
    }
    if (_GetNumAttach(&weapon->common.shotAttach) != 0) {
        return 1;
    }
    return weapon->active != 0;
}

void _EnergyFree(RgWeaponEnergyType *weapon)
{
    if (weapon == 0) {
        assert_prog(D_00A536F0, D_00A53658, 1138);
    }
    weapon->active = 0;
    weapon->energyCharge = 0;
}

/*
 * The energy type's shoot method: records the request for _EnergyPassTime
 * and reports whether a new shot can start (no attached shot and not
 * already active).
 */
int _EnergyShot(RgWeaponEnergyType *weapon)
{
    weapon->shotPending = 1;
    if (_GetNumAttach(&weapon->common.shotAttach) != 0) {
        return 0;
    }
    return weapon->active == 0;
}

void _EnergyPassTime(RgWeaponEnergyType *weapon, float dt)
{
    RgWeaponShotRequest info;
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
        RgShotSetBgHitSound(shot, weapon->common.bgHitSound, weapon->common.extraVolume0);
        RgShotSetNoLifeSound(shot, weapon->common.bgHitVolume, weapon->common.extraVolume1);
        RgShotSetHideSound(shot, weapon->common.shotSound);
        XrgSoundRingVol(weapon->common.sound, weapon->common.shotSound, weapon->common.shotVolume);
        if (shot != 0 && RgShotIsReleased(shot) == 0) {
            _AddShotAttach(&weapon->common.shotAttach, shot);
        }
        startShot = 0;
    }

    if (startShot == 0) {
        float newCharge;

        if (weapon->common.shotNum != 1.0e8f) {
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

    _InitWeaponCommon(&weapon->common, essence, info);
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
    _InitWeaponCommonEffector(common, info->robotId, info->equipType);
}

RgWeaponEnergyType *_CreateWeaponEnergyType(RgWeaponEnergyEssence *pEss,
                                             RgWeaponCreateInfo *pInfo)
{
    RgWeaponEnergyType *pWpn;

    pWpn = RgHeapAlloc(InstanceOfRgHeap(), 0x210, D_00A53658, 1302);
    if (pEss == 0) {
        assert_prog(D_00A53710, D_00A53658, 1305);
    }
    if (pWpn == 0) {
        assert_prog(D_00A536F0, D_00A53658, 1306);
    }
    if (pInfo == 0) {
        assert_prog(D_00A53720, D_00A53658, 1307);
    }
    _InitEnergyType(pWpn, pEss, pInfo);
    return pWpn;
}

void InitRgWeaponEnergyEssence(RgWeaponEnergyEssence *pEss)
{
    RgWeaponEnergyEssenceInit *init = (RgWeaponEnergyEssenceInit *) pEss;

    if (pEss == 0) {
        assert_prog(D_00A53710, D_00A53658, 1318);
    }
    _InitEssenceCommon(pEss);
    init->m_pCreateMethod = _CreateWeaponEnergyType;
    init->capacity = 200.0f;
    init->controlMode = 0x20;
    init->shotEnergy = 1.0f;
    init->upTemp = 100.0f;
    init->limitTemp = 100.0f;
    init->downTemp = 50.0f;
}

/* ov12:0x00a53768 "pEss != NIL && pInfo != NIL" */
extern const char D_00A53768[];
/* ov12:0x00a53788 "pEss->m_pCreateMethod != NIL" */
extern const char D_00A53788[];

RgWeapon *CreateRgWeaponFromEssence(RgWeaponEssenceCommon *pEss, RgWeaponCreateInfo *pInfo)
{
    if (pEss == 0 || pInfo == 0) {
        assert_prog((char *) D_00A53768, D_00A53658, 1340);
    }
    if (pEss->m_pCreateMethod == 0) {
        assert_prog((char *) D_00A53788, D_00A53658, 1341);
    }
    return pEss->m_pCreateMethod(pEss, pInfo);
}

void DisposeRgWeapon(RgWeapon *weapon)
{
    if (weapon == 0) {
        assert_prog(D_00A536F0, D_00A53658, 1348);
    }
    if (weapon->dispose != 0) {
        weapon->dispose(weapon);
    }
    _DestructWeaponCommon(weapon);
    RgHeapFree(InstanceOfRgHeap(), weapon, D_00A53658, 1352);
}

XrgActor *DisposeRgWeaponNotActor(RgWeapon *weapon)
{
    XrgActor *actor;

    if (weapon == 0) {
        assert_prog(D_00A536F0, D_00A53658, 0x551);
    }
    if (weapon->dispose != 0) {
        weapon->dispose(weapon);
    }
    actor = weapon->actor;
    weapon->actor = 0;
    _DestructWeaponCommon(weapon);
    RgHeapFree(InstanceOfRgHeap(), weapon, D_00A53658, 0x557);
    return actor;
}

void RgWeaponSetSound(RgWeapon *weapon, int sound)
{
    if (weapon == 0) {
        assert_prog(D_00A536F0, D_00A53658, 0x585);
    }
    weapon->sound = sound;
}

int RgWeaponIsLockedOn(RgWeapon *weapon)
{
    if (weapon == 0) {
        assert_prog(D_00A536F0, D_00A53658, 0x58F);
    }
    return weapon->lockedOn;
}

int RgWeaponIsTargetting(RgWeapon *weapon)
{
    if (weapon == 0) {
        assert_prog(D_00A536F0, D_00A53658, 0x597);
    }
    return weapon->targeting;
}

float RgWeaponGetShotNum(RgWeapon *weapon)
{
    if (weapon == 0) {
        assert_prog(D_00A536F0, D_00A53658, 0x59F);
    }
    return weapon->shotNum;
}

int RgWeaponIsNotBusy(RgWeapon *weapon)
{
    if (weapon == 0) {
        assert_prog(D_00A536F0, D_00A53658, 0x5A8);
    }
    if (weapon->isBusy != 0) {
        return weapon->isBusy(weapon) == 0;
    }
    return 1;
}

extern void RgWarn(const char *format, const char *file, int line, ...);
extern const char D_00A537A8[];
extern const char D_00A537B8[];
extern int RgWeaponGetEss(RgWeapon *weapon);
extern RgWeaponShotEssence *RgWeaponEssCastToShot(RgWeaponEssenceCommon *ess);

int RgWeaponGetRestUnit(RgWeapon *weapon)
{
    RgWeaponBaseEssence *ess;
    int result;

    if (weapon == 0) {
        assert_prog(D_00A537A8, D_00A53658, 1461);
    }
    ess = (RgWeaponBaseEssence *) RgWeaponGetEss(weapon);
    result = 2;
    if (ess == 0) {
        RgWarn(D_00A537B8, D_00A53658, 1466, weapon, weapon->shotMotionOffset);
        return 2;
    }
    if (ess->capacity == 1e8f) {
        return result;
    }
    if (RgWeaponEssCastToShot((RgWeaponEssenceCommon *) ess) != 0) {
        return 0;
    }
    if (RgWeaponEssCastToAttack((RgWeaponEssenceCommon *) ess) != 0) {
        result = 2;
    } else {
        result = 1;
    }
    return result;
}

int RgWeaponRobotShotMotion(RgWeapon *weapon, int eCharID)
{
    if (!(0 <= eCharID && eCharID < RG_ACTOR_CHAR_ROBNUM)) {
        assert_prog(D_00A537D8, D_00A53658, 0x5D2);
    }
    if (weapon == 0) {
        assert_prog(D_00A536F0, D_00A53658, 0x5D4);
    }
    return weapon->shotMotionTable[eCharID][weapon->shotMotionOffset];
}

XrgActor *RgWeaponGetActor(RgWeapon *weapon)
{
    if (weapon == 0) {
        assert_prog(D_00A536F0, D_00A53658, 0x5DC);
    }
    return weapon->actor;
}

RgEquip *RgWeaponGetEquip(RgWeapon *weapon)
{
    if (weapon == 0) {
        assert_prog(D_00A536F0, D_00A53658, 0x5E4);
    }
    return &weapon->equip;
}

float RgWeaponGetWeight(RgWeapon *weapon)
{
    if (weapon == 0) {
        assert_prog(D_00A536F0, D_00A53658, 0x5EC);
    }
    return weapon->weight;
}

int RgWeaponIsAttachedShot(RgWeapon *weapon)
{
    if (weapon == 0) {
        assert_prog(D_00A536F0, D_00A53658, 1525);
    }
    return _GetNumAttach(&weapon->shotAttach) != 0;
}

int RgWeaponGetControlFlag(RgWeapon *weapon)
{
    if (weapon == 0) {
        assert_prog(D_00A536F0, D_00A53658, 0x5FE);
    }
    return weapon->controlFlags;
}

int RgWeaponGetEss(RgWeapon *weapon)
{
    if (weapon == 0) {
        assert_prog(D_00A536F0, D_00A53658, 0x606);
    }
    return weapon->ess;
}

unsigned int RgWeaponGetAttachedShot(RgWeapon *weapon, RgShot **shots)
{
    if (weapon == 0) {
        assert_prog(D_00A536F0, D_00A53658, 0x624);
    }
    if (shots == 0) {
        return _GetNumAttach(&weapon->shotAttach);
    }
    return _GetAllAttach(&weapon->shotAttach, shots);
}

int RgWeaponIsActiveLockOnSys(RgWeapon *weapon)
{
    if (weapon == 0) {
        assert_prog(D_00A536F0, D_00A53658, 0x62F);
    }
    return weapon->lockOnEffector != 0;
}

void RgWeaponPlayMotion(RgWeapon *weapon, int motion)
{
    XrgActor *actor;

    if (weapon == 0) {
        assert_prog(D_00A536F0, D_00A53658, 0x63E);
    }
    actor = weapon->actor;
    if (actor != 0) {
        XrgActorSetMotion(actor, motion);
    }
}

void RgWeaponPlayShotMotion(RgWeapon *weapon)
{
    if (weapon == 0) {
        assert_prog(D_00A536F0, D_00A53658, 0x649);
    }
    _PlayShotMotion(weapon);
}

int RgWeaponShot(RgWeapon *weapon)
{
    if (weapon == 0) {
        assert_prog(D_00A536F0, D_00A53658, 0x651);
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
        assert_prog(D_00A536F0, D_00A53658, 0x660);
    }
    weapon->equipped = 1;
}

void RgWeaponSetdown(RgWeapon *weapon)
{
    if (weapon == 0) {
        assert_prog(D_00A536F0, D_00A53658, 0x669);
    }
    weapon->equipped = 0;
}

void RgWeaponStopLockon(RgWeapon *weapon)
{
    if (weapon == 0) {
        assert_prog(D_00A536F0, D_00A53658, 0x672);
    }
    weapon->lockOnStopped = 1;
}

void RgWeaponRestartLockon(RgWeapon *weapon)
{
    if (weapon == 0) {
        assert_prog(D_00A536F0, D_00A53658, 0x67B);
    }
    weapon->lockOnStopped = 0;
}

void RgWeaponDisposeLockon(RgWeapon *weapon)
{
    if (weapon == 0) {
        assert_prog(D_00A536F0, D_00A53658, 0x684);
    }
    if (weapon->lockOnEffector != 0) {
        XrgActorDisposeEffector(RgRobotGetActor(weapon->robotId), weapon->lockOnEffector);
        weapon->lockOnEffector = 0;
    }
}

void RgWeaponShotStop(RgWeapon *weapon)
{
    /* separate load keeps the type's shot-stop callback in its own register */
    void (*shotStop)(RgWeapon *weapon);

    if (weapon == 0) {
        assert_prog(D_00A536F0, D_00A53658, 0x691);
    }
    shotStop = weapon->shotStop;
    weapon->shotRequested = 0;
    if (shotStop != 0) {
        shotStop(weapon);
    }
    weapon->equipped = 0;
}

void RgWeaponHitRobot(RgWeapon *weapon, int robotId, int damage)
{
    if (weapon == 0) {
        assert_prog(D_00A536F0, D_00A53658, 0x69E);
    }
    if (weapon->hitRobot != 0) {
        weapon->hitRobot(weapon, robotId, damage);
    }
}

void RgWeaponHitBG(RgWeapon *weapon, int bgObject, int damage)
{
    if (weapon == 0) {
        assert_prog(D_00A536F0, D_00A53658, 0x6A9);
    }
    if (weapon->hitBG != 0) {
        weapon->hitBG(weapon, bgObject, damage);
    }
    XrgSoundRingVol(weapon->sound, weapon->bgHitSound, weapon->bgHitSound);
}

void RgWeaponSubShotNum(RgWeapon *weapon, float delta)
{
    float shotNum;
    float newShotNum;

    if (weapon == 0) {
        assert_prog(D_00A536F0, D_00A53658, 0x6B5);
    }
    shotNum = weapon->shotNum;
    if (shotNum != 1e8f) {
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
        assert_prog(D_00A536F0, D_00A53658, 0x6C3);
    }
    if (weapon->setFree != 0) {
        weapon->setFree(weapon);
    }
}

extern void XrgActorDraw(XrgActor *actor);

void RgWeaponDisp(RgWeapon *weapon)
{
    if (weapon == 0) {
        assert_prog(D_00A536F0, D_00A53658, 0x6CD);
    }
    weapon->disp(weapon);
    if (weapon->actor != 0) {
        XrgActorDraw(weapon->actor);
    }
}

void RgWeaponPassTime(RgWeapon *weapon, float dt)
{
    if (weapon == 0) {
        assert_prog(D_00A536F0, D_00A53658, 0x6D9);
    }
    _CommonPassTime(weapon, dt);
    if (weapon->passTime != 0) {
        weapon->passTime(weapon, dt);
    }
    if (weapon->actor != 0) {
        XrgActorPassTime(weapon->actor, dt);
    }
}

extern void RgGeomRobotCalcLocal(int geom, RgMatrix local);
extern int RgRobotGetStatusFlags(int robotId);
extern float atan2f(float y, float x);

void _CheckLockOn(RgWeapon *weapon)
{
    RgMatrix local;
    RgMatrix invLocal;
    RgVector targetLocal;
    RgVector targetWorld;
    float angle;
    int robotId;
    int statusFlags;
    int wasLockedOn;
    int lockedOn;
    void *target;

    robotId = weapon->robotId;
    if (weapon == 0) {
        assert_prog(D_00A536F0, D_00A53658, 1779);
    }
    wasLockedOn = weapon->lockedOn;
    weapon->targeting = 0;
    weapon->lockedOn = 0;
    statusFlags = RgRobotGetStatusFlags(robotId);
    if (robotId != 0 && (statusFlags & 1) && RgRobotIsConfused(robotId) == 0 &&
            !(statusFlags & 0x100) && weapon->lockOnStopped == 0) {
        RgGeomRobotCalcLocal(RgRobotGetGeom(robotId), local);
        XrgInvMatrix(invLocal, local);
        target = RgRobotGetTarget(robotId);
        if (target != 0) {
            __RgGeomPointGetPos((RgGeomPoint *) target, (RgPointVector *) targetWorld, D_00A53658, 1819);
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
            angle = atan2f(targetLocal[0], targetLocal[2]) + 3.1415927f;
            while (angle > 3.1415927f) {
                angle -= 6.2831855f;
            }
            if (angle < -3.1415927f) {
                do {
                    angle += 6.2831855f;
                } while (angle < -3.1415927f);
            }
            lockedOn = 0;
            if (weapon->lockOnAngleMin <= angle && angle <= weapon->lockOnAngleMax) {
                lockedOn = 1;
            }
            weapon->targeting = lockedOn;
            if (weapon->controlFlags & 0x20) {
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
        assert_prog(D_00A536F0, D_00A53658, 0x73A);
    }
    if (weapon->robotId != 0) {
        return RgRobotGetCharID(weapon->robotId);
    }
    return -1;
}

void _PlayShotMotion(RgWeapon *weapon)
{
    /* separate load keeps shotMotionOffset live across the call below */
    int shotMotionOffset;
    int charID;
    int motion;

    if (weapon == 0) {
        assert_prog(D_00A536F0, D_00A53658, 0x747);
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

int _GetDefShotPosDir(RgWeapon *pWpn, RgVector position, RgVector direction)
{
    if (pWpn == 0) {
        assert_prog(D_00A536F0, D_00A53658, 0x756);
    }
    if (pWpn->shotRequested == 0) {
        return 0;
    }
    _GetDefShotPosDirFunc(pWpn, position, direction);
    return 1;
}

extern void XrgCopyMatrix(RgMatrix destination, const RgMatrix source);
extern void XrgRotMatrixX(RgMatrix destination, const RgMatrix source, float angle);
extern void XrgRotMatrixY(RgMatrix destination, const RgMatrix source, float angle);
extern void XrgCalcMatrixZtoY(RgMatrix matrix, RgVector zAxis, RgVector yAxis);
extern float *XrgVectorY(void);
extern float XrgLengthVector(RgVector vector);
extern void XrgNegateVector(RgVector destination, RgVector source);

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
    RgWeaponBaseEssence *ess;
    float length;
    int joint;
    int found;
    int robotId;

    robotId = weapon->robotId;
    ess = (RgWeaponBaseEssence *) weapon->ess;
    found = 0;
    if (robotId == 0) {
        assert_prog(D_00A53808, D_00A53658, 1892);
    }
    RgGeomRobotCalcLocal(RgRobotGetGeom(robotId), robotLocal);
    if (weapon->actor == 0) {
        XrgCopyMatrix(jointLocal, robotLocal);
    } else {
        if (ess->aimJointCount != 0) {
            unsigned int cursor;
            unsigned int count;
            do {
                cursor = weapon->aimJointCursor;
                weapon->aimJointCursor = cursor + 1;
                joint = ess->aimJointTable[cursor];
            } while (0);
            count = ess->aimJointCount;
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
    if (ess->aimAxis != 3 && found != 0) {
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
    XrgSubVector(aimVector, direction, position);
    length = XrgLengthVector(aimVector);
    XrgCalcMatrixZtoY(jointLocal, aimVector, XrgVectorY());
    XrgRotMatrixY(jointLocal, jointLocal, ess->shotYaw);
    XrgRotMatrixX(jointLocal, jointLocal, ess->shotPitch);
    XrgScaleVector(aimVector, &jointLocal[8], length);
    XrgAddVector(direction, position, aimVector);
}

void _GetShotInfo(RgWeapon *weapon, RgWeaponShotRequest *info)
{
    /* separate load keeps the owning robot's id in its own register, read
     * once and passed to both RgRobotGetTarget and RgRobotIsConfused */
    int robotId;

    robotId = weapon->robotId;
    if (robotId == 0) {
        assert_prog(D_00A53808, D_00A53658, 0x7A5);
    }
    info->target = RgRobotGetTarget(robotId);
    info->id = weapon->id;
    info->weapon = weapon;
    info->getPosDir = _GetDefShotPosDir;
    info->targetConfused = RgRobotIsConfused(robotId);
}

RgWeaponShotEssence *RgWeaponEssCastToShot(RgWeaponEssenceCommon *ess)
{
    if (ess != 0 && (int) ess->m_pCreateMethod == (int) _CreateWeaponShotType) {
        return (RgWeaponShotEssence *) ess;
    }
    return 0;
}

RgWeaponEnergyEssence *RgWeaponEssCastToEnergy(RgWeaponEssenceCommon *ess)
{
    if (ess != 0 && (int) ess->m_pCreateMethod == (int) _CreateWeaponEnergyType) {
        return (RgWeaponEnergyEssence *) ess;
    }
    return 0;
}

RgWeaponAttackEssence *RgWeaponEssCastToAttack(RgWeaponEssenceCommon *ess)
{
    if (ess != 0) {
        if ((int) ess->m_pCreateMethod == (int) _CreateWeaponAttackType) {
            return (RgWeaponAttackEssence *) ess;
        }
        if ((int) ess->m_pCreateMethod == (int) _CreateWeaponUnArmedType) {
            return (RgWeaponAttackEssence *) ess;
        }
    }
    return 0;
}

RgWeaponShieldEssence *RgWeaponEssCastToShield(RgWeaponEssenceCommon *ess)
{
    if (ess != 0 && (int) ess->m_pCreateMethod == (int) _CreateWeaponShieldType) {
        return (RgWeaponShieldEssence *) ess;
    }
    return 0;
}

RgWeaponUnArmedEssence *RgWeaponEssCastToUnArmed(RgWeaponEssenceCommon *ess)
{
    if (ess != 0 && (int) ess->m_pCreateMethod == (int) _CreateWeaponUnArmedType) {
        return (RgWeaponUnArmedEssence *) ess;
    }
    return 0;
}
