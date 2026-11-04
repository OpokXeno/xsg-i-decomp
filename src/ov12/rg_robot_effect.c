/*
 * OV12 original TU 10: 0x00a0df70..0x00a0ece8 (30 functions)
 */
#include "common.h"
#include "shared.h"
#include "rg_robot_effect.h"
#include "ov12/rg_effect_env.h"
#include "ov12/rg_particle_effect.h"
#include "ov12/xrg_rand_int.h"
#include "ov12/assert_prog.h"
#include "ov12/rg_geom_point.h"
#include "ov12/rg_heap.h"
#include "ov12/xrg_actor.h"


/* A clear or stop time that never runs out (the item is idle). */
#define RG_ROBOT_EFFECT_TIME_NEVER 999999.0f

/* Joints of the robot actor the two jet particle emitters follow. */
#define RG_ROBOT_JET_JOINT_FIRST 16
#define RG_ROBOT_JET_JOINT_SECOND 17

/* Particles of each default essence this TU fills. */
#define RG_ROBOT_EFFECT_DEFAULT_PARTICLES 32

/* Same TU, not part of this allocation. */
static void _InitRobEff(RgRobotEffect *effect);
static void _ClearEffect(RgRobotEffectItem *item);
static int _GetJetShootLocal(RgRobotEffect *generator, int index,
                             void *local, RgVector inertia);
static int _InitJetDefault(void *buffer);
static int _GetDashShootLocal(RgRobotEffect *generator, int index,
                              void *local, RgVector inertia);
static int _InitDashDefault(void *buffer);

/*
 * Partial view of RgParticleEffectEssence (measured: the default fillers copy
 * the texture name with aligned ld/sd doublewords, so the name needs a
 * doubleword-aligned member; adding one to the canonical record changes the
 * struct copies in rg_particle_effect.c). Same layout, textureWords is never
 * read on its own.
 */
typedef struct RgParticleEffectEssenceDefaultsView {
    union {
        char textureName[0x40];
        unsigned long long textureWords[8];
    };
    int particleCount;
    u8 unmodeled_44[0x50 - 0x44];
    RgVector appearance;
    RgVector appearanceRate;
    float interval;
    float duration;
    u8 unmodeled_78[0x90 - 0x78];
    float speed;
    float life;
    float shotLife;
    int repeat;
} RgParticleEffectEssenceDefaultsView;

static int _InitJetDefault(void *buffer)
{
    RgParticleEffectEssenceDefaultsView *essence;

    essence = buffer;
    InitRgParticleEffectEssence(buffer);
    __builtin_strcpy(essence->textureName, "cld01.bmp");
    essence->particleCount = RG_ROBOT_EFFECT_DEFAULT_PARTICLES;
    essence->interval = 5.0f;
    essence->appearance[0] = 0.26f;
    essence->appearance[1] = 1.0f;
    essence->appearanceRate[0] = -0.1f;
    essence->appearanceRate[1] = -1.0f;
    essence->duration = 91.3f;
    essence->shotLife = 2.0f;
    essence->life = 2.0f;
    essence->speed = 0.55850536f;
    essence->repeat = 1;
    return 1;
}

static int _InitDashDefault(void *buffer)
{
    RgParticleEffectEssenceDefaultsView *essence;

    essence = buffer;
    InitRgParticleEffectEssence(buffer);
    __builtin_strcpy(essence->textureName, "tama03.bmp");
    essence->particleCount = RG_ROBOT_EFFECT_DEFAULT_PARTICLES;
    essence->interval = 2.4f;
    essence->appearance[0] = 0.56f;
    essence->appearance[1] = 1.0f;
    essence->appearanceRate[0] = 5.6f;
    essence->appearanceRate[1] = -5.0f;
    essence->duration = 91.9f;
    essence->shotLife = 1.5f;
    essence->life = 1.5f;
    essence->speed = 6.2831855f;
    essence->repeat = 1;
    return 1;
}

static void _InitEffect(RgRobotEffectItem *item)
{
    item->inertia = 0;
    item->particle_count = 0;
    item->clear_time = RG_ROBOT_EFFECT_TIME_NEVER;
    item->shoot_generator = 0;
    item->shoot_func = 0;
    item->stop_time = RG_ROBOT_EFFECT_TIME_NEVER;
}

void _ClearEffect(RgRobotEffectItem *item)
{
    RgParticleEffect *particle;
    u32 i;

    for (i = 0; i < (u32) item->particle_count; i++) {
        particle = item->particles[i];
        if (particle != 0) {
            DisposeRgParticleEffect(particle);
        }
        item->particles[i] = 0;
    }
    item->clear_time = RG_ROBOT_EFFECT_TIME_NEVER;
    item->particle_count = 0;
    item->stop_time = RG_ROBOT_EFFECT_TIME_NEVER;
}

static void _SetShootLocalGenEffect(RgRobotEffectItem *item,
                                    RgRobotEffect *generator,
                                    RgRobotEffectShootLocalFunc func)
{
    if (generator == 0 || func == 0) {
        assert_prog("pGenerator != NIL && pFunc != NIL",
                    "../rg_robot_effect.euc.c", 113);
    }
    item->shoot_generator = generator;
    item->shoot_func = func;
}

static void _AddEffect(RgRobotEffectItem *item, RgParticleEffectEssence *essence,
                       int count)
{
    RgParticleEffect *particle;
    int index;

    if ((u32) item->particle_count >= RG_ROBOT_EFFECT_MAX) {
        assert_prog("pEff->m_uPtclNum < EFF_MAX",
                    "../rg_robot_effect.euc.c", 122);
    }
    particle = CreateRgParticleEffect(essence, count);
    index = item->particle_count;
    item->particle_count = index + 1;
    item->particles[index] = particle;
}

static void _StopEffect(RgRobotEffectItem *item)
{
    u32 i;

    for (i = 0; i < (u32) item->particle_count; i++) {
        RgParticleEffectStopAlive(item->particles[i], 0.5f);
    }
    item->clear_time = 0.5f;
}

static void _SetStopTimeEffect(RgRobotEffectItem *item, float stop_time)
{
    item->stop_time = stop_time;
}

static void _PassTimeEffect(RgRobotEffectItem *item, float time)
{
    RgMatrix local;
    RgVector inertia;
    u32 i;

    if (item->clear_time <= 0.0f) {
        return;
    }
    if (item->shoot_generator != 0 && item->particle_count != 0) {
        i = 0;
        do {
            if (item->shoot_func(item->shoot_generator, i, local, inertia)) {
                RgParticleEffectSetShootLocal(item->particles[i], local);
                if (item->inertia != 0) {
                    RgParticleEffectSetShootInertia(item->particles[i], inertia);
                }
            }
            i++;
        } while (i < (u32) item->particle_count);
    }
    for (i = 0; i < (u32) item->particle_count; i++) {
        RgParticleEffectPassTime(item->particles[i], time);
    }
    if (item->particle_count == 0) {
        return;
    }
    item->clear_time -= time;
    if (item->clear_time <= 0.0f) {
        _ClearEffect(item);
    }
    item->stop_time -= time;
    if (item->stop_time <= 0.0f) {
        _StopEffect(item);
    }
}

static void _DispEffect(RgRobotEffectItem *item)
{
    u32 i;

    if (item->clear_time > 0.0f) {
        for (i = 0; i < (u32) item->particle_count; i++) {
            RgParticleEffectDisp(item->particles[i]);
        }
    }
}

static void _SetInertiaEffect(RgRobotEffectItem *item, int inertia)
{
    item->inertia = inertia;
}

void _InitRobEff(RgRobotEffect *effect) {
    RgRobotEffectItem *jet;

    jet = &effect->jet;
    if (effect == 0) {
        assert_prog("pRobEff != NIL", "../rg_robot_effect.euc.c", 218);
    }
    effect->actor = 0;
    effect->geom = 0;
    _InitEffect(jet);
    _SetInertiaEffect(jet, 1);
    _InitEffect(&effect->dash);
    effect->jet.ptcl_name[0] = 0;
    effect->dash.ptcl_name[0] = 0;
}

static void _DestructRobEff(RgRobotEffect *effect)
{
    if (effect == 0) {
        assert_prog("pRobEff != NIL", "../rg_robot_effect.euc.c", 230);
    }
    _ClearEffect(&effect->jet);
    _ClearEffect(&effect->dash);
}

static void _GetGeomVel(RgRobotEffect *effect, RgVector velocity)
{
    RgGeomPoint *geom;

    geom = effect->geom;
    if (geom != 0) {
        RgGeomPointGetVel(geom, velocity);
        return;
    }
    XrgClearVector(velocity);
}

static int _GetJetShootLocal(RgRobotEffect *generator, int index, void *local,
                             RgVector inertia)
{
    float *matrix_values;
    int result;

    if (generator->actor == 0) {
        return 0;
    }
    result = XrgActorGetJointLocal(generator->actor,
                                   index == 0 ? RG_ROBOT_JET_JOINT_FIRST : RG_ROBOT_JET_JOINT_SECOND, local);
    matrix_values = local;
    XrgNegateVector(&matrix_values[8], &matrix_values[8]);
    _GetGeomVel(generator, inertia);
    return result;
}

static int _GetDashShootLocal(RgRobotEffect *generator, int index, void *local,
                              RgVector inertia)
{
    float *matrix_values;

    if (generator->actor == 0) {
        return 0;
    }
    XrgActorGetLocal(generator->actor, local);
    matrix_values = local;
    matrix_values[13] = 0.2f;
    _GetGeomVel(generator, inertia);
    return 1;
}

RgRobotEffect *CreateRgRobotEffect(void)
{
    RgRobotEffect *effect;

    effect = RgHeapAlloc(InstanceOfRgHeap(), sizeof(RgRobotEffect),
                         "../rg_robot_effect.euc.c", 284);
    _InitRobEff(effect);
    return effect;
}

void DisposeRgRobotEffect(RgRobotEffect *effect)
{
    if (effect == 0) {
        assert_prog("pRobEff != NIL", "../rg_robot_effect.euc.c", 292);
    }
    _DestructRobEff(effect);
    RgHeapFree(InstanceOfRgHeap(), effect, "../rg_robot_effect.euc.c", 294);
}

void RgRobotEffectSetActor(RgRobotEffect *effect, void *actor)
{
    if (effect == 0) {
        assert_prog("pRobEff != NIL", "../rg_robot_effect.euc.c", 304);
    }
    effect->actor = actor;
}

void RgRobotEffectSetGeom(RgRobotEffect *effect, RgGeomPoint *geom)
{
    if (effect == 0) {
        assert_prog("pRobEff != NIL", "../rg_robot_effect.euc.c", 311);
    }
    effect->geom = geom;
}

void RgRobotEffectSetJetPtclName(RgRobotEffect *effect, const char *name)
{
    if (effect == 0) {
        assert_prog("pRobEff != NIL", "../rg_robot_effect.euc.c", 319);
    }
    strcpy(effect->jet.ptcl_name, name);
}

void RgRobotEffectSetDashPtclName(RgRobotEffect *effect, const char *name)
{
    if (effect == 0) {
        assert_prog("pRobEff != NIL", "../rg_robot_effect.euc.c", 327);
    }
    strcpy(effect->dash.ptcl_name, name);
}

void RgRobotEffectSetJetInertia(RgRobotEffect *effect, int inertia) {
    if (effect == 0) {
        assert_prog("pRobEff != NIL", "../rg_robot_effect.euc.c", 335);
    }
    _SetInertiaEffect(&effect->jet, inertia);
}

void RgRobotEffectTermAll(RgRobotEffect *effect)
{
    RgRobotEffectTermJet(effect);
}

void RgRobotEffectStartJet(RgRobotEffect *effect, float stopTime) {
    /* Raw particle-essence lookup buffer: opaque to this TU, forwarded to
     * _AddEffect / CreateRgParticleEffect (ov12/rg_particle_effect.c). */
    int shotData[0x28];
    /* Default shot-speed table this TU fills when the jet has no configured
     * particle data; entries are 0xB0 bytes (0x2C floats) apart and only the
     * first float of each is set here. Not otherwise read by this TU. */
    float shotDefaults[0x88];
    RgRobotEffectItem *jet;
    float *shotDefault;
    u32 shotCount;
    u32 i;

    if (effect == 0) {
        assert_prog("pRobEff != NIL", "../rg_robot_effect.euc.c", 356);
    }
    jet = &effect->jet;
    if (effect->actor != 0) {
        _ClearEffect(jet);
        shotCount = RgEffectEnvGetParticleData(InstanceOfRgEffectEnv(),
                                               effect->jet.ptcl_name,
                                               shotData);
        if (shotCount == 0) {
            shotCount = _InitJetDefault(shotData);
        }
        i = 0;
        if (shotCount != 0) {
            shotDefault = &shotDefaults[0];
            do {
                i += 1;
                *shotDefault = 2.5f;
                shotDefault += sizeof(RgParticleEffectEssence) / sizeof(float);
            } while (i < shotCount);
        }
        _AddEffect(jet, (RgParticleEffectEssence *) shotData, shotCount);
        _SetShootLocalGenEffect(jet, effect, _GetJetShootLocal);
        _AddEffect(jet, (RgParticleEffectEssence *) shotData, shotCount);
        _SetShootLocalGenEffect(jet, effect, _GetJetShootLocal);
        _SetStopTimeEffect(jet, stopTime);
    }
}

void RgRobotEffectTermJet(RgRobotEffect *effect)
{
    if (effect == 0) {
        assert_prog("pRobEff != NIL", "../rg_robot_effect.euc.c", 387);
    }
    if (effect->actor != 0) {
        _StopEffect(&effect->jet);
    }
}

void RgRobotEffectStartDash(RgRobotEffect *effect, float stopTime) {
    /* Raw particle-essence lookup buffer, opaque to this TU; see
     * RgRobotEffectStartJet. */
    int shotData[0xB0];
    RgRobotEffectItem *dash;
    int shotCount;

    dash = &effect->dash;
    if (effect == 0) {
        assert_prog("pRobEff != NIL", "../rg_robot_effect.euc.c", 402);
    }
    if (effect->actor != 0) {
        _ClearEffect(dash);
        shotCount = RgEffectEnvGetParticleData(InstanceOfRgEffectEnv(),
                                               effect->dash.ptcl_name,
                                               shotData);
        if (shotCount == 0) {
            shotCount = _InitDashDefault(shotData);
        }
        _AddEffect(dash, (RgParticleEffectEssence *) shotData, shotCount);
        _SetShootLocalGenEffect(dash, effect, _GetDashShootLocal);
        _SetStopTimeEffect(dash, stopTime);
    }
}

void RgRobotEffectTermDash(RgRobotEffect *effect) {
    if (effect == 0) {
        assert_prog("pRobEff != NIL", "../rg_robot_effect.euc.c", 427);
    }
    if (effect->actor != 0) {
        _StopEffect(&effect->dash);
    }
}

void RgRobotEffectPassTime(RgRobotEffect *effect, float time) {
    if (effect == 0) {
        assert_prog("pRobEff != NIL", "../rg_robot_effect.euc.c", 441);
    }
    if (effect->actor != 0) {
        _PassTimeEffect(&effect->jet, time);
        _PassTimeEffect(&effect->dash, time);
    }
}

void RgRobotEffectDisp(RgRobotEffect *effect) {
    if (effect == 0) {
        assert_prog("pRobEff != NIL", "../rg_robot_effect.euc.c", 453);
    }
    if (effect->actor == 0) {
        return;
    }
    _DispEffect(&effect->jet);
    _DispEffect(&effect->dash);
}
