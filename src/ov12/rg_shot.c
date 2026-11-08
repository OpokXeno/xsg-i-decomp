/*
 * OV12 original TU 23: 0x00a15600..0x00a17c90 (63 functions)
 */
#include "common.h"
#include "shared.h"
#include "ov12/assert_prog.h"
#include "ov12/rg_char.h"
#include "ov12/rg_geom_ball.h"
#include "ov12/rg_geom_point.h"
#include "ov12/rg_particle_effect.h"
#include "ov12/rg_robot.h"
#include "ov12/rg_shot.h"
#include "ov12/xrg_rand_int.h"
#include "ov12/rg_bgobj.h"
#include "ov12/rg_error.h"
#include "ov12/rg_geom_group.h"
#include "ov12/xrg_sound.h"
#include "ov12/create_rg.h"

/* RgChar type of every shot, and the size of each kind of shot. */
#define RG_SHOT_CHAR_TYPE 3
#define RG_NORMAL_SHOT_SIZE 0x70
#define RG_HOMING_SHOT_SIZE 0x90
#define RG_GRENADE_SHOT_SIZE 0xD0
#define RG_FIRE_SHOT_SIZE 0xE0
#define RG_BEAM_SHOT_SIZE 0xD0

const char shotAssertShotNotNil[16] = "pShot != NIL";

const char shotSourcePath[24] = "../rg_shot.euc.c";

const char shotAssertGeomNotNil[16] = "pGeom != NIL";

const char shotAssertExtInfoNotNil[16] = "pExtInfo != NIL";

const char shotAssertEssenceNotNil[16] = "pEssence != NIL";

const char shotAssertInfoNotNil[16] = "pInfo != NIL";

const char shotAssertBomNotNil[16] = "pBom != NIL";

const char shotGrenadeBomModelName[8] = "tama";

const char shotGrenadeHitEffectName[16] = "eff11.ptcl";

const char shotAssertEssNotNil[16] = "pEss != NIL";

const char shotAssertFireNotNil[16] = "pFire != NIL";

const char shotAssertBeamNotNil[16] = "pBeam != NIL";

const char shotAssertRobNotNil[16] = "pRob != NIL";

const char shotAssertBgObjNotNil[16] = "pBgObj != NIL";

extern const char shotUnknownFireStatusFormat[24];


static RgGeomBall *_CommonGetGeom(RgShot *shot);
static void _ShotCommonPassTime(RgShot *shot, float deltaTime);
static void _ShotCommonHitBg(RgShot *shot, struct RgBgObj *bgObject,
                             RgVector position);
static void _ShotCommonHitRobot(RgShot *shot, RgStatus *robot,
                                RgVector position);
static void _InitShotCommonSub(RgShot *shot, struct RgGeomGroup *shotGroup);
static void _InitRgShotEssence(RgShotEssence *essence);
static void _HomingPassTime(RgHomingShot *shot, float deltaTime);
static void _InitHoming(RgHomingShot *shot, RgHomingShotEssence *essence,
                        RgShotRequest *info);
static int _GrenadeEntryHistory(RgShot *shot, void *object);
static void _GrenadeBomPassTime(RgShot *shot, float deltaTime);
static void _GrenadeSetBom(RgShot *shot);
static void _GrenadeHitRobot(RgShot *shot, RgStatus *robot, RgVector position);
static RgShot *_CreateGrenade(RgShotEssence *essence, RgShotRequest *info);
static void _FireKill(RgShot *shot);
static void _FirePassTime(RgShot *shot, float deltaTime);
static void _InitFire(RgFireShot *fire, RgFireEssence *essence,
                      RgShotRequest *info);
static void _InitBeam(RgBeamShot *beam, RgBeamEssence *essence,
                      RgShotRequest *info);

/* Returns the ball geometry of a shot, asserting that the shot and its geometry exist. */
static RgGeomBall *_CommonGetGeom(RgShot *shot)
{
    RgGeomBall *geom;

    if (shot == 0) {
        assert_prog(shotAssertShotNotNil, shotSourcePath, 0x58);
    }
    geom = shot->geomTray;
    if (geom == 0) {
        assert_prog(shotAssertGeomNotNil, shotSourcePath, 0x5A);
    }
    return geom;
}

/*
 * Asks the request's position callback for the shot position and aim point.
 * Returns 0 when there is no owner or no callback.
 */
static int _GetShotPos2(RgShotRequest *request, RgVector position,
                        RgVector aimPoint)
{
    void *posOwner;

    XrgClearVector(position);
    XrgClearVector(aimPoint);
    posOwner = request->posOwner;
    if (posOwner == 0) {
        return 0;
    }
    if (request->getPos == 0) {
        return 0;
    }
    return request->getPos(posOwner, position, aimPoint);
}

/*
 * Gets the shot position and a normalized direction toward its aim point.
 * Returns 0 when no position is available.
 */
static int _GetShotPosDir(RgShotRequest *request, RgVector position,
                          RgVector direction)
{
    RgVector shotPosition;
    RgVector aimPoint;

    XrgClearVector(position);
    XrgClearVector(direction);
    if (_GetShotPos2(request, shotPosition, aimPoint) == 0) {
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
    RgGeomPassTime(&_CommonGetGeom(shot)->point.geom, deltaTime);
}

/*
 * Counts down the shot's life. When it runs out, the expire callback runs and the
 * no-life sound plays, or the shot is freed when it has no callback.
 */
static void _ShotCommonPassTimeLife(RgShot *shot, float deltaTime)
{
    float remainingLife;

    remainingLife = shot->life;
    if (remainingLife == RG_SHOT_LIFE_INFINITE) {
        return;
    }
    remainingLife -= deltaTime;
    shot->life = remainingLife;
    if (remainingLife < 0.0f) {
        if (shot->expire != 0) {
            shot->expire(shot);
        } else {
            RgCharFree(&shot->base);
            return;
        }
    } else {
        return;
    }
    XrgSoundRingVol(shot->soundDriver, shot->noLifeSoundId,
                    shot->noLifeSoundVolume);
}

/*
 * Moves the shot's effect between the geometry point's old and current positions,
 * then advances it.
 */
static void _ShotCommonPassTimeEffect(RgShot *shot, float deltaTime)
{
    RgVector position;
    RgVector oldPosition;

    __RgGeomPointGetPos(&_CommonGetGeom(shot)->point, position, shotSourcePath,
                        161);
    __RgGeomPointGetOldPos(&_CommonGetGeom(shot)->point, oldPosition,
                           shotSourcePath, 162);
    RgShotEffectSetPos(shot->effect, oldPosition, position);
    RgShotEffectPassTime(shot->effect, deltaTime);
}

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

/* Frees the shot's geometry and effect and stops its hide sound. */
static void _ShotCommonDestruct(RgShot *shot)
{
    if (shot->geomTray != 0) {
        RgGeomFree(&shot->geomTray->point.geom);
        shot->geomTray = 0;
    }
    if (shot->effect != 0) {
        DisposeRgShotEffect(shot->effect);
        shot->effect = 0;
    }
    XrgSoundRingStop(shot->soundDriver, shot->hideSoundId);
}

static void _ShotCommonHit(RgShot *shot, RgVector position)
{
    RgGeomResetStatus(&shot->geomTray->point.geom, RG_GEOM_STATUS_ACTIVE);
    RgCharFree(&shot->base);
}

static int _ShotCommonIsReleased(RgShot *shot)
{
    return 1;
}

const char shotAssertShotGroupNotNil[24] = "pShotGroup != NIL";

/*
 * Sets the common method pointers and default values of a shot and creates its
 * geometry ball (inside shotGroup when given) and its effect.
 */
static void _InitShotCommonSub(RgShot *shot, struct RgGeomGroup *shotGroup)
{
    if (shotGroup == 0) {
        assert_prog(shotAssertShotGroupNotNil, shotSourcePath, 242);
    }
    RgCharControlMethod(&shot->base, _ShotCommonControl);
    RgCharDispMethod(&shot->base, (RgCharDispFunc)_ShotCommonDisp);
    RgCharDestructMethod(&shot->base, (RgCharDestructFunc)_ShotCommonDestruct);
    RgCharPassTimeMethod(&shot->base,
                         (RgCharPassTimeFunc)_ShotCommonPassTime);
    shot->notifyHit = _ShotCommonHit;
    shot->hitRobot = _ShotCommonHitRobot;
    shot->hitBg = _ShotCommonHitBg;
    shot->isReleased = _ShotCommonIsReleased;
    shot->expire = 0;
    shot->essence = 0;
    shot->life = 0.0f;
    shot->damage = 0.0f;
    shot->hitRobotCount = 0;
    shot->effect = CreateRgShotEffect();
    if (shotGroup != 0) {
        shot->geomTray = RgGeomGroupCreateBall(shotGroup, shot);
    } else {
        shot->geomTray = CreateRgGeomBall(0.5f, 1.0f);
    }
    RgGeomBallSetRadius(shot->geomTray, 0.5f);
    RgGeomResetStatus(&shot->geomTray->point.geom, RG_GEOM_STATUS_PUSH_OUT);
    shot->soundDriver = 0;
    shot->hideSoundId = 0;
    shot->noLifeSoundVolume = RG_SHOT_SOUND_VOLUME_DEFAULT;
    shot->hitSoundId = 0;
    shot->bgHitSoundId = 0;
    shot->noLifeSoundId = 0;
    do {
        shot->hitSoundVolume = RG_SHOT_SOUND_VOLUME_DEFAULT;
        shot->bgHitSoundVolume = RG_SHOT_SOUND_VOLUME_DEFAULT;
    } while (0);
}

/*
 * Initializes the common part of a shot from its essence and request. The geometry
 * takes the request's position and a velocity along the aim direction scaled by speed.
 */
static void _InitShotCommon(RgShot *shot, RgShotEssence *essence,
                            RgShotRequest *extInfo, float speed)
{
    RgVector position;
    RgVector direction;

    if (shot == 0) {
        assert_prog(shotAssertShotNotNil, shotSourcePath, 290);
    }
    if (extInfo == 0) {
        assert_prog(shotAssertExtInfoNotNil, shotSourcePath, 291);
    }
    _InitShotCommonSub(shot, extInfo->shotGroup);
    shot->essence = essence;
    shot->life = essence->life;
    shot->damage = essence->damage;
    RgShotEffectSetModel(shot->effect, essence->modelVariant);
    RgShotEffectSetParticle(shot->effect, essence->particleFile);
    RgShotEffectSetTexLine(shot->effect, essence->texLineFile);
    RgShotEffectSetTexLineModelSize(shot->effect, essence->texLineWidth,
                                    essence->texLineHeight);
    RgShotEffectSetTexLineModelColor(shot->effect, &essence->texLineColor);
    if (_GetShotPosDir(extInfo, position, direction) != 0) {
        XrgScaleVector(direction, direction, speed);
        RgGeomPointSetPos(&shot->geomTray->point, position);
        RgGeomPointSetVel(&shot->geomTray->point, direction);
        return;
    }
    XrgClearVector(position);
}

/* Resets the common fields of a shot essence to their defaults. */
static void _InitRgShotEssence(RgShotEssence *essence)
{
    essence->life = 0.0f;
    essence->texLineWidth = 1.0f;
    essence->modelVariant[0] = '\0';
    essence->particleFile[0] = '\0';
    essence->texLineHeight = 0.2f;
    essence->texLineFile[0] = '\0';
    essence->texLineColor.b = 0x80;
    essence->texLineColor.g = 0x80;
    essence->texLineColor.r = 0x80;
    essence->texLineColor.a = 0x7f;
    essence->damage = 5.0f;
    essence->hitEffectFile[0] = '\0';
    essence->damageParticleFile[0] = '\0';
    essence->notDamLimit = 0.0f;
}

static RgShot *_CreateRgNormalShot(RgShotEssence *essence,
                                   RgShotRequest *info)
{
    RgShot *shot;

    shot = (RgShot *)RgCharAlloc(RG_NORMAL_SHOT_SIZE, RG_SHOT_CHAR_TYPE);
    if (shot == 0) {
        assert_prog(shotAssertShotNotNil, shotSourcePath, 348);
    }
    _InitShotCommon(shot, essence, info,
                    ((RgNormalShotEssence *)essence)->speed);
    return shot;
}

/* Sets up a normal shot essence with its create function and default speed. */
void InitRgNormalShotEssence(RgNormalShotEssence *essence)
{
    if (essence == 0) {
        assert_prog(shotAssertEssenceNotNil, shotSourcePath, 0x164);
    }
    _InitRgShotEssence(&essence->common);
    essence->common.createFunc = _CreateRgNormalShot;
    essence->speed = 45.0f;
}

/*
 * Advances a homing shot. When its delay has run out, it turns its velocity toward
 * the target, with a random offset when the target is confused.
 */
static void _HomingPassTime(RgHomingShot *shot, float deltaTime)
{
    RgVector targetPosition;
    RgVector ownPosition;
    RgVector ownVelocity;
    RgVector aimDirection;
    RgMatrix aimBasis;
    float delay;

    if (shot == 0) {
        assert_prog(shotAssertShotNotNil, shotSourcePath, 0x189);
    }
    _ShotCommonPassTime(&shot->common, deltaTime);
    delay = shot->delay - deltaTime;
    shot->delay = delay;
    if (delay < 0.0f) {
        __RgGeomPointGetPos(&_CommonGetGeom(&shot->common)->point,
                            ownPosition, shotSourcePath, 0x19B);
        RgGeomPointGetVel(&_CommonGetGeom(&shot->common)->point, ownVelocity);
        __RgGeomPointGetPos(shot->target, targetPosition, shotSourcePath, 0x19D);
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
            RgGeomPointSetVel(&_CommonGetGeom(&shot->common)->point,
                              ownVelocity);
        }
    }
}

/*
 * Initializes a homing shot's turn rate, cosine threshold, delay, speed and target
 * from its essence and request.
 */
static void _InitHoming(RgHomingShot *shot, RgHomingShotEssence *essence,
                        RgShotRequest *info)
{
    float turnRate;
    float cosThreshold;
    float delay;
    float speed;
    RgGeomPoint *target;
    int randomTargeting;

    _InitShotCommon(&shot->common, &essence->common, info, essence->speed);
    RgCharPassTimeMethod(&shot->common.base,
                         (RgCharPassTimeFunc)_HomingPassTime);
    turnRate = essence->turnRate;
    shot->turnRate = turnRate;
    target = info->target;
    randomTargeting = info->targetConfused;
    cosThreshold = essence->cosThreshold;
    shot->randomTargeting = randomTargeting;
    shot->cosThreshold = cosThreshold;
    shot->target = target;
    delay = essence->delay;
    shot->delay = delay;
    speed = essence->speed;
    shot->speed = speed;
}

/* Allocates and initializes a homing shot. */
static RgShot *_CreateHoming(RgShotEssence *essence, RgShotRequest *info)
{
    RgShot *shot;

    shot = (RgShot *)RgCharAlloc(RG_HOMING_SHOT_SIZE, RG_SHOT_CHAR_TYPE);
    if (shot == 0) {
        assert_prog(shotAssertShotNotNil, shotSourcePath, 467);
    }
    if (essence == 0) {
        assert_prog(shotAssertEssenceNotNil, shotSourcePath, 468);
    }
    if (info == 0) {
        assert_prog(shotAssertInfoNotNil, shotSourcePath, 469);
    }
    _InitHoming((RgHomingShot *)shot, (RgHomingShotEssence *)essence, info);
    return shot;
}

/* Sets up a homing shot essence with its create function and default values. */
void InitRgHomingShotEssence(RgHomingShotEssence *essence)
{
    float cosThreshold;

    if (essence == 0) {
        assert_prog(shotAssertEssenceNotNil, shotSourcePath, 479);
    }
    _InitRgShotEssence(&essence->common);
    essence->common.createFunc = _CreateHoming;
    essence->common.life = 2.0f;
    essence->speed = 30.0f;
    essence->turnRate = XRG_QUARTER_PI;
    cosThreshold = cosf(0.19634955f);
    essence->cosThreshold = cosThreshold;
    essence->delay = 0.3f;
}

/*
 * Adds object to the grenade's hit history. Returns 1 when it was added, or 0 when
 * it is null, already recorded or the history is full.
 */
static int _GrenadeEntryHistory(RgShot *shot, void *object)
{
    RgGrenadeShot *grenade;
    unsigned int i;

    grenade = (RgGrenadeShot *)shot;
    if (shot == 0) {
        assert_prog(shotAssertBomNotNil, shotSourcePath, 522);
    }
    if (object == 0) {
        return 0;
    }
    if (grenade->historyCount >= RG_GRENADE_HISTORY_MAX) {
        return 0;
    }
    for (i = 0; i < grenade->historyCount; i++) {
        if (grenade->hitHistory[i] == object) {
            return 0;
        }
    }
    {
        unsigned int *historyCountPtr;
        unsigned int count;

        historyCountPtr = &grenade->historyCount;
        count = *historyCountPtr;
        *historyCountPtr = count + 1;
        grenade->hitHistory[count] = object;
    }
    return 1;
}

const char shotAssertRobotNotNil[16] = "pRobot != NIL";

const char shotDamageParticleEffectName[16] = "eff06.ptcl";

/*
 * Advances an exploded grenade: its radius grows during the first half of its
 * duration and its effect fades near the end. Once the duration has passed, it
 * sets the shot's life to zero.
 */
static void _GrenadeBomPassTime(RgShot *shot, float deltaTime)
{
    RgGrenadeShot *grenade;
    RgGeomBall *geom;
    float twiceElapsed;
    float factor;
    float radius;
    float angle;
    float alpha;
    float fadeWindow;
    float maxTransparency;
    float halfTurn;

    grenade = (RgGrenadeShot *)shot;
    if (shot == 0) {
        assert_prog(shotAssertBomNotNil, shotSourcePath, 542);
    }
    geom = _CommonGetGeom(shot);
    _ShotCommonPassTime(shot, deltaTime);
    grenade->elapsedTime = grenade->elapsedTime + deltaTime;
    if (grenade->duration < grenade->elapsedTime) {
        RgGeomResetStatus(&geom->point.geom, RG_GEOM_STATUS_ACTIVE);
        shot->life = 0.0f;
    } else {
        /* The radius reaches its maximum at half the duration. */
        twiceElapsed = grenade->elapsedTime + grenade->elapsedTime;
        if (twiceElapsed < 0.0f) {
            factor = 0.0f;
        } else if (grenade->duration < twiceElapsed) {
            factor = grenade->duration;
        } else {
            factor = twiceElapsed;
        }
        maxTransparency = 0.9f;
        fadeWindow = 0.5f;
        radius = grenade->explosionRadius * factor / grenade->duration;
        RgGeomBallSetRadius(geom, radius);
        RgShotEffectSetScale(shot->effect, radius * 1.5f);

        angle = grenade->spinAngle + 0.19634954f;
        halfTurn = XRG_PI;
        if (halfTurn < angle) {
            do {
                angle -= XRG_TWO_PI;
            } while (halfTurn < angle);
        }
        if (angle < -XRG_PI) {
            do {
                angle += XRG_TWO_PI;
            } while (angle < -XRG_PI);
        }
        grenade->spinAngle = angle;
        RgShotEffectSetYRot(shot->effect, angle);

        /* A confused grenade is made nearly transparent; otherwise it fades out near the end. */
        if (0.0f < (float)grenade->confuseTime) {
            RgShotEffectSetAddAlpha(shot->effect);
            RgShotEffectSetTransparent(shot->effect, 0.1f);
            XrgSoundRingVol(shot->soundDriver, shot->hitSoundId,
                            shot->hitSoundVolume);
        } else {
            RgShotEffectSetAddAlpha(shot->effect);
            if (grenade->duration < grenade->elapsedTime + fadeWindow) {
                alpha = (grenade->duration - grenade->elapsedTime)
                        / fadeWindow * maxTransparency;
                if (alpha < 0.0f) {
                    alpha = 0.0f;
                } else if (maxTransparency < alpha) {
                    alpha = maxTransparency;
                }
                RgShotEffectSetTransparent(shot->effect, alpha);
            } else {
                RgShotEffectSetTransparent(shot->effect, 0.9f);
            }
        }
    }
}

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

/*
 * Handles a grenade hitting a robot. A grenade with no confuse time damages the
 * robot and pushes it horizontally away; otherwise it confuses the robot.
 */
static void _GrenadeHitRobot(RgShot *shot, RgStatus *robot, RgVector position)
{
    RgGrenadeShot *grenade;
    RgGeomPoint *robotGeom;
    RgVector robotPosition;
    RgVector force;

    grenade = (RgGrenadeShot *)shot;
    if (_GrenadeEntryHistory(shot, robot) == 0) {
        return;
    }
    if ((float)grenade->confuseTime <= 0.0f) {
        _ShotCommonHitRobot(shot, robot, position);
        robotGeom = RgRobotGetGeom(robot);
        __RgGeomPointGetPos(robotGeom, robotPosition, shotSourcePath, 638);
        __RgGeomPointGetPos(&_CommonGetGeom(shot)->point, force, shotSourcePath,
                            639);
        XrgSubVector(force, robotPosition, force);
        force[1] = 0.0f;
        XrgNormalizeVector(force, force);
        XrgScaleVector(force, force, 1.5e6f);
        RgGeomPointAddForce(robotGeom, force);
    } else {
        if ((RgRobotGetStatusFlags(robot) & 0x80) != 0) {
            return;
        }
        RgRobotSetConfuse(robot, (float)grenade->confuseTime);
    }
}

static void _GrenadeHitBg(RgShot *shot, struct RgBgObj *bgObject,
                          RgVector position)
{
    if (_GrenadeEntryHistory(shot, bgObject) != 0) {
        _ShotCommonHitBg(shot, bgObject, position);
    }
}

/*
 * Switches a grenade to its bomb state: it stops moving and its geometry collapses.
 * A hit effect is created at its position when its confuse time is set, and its
 * particle emission is stopped.
 */
static void _GrenadeSetBom(RgShot *shot)
{
    RgGrenadeShot *grenade;
    RgGeomBall *geomTray;
    RgVector velocity;
    RgVector position;
    float hitEffectTrigger;
    RgParticleEffect *particleEffect;

    grenade = (RgGrenadeShot *)shot;
    geomTray = shot->geomTray;
    RgCharPassTimeMethod(&shot->base, (RgCharPassTimeFunc)_GrenadeBomPassTime);
    shot->notifyHit = _GrenadeBomHit;
    shot->life = RG_SHOT_LIFE_INFINITE;
    shot->expire = 0;
    RgGeomBallSetRadius(geomTray, 0.0f);
    XrgClearVector(velocity);
    RgGeomPointSetVel(&geomTray->point, velocity);
    RgShotEffectSetModel(shot->effect, shotGrenadeBomModelName);
    hitEffectTrigger = (float)grenade->confuseTime;
    if (hitEffectTrigger > 0.0f) {
        __RgGeomPointGetPos(&geomTray->point, position, shotSourcePath, 693);
        CreateRgHitEffectPos(position, shot->essence->hitEffectFile,
                             shotGrenadeHitEffectName);
        RgShotEffectSetModel(shot->effect, shotGrenadeBomModelName);
    }
    particleEffect = RgShotEffectGetPtclEffect(shot->effect);
    if (particleEffect != 0) {
        RgParticleEffectStopShoot(particleEffect);
    }
}

/*
 * Advances a homing grenade, and switches it to the bomb state once it is within
 * its proximity distance of the target.
 */
static void _GrenadeHomingPassTime(RgGrenadeShot *bom, float deltaTime)
{
    RgVector targetPosition;
    RgVector diff;
    RgGeomPoint *target;

    if (bom == 0) {
        assert_prog(shotAssertBomNotNil, shotSourcePath, 710);
    }
    _HomingPassTime(&bom->common, deltaTime);
    if (bom->proximity > 0.0f) {
        target = bom->common.target;
        if (target != 0) {
            __RgGeomPointGetPos(target, targetPosition, shotSourcePath, 718);
            __RgGeomPointGetPos(&_CommonGetGeom(&bom->common.common)->point,
                                diff, shotSourcePath, 719);
            XrgSubVector(diff, diff, targetPosition);
            if (XrgLengthVector(diff) < bom->proximity) {
                _GrenadeSetBom(&bom->common.common);
            }
        }
    }
}

/*
 * Initializes a grenade's homing base, explosion radius, duration, proximity and
 * confuse time from its essence and request.
 */
static void _InitGrenade(RgGrenadeShot *shot, RgGrenadeEssence *essence,
                         RgShotRequest *info)
{
    _InitHoming(&shot->common, &essence->homing, info);
    RgCharPassTimeMethod(&shot->common.common.base,
                         (RgCharPassTimeFunc)_GrenadeHomingPassTime);
    shot->explosionRadius = essence->explosionRadius;
    shot->proximity = essence->proximity;
    shot->common.common.notifyHit = _GrenadeHomingHit;
    shot->duration = essence->duration;
    shot->elapsedTime = 0.0f;
    shot->spinAngle = 0.0f;
    shot->common.common.hitRobot = _GrenadeHitRobot;
    shot->common.common.hitBg = _GrenadeHitBg;
    shot->common.common.expire = _GrenadeShotZeroLife;
    shot->historyCount = 0;
    shot->confuseTime = (unsigned int)essence->confuseTime;
}

/* Allocates and initializes a grenade shot. */
static RgShot *_CreateGrenade(RgShotEssence *essence, RgShotRequest *info)
{
    RgShot *bom;

    bom = (RgShot *)RgCharAlloc(RG_GRENADE_SHOT_SIZE, RG_SHOT_CHAR_TYPE);
    if (bom == 0) {
        assert_prog(shotAssertBomNotNil, shotSourcePath, 769);
    }
    if (essence == 0) {
        assert_prog(shotAssertEssNotNil, shotSourcePath, 770);
    }
    if (info == 0) {
        assert_prog(shotAssertInfoNotNil, shotSourcePath, 771);
    }
    _InitGrenade((RgGrenadeShot *)bom, (RgGrenadeEssence *)essence, info);
    return bom;
}

/* Sets up a grenade essence on top of the homing defaults. */
void InitRgGrenadeEssence(RgGrenadeEssence *essence)
{
    InitRgHomingShotEssence(&essence->homing);
    essence->homing.common.createFunc = _CreateGrenade;
    essence->explosionRadius = 10.0f;
    essence->duration = 1.0f;
    essence->proximity = 0.0f;
    essence->confuseTime = 0.0f;
}

/* Starts a fire shot fading. With quickFade the fade time is zero, otherwise one second. */
static void _FireKill(RgShot *shot)
{
    RgFireShot *fire;

    if (shot == 0) {
        assert_prog(shotAssertFireNotNil, shotSourcePath, 813);
    }
    fire = (RgFireShot *)shot;
    fire->status = RG_FIRE_STATUS_FADE;
    if (fire->quickFade != 0) {
        fire->fadeTime = 0.0f;
    } else {
        fire->fadeTime = 1.0f;
    }
    RgShotEffectStopAlive(shot->effect, fire->fadeTime);
}

/*
 * Advances a fire shot through its start, burn and fade states, updating its origin
 * and tip. An unknown status raises an error.
 */
static void _FirePassTime(RgShot *shot, float deltaTime)
{
    RgFireShot *fire;
    RgGeomBall *geom;
    RgVector position;
    RgVector direction;
    unsigned int status;
    float *originPos;
    float *tipPos;

    fire = (RgFireShot *)shot;
    if (shot == 0) {
        assert_prog(shotAssertFireNotNil, shotSourcePath, 831);
    }
    geom = _CommonGetGeom(shot);
    _ShotCommonPassTimeGeom(shot, deltaTime);
    _ShotCommonPassTimeLife(shot, deltaTime);
    status = fire->status;
    /* Start, burn and fade states; the default case reports an unknown status. */
    switch (status) {
    default:
        RgError(shotUnknownFireStatusFormat, shotSourcePath, 876, status);
        originPos = fire->origin;
        tipPos = fire->tipPosition;
        break;
    case RG_FIRE_STATUS_START:
        RgGeomSetStatus(&geom->point.geom, RG_GEOM_STATUS_ACTIVE);
        fire->status = RG_FIRE_STATUS_BURN;
        /* fall through */
    case RG_FIRE_STATUS_BURN:
        shot->damage = fire->damageRate * deltaTime;
        if (_GetShotPosDir(&fire->request, position, direction) != 0) {
            originPos = fire->origin;
            XrgCopyVector(originPos, position);
            XrgNormalizeVector(direction, direction);
            XrgScaleVector(direction, direction, fire->speed);
            tipPos = fire->tipPosition;
            XrgAddVector(tipPos, position, direction);
        } else {
            _FireKill(shot);
        }
        originPos = fire->origin;
        tipPos = fire->tipPosition;
        fire->remainingTime = fire->remainingTime - deltaTime;
        if (fire->remainingTime < 0.0f) {
            _FireKill(shot);
        }
        break;
    case RG_FIRE_STATUS_FADE:
        RgGeomResetStatus(&geom->point.geom, RG_GEOM_STATUS_ACTIVE);
        fire->fadeTime = fire->fadeTime - deltaTime;
        if (fire->fadeTime < 0.0f) {
            RgCharFree(&shot->base);
            originPos = fire->origin;
            tipPos = fire->tipPosition;
        } else {
            originPos = fire->origin;
            tipPos = fire->tipPosition;
        }
        break;
    }
    /* The geometry and effect follow the origin and tip chosen by the state. */
    RgGeomPointSetPos(&geom->point, originPos);
    RgGeomPointMovePos(&geom->point, tipPos);
    RgShotEffectSetPos(shot->effect, originPos, tipPos);
    RgShotEffectPassTime(shot->effect, deltaTime);
}

/* Returns nonzero once a fire shot has reached its fade state. */
static int _FireIsReleased(RgShot *shot)
{
    RgFireShot *fire;

    if (shot == 0) {
        assert_prog(shotAssertFireNotNil, shotSourcePath, 896);
    }
    fire = (RgFireShot *)shot;
    return (unsigned int)fire->status >= RG_FIRE_STATUS_FADE;
}

/* Starts a fire shot fading early, unless it is already fading. */
static void _FireRelease(RgShot *shot)
{
    if (shot == 0) {
        assert_prog(shotAssertFireNotNil, shotSourcePath, 0x38D);
    }
    if (((RgFireShot *)shot)->status != RG_FIRE_STATUS_FADE) {
        _FireKill(shot);
    }
}

static void _FireHit(RgShot *shot, RgVector position)
{
}

/* Damages a robot on hit. A shot that hits only once skips robots it has already hit. */
static void _FireHitRobot(RgShot *shot, RgStatus *robot, RgVector position)
{
    RgFireShot *fire = (RgFireShot *)shot;

    if (fire->onceHit == 0 || fire->alreadyHitRobot == 0) {
        _ShotCommonHitRobot(shot, robot, position);
    }
    fire->alreadyHitRobot = 1;
}

/*
 * Initializes a fire shot's origin, tip, timing, speed and damage from its essence
 * and request.
 */
static void _InitFire(RgFireShot *fire, RgFireEssence *essence,
                      RgShotRequest *info)
{
    RgShot *shot;

    shot = &fire->common;
    _InitShotCommon(shot, &essence->common, info, 0.0f);
    shot->notifyHit = _FireHit;
    shot->isReleased = _FireIsReleased;
    shot->release = _FireRelease;
    shot->hitRobot = _FireHitRobot;
    RgGeomBallSetRadius(shot->geomTray, 1.0f);
    RgShotEffectSetParticleShootReverse(shot->effect);
    XrgClearVector(fire->origin);
    XrgClearVector(fire->tipPosition);
    fire->status = RG_FIRE_STATUS_START;
    fire->speed = essence->length;
    fire->remainingTime = essence->attachTime;
    fire->damageRate = shot->damage;
    fire->quickFade = essence->immDead;
    fire->onceHit = essence->onceHit;
    fire->alreadyHitRobot = 0;
    shot->damage = 0.0f;
    shot->life = RG_SHOT_LIFE_INFINITE;
    RgCharPassTimeMethod(&shot->base, (RgCharPassTimeFunc)_FirePassTime);
    fire->request.posOwner = info->posOwner;
    fire->request.getPos = info->getPos;
    fire->request.target = info->target;
    fire->request.shotGroup = info->shotGroup;
}

/* Allocates and initializes a fire shot. */
static RgShot *_CreateFire(RgShotEssence *essence, RgShotRequest *info)
{
    RgShot *fire;

    fire = (RgShot *)RgCharAlloc(RG_FIRE_SHOT_SIZE, RG_SHOT_CHAR_TYPE);
    if (essence == 0) {
        assert_prog(shotAssertEssNotNil, shotSourcePath, 0x3D3);
    }
    if (info == 0) {
        assert_prog(shotAssertInfoNotNil, shotSourcePath, 0x3D4);
    }
    _InitFire((RgFireShot *)fire, (RgFireEssence *)essence, info);
    return fire;
}

/* Sets up a fire shot essence with its create function and default timing. */
void InitRgFireEssence(RgFireEssence *essence)
{
    if (essence == 0) {
        assert_prog(shotAssertEssNotNil, shotSourcePath, 990);
    }
    _InitRgShotEssence(&essence->common);
    essence->common.createFunc = _CreateFire;
    essence->common.life = RG_SHOT_LIFE_INFINITE;
    essence->length = 20.0f;
    essence->attachTime = 2.0f;
    essence->immDead = 0;
}

/*
 * Advances a beam shot: it updates its damage, extends its length along the aim while
 * alive, and draws its geometry from origin to tip. A hit shortens the beam, and the
 * beam is freed once its length runs out.
 */
static void _BeamPassTime(RgBeamShot *beam, float deltaTime)
{
    RgVector movement;
    RgVector position;
    RgVector direction;
    RgGeomBall *geom;
    float damage;
    float remainingLifetime;
    float moveAmount;
    float travelled;
    float collapsedLength;

    if (beam == 0) {
        assert_prog(shotAssertBeamNotNil, shotSourcePath, 1034);
    }
    _ShotCommonPassTime(&beam->common, deltaTime);
    damage = beam->damageRate * deltaTime;
    remainingLifetime = beam->lifetime - deltaTime;
    moveAmount = beam->speed * deltaTime;
    beam->lifetime = remainingLifetime;
    beam->common.damage = damage;
    if (remainingLifetime > 0.0f) {
        if (_GetShotPosDir(&beam->request, position, direction) != 0) {
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
            RgCharFree(&beam->common.base);
            return;
        }
    }
    geom = _CommonGetGeom(&beam->common);
    XrgScaleVector(movement, beam->direction, beam->length);
    XrgAddVector(position, beam->origin, movement);
    RgGeomPointSetPos(&geom->point, beam->origin);
    RgGeomPointMovePos(&geom->point, position);
    RgShotEffectSetPos(beam->common.effect, beam->origin, position);
}

static int _BeamIsReleased(RgShot *shot)
{
    if (shot == 0) {
        assert_prog(shotAssertBeamNotNil, shotSourcePath, 0x44B);
    }
    return ((RgBeamShot *)shot)->lifetime <= 0.0f;
}

/* Ends a beam's lifetime when it hits something and marks it as hit. */
static void _BeamHit(RgShot *shot, RgVector position)
{
    RgBeamShot *beam;

    if (shot == 0) {
        assert_prog(shotAssertBeamNotNil, shotSourcePath, 0x454);
    }
    beam = (RgBeamShot *)shot;
    beam->lifetime = 0.0f;
    beam->hit = 1;
}

static void _BeamRelease(RgShot *shot)
{
    ((RgBeamShot *)shot)->lifetime = -0.5f;
}

/*
 * Initializes a beam shot's origin, direction, length, lifetime and damage from its
 * essence and request.
 */
static void _InitBeam(RgBeamShot *beam, RgBeamEssence *essence,
                      RgShotRequest *info)
{
    RgShot *shot;
    RgVector origin;
    RgVector direction;
    float damageRate;
    float lifetime;

    shot = &beam->common;
    _InitShotCommon(shot, &essence->common, info, essence->speed);
    shot->notifyHit = _BeamHit;
    shot->isReleased = _BeamIsReleased;
    shot->release = _BeamRelease;
    RgGeomBallSetRadius(shot->geomTray, 1.0f);
    XrgCopyVector(beam->origin, origin);
    XrgCopyVector(beam->direction, direction);
    beam->speed = essence->speed;
    beam->length = 0.0f;
    damageRate = shot->damage;
    lifetime = essence->attachTime;
    beam->hit = 0;
    beam->damageRate = damageRate;
    beam->lifetime = lifetime;
    shot->damage = 0.0f;
    RgCharPassTimeMethod(&shot->base, (RgCharPassTimeFunc)_BeamPassTime);
    beam->request.posOwner = info->posOwner;
    beam->request.getPos = info->getPos;
    beam->request.target = info->target;
    beam->request.shotGroup = info->shotGroup;
}

/* Allocates and initializes a beam shot. */
static RgShot *_CreateBeam(RgShotEssence *essence, RgShotRequest *info)
{
    RgShot *beam;

    beam = (RgShot *)RgCharAlloc(RG_BEAM_SHOT_SIZE, RG_SHOT_CHAR_TYPE);
    if (essence == 0)
        assert_prog(shotAssertEssNotNil, shotSourcePath, 1166);
    if (info == 0)
        assert_prog(shotAssertInfoNotNil, shotSourcePath, 1167);
    _InitBeam((RgBeamShot *)beam, (RgBeamEssence *)essence, info);
    return beam;
}

/* Sets up a beam shot essence with its create function and default speed. */
void InitRgBeamEssence(RgBeamEssence *essence)
{
    if (essence == 0) {
        assert_prog(shotAssertEssenceNotNil, shotSourcePath, 1177);
    }
    _InitRgShotEssence(&essence->common);
    essence->common.createFunc = _CreateBeam;
    essence->common.life = 99.0f;
    essence->speed = 40.0f;
    essence->attachTime = 2.0f;
}

/* Creates a shot through the create function stored in its essence. */
RgShot *CreateRgShotFromEssence(RgShotEssence *essence, RgShotRequest *info)
{
    if (essence == 0) {
        assert_prog(shotAssertEssNotNil, shotSourcePath, 0x4AB);
    }
    return essence->createFunc(essence, info);
}

void RgShotSetSoundDriver(RgShot *shot, XrgSound *driver)
{
    if (shot == 0)
        assert_prog(shotAssertShotNotNil, shotSourcePath, 1206);
    shot->soundDriver = driver;
}

void RgShotSetHitSound(RgShot *shot, int soundId, int volume)
{
    if (shot == 0)
        assert_prog(shotAssertShotNotNil, shotSourcePath, 1214);
    shot->hitSoundId = soundId;
    shot->hitSoundVolume = volume;
}

void RgShotSetBgHitSound(RgShot *shot, int soundId, int volume)
{
    if (shot == 0)
        assert_prog(shotAssertShotNotNil, shotSourcePath, 1223);
    shot->bgHitSoundId = soundId;
    shot->bgHitSoundVolume = volume;
}

void RgShotSetNoLifeSound(RgShot *shot, int soundId, int volume)
{
    if (shot == 0)
        assert_prog(shotAssertShotNotNil, shotSourcePath, 1232);
    shot->noLifeSoundId = soundId;
    shot->noLifeSoundVolume = volume;
}

void RgShotSetHideSound(RgShot *shot, int soundId)
{
    if (shot == 0)
        assert_prog(shotAssertShotNotNil, shotSourcePath, 1241);
    shot->hideSoundId = soundId;
}

float RgShotGetDamage(RgShot *shot)
{
    if (shot == 0)
        assert_prog(shotAssertShotNotNil, shotSourcePath, 1252);
    return shot->damage;
}

/*
 * Returns nonzero when the shot is released. A shot without a release check counts as
 * released.
 */
int RgShotIsReleased(RgShot *shot)
{
    if (shot == 0) {
        assert_prog(shotAssertShotNotNil, shotSourcePath, 1260);
    }
    if (shot->isReleased != 0) {
        return shot->isReleased(shot);
    }
    return 1;
}

int RgShotGetHitRobNum(RgShot *shot)
{
    if (shot == 0)
        assert_prog(shotAssertShotNotNil, shotSourcePath, 1271);
    return shot->hitRobotCount;
}

void RgShotRelease(RgShot *shot)
{
    if (shot == 0)
        assert_prog(shotAssertShotNotNil, shotSourcePath, 1283);
    if (shot->release != 0)
        shot->release(shot);
}

/* Counts a hit on a robot, then runs the shot's robot-hit and notify callbacks. */
void RgShotHitRobot(RgShot *shot, RgStatus *robot, RgVector position)
{
    if (shot == 0)
        assert_prog(shotAssertShotNotNil, shotSourcePath, 1292);
    if (robot == 0)
        assert_prog(shotAssertRobNotNil, shotSourcePath, 1293);
    shot->hitRobotCount++;
    if (shot->hitRobot != 0)
        shot->hitRobot(shot, robot, position);
    if (shot->notifyHit != 0)
        shot->notifyHit(shot, position);
}

/* Reports a hit on a background object to the shot's background-hit and notify callbacks. */
void RgShotHitBg(RgShot *shot, struct RgBgObj *bgObject, RgVector position)
{
    if (shot == 0)
        assert_prog(shotAssertShotNotNil, shotSourcePath, 1306);
    if (bgObject == 0)
        assert_prog(shotAssertBgObjNotNil, shotSourcePath, 1307);
    if (shot->hitBg != 0)
        shot->hitBg(shot, bgObject, position);
    if (shot->notifyHit != 0)
        shot->notifyHit(shot, position);
}

/*
 * Applies the shot's damage to a robot, or the invalid-attack reaction when the robot
 * rejects the direction, and creates the hit effect.
 */
static void _ShotCommonHitRobot(RgShot *shot, RgStatus *robot,
                                RgVector position)
{
    RgVector direction;
    RgVector robotPosition;

    if (shot == 0)
        assert_prog(shotAssertShotNotNil, shotSourcePath, 1324);
    if (robot == 0)
        assert_prog(shotAssertRobotNotNil, shotSourcePath, 1325);
    __RgGeomPointGetOldPos(&_CommonGetGeom(shot)->point, direction,
                           shotSourcePath, 1328);
    __RgGeomPointGetPos(RgRobotGetGeom(robot), robotPosition, shotSourcePath,
                        1329);
    XrgSubVector(direction, direction, robotPosition);
    if (RgRobotIsInvalidAttack(robot, direction) == 0) {
        if (shot->essence != 0 && 0.0f < shot->essence->notDamLimit) {
            RgRobotGiveWeakDamage(robot, direction, shot->damage,
                                  shot->essence->notDamLimit,
                                  shot->essence->notDamTime,
                                  shot->essence->slowTime,
                                  shot->essence->slowRate);
        } else {
            RgRobotGiveDamage(robot, direction, 1, shot->damage);
        }
        XrgSoundRingVol(shot->soundDriver, shot->hitSoundId,
                        shot->hitSoundVolume);
        CreateRgHitEffectPosDir(position, direction,
                                shot->essence->damageParticleFile, shotDamageParticleEffectName);
    } else {
        CreateRgHitEffectPosDir(position, direction,
                                shot->essence->hitEffectFile, shotGrenadeHitEffectName);
        RgRobotInvalidAttack(robot, direction, shot->damage);
    }
}

/* Breaks a background object by the shot's damage, then plays its hit sound and effect. */
static void _ShotCommonHitBg(RgShot *shot, struct RgBgObj *bgObject,
                             RgVector position)
{
    if (shot == 0)
        assert_prog(shotAssertShotNotNil, shotSourcePath, 1365);
    if (bgObject == 0)
        assert_prog(shotAssertBgObjNotNil, shotSourcePath, 1366);
    if (RgBgObjTryToBreak(bgObject, shot->damage) != 0)
        XrgSoundRingVol(shot->soundDriver, shot->bgHitSoundId,
                        shot->bgHitSoundVolume);
    CreateRgHitEffectPos(position, shot->essence->hitEffectFile, shotGrenadeHitEffectName);
}



const char shotUnknownFireStatusFormat[24] = "unknown fire status %d";
