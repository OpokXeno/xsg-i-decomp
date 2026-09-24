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
extern const char D_00A53338[]; /* "pExtInfo != NIL" */
extern const char D_00A53348[]; /* "pEssence != NIL" */
extern const char D_00A53358[]; /* "pInfo != NIL" */
extern const char D_00A53368[]; /* "pBom != NIL" */
extern const char D_00A53378[]; /* "tama" (bomb model variant) */
extern const char D_00A53390[]; /* "pEss != NIL" */
extern const char D_00A533D8[]; /* "pRob != NIL" */
extern const char D_00A533E8[]; /* "pBgObj != NIL" */
extern const char D_00A53380[]; /* "eff11.ptcl" (default hit-effect file) */
extern const char D_00A533A0[]; /* "pFire != NIL" */
extern const char D_00A533C8[]; /* "pBeam != NIL" */
extern const char D_00A53310[]; /* "pGeom != NIL" */

extern void *RgCharAlloc(unsigned int size, int type);
static void *_InitBeam(void *beam, void *essence, void *info);

/* Local siblings of this TU still in asm, needed by functions of this allocation. */
static void _InitShotCommonSub(RgShot *shot, int id);
static void _InitRgShotEssence(RgShotEssence *essence);
static int _GrenadeEntryHistory(RgShot *shot, int bgObject);
static void _GrenadeBomPassTime(RgShot *shot, float deltaTime);
static void _InitGrenade(void *shot, void *essence, void *info);
static void _HomingPassTime(RgHomingShot *shot, float deltaTime);
static void _ShotCommonPassTime(RgShot *shot, float deltaTime);

extern int RgBgObjTryToBreak(int bgObject, float damage);
extern void XrgSoundRingVol(int driver, int soundId, int volume);
extern void CreateRgHitEffectPos(RgVector position, const char *effectFile,
                                 const char *defaultEffectFile);

/* Defined by other translation units of this overlay. */
extern void RgGeomFree(RgGeom *geom);
extern void RgGeomResetStatus(RgGeom *geom, unsigned int flags);
extern void RgGeomPassTime(RgGeom *geom, float deltaTime);
extern void RgCharFree(RgChar *pChar);
extern void RgCharPassTimeMethod(RgChar *pChar,
                                 void (*passTimeMethod)(RgChar *pChar, float deltaTime));
extern void RgShotEffectDisp(RgShotEffect *effect);
extern void DisposeRgShotEffect(RgShotEffect *effect);
extern void XrgSoundRingStop(int driver, int soundId);

extern void XrgCopyVector(RgVector destination, RgVector source);
extern void XrgSubVector(RgVector destination, RgVector first, RgVector second);
extern void XrgAddVector(RgVector destination, RgVector first, RgVector second);
extern void XrgScaleVector(RgVector destination, RgVector source, float scale);
extern void XrgNormalizeVector(RgVector destination, RgVector source);
extern float XrgLengthVector(RgVector vector);
extern float XrgInnerVector(RgVector first, RgVector second);
extern void XrgCalcMatrixXtoY(RgMatrix destination, RgVector x_axis,
                              RgVector y_axis);
extern void XrgRotMatrixZ(RgMatrix destination, RgMatrix source, float angle);
extern float XrgRand(float lower, float upper);
extern float cosf(float angle);

extern void RgGeomBallSetRadius(RgGeomBall *geom, float radius);
extern void RgGeomPointSetVel(RgGeomPoint *point, RgVector velocity);
extern void RgGeomPointGetVel(RgGeomPoint *point, RgVector velocity);
extern void RgGeomPointSetPos(RgGeomPoint *point, RgVector position);
extern void RgGeomPointMovePos(RgGeomPoint *point, RgVector position);
extern void __RgGeomPointGetPos(RgGeomPoint *point, RgVector destination,
                                const char *source_file, int source_line);
extern void __RgGeomPointGetOldPos(RgGeomPoint *point, RgVector destination,
                                   const char *source_file, int source_line);

extern void RgShotEffectSetPos(RgShotEffect *effect, RgVector from, RgVector to);
extern void RgShotEffectPassTime(RgShotEffect *effect, float deltaTime);
extern void RgShotEffectSetModel(RgShotEffect *effect, const char *variant);
extern void RgShotEffectSetParticle(RgShotEffect *effect, const char *file);
extern void RgShotEffectSetTexLine(RgShotEffect *effect, const char *file);
extern void RgShotEffectSetTexLineModelSize(RgShotEffect *effect, float width,
                                            float height);
extern void RgShotEffectSetTexLineModelColor(RgShotEffect *effect,
                                             const void *color);
extern void RgShotEffectStopAlive(RgShotEffect *effect, float time);
extern RgParticleEffect *RgShotEffectGetPtclEffect(RgShotEffect *effect);
extern void RgParticleEffectStopShoot(RgParticleEffect *effect);

/* Defined later in this TU (a local sibling still in asm). */
static RgGeom *_CommonGetGeom(RgShot *shot);

RgGeom *_CommonGetGeom(RgShot *shot)
{
    RgGeom *geom;

    if (shot == 0) {
        assert_prog(D_00A532E8, D_00A532F8, 0x58);
    }
    geom = shot->geomTray;
    if (geom == 0) {
        assert_prog(D_00A53310, D_00A532F8, 0x5A);
    }
    return geom;
}

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

static int _GetShotPosDir(RgShot *shot, RgVector position, RgVector direction)
{
    RgVector shotPosition;
    RgVector aimPoint;

    XrgClearVector(position);
    XrgClearVector(direction);
    if (_GetShotPos2(shot, shotPosition, aimPoint) == 0) {
        return 0;
    }
    XrgCopyVector(position, shotPosition);
    XrgSubVector(direction, aimPoint, shotPosition);
    XrgNormalizeVector(direction, direction);
    return 1;
}

static void _ShotCommonControl(RgChar *pChar)
{
}

static void _ShotCommonPassTimeGeom(RgShot *shot, float deltaTime)
{
    RgGeomPassTime(_CommonGetGeom(shot), deltaTime);
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_shot", _ShotCommonPassTimeLife);

static void _ShotCommonPassTimeEffect(RgShot *shot, float deltaTime)
{
    RgVector position;
    RgVector oldPosition;

    __RgGeomPointGetPos((RgGeomPoint *) _CommonGetGeom(shot), position,
                        D_00A532F8, 161);
    __RgGeomPointGetOldPos((RgGeomPoint *) _CommonGetGeom(shot), oldPosition,
                           D_00A532F8, 162);
    RgShotEffectSetPos(shot->effect, oldPosition, position);
    RgShotEffectPassTime(shot->effect, deltaTime);
}

/* Local sibling of this TU still in asm, needed by functions of this allocation. */
static void _ShotCommonPassTimeLife(RgShot *shot, float deltaTime);

static void _ShotCommonPassTime(RgShot *shot, float deltaTime)
{
    _ShotCommonPassTimeGeom(shot, deltaTime);
    _ShotCommonPassTimeLife(shot, deltaTime);
    _ShotCommonPassTimeEffect(shot, deltaTime);
}

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

static void _InitShotCommon(void *shot, void *essence, void *info, float speed)
{
    RgShot *typedShot;
    RgShotEssence *typedEssence;
    RgShotRequest *extInfo;
    RgVector position;
    RgVector direction;

    typedShot = shot;
    typedEssence = essence;
    extInfo = info;
    if (typedShot == 0) {
        assert_prog(D_00A532E8, D_00A532F8, 290);
    }
    if (extInfo == 0) {
        assert_prog(D_00A53338, D_00A532F8, 291);
    }
    _InitShotCommonSub(typedShot, extInfo->id);
    typedShot->essence = typedEssence;
    typedShot->life = typedEssence->life;
    typedShot->damage = typedEssence->damage;
    RgShotEffectSetModel(typedShot->effect, typedEssence->modelVariant);
    RgShotEffectSetParticle(typedShot->effect, typedEssence->particleFile);
    RgShotEffectSetTexLine(typedShot->effect, typedEssence->texLineFile);
    RgShotEffectSetTexLineModelSize(typedShot->effect, typedEssence->texLineWidth,
                                    typedEssence->texLineHeight);
    RgShotEffectSetTexLineModelColor(typedShot->effect, typedEssence->texLineColor);
    if (_GetShotPosDir((RgShot *) extInfo, position, direction) != 0) {
        XrgScaleVector(direction, direction, speed);
        RgGeomPointSetPos((RgGeomPoint *) typedShot->geomTray, position);
        RgGeomPointSetVel((RgGeomPoint *) typedShot->geomTray, direction);
        return;
    }
    XrgClearVector(position);
}

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

void InitRgNormalShotEssence(RgNormalShotEssence *essence)
{
    if (essence == 0) {
        assert_prog(D_00A53348, D_00A532F8, 0x164);
    }
    _InitRgShotEssence(&essence->common);
    essence->common.createFunc = (void *(*)(void *, void *)) _CreateRgNormalShot;
    essence->speed = 45.0f;
}

static void _HomingPassTime(RgHomingShot *shot, float deltaTime)
{
    RgVector targetPosition;
    RgVector ownPosition;
    RgVector ownVelocity;
    RgVector aimDirection;
    RgMatrix aimBasis;
    float delay;

    if (shot == 0) {
        assert_prog(D_00A532E8, D_00A532F8, 0x189);
    }
    _ShotCommonPassTime(&shot->common, deltaTime);
    delay = shot->delay - deltaTime;
    shot->delay = delay;
    if (delay < 0.0f) {
        __RgGeomPointGetPos((RgGeomPoint *) _CommonGetGeom(&shot->common),
                            ownPosition, D_00A532F8, 0x19B);
        RgGeomPointGetVel((RgGeomPoint *) _CommonGetGeom(&shot->common),
                          ownVelocity);
        __RgGeomPointGetPos((RgGeomPoint *) shot->target, targetPosition,
                            D_00A532F8, 0x19D);
        if (shot->randomTargeting != 0) {
            targetPosition[0] = ownPosition[0] + XrgRand(-50.0f, 50.0f);
            targetPosition[1] = ownPosition[1] + XrgRand(-50.0f, 50.0f);
            targetPosition[2] = ownPosition[2] + XrgRand(-50.0f, 50.0f);
        }
        XrgSubVector(aimDirection, targetPosition, ownPosition);
        XrgCalcMatrixXtoY(aimBasis, ownVelocity, aimDirection);
        if (!(shot->cosThreshold < XrgInnerVector(aimBasis, aimBasis + 4))) {
            XrgRotMatrixZ(aimBasis, aimBasis, shot->turnRate * deltaTime);
            XrgScaleVector(ownVelocity, aimBasis, shot->speed);
            RgGeomPointSetVel((RgGeomPoint *) _CommonGetGeom(&shot->common),
                              ownVelocity);
        }
    }
}

static void _InitHoming(void *shot, void *essence, void *info)
{
    RgHomingShot *typedShot;
    RgHomingShotEssence *typedEssence;
    RgShotRequest *typedInfo;
    float turnRate;
    float cosThreshold;
    float delay;
    float speed;
    RgGeom *target;
    int randomTargeting;

    typedShot = shot;
    typedEssence = essence;
    typedInfo = info;
    _InitShotCommon(&typedShot->common, &typedEssence->common, info,
                    typedEssence->speed);
    RgCharPassTimeMethod((RgChar *) typedShot,
                         (void (*)(RgChar *, float)) _HomingPassTime);
    turnRate = typedEssence->turnRate;
    typedShot->turnRate = turnRate;
    target = typedInfo->target;
    randomTargeting = typedInfo->targetConfused;
    cosThreshold = typedEssence->cosThreshold;
    typedShot->randomTargeting = randomTargeting;
    typedShot->cosThreshold = cosThreshold;
    typedShot->target = target;
    delay = typedEssence->delay;
    typedShot->delay = delay;
    speed = typedEssence->speed;
    typedShot->speed = speed;
}

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

void InitRgHomingShotEssence(void *essence)
{
    RgHomingShotEssence *typedEssence;
    float cosThreshold;

    typedEssence = essence;
    if (typedEssence == 0) {
        assert_prog(D_00A53348, D_00A532F8, 479);
    }
    _InitRgShotEssence(&typedEssence->common);
    typedEssence->common.createFunc = _CreateHoming;
    typedEssence->common.life = 2.0f;
    typedEssence->speed = 30.0f;
    typedEssence->turnRate = 0.7853982f;
    cosThreshold = cosf(0.19634955f);
    typedEssence->cosThreshold = cosThreshold;
    typedEssence->delay = 0.3f;
}

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

/* Defined later in this TU (a local sibling still in asm). */
static void _ShotCommonHitBg(RgShot *shot, int bgObject, RgVector position);

static void _GrenadeHitBg(RgShot *shot, int bgObject, RgVector position)
{
    if (_GrenadeEntryHistory(shot, bgObject) != 0) {
        _ShotCommonHitBg(shot, bgObject, position);
    }
}

static void _GrenadeSetBom(RgShot *shot)
{
    RgGrenadeShot *typedShot;
    RgGeom *geomTray;
    RgVector velocity;
    RgVector position;
    float hitEffectTrigger;
    RgParticleEffect *particleEffect;

    typedShot = (RgGrenadeShot *) shot;
    geomTray = shot->geomTray;
    RgCharPassTimeMethod((RgChar *) shot,
                         (void (*)(RgChar *, float)) _GrenadeBomPassTime);
    shot->notifyHit = _GrenadeBomHit;
    shot->life = 1e8f;
    shot->hitHistory = 0;
    RgGeomBallSetRadius((RgGeomBall *) geomTray, 0.0f);
    XrgClearVector(velocity);
    RgGeomPointSetVel((RgGeomPoint *) geomTray, velocity);
    RgShotEffectSetModel(shot->effect, D_00A53378);
    hitEffectTrigger = (float) typedShot->repeatCount;
    if (hitEffectTrigger > 0.0f) {
        __RgGeomPointGetPos((RgGeomPoint *) geomTray, position, D_00A532F8, 693);
        CreateRgHitEffectPos(position, shot->essence->hitEffectFile, D_00A53380);
        RgShotEffectSetModel(shot->effect, D_00A53378);
    }
    particleEffect = RgShotEffectGetPtclEffect(shot->effect);
    if (particleEffect != 0) {
        RgParticleEffectStopShoot(particleEffect);
    }
}

static void _GrenadeHomingPassTime(RgGrenadeShot *bom, float deltaTime)
{
    RgVector targetPosition;
    RgVector diff;
    RgGeom *target;

    if (bom == 0) {
        assert_prog(D_00A53368, D_00A532F8, 710);
    }
    _HomingPassTime(&bom->common, deltaTime);
    if (bom->proximity > 0.0f) {
        target = bom->common.target;
        if (target != 0) {
            __RgGeomPointGetPos((RgGeomPoint *) target, targetPosition,
                                D_00A532F8, 718);
            __RgGeomPointGetPos((RgGeomPoint *) _CommonGetGeom(&bom->common.common),
                                diff, D_00A532F8, 719);
            XrgSubVector(diff, diff, targetPosition);
            if (XrgLengthVector(diff) < bom->proximity) {
                _GrenadeSetBom(&bom->common.common);
            }
        }
    }
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_shot", _InitGrenade);

static void *_CreateGrenade(void *essence, void *info)
{
    void *bom;

    bom = RgCharAlloc(0xD0, 3);
    if (bom == 0) {
        assert_prog(D_00A53368, D_00A532F8, 769);
    }
    if (essence == 0) {
        assert_prog(D_00A53390, D_00A532F8, 770);
    }
    if (info == 0) {
        assert_prog(D_00A53358, D_00A532F8, 771);
    }
    _InitGrenade(bom, essence, info);
    return bom;
}

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

static void _FireKill(RgShot *shot)
{
    RgFireShot *typedShot;

    if (shot == 0) {
        assert_prog(D_00A533A0, D_00A532F8, 813);
    }
    typedShot = (RgFireShot *) shot;
    typedShot->status = 2;
    if (typedShot->quickFade != 0) {
        typedShot->fadeTime = 0.0f;
    } else {
        typedShot->fadeTime = 1.0f;
    }
    RgShotEffectStopAlive(shot->effect, typedShot->fadeTime);
}

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

void InitRgFireEssence(void *essence)
{
    RgFireEssence *typedEssence;

    typedEssence = essence;
    if (typedEssence == 0) {
        assert_prog(D_00A53390, D_00A532F8, 990);
    }
    _InitRgShotEssence(&typedEssence->common);
    typedEssence->common.createFunc = _CreateFire;
    typedEssence->common.life = 1e8f;
    typedEssence->speed = 20.0f;
    typedEssence->duration = 2.0f;
    typedEssence->trailingCount = 0;
}

static void _BeamPassTime(RgBeamShot *beam, float deltaTime)
{
    RgVector movement;
    RgVector position;
    RgVector direction;
    RgGeom *geom;
    float damage;
    float remainingLifetime;
    float moveAmount;
    float travelled;
    float collapsedLength;

    if (beam == 0) {
        assert_prog(D_00A533C8, D_00A532F8, 1034);
    }
    _ShotCommonPassTime(&beam->common, deltaTime);
    damage = beam->damageRate * deltaTime;
    remainingLifetime = beam->lifetime - deltaTime;
    moveAmount = beam->speed * deltaTime;
    beam->lifetime = remainingLifetime;
    beam->common.damage = damage;
    if (remainingLifetime > 0.0f) {
        if (_GetShotPosDir((RgShot *) &beam->request, position, direction) != 0) {
            XrgCopyVector(beam->origin, position);
            XrgCopyVector(beam->direction, direction);
            travelled = beam->length + moveAmount;
            beam->length = travelled;
        } else {
            beam->lifetime = 0.0f;
        }
    }
    if (beam->lifetime <= 0.0f) {
        XrgScaleVector(movement, beam->direction, moveAmount);
        XrgAddVector(beam->origin, beam->origin, movement);
    }
    if (beam->hit != 0) {
        beam->hit = 0;
        collapsedLength = beam->length - moveAmount;
        beam->length = collapsedLength;
        if (collapsedLength <= 0.0f) {
            RgCharFree((RgChar *) beam);
            return;
        }
    }
    geom = _CommonGetGeom(&beam->common);
    XrgScaleVector(movement, beam->direction, beam->length);
    XrgAddVector(position, beam->origin, movement);
    RgGeomPointSetPos((RgGeomPoint *) geom, beam->origin);
    RgGeomPointMovePos((RgGeomPoint *) geom, position);
    RgShotEffectSetPos(beam->common.effect, beam->origin, position);
}

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

void InitRgBeamEssence(void *essence)
{
    RgBeamEssence *typedEssence;

    typedEssence = essence;
    if (typedEssence == 0) {
        assert_prog(D_00A53348, D_00A532F8, 1177);
    }
    _InitRgShotEssence(&typedEssence->common);
    typedEssence->common.createFunc = _CreateBeam;
    typedEssence->common.life = 99.0f;
    typedEssence->speed = 40.0f;
    typedEssence->duration = 2.0f;
}

void *CreateRgShotFromEssence(RgShotEssence *essence, void *info)
{
    if (essence == 0) {
        assert_prog(D_00A53390, D_00A532F8, 0x4AB);
    }
    return essence->createFunc(essence, info);
}

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
