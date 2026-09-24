/*
 * OV12 original TU 10: 0x00a0df70..0x00a0ece8 (30 functions)
 */
#include "common.h"
#include "shared.h"
#include "rg_robot_effect.h"
#include "ov12/xrg_rand_int.h"

/*
 * ov12:0x00a52378, "pGenerator != NIL && pFunc != NIL"
 * ov12:0x00a523a0, "../rg_robot_effect.euc.c"
 * ov12:0x00a523e0, "pRobEff != NIL"
 * All three strings are scaffold .rodata (config/tu-build.json data_ownership
 * .rodata: owner asm) with no config/symbols/ov12.txt entry, so they keep
 * their splat names.
 */
extern const char D_00A52378[];
extern const char D_00A523A0[];
extern const char D_00A523E0[];

extern void assert_prog(const char *expression, const char *source_file,
                        int line);
extern char *strcpy(char *destination, const char *source);
extern RgHeap *InstanceOfRgHeap(void);
extern void *RgHeapAlloc(void *heap, unsigned int size,
                         const char *source_file, int line);
extern void RgHeapFree(RgHeap *heap, void *ptr, const char *source_file,
                       int line);
extern void RgGeomPointGetVel(RgGeomPoint *point, RgVector velocity);

/* Same TU, not part of this allocation. */
static void _InitRobEff(RgRobotEffect *effect);
static void _ClearEffect(RgRobotEffectItem *item);
extern void RgRobotEffectTermJet(RgRobotEffect *effect);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot_effect", _InitJetDefault);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot_effect", _InitDashDefault);

static void _InitEffect(RgRobotEffectItem *item)
{
    item->inertia = 0;
    item->particle_count = 0;
    item->clear_time = 999999.0f;
    item->shoot_generator = 0;
    item->shoot_func = 0;
    item->stop_time = 999999.0f;
}

extern void DisposeRgParticleEffect(RgParticleEffect *particleEffect);

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
    item->clear_time = 999999.0f;
    item->particle_count = 0;
    item->stop_time = 999999.0f;
}

static void _SetShootLocalGenEffect(RgRobotEffectItem *item,
                                    RgRobotEffect *generator,
                                    RgRobotEffectShootLocalFunc func)
{
    if (generator == 0 || func == 0) {
        assert_prog(D_00A52378, D_00A523A0, 113);
    }
    item->shoot_generator = generator;
    item->shoot_func = func;
}

/* Opaque: this TU only forwards a pointer to it, from a stack buffer another
 * TU's function fills; see RgRobotEffectStartJet/RgRobotEffectStartDash. */
typedef struct RgParticleEffectEssence RgParticleEffectEssence;
extern RgParticleEffect *CreateRgParticleEffect(RgParticleEffectEssence *essence,
                                                int count);

/* ov12:0x00a0e25c, "../rg_robot_effect.euc.c" (D_00A523A0), no
 * config/symbols/ov12.txt entry: scaffold .rodata like the other assert
 * strings above. */
extern const char D_00A523C0[];

static void _AddEffect(RgRobotEffectItem *item, RgParticleEffectEssence *essence,
                       int count)
{
    RgParticleEffect *particle;
    int index;

    if ((u32) item->particle_count >= 2) {
        assert_prog(D_00A523C0, D_00A523A0, 122);
    }
    particle = CreateRgParticleEffect(essence, count);
    index = item->particle_count;
    item->particle_count = index + 1;
    item->particles[index] = particle;
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot_effect", _StopEffect);

static void _SetStopTimeEffect(RgRobotEffectItem *item, float stop_time)
{
    item->stop_time = stop_time;
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot_effect", _PassTimeEffect);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot_effect", _DispEffect);

static void _SetInertiaEffect(RgRobotEffectItem *item, int inertia)
{
    item->inertia = inertia;
}

void _InitRobEff(RgRobotEffect *effect) {
    RgRobotEffectItem *jet;

    jet = &effect->jet;
    if (effect == 0) {
        assert_prog(D_00A523E0, D_00A523A0, 218);
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
        assert_prog(D_00A523E0, D_00A523A0, 230);
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

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot_effect", _GetJetShootLocal);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot_effect", _GetDashShootLocal);

RgRobotEffect *CreateRgRobotEffect(void)
{
    RgRobotEffect *effect;

    effect = RgHeapAlloc(InstanceOfRgHeap(), sizeof(RgRobotEffect),
                         D_00A523A0, 284);
    _InitRobEff(effect);
    return effect;
}

void DisposeRgRobotEffect(RgRobotEffect *effect)
{
    if (effect == 0) {
        assert_prog(D_00A523E0, D_00A523A0, 292);
    }
    _DestructRobEff(effect);
    RgHeapFree(InstanceOfRgHeap(), effect, D_00A523A0, 294);
}

void RgRobotEffectSetActor(RgRobotEffect *effect, void *actor)
{
    if (effect == 0) {
        assert_prog(D_00A523E0, D_00A523A0, 304);
    }
    effect->actor = actor;
}

void RgRobotEffectSetGeom(RgRobotEffect *effect, RgGeomPoint *geom)
{
    if (effect == 0) {
        assert_prog(D_00A523E0, D_00A523A0, 311);
    }
    effect->geom = geom;
}

void RgRobotEffectSetJetPtclName(RgRobotEffect *effect, const char *name)
{
    if (effect == 0) {
        assert_prog(D_00A523E0, D_00A523A0, 319);
    }
    strcpy(effect->jet.ptcl_name, name);
}

void RgRobotEffectSetDashPtclName(RgRobotEffect *effect, const char *name)
{
    if (effect == 0) {
        assert_prog(D_00A523E0, D_00A523A0, 327);
    }
    strcpy(effect->dash.ptcl_name, name);
}

void RgRobotEffectSetJetInertia(RgRobotEffect *effect, int inertia) {
    if (effect == 0) {
        assert_prog(D_00A523E0, D_00A523A0, 335);
    }
    _SetInertiaEffect(&effect->jet, inertia);
}

void RgRobotEffectTermAll(RgRobotEffect *effect)
{
    RgRobotEffectTermJet(effect);
}

/* Opaque: the other TU (rg_effect_env.c) that owns RgEffectEnv still keeps
 * RgEffectEnvGetParticleData as scaffold asm, so it has no published
 * prototype yet. */
typedef struct RgEffectEnv RgEffectEnv;
RgEffectEnv *InstanceOfRgEffectEnv(void);
int RgEffectEnvGetParticleData(RgEffectEnv *env, char *name, void *buffer);
int _GetJetShootLocal(RgRobotEffect *generator, int index, void *local, RgVector inertia);
int _InitJetDefault(void *buffer);

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
        assert_prog(D_00A523E0, D_00A523A0, 356);
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
                shotDefault += 0x2C;
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
        assert_prog(D_00A523E0, D_00A523A0, 387);
    }
    if (effect->actor != 0) {
        _StopEffect(&effect->jet);
    }
}

int _GetDashShootLocal(RgRobotEffect *generator, int index, void *local, RgVector inertia);
int _InitDashDefault(void *buffer);

void RgRobotEffectStartDash(RgRobotEffect *effect, float stopTime) {
    /* Raw particle-essence lookup buffer, opaque to this TU; see
     * RgRobotEffectStartJet. */
    int shotData[0xB0];
    RgRobotEffectItem *dash;
    int shotCount;

    dash = &effect->dash;
    if (effect == 0) {
        assert_prog(D_00A523E0, D_00A523A0, 402);
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
        assert_prog(D_00A523E0, D_00A523A0, 427);
    }
    if (effect->actor != 0) {
        _StopEffect(&effect->dash);
    }
}

void _PassTimeEffect(RgRobotEffectItem *item, float time);

void RgRobotEffectPassTime(RgRobotEffect *effect, float time) {
    if (effect == 0) {
        assert_prog(D_00A523E0, D_00A523A0, 441);
    }
    if (effect->actor != 0) {
        _PassTimeEffect(&effect->jet, time);
        _PassTimeEffect(&effect->dash, time);
    }
}

void _DispEffect(RgRobotEffectItem *item);

void RgRobotEffectDisp(RgRobotEffect *effect) {
    if (effect == 0) {
        assert_prog(D_00A523E0, D_00A523A0, 453);
    }
    if (effect->actor == 0) {
        return;
    }
    _DispEffect(&effect->jet);
    _DispEffect(&effect->dash);
}
