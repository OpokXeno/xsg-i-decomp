/*
 * OV12 original TU 23: 0x00a15600..0x00a17c90 (63 functions)
 */
#include "common.h"
#include "shared.h"
#include "rg_shot.h"

extern void assert_prog(const char *expression, const char *source_file,
                        int line);

const char D_00A532E8[16] = "pShot != NIL";

const char D_00A532F8[24] = "../rg_shot.euc.c";

const char D_00A53310[16] = "pGeom != NIL";

const char D_00A53338[16] = "pExtInfo != NIL";

const char D_00A53348[16] = "pEssence != NIL";

const char D_00A53358[16] = "pInfo != NIL";

const char D_00A53368[16] = "pBom != NIL";

const char D_00A53378[8] = "tama";

const char D_00A53380[16] = "eff11.ptcl";

const char D_00A53390[16] = "pEss != NIL";

const char D_00A533A0[16] = "pFire != NIL";

const char D_00A533C8[16] = "pBeam != NIL";

const char D_00A533D8[16] = "pRob != NIL";

const char D_00A533E8[16] = "pBgObj != NIL";

extern void *RgCharAlloc(unsigned int size, int type);
static void _InitBeam(void *beam, RgBeamEssence *essence, RgShotRequest *info);
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
                                             const int *color);
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

static void _ShotCommonPassTimeLife(RgShot *shot, float deltaTime)
{
    float remainingLife;

    remainingLife = shot->life;
    if (remainingLife == 1e8f) {
        return;
    }
    remainingLife -= deltaTime;
    shot->life = remainingLife;
    if (remainingLife < 0.0f) {
        if (shot->hitHistory != 0) {
            shot->hitHistory();
        } else {
            RgCharFree((RgChar *)shot);
            return;
        }
    } else {
        return;
    }
    XrgSoundRingVol(shot->soundDriver, shot->noLifeSoundId,
                    shot->noLifeSoundVolume);
}

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

const char D_00A53320[24] = "pShotGroup != NIL";

extern RgShotEffect *CreateRgShotEffect(void);
extern void RgCharControlMethod(RgChar *pChar,
                                void (*controlMethod)(RgChar *pChar));
extern void RgCharDispMethod(RgChar *pChar,
                             void (*dispMethod)(RgChar *pChar));
extern void RgCharDestructMethod(RgChar *pChar,
                                 void (*destructMethod)(RgChar *pChar));
extern RgGeomBall *CreateRgGeomBall(float radius, float weight);
struct RgGeomGroup;
extern RgGeom *RgGeomGroupCreateBall(struct RgGeomGroup *pGroup, void *parent);
/* Defined later in this TU (a local sibling still in asm). */
static void _ShotCommonHitBg(RgShot *shot, int bgObject, RgVector position);
static void _ShotCommonHitRobot(RgShot *shot, int robot, RgVector position);

static void _InitShotCommonSub(RgShot *shot, int id)
{
    if (id == 0) {
        assert_prog(D_00A53320, D_00A532F8, 242);
    }
    RgCharControlMethod((RgChar *) shot, (void (*)(RgChar *)) _ShotCommonControl);
    RgCharDispMethod((RgChar *) shot, (void (*)(RgChar *)) _ShotCommonDisp);
    RgCharDestructMethod((RgChar *) shot, (void (*)(RgChar *)) _ShotCommonDestruct);
    RgCharPassTimeMethod((RgChar *) shot,
                         (void (*)(RgChar *, float)) _ShotCommonPassTime);
    shot->notifyHit = (void (*)(RgShot *, RgVector)) _ShotCommonHit;
    shot->hitRobot = _ShotCommonHitRobot;
    shot->hitBg = _ShotCommonHitBg;
    shot->isReleased = _ShotCommonIsReleased;
    shot->hitHistory = 0;
    shot->essence = 0;
    shot->life = 0.0f;
    shot->damage = 0.0f;
    shot->hitRobotCount = 0;
    shot->effect = CreateRgShotEffect();
    if (id != 0) {
        shot->geomTray = RgGeomGroupCreateBall((struct RgGeomGroup *) id, shot);
    } else {
        shot->geomTray = (RgGeom *) CreateRgGeomBall(0.5f, 1.0f);
    }
    RgGeomBallSetRadius((RgGeomBall *) shot->geomTray, 0.5f);
    RgGeomResetStatus(shot->geomTray, 16);
    shot->soundDriver = 0;
    shot->hideSoundId = 0;
    shot->noLifeSoundVolume = 128;
    shot->hitSoundId = 0;
    shot->bgHitSoundId = 0;
    shot->noLifeSoundId = 0;
    do {
        shot->hitSoundVolume = 128;
        shot->bgHitSoundVolume = 128;
    } while (0);
}

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

static void _InitRgShotEssence(RgShotEssence *essence)
{
    essence->life = 0.0f;
    essence->texLineWidth = 1.0f;
    essence->modelVariant[0] = '\0';
    essence->particleFile[0] = '\0';
    essence->texLineHeight = 0.2f;
    essence->texLineFile[0] = '\0';
    essence->texLineColor[2] = 0x80;
    essence->texLineColor[1] = 0x80;
    essence->texLineColor[0] = 0x80;
    essence->texLineColor[3] = 0x7f;
    essence->damage = 5.0f;
    essence->hitEffectFile[0] = '\0';
    essence->damageParticleFile[0] = '\0';
    essence->notDamLimit = 0.0f;
}

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

static int _GrenadeEntryHistory(RgShot *shot, int bgObject)
{
    RgGrenadeShot *typedShot;
    unsigned int i;

    typedShot = (RgGrenadeShot *) shot;
    if (shot == 0) {
        assert_prog(D_00A53368, D_00A532F8, 522);
    }
    if (bgObject == 0) {
        return 0;
    }
    if (typedShot->historyCount >= 8) {
        return 0;
    }
    for (i = 0; i < typedShot->historyCount; i++) {
        if (typedShot->bgObjectHistory[i] == bgObject) {
            return 0;
        }
    }
    {
        unsigned int *historyCountPtr;
        unsigned int count;

        historyCountPtr = &typedShot->historyCount;
        count = *historyCountPtr;
        *historyCountPtr = count + 1;
        typedShot->bgObjectHistory[count] = bgObject;
    }
    return 1;
}

const char D_00A533F8[16] = "pRobot != NIL";

const char D_00A53408[16] = "eff06.ptcl";

extern void RgShotEffectSetScale(RgShotEffect *effect, float scale);
extern void RgShotEffectSetYRot(RgShotEffect *effect, float yRotation);
extern void RgShotEffectSetTransparent(RgShotEffect *effect, float transparent);
extern void RgShotEffectSetAddAlpha(RgShotEffect *effect);

static void _GrenadeBomPassTime(RgShot *shot, float deltaTime)
{
    RgGrenadeShot *typedShot;
    RgGeom *geom;
    float twiceElapsed;
    float factor;
    float radius;
    float angle;
    float alpha;
    float fadeWindow;
    float maxTransparency;
    float halfTurn;

    typedShot = (RgGrenadeShot *) shot;
    if (shot == 0) {
        assert_prog(D_00A53368, D_00A532F8, 542);
    }
    geom = _CommonGetGeom(shot);
    _ShotCommonPassTime(shot, deltaTime);
    typedShot->elapsedTime = typedShot->elapsedTime + deltaTime;
    if (typedShot->duration < typedShot->elapsedTime) {
        RgGeomResetStatus(geom, 1);
        shot->life = 0.0f;
    } else {
        twiceElapsed = typedShot->elapsedTime + typedShot->elapsedTime;
        if (twiceElapsed < 0.0f) {
            factor = 0.0f;
        } else if (typedShot->duration < twiceElapsed) {
            factor = typedShot->duration;
        } else {
            factor = twiceElapsed;
        }
        maxTransparency = 0.9f;
        fadeWindow = 0.5f;
        radius = typedShot->explosionRadius * factor / typedShot->duration;
        RgGeomBallSetRadius((RgGeomBall *) geom, radius);
        RgShotEffectSetScale(shot->effect, radius * 1.5f);

        angle = typedShot->spinAngle + 0.19634954f;
        halfTurn = 3.14159265f;
        if (halfTurn < angle) {
            do {
                angle -= 6.28318531f;
            } while (halfTurn < angle);
        }
        if (angle < -3.14159265f) {
            do {
                angle += 6.28318531f;
            } while (angle < -3.14159265f);
        }
        typedShot->spinAngle = angle;
        RgShotEffectSetYRot(shot->effect, angle);

        if (0.0f < (float) typedShot->repeatCount) {
            RgShotEffectSetAddAlpha(shot->effect);
            RgShotEffectSetTransparent(shot->effect, 0.1f);
            XrgSoundRingVol(shot->soundDriver, shot->hitSoundId,
                            shot->hitSoundVolume);
        } else {
            RgShotEffectSetAddAlpha(shot->effect);
            if (typedShot->duration < typedShot->elapsedTime + fadeWindow) {
                alpha = (typedShot->duration - typedShot->elapsedTime)
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

extern RgGeomPoint *RgRobotGetGeom(RgStatus *pRobot);
extern unsigned int RgRobotGetStatusFlags(RgStatus *pRobot);
extern void RgRobotSetConfuse(RgStatus *pRobot, float confuse);
extern void RgGeomPointAddForce(RgGeomPoint *point, RgVector force);

static void _GrenadeHitRobot(RgShot *shot, int robot, RgVector position)
{
    RgGrenadeShot *typedShot;
    RgGeomPoint *robotGeom;
    RgVector robotPosition;
    RgVector force;

    typedShot = (RgGrenadeShot *) shot;
    if (_GrenadeEntryHistory(shot, robot) == 0) {
        return;
    }
    if ((float) typedShot->repeatCount <= 0.0f) {
        _ShotCommonHitRobot(shot, robot, position);
        robotGeom = RgRobotGetGeom((RgStatus *) robot);
        __RgGeomPointGetPos(robotGeom, robotPosition, D_00A532F8, 638);
        __RgGeomPointGetPos((RgGeomPoint *) _CommonGetGeom(shot), force,
                            D_00A532F8, 639);
        XrgSubVector(force, robotPosition, force);
        force[1] = 0.0f;
        XrgNormalizeVector(force, force);
        XrgScaleVector(force, force, 1.5e6f);
        RgGeomPointAddForce(robotGeom, force);
    } else {
        if ((RgRobotGetStatusFlags((RgStatus *) robot) & 0x80) != 0) {
            return;
        }
        RgRobotSetConfuse((RgStatus *) robot, (float) typedShot->repeatCount);
    }
}

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

static void _GrenadeHitRobot(RgShot *shot, int robot, RgVector position);

static void _InitGrenade(void *shot, void *essence, void *info)
{
    RgGrenadeShot *typedShot;
    RgGrenadeEssence *typedEssence;

    typedShot = shot;
    typedEssence = essence;
    _InitHoming(shot, essence, info);
    RgCharPassTimeMethod((RgChar *) shot,
                         (void (*)(RgChar *, float)) _GrenadeHomingPassTime);
    typedShot->explosionRadius = typedEssence->explosionRadius;
    typedShot->proximity = typedEssence->proximity;
    typedShot->common.common.notifyHit = _GrenadeHomingHit;
    typedShot->duration = typedEssence->duration;
    typedShot->elapsedTime = 0.0f;
    typedShot->spinAngle = 0.0f;
    typedShot->common.common.hitRobot = _GrenadeHitRobot;
    typedShot->common.common.hitBg = _GrenadeHitBg;
    typedShot->common.common.hitHistory = (void (*)(void)) _GrenadeShotZeroLife;
    typedShot->historyCount = 0;
    typedShot->repeatCount = (unsigned int) typedEssence->repeatCount;
}

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

extern void RgGeomSetStatus(RgGeom *geom, unsigned int flags);
extern void RgError(const char *message, const char *source_file, int line, ...);
extern const char D_00A533B0[24];

static void _FirePassTime(RgShot *shot, float deltaTime)
{
    RgFireShot *typedShot;
    RgGeom *geom;
    RgVector position;
    RgVector direction;
    unsigned int status;
    float *originPos;
    float *tipPos;

    typedShot = (RgFireShot *) shot;
    if (shot == 0) {
        assert_prog(D_00A533A0, D_00A532F8, 831);
    }
    geom = _CommonGetGeom(shot);
    _ShotCommonPassTimeGeom(shot, deltaTime);
    _ShotCommonPassTimeLife(shot, deltaTime);
    status = typedShot->status;
    switch (status) {
    default:
        RgError(D_00A533B0, D_00A532F8, 876, status);
        originPos = typedShot->origin;
        tipPos = typedShot->tipPosition;
        break;
    case 0:
        RgGeomSetStatus(geom, 1);
        typedShot->status = 1;
        /* fall through */
    case 1:
        shot->damage = typedShot->damageRate * deltaTime;
        if (_GetShotPosDir((RgShot *) &typedShot->request, position,
                           direction) != 0) {
            originPos = typedShot->origin;
            XrgCopyVector(originPos, position);
            XrgNormalizeVector(direction, direction);
            XrgScaleVector(direction, direction, typedShot->speed);
            tipPos = typedShot->tipPosition;
            XrgAddVector(tipPos, position, direction);
        } else {
            _FireKill(shot);
        }
        originPos = typedShot->origin;
        tipPos = typedShot->tipPosition;
        typedShot->remainingTime = typedShot->remainingTime - deltaTime;
        if (typedShot->remainingTime < 0.0f) {
            _FireKill(shot);
        }
        break;
    case 2:
        RgGeomResetStatus(geom, 1);
        typedShot->fadeTime = typedShot->fadeTime - deltaTime;
        if (typedShot->fadeTime < 0.0f) {
            RgCharFree((RgChar *) shot);
            originPos = typedShot->origin;
            tipPos = typedShot->tipPosition;
        } else {
            originPos = typedShot->origin;
            tipPos = typedShot->tipPosition;
        }
        break;
    }
    RgGeomPointSetPos((RgGeomPoint *) geom, originPos);
    RgGeomPointMovePos((RgGeomPoint *) geom, tipPos);
    RgShotEffectSetPos(shot->effect, originPos, tipPos);
    RgShotEffectPassTime(shot->effect, deltaTime);
}

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

static void _FireHitRobot(RgShot *shot, int robot, RgVector position)
{
    RgFireShot *fire = (RgFireShot *)shot;

    if (fire->robot == 0 || fire->alreadyHitRobot == 0) {
        _ShotCommonHitRobot(shot, robot, position);
    }
    fire->alreadyHitRobot = 1;
}

extern void RgShotEffectSetParticleShootReverse(RgShotEffect *effect);
static void _FirePassTime(RgShot *shot, float deltaTime);

static void _InitFire(void *fire, void *essence, void *info)
{
    RgShot *shot;
    RgFireShot *typedShot;
    RgFireEssence *typedEssence;
    RgShotRequest *typedInfo;

    shot = fire;
    typedShot = fire;
    typedEssence = essence;
    typedInfo = info;
    _InitShotCommon(fire, essence, info, 0.0f);
    shot->notifyHit = _FireHit;
    shot->isReleased = _FireIsReleased;
    shot->release = _FireRelease;
    shot->hitRobot = _FireHitRobot;
    RgGeomBallSetRadius((RgGeomBall *) shot->geomTray, 1.0f);
    RgShotEffectSetParticleShootReverse(shot->effect);
    XrgClearVector(typedShot->origin);
    XrgClearVector(typedShot->tipPosition);
    typedShot->status = 0;
    typedShot->speed = typedEssence->speed;
    typedShot->remainingTime = typedEssence->duration;
    typedShot->damageRate = shot->damage;
    typedShot->quickFade = typedEssence->trailingCount;
    typedShot->robot = typedEssence->robot;
    typedShot->alreadyHitRobot = 0;
    shot->damage = 0.0f;
    shot->life = 1e8f;
    RgCharPassTimeMethod((RgChar *) shot,
                         (void (*)(RgChar *, float)) _FirePassTime);
    typedShot->request.posOwner = typedInfo->posOwner;
    typedShot->request.getPos = typedInfo->getPos;
    typedShot->request.target = typedInfo->target;
    typedShot->request.id = typedInfo->id;
}

static void _InitFire(void *fire, void *essence, void *info);

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

static void _InitBeam(void *beam, RgBeamEssence *essence, RgShotRequest *info)
{
    RgShot *shot;
    RgBeamShot *typedBeam;
    RgVector origin;
    RgVector direction;
    float damageRate;
    float lifetime;

    shot = beam;
    typedBeam = beam;
    _InitShotCommon(beam, essence, info, essence->speed);
    shot->notifyHit = _BeamHit;
    shot->isReleased = _BeamIsReleased;
    shot->release = _BeamRelease;
    RgGeomBallSetRadius((RgGeomBall *) shot->geomTray, 1.0f);
    XrgCopyVector(typedBeam->origin, origin);
    XrgCopyVector(typedBeam->direction, direction);
    typedBeam->speed = essence->speed;
    typedBeam->length = 0.0f;
    damageRate = shot->damage;
    lifetime = essence->duration;
    typedBeam->hit = 0;
    typedBeam->damageRate = damageRate;
    typedBeam->lifetime = lifetime;
    shot->damage = 0.0f;
    RgCharPassTimeMethod((RgChar *) shot,
                         (void (*)(RgChar *, float)) _BeamPassTime);
    ((RgShot *) &typedBeam->request)->posOwner = ((RgShot *) info)->posOwner;
    ((RgShot *) &typedBeam->request)->getPos = ((RgShot *) info)->getPos;
    typedBeam->request.target = info->target;
    typedBeam->request.id = info->id;
}

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

int RgShotIsReleased(RgShot *shot)
{
    if (shot == 0) {
        assert_prog(D_00A532E8, D_00A532F8, 1260);
    }
    if (shot->isReleased != 0) {
        return shot->isReleased(shot);
    }
    return 1;
}

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

extern void CreateRgHitEffectPosDir(RgVector position, RgVector direction,
                                    const char *name1, const char *name2);
extern int RgRobotIsInvalidAttack(RgStatus *pRobot, RgVector direction);
extern void RgRobotGiveDamage(RgStatus *pRobot, RgVector direction,
                              int damageType, float damage);
extern void RgRobotGiveWeakDamage(RgStatus *pRobot, RgVector direction,
                                  float damage, float threshold,
                                  float windowSeed, float weakHitMotion,
                                  float weakHitPower);
extern void RgRobotInvalidAttack(RgStatus *pRobot, RgVector direction,
                                 float scale);

static void _ShotCommonHitRobot(RgShot *shot, int robot, RgVector position)
{
    RgVector direction;
    RgVector robotPosition;

    if (shot == 0)
        assert_prog(D_00A532E8, D_00A532F8, 1324);
    if (robot == 0)
        assert_prog(D_00A533F8, D_00A532F8, 1325);
    __RgGeomPointGetOldPos((RgGeomPoint *) _CommonGetGeom(shot), direction,
                           D_00A532F8, 1328);
    __RgGeomPointGetPos(RgRobotGetGeom((RgStatus *) robot), robotPosition,
                        D_00A532F8, 1329);
    XrgSubVector(direction, direction, robotPosition);
    if (RgRobotIsInvalidAttack((RgStatus *) robot, direction) == 0) {
        if (shot->essence != 0 && 0.0f < shot->essence->notDamLimit) {
            RgRobotGiveWeakDamage((RgStatus *) robot, direction, shot->damage,
                                  shot->essence->notDamLimit,
                                  shot->essence->notDamTime,
                                  shot->essence->slowTime,
                                  shot->essence->slowRate);
        } else {
            RgRobotGiveDamage((RgStatus *) robot, direction, 1, shot->damage);
        }
        XrgSoundRingVol(shot->soundDriver, shot->hitSoundId,
                        shot->hitSoundVolume);
        CreateRgHitEffectPosDir(position, direction,
                                shot->essence->damageParticleFile, D_00A53408);
    } else {
        CreateRgHitEffectPosDir(position, direction,
                                shot->essence->hitEffectFile, D_00A53380);
        RgRobotInvalidAttack((RgStatus *) robot, direction, shot->damage);
    }
}

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
