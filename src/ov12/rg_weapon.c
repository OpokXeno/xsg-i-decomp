/*
 * OV12 original TU 26: 0x00a19388..0x00a1c7d8 (90 functions)
 */
#include "common.h"
#include "shared.h"
#include "rg_weapon.h"

extern void assert_prog(const char *expression, const char *source_file,
                        int line);

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
extern void _InitWeaponUnArmedType(RgWeapon *weapon,
                                    RgWeaponUnArmedEssence *essence,
                                    RgWeaponCreateInfo *info);
extern void _InitWeaponShield(RgWeapon *weapon, RgWeaponShieldEssence *essence,
                               RgWeaponCreateInfo *info);
extern void _InitEnergyType(RgWeaponEnergyType *weapon,
                             RgWeaponEnergyEssence *essence,
                             RgWeaponCreateInfo *info);
extern void _InitEssenceCommon(void *essence);
static int _GetNumAttach(RgWeaponAttach *pAttach);
extern void _CommonPassTime(RgWeapon *weapon, float dt);
extern int _GetDefShotPosDir(RgWeaponShotRequest *info);
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

INCLUDE_ASM("asm/nonmatchings/ov12/rg_weapon", _ReleaseAttach);

static int _GetNumAttach(RgWeaponAttach *pAttach)
{
    if (pAttach == 0) {
        assert_prog(D_00A53648, D_00A53658, 54);
    }
    return pAttach->m_uAttachNum;
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_weapon", _GetAllAttach);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_weapon", _AddShotAttach);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_weapon", _CheckAttach);

void _CommonDispose(void)
{
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_weapon", _CommonPassTime);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_weapon", _CommonDisp);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_weapon", _InitWeaponCommon);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_weapon", _DestructWeaponCommon);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_weapon", _InitEssenceCommon);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_weapon", _ShotTypeShot);

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

INCLUDE_ASM("asm/nonmatchings/ov12/rg_weapon", _ShotTypePassTime);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_weapon", _InitWeaponCommonEffector);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_weapon", _InitWeaponShot);

extern void _InitWeaponShot(RgWeaponShotType *weapon, RgWeaponShotEssence *essence,
                             RgWeaponCreateInfo *info);

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

INCLUDE_ASM("asm/nonmatchings/ov12/rg_weapon", InitRgWeaponShotEssence);

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

INCLUDE_ASM("asm/nonmatchings/ov12/rg_weapon", _PassTimeAttackType);

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

INCLUDE_ASM("asm/nonmatchings/ov12/rg_weapon", _HitRobotAttackType);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_weapon", _InitWeaponAttack);

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

INCLUDE_ASM("asm/nonmatchings/ov12/rg_weapon", InitRgWeaponAttackEssence);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_weapon", _InitWeaponUnArmedType);

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

INCLUDE_ASM("asm/nonmatchings/ov12/rg_weapon", InitRgWeaponUnArmedEssence);

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

INCLUDE_ASM("asm/nonmatchings/ov12/rg_weapon", _InitWeaponShield);

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

INCLUDE_ASM("asm/nonmatchings/ov12/rg_weapon", _EnergyShot);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_weapon", _EnergyPassTime);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_weapon", _InitEnergyType);

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

INCLUDE_ASM("asm/nonmatchings/ov12/rg_weapon", InitRgWeaponEnergyEssence);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_weapon", CreateRgWeaponFromEssence);

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

INCLUDE_ASM("asm/nonmatchings/ov12/rg_weapon", RgWeaponIsNotBusy);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_weapon", RgWeaponGetRestUnit);

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

INCLUDE_ASM("asm/nonmatchings/ov12/rg_weapon", RgWeaponIsAttachedShot);

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

INCLUDE_ASM("asm/nonmatchings/ov12/rg_weapon", RgWeaponGetAttachedShot);

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

INCLUDE_ASM("asm/nonmatchings/ov12/rg_weapon", RgWeaponDisp);

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

INCLUDE_ASM("asm/nonmatchings/ov12/rg_weapon", _CheckLockOn);

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

INCLUDE_ASM("asm/nonmatchings/ov12/rg_weapon", _GetDefShotPosDir);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_weapon", _GetDefShotPosDirFunc);

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

INCLUDE_ASM("asm/nonmatchings/ov12/rg_weapon", RgWeaponEssCastToShot);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_weapon", RgWeaponEssCastToEnergy);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_weapon", RgWeaponEssCastToAttack);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_weapon", RgWeaponEssCastToShield);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_weapon", RgWeaponEssCastToUnArmed);
