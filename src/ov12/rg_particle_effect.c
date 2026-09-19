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

INCLUDE_ASM("asm/nonmatchings/ov12/rg_particle_effect", _ReInitShoot);

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

INCLUDE_ASM("asm/nonmatchings/ov12/rg_particle_effect", DisposeRgParticleEffect);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_particle_effect", InitRgParticleEffectEssence);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_particle_effect", RgParticleEffectEssenceSerialize);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_particle_effect", RgParticleEffectEssenceResume);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_particle_effect", RgParticleEffectSetShootLocal);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_particle_effect", RgParticleEffectSetShootInertia);

void RgParticleEffectStopShoot(RgParticleEffect *effect)
{
    if (effect == 0) {
        assert_prog(D_00A542C8, D_00A542D8, 432);
    }
    effect->shootActive = 0;
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_particle_effect", RgParticleEffectStopAlive);

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

INCLUDE_ASM("asm/nonmatchings/ov12/rg_particle_effect", RgParticleEffectSetControlData);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_particle_effect", RgParticleEffectIsAlive);

int RgParticleEffectIsActive(RgParticleEffect *effect)
{
    if (effect == 0) {
        assert_prog(D_00A542C8, D_00A542D8, 515);
    }
    return (effect->state & (RG_PARTICLE_EFFECT_STATE_ACTIVE | RG_PARTICLE_EFFECT_STATE_LOCKED))
           == RG_PARTICLE_EFFECT_STATE_ACTIVE;
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_particle_effect", _decr_timer);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_particle_effect", RgParticleEffectPassTime);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_particle_effect", RgParticleEffectDisp);

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

INCLUDE_ASM("asm/nonmatchings/ov12/rg_particle_effect", _PassTimePtclChar);

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

INCLUDE_ASM("asm/nonmatchings/ov12/rg_particle_effect", _InitPtclChar);

void CreateRgParticleEffectChar(RgParticleEffect *effect)
{
    _InitPtclChar((RgParticleChar *)RgCharAlloc(sizeof(RgParticleChar), 4), effect);
}
