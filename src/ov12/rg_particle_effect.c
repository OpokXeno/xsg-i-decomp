#include "common.h"
#include "shared.h"
#include "ov12/assert_prog.h"
#include "ov12/rg_heap.h"
#include "ov12/rg_bxx.h"
#include "ov12/rg_effect_env.h"
#include "ov12/rg_particle_effect.h"
#include "ov12/xrg_rand_int.h"
#include "ov12/rg_error.h"

/* The size of the block CreateRgParticleEffect allocates. */
#define RG_PARTICLE_EFFECT_SIZE 0x470
/* Size of the buffer that RgParticleEffectEssenceSerialize clears before it copies an essence. */
#define RG_PARTICLE_ESSENCE_SLOT_SIZE 0x100



/* Assertion expressions, source file and error formats used below. */
const char particleEffectAssertEffectNotNil[16] = "pEff != NIL";

const char particleEffectSourcePath[32] = "../rg_particle_effect.euc.c";

const char particleEffectAssertEssenceArrayNotNil[16] = "aEss != NIL";

const char particleEffectAssertEssenceNotNil[16] = "pEss != NIL";

const char particleEffectAssertDataNotNil[16] = "pData != NIL";

const char particleEffectAssertSourceNotNil[16] = "pSrc != NIL";

const char particleEffectAssertCharNotNil[16] = "pChar != NIL";

/* Copies an essence's timing, life and appearance into a shoot and resets its emission state. */
static void _InitShoot(RgParticleShoot *shoot, RgParticleEffectEssence *essence)
{
    shoot->interval = essence->interval;
    shoot->duration = essence->duration;
    shoot->speed = essence->speed;
    shoot->lifeMax = shoot->life = essence->life;
    XrgCopyVector(shoot->appearance, essence->appearance);
    XrgCopyVector(shoot->appearanceRate, essence->appearanceRate);
    shoot->cursor = 0;
    shoot->emitRemainder = 0;
    XrgUnitMatrix(shoot->localMatrix);
    shoot->hasPrevious = 0;
    XrgUnitMatrix(shoot->previousMatrix);
    XrgClearVector(shoot->inertia);
}

static inline int _MaxInt(int a, int b)
{
    return (a >= b) ? a : b;
}

/*
 * Emits the particles of one shot. An opening shot emits its full count once its duration
 * has run out; a running shot emits from duration * deltaTime plus the carried remainder.
 * Particles are spread along the segment between the previous and the current origin.
 */
static void _ShootParticles(RgParticleShot *shot, float deltaTime, int opening)
{
    RgParticleShoot *shoot;
    XrgParticle *end;
    unsigned int count;
    XrgParticle *cursor;
    RgMatrix matrix;
    float *localMatrix;
    RgVector base0;
    RgVector base1;
    RgVector inertia;
    RgVector point;
    RgVector dir;
    RgVector zero;
    RgVector appearance;
    RgVector appearanceRate;
    float pitchRange;
    float yawRange;
    float piBound;
    float hiMin;
    float twoPi;
    float emitted;
    float ret;
    float invCount;
    float interval;
    float scale;
    unsigned int requested;
    int intRet;

    end = shot->particles + shot->particleCount;
    shoot = &shot->shoot;
    /* Opening shots emit once; running shots emit for the elapsed time. */
    if (opening != 0) {
        if (shoot->duration > 0.0f) {
            return;
        }
        count = shot->particleCount;
    } else {
        shoot->life = shoot->life - deltaTime;
        if (shoot->life < 0.0f) {
            return;
        }
        emitted = shoot->duration * deltaTime + shoot->emitRemainder;
        ret = floorf(emitted);
        shoot->emitRemainder = emitted - ret;
        if (ret <= 0.5f) {
            return;
        }
        intRet = (int)ret;
        count = _MaxInt(intRet, 1);
    }
    XrgCopyVector(inertia, shoot->inertia);
    if (shoot->cursor == 0) {
        shoot->cursor = shot->particles;
    }
    cursor = shoot->cursor;
    piBound = XRG_PI;
    pitchRange = shoot->speed;
    if (pitchRange < 0.0f) {
        pitchRange = 0.0f;
    } else if (pitchRange > piBound) {
        pitchRange = piBound;
    }
    yawRange = shoot->speed;
    hiMin = 0.0f;
    twoPi = XRG_TWO_PI;
    if (yawRange < hiMin) {
        yawRange = hiMin;
    } else if (yawRange > twoPi) {
        yawRange = twoPi;
    }
    localMatrix = shoot->localMatrix;
    XrgCopyMatrix(matrix, localMatrix);
    XrgUnitVector(&matrix[12]);
    XrgCopyVector(base0, &localMatrix[12]);
    if (shoot->hasPrevious != 0) {
        XrgCopyVector(base1, &shoot->previousMatrix[12]);
    } else {
        XrgCopyVector(base1, base0);
        shoot->hasPrevious = 1;
    }
    XrgCopyMatrix(shoot->previousMatrix, localMatrix);
    invCount = 1.0f / (float)(unsigned int)count;
    requested = count;
    count = count - 1;
    if (requested != 0) {
        do {
            XrgLinearIntpVector(point, base0, base1, (float)count * invCount);
            XrgRandVectorRange(dir, pitchRange, yawRange);
            interval = shoot->interval;
            scale = XrgRand(interval * 0.5f, interval);
            XrgScaleVectorXYZ(dir, dir, scale);
            __asm__ __volatile__(
                "lqc2 $vf2, 0(%2)\n\t"
                "lqc2 $vf3, 0(%1)\n\t"
                "lqc2 $vf4, 16(%1)\n\t"
                "lqc2 $vf5, 32(%1)\n\t"
                "lqc2 $vf6, 48(%1)\n\t"
                "vmulax.xyzw ACCxyzw, vf3xyzw, vf2x\n\t"
                "vmadday.xyzw ACCxyzw, vf4xyzw, vf2y\n\t"
                "vmaddaz.xyzw ACCxyzw, vf5xyzw, vf2z\n\t"
                "vmaddw.xyzw vf2xyzw, vf6xyzw, vf2w\n\t"
                "sqc2 $vf2, 0(%0)\n\t"
                : : "r"(dir), "r"(matrix), "r"(dir) : "memory");
            XrgAddVectorXYZ(dir, dir, inertia);
            XrgClearVector(zero);
            XrgCopyVector(appearance, shoot->appearance);
            XrgCopyVector(appearanceRate, shoot->appearanceRate);
            appearance[2] = appearance[2] * XRG_TWO_PI;
            appearance[3] = appearance[3] * XRG_TWO_PI;
            if (appearanceRate[2] != 0.0f) {
                appearance[2] = XrgRand(0.0f, XRG_PI);
            }
            if (appearanceRate[3] != 0.0f) {
                appearance[3] = XrgRand(0.0f, XRG_PI);
            }
            XrgCopyVector(cursor->position.vec, point);
            XrgCopyVector(cursor->velocity.vec, dir);
            XrgCopyVector(cursor->acceleration.vec, zero);
            XrgCopyVector(cursor->appearance.vec, appearance);
            XrgCopyVector(cursor->appearanceRate.vec, appearanceRate);
            cursor++;
            if (cursor >= end) {
                cursor = shot->particles;
            }
        } while (count-- > 0);
    }
    shoot->cursor = cursor;
}

static void _ReInitShoot(RgParticleShoot *shoot)
{
    shoot->hasPrevious = 0;
    shoot->life = shoot->lifeMax;
    XrgClearVector(shoot->inertia);
}

const char particleEffectAssertBxxNotNil[16] = "pBxx != NIL";

const char particleEffectTooManyParticleTotalFormat[32] = "too many particle total (%d)";

const char particleEffectMissingTextureFormat[48] = "there is not texture named '%s' for particle";

/*
 * Sets up an effect from an essence array: one particle array shared by all shots, and
 * one shot per essence bound to its texture.
 */
static void _InitEffect(RgParticleEffect *effect, RgParticleEffectEssence *essence,
                        int essenceCount)
{
    RgBxx *bxx;
    unsigned int particleTotal;
    unsigned int i;
    RgParticleShot *shot;
    XrgParticle *particles;
    RgBxxPic *pic;
    int repeat;

    bxx = RgEffectEnvGetBxx(InstanceOfRgEffectEnv());
    if (effect == 0) {
        assert_prog(particleEffectAssertEffectNotNil, particleEffectSourcePath, 246);
    }
    if (essence == 0) {
        assert_prog(particleEffectAssertEssenceArrayNotNil, particleEffectSourcePath, 247);
    }
    if (bxx == 0) {
        assert_prog(particleEffectAssertBxxNotNil, particleEffectSourcePath, 248);
    }
    effect->state = RG_PARTICLE_EFFECT_STATE_ACTIVE;
    particleTotal = 0;
    effect->shootActive = 1;
    InitRgParticleEffectControlData(&effect->controlData);
    for (i = 0; i < essenceCount; i++) {
        particleTotal += essence[i].particleCount;
    }
    effect->particleTotal = particleTotal;
    effect->shootCount = essenceCount;
    if (essenceCount == 0 || particleTotal == 0) {
        effect->particles = 0;
        return;
    }
    if (particleTotal > RG_PARTICLE_TOTAL_MAX) {
        RgError(particleEffectTooManyParticleTotalFormat, particleEffectSourcePath, 267, particleTotal);
    }
    effect->particles = CreateArrayOfXrgParticle(particleTotal);
    particles = effect->particles;
    for (i = 0; i < essenceCount; i++) {
        shot = &effect->shots[i];
        shot->particleCount = essence[i].particleCount;
        shot->particles = particles;
        pic = RgBxxGetPic(bxx, essence[i].textureName);
        shot->life = essence[i].shotLife;
        shot->started = 0;
        repeat = essence[i].repeat;
        shot->lifeMax = essence[i].shotLife;
        shot->pic = pic;
        shot->flags = 0;
        shot->resist = essence[i].resist;
        if (repeat) {
            shot->flags = RG_PARTICLE_SHOT_REPEAT;
        }
        if (shot->pic == 0) {
            RgError(particleEffectMissingTextureFormat, particleEffectSourcePath, 290, essence[i].textureName);
        }
        _InitShoot(&shot->shoot, &essence[i]);
        particles += essence[i].particleCount;
    }
}

static void _DestructEffect(RgParticleEffect *effect)
{
    if (effect == 0) {
        assert_prog(particleEffectAssertEffectNotNil, particleEffectSourcePath, 308);
    }
    if (effect->particles != 0) {
        DisposeArrayOfXrgParticle(effect->particles);
    }
}

/* Allocates an effect and initializes it from the essence array. */
RgParticleEffect *CreateRgParticleEffect(RgParticleEffectEssence *essence,
                                         int essenceCount)
{
    RgParticleEffect *effect;

    effect = RgHeapAlloc(InstanceOfRgHeap(), RG_PARTICLE_EFFECT_SIZE, particleEffectSourcePath,
                         322);
    if (effect == 0) {
        assert_prog(particleEffectAssertEffectNotNil, particleEffectSourcePath, 323);
    }
    if (essence == 0) {
        assert_prog(particleEffectAssertEssenceArrayNotNil, particleEffectSourcePath, 324);
    }
    _InitEffect(effect, essence, essenceCount);
    return effect;
}

void DisposeRgParticleEffect(RgParticleEffect *effect)
{
    if (effect == 0) {
        assert_prog(particleEffectAssertEffectNotNil, particleEffectSourcePath, 331);
    }
    _DestructEffect(effect);
    RgHeapFree(InstanceOfRgHeap(), effect, particleEffectSourcePath, 333);
}

/* Fills an essence with the default emission values. */
void InitRgParticleEffectEssence(RgParticleEffectEssence *essence)
{
    if (essence == 0) {
        assert_prog(particleEffectAssertEssenceNotNil, particleEffectSourcePath, 339);
    }
    essence->textureName[0] = '\0';
    essence->particleCount = 32;
    XrgClearVector(essence->appearance);
    XrgClearVector(essence->appearanceRate);
    essence->appearance[0] = 0.1f;
    essence->appearance[1] = 1.0f;
    essence->appearanceRate[0] = -0.1f;
    essence->duration = 0.0f;
    essence->appearanceRate[1] = -1.0f;
    essence->interval = 5.0f;
    XrgSetVectorXYZ(essence->unitDirection, 0.0f, 0.0f, 1.0f);
    essence->speed = 6.2831855f;
    essence->life = 0.0f;
    essence->shotLife = 5.0f;
    essence->repeat = 0;
    essence->resist = 0.0f;
}

/* Copies an essence into a slot-sized buffer, clearing the rest of the slot first. */
void RgParticleEffectEssenceSerialize(RgParticleEffectEssence *essence,
                                      RgParticleEffectEssence *destination)
{

    if (essence == 0) {
        assert_prog(particleEffectAssertEssenceNotNil, particleEffectSourcePath, 365);
    }
    memset(destination, 0, RG_PARTICLE_ESSENCE_SLOT_SIZE);
    *destination = *essence;
}

void RgParticleEffectEssenceResume(RgParticleEffectEssence *essence,
                                   RgParticleEffectEssence *source)
{
    if (essence == 0) {
        assert_prog(particleEffectAssertEssenceNotNil, particleEffectSourcePath, 375);
    }
    *essence = *source;
}

void RgParticleEffectSetShootLocal(RgParticleEffect *effect, RgMatrix local)
{
    unsigned int i;

    if (effect == 0) {
        assert_prog(particleEffectAssertEffectNotNil, particleEffectSourcePath, 408);
    }
    for (i = 0; i < effect->shootCount; i++) {
        XrgCopyMatrix(effect->shots[i].shoot.localMatrix, local);
    }
}

void RgParticleEffectSetShootInertia(RgParticleEffect *effect, RgVector inertia)
{
    unsigned int i;

    if (effect == 0) {
        assert_prog(particleEffectAssertEffectNotNil, particleEffectSourcePath, 421);
    }
    for (i = 0; i < effect->shootCount; i++) {
        XrgCopyVector(effect->shots[i].shoot.inertia, inertia);
    }
}

void RgParticleEffectStopShoot(RgParticleEffect *effect)
{
    if (effect == 0) {
        assert_prog(particleEffectAssertEffectNotNil, particleEffectSourcePath, 432);
    }
    effect->shootActive = 0;
}

void RgParticleEffectStopAlive(RgParticleEffect *effect, float time)
{
    unsigned int count;
    unsigned int i;

    if (effect == 0) {
        assert_prog(particleEffectAssertEffectNotNil, particleEffectSourcePath, 441);
    }
    count = effect->shootCount;
    effect->shootActive = 0;
    for (i = 0; i < count; i++) {
        effect->shots[i].life = time;
    }
}

void RgParticleEffectSetActive(RgParticleEffect *effect, int active)
{
    if (effect == 0) {
        assert_prog(particleEffectAssertEffectNotNil, particleEffectSourcePath, 453);
    }
    if (!(effect->state & RG_PARTICLE_EFFECT_STATE_LOCKED)) {
        if (active) {
            effect->state |= RG_PARTICLE_EFFECT_STATE_ACTIVE;
        } else {
            effect->state &= ~RG_PARTICLE_EFFECT_STATE_ACTIVE;
        }
    }
}

void InitRgParticleEffectControlData(RgParticleEffectControlData *pData)
{
    if (pData == 0) {
        assert_prog(particleEffectAssertDataNotNil, particleEffectSourcePath, 466);
    }
    pData->timer[0] = RG_PARTICLE_TIMER_OFF;
    pData->timer[1] = RG_PARTICLE_TIMER_OFF;
    pData->timer[2] = RG_PARTICLE_TIMER_OFF;
    pData->flags = 0;
    pData->counter = 0;
}

void RgParticleEffectSetControlData(RgParticleEffect *effect, RgParticleEffectControlData *pSrc)
{
    float value0;
    RgParticleEffectControlData *dst;
    int counter;
    int flags;
    float value1;
    float value2;

    if (effect == 0) {
        assert_prog(particleEffectAssertEffectNotNil, particleEffectSourcePath, 479);
    }
    if (pSrc == 0) {
        assert_prog(particleEffectAssertSourceNotNil, particleEffectSourcePath, 480);
    }
    value0 = pSrc->timer[0];
    dst = &effect->controlData;
    counter = pSrc->counter;
    dst->timer[0] = value0;
    flags = pSrc->flags;
    value1 = pSrc->timer[1];
    dst->timer[1] = value1;
    value2 = pSrc->timer[2];
    dst->counter = counter;
    dst->flags = flags;
    dst->timer[2] = value2;
}

/* Returns nonzero when the effect is not locked and every shot still has life left. */
int RgParticleEffectIsAlive(RgParticleEffect *effect)
{
    unsigned int alive;
    unsigned int i;
    RgParticleShot *shot;

    alive = 0;
    if (effect->state & RG_PARTICLE_EFFECT_STATE_LOCKED) {
        return 0;
    }
    for (i = 0; i < effect->shootCount; i++) {
        shot = &effect->shots[i];
        if (shot->life > 0.0f) {
            alive++;
        }
    }
    return alive >= effect->shootCount;
}

int RgParticleEffectIsActive(RgParticleEffect *effect)
{
    if (effect == 0) {
        assert_prog(particleEffectAssertEffectNotNil, particleEffectSourcePath, 515);
    }
    return (effect->state & (RG_PARTICLE_EFFECT_STATE_ACTIVE | RG_PARTICLE_EFFECT_STATE_LOCKED))
           == RG_PARTICLE_EFFECT_STATE_ACTIVE;
}

/* Counts a running timer down by dt. Returns 0 when it expires, 1 while it runs, 2 when it is not running. */
static int _decr_timer(float *timer, float dt)
{
    float updated;
    int status;

    status = 2;
    if (*timer > 0.0f) {
        updated = *timer - dt;
        status = 1;
        *timer = updated;
        if (updated <= 0.0f) {
            return 0;
        }
        return status;
    }
    return status;
}

/*
 * Applies the timers, then advances every shot by deltaTime. Shots emit while they are
 * alive, and repeating shots restart when their life ends.
 */
void RgParticleEffectPassTime(RgParticleEffect *effect, float deltaTime)
{
    XrgParticleDriver *driver;
    RgParticleShot *shot;
    unsigned int i;
    int startTimer;
    int shootTimer;
    int aliveTimer;

    driver = InstanceOfXrgParticleDriver();
    if (effect == 0) {
        assert_prog(particleEffectAssertEffectNotNil, particleEffectSourcePath, 549);
    }
    startTimer = _decr_timer(&effect->controlData.timer[0], deltaTime);
    shootTimer = _decr_timer(&effect->controlData.timer[1], deltaTime);
    aliveTimer = _decr_timer(&effect->controlData.timer[2], deltaTime);
    if (startTimer == 0) {
        effect->state |= RG_PARTICLE_EFFECT_STATE_ACTIVE;
    }
    if (shootTimer == 0) {
        RgParticleEffectStopShoot(effect);
    }
    if (aliveTimer == 0) {
        effect->state = (effect->state | RG_PARTICLE_EFFECT_STATE_LOCKED)
                        & ~RG_PARTICLE_EFFECT_STATE_ACTIVE;
    }
    if (effect->particles == 0) {
        return;
    }
    if (!(effect->state & RG_PARTICLE_EFFECT_STATE_ACTIVE)) {
        return;
    }
    /* Each shot emits while alive and passes its particles to the driver's calc queue. */
    for (i = 0; i < effect->shootCount; i++) {
        shot = &effect->shots[i];
        shot->life -= deltaTime;
        if (shot->started == 0) {
            _ShootParticles(shot, 0.0f, 1);
            shot->started = 1;
        }
        if (shot->life >= 0.0f) {
            if (effect->shootActive != 0) {
                _ShootParticles(shot, deltaTime, 0);
            }
            XrgParticleDriverPassTimeReq(driver, shot->particles,
                                         shot->particleCount, shot->resist);
        } else if (shot->flags & RG_PARTICLE_SHOT_REPEAT) {
            shot->life = shot->lifeMax;
            _ReInitShoot(&shot->shoot);
            _ShootParticles(shot, 0.0f, 1);
        }
    }
    XrgParticleDriverPassTime(driver, deltaTime);
}

/* Queues the display request of every living shot while the effect is active. */
void RgParticleEffectDisp(RgParticleEffect *effect)
{
    XrgParticleDriver *driver;
    unsigned int i;
    RgParticleShot *shot;

    driver = InstanceOfXrgParticleDriver();
    if (effect == 0) {
        assert_prog(particleEffectAssertEffectNotNil, particleEffectSourcePath, 616);
    }
    if ((effect->particles != 0) && (effect->state & RG_PARTICLE_EFFECT_STATE_ACTIVE)) {
        for (i = 0; i < effect->shootCount; i++) {
            shot = &effect->shots[i];
            if (shot->life >= 0.0f) {
                XrgParticleDriverDispReq(driver, shot->particles,
                                         shot->particleCount, shot->pic);
            }
        }
    }
}

static void _DestructPtclChar(RgParticleChar *pChar)
{
    if (pChar == 0) {
        assert_prog(particleEffectAssertCharNotNil, particleEffectSourcePath, 652);
    }
    if (pChar->effect != 0) {
        DisposeRgParticleEffect(pChar->effect);
    }
    pChar->effect = 0;
}

/* Advances the effect and frees the character unless the effect is still alive and active. */
static void _PassTimePtclChar(RgParticleChar *pChar, float deltaTime)
{
    int alive;

    alive = 0;
    if (pChar == 0) {
        assert_prog(particleEffectAssertCharNotNil, particleEffectSourcePath, 663);
    }
    if (pChar->effect != 0) {
        RgParticleEffectPassTime(pChar->effect, deltaTime);
        if (RgParticleEffectIsAlive(pChar->effect) != 0) {
            alive = RgParticleEffectIsActive(pChar->effect) != 0;
        }
    }
    if (alive != 0) {
        return;
    }
    RgCharFree(&pChar->base);
}

static void _InitPtclChar(RgParticleChar *pChar, RgParticleEffect *effect);

static void _DispPtclChar(RgParticleChar *pChar)
{
    if (pChar == 0) {
        assert_prog(particleEffectAssertCharNotNil, particleEffectSourcePath, 677);
    }
    if (pChar->effect != 0) {
        RgParticleEffectDisp(pChar->effect);
    }
}

static void _InitPtclChar(RgParticleChar *pChar, RgParticleEffect *effect)
{
    if (pChar == 0) {
        assert_prog(particleEffectAssertCharNotNil, particleEffectSourcePath, 687);
    }
    RgCharDestructMethod(&pChar->base, (RgCharDestructFunc)_DestructPtclChar);
    RgCharPassTimeMethod(&pChar->base, (RgCharPassTimeFunc)_PassTimePtclChar);
    RgCharDispMethod(&pChar->base, (RgCharDispFunc)_DispPtclChar);
    pChar->effect = effect;
}

/* Wraps effect in a new particle character, which then owns it. */
void CreateRgParticleEffectChar(RgParticleEffect *effect)
{
    _InitPtclChar((RgParticleChar *)RgCharAlloc(sizeof(RgParticleChar), 4), effect);
}
