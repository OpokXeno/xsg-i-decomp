/*
 * OV12 original TU 35: 0x00a20718..0x00a21c58 (27 functions)
 */
#include "common.h"
#include "shared.h"
#include "rg_particle_effect.h"
#include "ov12/xrg_rand_int.h"

extern void assert_prog(const char *expression, const char *source_file,
                        int line);
extern RgHeap *InstanceOfRgHeap(void);
extern void *RgHeapAlloc(void *heap, unsigned int size,
                         const char *source_file, int line);
extern void XrgUnitMatrix(RgMatrix destination);
extern void DisposeArrayOfXrgParticle(XrgParticle *particles);
extern void DisposeRgParticleEffect(RgParticleEffect *effect);
extern void RgHeapFree(RgHeap *heap, void *pointer, const char *source_file,
                       int line);
int RgParticleEffectIsAlive(RgParticleEffect *effect);
extern void RgParticleEffectPassTime(RgParticleEffect *effect, float time);

/*
 * RgChar is defined in src/ov12/rg_char.h (another translation unit); only
 * its pointer identity is needed here, ahead of the comment and #include
 * further below that names this TU's own local sibling using it.
 */
typedef struct RgChar RgChar;

extern void RgCharFree(RgChar *pChar);
static void _InitEffect(RgParticleEffect *effect, RgParticleEffectEssence *essence,
                        int context);

/*
 * External file-backed witnesses, not candidate-emitted data: this window is
 * asm-owned scaffold data (splat names, no config/symbols/ov12.txt entry).
 *
 * ov12:0x00a542c8 contains the assertion expression "pEff != NIL".
 * ov12:0x00a542d8 contains the source filename "../rg_particle_effect.euc.c".
 * ov12:0x00a542f8 contains the assertion expression "aEss != NIL".
 * ov12:0x00a54378 contains the assertion expression "pData != NIL".
 * ov12:0x00a54398 contains the assertion expression "pChar != NIL".
 */
const char D_00A542C8[16] = "pEff != NIL";

const char D_00A542D8[32] = "../rg_particle_effect.euc.c";

const char D_00A542F8[16] = "aEss != NIL";

const char D_00A54368[16] = "pEss != NIL";

const char D_00A54378[16] = "pData != NIL";

const char D_00A54388[16] = "pSrc != NIL";

const char D_00A54398[16] = "pChar != NIL";

static void _InitShoot(RgParticleShoot *shoot, RgParticleEffectEssence *essence)
{
    shoot->interval = essence->interval;
    shoot->duration = essence->duration;
    shoot->speed = essence->speed;
    shoot->lifeMax = shoot->life = essence->life;
    XrgCopyVector(shoot->position, essence->position);
    XrgCopyVector(shoot->direction, essence->direction);
    shoot->elapsedFrames = 0;
    shoot->shotCount = 0;
    XrgUnitMatrix(shoot->localMatrix);
    shoot->flags = 0;
    XrgUnitMatrix(shoot->worldMatrix);
    XrgClearVector(shoot->force);
}

extern float floorf(float value);

extern float XrgRand(float lower, float upper);
extern void XrgUnitVector(RgVector vector);
extern void XrgRandVectorRange(RgVector destination, float lower, float upper);
extern void XrgLinearIntpVector(RgVector destination, RgVector first,
                                RgVector second, float weight);
extern void XrgScaleVectorXYZ(RgVector destination, RgVector source, float scale);
extern void XrgAddVectorXYZ(RgVector destination, RgVector first, RgVector second);

static inline int _MaxInt(int a, int b)
{
    return (a >= b) ? a : b;
}

static void _ShootParticles(RgParticleShot *shot, float deltaTime, int opening)
{
    RgParticleShoot *shoot;
    RgVector *end;
    unsigned int count;
    RgVector *cursor;
    RgMatrix matrix;
    float *localMatrix;
    RgVector base0;
    RgVector base1;
    RgVector force;
    RgVector point;
    RgVector dir;
    RgVector zero;
    RgVector spinePos;
    RgVector spineDir;
    float lo;
    float hi;
    float piBound;
    float hiMin;
    float twoPi;
    float emitted;
    float ret;
    float invCount;
    float interval;
    float scale;
    int elapsed;
    unsigned int requested;
    int intRet;

    end = (RgVector *)shot->dispArg0 + shot->dispArg1 * 5;
    shoot = (RgParticleShoot *)shot->shoot;
    if (opening != 0) {
        if (shoot->duration > 0.0f) {
            return;
        }
        count = shot->dispArg1;
    } else {
        shoot->life = shoot->life - deltaTime;
        if (shoot->life < 0.0f) {
            return;
        }
        emitted = shoot->duration * deltaTime + *(float *)&shoot->shotCount;
        ret = floorf(emitted);
        *(float *)&shoot->shotCount = emitted - ret;
        if (ret <= 0.5f) {
            return;
        }
        intRet = (int)ret;
        count = _MaxInt(intRet, 1);
    }
    XrgCopyVector(force, shoot->force);
    if (shoot->elapsedFrames == 0) {
        shoot->elapsedFrames = (int)shot->dispArg0;
    }
    cursor = (RgVector *)shoot->elapsedFrames;
    piBound = 3.1415927f;
    lo = shoot->speed;
    if (lo < 0.0f) {
        lo = 0.0f;
    } else if (lo > piBound) {
        lo = piBound;
    }
    hi = shoot->speed;
    hiMin = 0.0f;
    twoPi = 6.2831855f;
    if (hi < hiMin) {
        hi = hiMin;
    } else if (hi > twoPi) {
        hi = twoPi;
    }
    localMatrix = shoot->localMatrix;
    XrgCopyMatrix(matrix, localMatrix);
    XrgUnitVector(&matrix[12]);
    XrgCopyVector(base0, &localMatrix[12]);
    if (shoot->flags != 0) {
        XrgCopyVector(base1, &shoot->worldMatrix[12]);
    } else {
        XrgCopyVector(base1, base0);
        shoot->flags = 1;
    }
    XrgCopyMatrix(shoot->worldMatrix, localMatrix);
    invCount = 1.0f / (float)(unsigned int)count;
    requested = count;
    count = count - 1;
    if (requested != 0) {
        do {
            XrgLinearIntpVector(point, base0, base1, (float)count * invCount);
            XrgRandVectorRange(dir, lo, hi);
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
            XrgAddVectorXYZ(dir, dir, force);
            XrgClearVector(zero);
            XrgCopyVector(spinePos, shoot->position);
            XrgCopyVector(spineDir, shoot->direction);
            spinePos[2] = spinePos[2] * 6.2831855f;
            spinePos[3] = spinePos[3] * 6.2831855f;
            if (spineDir[2] != 0.0f) {
                spinePos[2] = XrgRand(0.0f, 3.1415927f);
            }
            if (spineDir[3] != 0.0f) {
                spinePos[3] = XrgRand(0.0f, 3.1415927f);
            }
            XrgCopyVector(cursor[0], point);
            XrgCopyVector(cursor[1], dir);
            XrgCopyVector(cursor[2], zero);
            XrgCopyVector(cursor[3], spinePos);
            XrgCopyVector(cursor[4], spineDir);
            cursor += 5;
            if (cursor >= end) {
                cursor = (RgVector *)shot->dispArg0;
            }
        } while (count-- > 0);
    }
    shoot->elapsedFrames = (int)cursor;
}

static void _ReInitShoot(RgParticleShoot *shoot)
{
    shoot->flags = 0;
    shoot->life = shoot->lifeMax;
    XrgClearVector(shoot->force);
}

#include "ov12/rg_bxx.h"
#include "ov12/rg_effect_env.h"

extern XrgParticle *CreateArrayOfXrgParticle(unsigned int count);
extern void RgError(const char *message, const char *source_file, int line, ...);

typedef struct RgEffectEnv RgEffectEnv;

extern RgEffectEnv *InstanceOfRgEffectEnv(void);
extern RgBxx *RgEffectEnvGetBxx(RgEffectEnv *env);

typedef struct RgBxxPic RgBxxPic;

extern RgBxxPic *RgBxxGetPic(RgBxx *pBxx, const char *pszName);

const char D_00A54308[16] = "pBxx != NIL";

const char D_00A54318[32] = "too many particle total (%d)";

const char D_00A54338[48] = "there is not texture named '%s' for particle";

extern float XrgRand(float min, float max);
extern void XrgUnitVector(RgVector vector);
extern void XrgLinearIntpVector(RgVector destination, RgVector first, RgVector second, float t);
extern void XrgRandVectorRange(RgVector destination, float min, float max);
extern void XrgScaleVectorXYZ(RgVector destination, RgVector vector, float scale);
extern void XrgAddVectorXYZ(RgVector destination, RgVector first, RgVector second);
extern void XrgClearVector(RgVector destination);
extern void XrgCopyVector(RgVector destination, RgVector source);
extern float func_00323DE8(float value);
extern void XrgCopyMatrix(RgMatrix destination, const RgMatrix source);

/*
 * Element stride of the XrgParticle array CreateArrayOfXrgParticle returns:
 * _InitEffect advances the particle cursor by particleCount * 0x50 bytes.
 */
struct XrgParticle {
    u8 unmodeled_00[0x50];
};

static void _InitEffect(RgParticleEffect *effect, RgParticleEffectEssence *essence,
                        int context)
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
        assert_prog(D_00A542C8, D_00A542D8, 246);
    }
    if (essence == 0) {
        assert_prog(D_00A542F8, D_00A542D8, 247);
    }
    if (bxx == 0) {
        assert_prog(D_00A54308, D_00A542D8, 248);
    }
    effect->state = RG_PARTICLE_EFFECT_STATE_ACTIVE;
    particleTotal = 0;
    effect->shootActive = 1;
    InitRgParticleEffectControlData(&effect->controlData);
    for (i = 0; i < context; i++) {
        particleTotal += essence[i].particleCount;
    }
    effect->particleTotal = particleTotal;
    effect->shootCount = context;
    if (context == 0 || particleTotal == 0) {
        effect->particles = 0;
        return;
    }
    if (particleTotal > 4096) {
        RgError(D_00A54318, D_00A542D8, 267, particleTotal);
    }
    effect->particles = CreateArrayOfXrgParticle(particleTotal);
    particles = effect->particles;
    for (i = 0; i < context; i++) {
        shot = &effect->shots[i];
        shot->dispArg1 = essence[i].particleCount;
        shot->dispArg0 = (int)particles;
        pic = RgBxxGetPic(bxx, essence[i].textureName);
        shot->life = essence[i].shotLife;
        shot->started = 0;
        repeat = essence[i].repeat;
        shot->lifeMax = essence[i].shotLife;
        shot->dispArg2 = (int)pic;
        shot->flags = 0;
        shot->resist = essence[i].resist;
        if (repeat) {
            shot->flags = RG_PARTICLE_SHOT_REPEAT;
        }
        if (shot->dispArg2 == 0) {
            RgError(D_00A54338, D_00A542D8, 290, essence[i].textureName);
        }
        _InitShoot((RgParticleShoot *)shot->shoot, &essence[i]);
        particles += essence[i].particleCount;
    }
}

static void _DestructEffect(RgParticleEffect *effect)
{
    if (effect == 0) {
        assert_prog(D_00A542C8, D_00A542D8, 308);
    }
    if (effect->particles != 0) {
        DisposeArrayOfXrgParticle(effect->particles);
    }
}

RgParticleEffect *CreateRgParticleEffect(RgParticleEffectEssence *essence, int context)
{
    RgParticleEffect *effect;

    effect = RgHeapAlloc(InstanceOfRgHeap(), 0x470, D_00A542D8, 322);
    if (effect == 0) {
        assert_prog(D_00A542C8, D_00A542D8, 323);
    }
    if (essence == 0) {
        assert_prog(D_00A542F8, D_00A542D8, 324);
    }
    _InitEffect(effect, essence, context);
    return effect;
}

void DisposeRgParticleEffect(RgParticleEffect *effect)
{
    if (effect == 0) {
        assert_prog(D_00A542C8, D_00A542D8, 331);
    }
    _DestructEffect(effect);
    RgHeapFree(InstanceOfRgHeap(), effect, D_00A542D8, 333);
}

void InitRgParticleEffectEssence(RgParticleEffectEssence *essence)
{
    extern void XrgSetVectorXYZ(RgVector destination, float x, float y, float z);

    if (essence == 0) {
        assert_prog(D_00A54368, D_00A542D8, 339);
    }
    essence->textureName[0] = '\0';
    essence->particleCount = 32;
    XrgClearVector(essence->position);
    XrgClearVector(essence->direction);
    essence->position[0] = 0.1f;
    essence->position[1] = 1.0f;
    essence->direction[0] = -0.1f;
    essence->duration = 0.0f;
    essence->direction[1] = -1.0f;
    essence->interval = 5.0f;
    XrgSetVectorXYZ(essence->unitDirection, 0.0f, 0.0f, 1.0f);
    essence->speed = 6.2831855f;
    essence->life = 0.0f;
    essence->shotLife = 5.0f;
    essence->repeat = 0;
    essence->resist = 0.0f;
}

void RgParticleEffectEssenceSerialize(RgParticleEffectEssence *essence,
                                      RgParticleEffectEssence *destination)
{

    if (essence == 0) {
        assert_prog(D_00A54368, D_00A542D8, 365);
    }
    memset(destination, 0, 0x100);
    *destination = *essence;
}

void RgParticleEffectEssenceResume(RgParticleEffectEssence *essence,
                                   RgParticleEffectEssence *source)
{
    if (essence == 0) {
        assert_prog(D_00A54368, D_00A542D8, 375);
    }
    *essence = *source;
}

void RgParticleEffectSetShootLocal(RgParticleEffect *effect, RgMatrix local)
{
    unsigned int i;

    if (effect == 0) {
        assert_prog(D_00A542C8, D_00A542D8, 408);
    }
    for (i = 0; i < effect->shootCount; i++) {
        XrgCopyMatrix(effect->shots[i].localMatrix, local);
    }
}

void RgParticleEffectSetShootInertia(RgParticleEffect *effect, RgVector inertia)
{
    unsigned int i;

    if (effect == 0) {
        assert_prog(D_00A542C8, D_00A542D8, 421);
    }
    for (i = 0; i < effect->shootCount; i++) {
        XrgCopyVector(effect->shots[i].inertia, inertia);
    }
}

void RgParticleEffectStopShoot(RgParticleEffect *effect)
{
    if (effect == 0) {
        assert_prog(D_00A542C8, D_00A542D8, 432);
    }
    effect->shootActive = 0;
}

void RgParticleEffectStopAlive(RgParticleEffect *effect, float time)
{
    unsigned int count;
    unsigned int i;

    if (effect == 0) {
        assert_prog(D_00A542C8, D_00A542D8, 441);
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
        assert_prog(D_00A542C8, D_00A542D8, 453);
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
        assert_prog(D_00A54378, D_00A542D8, 466);
    }
    pData->value[0] = -1.0f;
    pData->value[1] = -1.0f;
    pData->value[2] = -1.0f;
    pData->flags = 0;
    pData->counter = 0;
}

/*
 * External file-backed witness, not candidate-emitted data: this window is
 * asm-owned scaffold data (splat name, no config/symbols/ov12.txt entry).
 *
 * ov12:0x00a54388 contains the assertion expression "pSrc != NIL".
 */
void RgParticleEffectSetControlData(RgParticleEffect *effect, RgParticleEffectControlData *pSrc)
{
    float value0;
    RgParticleEffectControlData *dst;
    int counter;
    int flags;
    float value1;
    float value2;

    if (effect == 0) {
        assert_prog(D_00A542C8, D_00A542D8, 479);
    }
    if (pSrc == 0) {
        assert_prog(D_00A54388, D_00A542D8, 480);
    }
    value0 = pSrc->value[0];
    dst = &effect->controlData;
    counter = pSrc->counter;
    dst->value[0] = value0;
    flags = pSrc->flags;
    value1 = pSrc->value[1];
    dst->value[1] = value1;
    value2 = pSrc->value[2];
    dst->counter = counter;
    dst->flags = flags;
    dst->value[2] = value2;
}

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
        assert_prog(D_00A542C8, D_00A542D8, 515);
    }
    return (effect->state & (RG_PARTICLE_EFFECT_STATE_ACTIVE | RG_PARTICLE_EFFECT_STATE_LOCKED))
           == RG_PARTICLE_EFFECT_STATE_ACTIVE;
}

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

struct XrgParticleDriver;
extern struct XrgParticleDriver *InstanceOfXrgParticleDriver(void);
extern void XrgParticleDriverPassTimeReq(struct XrgParticleDriver *driver, int dispArg0,
                                         int dispArg1, float resist);
extern void XrgParticleDriverPassTime(struct XrgParticleDriver *driver, float deltaTime);
static void _ShootParticles(RgParticleShot *shot, float deltaTime, int opening);

void RgParticleEffectPassTime(RgParticleEffect *effect, float deltaTime)
{
    struct XrgParticleDriver *driver;
    RgParticleShot *shot;
    unsigned int i;
    int startTimer;
    int shootTimer;
    int aliveTimer;

    driver = InstanceOfXrgParticleDriver();
    if (effect == 0) {
        assert_prog(D_00A542C8, D_00A542D8, 549);
    }
    startTimer = _decr_timer(&effect->controlData.value[0], deltaTime);
    shootTimer = _decr_timer(&effect->controlData.value[1], deltaTime);
    aliveTimer = _decr_timer(&effect->controlData.value[2], deltaTime);
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
            XrgParticleDriverPassTimeReq(driver, shot->dispArg0, shot->dispArg1, shot->resist);
        } else if (shot->flags & RG_PARTICLE_SHOT_REPEAT) {
            shot->life = shot->lifeMax;
            /*
             * shot->shoot names the first bytes of the RgParticleShoot record
             * embedded at 0x10, the address _InitEffect also hands to
             * _InitShoot; struct RgParticleShot already names that record's
             * localMatrix and force at 0x60 and 0xF0, so the record itself
             * carries the type only here.
             */
            _ReInitShoot((RgParticleShoot *)shot->shoot);
            _ShootParticles(shot, 0.0f, 1);
        }
    }
    XrgParticleDriverPassTime(driver, deltaTime);
}

typedef struct XrgParticleDriver XrgParticleDriver;

extern XrgParticleDriver *InstanceOfXrgParticleDriver(void);
extern void XrgParticleDriverDispReq(XrgParticleDriver *driver, int dispArg0,
                                     int dispArg1, int dispArg2);

void RgParticleEffectDisp(RgParticleEffect *effect)
{
    XrgParticleDriver *driver;
    unsigned int i;
    RgParticleShot *shot;

    driver = InstanceOfXrgParticleDriver();
    if (effect == 0) {
        assert_prog(D_00A542C8, D_00A542D8, 616);
    }
    if ((effect->particles != 0) && (effect->state & RG_PARTICLE_EFFECT_STATE_ACTIVE)) {
        for (i = 0; i < effect->shootCount; i++) {
            shot = &effect->shots[i];
            if (shot->life >= 0.0f) {
                XrgParticleDriverDispReq(driver, shot->dispArg0, shot->dispArg1, shot->dispArg2);
            }
        }
    }
}

static void _DestructPtclChar(RgParticleChar *pChar)
{
    if (pChar == 0) {
        assert_prog(D_00A54398, D_00A542D8, 652);
    }
    if (pChar->effect != 0) {
        DisposeRgParticleEffect(pChar->effect);
    }
    pChar->effect = 0;
}

static void _PassTimePtclChar(RgParticleChar *pChar, float deltaTime)
{
    int alive;

    alive = 0;
    if (pChar == 0) {
        assert_prog(D_00A54398, D_00A542D8, 663);
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
    RgCharFree((RgChar *)pChar);
}

#include "ov12/rg_char.h"

extern void RgParticleEffectDisp(RgParticleEffect *effect);
static void _InitPtclChar(RgParticleChar *pChar, RgParticleEffect *effect);

static void _DispPtclChar(RgParticleChar *pChar)
{
    if (pChar == 0) {
        assert_prog(D_00A54398, D_00A542D8, 677);
    }
    if (pChar->effect != 0) {
        RgParticleEffectDisp(pChar->effect);
    }
}

extern void RgCharDestructMethod(RgChar *pChar, RgCharDestructFunc destructMethod);
extern void RgCharDispMethod(RgChar *pChar, RgCharDispFunc dispMethod);
extern void RgCharPassTimeMethod(RgChar *pChar, RgCharPassTimeFunc passTimeMethod);

void _InitPtclChar(RgParticleChar *pChar, RgParticleEffect *effect)
{
    if (pChar == 0) {
        assert_prog(D_00A54398, D_00A542D8, 687);
    }
    RgCharDestructMethod((RgChar *)pChar, (RgCharDestructFunc)_DestructPtclChar);
    RgCharPassTimeMethod((RgChar *)pChar, (RgCharPassTimeFunc)_PassTimePtclChar);
    RgCharDispMethod((RgChar *)pChar, (RgCharDispFunc)_DispPtclChar);
    pChar->effect = effect;
}

void CreateRgParticleEffectChar(RgParticleEffect *effect)
{
    _InitPtclChar((RgParticleChar *)RgCharAlloc(sizeof(RgParticleChar), 4), effect);
}
