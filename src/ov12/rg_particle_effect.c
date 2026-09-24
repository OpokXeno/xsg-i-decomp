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
extern int RgParticleEffectIsAlive(RgParticleEffect *effect);
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
extern const char D_00A542C8[];
extern const char D_00A542D8[];
extern const char D_00A542F8[];
extern const char D_00A54378[];
extern const char D_00A54398[];

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

INCLUDE_ASM("asm/nonmatchings/ov12/rg_particle_effect", _ShootParticles);

static void _ReInitShoot(RgParticleShoot *shoot)
{
    shoot->flags = 0;
    shoot->life = shoot->lifeMax;
    XrgClearVector(shoot->force);
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_particle_effect", _InitEffect_00A20C70);

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

INCLUDE_ASM("asm/nonmatchings/ov12/rg_particle_effect", InitRgParticleEffectEssence);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_particle_effect", RgParticleEffectEssenceSerialize);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_particle_effect", RgParticleEffectEssenceResume);

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
extern const char D_00A54388[];

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

INCLUDE_ASM("asm/nonmatchings/ov12/rg_particle_effect", RgParticleEffectIsAlive);

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

/*
 * RgChar is defined in src/ov12/rg_char.h (another translation unit). This
 * TU only passes RgChar pointers through (RgCharAlloc's result), so the tag
 * is forward-declared here without repeating that TU's member layout, the
 * same pattern src/ov12/rg_charmgr.c uses.
 */
typedef struct RgChar RgChar;
#include "ov12/rg_char.h"
extern void XrgCopyVector(RgVector destination, RgVector source);
extern void XrgClearVector(RgVector destination);
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
