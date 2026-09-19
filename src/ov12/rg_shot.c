/*
 * OV12 original TU 23: 0x00a15600..0x00a17c90 (63 functions)
 */
#include "common.h"
#include "shared.h"
#include "rg_shot.h"

extern void assert_prog(const char *expression, const char *source_file,
                        int line);

extern const char D_00A532E8[]; /* "pShot != NIL" */
extern const char D_00A532F8[]; /* "../rg_shot.euc.c" */
extern const char D_00A53348[]; /* "pEssence != NIL" */
extern const char D_00A53358[]; /* "pInfo != NIL" */
extern const char D_00A53390[]; /* "pEss != NIL" */
extern const char D_00A533D8[]; /* "pRob != NIL" */
extern const char D_00A533E8[]; /* "pBgObj != NIL" */
extern const char D_00A53380[]; /* "eff11.ptcl" (default hit-effect file) */
extern const char D_00A533A0[]; /* "pFire != NIL" */
extern const char D_00A533C8[]; /* "pBeam != NIL" */

extern void *RgCharAlloc(unsigned int size, int type);
void *_InitBeam(void *beam, void *essence, void *info);

extern int RgBgObjTryToBreak(int bgObject, float damage);
extern void XrgSoundRingVol(int driver, int soundId, int volume);
extern void CreateRgHitEffectPos(RgVector position, const char *effectFile,
                                 const char *defaultEffectFile);

/* Defined by other translation units of this overlay. */
extern void RgGeomFree(RgGeom *geom);
extern void RgGeomResetStatus(RgGeom *geom, unsigned int flags);
extern void RgGeomPassTime(RgGeom *geom, float deltaTime);
extern void RgCharFree(RgChar *pChar);
extern void RgShotEffectDisp(RgShotEffect *effect);
extern void DisposeRgShotEffect(RgShotEffect *effect);
extern void XrgSoundRingStop(int driver, int soundId);

/* Defined later in this TU (a local sibling still in asm). */
static RgGeom *_CommonGetGeom(RgShot *shot);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_shot", _CommonGetGeom);

extern void XrgClearVector(RgVector vector);

static int _GetShotPos2(RgShot *shot, RgVector position, RgVector aimPoint)
{
    void *posOwner;

    XrgClearVector(position);
    XrgClearVector(aimPoint);
    posOwner = shot->posOwner;
    if (posOwner == 0) {
        return 0;
    }
    if (shot->getPos == 0) {
        return 0;
    }
    return shot->getPos(posOwner, position, aimPoint);
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_shot", _GetShotPosDir);

static void _ShotCommonControl(RgChar *pChar)
{
}

static void _ShotCommonPassTimeGeom(RgShot *shot, float deltaTime)
{
    RgGeomPassTime(_CommonGetGeom(shot), deltaTime);
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_shot", _ShotCommonPassTimeLife);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_shot", _ShotCommonPassTimeEffect);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_shot", _ShotCommonPassTime);

static void _ShotCommonDisp(RgShot *shot)
{
    RgShotEffectDisp(shot->effect);
}

static void _ShotCommonDestruct(RgShot *shot)
{
    if (shot->geomTray != 0) {
        RgGeomFree(shot->geomTray);
        shot->geomTray = 0;
    }
    if (shot->effect != 0) {
        DisposeRgShotEffect(shot->effect);
        shot->effect = 0;
    }
    XrgSoundRingStop(shot->soundDriver, shot->hideSoundId);
}

static void _ShotCommonHit(RgShot *shot)
{
    RgGeomResetStatus(shot->geomTray, 1);
    RgCharFree((RgChar *)shot);
}

static int _ShotCommonIsReleased(RgShot *shot)
{
    return 1;
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_shot", _InitShotCommonSub);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_shot", _InitShotCommon);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_shot", _InitRgShotEssence);

/* Defined earlier in this TU (a local sibling still in asm). */
static void _InitShotCommon(void *shot, void *essence, void *info, float speed);

static void *_CreateRgNormalShot(RgNormalShotEssence *essence, void *info)
{
    void *shot;

    shot = RgCharAlloc(0x70, 3);
    if (shot == 0) {
        assert_prog(D_00A532E8, D_00A532F8, 348);
    }
    _InitShotCommon(shot, essence, info, essence->speed);
    return shot;
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_shot", InitRgNormalShotEssence);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_shot", _HomingPassTime);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_shot", _InitHoming);

/* Defined earlier in this TU (a local sibling still in asm). */
static void _InitHoming(void *shot, void *essence, void *info);

static void *_CreateHoming(void *essence, void *info)
{
    void *shot;

    shot = RgCharAlloc(0x90, 3);
    if (shot == 0) {
        assert_prog(D_00A532E8, D_00A532F8, 467);
    }
    if (essence == 0) {
        assert_prog(D_00A53348, D_00A532F8, 468);
    }
    if (info == 0) {
        assert_prog(D_00A53358, D_00A532F8, 469);
    }
    _InitHoming(shot, essence, info);
    return shot;
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_shot", InitRgHomingShotEssence);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_shot", _GrenadeEntryHistory);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_shot", _GrenadeBomPassTime);

/* Defined later in this TU (a local sibling still in asm). */
static void _GrenadeSetBom(RgShot *shot);

static void _GrenadeShotZeroLife(RgShot *shot)
{
    _GrenadeSetBom(shot);
}

static void _GrenadeHomingHit(RgShot *shot, RgVector position)
{
    _GrenadeSetBom(shot);
}

static void _GrenadeBomHit(RgShot *shot, RgVector position)
{
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_shot", _GrenadeHitRobot);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_shot", _GrenadeHitBg);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_shot", _GrenadeSetBom);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_shot", _GrenadeHomingPassTime);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_shot", _InitGrenade);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_shot", _CreateGrenade);

/* Defined earlier in this TU (a local sibling still in asm). */
extern void InitRgHomingShotEssence(void *essence);

/* Defined earlier in this TU (a local sibling still in asm). */
static void *_CreateGrenade(void *essence, void *info);

void InitRgGrenadeEssence(RgGrenadeEssence *essence)
{
    InitRgHomingShotEssence(essence);
    essence->createFunc = _CreateGrenade;
    essence->explosionRadius = 10.0f;
    essence->duration = 1.0f;
    essence->proximity = 0.0f;
    essence->repeatCount = 0.0f;
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_shot", _FireKill);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_shot", _FirePassTime);

static int _FireIsReleased(RgShot *shot)
{
    RgFireShot *fire;

    if (shot == 0) {
        assert_prog(D_00A533A0, D_00A532F8, 896);
    }
    fire = (RgFireShot *)shot;
    return (unsigned int)fire->status >= 2;
}

/* Defined earlier in this TU (a local sibling still in asm). */
static void _FireKill(RgShot *shot);

static void _FireRelease(RgShot *shot)
{
    if (shot == 0) {
        assert_prog(D_00A533A0, D_00A532F8, 0x38D);
    }
    if (((RgFireShot *)shot)->status != 2) {
        _FireKill(shot);
    }
}

static void _FireHit(RgShot *shot, RgVector position)
{
}

/* Defined later in this TU (a local sibling still in asm). */
static void _ShotCommonHitRobot(RgShot *shot, int robot, RgVector position);

static void _FireHitRobot(RgShot *shot, int robot, RgVector position)
{
    RgFireShot *fire = (RgFireShot *)shot;

    if (fire->robot == 0 || fire->alreadyHitRobot == 0) {
        _ShotCommonHitRobot(shot, robot, position);
    }
    fire->alreadyHitRobot = 1;
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_shot", _InitFire);

/* Defined earlier in this TU (a local sibling still in asm). */
static void *_InitFire(void *fire, void *essence, void *info);

static void *_CreateFire(void *essence, void *info)
{
    void *fire;

    fire = RgCharAlloc(0xE0, 3);
    if (essence == 0) {
        assert_prog(D_00A53390, D_00A532F8, 0x3D3);
    }
    if (info == 0) {
        assert_prog(D_00A53358, D_00A532F8, 0x3D4);
    }
    _InitFire(fire, essence, info);
    return fire;
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_shot", InitRgFireEssence);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_shot", _BeamPassTime);

static int _BeamIsReleased(RgShot *shot)
{
    if (shot == 0) {
        assert_prog(D_00A533C8, D_00A532F8, 0x44B);
    }
    return ((RgBeamShot *)shot)->lifetime <= 0.0f;
}

static void _BeamHit(RgShot *shot, RgVector position)
{
    RgBeamShot *beam;

    if (shot == 0) {
        assert_prog(D_00A533C8, D_00A532F8, 0x454);
    }
    beam = (RgBeamShot *)shot;
    beam->lifetime = 0.0f;
    beam->hit = 1;
}

/*
 * Partial view of a beam object.  The callback's only evidenced state is the
 * lifetime value at byte offset 0x98; the surrounding object layout remains
 * outside this allocation.
 */

static void _BeamRelease(void *beam)
{
    BeamLifetimeField *lifetime_field =
        (BeamLifetimeField *)((unsigned char *)beam + 0x98);

    lifetime_field->lifetime = -0.5f;
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_shot", _InitBeam);

static void *_CreateBeam(void *essence, void *info)
{
    void *beam;

    beam = RgCharAlloc(0xD0, 3);
    if (essence == 0)
        assert_prog(D_00A53390, D_00A532F8, 1166);
    if (info == 0)
        assert_prog(D_00A53358, D_00A532F8, 1167);
    _InitBeam(beam, essence, info);
    return beam;
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_shot", InitRgBeamEssence);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_shot", CreateRgShotFromEssence);

void RgShotSetSoundDriver(RgShot *shot, int driver)
{
    if (shot == 0)
        assert_prog(D_00A532E8, D_00A532F8, 1206);
    shot->soundDriver = driver;
}

void RgShotSetHitSound(RgShot *shot, int soundId, int volume)
{
    if (shot == 0)
        assert_prog(D_00A532E8, D_00A532F8, 1214);
    shot->hitSoundId = soundId;
    shot->hitSoundVolume = volume;
}

void RgShotSetBgHitSound(RgShot *shot, int soundId, int volume)
{
    if (shot == 0)
        assert_prog(D_00A532E8, D_00A532F8, 1223);
    shot->bgHitSoundId = soundId;
    shot->bgHitSoundVolume = volume;
}

void RgShotSetNoLifeSound(RgShot *shot, int soundId, int volume)
{
    if (shot == 0)
        assert_prog(D_00A532E8, D_00A532F8, 1232);
    shot->noLifeSoundId = soundId;
    shot->noLifeSoundVolume = volume;
}

void RgShotSetHideSound(RgShot *shot, int soundId)
{
    if (shot == 0)
        assert_prog(D_00A532E8, D_00A532F8, 1241);
    shot->hideSoundId = soundId;
}

float RgShotGetDamage(RgShot *shot)
{
    if (shot == 0)
        assert_prog(D_00A532E8, D_00A532F8, 1252);
    return shot->damage;
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_shot", RgShotIsReleased);

int RgShotGetHitRobNum(RgShot *shot)
{
    if (shot == 0)
        assert_prog(D_00A532E8, D_00A532F8, 1271);
    return shot->hitRobotCount;
}

void RgShotRelease(RgShot *shot)
{
    if (shot == 0)
        assert_prog(D_00A532E8, D_00A532F8, 1283);
    if (shot->release != 0)
        shot->release(shot);
}

void RgShotHitRobot(RgShot *shot, int robot, RgVector position)
{
    if (shot == 0)
        assert_prog(D_00A532E8, D_00A532F8, 1292);
    if (robot == 0)
        assert_prog(D_00A533D8, D_00A532F8, 1293);
    shot->hitRobotCount++;
    if (shot->hitRobot != 0)
        shot->hitRobot(shot, robot, position);
    if (shot->notifyHit != 0)
        shot->notifyHit(shot, position);
}

void RgShotHitBg(RgShot *shot, int bgObject, RgVector position)
{
    if (shot == 0)
        assert_prog(D_00A532E8, D_00A532F8, 1306);
    if (bgObject == 0)
        assert_prog(D_00A533E8, D_00A532F8, 1307);
    if (shot->hitBg != 0)
        shot->hitBg(shot, bgObject, position);
    if (shot->notifyHit != 0)
        shot->notifyHit(shot, position);
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_shot", _ShotCommonHitRobot);

static void _ShotCommonHitBg(RgShot *shot, int bgObject, RgVector position)
{
    if (shot == 0)
        assert_prog(D_00A532E8, D_00A532F8, 1365);
    if (bgObject == 0)
        assert_prog(D_00A533E8, D_00A532F8, 1366);
    if (RgBgObjTryToBreak(bgObject, shot->damage) != 0)
        XrgSoundRingVol(shot->soundDriver, shot->bgHitSoundId, shot->bgHitSoundVolume);
    CreateRgHitEffectPos(position, shot->essence->hitEffectFile, D_00A53380);
}
